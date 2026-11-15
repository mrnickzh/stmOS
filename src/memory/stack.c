#include "memory/stack.h"

#include "platform/Cortex-M4.h"

const uint32_t g_class_words[STACK_NUM_CLASSES] = {
    128,    // 512b
    256,    // 1kb
    512,    // 2kb
    1024,   // 4kb
};

uint32_t  g_stack_pool[STACK_POOL_WORDS] __attribute__((aligned(8)));
uint32_t *g_stack_brk = 0;
uint32_t *g_stack_free[STACK_NUM_CLASSES] = { 0 };

int stack_class_index(uint32_t bytes) {
    uint32_t words = (bytes + 7u) / 8u * 2u;
    for (int i = 0; i < STACK_NUM_CLASSES; i++)
        if (words <= g_class_words[i]) return i;
    return -1;
}

void stack_pool_init(void) {
    g_stack_brk = g_stack_pool;
    for (int i = 0; i < STACK_NUM_CLASSES; i++) g_stack_free[i] = 0;
}

uint32_t *stack_alloc(uint32_t bytes) {
    int cls = stack_class_index(bytes);
    if (cls < 0) return 0;
	
	uint32_t words = g_class_words[cls];
    uint32_t *base;
	uint32_t irq_mask = irq_save();
	
	if (g_stack_free[cls]) {
        base = g_stack_free[cls];
        g_stack_free[cls] = *(uint32_t **)base; 
    } else {
        if (g_stack_brk + words > g_stack_pool + STACK_POOL_WORDS) {
            irq_restore(irq_mask);
            return 0;
        }
        base = g_stack_brk;
        g_stack_brk += words;
    }
	
	irq_restore(irq_mask);
	
	for (uint32_t i = 0; i < words; i++) base[i] = STACK_PAINT;
    base[0] = STACK_GUARD;
	
	return base;
}

void stack_free(uint32_t *base, uint32_t bytes) {
    if (!base) return;
    int cls = stack_class_index(bytes);
    if (cls < 0) return;
	
	// canary check
	if (base[0] != STACK_GUARD) {
        __asm volatile("bkpt 0" ::: "memory");
        return;
    }
	
	uint32_t irq_mask = irq_save();
    *(uint32_t **)base = g_stack_free[cls];   // free list
    g_stack_free[cls] = base;
    irq_restore(irq_mask);
}

uint32_t stack_peak_usage(uint32_t *base, uint32_t words) {
    uint32_t i = 1;   // skip canary
    while (i < words && base[i] == STACK_PAINT) i++;
    return (words - i) * 4;
}