/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/

#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define BLASTER_SIZ_DEVICE_DESC                 18
#define BLASTER_SIZ_CONFIG_DESC                 32
#define BLASTER_SIZ_STRING_LANGID               4
#define BLASTER_SIZ_STRING_VENDOR               14
#define BLASTER_SIZ_STRING_PRODUCT              24
#define BLASTER_SIZ_STRING_SERIAL               18

/* Exported functions ------------------------------------------------------- */

extern const uint8_t Blaster_DeviceDescriptor[BLASTER_SIZ_DEVICE_DESC];
extern const uint8_t Blaster_ConfigDescriptor[BLASTER_SIZ_CONFIG_DESC];
extern const uint8_t Blaster_StringLangID[BLASTER_SIZ_STRING_LANGID];
extern const uint8_t Blaster_StringVendor[BLASTER_SIZ_STRING_VENDOR];
extern const uint8_t Blaster_StringProduct[BLASTER_SIZ_STRING_PRODUCT];
extern uint8_t Blaster_StringSerial[BLASTER_SIZ_STRING_SERIAL];

#endif /* __USB_DESC_H */

