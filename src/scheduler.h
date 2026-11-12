#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

#define TASK_FREE    0
#define TASK_READY   1
#define TASK_RUNNING 2
#define TASK_BLOCKED 3

#define MAX_TASKS      32

typedef struct task_ctx_t {
    uint32_t *sp;
	struct task_ctx_t *next;
    uint32_t state;
    const char *name;
    uint32_t *stack_base;
    uint32_t stack_words;
	uint32_t wake_at;
} task_ctx_t;

extern task_ctx_t  g_tasks_pool[MAX_TASKS];
extern uint32_t    g_tasks_bitmap;

extern task_ctx_t *volatile g_ready_list;
extern task_ctx_t *volatile g_blocked_list;

extern task_ctx_t *volatile g_current_task;

int task_create(const char *name, void (*entry)(void), uint32_t stack_bytes);
void task_yield(void);
void task_exit(void);
void task_sleep(uint32_t ms);

void scheduler_init(void);
void scheduler_start(void);
task_ctx_t *schedule(void);

void idle_task(void);

// READY LIST
void ready_push(task_ctx_t *task);
void ready_remove(task_ctx_t *task);

// BLOCKED LIST
void blocked_push(task_ctx_t *task);
void blocked_remove(task_ctx_t *task);

#endif // SCHEDULER_H