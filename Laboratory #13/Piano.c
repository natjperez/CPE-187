// Piano.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano; Edited by Natalia Perez
// December 29, 2014

// Port E bits 3-0 have 4 piano keys

// After the initialization process, the Piano function work
// to retrieve inputs from Port E (key inputs), which translate
// into sound once the Sound_Tone() subroutine is called. 

#include "Piano.h"
#include "..//tm4c123gh6pm.h"

// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void){ unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x10; 								// Activate Port E
	delay = SYSCTL_RCGC2_R; 								// Allow time to finish activating
	GPIO_PORTE_AMSEL_R &= ~0x0F;						// No analog on PE3-PE0
	GPIO_PORTE_PCTL_R &= ~0x0000FFFF;				// Regular GPIO function
	GPIO_PORTE_DIR_R &= ~0x0F;							// Make PE3-PE0 inputs
	GPIO_PORTE_AFSEL_R &= ~0x0F;						// Disable alternate function on PE3-PE0
	GPIO_PORTE_DEN_R |= 0x0F;								// Enable digital I/O on PE3-PE0
}

// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void){
  return GPIO_PORTE_DATA_R&0x0F; // remove this (0), replace with input
}
