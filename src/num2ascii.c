#include "num2ascii.h"

static char hex_char[] = "0123456789ABCDEF";

void strncopy(char* dst, char* src, int i) {
	if (i <= 0) { return; }
    while (i--) { *dst++ = *src++; };
}

void strcopy(char* dst, char* src) {
    while (*dst++ = *src++) {};
}

int s2ascii(const char *s, char *str) {
    int i = 0;
    if (!s) s = "(null)";
    while (*s) { *str++ = *s++; i++; }
    return i;
}

int ui2ascii(unsigned int num, char* str) {
    int i = 0;
    int i2 = 0;
    
    char tmp[10];
    
    if (num == 0) { *str++ = '0'; return 1; }
    while (num) {
        tmp[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    i2 = i;
    while (i--) { *str++ = tmp[i]; };
    return i2;
}

int i2ascii(int num, char* str) {
    if (num < 0) {
        *str++ = '-';
        unsigned int uint = (unsigned)num;
        uint = -uint;
        return ui2ascii(uint, str) + 1;
    }
    return ui2ascii((unsigned)num, str);
}

int i2hex(int num, char* str) {
    int i = 0;
    int i2 = 0;
    
    char tmp[8];
    
    int shift = 0;
    if (num == 0) { *str++ = '0'; return 1; }
    while (shift <= 28) {
        int idx = (num >> shift) & 0xF;
        tmp[i++] = hex_char[idx];
        shift += 4;
    }
    
    i2 = i;
    while (i--) { *str++ = tmp[i]; };
    return i2;
}

int i2bin(int num, char* str) {
    int i = 0;
    int i2 = 0;
    
    char tmp[32];
    
    int shift = 0;
    if (num == 0) { *str++ = '0'; return 1; }
    while (shift < 32) {
        int bit = (num >> shift) & 1;
        tmp[i++] = '0' + bit;
        shift++;
    }
    
    i2 = i;
    while (i--) { *str++ = tmp[i]; };
    return i2;
}

int f2ascii(float num, char* str, int prec) {
    int i = 0;
    
    float rnd = 0.5;
    for (int d = 0; d < prec; d++) {
        rnd /= 10.0;
    }
    num += rnd;

    int ipart = (int)num;
    float fpart = num - (float)ipart;

    if (ipart == 0) {
        str[i++] = '0';
    }
    else {
        i += i2ascii(ipart, str);
    }

    if (prec > 0) {
        str[i++] = '.';
        for (int j = 0; j < prec; j++) {
            fpart *= 10.0f;
            int digit = (int)fpart;
            str[i++] = digit + '0';
            fpart -= digit;
        }
    }
    
    return i;
}

// void print_int(char *fmt, int num) {
    // char out[128];
    
    // int ii = 0;
    // int oi = 0;

    // int mark = 0;
    
    // for (;;) {
        // if (!fmt[ii]) {
            // break;
        // }
        // if (fmt[ii] == '%') {
            // mark = 1;
            // ii++;
            // continue;
        // }
        // else if (mark) {
            // if (fmt[ii] == 'd') {
                // ii++;
                // char str[11];
                // int digits = i2ascii(num, (out + oi));
                // oi+=digits;
                // mark = 0;
            // }
        // }
        
        // out[oi] = fmt[ii];
        // ii++; oi++;
    // }
    
    // out[oi] = '\0';
// }

// EXAMPLE
// int offset = 0;
// char str[64];
// offset += s2ascii("lol ", (str + offset));
// offset += f2ascii(13.3777, (str + offset), 2);
// offset += s2ascii(" kek 0x", (str + offset));
// offset += i2hex(0xDEADBEEF, (str + offset));
// offset += s2ascii("\n", (str + offset));
// str[offset] = '\0';