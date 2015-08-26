/*	rtc.c - the functions for initializing and handling interrupts from the Real Time Clock
*
*    Helpful RTC docs:	http://ftp.lanet.lv/ftp/mirror/x2ftp/msdos/programming/serial/rtc
*						http://wiki.osdev.org/RTC
*/

#include "rtc.h"


/*
 *	rtc_init
 *		Function:	Initialize the RTC
 *		Inputs:		none
 *		Outputs:	none
 *		Effect:		Initializes the RTC controller by enabling periodic RTC interrupts
 */
void rtc_init()
{
	uint8_t saveB;

	rtc_sync_flag = 0;
	rtc_read_flag = 0;

	outb(REG_B_NMI,RTC_ADDRESS);
	saveB = inb(RTC_DATA);
	
	/*Enable the interrupts by setting register B*/
	outb(REG_B_NMI,RTC_ADDRESS);
	outb(saveB | REG_B_EN,RTC_DATA);

	rtc_ops.openptr=&rtc_open;
    rtc_ops.closeptr=&rtc_close;
    rtc_ops.readptr=&rtc_read;
    rtc_ops.writeptr=&rtc_write;
}


/*
 *	rtc_handler
 *		Function:	performs an interrupt action whenever an rtc interrupt is recieved
 *		Inputs:		none
 *		Outputs:	none
*/
void rtc_handler()
{
	asm volatile("pushal;");

	/*read the interrupt flag from register C*/
	send_eoi(RTC_IRQ_NUM);

	//Synchronization for the RTC read function
	if(rtc_read_flag)
		rtc_sync_flag = 1;

	// We must read Register C on RTC for it to send interrupt again
	outb(REG_C,RTC_ADDRESS);
	inb(RTC_DATA);
	
	//	test_interrupts();

	asm volatile ("popal;"
				"leave;"
				   "iret");
}


/*
 *	rtc_open
 *		Function:	Open the RTC
 *		Inputs:		none
 *		Outputs:	return 0 for success
 *		Effect:		Initializes the RTC controller to 2hz interrupt mode
 */
uint32_t rtc_open(const uint8_t* filename)
{
	uint8_t saveA;

	outb(REG_A, RTC_ADDRESS);
	saveA = inb(RTC_DATA);

	outb(REG_A, RTC_ADDRESS);
	outb(saveA | 0x0F, RTC_DATA);	//Sets freq to 2Hz

	return 0;
}


/*
 *	rtc_close
 *		Function:	Close the RTC
 *		Inputs:		none
 *		Outputs:	returns 0 for success
 *		Effect:		none
 */
uint32_t rtc_close(uint32_t fd)
{
	return 0; //success
}


/*
 *	rtc_read
 *		Function:	Read the RTC
 *		Inputs:		none
 *		Outputs:	none
 *		Effect:		Waits for an RTC interrupt before returning
 */
uint32_t rtc_read(uint32_t fd, void* buf, int32_t nbytes)
{	
	rtc_read_flag = 1;

	while (!rtc_sync_flag) { }

	rtc_sync_flag = 0;
	rtc_read_flag = 0;

	return 0; //success
}


/* 
 *	rtc_write
 *		Function:	Write the RTC
 *		Inputs:		fd - unused
					buf - the new freq to set the RTC IRQ rate
					nbytes - unused
 *		Outputs:	returns 0 for success, -1 for out of bound input
 *		Effect:		Rounds UP to the closest available power of 2 RTC interrupt frequency
 *					ie. new_freq = 550Hz will set RTC to 1024Hz.
 */
uint32_t rtc_write(uint32_t fd, const void * buf, int32_t nbytes)
{
	uint8_t rtc_freq;
	uint8_t saveA;
	uint32_t new_freq;

	uint32_t *new_buf= (uint32_t*) buf;
	new_freq = *new_buf;

	//OS should not allow more than 1024 interrupts per second
	if (new_freq > 1024)
		return -1;

	if (new_freq <= 1024 && new_freq > 512)
		rtc_freq = 0x06;	//1024

	else if (new_freq <= 512 && new_freq > 256)
		rtc_freq = 0x07;	//512

	else if (new_freq <= 256 && new_freq > 128)
		rtc_freq = 0x08;	//256

	else if (new_freq <= 128 && new_freq > 64)
		rtc_freq = 0x09;	//128

	else if (new_freq <= 64 && new_freq > 32)
		rtc_freq = 0x0A;	//64

	else if (new_freq <= 32 && new_freq > 16)
		rtc_freq = 0x0B;	//32

	else if (new_freq <= 16 && new_freq > 8)
		rtc_freq = 0x0C;	//16

	else if (new_freq <= 8 && new_freq > 4)
		rtc_freq = 0x0D;	//8

	else if (new_freq <= 4 && new_freq > 2)
		rtc_freq = 0x0E;	//4

	else rtc_freq = 0x0F;	//2	

	disable_irq(RTC_IRQ_NUM);

	outb(REG_A_NMI, RTC_ADDRESS);
	saveA = inb(RTC_DATA);

	outb(REG_A_NMI, RTC_ADDRESS);
	outb((saveA & HIGH_BITS) | rtc_freq, RTC_DATA);

	enable_irq(RTC_IRQ_NUM);

	return 0;
}



