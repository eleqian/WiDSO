#ifndef __SPI_FLASH__
#define __SPI_FLASH__

#include "base.h"

// 定义器件类型
//#define SPI_FLASH_SST25
#define SPI_FLASH_W25

// 定义器件型号
//#define SPI_FLASH_SST25VF016B
#define SPI_FLASH_W25Q64BV
//#define SPI_FLASH_W25X40BV

// 器件信息
#ifdef SPI_FLASH_SST25VF016B
#define SPI_FLASH_JEDEC_ID              0xBF2541
// Falsh块大小和个数，2M=4k*512
#define SPI_FLASH_SECTOR_SIZE           4096
#define SPI_FLASH_SECTOR_COUNT          512
#endif //SPI_FLASH_SST25VF016B

#ifdef SPI_FLASH_W25Q64BV
#define SPI_FLASH_JEDEC_ID              0xEF4017
// Falsh块大小和个数，8M=4k*2048
#define SPI_FLASH_SECTOR_SIZE           4096
#define SPI_FLASH_SECTOR_COUNT          2048
#endif // SPI_FLASH_W25Q64BV

#ifdef SPI_FLASH_W25X40BV
#define SPI_FLASH_JEDEC_ID              0xEF3013
// Falsh块大小和个数，512k=4k*128
#define SPI_FLASH_SECTOR_SIZE           4096
#define SPI_FLASH_SECTOR_COUNT          128
#endif // SPI_FLASH_W25X40BV

#define SPI_FLASH_CMD_BP                0x02  /* Program one data byte instruction */
#define SPI_FLASH_CMD_AAI               0xAD  /* Auto Address Increment Program instruction */
#define SPI_FLASH_CMD_EWSR              0x50  /* Enable write Status Register instruction */
#define SPI_FLASH_CMD_WRSR              0x01  /* Write Status Register instruction */
#define SPI_FLASH_CMD_WREN              0x06  /* Write enable instruction */
#define SPI_FLASH_CMD_WRDI              0x04  /* Write disable instruction */

#define SPI_FLASH_CMD_READ              0x03  /* Read Data instruction */
#define SPI_FLASH_CMD_RDSR              0x05  /* Read Status Register instruction  */
#define SPI_FLASH_CMD_RDID              0x90  /* Read ID instruction  */
#define SPI_FLASH_CMD_JEDECID           0x9F  /* Read JEDEC ID instruction  */
#define SPI_FLASH_CMD_SE                0x20  /* Sector Erase instruction */
#define SPI_FLASH_CMD_CE                0x60  /* Chip Erase instruction */

#define SPI_FLASH_BUSY_FLAG             0x01  /* Write operation in progress */

#define SPI_FLASH_DUMMY_BYTE            0xA5

void SPI_Flash_Init(void);

uint32_t SPI_Flash_ReadJEDECID(void);

void SPI_Flash_ChipErase(void);

void SPI_Flash_SectorErase(uint32_t SectorAddr);

void SPI_Flash_Write(uint32_t WriteAddr, const void *WriteBuffer, uint16_t NumOfWriteByte);

void SPI_Flash_Read(uint32_t ReadAddr, void *ReadBuffer, uint16_t NumOfReadByte);

void SPI_Flash_WaitForWriteEnd(void);

uint8_t SPI_Flash_ReadStatus(void);

void SPI_Flash_WriteStatus(uint8_t status);

void SPI_Flash_WriteEnable(void);

void SPI_Flash_WriteDisable(void);

void SPI_Flash_ProtectionEnable(void);

void SPI_Flash_ProtectionDisable(void);

#endif //__SPI_FLASH__
