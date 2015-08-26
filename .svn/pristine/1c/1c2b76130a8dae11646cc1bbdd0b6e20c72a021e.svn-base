#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "terminal.h"
#include "paging.h" 
#include "pcb.h"
#include "keyboard.h"
#include "i8259.h"
#include "filesystem.h"
#include "x86_desc.h"

#define STACKPOINTER 0x83FFFFC
#define PAGE_VIRT_ADDR	0x8000000
#define MAXSYSCALL 10
#define ELF_MAGIC_BYTE 0x7F


extern int32_t halt(uint8_t status);

extern int32_t execute(const uint8_t* command);

extern int32_t read(int32_t fd, void* buf, int32_t nbytes);

extern int32_t write(int32_t fd, const void* buf, int32_t nbytes);

extern int32_t open (const uint8_t* filename);

extern int32_t close (int32_t fd);

extern int32_t getargs (uint8_t* buf, int32_t nbytes);

extern int32_t vidmap(uint8_t** screen_start);


extern int32_t check_command(const uint8_t* command, uint8_t* exec_name, uint8_t* exec_args);
extern int32_t check_executable(uint8_t* exec_name, uint32_t *entry_addr);



//EXTRA CREDIT SYSCALLS
extern int32_t set_handler(int32_t signum, void* handler_address);
extern int32_t sigreturn(void);

#endif
