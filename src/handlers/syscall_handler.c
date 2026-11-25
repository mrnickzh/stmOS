#include "handlers/syscall_handler.h"
#include "scheduler/scheduler.h"
#include "platform/Cortex-M4.h"
#include "shell/term.h"

void syscall_handler(uint32_t *frame)
{
    uint32_t num  = frame[0];    // r0 = syscall number
    uint32_t arg1 = frame[1];    // r1
    uint32_t arg2 = frame[2];    // r2
    uint32_t arg3 = frame[3];    // r3
    
    switch (num) {
        case SYS_YIELD:
            *(volatile uint32_t *)PENDSVSET = (1U << 28);   // PENDSVSET
            frame[0] = 0;   // return 0
            break;
        
        case SYS_WRITE:
            // arg1 = ptr, arg2 = len
            term_n_putstring((unsigned char*)arg1, arg2);
            frame[0] = arg2;   // return bytes written
            break;
        
        case SYS_EXIT:
			ready_remove(g_current_task);
			g_current_task->state = TASK_FREE;
            *(volatile uint32_t *)PENDSVSET = (1U << 28);   // PENDSVSET
            break;
        
        // case SYS_READ:
            // // arg1 = buf, arg2 = maxlen
            // // Блокирующее чтение — сложнее, обсудим отдельно
            // frame[0] = 0;
            // break;
        
        default:
            frame[0] = (uint32_t)-1;
            break;
    }
}