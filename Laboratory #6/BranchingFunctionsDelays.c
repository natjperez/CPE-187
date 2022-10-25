// BranchingFunctionsDelays.c Lab 6
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to 
// toggle an LED while a button is pressed and 
// turn the LED on when the button is released.  
// This lab will use the hardware already built into the LaunchPad.
// Daniel Valvano, Jonathan Valvano; Edited by Natalia Perez
// January 15, 2016

// built-in connection: PF0 connected to negative logic momentary switch, SW2
// built-in connection: PF1 connected to red LED
// built-in connection: PF2 connected to blue LED
// built-in connection: PF3 connected to green LED
// built-in connection: PF4 connected to negative logic momentary switch, SW1

#include "TExaS.h"

#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))

// Global Variables
unsigned long SW1;

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortF_Init(void);	// Initialize PortF
void Delay100ms(unsigned long time);	// Create dirty delay

int main(void){ unsigned long volatile delay;
  TExaS_Init(SW_PIN_PF4, LED_PIN_PF2);  // activate grader and set system clock to 80 MHz
	PortF_Init();	// initialization goes here
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
    // body goes here
		SW1 = GPIO_PORTF_DATA_R&0x10;	// Read value of PF4 (SW1)
		Delay100ms(1);	// 100ms

		if(!SW1)	// If pressed
		{
			GPIO_PORTF_DATA_R ^= 0x04; // toggle
		}
		else		// If unpressed
		{
			GPIO_PORTF_DATA_R = 0x04;	// keep PF2 LED on
		}
  }
}

void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function	| clear PF4/PF2
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  | clear PF4/PF2 for GPIO
  GPIO_PORTF_DIR_R = 0x04;          // 5) PF4 input, PF2 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function	| clear PF4/Pf2 to disable AF
  GPIO_PORTF_PUR_R = 0x10;          // enable pullup resistors on PF4      
  GPIO_PORTF_DEN_R = 0x14;          // 7) enable digital pins PF4 and PF2
	GPIO_PORTF_DATA_R |= 0x04;					// LED initially on
}
// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06

// Function used for delay
// Inputs: time
// Outputs: (delay)
// Notes: ...
void Delay100ms(unsigned long time){
  unsigned long i;
  while(time > 0){
		i = 1333333; 	// this number means 100ms
		while(i > 0) {
			i = i - 1;
		}
		time = time - 1;	// decrements every 100ms
  }
}
