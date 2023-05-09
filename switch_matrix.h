#ifndef SWITCH_MATRIX_H
#define SWITCH_MATRIX_H

#include <stdint.h>
#include "TM4C123GH6PM.h"

// Define the port masks for rows and columns
#define ROW_PORT_BASE GPIO_PORTE_BASE
#define ROW_PINS 0x0F
#define COLUMN_PORT_BASE GPIO_PORTD_BASE
#define COLUMN_PINS 0x0F

// Function prototypes
void switch_matrix_init(void);
uint8_t switch_matrix_read(void);

#endif // SWITCH_MATRIX_H
