#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_YIELD   0
#define SYS_WRITE   1
#define SYS_EXIT    2
#define SYS_READ    3
#define SYS_MALLOC  4
#define SYS_FREE    5

int syscall(int num, int arg1, int arg2, int arg3);

#endif // SYSCALL_H
