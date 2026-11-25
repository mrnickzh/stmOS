#include "syscall.h"

static int my_strlen(const char *s) {
    int n = 0;
    while (s[n]) n++;
    return n;
}

int main(void) {
    unsigned char *msg = "Hello from user!\n";
    syscall(SYS_WRITE, (int)msg, my_strlen(msg), 0);
    return 0;
}