#include "memory/page.h"

#include "platform/Cortex-M4.h"

uint8_t g_page_pool[NUM_PAGES * PAGE_SIZE];
uint32_t g_page_bitmap[NUM_BITMAPS];

void page_init() {
    for (int i = 0; i < NUM_BITMAPS; i++) {
        g_page_bitmap[i] = 0xFFFFFFFFu;
    }
}

void* page_alloc(int pages) {
    if (pages <= 0) { return NULL; }
	
	uint32_t irq_mask = irq_save();
    
    int len = 0;
    int start_page = 0;
    for (int i = 0; i < NUM_BITMAPS; i++) {
        if (g_page_bitmap[i] == 0) { len = 0; continue; }
        
        for (int b = 0; b < (sizeof(uint32_t) * 8); b++) {
            int current_page = (uint32_t)i * (sizeof(uint32_t) * 8) + b;
            if (current_page >= NUM_PAGES) { break; }
            
            uint32_t status = g_page_bitmap[i] & (1u << b);
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
    
    if (len < pages) { irq_restore(irq_mask); return NULL; }
    for (int i = 0; i < len; i++) {
        uint32_t page = start_page + i;
        g_page_bitmap[page / 32] &= ~(1u << (page % 32));
    }
    
	irq_restore(irq_mask);
    return (void*)(g_page_pool + (start_page * PAGE_SIZE));
}

void page_free(void* addr, int pages) {
    uint32_t page_start_index = ((uint8_t*)addr - (uint8_t*)g_page_pool) / PAGE_SIZE;
	
	uint32_t irq_mask = irq_save();
    
    for (int i = 0; i < pages; i++) {
        uint32_t p = page_start_index + i;
        g_page_bitmap[p / 32] |= (1u << (p % 32));
    }
	
	irq_restore(irq_mask);
}