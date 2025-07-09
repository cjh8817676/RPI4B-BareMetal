# Raspberry Pi 4B Bare-Metal GPIO 專案

## 專案結構
```
rpi4-baremetal-gpio/
├── src/
│   ├── boot.S          # 啟動程式碼
│   ├── main.c          # 主程式
│   └── gpio.c          # GPIO 控制函式
├── include/
│   ├── types.h         # 基本類型定義
│   ├── gpio.h          # GPIO 標頭檔
│   └── rpi4.h          # RPi4 硬體定義
├── linker.ld           # 連結腳本
├── Makefile            # 編譯腳本
└── config.txt          # SD 卡配置檔
```

## 1. boot.S - 啟動程式碼
```assembly
.section ".text.boot"

.global _start

_start:
    // 讀取當前處理器 ID
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, master              // 如果是 CPU0，跳到 master
    b       hang                    // 其他 CPU 進入無限迴圈

master:
    // 設定堆疊指標
    ldr     x0, =_start
    mov     sp, x0

    // 清除 BSS 段
    ldr     x0, =__bss_start
    ldr     x1, =__bss_end
    sub     x1, x1, x0
    bl      memzero

    // 跳到 C 程式碼
    bl      main
    b       hang

hang:
    wfe
    b       hang

memzero:
    str     xzr, [x0], #8
    subs    x1, x1, #8
    b.gt    memzero
    ret
```

## 2. include/types.h - 基本類型定義
```c
#ifndef TYPES_H
#define TYPES_H

// 基本整數類型定義
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

// 大小類型
typedef unsigned long       size_t;
typedef long                ssize_t;

// 布林類型
typedef enum {
    false = 0,
    true = 1
} bool;

// NULL 定義
#define NULL ((void*)0)

#endif
```

## 3. include/rpi4.h - 硬體定義
```c
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
```

## 4. include/gpio.h - GPIO 函式宣告
```c
#ifndef GPIO_H
#define GPIO_H

#include "types.h"

// GPIO 功能定義
typedef enum {
    GPIO_FUNC_INPUT  = 0,
    GPIO_FUNC_OUTPUT = 1,
    GPIO_FUNC_ALT0   = 4,
    GPIO_FUNC_ALT1   = 5,
    GPIO_FUNC_ALT2   = 6,
    GPIO_FUNC_ALT3   = 7,
    GPIO_FUNC_ALT4   = 3,
    GPIO_FUNC_ALT5   = 2
} gpio_func_t;

// GPIO 上拉/下拉定義
typedef enum {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP   = 1,
    GPIO_PULL_DOWN = 2
} gpio_pull_t;

// GPIO 函式
void gpio_set_function(uint32_t pin, gpio_func_t func);
void gpio_set_pull(uint32_t pin, gpio_pull_t pull);
void gpio_set(uint32_t pin);
void gpio_clear(uint32_t pin);
uint32_t gpio_read(uint32_t pin);
void gpio_toggle(uint32_t pin);

#endif
```

## 5. src/gpio.c - GPIO 函式實作
```c
#include "gpio.h"
#include "rpi4.h"

void gpio_set_function(uint32_t pin, gpio_func_t func) {
    uint32_t reg_num = pin / 10;
    uint32_t bit_offset = (pin % 10) * 3;
    
    volatile uint32_t *reg = GPFSEL0 + reg_num;
    uint32_t value = *reg;
    
    // 清除現有功能位元
    value &= ~(7 << bit_offset);
    // 設定新功能
    value |= (func << bit_offset);
    
    *reg = value;
}

void gpio_set_pull(uint32_t pin, gpio_pull_t pull) {
    uint32_t reg_num = pin / 16;
    uint32_t bit_offset = (pin % 16) * 2;
    
    volatile uint32_t *reg = GPIO_PUP_PDN_CNTRL_REG0 + reg_num;
    uint32_t value = *reg;
    
    // 清除現有設定
    value &= ~(3 << bit_offset);
    // 設定新的上拉/下拉
    value |= (pull << bit_offset);
    
    *reg = value;
}

void gpio_set(uint32_t pin) {
    if (pin < 32) {
        *GPSET0 = (1 << pin);
    } else {
        *GPSET1 = (1 << (pin - 32));
    }
}

void gpio_clear(uint32_t pin) {
    if (pin < 32) {
        *GPCLR0 = (1 << pin);
    } else {
        *GPCLR1 = (1 << (pin - 32));
    }
}

uint32_t gpio_read(uint32_t pin) {
    if (pin < 32) {
        return (*GPLEV0 >> pin) & 1;
    } else {
        return (*GPLEV1 >> (pin - 32)) & 1;
    }
}

void gpio_toggle(uint32_t pin) {
    if (gpio_read(pin)) {
        gpio_clear(pin);
    } else {
        gpio_set(pin);
    }
}
```

## 6. src/main.c - 主程式（LED 閃爍範例）
```c
#include "gpio.h"
#include "rpi4.h"

// 簡單的延遲函式
void delay(uint32_t count) {
    for (volatile uint32_t i = 0; i < count; i++) {
        asm volatile("nop");
    }
}

void main(void) {
    // 使用 GPIO 21 作為 LED 輸出（可根據你的硬體調整）
    const uint32_t LED_PIN = 21;
    
    // 設定 GPIO 21 為輸出
    gpio_set_function(LED_PIN, GPIO_FUNC_OUTPUT);
    gpio_set_pull(LED_PIN, GPIO_PULL_NONE);
    
    // LED 閃爍迴圈
    while (1) {
        gpio_set(LED_PIN);      // LED 亮
        delay(5000000);         // 延遲
        gpio_clear(LED_PIN);    // LED 滅
        delay(5000000);         // 延遲
    }
}
```

## 7. linker.ld - 連結腳本
```ld
SECTIONS
{
    . = 0x80000;
    
    .text : {
        KEEP(*(.text.boot))
        *(.text .text.* .gnu.linkonce.t*)
    }
    
    .rodata : {
        *(.rodata .rodata.* .gnu.linkonce.r*)
    }
    
    .data : {
        *(.data .data.* .gnu.linkonce.d*)
    }
    
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    
    _end = .;
    
    /DISCARD/ : {
        *(.comment)
        *(.gnu*)
        *(.note*)
        *(.eh_frame*)
    }
}

__bss_size = (__bss_end - __bss_start) >> 3;
```

## 8. Makefile
```makefile
# 交叉編譯工具鏈
CROSS_COMPILE = aarch64-linux-gnu-
AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

# 編譯選項
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
CFLAGS += -I./include -mgeneral-regs-only

# 連結選項
LDFLAGS = -nostdlib -nostartfiles

# 目標檔案
TARGET = kernel8
ELF = $(TARGET).elf
IMG = $(TARGET).img

# 原始檔案
SRCS_C = src/main.c src/gpio.c
SRCS_S = src/boot.S
OBJS = $(SRCS_C:.c=.o) $(SRCS_S:.S=.o)

# 預設目標
all: $(IMG)

# 編譯規則
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) -c $< -o $@

# 連結
$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -T linker.ld $(OBJS) -o $@

# 產生 kernel8.img
$(IMG): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(IMG)

# 反組譯（除錯用）
dump: $(ELF)
	$(OBJDUMP) -D $(ELF) > $(TARGET).dump

# 清理
clean:
	rm -f src/*.o $(ELF) $(IMG) $(TARGET).dump

.PHONY: all clean dump
```

## 9. config.txt - SD 卡配置
```
# 啟用 64 位元模式
arm_64bit=1

# 設定 UART
enable_uart=1

# 關閉音訊（釋放 GPIO）
dtparam=audio=off

# 設定核心名稱
kernel=kernel8.img
```

## 編譯與使用步驟

### 1. 安裝交叉編譯工具鏈
```bash
# Ubuntu/Debian
sudo apt-get install gcc-aarch64-linux-gnu

# Arch Linux
sudo pacman -S aarch64-linux-gnu-gcc
```

### 2. 建立專案目錄結構
```bash
mkdir -p rpi4-baremetal-gpio/{src,include}
cd rpi4-baremetal-gpio

# 建立檔案（依照上面的內容）
# src/boot.S
# src/main.c
# src/gpio.c
# include/types.h
# include/gpio.h
# include/rpi4.h
# linker.ld
# Makefile
# config.txt
```

### 3. 編譯專案
```bash
make
```

### 4. 準備 SD 卡
1. 格式化 SD 卡為 FAT32
2. 從 <a href = "https://github.com/raspberrypi/firmware/tree/master/boot" Raspberry Pi 官方 </a> 下載，將boot裡面的全部內容複製到 SD 卡：
3. 複製 config.txt 到 SD 卡，取代掉原有的。
4. 複製編譯好的 kernel8.img 到 SD 卡，取代掉原有的。

### 5. 硬體連接
- 將 LED 正極連接到 GPIO 21（實體腳位 40）
- 將 LED 負極透過 330Ω 電阻連接到 GND

### 6. 測試
將 SD 卡插入 Raspberry Pi 4B 並上電，LED 應該會開始閃爍。

## LED 閃爍示範影片

<video src="https://private-user-images.githubusercontent.com/49804319/464007742-cfd6cca9-7378-422e-a6de-6b15d21e2a2a.mp4?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NTIwNDA5NDUsIm5iZiI6MTc1MjA0MDY0NSwicGF0aCI6Ii80OTgwNDMxOS80NjQwMDc3NDItY2ZkNmNjYTktNzM3OC00MjJlLWE2ZGUtNmIxNWQyMWUyYTJhLm1wND9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNTA3MDklMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjUwNzA5VDA1NTcyNVomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPTIyYTY3NDU2MGM4MTYwMDIyOTUyOWU3ZTFmOTQyY2Y4YjFjZWJkODkzMzAyYzRkNzg4ZjY0NTAyZTE3ZWNmNzcmWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0In0.FLNj5ngffX0CjDF0aq-Ef88TkgWMGtdjArBCCegmRkk" controls width="480"></video>

## 進階功能範例

### 按鈕輸入範例
```c
void button_example(void) {
    const uint32_t BUTTON_PIN = 20;
    const uint32_t LED_PIN = 21;
    
    // 設定按鈕為輸入，啟用上拉電阻
    gpio_set_function(BUTTON_PIN, GPIO_FUNC_INPUT);
    gpio_set_pull(BUTTON_PIN, GPIO_PULL_UP);
    
    // 設定 LED 為輸出
    gpio_set_function(LED_PIN, GPIO_FUNC_OUTPUT);
    
    while (1) {
        // 按鈕按下時為低電平
        if (gpio_read(BUTTON_PIN) == 0) {
            gpio_set(LED_PIN);
        } else {
            gpio_clear(LED_PIN);
        }
    }
}
```

### PWM 軟體實現範例
```c
void software_pwm(uint32_t pin, uint32_t duty_cycle) {
    const uint32_t period = 1000;
    
    gpio_set_function(pin, GPIO_FUNC_OUTPUT);
    
    while (1) {
        gpio_set(pin);
        delay(duty_cycle);
        gpio_clear(pin);
        delay(period - duty_cycle);
    }
}
```

## 除錯提示
1. 使用 `make dump` 產生反組譯檔案檢查程式碼
2. 確保 config.txt 中的設定正確
3. 檢查 GPIO 腳位號碼是否正確
4. 使用 UART 輸出除錯訊息（需要額外實作 UART 驅動）

## 注意事項
- Raspberry Pi 4B 使用 BCM2711 晶片，週邊基址與舊版不同
- GPIO 電壓為 3.3V，注意不要超過此電壓
- 某些 GPIO 腳位有特殊功能，使用前請查閱文件
