/*	keyboard.h
*/

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "syscall.h"

/* Size of the keyboard buffer */
#define BUFFER_MAX_SIZE 	128

/* Ports for keyboard */
#define KEYBOARD_DATA_PORT	0x60
#define KEYBOARD_CMD_PORT	0x64
#define KEYBOARD_IRQ_NUM	1

#define KEYBOARD_ACK		0xFA	//Keyboard response for an acknowledge

#define SET_SCAN			0xF0	//Command to set the scan code to code 1
#define	SCAN_1				0x01
#define SCAN_EN				0xF4	//Command to enable keyboard scanning

#define SCANCODE_END		0x3A	//The size of the keys array

#define KEY_RELEASE 		0x80

/* Definitions for keypress and release scancodes */
#define BACKSPACE 			0x0E
#define L_CTRL 				0x1D
#define L_CTRL_RELEASE		0x9D
#define L_BUTTON			0x26
#define C_BUTTON			0x2E

#define LSHIFT				0x2A
#define RSHIFT				0x36
#define LSHIFT_RELEASE		0xAA
#define RSHIFT_RELEASE		0xB6
#define CAPLOCK				0x3A

#define L_ALT 				0x38
#define L_ALT_RELEASE		0xB8
#define KEY_F1				0x3B
#define KEY_F2				0x3C
#define KEY_F3				0x3D


volatile int8_t enter_flag;
int ctrl_flag;
int cap_flag;
int alt_flag;


/*A function to initialize the keyboard*/
void keyboard_init();

/*A function to handle the keyboard interrupts*/
void keyboard_handler();

#endif
