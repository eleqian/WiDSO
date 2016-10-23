#ifndef __VCP_H__
#define __VCP_H__

#include "base.h"

#define USART_RX_DATA_SIZE      2048

#define VCP_LINE_STATE_DTR      0x1
#define VCP_LINE_STATE_RTS      0x2

typedef struct {
    uint32_t bitrate;     /* bps */
    uint8_t format;       /* 0:1 Stop bit/1:1.5 Stop bits/2:2 Stop bits */
    uint8_t paritytype;   /* 0:None/1:Odd/2:Even/3:Mark/4:Space */
    uint8_t datatype;     /* 5, 6, 7, 8 or 16 */
} LINE_CODING;

void vcp_init(void);
void vcp_ctrl_line_set(u32 state);
u32 vcp_com_reconfig(void);
void USB_To_USART_Send_Data(uint8_t* data_buffer, uint8_t Nb_bytes);
void USART_To_USB_Send_Data(void);
void Handle_USBAsynchXfer (void);

#endif //__VCP_H__
