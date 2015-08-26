# syscall.S - Handles system calls
# vim:ts=4 noexpandtab

#define ASM     1
#define KERNEL_DS 0x0018
#define MAX_SYSCALL 10
#define USER_CS 0x0023
#define USER_DS 0x002B

.text
.globl SYS_SAVE_ALL, SYS_POP_ALL, syscall_handler, syscall_return, syscall_error
.data
	
SYS_SAVE_ALL:
	pushl %fs
	pushl %es
	pushl %ds 
	pushl %eax
	pushl %ebp
	pushl %edi
	pushl %esi 
	pushl %edx
	pushl %ecx
	pushl %ebx
	movl $KERNEL_DS, %edx 
	movl %edx, %ds
	movl %edx, %es
	ret

SYS_POP_ALL:
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popl %eax	
	popl %ds
	popl %es
	popl %fs

	ret

/*
 *	System Call Handler
 *		Function:	
 *		Inputs:		none
 *		Outputs:	none
 *		Effect:		
*/
syscall_handler:
	pushl %fs
	pushl %es
	pushl %ds 
	pushl %ebp
	pushl %edi
	pushl %esi 
	pushl %edx
	pushl %ecx
	pushl %ebx
	movl $KERNEL_DS, %edx 
	movl %edx, %ds
	movl %edx, %es

	cmpl $(MAX_SYSCALL), %eax	# syscall validity check
	ja syscall_error

	cmpl $0, %eax			
	jle syscall_error

	subl $1, %eax				# call the corresponding function
	call *syscall_table(, %eax, 4) 

	#movl %eax, 24(%esp)		# return eax for usermode
							# stack's saved EAX is overwritten
							# so when we pop it fill EAX with our return value

syscall_return:
	cli
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp	
	popl %ds
	popl %es
	popl %fs
	iret

syscall_error:
	movl $-1, %eax			# Return -1 for error
	jmp syscall_return

syscall_table:
		.long halt, execute, read, write, open, close, getargs, vidmap



