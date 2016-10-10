#ifndef __FT245ROM_H__
#define __FT245ROM_H__

#define FT245ROM_SIZE           (128)
#define FT245ROM_STR_LIMIT      (100)

int ft245rom_init(void);

uint16_t ft245rom_read(uint8_t addr);

#endif //__FT245ROM_H__
