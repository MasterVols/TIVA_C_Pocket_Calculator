#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <stdint.h>
#include "TM4C123GH6PM.h"

void PortB_Init(void);
void PortB_Write(uint8_t data);
void SysTick_Init(void);
void delayMicroseconds(uint32_t microseconds);
void initLCD(void);
void writeChar(char c);
void writeString(const char *str);
uint8_t reverseBits(uint8_t n);

#endif // LCD_DISPLAY_H