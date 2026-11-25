#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#include <stdint.h>
#include "utils/syscall.h"

void syscall_handler(uint32_t *frame);

#endif // SYSCALL_HANDLER_H