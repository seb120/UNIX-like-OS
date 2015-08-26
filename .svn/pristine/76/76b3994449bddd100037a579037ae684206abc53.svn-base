#include "paging.h"


extern void loadPageDirectory(uint32_t*);
extern void enablePaging();


/*
*	paging_init
*		Function:		Initialize the paging system
*		Inputs:			none
*		Outputs:		none
*		Effect:			Create a directory. Setup video memory table and page. Create 4MB kernel page.
*		Inspiration: 	http://wiki.osdev.org/Setting_Up_Paging
*/
void paging_init()
{
	int i;
	unsigned int j;

	//initialize all 1024 entries of directory
	for(i = 0; i < ENTRIES; i++)
	{
	    page_directories[0].page_directory_entry[i] = ATTRIBUTE_0;
	}

 	//set all 1024 entries in the video table to not present except for page at 0xB8000
	for(j = 0; j < ENTRIES; j++)
	{
		if((j*PAGE_SIZE_4KB) == VIDEO_MEM)
			page_table[j] = ((j * PAGE_SIZE_4KB)) | ATTRIBUTE_1;
		else
			page_table[j] = ((j * PAGE_SIZE_4KB)) | ATTRIBUTE_1;  
	}

	//add tables and pages to directory
	page_directories[0].page_directory_entry[VIDEO_OFFSET] = ((uint32_t)page_table) | ATTRIBUTE_1;
	page_directories[0].page_directory_entry[KERNEL_OFFSET] = KERNEL_PAGE;

	loadPageDirectory(&page_directories[0].page_directory_entry[0]);
	enablePaging();
}


/*
*	new_task_pd
*		Function:		Create a new page directory for making a new task
*		Inputs:			program_id, the new program number to be created
*		Returns:		0 for success, -1 if the program_id is exceeds our process limit.
*		Effect:			Create a page directory entry and reload CR3
*/
uint32_t new_task_pd(uint32_t program_id)
{
	int i;
	if(program_id > MAX_TASKS){
		return -1;
	}

	//initialize all 1024 entries of directory
	for(i = 0; i < ENTRIES; i++)
	{
	    page_directories[program_id].page_directory_entry[i] = ATTRIBUTE_0;
	}
	//add table and pages to directory
	page_directories[program_id].page_directory_entry[VIDEO_OFFSET] = ((uint32_t)page_table) | ATTRIBUTE_1;
	page_directories[program_id].page_directory_entry[KERNEL_OFFSET] = KERNEL_PAGE;

	//map virtual addr 0x8000000 to either physical 8mb or 12mb
	page_directories[program_id].page_directory_entry[PROGRAM_OFFSET] = USER_PAGE_8MB + (program_id * fourMB);

	//load cr3 with address of new directory
	loadPageDirectory(&page_directories[program_id].page_directory_entry[0]);

	return 0;
}

/*
*	create_vidmap()
*		Function:		Helper function for vidmap system call.
*		Inputs:			none
*		Outputs:		none
*		Effect:			Set page directory of Video Memory location
*		Inspiration: 	http://wiki.osdev.org/Setting_Up_Paging
*/
int32_t create_vidmap(){
	//VIDMAP 0x088B8000
	//set all 1024 entries in the video table to not present except for page at 0xB8000
	int j;
	for(j = 0; j < ENTRIES; j++)
	{
		if((j*PAGE_SIZE_4KB) == VIDEO_MEM)
			page_table_vidmap[j] = ((VIDEO_MEM)) | ATTRIBUTE_1;
		else
			page_table_vidmap[j] = ((VIDEO_MEM)) | ATTRIBUTE_0;  
	}
	page_directories[current_pcb->program_id-1].page_directory_entry[VIDMAP_OFFSET] = ((uint32_t)page_table_vidmap) | ATTRIBUTE_1;
	return 0;
}
