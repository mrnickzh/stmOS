#include "memory/slot.h"
#include "platform/STM32F411xE.h"

// const log2
#define SLOT_ARENA_LOG2   16    // log2(64kb)
#define SLOT_MIN_LOG2     11    // log2(2kb)

// ARENA_LOG2 - MIN_LOG2 + 1 = 6 (orders 0..5)
#define SLOT_ORDER_COUNT  (SLOT_ARENA_LOG2 - SLOT_MIN_LOG2 + 1)

static uint8_t g_slot_arena[SLOT_ARENA_SIZE] __attribute__((aligned(SLOT_MIN_SIZE)));
static uint32_t g_slot_free[SLOT_ORDER_COUNT];

static inline uint32_t block_size(int order)
{
    return (uint32_t)SLOT_MIN_SIZE << order;
}

static inline int block_count(int order)
{
    return (int)(SLOT_ARENA_SIZE >> (SLOT_MIN_LOG2 + order));
}

static int uint32_to_order(uint32_t bytes)
{
    if (bytes == 0 || bytes > SLOT_MAX_ALLOC) return -1;

    int order = 0;
    uint32_t size  = SLOT_MIN_SIZE;
    while (size < bytes) {
        size <<= 1;
        order++;
    }
    return order;
}

static inline uint32_t buddy_offset(uint32_t off, int order)
{
    return off ^ order;
}

uint32_t order_bitmap(int order)
{
    return g_slot_free[order];
}

void slot_init(void)
{
    for (int i = 0; i < SLOT_ORDER_COUNT; i++)
        g_slot_free[i] = 0; // taken

    g_slot_free[SLOT_ORDER_COUNT - 1] = 1; // free
}

// allocation
// get the biggest block and split them into smaller ones that still fit

void *slot_alloc(uint32_t bytes)
{
    int want = uint32_to_order(bytes);
    if (want < 0) return NULL;

    uint32_t irq_mask = irq_save();

    // find smallest order with free block, (want..SLOT_ORDER_COUNT)
    int find = want;
    while (find < SLOT_ORDER_COUNT && g_slot_free[find] == 0) {
        find++;
	}
	
	// too fat
    if (find == SLOT_ORDER_COUNT) {
        irq_restore(irq_mask);
        return NULL;
    }

    int idx = __builtin_ctz(g_slot_free[find]);
    // g_slot_free[find] &= ~(1u << idx);
	BIT_CLEAR(g_slot_free[find], (1u << idx)); // sets g_slot_free[class] bit to 0 with block idx, marks it as busy

    uint32_t off = (uint32_t)idx * block_size(find);

    // split large block 
    while (find > want) {
        find--;
        uint32_t half = block_size(find);

        uint32_t buddy = buddy_offset(off, half);
        int buddy_idx = (int)(buddy / half);

        // g_slot_free[find] |= (1u << buddy_idx);
		BIT_SET(g_slot_free[find], (1u << buddy_idx)); // sets g_slot_free[class] bit to 1 with buddy block idx, marks it as free
        // offset stays, using the upper half
    }

    irq_restore(irq_mask);
    return g_slot_arena + off;
}

// deallocation 
// mark the block as "free" by setting 1
// then go through all blocks under this ancestor and check if they are also free
// if they are free - set ancestor bit to 1

void slot_free(void *ptr, uint32_t bytes)
{
    if (!ptr) return;

    int order = uint32_to_order(bytes);
    if (order < 0) return;

    uint32_t off = (uint8_t *)ptr - g_slot_arena;
    if (off >= SLOT_ARENA_SIZE) { return; }
    if (off & (block_size(order) - 1)) { return; }

    uint32_t irq_mask = irq_save();

    // double-free prevention
    int idx = (int)(off / block_size(order));
    if (g_slot_free[order] & (1u << idx)) {
        irq_restore(irq_mask);
        __asm volatile("bkpt 0"); // double free
        return;
    }
	
    // zero branches should be free to merge, obviously
    {
        uint32_t a_off = off;
        for (int a = order; a < SLOT_ORDER_COUNT; a++) {
            int a_idx = (int)(a_off / block_size(a));
            if (g_slot_free[a] & (1u << a_idx)) {
                irq_restore(irq_mask);
                __asm volatile("bkpt 0"); // double free (merge)
                return;
            }
            // a_off &= ~block_size(a);
			BIT_CLEAR(a_off, block_size(a)); // going up the tree
        }
    }

    // merge
    while (order < SLOT_ORDER_COUNT - 1) {
        uint32_t size  = block_size(order);
        uint32_t buddy = buddy_offset(off, size);
        if (buddy >= SLOT_ARENA_SIZE) break; // biggest block

        int buddy_idx = (int)(buddy / size);
        if (!(g_slot_free[order] & (1u << buddy_idx)))
            break; // buddy not free

        // merge
        // g_slot_free[order] &= ~(1u << buddy_idx);
        // off &= ~size;
		BIT_CLEAR(g_slot_free[order], (1u << buddy_idx)); // buddy taken
		BIT_CLEAR(off, size); // parent = lower
        order++;
    }

    // merged block marked as free
    idx = (int)(off / block_size(order));
    // g_slot_free[order] |= (1u << idx);
	BIT_SET(g_slot_free[order], (1u << idx));

    irq_restore(irq_mask);
}