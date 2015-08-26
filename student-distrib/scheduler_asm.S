#define ASM 1


.text
.globl   scheduler_wrapper
.align   4
 
scheduler_wrapper:
    pushal
    call pit_handler
    popal
    iret