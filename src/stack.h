#ifndef STACK_H
#define STACK_H

#include <stdint.h>

// STACK ENTRY STRUCTUE
// The g_free[CLASS] is a singly-linked list
// The stack block itself is an intrusive list entry
// The index [0] of a stack allocation holds:
// live stack alloc = stack canary
// head of a free list = NULL
// not head of a free list = next free block

#define STACK_NUM_CLASSES   5
#define STACK_POOL_WORDS    8192     // 32kb

#define STACK_PAINT 0xA5A5A5A5
#define STACK_GUARD 0xDEADBEEF // stack canary

extern uint32_t g_stack_pool[STACK_POOL_WORDS];
extern uint32_t *g_stack_brk;
extern uint32_t *g_free[STACK_NUM_CLASSES];

extern const uint32_t g_class_words[STACK_NUM_CLASSES];

int stack_class_index(uint32_t bytes);
void stack_pool_init(void);
uint32_t *stack_alloc(uint32_t bytes);
void stack_free(uint32_t *base, uint32_t bytes);
uint32_t stack_peak_usage(uint32_t *base, uint32_t words);

#endif // STACK_H