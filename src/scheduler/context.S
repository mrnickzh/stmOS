.syntax unified
.cpu cortex-m4
.thumb

.global SVC_Handler
.global PendSV_Handler

@ SVC_Handler: scheduler start
.thumb_func
.type SVC_Handler, "function"
SVC_Handler:
    ldr  r0, =g_current_task
    ldr  r1, [r0]
    cbz  r1, 1f                  @ g_current_task < 0 = first start

    @ yield
    ldr  r2, =0xE000ED04     @ ICSR
    mov  r3, #(1 << 28)      @ PENDSVSET
    str  r3, [r2]
    bx   lr

1:  bl   schedule
    ldr  r1, [r0]
	
	@ load task
    ldmia r1!, {r4-r11}
    msr  psp, r1
	
    isb
    ldr  lr, =0xFFFFFFFD @ EXC_RETURN: Thread mode, PSP
    bx   lr

@ PendSV_Handler: ctx switch
.thumb_func
.type PendSV_Handler, "function"
PendSV_Handler:
    cpsid i

    ldr  r1, =g_current_task
    ldr  r2, [r1]
    cbz  r2, 2f
	
	@ save task
	mrs  r0, psp
    stmdb r0!, {r4-r11}
	
    str  r0, [r2]

2:  bl   schedule
    ldr  r0, [r0]
	
	@ load task
    ldmia r0!, {r4-r11}
    msr  psp, r0
	
    isb
    cpsie i
    ldr  lr, =0xFFFFFFFD @ EXC_RETURN: Thread mode, PSP
    bx   lr