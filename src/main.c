#include "platform/STM32F411xE.h"
#include "handlers/int_handlers.h"
#include "scheduler/scheduler.h"
#include "utils/num2ascii.h"
#include "debug/semihosting.h"
#include "memory/slot.h"
#include "config.h"
#include "platform/board.h"
#include "drivers/button.h"
#include "drivers/ssd1306.h"
#include "shell/term.h"
#include "shell/vkbd.h"

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
	mutex_lock(&print_lock);
	
	void *ptrs[32] = {0};
	for (int round = 0; round < 10; round++) {
		// alloc
		for (int i = 0; i < 8; i++) {
			uint32_t sz = 2048 << (round & 3);
			ptrs[i] = slot_alloc(sz);
			
			int offset = 0;
			char str[128];
			offset += s2ascii("alloc ", (str + offset));
			offset += i2ascii(sz, (str + offset));
			offset += s2ascii(" b addr ", (str + offset));
			offset += i2hex((uint32_t)ptrs[i], (str + offset));
			offset += s2ascii("\n", (str + offset));
			str[offset] = '\0';
			semihost_write(str, offset);
		}
		
		for (int o = 0; o < 5; o++) {
			int offset = 0;
			char str[128];
			offset += s2ascii("order [", (str + offset));
			offset += i2ascii(o, (str + offset));
			offset += s2ascii("] busy bitmap: 0b", (str + offset));
			offset += i2bin(order_bitmap(o), (str + offset));
			offset += s2ascii("\n", (str + offset));
			str[offset] = '\0';
			semihost_write(str, offset);
		}
		
		// free
		for (int i = 7; i >= 0; i--) {
			uint32_t sz = 2048 << (round & 3);
			if (ptrs[i]) { slot_free(ptrs[i], sz); ptrs[i] = 0; }
			
			int offset = 0;
			char str[128];
			offset += s2ascii("free ", (str + offset));
			offset += i2ascii(sz, (str + offset));
			offset += s2ascii(" b addr ", (str + offset));
			offset += i2hex((uint32_t)ptrs[i], (str + offset));
			offset += s2ascii("\n", (str + offset));
			str[offset] = '\0';
			semihost_write(str, offset);
		}
		
		for (int o = 0; o < 5; o++) {
			int offset = 0;
			char str[128];
			offset += s2ascii("order [", (str + offset));
			offset += i2ascii(o, (str + offset));
			offset += s2ascii("] clean bitmap: 0b", (str + offset));
			offset += i2bin(order_bitmap(o), (str + offset));
			offset += s2ascii("\n", (str + offset));
			str[offset] = '\0';
			semihost_write(str, offset);
		}
	}
	
	void *slot1 = slot_alloc(2048);
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("slot1 addr: 0x", (str + offset));
		offset += i2hex((uint32_t)(slot1), (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	void *slot2 = slot_alloc(2048);
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("slot2 addr: 0x", (str + offset));
		offset += i2hex((uint32_t)(slot2), (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	void *slot3 = slot_alloc(4096);
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("slot3 addr: 0x", (str + offset));
		offset += i2hex((uint32_t)(slot3), (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	slot_free(slot2, 2048);
	slot_free(slot1, 2048);
	
	void *slot4 = slot_alloc(2048);
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("slot4 addr: 0x", (str + offset));
		offset += i2hex((uint32_t)(slot4), (str + offset));
		offset += s2ascii("\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	mutex_unlock(&print_lock);
	
	task_exit();
}

void scrtest(void) {
	for(;;) {
		// ─── ТЕСТ SSD1306 ───
		// 1. Проверка: экран белый на 1 сек
		ssd1306_Fill(COLOR_WHITE);
		ssd1306_UpdateScreen();
		task_sleep(250);

		// 2. Проверка: экран чёрный
		ssd1306_Fill(COLOR_BLACK);
		ssd1306_UpdateScreen();
		task_sleep(250);

		// 3. Проверка: пиксели в углах
		ssd1306_DrawPixel(0,   0,  COLOR_WHITE);   // левый верх
		ssd1306_DrawPixel(127, 0,  COLOR_WHITE);   // правый верх
		ssd1306_DrawPixel(0,   63, COLOR_WHITE);   // левый низ
		ssd1306_DrawPixel(127, 63, COLOR_WHITE);   // правый низ
		ssd1306_DrawPixel(64,  32, COLOR_WHITE);   // центр
		ssd1306_UpdateScreen();
		task_sleep(250);

		// 4. Проверка: горизонтальная и вертикальная линии
		for (uint8_t x = 0; x < 128; x++) ssd1306_DrawPixel(x, 10, COLOR_WHITE);
		for (uint8_t y = 0; y < 64;  y++) ssd1306_DrawPixel(64, y, COLOR_WHITE);
		ssd1306_UpdateScreen();
		task_sleep(250);

		// 2. Проверка: экран чёрный
		ssd1306_Fill(COLOR_BLACK);
		ssd1306_UpdateScreen();
		
		task_exit();
	}
}

// temporary shell
void tmpshell(void) {
    for(;;) {
		int symbol = 0;
		
		if (is_button_pressed(0)) {
			right_kbd();
		}
		else if (is_button_pressed(1)) {
			left_kbd();
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
		
		if (symbol > 0) { term_putchar((unsigned char)symbol); }
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
	
	slot_init();
	board_init();
	
	{
		int offset = 0;
		char str[128];
		offset += s2ascii("KERNEL LIVE!\n", (str + offset));
		str[offset] = '\0';
		semihost_write(str, offset);
	}
	
	task_create("IDLE", idle_task, 128);
	
	//task_create("SCRT", scrtest, 1024);
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