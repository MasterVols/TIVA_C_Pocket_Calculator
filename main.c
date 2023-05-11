
#include <stdint.h>
#include <stdbool.h>
#include "LCD_DISPLAY.h"
#include "switch_matrix.h"

struct component
{
    char cont;
    float val;
    int layer;
};

//void parse_input (struct component*, char*, int*, int*);

float num_evaluation(char*, unsigned int*);
int float_size(float);
char* float_to_string(char* s, float f);

void initButtons(void) {
    // Enable the clock for Port A
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

    // Wait for the GPIOA to be ready
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0) == 0) {};

    // Unlock Port A
    GPIO_PORTA_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTA_CR_R |= 0x0C; // Enable changes to PA2 and PA3

    // Set PA2 and PA3 pins as digital
    GPIO_PORTA_DEN_R |= 0x0C;

    // Set PA2 and PA3 pins as inputs
    GPIO_PORTA_DIR_R &= ~0x0C;

    // Enable pull-up resistors for PA2 and PA3
    GPIO_PORTA_PUR_R |= 0x0C;

    // Lock Port A
    GPIO_PORTA_LOCK_R = 0;
}

// Function to read the state of PA2 and PA3 buttons
uint8_t readButtons(void) {
    uint8_t buttonState = 0;

    // Read PA2 and PA3 pins state
    buttonState = (GPIO_PORTA_DATA_R & 0x0C) >> 2;

    return buttonState;
}

int main(void) {
    PortB_Init();
		SysTick_Init();
    initLCD();
		initButtons(); // Initialize the buttons
	
		clearDisplay();
		writeString("ECE-256 Final");
		delayMicroseconds(3000000);

		char options[18] = {'0','1','2','3','4','5','6','7','8','9','+','-','*','/','(',')','!','C'};
    int selection = 0;
		char current[17] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
		unsigned int indexer = 0;
		
		clearDisplay();
		delayMicroseconds(100000);
		uint8_t buttonState = readButtons();
		int contm = 1;
		
		//float f = 12.12;
		//char flt[float_size(f)];
		//float_to_string(flt,f);
		//writeString(flt);
			
		selection = 0;
		indexer = 0;
		while (contm)
		{
			int operator_index = -1;
			int eos = -1;
			while (1)
			{
			buttonState = readButtons();
			// Button one is on the right
			// Display the respective thing				
			if ((buttonState & 0x01) == 0)
			{

				selection = selection + 1;
				if (selection >= 18)
				{
					selection = 0;
				}
			}
			else if ((buttonState & 0x02) == 0)
			{
				
				if (options[selection] == '!')
				{
					eos = indexer;
					break;
				}
				else if (options[selection] == 'C')
				{
					int i = 0;
					for (i = 0; i < 17; i++)
					{
						current[i] = ' ';
						selection = 0;
						indexer = 0;
					}
				}
				else
				{
					if (options[selection] == '+' || options[selection] == '-' || options[selection] == '*' || options[selection] == '/')
					{
						operator_index = indexer;
					}
					indexer = indexer + 1;
					selection = 0;
				}
			}
			current[indexer] = options[selection];
			clearDisplay();
			writeString(current);
			delayMicroseconds(500);
		}
			// Handle the equation
			indexer = 0;
			float a = num_evaluation(current,&indexer);
			indexer++;
			char op = current[indexer];
			indexer++;
			float b = num_evaluation(current,&indexer);
		
			float eval = 0;
			if (op == '*') eval = a * b;
			else if (op == '/') eval = a / b;
			else if (op == '+') eval = a + b;
			else if (op == '-') eval = a - b;
			
			char evals[float_size(eval)];
			float_to_string(evals,eval);
			clearDisplay();
			delayMicroseconds(500);
			int i = 0;
			for (i = 0; i < 17; i++) current[i] = ' ';
			
			if (eval < 0) for (i = 0; i < float_size(eval) + 1; i++) current[i] = evals[i];
			else for (i = 0; i < float_size(eval); i++) current[i] = evals[i];
			writeString(current);
			delayMicroseconds(10000);
			while ((buttonState & 0x01) == 1) buttonState = readButtons();
			for (i = 0; i < 17; i++) current[i] = ' ';
			indexer = 0;
			selection = 0;
	}
		
	
    return 0;
}

float num_evaluation(char* str, unsigned int* index)
{
	float eval = 0;
	unsigned int eof = *index;
	unsigned int deci = *index;
	unsigned int i = 0;
	for (i = *index; i < 1000; i++)
	{
		unsigned char check = str[i];
		if (check == '.') deci = i;
    else if (check != '0' && check != '1' && check != '2' && check != '3' && check != '4' && check != '5' && check != '6' && check != '7' && check != '8' && check != '9' && check != '.')
    {
			if (deci == *index) deci = i;
      eof = i;
      break;
    }
  }
  unsigned int sof = eof - *index;
  unsigned int nbd = deci - *index;
  unsigned int nad;
  if (eof == deci) nad = 0;
  else nad = eof - 1 - deci;
	
  for (i = *index; i < deci; i = i + 1)
  {
		float multiplier = 1;
    unsigned int j = 1;
		for (j=j; j < (deci - i); j = j + 1)
    {
			multiplier = multiplier * 10;
    }
    eval = eval + multiplier * (str[i] - 48);
  }
	for (i = deci + 1; i <= (deci + nad); i = i + 1)
  {
		float multiplier = 1;
		unsigned int j = 0;
    for (j = 1; j <= (i - deci); j = j + 1)
    {
			multiplier = multiplier * 0.1;
    }
    eval = eval + multiplier * (str[i] - 48);
  }
  *index = eof - 1;
  return eval;
}



int float_size(float f)
{
	int signnum = 1;
  if (f < 0)
  {
		f = f * -1;
    signnum = -1;
  }
  int predec = f;
  int postdec = (f - predec) * 1000000;
  int size_before = 0;
  int mult = 1;
  while ((f - mult) > 0)
  {
		mult = mult * 10;
    size_before++;
  }
  int size_after = 0;
  mult = 1;
  while ((((f - predec) * 1000000) - mult) > 0)
  {
		mult = mult * 10;
    size_after++;
  }
  int string_len;
  if (size_after > 0) string_len = size_before + 1 + size_after;
  else string_len = size_before;
  if (signnum == -1) string_len += 1;
  return string_len;
}


char* float_to_string(char* s, float f)
{
	int signnum = 1;
  if (f < 0)
  {
		f = f * -1;
    signnum = -1;
  }
  int predec = f;
  int postdec = (f - predec) * 1000000;
  int size_before = 0;
  int mult = 1;
  while ((f - mult) > 0)
  {
		mult = mult * 10;
    size_before++;
  }
  int size_after = 0;
  mult = 1;
  while ((((f - predec) * 1000000) - mult) > 0)
  {
		mult = mult * 10;
    size_after++;
  }
  int si = 0;
  if (signnum == -1)
  {
		s[si] = '-';
    si++;
  }
  int tenpow;
  int i = 0;
	for (i = size_before - 1; i >= 0 ; i--)
  {
		tenpow = 1;
    int j = 0;
		for (j = 0; j < i; j++) tenpow = tenpow * 10;
    int divs = predec / tenpow;
    predec = predec - tenpow * divs;
    s[si] = (char)(48 + divs);
    si++;   
  }
  if (size_after > 0)
  {
		s[si] = '.';
    si++;
  }
  tenpow = 1;
	for (i = size_after - 1; i >= 0; i--)
  {
		tenpow = 1;
		int j = 0;
		for (j = 0; j < i; j++) tenpow = tenpow * 10;
    int divs = postdec / tenpow;
    postdec = postdec - tenpow * divs;
    s[si] = (char)(48 + divs);
    si++;
  }
  return s;
}



