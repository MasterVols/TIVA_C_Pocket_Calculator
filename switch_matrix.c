#include "switch_matrix.h"

void delay(uint32_t count) {
    while (count--) {
        __asm("NOP");
    }
}

void switch_matrix_init(void) {
    // Enable clock for GPIO Port E and Port D
    SYSCTL_RCGCGPIO_R |= (SYSCTL_RCGCGPIO_R4 | SYSCTL_RCGCGPIO_R3);
    // Wait for the clock to be stable
    while ((SYSCTL_PRGPIO_R & (SYSCTL_PRGPIO_R4 | SYSCTL_PRGPIO_R3)) == 0) {}

    // Set row pins as output and column pins as input
    GPIO_PORTE_DIR_R |= ROW_PINS;
    GPIO_PORTD_DIR_R &= ~COLUMN_PINS;

    // Enable digital functions for row and column pins
    GPIO_PORTE_DEN_R |= ROW_PINS;
    GPIO_PORTD_DEN_R |= COLUMN_PINS;

    // Enable pull-up resistors for column pins
    GPIO_PORTD_PUR_R |= COLUMN_PINS;
}

uint8_t switch_matrix_read(void) {
    uint8_t row, col;
		uint8_t button = 0xFF;

    for (row = 0; row < 4; row++) {
        // Set the current row pin as low
        GPIO_PORTE_DATA_R = ~(1 << row) & ROW_PINS;

        delay(1000); // Add a small delay for synchronization

        // Read the column pins
        col = GPIO_PORTD_DATA_R & COLUMN_PINS;

        if (col != COLUMN_PINS) {
            // A button press is detected, calculate the button number
            button = (row * 4) + (__builtin_ctz(col));
            break;
        }
    }
    return button;
}
