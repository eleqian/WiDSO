#ifndef __HW_CONFIG_H__
#define __HW_CONFIG_H__

#include "base.h"

void USB_SetSystem(void);
void USB_SetClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Led_Config(void);
void USB_Led_RW_ON(void);
void USB_Led_RW_OFF(void);
void USB_Configured_LED(void);
void USB_NotConfigured_LED(void);
void USB_Disconnect_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void Get_SerialNum(void);

#endif  /*__HW_CONFIG_H__*/
