#ifndef NUM2ASCII_H
#define NUM2ASCII_H

void strncopy(char* dst, char* src, int i);
void strcopy(char* dst, char* src);
int s2ascii(const char *s, char *str);
int ui2ascii(unsigned int num, char* str);
int i2ascii(int num, char* str);
int i2hex(int num, char* str);
int i2bin(int num, char* str);

#endif // NUM2ASCII_H