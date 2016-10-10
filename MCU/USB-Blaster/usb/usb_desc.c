/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
const uint8_t Blaster_DeviceDescriptor[BLASTER_SIZ_DEVICE_DESC] = {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x02,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    0x40,                       /*bMaxPacketSize(64bytes)*/
    0xFB,                       /*idVendor (0x09FB=Altera)*/
    0x09,
    0x01,                       /*idProduct(0x6001=USB-Blaster)*/
    0x60,
    0x00,                       /*bcdDevice rel. B*/
    0x04,
    1,                          /*Index of string descriptor describing manufacturer */
    2,                          /*Index of string descriptor describing product*/
    3,                          /*Index of string descriptor describing the device serial number */
    0x01                        /*bNumConfigurations*/
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t Blaster_ConfigDescriptor[BLASTER_SIZ_CONFIG_DESC] = {
    /* Configuration Descriptor */
    /* 00 */
    0x09,         /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    BLASTER_SIZ_CONFIG_DESC,  /* wTotalLength: Bytes returned */
    0x00,
    0x01,         /* bNumInterfaces: 1 interface */
    0x01,         /* bConfigurationValue: Configuration value */
    0x00,         /* iConfiguration: Index of string descriptor describing the configuration*/
    0x80,         /* bmAttributes: Bus powered(bit6=0) */
    0xE1,         /* MaxPower 450mA(225*2) */

    /* Interface Descriptor */
    /* 09 */
    0x09,         /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type */
    0x00,         /* bInterfaceNumber: Number of Interface */
    0x00,         /* bAlternateSetting: Alternate setting */
    EP_NUM - 1,   /* bNumEndpoints */
    0xFF,         /* bInterfaceClass: NA */
    0xFF,         /* bInterfaceSubClass : NA */
    0xFF,         /* nInterfaceProtocol : NA */
    0,            /* iInterface: Index of string descriptor */
    
    /* Endpoint Descriptor */
    /* 18 */
    0x07,          /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint descriptor */
    EP1_IN,        /* bEndpointAddress: Endpoint 1 IN */
    0x02,          /* bmAttributes: Bulk endpoint */
    ENDP1_TXSIZE,  /* wMaxPacketSize: 64 Bytes max */
    0x00,
    0x01,          /* bInterval: Polling Interval (1 ms) */
    /* 25 */
    0x07,          /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint descriptor */
    EP2_OUT,       /* bEndpointAddress: Endpoint 2 OUT */
    0x02,          /* bmAttributes: Bulk endpoint */
    ENDP2_RXSIZE,  /* wMaxPacketSize: 64 Bytes max  */
    0x00,
    0x01,          /* bInterval: Polling Interval (1 ms) */
    /* 32 */
};

/* USB String Descriptors (optional) */

const uint8_t Blaster_StringLangID[BLASTER_SIZ_STRING_LANGID] = {
    BLASTER_SIZ_STRING_LANGID, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    0x09, /* LangID = 0x0409: U.S. English */
    0x04
};

const uint8_t Blaster_StringVendor[BLASTER_SIZ_STRING_VENDOR] = {
    BLASTER_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    'A', 0, 'l', 0, 't', 0, 'e', 0, 'r', 0, 'a', 0 /* Manufacturer: "Altera" */
};

const uint8_t Blaster_StringProduct[BLASTER_SIZ_STRING_PRODUCT] = {
    BLASTER_SIZ_STRING_PRODUCT,   /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,     /* bDescriptorType */
    'U', 0, 'S', 0, 'B', 0, '-', 0, 'B', 0, 'l', 0, 'a', 0, 's', 0, 't', 0, 'e', 0, 'r', 0  /* "USB-Blaster" */
};

uint8_t Blaster_StringSerial[BLASTER_SIZ_STRING_SERIAL] = {
    BLASTER_SIZ_STRING_SERIAL,    /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,     /* bDescriptorType */
    '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0, '0', 0  /* "00000000" */
};

