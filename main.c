
#include <stdint.h>
#include <stdbool.h>
#include "LCD_DISPLAY.h"

int main(void) {
    PortB_Init();
		SysTick_Init();
    initLCD();
    
		writeString("ABCDEFGHIJ");

    return 0;
}

