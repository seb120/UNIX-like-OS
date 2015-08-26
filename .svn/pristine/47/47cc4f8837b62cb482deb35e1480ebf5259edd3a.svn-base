#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "lib.h"
#include "pcb.h"
#include "terminal.h"

/*attributes: not present, read/write, supervisor level*/
#define ATTRIBUTE_0 0x6

/*attributes: present, read/write, user level*/
#define ATTRIBUTE_1 0x7
 
/*
*attributes: global, 4mb, present, read/write, supervisor level
starting at location x400000*
*/
#define KERNEL_PAGE 0x00400183
#define USER_PAGE_8MB 0x00800087
#define USER_PAGE_12MB 0x00C00087
#define fourMB 0x400000

#define VIDMAP_OFFSET 0x21
#define PROGRAM_OFFSET 0x20
#define VIDEO_OFFSET 0
#define KERNEL_OFFSET 1
/*size of a 4KB page*/
#define PAGE_SIZE_4KB 0x1000

#define PAGE_SIZE_4MB 0x400000

/*video memory location*/
#define VIDEO_MEM 0xB8000
#define VIDMAP 0x084B8000
#define ENTRIES 1024
#define MAX_TASKS 6

uint32_t page_table[ENTRIES] __attribute__((aligned(4096)));
uint32_t page_table_vidmap[ENTRIES] __attribute__((aligned(4096)));

typedef struct page_directory{
	uint32_t page_directory_entry[ENTRIES] __attribute__((aligned(4096))); //aligned global page directory
} page_directory_t;

/*array of page directories*/ 
page_directory_t page_directories[MAX_TASKS] __attribute__((aligned(4096)));

extern void paging_init();//init function for paging
extern uint32_t new_task_pd(uint32_t program_id);
extern int32_t create_vidmap();

#endif
