#include "keyboard.h"

/* Indexing this array with a hex scan code will give
*  the corresponding 8-bit LOWERCASE char keycode
*/
static unsigned char keys[] = {
	'\0',  // Reserved first char in array
		   // so indexing with scancode does not require an offset

	'\0',	//Escape

	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',

	 '\0',  //Backspace
	 '\0',  //Tab

	 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',

	'\n',  //Enter
	'\0',  //Left CTRL

	'a', 's','d', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 

	'\0',  //Left Shift

	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 

	'\0',  //Right Shift
	'\0',  //(keypad) *
	'\0',  //Left ALT

	 ' ',  //SPACE

	 '\0'  //CAPSLOCK
};

/* Indexing this array with a hex scan code will give
*  the corresponding 8-bit UPPERCASE char keycode
*/
static unsigned char cap_keys[] = {
	'\0',  // Reserved first char in array
		   // so indexing with scancode does not require an offset

	'\0',	//Escape

	'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',

	 '\0',  //Backspace
	 '\0',  //Tab

	 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',

	'\n',  //Enter
	'\0',  //Left CTRL

	'A', 'S','D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 

	'\0',  //Left Shift

	'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 

	'\0',  //Right Shift
	'\0',  //(keypad) *
	'\0',  //Left ALT

	 ' ',  //SPACE

	 '\0'  //CAPSLOCK
};


/*
*	keyboard_init
*		Function:	Initialize the keyboard
*		Inputs:		none
*		Outputs:	none
*		Effect:		Sets the keyboard to use scan code 1 mode and start sending
*					scan codes to the PIC
*/
void keyboard_init(){
	enter_flag = 0;
	enable_irq(KEYBOARD_IRQ_NUM);
	ctrl_flag = 0;
	cap_flag = 0;
	alt_flag = 0;
}

/*
*	keyboard_handler
*		Function:	Outputs the symbol of a pressed key to the screen
*		Inputs:		none
*		Outputs:	none
*		Effect:		Sends the pressed key (lowercase letters or numbers) to the screen
*/
void keyboard_handler(){
	uint8_t	scan_code;	//the key sent in by the keyboard

	cli();
	scan_code = inb(KEYBOARD_DATA_PORT);	//get the key value

	if (scan_code <= KEY_RELEASE)
	{
	    if(keys[scan_code] == '\n')
	    {
	    	/* //MP3.2 Terminal TEST
	    	//uint8_t a=(uint8_t)5;
	        unsigned char buf[128] = { 0 };
			int32_t count = terminal_read(a,buf,128);
			terminal_write(a, buf, count);
			*/

			add_to_buffer('\n');
			enter_flag = 1;
			
	    }
	    else if(scan_code == BACKSPACE)
	    {
	    	backspace();
	    	print_buffer();
	    }
	    else if(scan_code == L_CTRL)
	    	ctrl_flag = 1;
	    else if(scan_code == L_ALT)
	    {
	    	alt_flag = 1;
	    }
	    else if ( (alt_flag == 1) && (scan_code == KEY_F1) )
	    {
	    	//switch to first terminal
	    	switch_terminal(TERMINAL_1);
	    }
  	    else if ( (alt_flag == 1) && (scan_code == KEY_F2) )
  	    {
  	    	//switch to second terminal
			switch_terminal(TERMINAL_2);
  	    }
	    else if ( (alt_flag == 1) && (scan_code == KEY_F3) )
		{
			//switch to third terminal
			switch_terminal(TERMINAL_3);
		}
	    else if((ctrl_flag == 1) && (scan_code == L_BUTTON))
	    {
	    	clear_screen();
	    	print_buffer();
		}
		else if ((ctrl_flag == 1) && (scan_code == C_BUTTON))
		{
			uint8_t status = 0;
			send_eoi(KEYBOARD_IRQ_NUM);
			halt(status);
		}
		else if(scan_code == LSHIFT || scan_code == RSHIFT)
			cap_flag = !cap_flag;
		else if(scan_code == CAPLOCK)
			cap_flag = !cap_flag;
	    else if(cap_flag)
		{
			add_to_buffer(cap_keys[scan_code]);
			print_buffer();
		}
		else
		{
			add_to_buffer(keys[scan_code]);
			print_buffer();
		}
	}
	else if(scan_code == L_CTRL_RELEASE)
	    	ctrl_flag = 0;
	else if(scan_code == LSHIFT_RELEASE || scan_code == RSHIFT_RELEASE)
		cap_flag = !cap_flag;
	else if(scan_code == L_ALT_RELEASE)
		alt_flag = !alt_flag;

	send_eoi(KEYBOARD_IRQ_NUM);
	sti();
}
