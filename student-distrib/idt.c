#include "idt.h"

extern void syscall_handler();

//array of function pointers for exceptions
void(*idt_func[19])(void)={exception0, exception1, exception2, exception3, exception4, 
	exception5, exception6, exception7, exception8, exception9, exception10,  exception11,
	 exception12, exception13, exception14, exception15, exception16, exception17, exception18};

//functions that print exception name based on trap vectors
//exception for divide by 0 
void exception0()
{
	cli();
	//clearscreen();
	printf("exception: divide by 0 \n");
	print_skull();
	while(1){};
	sti();
}

//exception for debug
void exception1()
{
	cli();
	printf("exception: debug \n");
	print_skull();
	while(1){};
	sti();
}

//exception for non-maskable interrupt
void exception2()
{
	cli();
	printf("exception:non-maskable interrupt\n");
	print_skull();
	while(1){};
	sti();
}

//exception for breakpoint
void exception3()
{
	cli();
	printf("exception: breakpoint\n ");
	print_skull();
	while(1){};
	sti();
}

//exception for overflow
void exception4()
{
	cli();
	printf("exception: overflow \n");
	print_skull();
	while(1){};
	sti();
}

//exception for bound range exceeded
void exception5()
{
	cli();
	printf("exception: bound range exceeded \n");
	print_skull();
	while(1){};
	sti();
}

//exception for invalid opcode
void exception6()
{
	cli();
	printf("exception: invalid opcode \n");
	print_skull();
	while(1){};
	sti();
}

//exception for device not available
void exception7()
{
	cli();
	printf("exception: device not available \n");
	print_skull();
	while(1){};
	sti();
}

//exception for double fault
void exception8()
{
	cli();
	printf("exception: double fault \n");
	print_skull();
	while(1){};
	sti();
}

//exception for coprocessor segment overrun
void exception9()
{
	cli();
	printf("exception: coprocessor segment overrun \n");
	print_skull();
	while(1){};
	sti();
}

//exception for invalid TSS
void exception10()
{
	cli();
	printf("exception: invalid TSS \n");
	print_skull();
	while(1){};
	sti();
}

//exception for segment not present
void exception11()
{
	cli();
	printf("exception: segment not present \n");
	print_skull();
	while(1){};
	sti();
}

//exception for stack segement fault
void exception12()
{
	cli();
	printf("exception: stack segement fault \n");
	print_skull();
	while(1){};
	sti();
}

//exception for general protection fault
void exception13()
{
	cli();
	printf("general proctection fault\n");
	print_skull();
	while(1){};
	sti();
}

//exception for page fault sebs a fatty
void exception14()
{
	cli();
	printf("exception: page fault  - HEY TA: you look good today!\n");
	print_skull();
	while(1){};
	sti();
}

//exception for Unknown Interrupt Exception
void exception15()
{
	cli();
	printf("exception: Unknown Interrupt Exception \n");
	print_skull();
	while(1){};
	sti();
}

//exception for x87 floating point
void exception16()
{
	cli();
	printf("exception:x87 floating point \n");
	print_skull();
	while(1){};
	sti();
}

//exception for alignment check
void exception17()
{
	cli();
	printf("exception: alignment check \n");
	print_skull();
	while(1){};
	sti();
}

//exception for machine check
void exception18()
{
	cli();
	printf("exception: machine check \n");
	print_skull();
	while(1){};
	sti();
}


void print_skull(){
	printf("                                     uuuuuuu\n");
	printf("                                 uu$$$$$$$$$$$uu\n");
	printf("                              uu$$$$$$$$$$$$$$$$$uu\n");
	printf("                             u$$$$$$$$$$$$$$$$$$$$$u\n");
	printf("                            u$$$$$$$$$$$$$$$$$$$$$$$u\n");
	printf("                           u$$$$$$:   :$$$:   :$$$$$$u\n");
	printf("                           :$$$$:      u$u       $$$$:\n");
	printf("                            $$$u       u$u       u$$$\n");
	printf("                            $$$u      u$$$u      u$$$\n");
	printf("                             :$$$$uu$$$   $$$uu$$$$:\n");
	printf("                              :$$$$$$$:   :$$$$$$$:\n");
	printf("                                u$$$$$$$u$$$$$$$u\n");
	printf("                                 u$:$:$:$:$:$:$u\n");
	printf("                      uuu        $$u$ $ $ $ $u$$       uuu\n");
	printf("                     u$$$$        $$$$$u$u$u$$$       u$$$$\n");
	printf("                      $$$$$uu      :$$$$$$$$$:     uu$$$$$$\n");
	printf("                    u$$$$$$$$$$$uu    ::::    uuuu$$$$$$$$$$\n");
	printf("                    $$$$:::$$$$$$$$$$uuu   uu$$$$$$$$$:::$$$:\n");
	printf("                     :::      ::$$$$$$$$$$$uu ::$:::\n");
	printf("                               uuuu ""$$$$$$$$$$uuu\n");
	printf("                      u$$$uuu$$$$$$$$$uu ::$$$$$$$$$$$uuu$$$\n");
	printf("                      $$$$$$$$$$:::           ::$$$$$$$$$$$:\n");
	printf("                       $$$$:                        :$$$$$:\n");
	return;
}

/*
 *	idt_init
 *		Function:	Initialize the IDT
 *		Inputs:		none
 *		Outputs:	none
 *		Effect:		Initializes the IDT with exception descriptors and hardware/software interrupt descriptors
*/
void idt_init(){
       int i;
       idt_desc_t idt_set; //IDT descriptor to use when we fill each entry

                idt_set.seg_selector=KERNEL_CS;
               idt_set.reserved0=0;
                idt_set.reserved1=1;
                idt_set.reserved2=1;
                idt_set.reserved4=0;
                idt_set.present=1;
                idt_set.size=1;
                idt_set.dpl=0;
        //exception handlers entry
        for(i=0; i<19;i++)
        {
                idt_set.reserved3=1;
                SET_IDT_ENTRY(idt_set, idt_func[i]);
                idt[i]=idt_set;
        }

       //keyboard descriptors
       idt_set.reserved3=0;
       SET_IDT_ENTRY(idt_set, &keyboard_wrapper);
       idt[KEYBOARDPORT]=idt_set;

       //rtc descriptors
       SET_IDT_ENTRY(idt_set, &rtc_handler);
       idt[RTCPORT]=idt_set;

		//PIT descriptors
       SET_IDT_ENTRY(idt_set, &scheduler_wrapper);
       idt[PIT_PORT] = idt_set;

       //Syscall descriptors
       idt_set.reserved3=1;
       idt_set.dpl = USER_LEVEL; //Set DPL for allow usermode to do syscalls
       SET_IDT_ENTRY(idt_set, &syscall_handler);
       idt[SYSCALL_PORT]=idt_set;
}

