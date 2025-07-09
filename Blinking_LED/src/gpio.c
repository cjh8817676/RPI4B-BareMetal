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