#include "shell/term.h"
#include "shell/font8x8_basic.h"
#include "drivers/ssd1306.h"
#include "utils/strutil.h"
#include "shell/vkbd.h"

static char term_buf[TERM_ROWS][TERM_COLS];
static uint8_t head_row = 0;
static uint8_t col = 0;
static uint8_t row = 0;

void term_clear() {
    memoryset((char*)term_buf, ' ', sizeof(term_buf));
    head_row = 0;
    col = 0;
    row = 0;
}

void term_update() {
	ssd1306_Fill(COLOR_BLACK);
	
	for (int y = 0; y < TERM_ROWS; y++) {
		uint8_t real_row = (head_row + y) % TERM_ROWS;
		for (int x = 0; x < TERM_COLS; x++) {
			char c = term_buf[real_row][x];
			if (c < 128) {
				ssd1306_DrawBitmap(x*8, y*8, (unsigned char*)font8x8_basic[(unsigned char)c], 8, 8, COLOR_WHITE);
			}
		}
	}
	
	draw_kbd();
	
	ssd1306_UpdateScreen();
}

static void term_newline() {
    col = 0;
    if (row < TERM_ROWS - 1) {
        row++;
    } else {
        head_row = (head_row + 1) % TERM_ROWS;
        uint8_t bottom_row = (head_row + TERM_ROWS - 1) % TERM_ROWS;
        memoryset(term_buf[bottom_row], ' ', TERM_COLS);
    }
}

void term_delchar() {
	if (col > 0) {
		uint8_t real_row = (head_row + row) % TERM_ROWS;
		col--;
		term_buf[real_row][col] = 0;
	}
}

void term_putchar(unsigned char c) {
	if (c == '\n') {
		term_newline();
		return;
	}
	else if (c == '\r') {
		col = 0;
		return;
	}
	
	if (col >= TERM_COLS) {
        term_newline();
    }

	uint8_t real_row = (head_row + row) % TERM_ROWS;
    term_buf[real_row][col++] = c;
}

void term_putstring(unsigned char *str) {
	while (*str) {
		term_putchar(*str++);
	}
}