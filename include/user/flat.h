#ifndef FLAT_H
#define FLAT_H

#include <stdint.h>

#define FLAT_MAGIC    0x464C4154   // 'FLAT'
#define FLAT_VERSION  1

typedef struct {
    uint32_t magic;         // 'FLAT'
    uint32_t version;       // 1
    uint32_t flags;         // 0 (reserved)
    uint32_t entry;         // смещение точки входа
    uint32_t text_size;     // .text + .rodata
    uint32_t data_size;     // .data
    uint32_t bss_size;      // .bss (не в файле)
    uint32_t stack_hint;    // 0 = default 1024
    uint32_t reloc_off;     // смещение таблицы релокаций
    uint32_t reloc_count;   // количество (N × uint32)
} flat_header_t;   // 40 байт

#endif