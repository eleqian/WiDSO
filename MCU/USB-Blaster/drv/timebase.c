/* 时基模块
 * 获取系统启动时间及软件延时
 * eleqian 2014-3-10
 */

#include "base.h"
#include "timebase.h"

// cycles per microsecond
static uint32_t usTicks = 0;

// cycles per millisecond
static uint32_t msTicks = 0;

// current uptime for 1kHz systick timer. will rollover after 49 days. hopefully we won't care.
static volatile uint32_t sysTickUptime = 0;

/*-----------------------------------*/

// SysTick中断
void SysTick_Handler(void)
{
    sysTickUptime++;
}

/*-----------------------------------*/

// 初始化
void TimeBase_Init(void)
{
    // 1us时钟数
    usTicks = SystemCoreClock / 1000000;
    // 1ms时钟数
    msTicks = SystemCoreClock / 1000;
    // 1ms时基中断
    SysTick_Config(msTicks);
    // 提高中断优先级，避免在其它中断中调用时计时出错
    NVIC_SetPriority(SysTick_IRQn, 0);
}

/*-----------------------------------*/

// return SysTick count
uint32_t ticks(void)
{
    return SysTick->VAL;
}

// 返回系统启动us数 (70分钟溢出)
uint32_t micros(void)
{
    register uint32_t ms, cycle_cnt;

    do {
        ms = sysTickUptime;
        cycle_cnt = SysTick->VAL;
    } while (ms != sysTickUptime);

    return (ms * 1000) + (msTicks - cycle_cnt) / usTicks;
}

// 返回系统启动毫秒数 (49天溢出)
uint32_t millis(void)
{
    return sysTickUptime;
}

/*-----------------------------------*/

// us级延时
void delay_us(uint32_t us)
{
    uint32_t now = micros();

    while (micros() - now < us);
}

// ms级延时
void delay_ms(uint32_t ms)
{
    while (ms--) {
        delay_us(1000);
    }
}
