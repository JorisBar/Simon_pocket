#ifndef GAME_H
#define GAME_H

#include <stdint.h>

// Callbacks from hardware interrupts
void exti_callback(uint8_t color);
void rtc_callback(void);

// Game Logic
void game_generate_new_sequence(void);

#endif // GAME_H
