#ifndef HARDWARE_H
#define HARDWARE_H

#include "stm32f030x6.h"
#include <stdint.h>
#include <stdbool.h>

#define RTC_PREDIV_SS 0x137

typedef enum {
    COLOR_RED = 0,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_COUNT
} color_t;

// Hardware Initialization
void system_clock_config(void);
void gpio_init(void);
void exti_init(void);
void rtc_init(void);
void adc_init(void);

// Hardware Actions
void rtc_delay_ms(uint32_t delay_ms);

void adc_enable(void);
void adc_disable(void);
uint8_t adc_generate_random_2bit(void);

void led_toggle_all(bool state);
void led_on(uint8_t color);
void led_off(uint8_t color);
bool led_is_on(uint8_t color);
bool button_is_pressed(uint8_t color);

#endif // HARDWARE_H
