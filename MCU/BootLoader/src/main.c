/*
 * MiCore核心板USB BootLoader for stm32f103c8
 * eleqian 2014-9-12
 */

#include "base.h"
#include "timebase.h"
#include "led.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "mass_mal.h"

uint8_t gDiskWriteProtect[MAX_LUN] = {0};

/*-----------------------------------*/

// 应用程序代码起始地址
#define FLASH_APP_ADDR      INTER_FLASH_BASE_ADDR
// FLASH页大小（STM32F103C8T6的为1K）
#define FLASH_PAGE_SIZE     INTER_FLASH_PAGE_SIZE

typedef void (*pAppMain_t)(void);

// 判断是否进入IAP模式
BOOL IAP_Check(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    BOOL isInto = FALSE;

    // 打开按键端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_DeInit(GPIOC);
    
    // 初始化KEY端口(PC13)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    delay_ms(10);

    // KEY按下时为低电平
    isInto = !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);

    // 复位按键端口状态
    GPIO_DeInit(GPIOC);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, DISABLE);

    return isInto;
}

// 载入应用代码
// 如果成功载入则本函数不会退出，失败时会返回
void IAP_LoadApp(void)
{
    pAppMain_t pAppMain;
    uint32_t JumpAddress;

    // 检测栈顶地址是否合法
    if (((*(__IO uint32_t *)FLASH_APP_ADDR) & 0x2FFE0000) == 0x20000000) {
        // 跳转到用户应用代码区
        JumpAddress = *(__IO uint32_t *)(FLASH_APP_ADDR + 4);
        pAppMain = (pAppMain_t)JumpAddress;
        // 初始化用户应用程序的堆栈指针
        __set_MSP(*(__IO uint32_t *)FLASH_APP_ADDR);
        pAppMain();
    }
}

// 执行IAP
void IAP_Update(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // 关JTAG，仅使用SWD
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    LED_Init();

    // 闪烁LED提示并等待插入USB进入IAP模式
    USB_SetSystem();
    USB_SetClock();
    //USB_Led_Config();
    USB_Interrupts_Config();
    USB_Init();
    while (bDeviceState != CONFIGURED) {
        LED_Light(TRUE);
        delay_ms(50);
        LED_Light(FALSE);
        delay_ms(50);
    }
    //USB_Configured_LED();
    LED_Light(TRUE);
}

/*-----------------------------------*/

// 主函数
int main(void)
{
    TimeBase_Init();
    
    if (!IAP_Check()) {
        // 载入应用程序
        IAP_LoadApp();
    }
    
    // 进入IAP模式
    IAP_Update();

    while (1) {
    }
}

/*-----------------------------------*/

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* Infinite loop */
    while (1) {
    }
}
#endif //USE_FULL_ASSERT
