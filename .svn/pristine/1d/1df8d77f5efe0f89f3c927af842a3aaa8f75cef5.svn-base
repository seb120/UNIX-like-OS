
#ifndef _PCB_H
#define _PCB_H

#include "lib.h"
#include "filesystem.h"

#define MAX_CMD_LENGTH 	128
#define MAX_PROCESSES	7
#define FD_SIZE 618
#define KERNEL_STACK_OFFSET 0x2000
#define KERNEL_STACK 0x00800000

uint32_t process_number;


typedef struct pcb_t
{
	fs_descriptor_t file_array[8];
	uint32_t program_id;
	//struct pcb_t *parent_pcb;
	uint32_t parent_id;
	uint32_t address;//physical address

	uint32_t esp;
	uint32_t ebp;
	uint32_t last_eip;
	uint8_t esp0;

	unsigned char cmd_name[MAX_CMD_LENGTH];
	unsigned char cmd_args[MAX_CMD_LENGTH];

	int process_terminal;
}pcb_t;

pcb_t* current_pcb;

pcb_t pcbs[MAX_PROCESSES];

//Create a new PCB and return the ptr
extern pcb_t *get_new_pcb();

//Get the pointer for the current PCB
extern pcb_t* get_cur_pcb();

//Get the parent (top) level PCB
extern pcb_t *get_top_pcb();

extern uint32_t find_free_pcb();

//Initialize the PCB
extern int32_t pcb_init(pcb_t *pcb_ptr);

//Destroy the PCB and free it
extern int32_t pcb_destroy(pcb_t *pcb_ptr);

//Return index of a free FD in the PCB
extern int32_t find_free_fd(pcb_t *pcb_ptr);

extern int32_t remove_fd(pcb_t *pcb_ptr, uint8_t fd);

extern int32_t find_free_fd(pcb_t *pcb_ptr);

extern uint32_t get_kernel_stack_ptr(uint32_t program_id);

#endif
