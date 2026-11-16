#ifndef STRUTIL_H
#define STRUTIL_H

void strncopy(char* dst, char* src, int i);
void strcopy(char* dst, char* src);
int strcomp(const char* first, const char* second);
void memoryset(char* dst, int val, int len);

#endif // STRUTIL_H