#ifndef _IDT_H
#define _IDT_H
#define PIT_PORT         0x20

#define KEYBOARDPORT 0x21
#define RTCPORT 0x28
#define SYSCALL_PORT 0x80

#define USER_LEVEL 3
#define KERNEL_LEVEL 0

#include "lib.h"
#include "scheduler.h"
#include "x86_desc.h"
#include "rtc.h"
#include "keyboard.h"

extern void keyboard_wrapper();
extern void scheduler_wrapper();

//function that transfers our descriptors to IDT
void idt_init();

//exception for divide by 0 
void  exception0();

//exception for debug
void  exception1();

//exception for non-maskable interrupt
void  exception2();

//exception for breakpoint
void  exception3();

//exception for overflow
void  exception4();

//exception for bound range exceeded
void  exception5();

//exception for invalid opcode
void  exception6();

//exception for device not available
void  exception7();

//exception for double fault
void  exception8();

//exception for coprocessor segment overrun
void  exception9();

//exception for invalid TSS
void  exception10();

//exception for segment not present
void  exception11();

//exception for stack segement fault
void  exception12();

//exception for general protection fault
void  exception13();

//exception for page fault sebs a fatty
void  exception14();

//exception for Unknown Interrupt Exception
void  exception15();

//exception for x87 floating point
void  exception16();

//exception for alignment check
void  exception17();

//exception for machine check
void  exception18();

//Prints a skull to the text mode display
void print_skull();



#endif 
