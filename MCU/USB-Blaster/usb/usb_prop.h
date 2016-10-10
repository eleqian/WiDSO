/**
  ******************************************************************************
  * @file    usb_prop.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   All processing related to USB Blaster
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PROP_H
#define __USB_PROP_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define FTDI_VEN_REQ_RESET              0x00
#define FTDI_VEN_REQ_SET_BAUDRATE       0x01
#define FTDI_VEN_REQ_SET_DATA_CHAR      0x02
#define FTDI_VEN_REQ_SET_FLOW_CTRL      0x03
#define FTDI_VEN_REQ_SET_MODEM_CTRL     0x04
#define FTDI_VEN_REQ_GET_MODEM_STA      0x05
#define FTDI_VEN_REQ_SET_EVENT_CHAR     0x06
#define FTDI_VEN_REQ_SET_ERR_CHAR       0x07
#define FTDI_VEN_REQ_SET_LAT_TIMER      0x09
#define FTDI_VEN_REQ_GET_LAT_TIMER      0x0A
#define FTDI_VEN_REQ_SET_BITMODE        0x0B
#define FTDI_VEN_REQ_RD_PINS            0x0C
#define FTDI_VEN_REQ_RD_EEPROM          0x90
#define FTDI_VEN_REQ_WR_EEPROM          0x91
#define FTDI_VEN_REQ_ES_EEPROM          0x92

#define FTDI_MODEM_STA_SIZE             0x02
#define FTDI_MODEM_STA_DUMMY0           0x01
#define FTDI_MODEM_STA_DUMMY1           0x60

/* Exported functions ------------------------------------------------------- */

void Blaster_Init(void);
void Blaster_Reset(void);
void Blaster_SetConfiguration(void);
void Blaster_SetDeviceAddress (void);
void Blaster_Status_In (void);
void Blaster_Status_Out (void);
RESULT Blaster_Data_Setup(uint8_t);
RESULT Blaster_NoData_Setup(uint8_t);
RESULT Blaster_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *Blaster_GetDeviceDescriptor(uint16_t );
uint8_t *Blaster_GetConfigDescriptor(uint16_t);
uint8_t *Blaster_GetStringDescriptor(uint16_t);

/* Exported define -----------------------------------------------------------*/

#define Blaster_GetConfiguration          NOP_Process
//#define Blaster_SetConfiguration          NOP_Process
#define Blaster_GetInterface              NOP_Process
#define Blaster_SetInterface              NOP_Process
#define Blaster_GetStatus                 NOP_Process
#define Blaster_ClearFeature              NOP_Process
#define Blaster_SetEndPointFeature        NOP_Process
#define Blaster_SetDeviceFeature          NOP_Process
//#define Blaster_SetDeviceAddress          NOP_Process

#endif /* __USB_PROP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
