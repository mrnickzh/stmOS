#include "debug/semihosting.h"
#include "config.h"

static int semihost_call(int reason, void *arg) {
#if CONFIG_SEMIHOSTING
    register int   r0 __asm__("r0") = reason;
    register void *r1 __asm__("r1") = arg;
    __asm__ volatile("bkpt 0xAB" : "+r"(r0) : "r"(r1) : "memory");
    return r0;
#endif
}

// write to gdb
int semihost_write(const char *buf, int len) {
#if CONFIG_SEMIHOSTING
    int args[3] = { 1, (int)buf, len };
    int not_written = semihost_call(SYS_WRITE, args);
    return len - not_written;
#endif
}