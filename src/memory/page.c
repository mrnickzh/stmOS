#include "memory/page.h"

#include "platform/Cortex-M4.h"

static uint8_t page_memory[NUM_PAGES * PAGE_SIZE];
static uint32_t page_bitmap[NUM_BITMAPS];

void page_init() {
    for (int i = 0; i < NUM_BITMAPS; i++) {
        page_bitmap[i] = 0xFFFFFFFFu;
    }
}

void* page_alloc(int pages) {
    if (pages <= 0) { return NULL; }
    
    int len = 0;
    int start_page = 0;
    for (int i = 0; i < NUM_BITMAPS; i++) {
        if (page_bitmap[i] == 0) { len = 0; continue; }
        
        for (int b = 0; b < (sizeof(uint32_t) * 8); b++) {
            int current_page = (uint32_t)i * (sizeof(uint32_t) * 8) + b;
            if (current_page >= NUM_PAGES) { break; }
            
            uint32_t status = page_bitmap[i] & (1u << b);
            if (status == 0) {
                len = 0;
            }
            else {
                if (len == 0) { start_page = current_page; }
                len++;
                if (len == pages) { break; }
            }
        }
        
        if (len == pages) { break; }
    }
    
    if (len < pages) { return NULL; }
    for (int i = 0; i < len; i++) {
        uint32_t page = start_page + i;
        page_bitmap[page / 32] &= ~(1u << (page % 32));
    }
    
    return (void*)(page_memory + (start_page * PAGE_SIZE));
}

void page_free(void* addr, int pages) {
    uint32_t page_start_index = ((uint8_t*)addr - (uint8_t*)page_memory) / PAGE_SIZE;
    
    for (int i = 0; i < pages; i++) {
        uint32_t p = page_start_index + i;
        page_bitmap[p / 32] |= (1u << (p % 32));
    }
}