// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano; Edited by Natalia Perez
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "TExaS.h"
#include "..//tm4c123gh6pm.h"

#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *)0xE000ED20))
#define NVIC_ST_CTRL_R 					(*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    		(*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   		(*((volatile unsigned long *)0xE000E018))

#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define GPIO_PORTA_DR8R_R       (*((volatile unsigned long *)0x40004508))

// Global Variables
unsigned long Input;	//PA3
unsigned long Output; //PA2
unsigned long Previous;

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
void Sound_Init(void);
void SysTick_Handler(void);

// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void){ unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000001; //Activate clock for Port A
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTA_AMSEL_R &= ~0x04; //Disable analog function
	GPIO_PORTA_PCTL_R &= 0xFFFF00FF; //Regular function (PMC2)
	GPIO_PORTA_DIR_R |= 0x04; //Make PA2 an output
	GPIO_PORTA_DR8R_R |= 0x04; //Drive up to 8mA out
	GPIO_PORTA_AFSEL_R &= ~0x0C; //Disable alternate function for PA2
	GPIO_PORTA_DEN_R |= 0x0C; //Enable digital I/O on PA2
	
	NVIC_ST_CTRL_R = 0; //Disable SysTick during setup
	NVIC_ST_RELOAD_R = 90908; //Reload value
	NVIC_ST_CURRENT_R = 0; //Any write to CURRENT clears it
	NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; //Priority 0
	NVIC_ST_CTRL_R = 0x00000007; //Enable with core clock and interrupts
	EnableInterrupts();
}

// called at 880 Hz
void SysTick_Handler(void) {
	if(Output) {
		GPIO_PORTA_DATA_R ^= 0x04; //Toggle
	}
	else {
		GPIO_PORTA_DATA_R &= ~0x04; //Clear
	}
}

int main(void){// activate grader and set system clock to 80 MHz
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();
  EnableInterrupts();   // enable after all initialization are done
	Output = 0;
	Previous = 0;
  while(1){
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
		Input = GPIO_PORTA_DATA_R&0x08; //Positive logic switch
		//2 input presses = output low/not toggling
		//1 input press = output toggling
		if(Input) {
			if(Previous != Input) {
				Output ^= 1;	//If different (0 and 1), output will be 1. If same (1 and 1, or 0 and 0, output will be 0)
				Previous = Input; //Copy input into previous
			}
		}
		else {
			if(Previous != Input) {
				Previous = Input; //Copy input into previous
			}
		}
  }
}
