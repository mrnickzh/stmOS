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

volatile uint32_t counter_a = 0;
volatile uint32_t counter_b = 0;
volatile uint32_t counter_c = 0;

void taska(void)
{
    while (1) {
        counter_a++;
        task_sleep(100);
        // task_yield();
    }
}

void taskc(void)
{
    while (1) {
        counter_c++;
        task_sleep(100);
        // task_yield();
		if (counter_c > 100) {
			counter_c = 0;
			
			int offset = 0;
			char str[64];
			offset += s2ascii("taskC killed at ", (str + offset));
			offset += ui2ascii(systick_counter, (str + offset));
			offset += s2ascii("\n", (str + offset));
			str[offset] = '\0';
			semihost_write(str, offset);
			
			task_exit();
		}
    }
}

void taskb(void)
{
    while (1) {
        counter_b++;
        task_sleep(20);
        // task_yield();
		if (counter_b % 10 == 0) {
			if (counter_b >= 100) { task_exit(); }
			
			int offset = 0;
			char str[64];
			offset += s2ascii("taskC spawned at ", (str + offset));
			offset += ui2ascii(systick_counter, (str + offset));
			offset += s2ascii("\n", (str + offset));
			str[offset] = '\0';
			semihost_write(str, offset);
			
			task_create("C", taskc, 128);
		}
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
	
	task_create("A", taska, 128);
    task_create("B", taskb, 128);

    scheduler_start();

    while (1)
    {
        // Do nothing.
    }
}