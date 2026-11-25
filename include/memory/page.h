#ifndef PAGE_H
#define PAGE_H

#include "platform/Cortex-M4.h"

#define NUM_PAGES 64
#define PAGE_SIZE 1024

#define NUM_BITMAPS (NUM_PAGES / 32)

__attribute__((section(".page_pool"), aligned(8)))
extern uint8_t g_page_pool[NUM_PAGES * PAGE_SIZE];

__attribute__((section(".bss")))
extern uint32_t g_page_bitmap[NUM_BITMAPS];

void page_init();
void *page_alloc(int pages);
void page_free(void* addr, int pages);

#endif // PAGE_H
