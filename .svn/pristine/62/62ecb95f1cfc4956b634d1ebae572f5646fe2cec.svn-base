#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "lib.h"
#include "types.h"
#include "keyboard.h"
#include "filesystem.h"
#include "pcb.h"
#include "paging.h"
#include "syscall.h"

#define HEIGHT 25
#define WIDTH 80
#define VIDEO_MEM 0xB8000
#define UNDERLINE_LOC 14
#define	START_BLINK 15
#define INDEX_PORT 0x3D4
#define WRITE_PORT 0x3D5
#define BLANK_SPACE 0x20
#define ASCII_BACKSPACE 0x08
#define BUFF_LEN 128
#define	MAX_TERMINALS 3

#define TERMINAL_1 		0
#define TERMINAL_2		1
#define	TERMINAL_3		2

#define TERMINAL_MEM_1	0x0001000
#define TERMINAL_MEM_2	0x0002000
#define TERMINAL_MEM_3	0x0003000

#define TRUE 	1
#define FALSE 	0

extern uint32_t terminal_open(const uint8_t* filename);
extern uint32_t terminal_close(uint32_t fd);
extern uint32_t terminal_write(uint32_t fd, const void * buf, int32_t count);
extern uint32_t terminal_read(uint32_t fd, void * buf, int32_t count);
extern int32_t switch_terminal(uint32_t terminal_num);

extern void terminal_init();

extern void putch(unsigned char c);
extern void print_buffer();

extern void backspace();
extern void new_line(unsigned char reset_buffer);
extern void add_to_buffer(unsigned char c);
extern void clear_buffer();
extern void clear_screen();

unsigned char terminal_buffer[MAX_TERMINALS][BUFF_LEN];
uint16_t buffer_len[MAX_TERMINALS];
int csr_x[MAX_TERMINALS], csr_y[MAX_TERMINALS], line_y[MAX_TERMINALS];
int line_beginning[MAX_TERMINALS];
int curr_terminal;

int attrib; 

typedef struct terminal_t
{
       uint32_t top_process_id;
       uint32_t num_process;

} terminal_t;

terminal_t terminal_info[MAX_TERMINALS];

uint32_t terminal_memory[MAX_TERMINALS];

#endif
