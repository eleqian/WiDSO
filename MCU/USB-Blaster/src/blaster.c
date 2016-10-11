/*
blaster.c - USB-Blaster core

MIT License

Copyright (c) 2016 Yongqian Tang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "base.h"
#include "timebase.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "ringbuf.h"
#include "blaster_port.h"
#include "blaster.h"

/*-----------------------------------*/

#define SEND_PACKET_SIZE            (ENDP1_TXSIZE)
#define SEND_PACKET_VALID_SIZE      (SEND_PACKET_SIZE - FTDI_MODEM_STA_SIZE)

#define RECV_BUFF_SIZE              (ENDP2_RXSIZE)
#define SEND_BUFF_SIZE              (ENDP2_RXSIZE)

#define BLASTER_HEARTBEAT_TIME      (10)    // 10ms

/*-----------------------------------*/

// usb rx/tx ready flag
__IO BOOL g_blaster_rx_req = FALSE;
__IO BOOL g_blaster_tx_ready = TRUE;

// usb receive buffer, to output
static uint8_t s_recv_buffer[RECV_BUFF_SIZE];
static uint32_t s_recv_buff_len;
static uint32_t s_recv_buff_idx;

// usb send buffer, input from outside
static uint8_t s_send_buffer[SEND_BUFF_SIZE];
static uint8_t s_send_packet_buff[SEND_PACKET_SIZE];
static ringbuf_t s_send_ring_buff;

// operate mode & count
static uint32_t s_blaster_shift_cnt;
static BOOL s_blaster_shift_en;
static BOOL s_blaster_read_en;

// send dummy modem status periodicity (FTDI driver required)
static uint32_t s_blaster_sendtime;
static BOOL s_blaster_senddummy;

/*-----------------------------------*/

static void blaster_usbrecv(void)
{
    uint16_t len;
    
    if (!g_blaster_rx_req) {
        return;
    }

    if (s_recv_buff_idx != s_recv_buff_len) {
        return;
    }
    
    if (GetENDPOINT(ENDP2) & EP_DTOG_TX) {
        /* read from buffer 0 */
        len = GetEPDblBuf0Count(ENDP2);
        PMAToUserBufferCopy(s_recv_buffer, ENDP2_RXADDR0, len);
    } else {
        /* read from buffer 1 */
        len = GetEPDblBuf1Count(ENDP2);
        PMAToUserBufferCopy(s_recv_buffer, ENDP2_RXADDR1, len);
    }
    s_recv_buff_len = len;
    s_recv_buff_idx = 0;
    g_blaster_rx_req = FALSE;
    FreeUserBuffer(ENDP2, EP_DBUF_OUT);
}

static void blaster_usbsend(void)
{
    uint32_t send_len;

    if (!g_blaster_tx_ready) {
        return;
    }

    send_len = ringbuf_use_len(&s_send_ring_buff);
    if (send_len > SEND_PACKET_VALID_SIZE) {
        // send full packet
        send_len = SEND_PACKET_VALID_SIZE;
    } else if (0 == send_len) {
        if (millis() - s_blaster_sendtime >= BLASTER_HEARTBEAT_TIME) {
            // send heart beat packet
        } else if (s_blaster_senddummy) {
            // send dummy packet
            s_blaster_senddummy = FALSE;
        } else {
            // nothing to send
            return;
        }
    } else {
        // send not full packet
    }

    s_blaster_sendtime = millis();

    ringbuf_get(&s_send_ring_buff, &s_send_packet_buff[FTDI_MODEM_STA_SIZE], send_len);
    
    /* Reset the control token to inform upper layer that a transfer is ongoing */
    g_blaster_tx_ready = FALSE;
    
    /* Copy data to ENDP1 Tx Packet Memory Area*/
    USB_SIL_Write(EP1_IN, s_send_packet_buff, FTDI_MODEM_STA_SIZE + send_len);
    /* Enable endpoint for transmission */
    SetEPTxValid(ENDP1);
}

/*-----------------------------------*/

static void blaster_ioproc(void)
{
    uint8_t d;

    while ((s_recv_buff_idx < s_recv_buff_len) && !ringbuf_is_full(&s_send_ring_buff)) {
        d = s_recv_buffer[s_recv_buff_idx];
        if (0 == s_blaster_shift_cnt) {
            // bit-bang mode (default)
            s_blaster_shift_en = (0 != (d & BLASTER_STA_SHIFT));
            s_blaster_read_en = (0 != (d & BLASTER_STA_READ));
            if (s_blaster_shift_en) {
                s_blaster_shift_cnt = (d & BLASTER_STA_CNT_MASK);
            } else if (s_blaster_read_en)  {
                bport_state_set(d);
                d = bport_state_get();
                ringbuf_putc(&s_send_ring_buff, d);
            } else {
                bport_state_set(d);
            }
        } else {
            // shift mode
            if (s_blaster_read_en) {
                d = bport_shift_io(d);
                ringbuf_putc(&s_send_ring_buff, d);
            } else {
                bport_shift_out(d);
            }
            s_blaster_shift_cnt--;
        }
        
        s_recv_buff_idx++;
    }
}

/*-----------------------------------*/

void blaster_init(void)
{
    g_blaster_rx_req = FALSE;
    g_blaster_tx_ready = TRUE;

    s_blaster_shift_cnt = 0;
    s_blaster_shift_en = FALSE;
    s_blaster_read_en = FALSE;

    s_blaster_sendtime = 0;
    s_blaster_senddummy = TRUE;
    
    s_recv_buff_len = 0;
    s_recv_buff_idx = 0;

    // first two bytes are modem status
    s_send_packet_buff[0] = FTDI_MODEM_STA_DUMMY0;
    s_send_packet_buff[1] = FTDI_MODEM_STA_DUMMY1;

    ringbuf_init(&s_send_ring_buff, s_send_buffer, sizeof(s_send_buffer));

    bport_init();
}

void blaster_exec(void)
{
    blaster_usbrecv();
    blaster_ioproc();
    blaster_usbsend();
}
