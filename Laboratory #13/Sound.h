// Sound.h
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano; Edited by Natalia Perez
// December 29, 2014

// The Sound function utilizes periodic interrupts to produce sound and
// turn sound off. 

#define G	6377 // Piano Key 3 
#define E 7583 // Piano Key 2 
#define D 8512 // Piano Key 1 
#define C 9555 // Piano Key 0

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also initializes DAC
// Input: none
// Output: none
void Sound_Init(void);

// **************Sound_Tone*********************
// Change SysTick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period);

// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void);
