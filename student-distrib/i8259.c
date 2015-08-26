/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"


/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */


/*
*	i8259_init
*		Function:	Initialize the 8259 PIC
*		Inputs:		none
*		Outputs:	none
*		Effect:		Send initial control words to PIC for initialization
*/
void i8259_init(void)
{
	// Send INIT Control Word to PIC
	outb(ICW1, MASTER_8259_PORT);
	outb(ICW1, SLAVE_8259_PORT);

	// Map IRQ0-7 to 0x20 to 0x27
	// Map IRQ8-15 to 0x28 to 0x36
	outb(ICW2_MASTER, MASTER_DATA);
	outb(ICW2_SLAVE, SLAVE_DATA);

	// Connect PICs using IRQ#2
	outb(ICW3_MASTER, MASTER_DATA);
	outb(ICW3_SLAVE, SLAVE_DATA);

	// Set final control word for 80x86 mode
	outb(ICW4, MASTER_DATA);
	outb(ICW4, SLAVE_DATA);

	// Clear all interrupts except IRQ2, which is our cascade
	outb(MASTER_MASK_ALL, MASTER_DATA);
	outb(SLAVE_MASK_ALL, SLAVE_DATA);
}


/*
*	enable_irq
*		Function:	enables specific irq
*		Inputs:		irq_num -- specific irq
*		Outputs:	none
*		Effect:		Enable (unmask) the specified IRQ
*/
void enable_irq(uint32_t irq_num)
{
	// Enables corresponding bit in register for given IRQ mask
	if (irq_num < MAX_IRQ)
	{
		// chosen IRQ was on master
		master_mask = inb(MASTER_DATA) & ~(1 << irq_num);
		outb(master_mask, MASTER_DATA);
	}	
	else
	{
		// chosen IRQ was on slave
		irq_num -= MAX_IRQ;
		slave_mask = inb(SLAVE_DATA) & ~(1 << irq_num);
		outb(slave_mask, SLAVE_DATA);
	}
}


/* Disable (mask) the specified IRQ */
/*
*	disable_irq
*		Function:	disables specific irq
*		Inputs:		irq_num -- specific irq
*		Outputs:	none
*		Effect:		disable (unmask) the specified IRQ
*/
void disable_irq(uint32_t irq_num)
{
	// Disables corresponding bit in register for given IRQ mask
	if (irq_num < MAX_IRQ)
	{
		// chosen IRQ was on master
		master_mask = inb(MASTER_DATA) | (1 << irq_num);
		outb(master_mask, MASTER_DATA);
	}	
	else
	{
		// chosen IRQ was on slave
		irq_num -= MAX_IRQ;
		slave_mask = inb(SLAVE_DATA) | (1 << irq_num);
		outb(slave_mask, SLAVE_DATA);
	}
}


/*
*	send_eoi(uint32_t irq_num)
*		Function:	Send end-of-interrupt signal for the specified IRQ
*		Inputs:		irq_num -- specific irq
*		Outputs:	none
*		Effect:		Send end-of-interrupt signal for the specified IRQ
*/
void send_eoi(uint32_t irq_num)
{
	if (irq_num >= MAX_IRQ)
	{
		irq_num -= MAX_IRQ;
		outb(EOI | irq_num, SLAVE_8259_PORT);
		outb(EOI | SLAVE_IRQ, MASTER_8259_PORT);	//IRQ2 should sent to MASTER
	}
	else
		outb(EOI | irq_num, MASTER_8259_PORT);
}
