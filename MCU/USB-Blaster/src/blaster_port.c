#include "base.h"
#include "bitband.h"
#include "blaster_port.h"

/*-----------------------------------*/

// TCK: PB3
#define TCK_OUT(d)      PBO(3) = (d)
#define TCK_0()         TCK_OUT(0)  //GPIOB->BRR = GPIO_Pin_3
#define TCK_1()         TCK_OUT(1)  //GPIOB->BSRR = GPIO_Pin_3

// TDO: PB4
#define TDO_IN()        PBI(4)      //GPIOB->IDR & GPIO_Pin_4

// TDI: PB5
#define TDI_OUT(d)      PBO(5) = (d)
#define TDI_0()         TDI_OUT(0)  //GPIOB->BRR = GPIO_Pin_5
#define TDI_1()         TDI_OUT(1)  //GPIOB->BSRR = GPIO_Pin_5

// TMS: PB6
#define TMS_OUT(d)      PBO(6) = (d)
#define TMS_0()         TMS_OUT(0)  //GPIOB->BRR = GPIO_Pin_6
#define TMS_1()         TMS_OUT(1)  //GPIOB->BSRR = GPIO_Pin_6

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
}

void bport_state_set(uint8_t d)
{
    TDI_OUT(0 != (d & BLASTER_STA_OUT_TDI));
    TMS_OUT(0 != (d & BLASTER_STA_OUT_TMS));
    TCK_OUT(0 != (d & BLASTER_STA_OUT_TCK));
}

uint8_t bport_state_get(void)
{
    uint8_t d = 0;

    d |= TDO_IN() << BLASTER_STA_IN_TDO_BIT;

    return d;
}

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

uint8_t bport_shift_io(uint8_t d)
{
    uint32_t dshift = d;
    uint32_t din;
    
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
