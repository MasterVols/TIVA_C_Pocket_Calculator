
#include <stdint.h>
#include <stdbool.h>
#include "TM4C123GH6PM.h"

// LCD port:
void PortB_Init(void);
void PortB_Write(uint8_t data);
int delayMS = 100000;
// timer handling:
void SysTick_Init(void);
void delayMicroseconds(uint32_t microseconds);

void initLCD(void) {


	/*
	RS(0) EN(0)0100 4
						 0000 0
						 1000 8
						 0000 0
						 0100 4
						 0000 0 
						 0011 3
	*/
		PortB_Write(0x04);
		PortB_Write(0x00);
		PortB_Write(0x08);
		PortB_Write(0x00);
		PortB_Write(0x04);
		PortB_Write(0x00);
		PortB_Write(0x03);
}

void writeChar(uint16_t num) {
		num |= 0x20; //sets RS to 1
		PortB_Write(num);
}

int main(void) {
    PortB_Init();
		SysTick_Init();
    initLCD();
    
		//use asscii table
		
    writeChar(0x02);
		writeChar(0x01);

    return 0;
}

// Function to initialize Port B
void PortB_Init(void) {
    // Enable the clock for Port B
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    // Wait for the GPIOB to be ready
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0) {};

    // Unlock Port B
    GPIO_PORTB_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTB_CR_R = 0xFF;

    // Set Port B pins as digital
    GPIO_PORTB_DEN_R = 0xFF;

    // Set Port B pins as outputs
    GPIO_PORTB_DIR_R = 0xFF;

    // Lock Port B
    GPIO_PORTB_LOCK_R = 0;
			
		GPIO_PORTB_DATA_R = 0;
}



// Function to write a byte (hexadecimal) to Port B
void PortB_Write(uint8_t data) {
		GPIO_PORTB_DATA_R &= ~0x0F;
    GPIO_PORTB_DATA_R |= data;
		delayMicroseconds(delayMS);
	
		GPIO_PORTB_DATA_R |= 0x10; //sends the enable
		delayMicroseconds(delayMS);
	
		GPIO_PORTB_DATA_R &= ~0x0F;
	
		delayMicroseconds(delayMS);
		GPIO_PORTB_DATA_R &= ~0x10; //turns off enable
}



void SysTick_Init(void) {
    NVIC_ST_CTRL_R = 0;                   // Disable SysTick during setup
    NVIC_ST_RELOAD_R = 0x00FFFFFF;        // Set the maximum reload value
    NVIC_ST_CURRENT_R = 0;                // Clear the current register
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE;
}

void delayMicroseconds(uint32_t microseconds) {
    NVIC_ST_RELOAD_R = (microseconds * 50) - 1; // Account for 50 cycles per microsecond (for 50MHz system clock)
    NVIC_ST_CURRENT_R = 0;                       // Clear the current register
    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0) {} // Wait for the count flag to be set
}