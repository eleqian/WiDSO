/*
main.c

MIT License

Copyright (c) 2016 Yongqian Tang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include "base.h"
#include "timebase.h"
#include "led.h"
#include "bitband.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "vcp.h"

/*-----------------------------------*/

// 判断按键是否按下
BOOL key_is_pressed(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    BOOL is_press = FALSE;

    // 打开按键端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_DeInit(GPIOC);
    
    // 初始化KEY端口(PC13)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    delay_ms(10);

    // KEY按下时为低电平
    is_press = !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);

    // 复位按键端口状态
    GPIO_DeInit(GPIOC);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, DISABLE);

    return is_press;
}

/*-----------------------------------*/

int main(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // disable JTAG，use SWD only
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    timebase_init();
    led_init();
    vcp_init();
    
    // for debug use
    if (!key_is_pressed()) {
        USB_HW_Config();
        USB_Init();
    }

    led_flash(1000, 100, 0);

    while (1) {
        led_update();
        if (bDeviceState == CONFIGURED)
        {
            if (key_is_pressed()) {
                GPIO_ResetBits(GPIOB, GPIO_Pin_2);
                GPIO_ResetBits(GPIOB, GPIO_Pin_1);
                while (key_is_pressed());
                GPIO_SetBits(GPIOB, GPIO_Pin_2);
            }
        }
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
