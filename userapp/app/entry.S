.syntax unified
.thumb
.global _start
_start:
    bl main
    mov r0, #2
    svc #1      @ SYS_EXIT
1:  b 1b