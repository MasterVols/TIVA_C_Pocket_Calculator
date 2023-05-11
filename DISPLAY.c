#include "LCD_DISPLAY.h"


// LCD port:
void PortB_Init(void);
void PortB_Write(uint8_t data);
int delayMS = 1000;
// timer handling:
void SysTick_Init(void);
void delayMicroseconds(uint32_t microseconds);

void initLCD(void) {

		// Let LCD Capacitance charge:
		delayMicroseconds(delayMS*1000);
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

uint8_t reverseBits(uint8_t n) {
    uint8_t result = 0;
		int i = 0;
    for (i = 0; i < 4; i++) {
        result <<= 1;
        result |= (n & 1);
        n >>= 1;
    }
    return result;
}

void writeChar(char c) {
    uint8_t upper_nibble = (c & 0xF0) >> 4; // Extract upper 4 bits
    uint8_t lower_nibble = c & 0x0F;        // Extract lower 4 bits
	
		upper_nibble = reverseBits(upper_nibble);
    lower_nibble = reverseBits(lower_nibble);

    upper_nibble |= 0x20; // Set RS to 1
    lower_nibble |= 0x20; // Set RS to 1

		PortB_Write(upper_nibble);
		PortB_Write(lower_nibble);
}

void writeString(const char *str) {
    while (*str) {
        writeChar(*str);
        str++;
    }
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
	
		delayMicroseconds(delayMS);
		GPIO_PORTB_DATA_R &= ~0x10; //turns off enable
		delayMicroseconds(delayMS);
		GPIO_PORTB_DATA_R &= ~0x0F;
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

void clearDisplay(void) {
		delayMicroseconds(40000);
    uint8_t upper_nibble = 0x00; // Clear Display command: 0x01
    uint8_t lower_nibble = 0x01;

    upper_nibble = reverseBits(upper_nibble);
    lower_nibble = reverseBits(lower_nibble);
	
		GPIO_PORTB_DATA_R &= ~0x20; // Set RS to 0
    GPIO_PORTB_DATA_R &= ~0x20;

    // Send command with RS set to 0
    PortB_Write(upper_nibble);
    PortB_Write(lower_nibble);
	
		GPIO_PORTB_DATA_R |= 0x20; // Set RS to 0
    GPIO_PORTB_DATA_R |= 0x20;

    // The Clear Display command takes longer to execute, add a longer delay
    delayMicroseconds(4000);
}