.syntax unified
.cpu cortex-m4
.thumb

.global SVC_Handler
.global PendSV_Handler
	
@ supervisor call (syscall) handler
.thumb_func
.type SVC_Handler, "function"
SVC_Handler:
    @ MSP or PSP
    tst   lr, #4                @ EXC_RETURN bit 2 (0b0100)
    ite   eq
    mrseq r0, msp               @ 0 MSP
    mrsne r0, psp               @ 4 PSP
    
    @ save EXC_RETURN
    push  {lr}
    
    @ call C handler
    bl    syscall_handler
    
    @ restore EXC_RETURN
    pop   {lr}
    bx    lr

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