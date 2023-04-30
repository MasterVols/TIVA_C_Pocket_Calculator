#include "SysTick.h"
#include "tm4c123gh6pm.h"
#include "stdint.h"

// Used in part 3
volatile uint32_t g_handler_calls;

//Initialize Systick
void SysTick_Init(void){

}


// Configure SysTick to generate an interrupt every 20ms
// Assume 16 MHz clock
void SysTick_Init_Interrupts(void){
	g_handler_calls = 0; //Initialize counter as 0

}


// Clock speed is 16 MHz
static void SysTick_Delay1ms_16MHz(void){
	//Use the Systick Timer to generate a 1ms delay
	
	// Choose the number of clock ticks to wait
	//NVIC_ST_RELOAD_R = ;
	
	NVIC_ST_CURRENT_R = 0; // Any value written to write clears it
	while((NVIC_ST_CTRL_R&0x00010000)==0){} // Wait for count flag
}


// Write code to generate a 2 sec delay
// Your code should call SysTick_Delay1ms()
void SysTick_Delay2s_16MHz(void){

} 


// Write code to generate 1ms delay assuming a clock speed of 50MHz
static void SysTick_Delay1ms_50MHz(void){

}


// Write code to generate a 2 sec delay when the clock speed is 50MHz
// Your code should call SysTick_Delay1ms_50MHz()
void SysTick_Delay2s_50MHz(void){

}
	


// Toggle green LED every 2 seconds
// Toggle blue LED every 1 second
void SysTick_Handler(void){
	
	g_handler_calls++;
}
