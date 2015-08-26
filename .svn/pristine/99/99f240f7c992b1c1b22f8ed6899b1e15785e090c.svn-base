#include "syscall.h"

extern void SYS_POP_ALL();

extern void syscall_handler();

extern void SYS_SAVE_ALL();

extern void syscall_return();

extern void syscall_error();

extern uint32_t syscall_switch_to_user(uint32_t*, uint32_t*);

extern void loadPageDirectory(uint32_t*);

int32_t close (int32_t fd);


/*
 *	HALT
 *		Function:	Terminates Caller Process, thus returning to parent task
 *		Inputs:		status, given by user program to be passed to user who first called execute
 *		Returns:	0 for success
*/
int32_t halt(uint8_t status)
{
	uint32_t status_32 = 0;
	status_32 = status;
	uint8_t* cmd = (uint8_t*)"shell";
	int i = 0;

	if ( (terminal_info[curr_terminal].num_process-1) <= 0 )
	{
		for(i=0; i<=(FD_MAX_IDX);i++)
		{
			close(i);
		}	
		pcb_destroy(current_pcb);
		execute(cmd);
	}

	asm volatile("movl %0, %%esp"
		:
		:"r"(current_pcb->esp)
		:"cc"
	);

	asm volatile("movl %0, %%ebp"
		:
		:"r"(current_pcb->ebp)
		:"cc"
	);

 	for(i=0; i<=(FD_MAX_IDX);i++)
	{
		close(i);
	}

 	//Destroy the current process
    pcb_destroy(current_pcb);

	(&tss)->esp0=current_pcb->address;
	loadPageDirectory(&page_directories[(current_pcb->program_id)-1].page_directory_entry[0]);

	asm volatile("jmp exec_halt");
	
	//we will never get here
	return 0;
}

/*
 *	EXECUTE
 *		Function:	Execute a new process and set the current process as parent
 *		Inputs:		status, given by user program to be passed to user who first called execute
 *		Returns:	0 for success (once the task halts)
 *					256 if the process is halted due to an exception
 *					0-255 if the user program calls the syscall halt
 *		Effect:		none
*/
int32_t execute(const uint8_t* command){
	send_eoi(KEYBOARD_IRQ_NUM);
    asm volatile("cli");

	if(process_number+1 >= MAX_PROCESSES)
	{
		printf("you can't run any more processes!\n");
		return 0;
	}
	process_number++;
	
	pcb_t *new_pcb_ptr = get_new_pcb();
	pcb_t *cur_pcb_ptr = get_cur_pcb();
	
	//Null ptr check
	if (new_pcb_ptr ==  0)
	{
		return -1;
	}

	//Initialize the new PCB
	pcb_init(new_pcb_ptr);
	
	//Update the parent process ID if it exists, otherwise pcb_init makes parent_id==0
	if(cur_pcb_ptr!=NULL)
	{
		new_pcb_ptr->parent_id=cur_pcb_ptr->program_id;
	}
	if(cur_pcb_ptr==&pcbs[0])
	{
		new_pcb_ptr->parent_id=0;
	}

	//Update our local current pointer to the new process we just created
	cur_pcb_ptr = get_cur_pcb();
	
	// Check command input
	if ( check_command(command, new_pcb_ptr->cmd_name, new_pcb_ptr->cmd_args) != 0 )
	{
		pcb_destroy(new_pcb_ptr);
		return -1;
	}
	
	uint32_t entry_addr;
    if ( check_executable( new_pcb_ptr->cmd_name, &entry_addr) < 0 )
    {
        pcb_destroy(new_pcb_ptr);
        return -1;
    }
	
	//  Set up a new page directory, 
    new_task_pd((current_pcb->program_id)-1);
    
	// 	Load the executable file into virtual memory
	copy_file_image(current_pcb->cmd_name);

	//	Set up the stack in order to context switch into user's program
	asm volatile(
		"movl %%esp, %0"
		:"=a"(cur_pcb_ptr->esp)
		:
		:"cc"
		);
	asm volatile(
		"movl %%ebp, %0"
		:"=a"(cur_pcb_ptr->ebp)
		:
		:"cc"
	);

	(&tss)->ss0 =KERNEL_DS;

	(&tss)->esp0=new_pcb_ptr->address;
	
	asm(
		"cli;"
		"movl	%0, %%ecx;"
		"movl	%1, %%edx;"
		"movw	%2, %%ax;"
		"movw	%%ax, %%ds;"
		"movw	%%ax, %%es;"
		"movw	%%ax, %%fs;"
		"movw	%%ax, %%gs;"
		
		"pushl	%2;"
		"pushl	%%edx;"
		"pushf;"
		
		"popl	%%eax;"
		"or		$0x200, %%eax;"
		"pushl	%%eax;"
		
		"pushl	%3;"

		"pushl	%%ecx;"
		
		"iret;"
		"exec_halt: "
		:
		: "g"(entry_addr), "g"(STACKPOINTER), "g"(USER_DS), "g" (USER_CS)
		:"eax","edx","ecx"
	);

	return 0;
}

/* 
 *	READ
 *		Function:	Reads data from a file by its descriptor
 *		Inputs:		fd, file descriptor
 *					buf, buffer to be read into
 *					nbytes, number of bytes to be read into the buffer
 *		Returns:	number of bytes written for success
 *					-1 for failure
*/
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
	if(current_pcb->file_array[fd].flags==0)
		return -1;
	if ( buf == NULL)
		return -1;
	else if ( fd > FD_MAX_IDX )
		return -1;
	else if ( fd < 0 )
		return -1;
	else if ( fd == 1 )
		return -1; 	// can't read from STDOUT
	else
	{
		return current_pcb->file_array[fd].file_ops->readptr(fd, buf, nbytes);
	}
}


/*
 *	WRITE
 *		Function:	Writes data to a file by its descriptor.
 *		Inputs:		fd, file descriptor
 *					buf, buffer with data to be written
 *					nbytes, number of bytes to be written
 *		Returns:	number of bytes written for success
 *					-1 for failure
*/
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
	if(current_pcb->file_array[fd].flags==0)
		return -1;
	if ( buf == NULL )
		return -1;
	else if ( fd > FD_MAX_IDX )
		return -1;
	else if ( fd <= 0 )
		return -1;	//can't write to STDIN
	else
	{
		return pcbs[current_pcb->program_id].file_array[fd].file_ops->writeptr(fd, buf, nbytes);
	}

}


/*
 *	OPEN
 *		Function:	Opens the given file by name.
 *		Inputs:		filename, char string pointer of filename to be opened.
 *		Returns:	index of new file descriptor for success.
 * 					-1 for failure.
 *		Effect:		Allocated a free fd.
*/
int32_t open (const uint8_t* filename)
{
	return open_file(filename);
}


/*
 *	CLOSE
 *		Function:	Closes the given file by its file descriptor.
 *		Inputs:		fd, the descriptor for the file to be closed.
 *		Returns:	0 for success, -1 for fail.
 *		Effect:		Frees the specified fd.
*/
int32_t close (int32_t fd)
{
	if(current_pcb->file_array[fd].flags==0)
		return -1;
	if ( fd < FD_MIN_IDX )
		return -1;
	else if ( fd > FD_MAX_IDX )
		return -1;
	else
	{
		pcb_t *pcb_ptr = get_cur_pcb();
		if ( pcb_ptr->file_array[fd].flags == FILE_NOT_IN_USE )
			return -1;	//can't close file that isn't active
		else
			pcb_ptr->file_array[fd].flags = FILE_NOT_IN_USE;
	
		//Call the closing function
		(((pcb_ptr->file_array)[fd]).file_ops->closeptr)(fd);

		//Remove the file from the file descriptor
		remove_fd(pcb_ptr, fd);
		
		return 0;
	}
}


/*
 *	GETARGS
 *		Function:	Reads programs command line arguments into buffer
 *		Inputs:		buf, user-level buffer to be filled
 *					nbytes, number of bytes to be written
 *		Returns:	0 for success, -1 for fail.
*/
int32_t getargs (uint8_t* buf, int32_t nbytes)
{
	if(buf == NULL)
		return -1;
	pcb_t* curr_pcb = get_cur_pcb();
	memcpy(buf, curr_pcb->cmd_args, nbytes);
	buf[nbytes] = '\0';
	return 0;
}


/*
 *	VIDMAP
 *		Function:	Maps textmode vid memory into user's virtual address
 *		Inputs:		screen_start, ptr to ptr of beginning of textmode vid mem
 *		Returns:	0 for success, -1 for fail
*/
int32_t vidmap(uint8_t** screen_start)
{
	if (screen_start == NULL)
		return -1;
	if(((uint32_t)screen_start < PAGE_VIRT_ADDR) || ((uint32_t)screen_start > (PAGE_VIRT_ADDR + PAGE_SIZE_4MB)))
		return -1;
	create_vidmap();
	*screen_start = (uint8_t *)VIDMAP;
	return 0;
}


/*
 *	check_command()
 *		Function:	Check if the given command and its arguments are valid and extract the info
 *		Inputs:		command: command string to check
 *					exec_name: pointer to executable name string to fill
 * 					exec_args: pointer to executable args string to fill
 *		Returns:	0 for success, -1 for fail
*/
int32_t check_command(const uint8_t* command, unsigned char* exec_name, unsigned char* exec_args)
{

	if (command == NULL)
	{
		printf("Command cannot be empty!");
		return -1;
	}

	else if (command[0] == ' ')
	{

		//printf("First command character cannot be a space!");
		return -1;
	}
	int i = 0;
	int j = 0;

	while((command[i] != ' ') && (command[i] != '\0')){
		exec_name[i] = command[i];
		i++;
	}
	exec_name[i] = '\0';

	while(command[i] == ' '){
		i++;
	}

	while(command[i] != '\0'){
		exec_args[j] = command[i];
		i++;
		j++;
	}

	exec_args[j] = '\0';

	return 0;
}


/*
 *	set_handler()
 *		Function:	Set handler (EC)
 *		Inputs:		int32_t signum, void* handler_address
 *		Returns:	0 for success, -1 for fail
*/
int32_t set_handler (int32_t signum, void* handler_address)
{
	return -1;
}


/*
 *	set_handler()
 *		Function:	Signal Return (EC)
 *		Inputs:		none
 *		Returns:	0 for success, -1 for fail
*/
int32_t sigreturn (void)
{
	return -1;
}


/*
 *	check_executable()
 *		Function:	Check if the given name matches a file from the filesystem.
 *					If executable, fill entry_addr with start address of the program
 * 					and its arguments are valid and extract the info
 *		Inputs:		exec_name: pointer to executable name
 * 					entry_addr: pointer to start address of the program, to be filled.
 *		Returns:	0 for success, -1 for fail
*/
int32_t check_executable(uint8_t* exec_name, uint32_t *entry_addr)
{
	file_header_t elf_header;

	dentry_t dentry_cur;
	if(read_dentry_by_name(exec_name, &dentry_cur)==-1){
		return -1;
	}

	if ( dentry_cur.inode== NULL )
	{
		return -1;
	}
	read_data(dentry_cur.inode, 0, elf_header.data, ELF_HEADER_SIZE);

	
	//Magic ELF characters were NOT found in the given exec_name
	if (!(elf_header.magic[0] == ELF_MAGIC_BYTE && elf_header.magic[1] == 'E' && elf_header.magic[2] == 'L' && elf_header.magic[3] == 'F'))
	{
		return -1;
	}
	//Entry pointer in ELF header is out of virtual memory bounds
	if ( (uint32_t) elf_header.entry < PAGE_VIRT_ADDR || (uint32_t) elf_header.entry > PAGE_VIRT_ADDR + PAGE_SIZE_4MB)
	{
		return -1;
	}

	*entry_addr = (uint32_t) elf_header.entry;
	return 0;
}

