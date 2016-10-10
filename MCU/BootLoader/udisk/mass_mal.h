#ifndef __MASS_MAL_H__
#define __MASS_MAL_H__

#include "base.h"

// 打开宏时使用外部SPI Flash
//#define MAL_SPI_FLASH

#define MAL_OK      0
#define MAL_FAIL    1

// 定义存储设备数
#ifdef MAL_SPI_FLASH
#define MAX_LUN     2
#else
#define MAX_LUN     1
#endif // MAL_SPI_FLASH

// 定义内部Flash可用大小，保留16K作为程序空间
#ifdef MCU_STM32L151C8
#define INTER_FLASH_PAGE_SIZE       (256)
#define INTER_FLASH_PAGE_COUNT      (256 - 64)
#define INTER_FLASH_BASE_ADDR       (0x08000000 + 256 * 64)
#endif //MCU_STM32L151C8
#ifdef MCU_STM32F103C8
#define INTER_FLASH_PAGE_SIZE       (1024)
#define INTER_FLASH_PAGE_COUNT      (128 - 16)  // 使用后面的64k隐藏空间
#define INTER_FLASH_BASE_ADDR       (0x08000000 + 1024 * 16)
#endif //MCU_STM32L151C8

// 最大存储设备块大小，必须为BULK_MAX_PACKET_SIZE(64)整数倍，至少512
#ifdef MAL_SPI_FLASH
// 这里取4096是适应SPI Flash，只用其它设备取较小的值可减小SRAM使用
#define MAL_BLOCK_SIZE_MAX      4096
#else
// 只使用内部Flash时就定义为页大小
#define MAL_BLOCK_SIZE_MAX      INTER_FLASH_PAGE_SIZE
#endif //MAL_SPI_FLASH

// 保证块大小>=512
#if MAL_BLOCK_SIZE_MAX < 512
#undef MAL_BLOCK_SIZE_MAX
#define MAL_BLOCK_SIZE_MAX      512
#endif //MAL_BLOCK_SIZE_MAX

extern uint32_t Mass_Block_Size[MAX_LUN];      // 块大小
extern uint32_t Mass_Block_Count[MAX_LUN];     // 块数目
extern uint32_t Mass_Memory_Size[MAX_LUN];     // 空间大小

uint16_t MAL_Init(uint8_t lun);
uint16_t MAL_GetStatus(uint8_t lun);
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint8_t *Readbuff, uint16_t Transfer_Length);
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, const uint8_t *Writebuff, uint16_t Transfer_Length);

#endif /* __MASS_MAL_H__ */
