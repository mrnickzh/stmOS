#include "STM32F411xE.h"
#include "int_handlers.h"
#include "scheduler.h"
#include "num2ascii.h"
#include "semihosting.h"

volatile uint32_t svc_call_count = 0;

// Delay for `t` milliseconds.
void delay_milliseconds(uint32_t t)
{
    const uint32_t start = systick_counter;

    while (systick_counter - start < t)
    {
        // Do nothing
    }
}

mutex_t print_lock;

void taska(void) {
    for(;;) {
        mutex_lock(&print_lock);
		
        int offset = 0;
		char str[64];
		offset += s2ascii("[Task A] systick: ", (str + offset));
		offset += ui2ascii(systick_counter, (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
		
        mutex_unlock(&print_lock);
		
        task_sleep(100);
    }
}

void taskb(void) {
    for(;;) {
        mutex_lock(&print_lock);
		
        int offset = 0;
		char str[64];
		offset += s2ascii("[Task B] systick: ", (str + offset));
		offset += ui2ascii(systick_counter, (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
		
        mutex_unlock(&print_lock);
		
        task_sleep(100);
    }
}

void taskc(void) {
    for(;;) {
        mutex_lock(&print_lock);
		
        int offset = 0;
		char str[64];
		offset += s2ascii("[Task C] systick: ", (str + offset));
		offset += ui2ascii(systick_counter, (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
		
        mutex_unlock(&print_lock);
		
        task_sleep(100);
    }
}

int main()
{
    HSE_Init();
	
	scheduler_init();
	SysTick_Init();
	
	// TIM2_Init();

    // // Enable GPIOC
    // BIT_SET(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

    // // Set GPIOC pin 13 mode to output
    // BIT_CLEAR(GPIOC->MODER, GPIO_MODER_MODER13_MASK);
    // BIT_SET(GPIOC->MODER, GPIO_MODER_MODER13_OUTPUT);
	
	int offset = 0;
    char str[128];
    offset += s2ascii("KERNEL LIVE!\n", (str + offset));
    str[offset] = '\0';
	semihost_write(str, offset);
	
	task_create("IDLE", idle_task, 128);
	
	task_create("A", taska, 256);
    task_create("B", taskb, 256);
	task_create("C", taskc, 256);

    scheduler_start();

    while (1)
    {
        // Do nothing.
    }
}