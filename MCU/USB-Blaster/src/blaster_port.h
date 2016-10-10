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
