# paging.S - Set up paging registers 
# vim:ts=4 noexpandtab

#define ASM     1

.text
.globl loadPageDirectory, enablePaging
.data
        PAGESIZE4MB  = 0x00000010
        PAGINGENABLE = 0x80000000


#	void loadPageDirectory(uint32_t*);
#		Function:		Load page directory
#		Inputs:			uint32_t*, stack argument pointer to the given page directory
#		Effect:			Update CR3 register
#
loadPageDirectory:
	pushl %ebp
	movl %esp, %ebp
	movl 8(%esp), %eax
	movl %eax, %cr3
	leave
	ret

#	void enablePaging();
#		Function:		enables paging in the CPU
#		Inputs:			none
#		Effect:			set bit 31 of cr0, set bit 4 of cr4 for 4MB pages
#
enablePaging:
	movl %cr4, %ecx
	orl	 $PAGESIZE4MB, %ecx 
	movl %ecx, %cr4

	movl %cr0, %ecx
	orl $PAGINGENABLE, %ecx
	movl %ecx, %cr0

	ret
