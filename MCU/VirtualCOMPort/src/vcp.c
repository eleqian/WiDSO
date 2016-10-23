#include "base.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "led.h"
#include "vcp.h"

uint8_t  USART_Rx_Buffer [USART_RX_DATA_SIZE];
uint32_t USART_Rx_ptr_in = 0;
uint32_t USART_Rx_ptr_out = 0;
uint32_t USART_Rx_length  = 0;
uint8_t  USB_Tx_State = 0;

LINE_CODING g_vcp_linecoding = {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* no. of bits 8*/
};

void vcp_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /* PB1: ESP_BOOT, PB2: ESP_EN */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_1);
    GPIO_SetBits(GPIOB, GPIO_Pin_2);
    
    /* PB10: UART3 Tx */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* PB11: UART3 Rx */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

    /* configure the USART to the default settings */
    /* USART3 configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - Parity No
          - Hardware flow control disabled
          - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure and enable the USART */
    USART_Init(USART3, &USART_InitStructure);

    /* Enable the USART Receive interrupt */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    
    /* Enable USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(USART3, ENABLE);
}

void vcp_ctrl_line_set(u32 state)
{
    if (state & VCP_LINE_STATE_RTS) {
        GPIO_SetBits(GPIOB, GPIO_Pin_1);
    } else {
        GPIO_ResetBits(GPIOB, GPIO_Pin_1);
    }
    
    if (state & VCP_LINE_STATE_DTR) {
        led_light(FALSE);
        GPIO_ResetBits(GPIOB, GPIO_Pin_2);
    } else {
        led_light(TRUE);
        GPIO_SetBits(GPIOB, GPIO_Pin_2);
    }
}

/*******************************************************************************
* Function Name  :  vcp_com_reconfig.
* Description    :  Configure the EVAL_COM1 according to the line coding structure.
* Input          :  None.
* Return         :  Configuration status
                    TRUE : configuration done with success
                    FALSE : configuration aborted.
*******************************************************************************/
u32 vcp_com_reconfig(void)
{
    USART_InitTypeDef USART_InitStructure;

    /* set the Stop bit*/
    switch (g_vcp_linecoding.format) {
    case 0:
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        break;
    case 1:
        USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
        break;
    case 2:
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
        break;
    default :
        return FALSE;
    }

    /* set the parity bit*/
    switch (g_vcp_linecoding.paritytype) {
    case 0:
        USART_InitStructure.USART_Parity = USART_Parity_No;
        break;
    case 1:
        USART_InitStructure.USART_Parity = USART_Parity_Even;
        break;
    case 2:
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
        break;
    default :
        return FALSE;
    }

    /*set the data type : only 8bits and 9bits is supported */
    switch (g_vcp_linecoding.datatype) {
    case 0x07:
        /* With this configuration a parity (Even or Odd) should be set */
        if (USART_Parity_No == USART_InitStructure.USART_Parity) {
            return FALSE;
        } else {
            USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        }
        break;
    case 0x08:
        if (USART_Parity_No == USART_InitStructure.USART_Parity) {
            USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        } else {
            USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        }
        break;
    default :
        return FALSE;
    }

    USART_InitStructure.USART_BaudRate = g_vcp_linecoding.bitrate;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    return TRUE;
}

void vcp_exec(void)
{

}

/*******************************************************************************
* Function Name  : USB_To_USART_Send_Data.
* Description    : send the received data from USB to the UART 0.
* Input          : data_buffer: data address.
                   Nb_bytes: number of bytes to send.
* Return         : none.
*******************************************************************************/
void USB_To_USART_Send_Data(uint8_t *data_buffer, uint8_t Nb_bytes)
{
    uint32_t i;

    for (i = 0; i < Nb_bytes; i++) {
        USART_SendData(USART3, *(data_buffer + i));
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    }
}

/*******************************************************************************
* Function Name  : UART_To_USB_Send_Data.
* Description    : send the received data from UART 0 to USB.
* Input          : None.
* Return         : none.
*******************************************************************************/
void USART_To_USB_Send_Data(void)
{
    if (g_vcp_linecoding.datatype == 7) {
        USART_Rx_Buffer[USART_Rx_ptr_in] = USART_ReceiveData(USART3) & 0x7F;
    } else if (g_vcp_linecoding.datatype == 8) {
        USART_Rx_Buffer[USART_Rx_ptr_in] = USART_ReceiveData(USART3);
    }

    USART_Rx_ptr_in++;

    /* To avoid buffer overflow */
    if (USART_Rx_ptr_in == USART_RX_DATA_SIZE) {
        USART_Rx_ptr_in = 0;
    }
}

/*******************************************************************************
* Function Name  : Handle_USBAsynchXfer.
* Description    : send data to USB.
* Input          : None.
* Return         : none.
*******************************************************************************/
void Handle_USBAsynchXfer(void)
{
    uint16_t USB_Tx_ptr;
    uint16_t USB_Tx_length;

    if (USB_Tx_State != 1) {
        if (USART_Rx_ptr_out == USART_RX_DATA_SIZE) {
            USART_Rx_ptr_out = 0;
        }

        if (USART_Rx_ptr_out == USART_Rx_ptr_in) {
            USB_Tx_State = 0;
            return;
        }

        if (USART_Rx_ptr_out > USART_Rx_ptr_in) { /* rollback */
            USART_Rx_length = USART_RX_DATA_SIZE - USART_Rx_ptr_out;
        } else {
            USART_Rx_length = USART_Rx_ptr_in - USART_Rx_ptr_out;
        }

        if (USART_Rx_length > VIRTUAL_COM_PORT_DATA_SIZE) {
            USB_Tx_length = VIRTUAL_COM_PORT_DATA_SIZE;
        } else {
            USB_Tx_length = USART_Rx_length;
        }
        USB_Tx_ptr = USART_Rx_ptr_out;
        USART_Rx_ptr_out += USB_Tx_length;
        USART_Rx_length -= USB_Tx_length;
        USB_Tx_State = 1;
        
        UserToPMABufferCopy(&USART_Rx_Buffer[USB_Tx_ptr], ENDP2_TXADDR, USB_Tx_length);
        SetEPTxCount(ENDP2, USB_Tx_length);
        SetEPTxValid(ENDP2);
    }
}

