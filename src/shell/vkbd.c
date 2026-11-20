#include "shell/vkbd.h"
#include "shell/font8x8_basic.h"
#include "drivers/ssd1306.h"

static const char kbd_icons[3][8] = {
    { 0x08, 0x1C, 0x3E, 0x7F, 0x1C, 0x1C, 0x00, 0x3E }, // caps lock
    { 0x00, 0x7C, 0x56, 0x6B, 0x56, 0x7C, 0x00, 0x00 }, // backspace
    { 0x00, 0x60, 0x60, 0x64, 0x66, 0x7F, 0x06, 0x04 } // enter
};

static const char kbd_rows[KB_ROWS][KB_COLS] = {
    {'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '='},
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', ',', '.'},
    {'z', 'x', 'c', 'v', 'b', 'n', 'm', ' ', '/', '<', '>', '{', '}'}
};


static const char kbd_rows_shift[KB_ROWS][KB_COLS] = {
    {'~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+'},
    {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '|'},
    {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '<', '>'},
    {'Z', 'X', 'C', 'V', 'B', 'N', 'M', ' ', '?', ',', '.', '[', ']'}
};

uint8_t row;
uint8_t col;

uint8_t caps = 0;

void draw_kbd() {
	// regular keys
	for (int x = 0; x < KB_COLS; x++) {
		char c;
		if (caps) {
			c = kbd_rows_shift[row][x];
		}
		else {
			c = kbd_rows[row][x];
		}

		SSD1306_COLOR clr = (x == col ? COLOR_BLACK : COLOR_WHITE);
		ssd1306_DrawBitmap(x*8, 7*8, (unsigned char*)font8x8_basic[(unsigned char)c], 8, 8, clr);
	}
	
	// special keys
	for (int x = 0; x < 3; x++) {
		SSD1306_COLOR clr = (x + KB_COLS == col ? COLOR_BLACK : COLOR_WHITE);
		ssd1306_DrawBitmap((x + KB_COLS)*8, 7*8, (unsigned char*)kbd_icons[(unsigned char)x], 8, 8, clr);
	}
}

void right_kbd() {
	col = (col + 1) % (KB_COLS + SPEC_COLS);
}

void left_kbd() {
	col = (col + (KB_COLS + SPEC_COLS) - 1) % (KB_COLS + SPEC_COLS);
}

void scroll_kbd() {
	row = (row + 1) % (KB_ROWS);
}

int select_kbd() {
	// regular keys
	if (col < 13) {
		char c;
		if (caps) {
			c = kbd_rows_shift[row][col];
		}
		else {
			c = kbd_rows[row][col];
		}
		return c;
	}
	// special keys
	else {
		uint8_t spec = col - KB_COLS;
		switch (spec) {
			case 0:
				caps = !caps;
				return -1;
			case 1:
				return '\b';
			case 2:
				return '\n';
		}
	}
}