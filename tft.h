/*******************************************
Project : 

Chip type           : ATmega32
Clock frequency     : 16,000000 MHz
*******************************************/
#ifndef _TFT_INCLUDED_
#define _TFT_INCLUDED_

#define TFT_DATA_PORT   PORTC
#define TFT_DATA_DIR    DDRC
#define TFT_CTRL_PORT   PORTA
#define TFT_CTRL_DIR    DDRA

#define TFT_RDX	7
#define TFT_WRX	6
#define TFT_DCX	5
#define TFT_CSX	4
#define TFT_RST	2

#define FONT_SPACE      10
#define FONT_X          16
#define FONT_Y          8

//----- Invert Colors --------
// #define RED            0xf800
// #define GREEN	        0x07e0
// #define BLUE	        0x001f
// #define BLACK	        0x0000
// #define YELLOW	        0xffe0
// #define WHITE	        0xffff
// #define CYAN		    0x07ff
// #define CYAN1		    0x07f0
// #define MAGENTA	    0xf810
// #define GRAY1		    0x8410
// #define GRAY2		    0x4208
// 
// #define BUTT_1		0x01
// #define BUTT_2 	0x02
// #define BUTT_3		0x04
// #define BUTT_4		0x08
 
//------Basic Colors ---------
 #define RED            0x07FF
 #define GREEN	        0xF81F
 #define GREEN1		    0xFA8F
 #define BLUE	        0xFFE0
 #define BLUE1		    0xFA00
 #define BLACK	        0xFFFF
 #define YELLOW	        0x001F
 #define YELLOW1	    0x002F
 #define YELLOW2        0x02F0
 #define WHITE	        0x0000
 #define CYAN		    0xF800
 #define MAGENTA	    0x07EF
 #define VIOLET 	    0x05E0
 #define GRAY1		    0x7BEF
 #define GRAY2		    0xBDF7
 
 #define BUTT_1		0x01
 #define BUTT_2 	0x02
 #define BUTT_3		0x03
 #define BUTT_4		0x04

#define TFTWIDTH	320
#define TFTHEIGHT	240
#define TFTBUTTON	240-40

#define TFT_RDX_LO	   {TFT_CTRL_PORT &=~(1 << TFT_RDX);}
#define TFT_RDX_HIGH	{TFT_CTRL_PORT |=(1 << TFT_RDX);}

#define TFT_WRX_LO	   {TFT_CTRL_PORT &=~(1 << TFT_WRX);}
#define TFT_WRX_HIGH	{TFT_CTRL_PORT |=(1 << TFT_WRX);}

#define TFT_DCX_LO	   {TFT_CTRL_PORT &=~(1 << TFT_DCX);}
#define TFT_DCX_HIGH	{TFT_CTRL_PORT |=(1 << TFT_DCX);}

#define TFT_CSX_LO	   {TFT_CTRL_PORT &=~(1 << TFT_CSX);}
#define TFT_CSX_HIGH	{TFT_CTRL_PORT |=(1 << TFT_CSX);}

#define TFT_RST_LO	   {TFT_CTRL_PORT &=~(1 << TFT_RST);}
#define TFT_RST_HIGH	{TFT_CTRL_PORT |=(1 << TFT_RST);}

#pragma used+
//void InitInterface(void);
//void TFT_FillScreen(unsigned int XL, unsigned int XR, unsigned int YU, unsigned int YD, unsigned int color);
////void TFT_drawBmp(unsigned int xLeft, unsigned int yUp, char height, char nuber);
void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
//void TFT_DrawLine(unsigned int x0,unsigned int y0,unsigned int x1, unsigned int y1, unsigned int color);
void ILI9341_DrawRectangle(unsigned int poX, unsigned int poY, unsigned int length, unsigned int width,unsigned int color);
//void TFT_DrawCircleHelper(int x0, int y0, int r, char cornername, unsigned int color);
//void TFT_FillCircleHelper(int x0, int y0, int r, char cornername, int delta, unsigned int color);
//void TFT_DrawCircle(int x0, int y0, int r, unsigned int color);
//void TFT_FillCircle(unsigned int poX, unsigned int poY, unsigned int r, unsigned int color);
//void TFT_DrawRoundRect(int x, int y, int w, int h, int r, unsigned int color);
//void TFT_FillRoundRect(int x, int y, int w, int h, int r, unsigned int color);
//void TFT_DrawChar( unsigned char ascii, unsigned int poX, unsigned int poY, unsigned char size, unsigned char bold, unsigned int fgcolor, unsigned int gcolor);
void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor, uint8_t size);

#endif /* _TFT_INCLUDED_ */