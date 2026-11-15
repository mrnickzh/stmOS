#include "debug/semihosting.h"

static int semihost_call(int reason, void *arg) {
    register int   r0 __asm__("r0") = reason;
    register void *r1 __asm__("r1") = arg;
    __asm__ volatile("bkpt 0xAB" : "+r"(r0) : "r"(r1) : "memory");
    return r0;
}

// write to gdb
int semihost_write(const char *buf, int len) {
    int args[3] = { 1, (int)buf, len };
    int not_written = semihost_call(SYS_WRITE, args);
    return len - not_written;
}