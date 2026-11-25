#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

int exec_flat(const uint8_t *data, uint32_t size, const char *name);

#endif // LOADER_H