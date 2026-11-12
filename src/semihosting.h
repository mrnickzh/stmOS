#ifndef SEMIHOSTING_H
#define SEMIHOSTING_H

#define SYS_WRITE 0x05

// write to gdb
int semihost_write(const char *buf, int len);

#endif // SEMIHOSTING_H