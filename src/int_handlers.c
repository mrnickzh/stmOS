#include "int_handlers.h"

#include "scheduler.h"
#include "Cortex-M4.h"
#include "STM32F411xE.h"

volatile uint32_t systick_counter = 0;

void SysTick_Handler(void)
{
    uint32_t now = ++systick_counter;
    int woke = 0;
    
    task_ctx_t *t = g_blocked_list;
    task_ctx_t *prev = 0;
    while (t) {
        task_ctx_t *next = t->next;
        if ((int32_t)(now - t->wake_at) >= 0) {

            if (prev) { prev->next = next; }
			else { g_blocked_list = next; }
            
            t->state = TASK_READY;
            ready_push(t);
            woke = 1;
        } else {
            prev = t;
        }
        t = next;
    }
    
    *(volatile uint32_t *)PENDSVSET = (1u << 28);   // PENDSVSET
}

// void TIM2_Handler()
// {
    // // Clear TIM2 update interrupt flag
    // BIT_CLEAR(TIM2->SR, TIM_SR_UIF);

    // // Toggle GPIOC pin 13
    // BIT_TOGGLE(GPIOC->ODR, GPIO_ODR_ODR13);
// }

// void SVC_Handler()
// {
    // svc_call_count++;
    // // stub
// }

void HSE_Init() 
{
	// Enable HSE clock
    BIT_SET(RCC->CR, RCC_CR_HSEON);

    // Wait until HSE clock becomes ready
    while (!BIT_CHECK(RCC->CR, RCC_CR_HSERDY))
    {
        // Do nothing.
    }

    // Select HSE clock as system clock
    BIT_CLEAR(RCC->CFGR, RCC_CFGR_SW_MASK);
    BIT_SET(RCC->CFGR, RCC_CFGR_SW_HSE);

    // Make sure that HSE clock is selected as system clock
    while (BIT_CHECK(RCC->CFGR, RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_HSE)
    {
        // Do nothing.
    }
}

void SysTick_Init()
{
	// Set SysTick reload value
    BIT_CLEAR(STK->LOAD, STK_LOAD_RELOAD_MASK);
    BIT_SET(STK->LOAD, SYSTICK_FREQ - 1);

    // Set processor clock as SysTick clock source
    BIT_SET(STK->CTRL, STK_CTRL_CLKSOURCE);

    // Enable SysTick exception request
    BIT_SET(STK->CTRL, STK_CTRL_TICKINT);

    // Enable SysTick counter
    BIT_SET(STK->CTRL, STK_CTRL_ENABLE);
}

void TIM2_Init()
{
	// Enable TIM2 clock
    BIT_SET(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);

    // Set TIM2 prescaler
    TIM2->PSC = 1000 - 1;

    // Set TIM2 auto-reload register
    TIM2->ARR = 12500 - 1;

    // Enable TIM2 update interrupt
    BIT_SET(TIM2->DIER, TIM_DIER_UIE);

    // Generate TIM2 update
    BIT_SET(TIM2->EGR, TIM_EGR_UG);

    // Enable TIM2 counter
    BIT_SET(TIM2->CR1, TIM_CR1_CEN);

    // Enable TIM2 interrupt
    irq_enable(TIM2_IRQn);	
}