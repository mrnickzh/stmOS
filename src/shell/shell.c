#include "shell/shell.h"

#include "utils/strutil.h"
#include "utils/num2ascii.h"
#include "drivers/button.h"
#include "shell/term.h"
#include "shell/vkbd.h"
#include "scheduler/scheduler.h"

#include "user/loader.h"
#include "user_hello.h"

int cmd_ps(int argc, char **argv) {
    task_ctx_t *t = g_ready_list;
    if (!t) {
        term_putstring("no tasks :(\n");
        return 0;
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
	
	return 0;
}

int cmd_echo(int argc, char **argv) {
	if (argc < 1) { return 0; }
    
	char buf[64];
	int offset = 0;
	for (int i = 1; i < argc; i++) {
		offset += s2ascii(argv[i], (buf + offset));
		buf[offset++] = ' ';
	}
	buf[offset] = '\0';
	term_putstring(buf);
	
	return 0;
}

int cmd_run(int argc, char **argv) {
    (void)argc; (void)argv;
    term_putstring("loading...\n");
    int rc = exec_flat(g_user_hello_h_data, g_user_hello_h_size, "hello");
	
	if (rc < 0) { return 1; }
	
	while (task_by_pid(rc)->state != TASK_FREE) {
        task_sleep(50);
    }
	
	char buf[64];
    int off = 0;
    off += s2ascii("rc=", buf);
    off += i2ascii(rc, buf + off);
    off += s2ascii("\n", buf + off);
    buf[off] = 0;
    term_putstring(buf);
	
	return 0;
}

struct shell_util {
	char* name;
	int (*func)(int, char**);
};

struct shell_util shell_utils[] = {{"ps", cmd_ps}, {"echo", cmd_echo}, {"run", cmd_run}};
uint32_t utils_len = 3;

char shell_buffer[64];
uint32_t buffer_len = 0;

uint32_t shell_args(char* cmd, char** argv, int max_args) {
	uint32_t argc = 0;
	char *p = cmd;
	
	while (p) {
        while (*p == ' ') { p++; }
		if (!*p) { break; }
		if (argc >= max_args) { break; }
        argv[argc++] = p;
        while (*p && (*p != ' ')) { p++; }
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
	}
	
	return argc;
}

void shell_process() {
	char *argv[MAX_ARGS];
	
	shell_buffer[buffer_len] = '\0';
	int argc = shell_args(shell_buffer, argv, MAX_ARGS);
	
	for (int i = 0; i < utils_len; i++) {
		if (!strcomp(argv[0], shell_utils[i].name)) {
			shell_utils[i].func(argc, argv);
			return;
		}
	}
	
	term_putstring("unknown cmd");
}

void shell_input(char input) {
	if (input == '\n') {
		term_putchar('\n');
		if (buffer_len > 0) { shell_process(); }
		term_putstring("\n> ");
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