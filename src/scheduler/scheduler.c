#include "scheduler/scheduler.h"

#include "handlers/int_handlers.h"
#include "utils/syscall.h"
#include "memory/stack.h"
#include "platform/Cortex-M4.h"
#include "platform/STM32F411xE.h"
#include "memory/page.h"

task_ctx_t g_tasks_pool[MAX_TASKS] __attribute__((aligned(8)));
uint32_t g_tasks_bitmap = 0;

task_ctx_t *volatile g_ready_list = 0;
task_ctx_t *volatile g_blocked_list = 0;
task_ctx_t *volatile g_current_task = 0;

void idle_task(void) {
    while (1) {
        __asm volatile("wfi");
    }
}

// MUTEX

void mutex_push(mutex_t *mutex, task_ctx_t *task) 
{
	task->next = NULL;
	if (mutex->waiters_tail) {
		mutex->waiters_tail->next = task;
	} else {
		mutex->waiters_head = task;
	}
	mutex->waiters_tail = task;
}

task_ctx_t *mutex_pop(mutex_t *mutex) 
{
	task_ctx_t *task = mutex->waiters_head;
	if (!task) return NULL;

	mutex->waiters_head = task->next;
	if (!mutex->waiters_head) {
		mutex->waiters_tail = NULL;
	}
	task->next = NULL;
	return task;
}

void mutex_lock(mutex_t *mutex) 
{
	if (mutex->owner == g_current_task) {
		// double lock, panic
		__asm volatile("bkpt 0");
		return;
	}
	
	for (;;) {
		uint32_t irq_mask = irq_save();
		
		if (mutex->owner == g_current_task) {
            irq_restore(irq_mask);
            return;
        }
		
		if (!mutex->owner) {
			mutex->owner = g_current_task;
			irq_restore(irq_mask);
			return;
		}
		
		if (mutex->owner->state == TASK_FREE) {
			mutex->owner = g_current_task;
			irq_restore(irq_mask);
			return;
		}
		
		g_current_task->state = TASK_BLOCKED;
		ready_remove(g_current_task);
		mutex_push(mutex, g_current_task);
		irq_restore(irq_mask);
		task_yield();
	}
}

void mutex_unlock(mutex_t *mutex) 
{
	uint32_t irq_mask = irq_save();

	if (mutex->owner != g_current_task) {
		irq_restore(irq_mask);
		// mutex unlock by the wrong task, panic
		__asm volatile("bkpt 0");
		return;
	}

	task_ctx_t *next = mutex_pop(mutex);
	if (next) {
		mutex->owner = next;
		next->state = TASK_READY;
		ready_push(next);
	} else {
		mutex->owner = NULL;
	}

	irq_restore(irq_mask);
}

// TASK POOL
static task_ctx_t *task_alloc(int *pid)
{
    uint32_t irq_mask = irq_save();
    uint32_t free_bits = ~g_tasks_bitmap;
    if (!free_bits) { irq_restore(irq_mask); return 0; }
    int i = __builtin_ctz(free_bits);
    // g_tasks_bitmap |= (1u << i);
	BIT_SET(g_tasks_bitmap, (1u << i));
    irq_restore(irq_mask);
	*pid = i;
    return &g_tasks_pool[i];
}

static void task_free(task_ctx_t *task)
{
    int i = task - g_tasks_pool;
    uint32_t irq_mask = irq_save();
    // g_tasks_bitmap &= ~(1u << i);
	BIT_CLEAR(g_tasks_bitmap, (1u << i));
    irq_restore(irq_mask);
}

// READY LIST
void ready_push(task_ctx_t *task)
{
    if (!g_ready_list) {
        g_ready_list = task;
        task->next = task;             // ring
    } else {
        task_ctx_t *p = g_ready_list;
        while (p->next != g_ready_list) p = p->next;
        p->next = task;
        task->next = g_ready_list;
    }
}

void ready_remove(task_ctx_t *task)
{
    if (!g_ready_list) return;
    if (task->next == task) {
        g_ready_list = 0;
        return;
    }
	int loop_guard = MAX_TASKS + 1;
    task_ctx_t *p = g_ready_list;
    while (p->next != task && --loop_guard) p = p->next;
	if (!loop_guard) return;
    p->next = task->next;
    if (g_ready_list == task) g_ready_list = task->next;
}

// BLOCKED LIST
void blocked_push(task_ctx_t *task)
{
    task->next = g_blocked_list;
    g_blocked_list = task;
}

void blocked_remove(task_ctx_t *task)
{
    task_ctx_t *p = g_blocked_list;
	task_ctx_t *prev = 0;
    while (p && p != task) { prev = p; p = p->next; }
    if (!p) return;
    if (prev) { prev->next = p->next; }
    else { g_blocked_list = p->next; }
}

task_ctx_t *task_by_pid(int pid) 
{
	int taken = g_tasks_bitmap << (1u << pid);
	if (taken) {
		return &(g_tasks_pool[pid]);
	}
	else {
		return 0;
	}
}

int task_create(const char *name, void (*entry)(void), uint32_t stack_bytes)
{	
	int pid;
    task_ctx_t *task = task_alloc(&pid);
    if (!task) return -1;
	
	uint32_t *stack = stack_alloc(stack_bytes);
    if (!stack) { task_free(task); return -1; }

    int cls = stack_class_index(stack_bytes);
	if (cls < 0) { task_free(task); return -1; }
	uint32_t stack_words = g_class_words[cls];

	uint32_t *top = stack + stack_words;

	// fake exception frame
	*(--top) = 0x01000000;          // xPSR, Thumb bit OR HardFault INVSTATE
	*(--top) = (uint32_t)entry;     // PC
	*(--top) = 0;                   // LR
	*(--top) = 0;                   // R12
	*(--top) = 0;                   // R3
	*(--top) = 0;                   // R2
	*(--top) = 0;                   // R1
	*(--top) = 0;                   // R0
	*(--top) = 0;                   // R11
	*(--top) = 0;                   // R10
	*(--top) = 0;                   // R9
	*(--top) = 0;                   // R8
	*(--top) = 0;                   // R7
	*(--top) = 0;                   // R6
	*(--top) = 0;                   // R5
	*(--top) = 0;                   // R4

	task->sp          = top;
    task->name        = name;
    task->state       = TASK_READY;
    task->stack_base  = stack;
    task->stack_words = stack_words;
	
	uint32_t irq_mask = irq_save();
    ready_push(task);
    irq_restore(irq_mask);
	
	return pid;
}

task_ctx_t *schedule(void)
{
	task_ctx_t *old = g_current_task;
	
	if (!g_ready_list) return 0;
    
    if (old && old->state == TASK_RUNNING) {
        old->state = TASK_READY;
    }
    
    if (!old || old->state == TASK_BLOCKED || old->state == TASK_FREE) {
        g_current_task = g_ready_list;
    } else {
        g_current_task = old->next;
    }
	
	g_current_task->state = TASK_RUNNING;
	
	if (old && old->state == TASK_FREE) {
        stack_free(old->stack_base, old->stack_words * 4);
        if (old->prog_pages > 0) {
            page_free((void *)old->prog_base, old->prog_pages);
        }
        task_free(old);
    }
    
    return g_current_task;
}

void task_yield(void)
{
	syscall(SYS_YIELD, 0, 0, 0);
}

void task_exit()
{
	task_ctx_t *task = g_current_task;
    if (!task) return;
	
    uint32_t irq_mask = irq_save();
	
    if (task->state == TASK_BLOCKED) {
        blocked_remove(task);
    } else {
        ready_remove(task);
    }
    task->state = TASK_FREE;

    *(volatile uint32_t *)PENDSVSET = (1u << 28);  // PendSV
	
	irq_restore(irq_mask);
    while (1);   
}

void task_sleep(uint32_t ms) 
{
    task_ctx_t *task = g_current_task;
    if (!task || ms == 0) {
        task_yield();   // ms 0, skip
        return;
    }
    
    uint32_t irq_mask = irq_save();
    task->state    = TASK_BLOCKED;
    task->wake_at  = systick_counter + ms;
	
	ready_remove(task);
    blocked_push(task);
	
    irq_restore(irq_mask);
    
    task_yield(); // svc 0
}

void scheduler_init(void)
{
	// priorities: SVC=0 (highest), SysTick=1, PendSV=15 (lowest)
	volatile uint32_t *shpr2 = (uint32_t *)0xE000ED1C;
    volatile uint32_t *shpr3 = (uint32_t *)0xE000ED20;
    *shpr2 = (*shpr2 & ~(0xFFu << 24)) | (0x00u << 24);
    *shpr3 = (*shpr3 & ~(0xFFu << 24)) | (0x10u << 24);  // SysTick
    *shpr3 = (*shpr3 & ~(0xFFu << 16)) | (0xFFu << 16);  // PendSV
	
	g_tasks_bitmap = 0;
    g_current_task    = 0;
    g_ready_list = 0;
	
	stack_pool_init();
}

void scheduler_start(void)
{	
	if (!g_ready_list) {
        // no tasks = loop
        while (1);
    }
	
	syscall(SYS_YIELD, 0, 0, 0);
    while (1);
}