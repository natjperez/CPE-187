// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional. ; Edited by Natalia Perez
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

// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE2(Ain1) and PD3
// Slide pot pin 1 connected to ground
#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"

void EnableInterrupts(void);  // Enable interrupts
void SysTick_Init(unsigned long period);
void SysTick_Handler(void);
void PortF_Init(void);

unsigned char String[10]; // null-terminated ASCII string
unsigned long Distance;   // units 0.001 cm
unsigned long ADCdata;    // 12-bit 0 to 4095 sample
unsigned long Flag;       // 1 means valid Distance, 0 means Distance is empty

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
  unsigned long calculation = (2000 * sample)/4095;
	return calculation;  // replace this line with real code
}

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0;																					//1) Disable SysTick during setup
	NVIC_ST_RELOAD_R = period - 1;															//2) Reload value
	NVIC_ST_CURRENT_R = 0;																			//3) Any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000;	//4) Priority 1
	NVIC_ST_CTRL_R = 0x07;																			//5) Enable, core clock, and interrupts
}
// executes every 25 ms, collects a sample, converts and stores in mailbox (ISR)
void SysTick_Handler(void){ 
	GPIO_PORTF_DATA_R ^= 0x02; 			//1) Toggle PF1
	GPIO_PORTF_DATA_R ^= 0x02;			//2) Toggle PF1 again
	ADCdata = ADC0_In(); 						//3) Sample ADC, calling your ADC0_In();
	Distance = Convert(ADCdata) + 1;//4) Convert the sample to distance, calling your Convert(), and storing the result into the global (+1 to receive accurate data)
	Flag = 1;												//5) Set the Flag, signifying the new data is ready
	GPIO_PORTF_DATA_R ^= 0x02;			//6) Toggle PF1 a third time
}

//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n){
// as part of Lab 11 you implemented this function
	if(n <= 9999) {
		String[0] = (n/1000);
		String[1] = '.';
		String[2] = ((n/100)%10);
		String[3] = ((n/10)%10);
		String[4] = (n%10);
		String[5] = ' ';
		String[6] = 'c';
		String[7] = 'm';
		String[8] = '0';
	}
	else {
		String[0] = '*';
		String[1] = '.';
		String[2] = '*';
		String[3] = '*';
		String[4] = '*';
		String[5] = ' ';
		String[6] = 'c';
		String[7] = 'm';
		String[8] = '0';
	}
}

// main1 is a simple main program allowing you to debug the ADC interface
int main1(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_NoScope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
  }
}
// once the ADC is operational, you can use main2 to debug the convert to distance
int main2(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  Nokia5110_Init();             // initialize Nokia5110 LCD
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
    Nokia5110_SetCursor(0, 0);
    Distance = Convert(ADCdata);
    UART_ConvertDistance(Distance); // from Lab 11
    Nokia5110_OutString(String);    // output to Nokia5110 LCD (optional)
  }
}
// once the ADC and convert to distance functions are operational,
// you should use this main to build the final solution with interrupts and mailbox
int main(void){ 
  volatile unsigned long delay;
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
	ADC0_Init(); // initialize ADC0, channel 1, sequencer 3
	PortF_Init();
	// initialize Nokia5110 LCD (optional)
	SysTick_Init(2000000); // initialize SysTick for 40 Hz interrupts
	Nokia5110_Init();
  //    wait for clock to stabilize
  EnableInterrupts();
  while(1){ 
		if(Flag) {
			// read mailbox
			ADCdata = ADC0_In();
			Distance = Convert(ADCdata) + 1;
			Flag = 0;
		}
  }
}

void PortF_Init(void) { volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000020;     
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTF_AMSEL_R = 0;        
  GPIO_PORTF_PCTL_R = 0;    
  GPIO_PORTF_DIR_R |= 0x02;          
  GPIO_PORTF_AFSEL_R &= ~0x02;        
  GPIO_PORTF_DEN_R |= 0x02;
	GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R&0x02;
}
