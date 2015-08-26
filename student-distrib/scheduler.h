#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "lib.h"
#include "pcb.h"
#include "paging.h"
#include "terminal.h"
#include "i8259.h"
#include "x86_desc.h"
 
#define PIT_IRQ_NUM			0

#define PIT_COMMAND_PORT	0x43
#define PIT_CH0_PORT		0x40
#define PIT_MODE_SETTING	0x34     // 00   11	  010	   0	...   CH0  LH   RateGen  Binary
#define PIT_FREQ_SETTING	0x2E9B	 // f = 1193182 / 100Hz = (11931)base-10



extern uint32_t pit_init();

extern void pit_handler();

uint32_t term_num;

pcb_t* sched_pcb;

#endif
