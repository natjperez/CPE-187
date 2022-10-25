// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano; Edited by Natalia Perez
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
#define TRAFFICLIGHTS (*((volatile unsigned long *)0x400050FC)) //PB5-PB0
#define SENSORS (*((volatile unsigned long *)0x4002401C)) //PE2-PE0
#define WALKLIGHTS (*((volatile unsigned long *)0x40025028)) //PF3, PF1
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))

#define GoW 		 0	//Go west
#define WaitW 	 1	//Wait west
#define GoS 		 2	//Go south
#define WaitS 	 3	//Wait south
#define Walk 		 4	//OK to walk
#define DontWalk 5	//Not OK to walk
#define WLO 		 6	//Walk light off
#define FlashDW  7	//Flash don't walk
#define FlashWLO 8	//Flash walk light off
struct State {
	unsigned long PBOut;		// Traffic LED output in hexadecimal
	unsigned long PFOut;		// Walk LED output in hexadecimal
	unsigned long Time;		// Time to wait
	unsigned long NextState[8]; // Next state
};
typedef const struct State StateType;
//Time kept between 1/2 sec to 1 sec for grading
StateType FSM[9] = 
{{0x0C, 0x02, 100, {GoW, GoW, WaitW, WaitW, WaitW, WaitW, WaitW, WaitW}},
{0x14, 0x02, 100, {GoW, GoW, GoS, GoS, Walk, Walk, GoS, GoS}},
{0x21, 0x02, 100, {GoS, WaitS, GoS, WaitS, WaitS, WaitS, WaitS, WaitS}},
{0x22, 0x02, 100, {GoS, GoW, GoS, GoW, Walk, Walk, Walk, Walk}},
{0x24, 0x08, 50, {Walk, DontWalk, DontWalk, DontWalk, Walk, DontWalk, DontWalk, DontWalk}},
{0x24, 0x02, 50, {DontWalk, WLO, WLO, WLO, WLO, WLO, WLO, WLO}},
{0x24, 0x00, 50, {WLO, FlashDW, FlashDW, FlashDW, FlashDW, FlashDW, FlashDW, FlashDW}},
{0x24, 0x02, 50, {FlashDW, FlashWLO, FlashWLO, FlashWLO, FlashWLO, FlashWLO, FlashWLO, FlashWLO}},
{0x24, 0x00, 50, {FlashWLO, GoW, GoS, GoW, Walk, GoW, GoS, GoW}}};

unsigned long S; //S = state index
unsigned long In; //In = input

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long delay);
void Ports_Init(void);

// ***** 3. Subroutines Section *****
void SysTick_Init(void) {
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CTRL_R = 0x00000005;
}

//Delay in units of 80MHz core clock (12.5ns)
void SysTick_Wait(unsigned long delay) {
	NVIC_ST_RELOAD_R = delay-1;
	NVIC_ST_CURRENT_R = 0;
	while((NVIC_ST_CTRL_R&0x00010000) == 0){
	
	}
}

//800000*12.5ns = 10ms
void SysTick_Wait10ms(unsigned long delay) {
	unsigned long i;
	for(i = 0; i < delay; i++) {
		SysTick_Wait(800000); //wait 10ms
	}
}

// PORTE = PE2-PE0 | sensor inputs
// PORTB = PB5-PB0 | traffic outputs
// PORTF = PF3, PF1 | walk outputs
void Ports_Init(void) { volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000032;
	delay = SYSCTL_RCGC2_R;
	// PortE Initializations
	GPIO_PORTE_AMSEL_R &= ~0x07; //Disable analog functions
	GPIO_PORTE_PCTL_R &= ~0x00000FFF; //Clear bit PCTL
	GPIO_PORTE_DIR_R &= ~0x07; //PE2-PE0 inputs 
	GPIO_PORTE_AFSEL_R &= ~0x07; //No alternate function
	GPIO_PORTE_DEN_R |= 0x07; //enable PE2-PE0 as digital inputs
	
	//PortB Initializations
	GPIO_PORTB_AMSEL_R &= ~0x3F; //Disable analog functions
	GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; //Clear bit PCTL 
	GPIO_PORTB_DIR_R |= 0x3F; //PB5-PE0 outputs 
	GPIO_PORTB_AFSEL_R &= ~0x3F; //No alternate function
	GPIO_PORTB_DEN_R |= 0x3F; //enable PE2-PE0 as digital pins
	
	// PortF Initializations
	GPIO_PORTF_AMSEL_R &= ~0x0A; //Disable analog functions
	GPIO_PORTF_PCTL_R &= ~0x0000F0F0; //Clear bit PCTL
	GPIO_PORTF_DIR_R |= 0x0A; //PF3,PF1 outputs 
	GPIO_PORTF_AFSEL_R &= ~0x0A; //No alternate function
	GPIO_PORTF_DEN_R |= 0x0A; //enable PF3,PF1 as digital inputs
}

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210, ScopeOff); // activate grader and set system clock to 80 MHz
  SysTick_Init();
	Ports_Init();
  S = GoW;
	EnableInterrupts();
	
  while(1){
		//Output Traffic LEDs in unfriendly manner (PB5-PB0)
		TRAFFICLIGHTS = FSM[S].PBOut;
		//Output detector LEDs in unfriendly manner (PF3, PF1)
		WALKLIGHTS = FSM[S].PFOut;
		//Wait time
		SysTick_Wait10ms(FSM[S].Time);
		//Input from sensors (PE2-PE0)
		In = SENSORS;
		//Go to next state
		S = FSM[S].NextState[In];
  }
}

