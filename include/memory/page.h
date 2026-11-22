#ifndef PAGE_H
#define PAGE_H

#include "platform/Cortex-M4.h"

#define NUM_PAGES 64
#define PAGE_SIZE 1024

#define NUM_BITMAPS (NUM_PAGES / 32)

void page_init();
void *page_alloc(int pages);
void page_free(void* addr, int pages);

#endif // PAGE_H
