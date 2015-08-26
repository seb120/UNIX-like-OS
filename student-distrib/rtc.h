/*rtc.h - functions and constants used for initializing and handling
*		the real time clock
*/

#ifndef _RTC_H
#define _RTC_H

#include "lib.h"
#include "filesystem.h" 
#include "i8259.h"

#define	RTC_ADDRESS 0x70	//The address used to write to the RTC
#define RTC_DATA	0x71	//The address used to read from the RTC

#define	REG_A		0x0A	//the offset to each of the rtc registers
#define	REG_B		0x0B
#define	REG_C		0x0C
#define	REG_D		0x0D

#define	REG_A_NMI	0x8A	//values to set the rtc registers to disable non-maskable interrups
#define	REG_B_NMI	0x8B
#define	REG_C_NMI	0x8C
#define	REG_D_NMI	0x8D

#define	REG_B_EN	0x40	//Enable interrupts setting for register B

#define RTC_IRQ_NUM	8

#define HIGH_BITS 	0xF0

volatile int rtc_sync_flag;
volatile int rtc_read_flag;

/*Initialize the RTC*/
void rtc_init();

/*handle any rtc interrupts*/
void rtc_handler();

// RTC OPEN
uint32_t rtc_open(const uint8_t* filename);

//RTC CLOSE
uint32_t rtc_close(uint32_t fd);

//RTC READ
uint32_t rtc_read(uint32_t fd, void* buf, int32_t nbytes);

//RTC WRITE
uint32_t rtc_write(uint32_t fd, const void * buf, int32_t nbytes);

#endif
