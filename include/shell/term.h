#ifndef TERM_H
#define TERM_H

#define TERM_COLS 16
#define TERM_ROWS 7

void term_clear();
void term_update();
void term_delchar();
void term_putchar(unsigned char c);
void term_putstring(unsigned char *str);
void term_n_putstring(unsigned char *str, int len);

#endif // TERM_H