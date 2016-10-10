/*
 * LED驱动模块
 * LED引脚 PA8
 * eleqian 2016-10-7
 */

#include "base.h"
#include "timebase.h"
#include "led.h"

// LED模式枚举
enum {
    LED_MODE_NORMAL,    // 普通模式（亮/灭）
    LED_MODE_FLASH,     // 闪烁模式

    LED_MODE_MAX
} LED_Mode;

// LED模式
static uint8_t gLEDMode;

// 上次更新时间(ms)
static uint32_t gLEDLastTime;

// LED计时(ms)
static uint16_t gLEDTimer;

// LED闪烁周期(ms)
static uint16_t gLEDPeriod;

// LED每闪烁周期点亮时间(ms)
static uint16_t gLEDPulse;

// LED闪烁次数
static uint16_t gLEDCycleCnt;

/*-----------------------------------*/

// 开/关LED
#define LED_ON()        GPIO_SetBits(GPIOA, GPIO_Pin_8)
#define LED_OFF()       GPIO_ResetBits(GPIOA, GPIO_Pin_8)

// LED端口初始化
static void LED_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // GPIOA Configuration: Pin 8
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*-----------------------------------*/

// LED初始化
void LED_Init(void)
{
    LED_GPIOConfig();
    LED_OFF();

    gLEDLastTime = millis();
    gLEDMode = LED_MODE_NORMAL;
}

// LED开关
void LED_Light(BOOL isLight)
{
    gLEDMode = LED_MODE_NORMAL;
    if (isLight) {
        LED_ON();
    } else {
        LED_OFF();
    }
}

// LED闪烁
// 参数：周期(ms)，点亮时间(ms)，闪烁次数(=0时一直闪烁)
void LED_Flash(uint16_t period, uint16_t pulse, uint16_t cycles)
{
    gLEDMode = LED_MODE_FLASH;
    gLEDPeriod = period;
    gLEDPulse = pulse;
    gLEDCycleCnt = cycles;
    gLEDTimer = 0;
    gLEDLastTime = millis();
    LED_OFF();
}

/*-----------------------------------*/

// LED定时任务
void LED_Exec(void)
{
    uint32_t t;

    switch (gLEDMode) {
    case LED_MODE_FLASH:
        t = millis();
        gLEDTimer += (t - gLEDLastTime);
        gLEDLastTime = t;
        if (gLEDTimer >= gLEDPulse) {
            LED_OFF();
        }
        if (gLEDTimer >= gLEDPeriod) {
            if (gLEDCycleCnt > 0) {
                gLEDCycleCnt--;
                if (gLEDCycleCnt == 0) {
                    gLEDMode = LED_MODE_NORMAL;
                    LED_OFF();
                    return;
                }
            }
            gLEDTimer = 0;
            LED_ON();
        }
        break;
    default:
        break;
    }
}
