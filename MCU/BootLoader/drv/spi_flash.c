/*
 * W25和SST25系列SPI_Flash底层驱动函数 for stm32f103c8 & stm32l151c8
 * 必须在头文件选择器件型号
 * eleqian 2014-9-11
 */

#include "base.h"
#include "spi_flash.h"

// Select SPI FLASH: ChipSelect pin low
#define SPI_SelectDevice(d)     GPIO_ResetBits(GPIOB, GPIO_Pin_1)
// Deselect SPI FLASH: ChipSelect pin high
#define SPI_DeselectDevice(d)   GPIO_SetBits(GPIOB, GPIO_Pin_1)

/*
* Initializes the peripherals used by the SPI FLASH driver.
*/
void SPI_Flash_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

#ifdef MCU_STM32F103C8
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // 关JTAG，仅使用SWD
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    // 重映射SPI1引脚
    GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
#endif //MCU_STM32F103C8
#ifdef MCU_STM32L151C8
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
#endif //MCU_STM32L151C8

    // 配置SPI引脚: SCK, MISO 和 MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
#ifdef MCU_STM32F103C8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#endif //MCU_STM32F103C8
#ifdef MCU_STM32L151C8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
#endif //MCU_STM32L151C8
    GPIO_Init(GPIOB, &GPIO_InitStructure);

#ifdef MCU_STM32L151C8
      /* Connect PXx to SD_SPI_SCK */
      GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
      /* Connect PXx to SD_SPI_MISO */
      GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1); 
      /* Connect PXx to SD_SPI_MOSI */
      GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);  
#endif //MCU_STM32L151C8

    // 配置片选: CS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
#ifdef MCU_STM32F103C8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
#endif //MCU_STM32F103C8
#ifdef MCU_STM32L151C8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
#endif //MCU_STM32L151C8
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    SPI_DeselectDevice(SPI_DEVICE_FLASH);

    // 使能SPI1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 , ENABLE);

    // SPI1配置
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // 使能SPI1
    SPI_Cmd(SPI1, ENABLE);
}

/*
* Description : Sends a byte through the SPI interface and return the byte received from the SPI bus.
* Input       : byte : byte to send.
* Output      : None
* Return      : The value of the received byte.
*/
uint8_t SPI_TransmitByte(uint8_t byte)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI peripheral */
    SPI_I2S_SendData(SPI1, byte);
    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}

/*********************************************************************************************************/

/**
  * @brief  Polls the status of the BUSY flag in the FLASH's status register and waits for
            the end of Flash writing.
  * @retval None
  **/
void SPI_Flash_WaitForWriteEnd(void)
{
    uint8_t Flash_Status = 0;

    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);

    /* Send "Read Status Register" instruction */
    SPI_TransmitByte(SPI_FLASH_CMD_RDSR);

    /* Loop as long as the busy flag is set */
    do {
        /* Send a dummy byte to generate the clock to read the value of the status register */
        Flash_Status = SPI_TransmitByte(SPI_FLASH_DUMMY_BYTE);
    } while ((Flash_Status & SPI_FLASH_BUSY_FLAG) == SET);

    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);
}

/**
  * @brief  Reads JEDEC ID.
  * @retval JEDEC ID value.
  **/
uint32_t SPI_Flash_ReadJEDECID(void)
{
    uint32_t Temp, Temp0, Temp1, Temp2;

    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);

    /* Send "JEDEC Read-ID" instruction */
    SPI_TransmitByte(SPI_FLASH_CMD_JEDECID);
    /* Read a byte from the FLASH */
    Temp0 = SPI_TransmitByte(SPI_FLASH_DUMMY_BYTE);
    /* Read a byte from the FLASH */
    Temp1 = SPI_TransmitByte(SPI_FLASH_DUMMY_BYTE);
    /* Read a byte from the FLASH */
    Temp2 = SPI_TransmitByte(SPI_FLASH_DUMMY_BYTE);

    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);

    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

    return Temp;
}

/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  **/
void SPI_Flash_SectorErase(uint32_t SectorAddr)
{
    /* Send write enable instruction */
    SPI_Flash_WriteEnable();

    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);
    /* Send "Sector Erase" instruction */
    SPI_TransmitByte(SPI_FLASH_CMD_SE);
    /* Send high byte address of SectorAddr */
    SPI_TransmitByte((SectorAddr & 0xFF0000) >> 16);
    /* Send medium byte address of SectorAddr */
    SPI_TransmitByte((SectorAddr & 0xFF00) >> 8);
    /* Send low byte address of SectorAddr */
    SPI_TransmitByte(SectorAddr & 0xFF);
    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);

    /* Wait the write opertaion has completed */
    SPI_Flash_WaitForWriteEnd();
}

/**
  * @brief  Erases the entire FLASH.
  * @retval None
  **/
void SPI_Flash_ChipErase(void)
{
    /* Send write enable instruction */
    SPI_Flash_WriteEnable();

    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);
    /* Send "Chip Erase" instruction  */
    SPI_TransmitByte(SPI_FLASH_CMD_CE);
    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);

    /* Wait the write opertaion has completed */
    SPI_Flash_WaitForWriteEnd();
}

/**
  * @brief  Reads a block of data from the FLASH.
  * @param  ReadAddr : FLASH's internal address to read from.
  * @param  ReadBuffer : pointer to the buffer that receives the data read from the FLASH.
  * @param  NumOfReadByte : number of bytes to read from the FLASH.
  * @retval None
  **/
void SPI_Flash_Read(uint32_t ReadAddr, void *ReadBuffer, uint16_t NumOfReadByte)
{
    uint8_t *pBuf = (uint8_t *)ReadBuffer;

    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);

    /* Send "Read from Memory " instruction */
    SPI_TransmitByte(SPI_FLASH_CMD_READ);

    /* Send high byte address of ReadAddr */
    SPI_TransmitByte((ReadAddr & 0xFF0000) >> 16);
    /* Send medium byte address of ReadAddr */
    SPI_TransmitByte((ReadAddr & 0xFF00) >> 8);
    /* Send low byte address of ReadAddr */
    SPI_TransmitByte(ReadAddr & 0xFF);

    while (NumOfReadByte--) {
        /* Read a byte from the FLASH */
        *pBuf = SPI_TransmitByte(SPI_FLASH_DUMMY_BYTE);
        /* Point to the next location where the byte read will be saved */
        pBuf++;
    }

    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);
}

#ifdef SPI_FLASH_SST25
/**
  * @brief  Writes one byte to the FLASH.
  * @param  WriteAddr : FLASH's internal address to write to.
  * @param  DataByte : the data byte to be written to the FLASH.
  * @retval None
  **/
void SPI_Flash_ByteWrite(uint32_t WriteAddr, uint8_t DataByte)
{
    /* Enable the write access to the FLASH */
    SPI_Flash_WriteEnable();

    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);
    /* Send "Program one data byte " instruction */
    SPI_TransmitByte(SPI_FLASH_CMD_BP);
    /* Send high byte address of WriteAddr */
    SPI_TransmitByte((WriteAddr & 0xFF0000) >> 16);
    /* Send medium byte address of WriteAddo */
    SPI_TransmitByte((WriteAddr & 0xFF00) >> 8);
    /* Send low byte address of WriteAddr */
    SPI_TransmitByte(WriteAddr & 0xFF);

    /* Send a byte to the FLASH */
    SPI_TransmitByte(DataByte);

    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);

    /* Wait the write opertaion has completed */
    SPI_Flash_WaitForWriteEnd();
}

/**
  * @brief  Writes block of data to the FLASH using Automatic Address Increment Programming.
  * @param  WriteAddr : FLASH's internal address to write to.
  * @param  WriteBuffer : pointer to the buffer  containing the data to be written to the FLASH.
  * @param  NumOfWriteByte : number of bytes to write to the FLASH.
  * @retval None
  **/
void SPI_Flash_AAIWrite(uint32_t WriteAddr, const void *WriteBuffer, uint16_t_t NumOfWriteByte)
{
    uint16_t_t temp, NumOfWrite = NumOfWriteByte / 2;
    uint8_t *pBuf = (uint8_t *)WriteBuffer;

    /* Enable the write access to the FLASH */
    SPI_Flash_WriteEnable();

    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);
    /* Send "Automatic Address Increment Programming" instruction */
    SPI_TransmitByte(SPI_FLASH_CMD_AAI);
    /* Send high byte address of WriteAddr */
    SPI_TransmitByte((WriteAddr & 0xFF0000) >> 16);
    /* Send medium byte address of WriteAddr */
    SPI_TransmitByte((WriteAddr & 0xFF00) >> 8);
    /* Send low byte address of WriteAddr */
    SPI_TransmitByte(WriteAddr & 0xFF);

    for (temp = 0; temp < 2; temp++) {
        /* Send the current byte */
        SPI_TransmitByte(*pBuf);
        /* Point on the next byte to be written */
        pBuf++;
    }

    NumOfWrite--;

    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);

    /* Wait the write opertaion has completed */
    SPI_Flash_WaitForWriteEnd();

    if (NumOfWrite) {
        while (NumOfWrite--) {
            /* Select the SPI FLASH */
            SPI_SelectDevice(SPI_DEVICE_FLASH);
            /* Send "Automatic Address Increment Programming" instruction */
            SPI_TransmitByte(SPI_FLASH_CMD_AAI);

            for (temp = 0; temp < 2; temp++) {
                /* Send the current byte */
                SPI_TransmitByte(*pBuf);
                /* Point on the next byte to be written */
                pBuf++;
            }

            /* Deselect the SPI FLASH */
            SPI_DeselectDevice(SPI_DEVICE_FLASH);

            /* Wait the end of Flash writing */
            SPI_Flash_WaitForWriteEnd();
        }
    }

    /* Disable the write access to the FLASH */
    SPI_Flash_WriteDisable();
}

/**
  * @brief  Writes block of data to the FLASH.
  * @param  WriteAddr : FLASH's internal address to write to.
  * @param  WriteBuffer : pointer to the buffer  containing the data to be written to the FLASH.
  * @param  NumOfWriteByte : number of bytes to write to the FLASH.
  * @retval None
  **/
void SPI_Flash_Write(uint32_t WriteAddr, const void *WriteBuffer, uint16_t_t NumOfWriteByte)
{
    uint8_t NumOfSingle = 0, Addr = 0;
    uint16_t_t NumOfPage = 0;
    uint8_t *pBuf = (uint8_t *)WriteBuffer;

    Addr = WriteAddr % 2;
    NumOfPage =  NumOfWriteByte / 2;
    NumOfSingle = NumOfWriteByte % 2;

    if (Addr == 0) {
        if (NumOfPage == 0) { /* Only one byte to be written to */
            SPI_Flash_ByteWrite(WriteAddr, *pBuf);
        } else {
            SPI_Flash_AAIWrite(WriteAddr, pBuf, NumOfWriteByte);
            WriteAddr +=  NumOfWriteByte;
            pBuf += NumOfWriteByte;
        }

        if (NumOfSingle) {
            SPI_Flash_ByteWrite(WriteAddr, *pBuf);
        }
    } else {
        if (NumOfPage == 0) { /* Only one byte to be written to */
            SPI_Flash_ByteWrite(WriteAddr, *pBuf);
        } else {
            SPI_Flash_ByteWrite(WriteAddr, *pBuf);
            WriteAddr +=  1;
            pBuf += 1;

            SPI_Flash_AAIWrite(WriteAddr, pBuf, NumOfWriteByte);
            WriteAddr +=  NumOfWriteByte;
            pBuf += NumOfWriteByte;

            if (NumOfSingle) {
                SPI_Flash_ByteWrite(WriteAddr, *pBuf);
            }
        }
    }
}
#endif //SPI_FLASH_SST25

#ifdef SPI_FLASH_W25
// 在一页(0~65535)内写入少于256个字节的数据
// 参数：开始写入的地址(24bit)，数据存储区，要写入的字节数(最大256，不应该超过该页的剩余字节数)
void SPI_Flash_Write_Page(uint32_t WriteAddr, const void *WriteBuffer, uint16_t NumByteToWrite)
{
    uint16_t i;
    const uint8_t *pBuffer = WriteBuffer;

    SPI_Flash_WriteEnable();                    //SET WEL

    SPI_SelectDevice(SPI_DEVICE_FLASH);         //使能器件

    SPI_TransmitByte(SPI_FLASH_CMD_BP);         //发送写页命令

    // 发送24bit地址
    SPI_TransmitByte((uint8_t)((WriteAddr) >> 16));
    SPI_TransmitByte((uint8_t)((WriteAddr) >> 8));
    SPI_TransmitByte((uint8_t)WriteAddr);

    // 循环写
    for (i = 0; i < NumByteToWrite; i++) {
        SPI_TransmitByte(pBuffer[i]);
    }

    SPI_DeselectDevice(SPI_DEVICE_FLASH);       //取消片选

    SPI_Flash_WaitForWriteEnd();                //等待写入结束
}

// 写SPI FLASH
// 必须确保所写的地址范围内的数据全部为0XFF，否则在非0XFF处写入的数据将失败
// 在指定地址开始写入指定长度的数据，具有自动换页功能，但是要确保地址不越界
// 参数：开始写入的地址(24bit)，数据存储区，要写入的字节数(最大65535)
void SPI_Flash_Write(uint32_t WriteAddr, const void *WriteBuffer, uint16_t NumByteToWrite)
{
    const uint8_t *pBuffer = WriteBuffer;
    uint16_t pageremain;

    pageremain = 256 - WriteAddr % 256; //单页剩余的字节数
    if (NumByteToWrite <= pageremain) {
        pageremain = NumByteToWrite;    //不大于256个字节
    }

    while (1) {
        SPI_Flash_Write_Page(WriteAddr, pBuffer, pageremain);
        if (NumByteToWrite == pageremain) {
            break;    //写入结束
        } else {
            pBuffer += pageremain;
            WriteAddr += pageremain;
            NumByteToWrite -= pageremain;       // 减去已经写入的字节数
            if (NumByteToWrite > 256) {
                pageremain = 256;               // 一次可以写入256个字节
            } else {
                pageremain = NumByteToWrite;    // 不够256个字节
            }
        }
    }
}
#endif //SPI_FLASH_W25

/**
  * @brief  Enables the write access to the FLASH.
  * @retval None
  **/
void SPI_Flash_WriteEnable(void)
{
    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);

    /* Send "Write Enable" instruction */
    SPI_TransmitByte(SPI_FLASH_CMD_WREN);

    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);
}

/**
  * @brief  Disables the write access to the FLASH.
  * @retval None
  **/
void SPI_Flash_WriteDisable(void)
{
    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);

    /* Send "Write Disable" instruction */
    SPI_TransmitByte(SPI_FLASH_CMD_WRDI);

    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);
}

/**
  * @brief  Writes the new value to the Status Register.
  * @param  Value : the new value to be written to the Status Register.
  * @retval None
  * @update eleqian 2013-5-5
  **/
void SPI_Flash_WriteStatus(uint8_t Value)
{
    SPI_Flash_WriteEnable();

    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);

    /* Send "Enable Write Status Register" instruction */
    //SPI_TransmitByte(SPI_FLASH_CMD_EWSR);
    /* Send "Write Status Register" instruction */
    SPI_TransmitByte(SPI_FLASH_CMD_WRSR);
    /* Writes to the Status Register */
    SPI_TransmitByte(Value);

    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);
}

/**
  * @brief  Reads the value of the Status Register.
  * @param  None
  * @retval The value of the Status Register.
  * @author eleqian 2013-5-5
  **/
uint8_t SPI_Flash_ReadStatus(void)
{
    uint8_t Flash_Status = 0;

    /* Select the SPI FLASH */
    SPI_SelectDevice(SPI_DEVICE_FLASH);

    /* Send "Read Status Register" instruction */
    SPI_TransmitByte(SPI_FLASH_CMD_RDSR);
    /* Reads the Status Register */
    Flash_Status = SPI_TransmitByte(SPI_FLASH_DUMMY_BYTE);

    /* Deselect the SPI FLASH */
    SPI_DeselectDevice(SPI_DEVICE_FLASH);

    return Flash_Status;
}

// 启用所有块写保护
void SPI_Flash_ProtectionEnable(void)
{
    SPI_Flash_WriteStatus(0x3c);
}

// 禁用所有块写保护
void SPI_Flash_ProtectionDisable(void)
{
    SPI_Flash_WriteStatus(0x00);
}
