
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

void parse_input (struct component*, char*, int*, int*);
float num_evaluation(char*, unsigned int*);
int eval_layer(struct component*,int*);
int float_size(float);
char* float_to_string(char* s, float f);

int main(void) {
    PortB_Init();
		SysTick_Init();
    initLCD();
		switch_matrix_init();
	
	
		writeString("");
	
		//loop:
		while (1) {
				clearDisplay();

        uint8_t button = switch_matrix_read();
				

        if (button != 0xFF) {
            // A button press is detected, convert button number to character and write it to the display
            char button_char = (button < 10) ? ('0' + button) : ('A' + button - 10);
            writeChar(button_char);
        }
    }
		
		
		/*
		char input[12];
		input[0] = '4';
		input[1] = '+';
		input[2] = '4';
		input[3] = '!';
		//input = "4+4!";  
		// set input here;
	
		
		float eval = 0;
	  struct component comps[100];
	  int failed = 0;
		int comps_size = 100;
		
		
		
		parse_input(comps,input,&comps_size,&failed);
		int ml = 1;
		if (failed)
		{
			writeString("ERROR:fail");
		}
		else
		{
			while (ml != 0)
			{
				ml = eval_layer(comps,&comps_size);
			}
			float f = comps[0].val;
			char flt[float_size(f)-1];
			float_to_string(flt,f);
			initLCD();
			writeString("test"); 
		}
		*/
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


void parse_input (struct component* comps, char* str, int* comps_size, int* failed)
{
	int comp_indexer = 0;
	unsigned int lv = 0;
  unsigned int max_layer = 0;
	unsigned int i;
  for (i = 0; i < 1000; i++)
  {
		if (str[i] == '!') 
    {
			if (comps[comp_indexer-1].layer != 0 && (str[i-1] != ')'))
      {
				writeString("ERROR:unbal");
				//printf("ERROR: Unbalanced parenthetical groups.\n");
        *failed = 1;
      }
      else if ((str[i-1] == ')') && comps[comp_indexer-1].layer != 1)
      {
				writeString("ERROR:unbal");
        //printf("ERROR: Unbalanced parenthetical groups.\n");
        *failed = 1;
      }
      else
      { 
				comps[comp_indexer].cont = ')';
				comps[comp_indexer].layer = 0;
        comps[comp_indexer++].val = 0;
      }    
      *comps_size = comp_indexer;
      break;
		}
    if (str[i] == '0' || str[i] == '1' || str[i] == '2' || str[i] == '3' || str[i] == '4' || str[i] == '5' || str[i] == '6' || str[i] == '7' || str[i] == '8' || str[i] == '9' || str[i] == '.')
    {
			comps[comp_indexer].cont = 'n';
			comps[comp_indexer].layer = lv;
      comps[comp_indexer++].val = num_evaluation(str,&i);
    }
    else if (str[i] == '(')
    {
			comps[comp_indexer].cont = '(';
			comps[comp_indexer].layer = ++lv;
			comps[comp_indexer++].val = 0;
    }
    else if (str[i] == ')')
    {
			comps[comp_indexer].cont = ')';
      comps[comp_indexer].layer = lv--;
			comps[comp_indexer++].val = 0;
		}
    else
    {
			comps[comp_indexer].cont = str[i];
      comps[comp_indexer].layer = lv;
      comps[comp_indexer++].val = 0;
    }
    if (lv > max_layer) max_layer = lv;
	}
}

int eval_layer(struct component* comps,int* size)
{
	//struct component comps[*size];
  //for (unsigned int i = 0; i < *size; i++) comps[i] = comp[i]; 
  int max_layer = -1;
  unsigned int mls = 0;
  unsigned int mle = 0;

	int i = 0;
  for (i = 0; i < *size; i++)
  {
		if (comps[i].layer > max_layer) max_layer = comps[i].layer;
  }
  for (i = 0; i < *size; i++)
  {
		if (comps[i].layer == max_layer)
    {
			if (comps[i].cont == '(') mls = i;
      else if (comps[i].cont == ')')
			{
				mle = i;
				break;
			}
		}
	}
  char prior = '!';
	unsigned lmd = 0;
	for (i = mls; i <= mle; i++)
  {
		if (prior == 'n' && comps[i].cont == '*')
    {
			comps[i].val = comps[i-1].val * comps[i+1].val;
      comps[i-1].cont = 's';
      comps[i+1].cont = 's';
      comps[i].cont = 'n';
      lmd = i;
    }
    else if (prior == 'n' && comps[i].cont == '/')
		{
			comps[i].val = comps[i-1].val / comps[i+1].val;
      comps[i-1].cont = 's';
      comps[i+1].cont = 's';
      comps[i].cont = 'n';
      lmd = i;
    }
    else if (prior == 's' && comps[i].cont == '*')
		{
			comps[i].val = comps[lmd].val * comps[i+1].val;
      comps[i+1].cont = 's';
      comps[lmd].cont = 's';
      comps[i].cont = 'n';
      lmd = i;
    }
		else if (prior == 's' && comps[i].cont == '/')
    {
			comps[i].val = comps[lmd].val / comps[i+1].val;
      comps[i+1].cont = 's';
      comps[lmd].cont = 's';
      comps[i].cont = 'n';
      lmd = i;
    }
    prior = comps[i].cont;
	}
  int shift = 0;
  for (i = 0; i < *size; i++)
  {
		if (i >= mls && i <= mle)
    {
			if (comps[i].cont == 's')shift++;
      else comps[i - shift] = comps[i];
    }
    else comps[i - shift] = comps[i];
  }
  mle = mle - shift;
  *size = *size - shift;
  prior = '!';
	unsigned int lps = 0;
  for (i = mls; i <= mle; i++)
  {
		if (prior == 'n' && comps[i].cont == '+')
    {
			comps[i].val = comps[i-1].val + comps[i+1].val;
      comps[i-1].cont = 's';
      comps[i+1].cont = 's';
      comps[i].cont = 'n';
      lps = i;
    }
    else if (prior == 'n' && comps[i].cont == '-')
    {
			comps[i].val = comps[i-1].val - comps[i+1].val;
      comps[i-1].cont = 's';
      comps[i+1].cont = 's';
      comps[i].cont = 'n';
      lps = i;
    }
    else if (prior == 's' && comps[i].cont == '+')
    {
			comps[i].val = comps[lps].val + comps[i+1].val;
      comps[i+1].cont = 's';
      comps[lps].cont = 's';
      comps[i].cont = 'n';
      lps = i;
    }
		else if (prior == 's' && comps[i].cont == '-')
    {
			comps[i].val = comps[lps].val - comps[i+1].val;
      comps[i+1].cont = 's';
      comps[lps].cont = 's';
      comps[i].cont = 'n';
      lps = i;
    }
    else if (prior == '(' && comps[i].cont == '-')
    {
			comps[i].val = 0 - comps[i+1].val;
      comps[i+1].cont = 's';
      comps[i].cont = 'n';
      lps = i;
    }
    prior = comps[i].cont;
	}
	shift = 0;
  for (i = 0; i < *size; i++)
  {
		if (i >= mls && i <= mle)
		{
			if (comps[i].cont == 's'|| comps[i].cont == '(' || comps[i].cont == ')') shift++;
      else
      {
				comps[i].layer = comps[i].layer - 1;
				comps[i - shift] = comps[i];
      }
    }
    else comps[i - shift] = comps[i];
  }
  mle = mle - shift;
  *size = *size - shift;
  return max_layer;
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
		int j = 0;
		for (j = 0; j < i; j++) tenpow = tenpow * 10;
    int divs = postdec / tenpow;
    postdec = postdec - tenpow * divs;
    s[si] = (char)(48 + divs);
    si++;
  }
  return s;
}
