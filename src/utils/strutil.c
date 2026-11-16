#include "utils/strutil.h"

void strncopy(char* dst, char* src, int i) 
{
	if (i <= 0) { return; }
    while (i--) { *dst++ = *src++; };
}

void strcopy(char* dst, char* src) 
{
    while (*dst++ = *src++) {};
}

int strcomp(const char* first, const char* second) 
{
	while ((*first++) && (second++)) {
	    if (*first != *second) { return 1; }
	}
	return 0;
}

void memoryset(char* dst, int val, int len) 
{
	if (len <= 0) { return; }

	while(len > 0)
	{
		*dst = val;
		dst++;
		len--;
	}
}