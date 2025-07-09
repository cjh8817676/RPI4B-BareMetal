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