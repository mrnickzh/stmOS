#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_YIELD   0
#define SYS_WRITE   1
#define SYS_EXIT    2
#define SYS_READ    3
#define SYS_MALLOC  4
#define SYS_FREE    5

int syscall(int num, int arg1, int arg2, int arg3) {
    register int r0 __asm__("r0") = num;
	register int r1 __asm__("r1") = arg1;
	register int r2 __asm__("r2") = arg2;
	register int r3 __asm__("r3") = arg3;
	
    __asm__ volatile("svc #1" : "+r"(r0) : "r"(r1), "r"(r2), "r"(r3) : "memory");
	
	return r0;
};

#endif // SYSCALL_H
