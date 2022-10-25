// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano; Edited by Natalia Perez
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

// The Digital-to-Analog Converter (DAC) cycles through
// an analog-equivalent wave of the approximated wave utilizing
// numerical values from an array to emit sound. These values are
// copied and written to the DAC_Out function.

#include "DAC.h"
#include "..//tm4c123gh6pm.h"
	
// **************DAC_Init*********************
// Initialize 4-bit DAC (PB3-PB0)
// Input: none
// Output: none
void DAC_Init(void){ unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x02; 								// Activate Port B 0x02
	delay = SYSCTL_RCGC2_R; 								// Allow time to finish activating
	GPIO_PORTB_AMSEL_R &= ~0x0F; 						// No analog on PB3-PB0
	GPIO_PORTB_PCTL_R &= ~0x0000FFFF; 			// Regular GPIO function
	GPIO_PORTB_DIR_R |= 0x0F; 							// Make PB3-PB0 outputs
	GPIO_PORTB_AFSEL_R &= ~0x0F; 						// Disable alternate function on PB3-PB0
	GPIO_PORTB_DEN_R |= 0x0F;								// Enable digital I/O on PB3-PB0
	GPIO_PORTB_DR8R_R |= 0x0F;
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTB_DATA_R = data;
}
