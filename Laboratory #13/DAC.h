// DAC.h
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Header file for the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano; Edited by Natalia Perez
// March 13, 2014

// The DAC simply outputs the sine wave created in order to produce sound
// by replicating the array values. 

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void);


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data);
  


