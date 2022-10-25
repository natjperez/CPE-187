// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano; Edited by Natalia Perez
// December 29, 2014
// This routine calls the 4-bit DAC

// By utilizing the array values in SineWave[16] and the specified period
// passed into Sound_Tone(), the Sound_Handler() is able to cycle through
// the numerical values and output a wave using DAC_Out().

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

//{4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3}
const unsigned long SineWave[16] = {8,11,13,14,15,14,13,11,8,5,3,2,1,2,3,5}; 
unsigned long Index;

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
  DAC_Init(); 																								// Port B is DAC
	Index = 0;
	NVIC_ST_CTRL_R = 0; 																				// Disable SysTick during setup
	NVIC_ST_RELOAD_R = 0; 																			// Reload value
	NVIC_ST_CURRENT_R = 0; 																			// Any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; 	// Priority 1
	NVIC_ST_CTRL_R = 0x07; 																			// Enable, core clock, and interrupts
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period - 1;
	NVIC_ST_CTRL_R = 0x07;
}

// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
	// this routine stops the sound output
	GPIO_PORTB_DATA_R &= ~0x0F;
	NVIC_ST_RELOAD_R = 0;
	DAC_Out(0);
}

// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	Index = (Index + 1)&0x0F; 
  DAC_Out(SineWave[Index]); 
}
