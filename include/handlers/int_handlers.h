#ifndef INT_HANDLERS_H
#define INT_HANDLERS_H

#include <stdint.h>

extern volatile uint32_t systick_counter;

// HANDLERS
void SysTick_Handler(void);

// INITS
void HSE_Init();
void SysTick_Init();
void TIM2_Init();

#endif // INT_HANDLERS_H