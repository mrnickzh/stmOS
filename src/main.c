#include "platform/STM32F411xE.h"
#include "handlers/int_handlers.h"
#include "scheduler/scheduler.h"
#include "utils/num2ascii.h"
#include "debug/semihosting.h"
#include "memory/page.h"
#include "config.h"
#include "platform/board.h"
#include "drivers/button.h"
#include "drivers/ssd1306.h"
#include "shell/term.h"
#include "shell/vkbd.h"
#include "shell/shell.h"

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
		
		task_sleep(100);
        mutex_unlock(&print_lock);
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
		
		task_sleep(10);
        mutex_unlock(&print_lock);
    }
}

void memtest(void) {
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("KERNEL LIVE!\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	void *ptrs[32] = {0};
	for (int round = 0; round < 4; round++) {
		// alloc
		for (int i = 0; i < 8; i++) {
			uint32_t sz = 2 * round;
			ptrs[i] = page_alloc(sz);
			
			int offset = 0;
			char str[128];
			offset += s2ascii("alloc addr: 0x", (str + offset));
			offset += i2hex((uint32_t)(ptrs[i]), (str + offset));
			offset += s2ascii("\n", (str + offset));
			str[offset] = '\0';
			semihost_write(str, offset);
		}
		// free
		for (int i = 7; i >= 0; i--) {
			if (ptrs[i]) { page_free(ptrs[i], 2 * round); ptrs[i] = 0; }
		}
	}
	
	void *slot1 = page_alloc(2);;
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("slot1 addr: 0x", (str + offset));
		offset += i2hex((uint32_t)(slot1), (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	void *slot2 = page_alloc(2);;
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("slot2 addr: 0x", (str + offset));
		offset += i2hex((uint32_t)(slot2), (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	void *slot3 = page_alloc(4);;
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("slot3 addr: 0x", (str + offset));
		offset += i2hex((uint32_t)(slot3), (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	page_free(slot2, 2);
	page_free(slot1, 2);
	
	void *slot4 = page_alloc(2);
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("slot4 addr: 0x", (str + offset));
		offset += i2hex((uint32_t)(slot4), (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	task_exit();
}

// temporary shell
void tmpshell(void) {
	term_putstring("> ");
    for(;;) {
		int symbol = 0;
		
		if (is_button_pressed(0)) {
			left_kbd();
		}
		else if (is_button_pressed(1)) {
			right_kbd();
		}
		else if (is_button_pressed(2)) {
			scroll_kbd();
		}
		else if (is_button_pressed(3)) {
			symbol = select_kbd();
		}
		
		// int offset = 0;
		// char str[32];
		// offset += s2ascii("sym: ", (str + offset));
		// offset += i2ascii(symbol, (str + offset));
		// offset += s2ascii("\n", (str + offset));
		// str[offset] = '\0';
		// semihost_write(str, offset);
		
		if (symbol > 0) { shell_input(symbol); }
		term_update();
    }
}

int main()
{
    HSE_Init();
	scheduler_init();
	
	// TIM2_Init();

    // // Enable GPIOC
    // BIT_SET(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

    // // Set GPIOC pin 13 mode to output
    // BIT_CLEAR(GPIOC->MODER, GPIO_MODER_MODER13_MASK);
    // BIT_SET(GPIOC->MODER, GPIO_MODER_MODER13_OUTPUT);
	
	page_init();
	board_init();
	
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("KERNEL LIVE!\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	task_create("IDLE", idle_task, 128);
	
	// task_create("SCRT", scrtest, 1024);
	// task_create("MEMT", memtest, 1024);
	
	task_create("SHLL", tmpshell, 2048);
	
	// task_create("A", taska, 256);
    // task_create("B", taskb, 256);
	// task_create("C", taskc, 256);

	SysTick_Init();
    scheduler_start();

    while (1)
    {
        // Do nothing.
    }
}