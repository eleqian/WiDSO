#ifndef __TIMEBASE_H__
#define __TIMEBASE_H__

#include "base.h"

void TimeBase_Init(void);

uint32_t micros(void);

uint32_t millis(void);

void delay_us(uint32_t us);

void delay_ms(uint32_t ms);

#endif //__TIMEBASE_H__
