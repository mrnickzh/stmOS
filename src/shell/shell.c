#include "shell/shell.h"

#include "utils/strutil.h"
#include "utils/num2ascii.h"
#include "drivers/button.h"
#include "shell/term.h"
#include "shell/vkbd.h"
#include "scheduler/scheduler.h"

void cmd_ps(int argc, char **argv) {
    (void)argc; (void)argv;
    task_ctx_t *t = g_ready_list;
    if (!t) {
        term_putstring("no tasks :(\n");
        return;
    }
    
    char buf[64];
	int offset = 0;
    do {
        offset += s2ascii(t->name, (buf + offset));
        offset += s2ascii(" | ", (buf + offset));
        offset += ui2ascii(t->state, (buf + offset));
        offset += s2ascii(" | ", (buf + offset));
        offset += ui2ascii(t->stack_words * 4, (buf + offset));
        offset += s2ascii("b\n", (buf + offset));
        buf[offset] = '\0';
        
        term_putstring(buf);
        t = t->next;
		
		offset = 0;
    } while (t != g_ready_list);
}

struct shell_util {
	char* name;
	void (*func)();
};

struct shell_util shell_utils[] = {"ps", cmd_ps};
uint32_t utils_len = 1;

char shell_buffer[64];
uint32_t buffer_len = 0;

void shell_process() {
	shell_buffer[buffer_len] = '\0';
	for (int i = 0; i < utils_len; i++) {
		if (!strcomp(shell_buffer, shell_utils[i].name)) {
			shell_utils[i].func();
			return;
		}
	}
	term_putstring("wrong cmd\n");
}

void shell_input(char input) {
	if (input == '\n') {
		term_putchar('\n');
		if (buffer_len > 0) { shell_process(); }
		term_putstring("> ");
		buffer_len = 0;
	}
	else if (input == '\b') {
		if (buffer_len > 0) {
			term_delchar();
			buffer_len--;
		}
	}
	else {
		if (buffer_len < 64) {
			shell_buffer[buffer_len] = input;
			term_putchar(input);
			buffer_len++;
		}
	}
}