/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "paging.h"
#include "idt.h"
#include "rtc.h"
#include "keyboard.h"
#include "terminal.h"
#include "filesystem.h"
#include "pcb.h"
 #include "syscall.h"

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

#define boot_delay	2000000
#define boot_height	27	//The height of the bootscreen image

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
entry (unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;
	int i;
	int j;

	/* Clear the screen. */
	clear();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%#x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%#x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%#x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	if (CHECK_FLAG (mbi->flags, 3)) {
		int mod_count = 0;
		int i;
		module_t* mod = (module_t*)mbi->mods_addr;
		while(mod_count < mbi->mods_count) {
			printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
			fs_start=mod->mod_start;
			printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
			printf("First few bytes of module:\n");
			for(i = 0; i<16; i++) {
				printf("0x%x ", *((char*)(mod->mod_start+i)));
			}
			printf("\n");
			mod_count++;
			mod++;
		} 
	}

	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

		printf ("elf_sec: num = %u, size = 0x%#x,"
				" addr = 0x%#x, shndx = 0x%#x\n",
				(unsigned) elf_sec->num, (unsigned) elf_sec->size,
				(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
					+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x,     base_addr = 0x%#x%#x\n"
					"     type = 0x%x,  length    = 0x%#x%#x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->type,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low);
	}

	/* Construct an LDT entry in the GDT */
	{
		seg_desc_t the_ldt_desc;
		the_ldt_desc.granularity    = 0;
		the_ldt_desc.opsize         = 1;
		the_ldt_desc.reserved       = 0;
		the_ldt_desc.avail          = 0;
		the_ldt_desc.present        = 1;
		the_ldt_desc.dpl            = 0x0;
		the_ldt_desc.sys            = 0;
		the_ldt_desc.type           = 0x2;

		SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
		ldt_desc_ptr = the_ldt_desc;
		lldt(KERNEL_LDT);
	}

	/* Construct a TSS entry in the GDT */
	{
		seg_desc_t the_tss_desc;
		the_tss_desc.granularity    = 0;
		the_tss_desc.opsize         = 0;
		the_tss_desc.reserved       = 0;
		the_tss_desc.avail          = 0;
		the_tss_desc.seg_lim_19_16  = TSS_SIZE & 0x000F0000;
		the_tss_desc.present        = 1;
		the_tss_desc.dpl            = 0x0;
		the_tss_desc.sys            = 0;
		the_tss_desc.type           = 0x9;
		the_tss_desc.seg_lim_15_00  = TSS_SIZE & 0x0000FFFF;

		SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

		tss_desc_ptr = the_tss_desc;

		tss.ldt_segment_selector = KERNEL_LDT;
		tss.ss0 = KERNEL_DS;
		tss.esp0 = 0x800000;
		ltr(KERNEL_TSS);
	}


	/* Initialize Paging */
	paging_init();
       
	/* Initialize the IDT */
	idt_init();

	/* Initialize the PIC */
	i8259_init();

	/* Initialize the filesystem */
	fs_init();

	/* Initialize the terminal */
	terminal_init();

	/* Initialize the RTC */
	rtc_init();

	/* Set RTC interrupt freq to 2Hz*/
	uint32_t buf12= 2;
	rtc_write(0, &buf12, 0);

	/* Unmask RTC interrupts */
	enable_irq(RTC_IRQ_NUM);

	/* Initialize the PIT */
	pit_init();

	/* Unmask the PIT interrupts */
	enable_irq(PIT_IRQ_NUM);

   	process_number = 0; //initial declaration for pcb processes 

	uint8_t* blankparam=(uint8_t*) "blank";    //just to fulfill function parameter requirements

	terminal_open(blankparam);
	clear_screen();
	printf("LOADING \n");
	printf("                                         ddddd                                 \n");
	printf("kkkk                                     d:::d     OOOOOOOO        SSSSSSSSSSS \n");
	printf("k::k                                     d:::d   OO::::::::OO    SS:::::::::::S\n");
	printf("k::k                                     d:::d OO::::::::::::OO S:::::SS::::::S\n");
	printf("k::k                                     d::d O:::::::OO:::::::OS:::::S SSSSSSS\n");
	printf("k::k    kkkkkkkuuuuu    uuuuu    ddddddddd::d O::::::O  O::::::OS:::::S        \n");
	printf("k::k   k:::::k u:::u    u:::u   d:::::::::::d O:::::O    O:::::OS:::::S        \n");
	printf("k::k  k:::::k  u:::u    u:::u  d::::::::::::d O:::::O    O:::::O S::::SS       \n");
	printf("k::k k:::::k   u:::u    u:::u d::::::ddddd::d O:::::O    O:::::O  SS::::SSS    \n");
	printf("k::k:::::k     u:::u    u:::u d:::::d    d::d O:::::O    O:::::O    SSS::::SS  \n");
	printf("k::::::::k     u:::u    u:::u d::::d     d::d O:::::O    O:::::O       SS::::S \n");
	printf("k::::::::k     u:::u    u:::u d::::d     d::d O:::::O    O:::::O        S:::::S\n");
	printf("k:::k:::::k    u::::uuuu::::u d::::d     d::d O::::::O  O::::::O        S:::::S\n");
	printf("k::k k:::::k   u:::::::::::::ud:::::ddddd:::ddO:::::::OO:::::::OSSSSSSS S:::::S\n");
	printf("k::k  k:::::k   u::::::::::::u d:::::::::::::d OO::::::::::::OO S::::::SS:::::S\n");
	printf("k::k   k:::::k   u::::::::u::u  d::::::::dd::d   OO::::::::OO   S:::::::::::SS \n");
	printf("kkkk    kkkkkkk   uuuuuuuu uuu  ddddddddd  ddd     OOOOOOOO      SSSSSSSSSSS   \n");
	printf("\n\nCreated by Dhruv Kanwal . Sebastian Jaronczyk . Ameer Kashani . Kyle Luhrsen\n");
	
	for(i=0; i<boot_height;i++)
	{
		for(j=0; j<boot_delay;j++)
		{	
			if(j%boot_delay==0)
			printf("=> ");	 		
		}
	}
	
	clear_screen();

	printf("Welcome to kudOS v.5!\n");
	
	/* Enable interrupts */
	sti();

	/* Display terminal text color selection menu */
	int8_t inputcolor[1];
    printf("choose your text color:\n");
    printf("Press 1:White  2:Red  3:Cyan  4:Yellow  5:Blue  6:Green\n");
    terminal_read(0, inputcolor,1);
    if(strncmp(inputcolor, "1", 1)==0)
            attrib = (0x0 << 4) | (0xF & 0x0F);
    else if(strncmp(inputcolor, "2", 1)==0)
            attrib = (0x0 << 4) | (0xC & 0x0F);
    else if(strncmp(inputcolor, "3", 1)==0)
            attrib = (0x0 << 4) | (0xB & 0x0F);
    else if(strncmp(inputcolor, "4", 1)==0)
            attrib = (0x0 << 4) | (0xE & 0x0F);
    else if(strncmp(inputcolor, "5", 1)==0)
            attrib = (0x0 << 4) | (0x9 & 0x0F);
    else if(strncmp(inputcolor, "6", 1)==0)
            attrib = (0x0 << 4) | (0xA & 0x0F);
    else 
            attrib = (0x0 << 4) | (0xF & 0x0F);
    
    /* Execute the primary shell */
	uint8_t* cmd = (uint8_t*)"shell";
	execute(cmd);

	/* Spin (nicely, so we don't chew up cycles) */
	asm volatile(".1: hlt; jmp .1;");
}

