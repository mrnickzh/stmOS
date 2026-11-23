#include "utils/memalloc.h"

#include <stdint.h>

#define MEM_SIZE 2048

typedef struct mem_block {
    struct mem_block *next;
    uint32_t size;
    uint8_t is_free;
} mem_block;

static uint8_t mem_pool[MEM_SIZE];
static uint32_t mem_break = 0;

void *memalloc(uint32_t size) {
    mem_block *cur_block = (mem_block*)mem_pool;
    mem_block *prev_block = NULL;
    if (mem_break == 0) { goto alc; }
    
    while (cur_block) {
        if ((cur_block->size == size) && cur_block->is_free) {
            cur_block->is_free = 0;
            return (void*)(cur_block + 1);
        }
        else {
            prev_block = cur_block;
            cur_block = cur_block->next;
        }
    };
    
alc:
    if (mem_break + size + sizeof(mem_block) > MEM_SIZE) { return NULL; }
    
    mem_block *new_block = (mem_block*)(mem_pool + mem_break);
    new_block->next = NULL;
    new_block->size = size;
    new_block->is_free = 0;
    
    if (prev_block) {
        prev_block->next = new_block;
    }
    
    uint32_t old_break = mem_break;
    mem_break = mem_break + size + sizeof(mem_block);
    
    return (void*)(mem_pool + old_break + sizeof(mem_block));
}

void memfree(void *ptr) {
    if (!ptr) return;
    
    mem_block *free_block = ((mem_block*)(ptr) - 1);
    free_block->is_free = 1;
}

// int main()
// {
    // int *arr1 = (int*)memalloc(sizeof(int) * 4);
    
    // for (int i = 1; i <= 4; i++) {
        // arr1[i - 1] = i;
    // }
    
    // printf("1 = %p\n", arr1);
    
    // int *arr2 = (int*)memalloc(sizeof(int) * 4);
    
    // for (int i = 11; i <= 14; i++) {
        // arr2[i - 11] = i;
    // }
    
    // printf("2 = %p\n", arr2);
    
    // memfree(arr1);
    
    // int *arr3 = (int*)memalloc(sizeof(int) * 4);
    
    // for (int i = 21; i <= 24; i++) {
        // arr3[i - 21] = i;
    // }
    
    // printf("3 = %p\n", arr3);
    
    // for (int i = 0; i < 4; i++) {
        // printf("3-%d = %d\n", i, arr3[i]);
    // }
    
    // for (int i = 0; i < 4; i++) {
        // printf("2-%d = %d\n", i, arr2[i]);
    // }

    // return 0;
// }
