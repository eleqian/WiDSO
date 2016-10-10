/**
  ******************************************************************************
  * @file    memory.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Memory management layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/

#include "usb_conf.h"
#include "usb_lib.h"
#include "usb_scsi.h"
#include "usb_bot.h"
#include "usb_regs.h"
#include "usb_mem.h"
#include "mass_mal.h"
#include "memory.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t Block_Read_count = 0;
static __IO uint32_t Block_offset = 0;
static __IO uint32_t Block_Write_count = 0;
static uint8_t Data_Buffer[MAL_BLOCK_SIZE_MAX];    // 数据缓冲区，必须>=存储设备块大小
static uint8_t TransferState = TXFR_IDLE;

/* Extern variables ----------------------------------------------------------*/
extern uint8_t Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];  /* data buffer*/
extern uint16_t Bulk_Data_Len;
extern uint8_t Bot_State;
//extern Bulk_Only_CBW CBW;
extern Bulk_Only_CSW CSW;

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Read_Memory
* Description    : Handle the Read operation from the microSD card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Read_Memory(uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length)
{
    static uint32_t Offset, Length;

    if (TransferState == TXFR_IDLE) {
        Offset = Memory_Offset * Mass_Block_Size[lun];
        Length = Transfer_Length * Mass_Block_Size[lun];
        TransferState = TXFR_ONGOING;
    }

    if (TransferState == TXFR_ONGOING) {
        // 缓冲区空时从存储设备读取数据
        if (0 == Block_Read_count) {
            MAL_Read(lun, Offset, Data_Buffer, Mass_Block_Size[lun]);
            Block_Read_count = Mass_Block_Size[lun];
            Block_offset = 0;
        }

        USB_SIL_Write(EP1_IN, (uint8_t *)Data_Buffer + Block_offset, BULK_MAX_PACKET_SIZE);
        Block_Read_count -= BULK_MAX_PACKET_SIZE;
        Block_offset += BULK_MAX_PACKET_SIZE;

        SetEPTxCount(ENDP1, BULK_MAX_PACKET_SIZE);
        SetEPTxStatus(ENDP1, EP_TX_VALID);
        Offset += BULK_MAX_PACKET_SIZE;
        Length -= BULK_MAX_PACKET_SIZE;

        CSW.dDataResidue -= BULK_MAX_PACKET_SIZE;
        USB_Led_RW_ON();
    }

    if (Length == 0) {
        Block_Read_count = 0;
        Block_offset = 0;
        Offset = 0;
        Bot_State = BOT_DATA_IN_LAST;
        TransferState = TXFR_IDLE;
        USB_Led_RW_OFF();
    }
}

/*******************************************************************************
* Function Name  : Write_Memory
* Description    : Handle the Write operation to the microSD card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Write_Memory(uint8_t lun, uint32_t Memory_Offset, uint32_t Transfer_Length)
{
    static uint32_t W_Offset, W_Length;
    uint32_t temp =  Block_Write_count + BULK_MAX_PACKET_SIZE;
    uint32_t idx;

    if (TransferState == TXFR_IDLE) {
        W_Offset = Memory_Offset * Mass_Block_Size[lun];
        W_Length = Transfer_Length * Mass_Block_Size[lun];
        TransferState = TXFR_ONGOING;
    }

    if (TransferState == TXFR_ONGOING) {
        for (idx = 0 ; Block_Write_count < temp; Block_Write_count++) {
            *((uint8_t *)Data_Buffer + Block_Write_count) = Bulk_Data_Buff[idx++];
        }
        W_Offset += Bulk_Data_Len;
        W_Length -= Bulk_Data_Len;

        // 缓冲区满时写入数据到存储设备
        if (0 == (W_Length % Mass_Block_Size[lun])) {
            Block_Write_count = 0;
            MAL_Write(lun, W_Offset - Mass_Block_Size[lun], Data_Buffer, Mass_Block_Size[lun]);
        }

        CSW.dDataResidue -= Bulk_Data_Len;
        SetEPRxStatus(ENDP2, EP_RX_VALID); /* enable the next transaction*/
        USB_Led_RW_ON();
    }

    if ((W_Length == 0) || (Bot_State == BOT_CSW_Send)) {
        Block_Write_count = 0;
        Set_CSW(CSW_CMD_PASSED, SEND_CSW_ENABLE);
        TransferState = TXFR_IDLE;
        USB_Led_RW_OFF();
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
