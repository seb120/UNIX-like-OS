#include "scheduler.h" 
#include "lib.h"

extern void loadPageDirectory(uint32_t*);


/*
 * pit_init()
 *	Function	-	Initialize the programmable interrupt timer
 *	Inputs		-	none
 *	Outputs		-	Return 0 for success
 *	Effects		-	Receive 10ms (100Hz) interrupts on IRQ0
*/
uint32_t pit_init()
{
	uint8_t freq_l, freq_h;
	term_num = 0;

	freq_l = (uint8_t) PIT_FREQ_SETTING;
	freq_h = (uint8_t) PIT_FREQ_SETTING >> 8;
    
    outb(PIT_MODE_SETTING, PIT_COMMAND_PORT);
    outb(freq_l, PIT_CH0_PORT);
    outb(freq_h, PIT_CH0_PORT);
	sched_pcb=0;
    return 0;
}


/*
 * pit_handler()
 *	Function	-	Handle PIT interrupt
 *	Inputs		-	none
 *	Outputs		-	none
 *	Effects		-	Performs scheduling operations via Round Robin Algorithm
*/
void pit_handler()
{
	
	/*sched_pcb = &pcbs[ terminal_info[term_num%3].top_process_id];
	
//printf("%d", sched_pcb->program_id );

	if ( sched_pcb->program_id == 0 || sched_pcb==current_pcb)
    {
    	term_num++;
    	send_eoi(PIT_IRQ_NUM);
     	return;
    }


    else
    {
    	 //printf("termina: %d id:%d\n", term_num%3,  terminal_info[term_num%3].top_process_id);
    	current_pcb->esp0=tss.esp0;
        //Update CR3 Paging
      
       asm volatile ("movl %%esp, %0;"
        	"movl %%ebp, %1;" 
        	:"=b" (current_pcb->esp), "=c" (current_pcb ->ebp));

        tss.esp0=sched_pcb->esp0;
        loadPageDirectory(&page_directories[ sched_pcb->program_id - 1 ].page_directory_entry[0]);
        asm volatile ("movl %0, %%esp;"
        	"movl %1, %%ebp;" 
        	:
        	:"b" (sched_pcb->esp), "c" (sched_pcb ->ebp));
  
    }

	term_num++;
	//current_pcb=sched_pcb;
*/
	// test_interrupts();
	send_eoi(PIT_IRQ_NUM);
   
	return;
    
} 
