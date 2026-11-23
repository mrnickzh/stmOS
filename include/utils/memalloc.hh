#ifndef MEMALLOC_H
#define MEMALLOC_H

#define MEM_SIZE 2048

typedef struct mem_block {
    struct mem_block *next;
    uint32_t size;
    uint8_t is_free;
} mem_block;

void *memalloc(uint32_t size);
void memfree(void *ptr);

#endif // MEMALLOC_H