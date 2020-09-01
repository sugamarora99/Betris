// dac.c
// This software configures DAC output
// Lab 6 requires a minimum of 4 bits for the DAC, but you could have 5 or 6 bits
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 1/17/2020 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x22; // heartbeat
		volatile int delay;
		delay++;
	
		GPIO_PORTB_DIR_R |= 0x3F; // port B is output for DAC
		GPIO_PORTB_DEN_R |= 0x3F;
	
		GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
		GPIO_PORTF_CR_R |= 0xFF;
		GPIO_PORTF_PUR_R |= 0x11; //unlock PORTF PIN 4
	
		GPIO_PORTF_DIR_R |= 0x04; // PF4 is negative logic on board switch, PF2 is on board LED
		GPIO_PORTF_DIR_R &= ~0x10;
		GPIO_PORTF_DEN_R |= 0x14;
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Input=n is converted to n*3.3V/15
// Output: none
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R = data;
}
