/*
blaster_port.h - USB-Blaster IO port header

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
#ifndef __BLASTER_PORT_H__
#define __BLASTER_PORT_H__

#include "base.h"

// mode set
#define BLASTER_STA_SHIFT           (0x80)
#define BLASTER_STA_READ            (0x40)
#define BLASTER_STA_CNT_MASK        (0x3f)

// bit-bang out
#define BLASTER_STA_OUT_OE          (0x20)
#define BLASTER_STA_OUT_TDI         (0x10)
#define BLASTER_STA_OUT_NCS         (0x08)
#define BLASTER_STA_OUT_NCE         (0x04)
#define BLASTER_STA_OUT_TMS         (0x02)
#define BLASTER_STA_OUT_TCK         (0x01)

// bit-bang in
#define BLASTER_STA_IN_TDO          (0x01)
#define BLASTER_STA_IN_DATAOUT      (0x02)

#define BLASTER_STA_IN_TDO_BIT      (0)
#define BLASTER_STA_IN_DATAOUT_BIT  (1)

void bport_init(void);

void bport_state_set(uint8_t d);
uint8_t bport_state_get(void);

void bport_shift_out(uint8_t d);
uint8_t bport_shift_io(uint8_t d);

#endif //__BLASTER_PORT_H__
