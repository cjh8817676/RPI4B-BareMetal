#ifndef RPI4_H
#define RPI4_H

#include "types.h"

// BCM2711 週邊基址
#define PERIPHERAL_BASE     0xFE000000

// GPIO 暫存器基址
#define GPIO_BASE           (PERIPHERAL_BASE + 0x00200000)

// GPIO 功能選擇暫存器
#define GPFSEL0             ((volatile uint32_t *)(GPIO_BASE + 0x00))
#define GPFSEL1             ((volatile uint32_t *)(GPIO_BASE + 0x04))
#define GPFSEL2             ((volatile uint32_t *)(GPIO_BASE + 0x08))

// GPIO 輸出設定暫存器
#define GPSET0              ((volatile uint32_t *)(GPIO_BASE + 0x1C))
#define GPSET1              ((volatile uint32_t *)(GPIO_BASE + 0x20))

// GPIO 輸出清除暫存器
#define GPCLR0              ((volatile uint32_t *)(GPIO_BASE + 0x28))
#define GPCLR1              ((volatile uint32_t *)(GPIO_BASE + 0x2C))

// GPIO 電平暫存器
#define GPLEV0              ((volatile uint32_t *)(GPIO_BASE + 0x34))
#define GPLEV1              ((volatile uint32_t *)(GPIO_BASE + 0x38))

// GPIO 上拉/下拉控制
#define GPIO_PUP_PDN_CNTRL_REG0  ((volatile uint32_t *)(GPIO_BASE + 0xE4))
#define GPIO_PUP_PDN_CNTRL_REG1  ((volatile uint32_t *)(GPIO_BASE + 0xE8))
#define GPIO_PUP_PDN_CNTRL_REG2  ((volatile uint32_t *)(GPIO_BASE + 0xEC))
#define GPIO_PUP_PDN_CNTRL_REG3  ((volatile uint32_t *)(GPIO_BASE + 0xF0))

#endif