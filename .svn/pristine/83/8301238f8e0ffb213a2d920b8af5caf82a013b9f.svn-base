#include "terminal.h"


uint16_t* terminal_ptr;
extern void loadPageDirectory(uint32_t*);


/*
 *  move_csr()
 *      Function:   Computes linear cursor index and blinks at Video Mem location
 *      Inputs:     none
 *      Outputs:    none
 *      Effect:     Flashes cursor at current (x,y) location
*/
void move_csr(void)
{
    unsigned cursor_location;

    /* find cursor location depeingding on its x and y coor */
    cursor_location = csr_y[curr_terminal] * WIDTH + csr_x[curr_terminal];

    /* Start cursor blinking at specified locaton */
    outb(UNDERLINE_LOC, INDEX_PORT);
    outb(cursor_location >> 8, WRITE_PORT);
    outb(START_BLINK, INDEX_PORT);
    outb(cursor_location, WRITE_PORT);
}


/*
 *  switch_terminal()
 *      Function:   Switches terminal to given number
 *      Inputs:     terminal_num, the new terminal to swithc to
 *      Outputs:    none
 *      Effect:     changes the current active terminal by swapping the memory of the old/active one
*/
int32_t switch_terminal(uint32_t terminal_num)
{
    uint8_t* cmd = (uint8_t*)"shell";

    //don't do anything if we try to switch to the same terminal
    if (terminal_num == curr_terminal)
    {
        return 0;
    }

    //copy memory from current terminal vidmem to saved physical block (for curr)
    memcpy( (uint8_t *) terminal_memory[curr_terminal], (uint8_t *) terminal_ptr, BLOCK_SIZE );

    //copy memory from terminal_num vidmem to physical B8000
    memcpy( (uint8_t *)terminal_ptr, (uint8_t *) terminal_memory[terminal_num], BLOCK_SIZE );

    // Update the curr_terminal (currently active)
    curr_terminal = terminal_num;
    
    //Update cursor location
    move_csr();
    
    //Update curr_pcb from 
    current_pcb = &pcbs[terminal_info[curr_terminal].top_process_id];

    //Start shell if there is no process in the new terminal
    if ( current_pcb->program_id == 0 )
    {
        execute(cmd);
    }
    else
    {
        //Update CR3 Paging
        loadPageDirectory(&page_directories[ current_pcb->program_id - 1 ].page_directory_entry[0]);

        //Update TSS (Flush TLB)
        (&tss)->esp0=current_pcb->address;
    }

    return 0;
}


/*
 *  scroll()
 *      Function:   Scrolls the current terminal up by one line
 *      Inputs:     none
 *      Outputs:    none
 *      Effect:     Changes the current cursor position and copies Video Memory Line 1 to Line 0, etc.
 *                  to shift text window upwards
*/
void scroll(void)
{
    unsigned blank, temp;
    int i;
    int j;
    uint16_t* temporary;
    uint16_t* t_ptr;

    /* black barground*/
    blank = BLANK_SPACE | (attrib << 8);
    cli();
    /* Row 25 is the end, this means we need to scroll up */
    if(csr_y[curr_terminal] >= HEIGHT)
    {
        /* Move the current text chunk that makes up the screen
        *  back in the buffer by a line */
        temp = 1;
        for(i = 0; i < HEIGHT - 1; i++){
            for(j = 0; j < WIDTH; j++){
                t_ptr = (terminal_ptr + (i * WIDTH + j));
                temporary = (terminal_ptr + (((i+1) * WIDTH) + j));
                *t_ptr = *temporary;
            }
        }
        for(j = 0; j < WIDTH; j++){
            t_ptr = (terminal_ptr + ((HEIGHT-1) * WIDTH + j));
            *t_ptr = blank;
        }
        //memcpy (terminal_ptr, terminal_ptr + temp * WIDTH, (HEIGHT - temp) * WIDTH * 2);
        /* Set the last line of text to 'blank' */
        //memset_word (terminal_ptr + (HEIGHT - temp) * WIDTH, blank, WIDTH);
        csr_y[curr_terminal] = HEIGHT - 1;
        line_y[curr_terminal] = csr_y[curr_terminal] -1;
        csr_x[curr_terminal] = 0;
    }
    sti();
}


/*
 *  new_line(TRUE/FALSE)
 *      Function:   Can be called to make a new line on the terminal.
 *      Inputs:     reset_buffer, TRUE/FALSE that tells whether or not to also clear the buffer.
 *      Outputs:    none
 *      Effect:     Moves the current location to the beginning of the next line.
*/
void new_line(unsigned char reset_buffer)
{
    csr_y[curr_terminal]++;
    csr_x[curr_terminal] = 0;
    scroll();
    move_csr();
    line_y[curr_terminal] = csr_y[curr_terminal];
    if (reset_buffer)
        clear_buffer();
}


/*
 *  add_to_buffer()
 *      Function:   Adds a character to the terminal buffer
 *      Inputs:     c, the character to be added.
 *      Outputs:    none
 *      Effect:     The character is added to the end of buffer.
*/
void add_to_buffer(unsigned char c)
{
    if(buffer_len[curr_terminal] < BUFF_LEN)
    {
        terminal_buffer[curr_terminal][buffer_len[curr_terminal]] = c;
        buffer_len[curr_terminal]++;
    }
    return;
}


/*
 *  clear_buffer()
 *      Function:   Empties the current terminal buffer
 *      Inputs:     none
 *      Outputs:    none
 *      Effect:     Fills the terminal buffer array with 0's.
*/
void clear_buffer()
{
    int i;

    for(i = 0; i < BUFF_LEN; i++)
    {
        terminal_buffer[curr_terminal][i] = 0;
    }
    buffer_len[curr_terminal] = 0;
}


/*
 *  clear_screen()
 *      Function:   Clears the current terminal.
 *      Inputs:     none
 *      Outputs:    none
 *      Effect:     Sets the entire 80x25 Video Memory array to blanks
 *                  while also changing the cursor position back to origin.
*/
void clear_screen()
{
    unsigned blank;
    int i;

    /* black background grey letters*/
    blank = BLANK_SPACE | (attrib << 8);

    /* clears the screen to set color */
    for(i = 0; i < HEIGHT; i++)
        memset_word (terminal_ptr + (i * WIDTH), blank, WIDTH);

    clear_buffer();

    /* Update cursor */
    csr_x[curr_terminal] = 0;
    csr_y[curr_terminal] = 0;
    line_y[curr_terminal] = 0;
    move_csr();
}


/*
 *  putch(c)
 *      Function:   Computes current cursor position index and writes character.
 *      Inputs:     c, character to be written to VGA text mode display.
 *      Outputs:    none
 *      Effect:     Tracks the cursor position to make sure the character is written to the correct place
 *                  by erasing for backspace and jumping the the next line when it becomes full.
*/
void putch(unsigned char c)
{
    unsigned short *where;
    unsigned att = attrib << 8;

    if(c=='\n')
    {
       new_line(TRUE);
    }
    /* backspace, move cursor back one space */
    else if(c == ASCII_BACKSPACE)
    {
        if(csr_x[curr_terminal] != 0) csr_x[curr_terminal]--;
    }
    /* Print characters to buffer*/
    else if(c >= ' ')
    {
        if((csr_y[curr_terminal] * WIDTH + csr_x[curr_terminal]) >= PAGE_SIZE_4KB)
            return;

        where = terminal_ptr + (csr_y[curr_terminal] * WIDTH + csr_x[curr_terminal]);
        if((uint32_t)where >= (VIDEO_MEM + PAGE_SIZE_4KB) || (uint32_t)where < VIDEO_MEM )
            return;
        *where = c | att;   /* Character AND attributes: color */
        csr_x[curr_terminal]++;
    }
    else if(c == NULL){
        return;
    }
    /* if cursor at edge, go to next line and put on left side */
    if(csr_x[curr_terminal] >= WIDTH)
    {
        csr_x[curr_terminal] = 0;
        csr_y[curr_terminal]++;
    }
    /* Scroll the screen if needed, and move the cursor */
    scroll();
    move_csr();
}


/*
 *  print_buffer()
 *      Function:   Prints the current terminal buffer to the VGA text mode display.
 *      Inputs:     none
 *      Outputs:    none
 *      Effect:     Goes to the beginning of the current line and starts printing from there.
*/
void print_buffer()
{
    int i;

    /*go to the begining of the current line*/
    csr_y[curr_terminal] = line_y[curr_terminal];
    csr_x[curr_terminal] = line_beginning[curr_terminal];
    /*print out the whole buffer*/
    for(i = 0; i < buffer_len[curr_terminal]; i++)
    {
        putch(terminal_buffer[curr_terminal][i]);
    }
}


/*
 *  backspace()
 *      Function:   Backspace function for handling user backspace input
 *      Inputs:     none
 *      Outputs:    none
 *      Effect:     Erases the last character in the buffer.
*/
void backspace()
{
    if(buffer_len[curr_terminal] > 0)
    {
        terminal_buffer[curr_terminal][buffer_len[curr_terminal]-1] = ' ';
        print_buffer();
        buffer_len[curr_terminal]--;
        move_csr();
    }
}


/*
 *  terminal_write(fd, buf, count)
 *      Function:   Writes a given buffer to the terminal.
 *      Inputs:     fd- not used
 *                  buf, pointer to buffer to be written to screen
                    fd - unused
 *      Outputs:    Returns number of characters written to screen.
 *      Effect:     Writes buffer to screen and moves to next line.
*/
uint32_t terminal_write(uint32_t fd, const void * buf, int32_t count)
{
    if(buf == NULL)
        return -1;
    unsigned char * terminal_buf= (unsigned char*) buf;
    int i;
    int bytes_written = 0;

    /*print out buffer, increment the bytes wrote*/
    for (i = 0; i < count; i++) {
        putch(terminal_buf[i]);
        bytes_written++;
    }
    line_beginning[curr_terminal] = csr_x[curr_terminal];
    line_y[curr_terminal] = csr_y[curr_terminal];
    move_csr();
    /*go to next line and move cursor*/
    /* Return the number of bytes printed. */
    return bytes_written;
}


/*
 *  terminal_read(fd, buf, count)
 *      Function:   Read into a given buffer for the terminal's buffer.
 *      Inputs:     fd- not used 
 *                  buf, pointer to the buffer that is to be filled.
 *      Outputs:    Returns the number of characters that were read.
 *      Effect:     buf parameter is filled with the terminal's buffer.
*/
uint32_t terminal_read(uint32_t fd, void * buf, int32_t count)
{
    while(!enter_flag){}
    int i=0;
    int bytes_read = 0;

    unsigned char * terminal_buf= (unsigned char*) buf;
    for (i = 0; (i < count) && (terminal_buffer[curr_terminal][i] != '\0') && (i < BUFFER_MAX_SIZE) && (terminal_buffer[curr_terminal][i] != '\n') ; i++)
    {
        terminal_buf[i] = terminal_buffer[curr_terminal][i];
        bytes_read++;
    }
    terminal_buf[i] = '\n';
    bytes_read++;
    //new_line(TRUE);
    //printf("count: %d read: %d\n",count,bytes_read );
    //terminal_write(0, buf, bytes_read);
    new_line(TRUE);
    enter_flag = 0;
    return bytes_read;
}


/*
 *  terminal_close(fd)
 *      Function:   Close the terminal
 *      Inputs:     fd-not used
 *      Outputs:    none
 *      Effect:     none
*/
uint32_t terminal_close(uint32_t fd)
{
    return 0;
}


/*
 *  terminal_init()
 *      Function:   Initializes the terminals
 *      Inputs:     none
 *      Outputs:    none
 *      Effect:     Sets the function pointers and clears the terminal process tracking struct
*/
void  terminal_init()
{
    int i = 0;

    terminal_ops.openptr=&terminal_open;
    terminal_ops.closeptr=&terminal_close;
    terminal_ops.readptr=&terminal_read;
    terminal_ops.writeptr=&terminal_write;

    stdin_ops.openptr=&terminal_open;
    stdin_ops.closeptr=&terminal_close;
    stdin_ops.readptr=&terminal_read;
    stdin_ops.writeptr=NULL;

    stdout_ops.openptr=&terminal_open;
    stdout_ops.closeptr=&terminal_close;
    stdout_ops.readptr=NULL;
    stdout_ops.writeptr=&terminal_write;

    //Clear the terminal info struct
    for(i = 0; i < MAX_TERMINALS; i++)
    {
        terminal_info[i].top_process_id = 0;
        terminal_info[i].num_process = 0;
    }
}


/*  
 *  terminal_open(fd)
 *      Function:   Opens the terminal
 *      Inputs:     fd-not used
 *      Outputs:    none
 *      Effect:     initializes video memory pointer and cursor position.
 *                  Also clears the memory of the inactive terminals
*/
uint32_t terminal_open(const uint8_t* filename)
{
    int i;
    unsigned blank;
   

    attrib = (0x0 << 4) | (0xF & 0x0F);
   
    /* Enable keyboard interrupt */
    keyboard_init();
    terminal_ptr = (uint16_t*) VIDEO_MEM;
    curr_terminal = 0;


    terminal_memory[TERMINAL_1] = TERMINAL_MEM_1;
    terminal_memory[TERMINAL_2] = TERMINAL_MEM_2;
    terminal_memory[TERMINAL_3] = TERMINAL_MEM_3;

    //Zero all the cursor/line locations in each terminal
    for(i = 0; i < MAX_TERMINALS; i++){
        line_beginning[i] = 0;
        csr_x[i] = 0;
        csr_y[i] = 0;
        line_y[i] = 0;
    }
    clear_screen();


    /* black barground grey letters*/
    blank = BLANK_SPACE | (attrib << 8);

    memset_word((uint8_t*)TERMINAL_MEM_2, blank,BLOCK_SIZE/2);

    memset_word((uint8_t*)TERMINAL_MEM_3, blank,BLOCK_SIZE/2);


    return 0;
}
