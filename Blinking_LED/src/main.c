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