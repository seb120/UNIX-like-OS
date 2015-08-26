#include "pcb.h"
#include "terminal.h"


/*
*	pcb_init
*		Function:	Initializes the PCB if we haven't exceeded the process limit
*		Inputs:		pcb_ptr*, the pcb_t to be initialized
*		Outputs:	none
*		Effect:		Sets up the PCB for STDIN/STDOUT, program number and address
*					Also does some housekeeping for keeping track of multiples terminal/process
*/
int32_t pcb_init(pcb_t *pcb_ptr)
{


	pcb_ptr->file_array[0].file_ops=&stdin_ops;
	pcb_ptr->file_array[0].inode_ptr=0;
	pcb_ptr->file_array[0].file_position=0;
	pcb_ptr->file_array[0].flags=1;

	pcb_ptr->file_array[1].file_ops=&stdout_ops;
	pcb_ptr->file_array[1].inode_ptr=0;
	pcb_ptr->file_array[1].file_position=0;
	pcb_ptr->file_array[1].flags=1;

	pcb_ptr->program_id=find_free_pcb();
	pcb_ptr->last_eip=0;
	
	pcb_ptr->process_terminal=curr_terminal;
	terminal_info[curr_terminal].num_process++;
	terminal_info[curr_terminal].top_process_id = pcb_ptr->program_id;

	pcb_ptr->address=get_kernel_stack_ptr(pcb_ptr->program_id);
	pcb_ptr->esp=pcb_ptr->address;
	pcb_ptr->ebp=pcb_ptr->address;
	pcb_ptr->esp0=pcb_ptr->address;
	//pcb_ptr->parent_pcb=0;
	pcb_ptr->parent_id=0;
	memcpy((uint32_t*)pcb_ptr->address-KERNEL_STACK_OFFSET, pcb_ptr, FD_SIZE); //copies pcb to the star of programs address+7Kb, 
	current_pcb=pcb_ptr;
	return 0; 
}


/*
*	pcb_destroy
*		Function:	Destroys the given PCB
*		Inputs:		pcb_ptr*, the pcb_t to be destroyed
*		Outputs:	none
*		Effect:		Essentially clears the PCB by undoing init. 
*/
int32_t pcb_destroy(pcb_t *pcb_ptr)
{
	//int i;
	process_number--;
	terminal_info[curr_terminal].num_process--;
	terminal_info[curr_terminal].top_process_id = pcb_ptr->parent_id;
	current_pcb=&pcbs[pcb_ptr->parent_id];
	/*
	for(i=0; i<(FD_MAX_IDX+1);i++)
	{
		pcb_ptr->file_array[i].file_ops=0;
		pcb_ptr->file_array[i].inode_ptr=0;
		pcb_ptr->file_array[i].file_position=0;
		pcb_ptr->file_array[i].flags=0;
	}
	*/
	pcb_ptr->process_terminal=-1;
	pcb_ptr->program_id=0;
	pcb_ptr->parent_id=0;
	pcb_ptr->address=0;//physical address
	//memset char args to null
		pcb_ptr->esp=0;
	pcb_ptr->ebp=0;
	pcb_ptr->esp0=0;
	return 0; 
}


/*
*	remove_fd
*		Function:	Outputs the symbol of a pressed key to the screen
*		Inputs:		pcb_ptr*, the pcb_t to be destroyed
*					fd, the index of the file descriptor
*		Returns:	-1 for an invalid fd index. 0 for success
*		Effect:		Clears the file descriptor at the given index.
*/
int32_t remove_fd(pcb_t *pcb_ptr, uint8_t fd)
{
	if(fd < FD_MIN_IDX)
		return -1;
	else if(fd > FD_MAX_IDX)
		return -1;
	
	pcb_ptr->file_array[fd].file_ops = NULL;
	return 0;
}


/*
* find_free_fd
*	Function:	find the index of the first free FD block in a pcb
*	Inputs:		pcb_t *pcb_ptr:	a pointer to the current pcb
*	Outputs:	The index of the first free FD block on success, -1 on failure
*	Effects:	None
*/
int32_t find_free_fd(pcb_t *pcb_ptr)
{
	int i;
	for(i = FD_MIN_IDX; i <= FD_MAX_IDX; i++)
	{
		if(pcb_ptr->file_array[i].file_ops == 0)	//if you find a free pointer
			break;	//break out and return it
	}
	if(i == FD_MAX_IDX + 1)	//if no free pointer was found
		return -1;	//return -1
	else
		return i;
}


/*
* get_new_pcb
*	Function:	Get a new free PCB if possible.
*	Inputs:		none
*	Returns:	Pointer to a new pcb_t if found
*	Effects:	None
*/
pcb_t *get_new_pcb()
{
	uint32_t free_process_id=find_free_pcb();
	return &pcbs[free_process_id];
}


/*
* find_free_pcb
*	Function:	Find the index of the next empty PCB
*	Inputs:		none
*	Returns:	index of pcb[] if found, -1 if pcbs[] is full
*	Effects:	None
*/
uint32_t find_free_pcb()
{
	int i;
	for(i=1; i < MAX_PROCESSES; i++)
	{
		if(pcbs[i].program_id==0)
			return i;
	}
	return -1;
}


/*
* get_cur_pcb
*	Function:	Get the pointer for the current PCB
*	Inputs:		none
*	Returns:	pointer to the current process pcb_t
*	Effects:	None
*/
pcb_t* get_cur_pcb()
{
	return current_pcb;
}


/*
* get_top_pcb
*	Function:	Get the pointer for the top PCB
*	Inputs:		none
*	Returns:	pointer to the parent process pcb_t
*	Effects:	None
*/
pcb_t *get_top_pcb()
{
	pcb_t *top = get_cur_pcb();
	while(top->parent_id != 0)
		top = &pcbs[top->parent_id];
	return top;
}


/*
* get_kernel_stack_ptr
*	Function:	Get the kernel stack pointer of the given process
*	Inputs:		program_id, the program we want the stack pointer of 
*	Returns:	the address of the kernel stack pointer for the given process
*	Effects:	None
*/
uint32_t get_kernel_stack_ptr(uint32_t program_id)
{
	return KERNEL_STACK - ( (program_id-1) * KERNEL_STACK_OFFSET);
}



