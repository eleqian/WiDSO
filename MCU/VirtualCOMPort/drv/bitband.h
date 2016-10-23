#ifndef __BITBAND_H__
#define __BITBAND_H__

// 位带操作
#define BITBAND(addr, bitn)     (((addr) & 0xF0000000) + 0x2000000 + (((addr) & 0xFFFFF) << 5) + ((bitn) << 2))
#define MEM_ADDR(addr)          *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitn)    MEM_ADDR(BITBAND(addr, bitn))

// 位带操作SRAM简化版，提高效率
#define BITBAND_SRAM(addr, bitn)    (0x22000000 + (((addr) & 0xFFFFF) << 5) + ((bitn) << 2))
#define BIT_ADDR_SRAM(addr, bitn)   MEM_ADDR(BITBAND_SRAM(addr, bitn))

// 位带操作GPIO简化版，提高效率
#define BITBAND_IO(addr, bitn)      (0x42000000 + (((addr) & 0xFFFFF) << 5) + ((bitn) << 2))
#define BIT_ADDR_IO(addr, bitn)     MEM_ADDR(BITBAND_IO(addr, bitn))

// IO口地址映射
#define GPIOA_ODR_Addr      (GPIOA_BASE + 12) //0x4001080C 
#define GPIOB_ODR_Addr      (GPIOB_BASE + 12) //0x40010C0C 
#define GPIOC_ODR_Addr      (GPIOC_BASE + 12) //0x4001100C 
#define GPIOD_ODR_Addr      (GPIOD_BASE + 12) //0x4001140C 
#define GPIOE_ODR_Addr      (GPIOE_BASE + 12) //0x4001180C 
#define GPIOF_ODR_Addr      (GPIOF_BASE + 12) //0x40011A0C    
#define GPIOG_ODR_Addr      (GPIOG_BASE + 12) //0x40011E0C    

#define GPIOA_IDR_Addr      (GPIOA_BASE + 8) //0x40010808 
#define GPIOB_IDR_Addr      (GPIOB_BASE + 8) //0x40010C08 
#define GPIOC_IDR_Addr      (GPIOC_BASE + 8) //0x40011008 
#define GPIOD_IDR_Addr      (GPIOD_BASE + 8) //0x40011408 
#define GPIOE_IDR_Addr      (GPIOE_BASE + 8) //0x40011808 
#define GPIOF_IDR_Addr      (GPIOF_BASE + 8) //0x40011A08 
#define GPIOG_IDR_Addr      (GPIOG_BASE + 8) //0x40011E08 

// IO口操作
// 确保n的值等于0~15
#define PAO(n)              BIT_ADDR_IO(GPIOA_ODR_Addr, n)
#define PAI(n)              BIT_ADDR_IO(GPIOA_IDR_Addr, n)

#define PBO(n)              BIT_ADDR_IO(GPIOB_ODR_Addr, n)
#define PBI(n)              BIT_ADDR_IO(GPIOB_IDR_Addr, n)

#define PCO(n)              BIT_ADDR_IO(GPIOC_ODR_Addr, n)
#define PCI(n)              BIT_ADDR_IO(GPIOC_IDR_Addr, n)

#define PDO(n)              BIT_ADDR_IO(GPIOD_ODR_Addr, n)
#define PDI(n)              BIT_ADDR_IO(GPIOD_IDR_Addr, n)

#define PEO(n)              BIT_ADDR_IO(GPIOE_ODR_Addr, n)
#define PEI(n)              BIT_ADDR_IO(GPIOE_IDR_Addr, n)

#define PFO(n)              BIT_ADDR_IO(GPIOF_ODR_Addr, n)
#define PFI(n)              BIT_ADDR_IO(GPIOF_IDR_Addr, n)

#define PGO(n)              BIT_ADDR_IO(GPIOG_ODR_Addr, n)
#define PGI(n)              BIT_ADDR_IO(GPIOG_IDR_Addr, n)

#endif //__BITBAND_H__
