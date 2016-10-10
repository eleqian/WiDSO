/*
 * USBÓ²¼þÅäÖÃÄ£¿é for stm32f103c8 & stm32l151c8
 * eleqian 2014-9-12
 */

#include "hw_config.h"
#include "mass_mal.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_lib.h"

// Unique Devices IDs register set
#ifdef MCU_STM32L151C8
#define ID1_ADDR        (0x1FF80050)
#define ID2_ADDR        (0x1FF80054)
#define ID3_ADDR        (0x1FF80064)
#endif //MCU_STM32L151C8

#ifdef MCU_STM32F103C8
#define ID1_ADDR        (0x1FFFF7E8)
#define ID2_ADDR        (0x1FFFF7EC)
#define ID3_ADDR        (0x1FFFF7F0)
#endif //MCU_STM32F103C8

/*******************************************************************************
* Function Name  : USB_Disconnect_Config
* Description    : Disconnect pin configuration
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Disconnect_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

#ifdef MCU_STM32L151C8
    // Enable the USB disconnect GPIO clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    // PA15 used as USB pull-up
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
#endif //MCU_STM32L151C8

#ifdef MCU_STM32F103C8
    // Enable USB disconnect GPIO clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA15 used as USB pull-up
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif //MCU_STM32F103C8

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_15);
}

/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable.
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config(FunctionalState NewState)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // PA15 used as USB pull-up
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

#ifdef MCU_STM32L151C8
    if (NewState != DISABLE) {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    } else {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    }
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
#endif //MCU_STM32L151C8

#ifdef MCU_STM32F103C8
    if (NewState != DISABLE) {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    } else {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
#endif //MCU_STM32F103C8

    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
    // Set the device state to suspend
    bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
    DEVICE_INFO *pInfo = &Device_Info;

    // Set the device state to the correct state
    if (pInfo->Current_Configuration != 0) {
        // Device configured
        bDeviceState = CONFIGURED;
    } else {
        bDeviceState = ATTACHED;
    }
    //Enable SystemCoreClock
    SystemInit();
}

/*******************************************************************************
* Function Name  : USB_SetSystem
* Description    : Configures Main system clocks & power
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_SetSystem(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    // Configure the EXTI line 18 connected internally to the USB IP
    EXTI_ClearITPendingBit(EXTI_Line18);
    EXTI_InitStructure.EXTI_Line = EXTI_Line18;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // Enable and Disconnect Line GPIO clock
    USB_Disconnect_Config();

    // MAL configuration
    MAL_Init(0);
#ifdef MAL_SPI_FLASH
    MAL_Init(1);
#endif //MAL_SPI_FLASH
}

/*******************************************************************************
* Function Name  : USB_SetClock
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_SetClock(void)
{
#ifdef MCU_STM32L151C8
    // Enable USB clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
#endif //MCU_STM32L151C8

#ifdef MCU_STM32F103C8
    // Select USBCLK source
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
    // Enable the USB clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
#endif //MCU_STM32L151C8
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : Configures the USB interrupts
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // 2 bit for pre-emption priority, 2 bits for subpriority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

#ifdef MCU_STM32L151C8
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable the USB Wake-up interrupt
    NVIC_InitStructure.NVIC_IRQChannel = USB_FS_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif //MCU_STM32L151C8

#ifdef MCU_STM32F103C8
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable the USB Wake-up interrupt
    NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
#endif //MCU_STM32F103C8
}

/*******************************************************************************
* Function Name  : USB_Led_Config
* Description    : configure the Read/Write LEDs.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Led_Config(void)
{
    /*GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // PB0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);*/
}

void USB_Led_RW_ON(void)
{
    //GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

void USB_Led_RW_OFF(void)
{
    //GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void USB_Configured_LED(void)
{
    //GPIO_SetBits(GPIOB, GPIO_Pin_0);
}

void USB_NotConfigured_LED(void)
{
    //GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntToUnicode(uint32_t value , uint8_t *pbuf , uint8_t len)
{
    uint8_t idx = 0;

    for (idx = 0 ; idx < len ; idx ++) {
        if (((value >> 28)) < 0xA) {
            pbuf[2 * idx] = (value >> 28) + '0';
        } else {
            pbuf[2 * idx] = (value >> 28) + 'A' - 10;
        }
        value = value << 4;
        pbuf[ 2 * idx + 1] = 0;
    }
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
    uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

    Device_Serial0 = *(uint32_t *)ID1_ADDR;
    Device_Serial1 = *(uint32_t *)ID2_ADDR;
    Device_Serial2 = *(uint32_t *)ID3_ADDR;

    Device_Serial0 += Device_Serial2;

    if (Device_Serial0 != 0) {
        IntToUnicode(Device_Serial0, &MASS_StringSerial[2] , 8);
        IntToUnicode(Device_Serial1, &MASS_StringSerial[18], 4);
    }
}
