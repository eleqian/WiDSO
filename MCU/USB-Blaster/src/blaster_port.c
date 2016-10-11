/*
blaster_port.c - USB-Blaster IO port

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

#include "base.h"
#include "bitband.h"
#include "blaster_port.h"

/*-----------------------------------*/

// uncomment to enable AS mode (no pins defined)
//#define BLASTER_AS_MODE_EN

// uncomment to enable OE/LED
#define BLASTER_OE_LED_EN

// TCK: PB3
#define TCK_OUT(d)      PBO(3) = (d)
#define TCK_0()         TCK_OUT(0)
#define TCK_1()         TCK_OUT(1)

// TDO: PB4
#define TDO_IN()        PBI(4)

// TDI: PB5
#define TDI_OUT(d)      PBO(5) = (d)

// TMS: PB6
#define TMS_OUT(d)      PBO(6) = (d)

#ifdef BLASTER_OE_LED_EN
// OE/LED: PA8
#define OE_OUT(d)       PAO(8) = (d)
#endif

#ifdef BLASTER_AS_MODE_EN
// NCE: ??
#define NCE_OUT(d)      

// NCS: ??
#define NCS_OUT(d)      
#define NCS_GETOUT()        1

// DATAOUT: ??
#define DATAOUT_IN()        1
#endif

/*-----------------------------------*/

void bport_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // GPIO Out Configuration: TCK(PB3), TDI(PB5), TMS(PB6)
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // GPIO In Configuration: TDO(PB4)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

#ifdef BLASTER_OE_LED_EN
    // led(OE) pin already init in led.c
#endif

#ifdef BLASTER_AS_MODE_EN
    // init AS mode pins here (nCS/nCE/DATAOUT)
#endif
}

// bit-band mode output
void bport_state_set(uint8_t d)
{
    TDI_OUT(0 != (d & BLASTER_STA_OUT_TDI));
    TMS_OUT(0 != (d & BLASTER_STA_OUT_TMS));
    TCK_OUT(0 != (d & BLASTER_STA_OUT_TCK));
#ifdef BLASTER_AS_MODE_EN
    NCE_OUT(0 != (d & BLASTER_STA_OUT_NCE));
    NCS_OUT(0 != (d & BLASTER_STA_OUT_NCS));
#endif
#ifdef BLASTER_OE_LED_EN
    OE_OUT(0 != (d & BLASTER_STA_OUT_OE));
#endif
}

// bit-bang mode input
uint8_t bport_state_get(void)
{
    uint32_t d = 0;

    d |= TDO_IN() << BLASTER_STA_IN_TDO_BIT;
#ifdef BLASTER_AS_MODE_EN
    d |= DATAOUT_IN() << BLASTER_STA_IN_DATAOUT_BIT;
#endif

    return (uint8_t)d;
}

// shift mode output
void bport_shift_out(uint8_t d)
{
#define BPORT_SHIFT_BIT()    TDI_OUT(d); TCK_1(); d >>= 1; TCK_0()

    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();

#undef BPORT_SHIFT_BIT
}

// shift mode input
uint8_t bport_shift_io(uint8_t d)
{
    uint32_t dshift = d;
    uint32_t din;
    
#ifdef BLASTER_AS_MODE_EN
#define BPORT_SHIFT_BIT()    TDI_OUT(dshift); din = DATAOUT_IN(); TCK_1(); dshift = (dshift >> 1) | (din << 7); TCK_0()

    if (NCS_GETOUT()) {
        BPORT_SHIFT_BIT();
        BPORT_SHIFT_BIT();
        BPORT_SHIFT_BIT();
        BPORT_SHIFT_BIT();
        BPORT_SHIFT_BIT();
        BPORT_SHIFT_BIT();
        BPORT_SHIFT_BIT();
        BPORT_SHIFT_BIT();
        return dshift & 0xff;
    }

#undef BPORT_SHIFT_BIT
#endif //BLASTER_AS_MODE_EN

#define BPORT_SHIFT_BIT()    TDI_OUT(dshift); din = TDO_IN(); TCK_1(); dshift = (dshift >> 1) | (din << 7); TCK_0()
    
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();
    BPORT_SHIFT_BIT();

#undef BPORT_SHIFT_BIT

    return dshift & 0xff;
}
