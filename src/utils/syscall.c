#include "utils/syscall.h"

int syscall(int num, int arg1, int arg2, int arg3) {
    register int r0 __asm__("r0") = num;
	register int r1 __asm__("r1") = arg1;
	register int r2 __asm__("r2") = arg2;
	register int r3 __asm__("r3") = arg3;
	
    __asm__ volatile("svc #1" : "+r"(r0) : "r"(r1), "r"(r2), "r"(r3) : "memory");
	
	return r0;
}