#ifndef __LED_H__
#define __LED_H__

#include "base.h"

void LED_Init(void);

void LED_Exec(void);

void LED_Light(BOOL isLight);

void LED_Flash(uint16_t period, uint16_t pulse, uint16_t cycles);

#endif //__LED_H__
