#ifndef SLOT_H
#define SLOT_H

#include "platform/Cortex-M4.h"

// so, this is a buddy heap allocator
// the concept works as following:
// we have a contigious arena, the minimal allocation size is 2kb and goes up to 32kb
// each block size class has a bitmap to determine what blocks are free and what arent
// the "buddy" part is needed to combine and split neighbouring blocks into different size blocks, while maintaining the contigious arena
// the blocks essentially form a tree, where a bigger block splits into 2 smaller blocks, like this
/*
	   [0, 32k]
        /    \ 
   [0, 16k] [16k, 32k]
*/
// the XOR magic comes from the fact that flipping log2(size) bit allows to convert address of one half into another like this
// [0x20000000](lower) <=> [0x20001000](upper)
// bit 1 means free, bit 0 means taken

// DeepSeek gave me this, i wouldnt come up with something like this myself, lol

#define SLOT_MIN_SIZE 2048        // 2kb
#define SLOT_ARENA_SIZE (64 * 1024)  // 64kb
#define SLOT_MAX_ALLOC (SLOT_ARENA_SIZE / 2)

void slot_init(void);
void *slot_alloc(uint32_t bytes);
void slot_free(void *ptr, uint32_t bytes);

#endif // SLOT_H
