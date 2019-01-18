#include "stdint.h"
#include "LCD_ILI9325.h"
#include "Open1768_LCD.h"
#include "asciiLib.h"

uint16_t s1[16] = {
0,
0,
0,
0,
0,
0,
0,
1<<8,
0,
0,
0,
0,
0,
0,
0,
0 //.
};

uint16_t s3[16] = {
0,
0,
0,
0,
0,
0,
1<<8,
1<<7 | 1<<8 | 1<<9,
1<<8,
0,
0,
0,
0,
0,
0,
0 //+
};

uint16_t s2[16] = {
0,
0,
0,
0,
0,
0,
1<<8 | 1<<9, 
1<<8 | 1<<9,
0,
0,
0,
0,
0,
0,
0 //o
};

uint16_t s4[16] = {
0,
0,
0,
0,
0,
0,
1<<9 | 1<<8,
1<<10 | 1<<9 | 1<<8 | 1<<7,
1<<10 | 1<<9 | 1<<8 | 1<<7,
1<<9 | 1<<8,
0,
0,
0,
0,
0,
0 //o
};

uint16_t s5[16] = {
0,
0,
0,
0,
0,
1<<9 | 1<<8 | 1<<7,
1<<10 | 1<<9 | 1<<8 | 1<<7 | 1<<6,
1<<10 | 1<<9 | 1<<8 | 1<<7 | 1<<6,
1<<10 | 1<<9 | 1<<8 | 1<<7 | 1<<6,
1<<9 | 1<<8 | 1<<7,
0,
0,
0,
0,
0,
0 //o
};

uint16_t s6[16] = {
0,
0,
0,
0,
0,
1<<9 | 1<<8 | 1<<7 | 1<<6,
1<<9 | 1<<8 | 1<<7 | 1<<6,
1<<10 | 1<<9 | 1<<8 | 1<<7 | 1<<6 | 1<<5,
1<<10 | 1<<9 | 1<<8 | 1<<7 | 1<<6 | 1<<5,
1<<9 | 1<<8 | 1<<7 | 1<<6,
1<<9 | 1<<8 | 1<<7 | 1<<6,
0,
0,
0,
0,
0 //o
};

uint16_t s7[16] = {
0,
0,
0,
0,
1<<9 | 1<<8 | 1<<7,
1<<10 | 1<<9 | 1<<8 | 1<<7 | 1<<6,
1<<11 | 1<<10 | 1<<9 | 1<<8 | 1<<7 | 1<<6 | 1<<5,
1<<11 | 1<<10 | 1<<9 | 1<<8 | 1<<7 | 1<<6 | 1<<5,
1<<11 | 1<<10 | 1<<9 | 1<<8 | 1<<7 | 1<<6 | 1<<5,
1<<10 | 1<<9 | 1<<8 | 1<<7 | 1<<6,
1<<9 | 1<<8 | 1<<7,
0,
0,
0,
0,
0 //o
};


extern uint32_t stroke;
extern uint32_t color;

void drawStroke(int x, int y);
void drawCStroke(int x, int y);

void getStroke(uint16_t* buf, int pos)
{
	pos++;
	switch(pos)
	{
		case 1:
			for(int i = 0; i < 16; ++i)
				buf[i] = s1[i];
			break;
		case 2:
			for(int i = 0; i < 16; ++i)
				buf[i] = s2[i];
			break;
		case 3:
			for(int i = 0; i < 16; ++i)
				buf[i] = s3[i];
			break;
		case 4:
			for(int i = 0; i < 16; ++i)
				buf[i] = s4[i];
			break;
		case 5:
			for(int i = 0; i < 16; ++i)
				buf[i] = s5[i];
			break;
		case 6:
			for(int i = 0; i < 16; ++i)
				buf[i] = s6[i];
			break;
		case 7:
			for(int i = 0; i < 16; ++i)
				buf[i] = s7[i];
			break;
	}
}

void printCharacter(int posx, int posy, char sgn)
{

	lcdWriteReg(ADRX_RAM, posx);
	lcdWriteReg(ADRY_RAM, posy);
	unsigned char buf[16];
	GetASCIICode(0, buf, sgn);
	for (int i = 0; i < 16; ++i)
	{
		lcdWriteIndex(DATA_RAM);
		for(int bit = 7; bit >= 0; --bit)
		{
			lcdWriteData((!(buf[i] & (1 << bit))) ? LCDGrey : LCDBlack);
		}
		lcdWriteReg(ADRX_RAM, posx);
		lcdWriteReg(ADRY_RAM, ++posy);
	}
}

void drawLayout(void)
{
	lcdWriteReg(ADRX_RAM, 0);
	lcdWriteReg(ADRY_RAM, 0);
	lcdWriteIndex(DATA_RAM);
	for (int y = 0; y < 320; ++y)
		for (int x = 0 ; x < 240; ++x)
		{
			if(y == 0 || y == 319 || y == 302 || x == 221 || x == 0 || x == 239)
				lcdWriteData(LCDBlack);
			else if(x > 221 && y <= 302)
			{
				if(y == 17 || y == 34 || y == 51 || y == 68 || y == 85 || y == 102 || y == 118 || y == 212 || y == 268 || y == 285)
					lcdWriteData(LCDBlack);
				else
					lcdWriteData(LCDGrey);
			}
			else if(y > 302)
			{
				if(x == 18 || x == 35 || x == 54 || x == 72 || x == 90 || x == 108 || x == 126 || x == 144 || x == 164 || x == 183 || x == 202)
					lcdWriteData(LCDBlack);
				else if (x < 18)
					lcdWriteData(LCDWhite);
				else if (x < 35)
					lcdWriteData(LCDBlack);
				else if (x < 54)
					lcdWriteData(LCDGrey);
				else if (x < 72)
					lcdWriteData(LCDBlue);
				else if (x < 90)
					lcdWriteData(LCDBlueSea);
				else if (x < 108)
					lcdWriteData(LCDPastelBlue);
				else if (x < 126)
					lcdWriteData(LCDViolet);
				else if (x < 144)
					lcdWriteData(LCDMagenta);
				else if (x < 165)
					lcdWriteData(LCDRed);
				else if (x < 183)
					lcdWriteData(LCDGinger);
				else if (x < 202)
					lcdWriteData(LCDGreen);
				else if (x < 221)
					lcdWriteData(LCDCyan);
				else 
					lcdWriteData(LCDYellow);
			}
			else
			  lcdWriteData(LCDWhite);
		}
	
    printCharacter(226, 125, 'C');
    printCharacter(226, 141, 'L');
    printCharacter(226, 157, 'E');
    printCharacter(226, 173, 'A');
    printCharacter(226, 189, 'R');
		
    int ctmp = color;
		color = LCDBlack;
    int tmp = stroke;
    for(int i = 1; i < 8; i++)
    {
        stroke = i;
        drawCStroke(230, 16 * i);
    }
		color = ctmp;
    stroke = tmp;
}
