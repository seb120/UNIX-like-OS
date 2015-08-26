#define ASM 1


.text
.globl   keyboard_wrapper
.align   4
 
keyboard_wrapper:
    pushal
    call keyboard_handler
    popal
    iret
