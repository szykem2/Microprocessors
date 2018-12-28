#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Joystick.h"             // ::Board Support:Joystick
#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "Open1768_LCD.h"
#include "LCD_ILI9325.h"
#include "asciiLib.h"
#include "TP_Open1768.h"

uint32_t TS_pressed = 0;
uint32_t stroke = 1;
uint32_t color = LCDBlack;
const int xmin = 200;
const int xmax = 4500;
const int ymin = 200;
const int ymax = 4500;
const int sizex = 240;
const int sizey = 320;

void configureGPIO(void);
void readTP(int *x, int *y);
void LCD_config(void);
void sleep(void);
void initializeUART(void);
void timerConfig(void);
void updateCanvas(void);
void sendString(const char* ptr);
void sendCharacter(char value);
void printCharacter(int posx, int posy, char sgn);
void drawLayout(void);
void drawStroke(int x, int y);
void getStroke(uint16_t* buf, int pos);
void handleSpecial(int x, int y);
void handleRightBar(int x, int y);
void handleBottomBar(int x, int y);
void drawColor(void);


int main(void)
{
	initializeUART();
	LCD_config();
	touchpanelInit();
	configureGPIO();
	while(1)
	{
		if (TS_pressed == 1)
		{
			updateCanvas();
		}
	}
}

void updateCanvas(void)
{
	int x, y;
	readTP(&x, &y);
	if(x == 0 || x == xmax || y == 0 || y == ymax)
	{
		sleep();
		return;
	}
	if(x < sizex - 17 && y < sizey - 17)
	{
		drawStroke(x, y);
	}
	else
	{
		handleSpecial(x, y);
	}
	
	sleep();
}

void sendCharacter(char value)
{
	while(!(LPC_UART0->LSR & (1<<5))){}
	LPC_UART0->THR = value;
}

void sendString(const char* ptr)
{
	int i = 0;
	while(ptr[i] != 0)
	{
		sendCharacter(ptr[i]);
	  i++;
	}
}

void EINT3_IRQHandler(void) 
{
	sendString("touched");
	TS_pressed ^= 1;
	LPC_GPIOINT->IO0IntClr = (1 << 19);
}

void initializeUART(void)
{
	LPC_UART0->LCR = (1<<7);
	LPC_UART0->DLL = (13<<0);
	LPC_UART0->DLM = 0;
	LPC_UART0->LCR = (3<<0);
	PIN_Configure (0, 2, PIN_FUNC_1, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
	PIN_Configure (0, 3, PIN_FUNC_1, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
}

void sleep(void)
{
	int value = 10000;
	while(value > 0) { value--; }
}

void timerConfig(void)
{
	LPC_TIM0 -> PR = 0;
	
	LPC_TIM0 -> MR0 = 12500000;
	LPC_TIM0 -> MR1 = 0;
	LPC_TIM0 -> MR2 = 0;
	LPC_TIM0 -> MR3 = 0;
	
	LPC_TIM0 -> MCR = (1<<0) | (1<<1);
	LPC_TIM0 -> TCR = 1;
	
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void LCD_config(void)
{
	lcdConfiguration();
	init_ILI9325();
	drawLayout();
	drawStroke(230,276);
	drawColor();
}

void readTP(int *x, int *y)
{
	int rx, ry;
	
	touchpanelGetXY(&rx, &ry);
	rx = (rx - xmin)/((float)xmax - xmin) * sizex;
	ry = (ry - ymin)/((float)ymax - ymin) * sizey;
	
	char buf[15];
	sprintf(buf, "(%d, %d)         \r", rx, ry);
	sendString(buf);
	*x = rx;
	*y = ry;
}

void configureGPIO()
{
	LPC_GPIOINT->IO0IntEnR = (1 << 19);
	NVIC_EnableIRQ(EINT3_IRQn);
}


void drawStroke(int x, int y)
{
	int posx = x - 8, posy = y - 8;
	uint16_t buf[16];
	getStroke(buf, stroke);
	
	for (int i = 0; i < 16; ++i)
	{
		for(int bit = 15; bit >= 0; --bit)
		{
			if(x > 1 && y > 1 && x < 239 && y < 319 && !(buf[i] & (1 << bit)))
			{
				lcdWriteReg(ADRX_RAM, posx);
				lcdWriteReg(ADRY_RAM, posy);
				lcdWriteReg(DATA_RAM, color);
			}
		}
		posy++;
	}
}

void handleSpecial(int x, int y)
{
	if(x > sizex - 17 && y > sizey - 17)
	{
		handleRightBar(x, y);
	}
	else if(y > sizey - 17)
	{
		handleBottomBar(x, y);
	}
}

void handleRightBar(int x, int y)
{
	if(y < 119)
	{
		stroke = y / 7;
		drawStroke(230,276);
	}
	else if(y < 212)
	{
		drawLayout();
		drawStroke(230,276);
		drawColor();
	}
}

void handleBottomBar(int x, int y)
{
	uint16_t colors[13] = {LCDWhite, LCDBlack, LCDGrey, LCDBlue, LCDBlueSea, LCDPastelBlue, LCDViolet, LCDMagenta, LCDRed, LCDGinger, LCDGreen, LCDCyan, LCDYellow};
	int idx = x % 13;
	color = colors[idx];
	drawColor();
}

void drawColor(void)
{
	for(int y = 285; y < 301; ++y)
	{
		lcdWriteReg(ADRX_RAM, 221);
		lcdWriteReg(ADRY_RAM, y);
		for(int x = 0; x < 16; ++x)
		{
			lcdWriteReg(DATA_RAM, color);
		}
	}
}