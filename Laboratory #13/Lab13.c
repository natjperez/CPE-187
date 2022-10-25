// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano; Edited by Natalia Perez
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

unsigned long Input;

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);

int main(void){ // Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
	// PortE used for piano keys, PortB used for DAC
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  // enable after all initialization are done
  while(1){                
		// input from keys to select tone
		Input = Piano_In();
		if(Input == 0x01) {
			Sound_Tone(C); //523.251 Hz
		}
		else if(Input == 0x02) {
			Sound_Tone(D); //587.330 Hz
		}
		else if(Input == 0x04) {
			Sound_Tone(E); //659.255 Hz
		}
		else if(Input == 0x08) {
			Sound_Tone(G); //783.991 Hz
		}
		else {
			Sound_Off();
		}
		delay(10);
  }     
}

/*
int main(void){ // this main is to debug the DAC
 // you must connect PD3 to your DAC output
	TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn);
	DAC_Init(); // initialize SysTick timer and DAC
	EnableInterrupts(); // enable after all initialization are done
	 while(1){unsigned long i; // static debugging
	 for(i=0;i<16;i++){
			DAC_Out(i);
			delay(10); // connect PD3 to DAC output
		}
	 } 
}*/

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}
