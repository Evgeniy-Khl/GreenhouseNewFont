/* vim: set ai et ts=4 sw=4: */
//#include "stm32f1xx_hal.h"
#include "ili9341.h"

void ILI9341_WriteCommand(unsigned char cmd) {
// 8080 MCU 8-bit bus interface I (page 27)
  TFT_CTRL_PORT = ((TFT_CTRL_PORT & 0b11111100) | (cmd & 0b00000011));// DATA[0..1]
  TFT_DATA_PORT = ((TFT_DATA_PORT & 0b00000011) | (cmd & 0b11111100));// DATA[2..7]
  TFT_CSX_LO;  // enable ILI9341 chip
  TFT_DCX_LO;  // commands
  TFT_RDX_HIGH;
  TFT_WRX_LO;
  #asm("nop");
  TFT_WRX_HIGH;// parallel data write strobe
}

void ILI9341_WriteByte(unsigned char data) {
// 8080 MCU 8-bit bus interface I
  TFT_CTRL_PORT = ((TFT_CTRL_PORT & 0b11111100) | (data & 0b00000011));// DATA[0..1]
  TFT_DATA_PORT = ((TFT_DATA_PORT & 0b00000011) | (data & 0b11111100));// DATA[2..7]
  TFT_DCX_HIGH;// display RAM data or commandТs parameters
  TFT_WRX_LO;
  #asm("nop");
  TFT_WRX_HIGH;// parallel data write strobe
}

void ILI9341_SetCol(unsigned int startCol,unsigned int endCol){
 ILI9341_WriteCommand(0x2A);// Column Command address
 ILI9341_WriteByte(startCol>>8); ILI9341_WriteByte(startCol&0xFF);
 ILI9341_WriteByte(endCol>>8); ILI9341_WriteByte(endCol&0xFF);
 TFT_CSX_HIGH;// disable ILI9341 chip
}

void ILI9341_SetRow(unsigned int startRow,unsigned int endRow){
 ILI9341_WriteCommand(0x2B);// Page Command address
 ILI9341_WriteByte(startRow>>8); ILI9341_WriteByte(startRow&0xFF);
 ILI9341_WriteByte(endRow>>8); ILI9341_WriteByte(endRow&0xFF);
 TFT_CSX_HIGH;// disable ILI9341 chip
}

static void ILI9341_SetAddressWindow(unsigned int startCol, unsigned int startRow, unsigned int endCol, unsigned int endRow){//0,0,319,239
    ILI9341_SetCol(startCol,endCol);     // column address set
    ILI9341_SetRow(startRow,endRow);     // row address set    
    ILI9341_WriteCommand(0x2C); // write to RAM
}

void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
    // обрезание  ILI9341_WIDTH = 320, ILI9341_HEIGHT = 240
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) w = ILI9341_WIDTH - x;
    if((y + h - 1) >= ILI9341_HEIGHT) h = ILI9341_HEIGHT - y;
    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);  //0,0,319,239
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
           ILI9341_WriteByte(color>>8); ILI9341_WriteByte(color&0xFF);
        }
    }
}

void  ILI9341_DrawHorizontalLine( unsigned int poX, unsigned int poY, unsigned int length,unsigned int color) {
unsigned int i;
 ILI9341_SetCol(poX,poX + length);
 ILI9341_SetRow(poY,poY);
 ILI9341_WriteCommand(0x2c);
 for(i=0; i<length; i++){ILI9341_WriteByte(color>>8); ILI9341_WriteByte(color&0xFF);}
 TFT_CSX_HIGH; // disable ILI9341 chip
}

void ILI9341_DrawVerticalLine( unsigned int poX, unsigned int poY, unsigned int length, unsigned int color) {
unsigned int i;
 ILI9341_SetCol(poX,poX);
 ILI9341_SetRow(poY,poY+length);
 ILI9341_WriteCommand(0x2c);
 for(i=0; i<length; i++){ILI9341_WriteByte(color>>8); ILI9341_WriteByte(color&0xFF);}
 TFT_CSX_HIGH; // disable ILI9341 chip
}

void ILI9341_DrawRectangle(unsigned int poX, unsigned int poY, unsigned int length, unsigned int width,unsigned int color){
 ILI9341_DrawHorizontalLine(poX, poY, length, color);
 ILI9341_DrawHorizontalLine(poX, poY+width, length, color);
 ILI9341_DrawVerticalLine(poX, poY, width,color);
 ILI9341_DrawVerticalLine(poX + length, poY, width,color);
}

static void ILI9341_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor, uint8_t size){
    uint32_t i, b, j;
    for(i = 0; i < font.height; i++) {
      if (ch>=32 && ch<127) b = font.data[(ch - 32) * font.height + i];// латиница
      else b = font.data[(ch - 97) * font.height + i];  // кирилица 192 - 96 = 96
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
              ILI9341_FillRectangle(x+j*size,y+i*size,size,size,color);
            } 
            else {
              ILI9341_FillRectangle(x+j*size,y+i*size,size,size,bgcolor);
            }
        }        
    }
}

void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor, uint8_t size){
    if(size==0) size=1;
    while(*str){
        if(x + font.width >= ILI9341_WIDTH){
            x = 0;
            y += font.height*size;
            if(y + font.height >= ILI9341_HEIGHT) break;
            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }
        ILI9341_WriteChar(x, y, *str, font, color, bgcolor, size);
        x += font.width*size;
        str++;
    }
}

void InitInterface(void){
  TFT_CTRL_DIR |= (1 << TFT_RDX) | (1 << TFT_WRX) | (1 << TFT_DCX) | (1 << TFT_CSX) | (1 << TFT_RST);
  TFT_CTRL_DIR |= 0x03;// DATA[0..1]
  TFT_CTRL_PORT |= (1 << TFT_RDX) | (1 << TFT_WRX) | (1 << TFT_DCX) | (1 << TFT_CSX) & (~(1 << TFT_RST));
  TFT_DATA_DIR |= 0xFC;// DATA[2..7]
  TFT_DATA_PORT &= 0x03;// DATA[2..7]
}

unsigned int constrain(unsigned int a, unsigned int b, unsigned int c)
{
  if (a < b) return b;
  if (c < a) return c;
  else return a;
}

void ILI9341_FillScreen(unsigned int XL, unsigned int XR, unsigned int YU, unsigned int YD, unsigned int color)
{
unsigned long XY=0;
unsigned long i=0;
 if(XL > XR){XL = XL^XR; XR = XL^XR; XL = XL^XR;}
 if(YU > YD){YU = YU^YD; YD = YU^YD; YU = YU^YD;}
 XL = constrain(XL, 0, max_X);
 XR = constrain(XR, 0, max_X);
 YU = constrain(YU, 0, max_Y);
 YD = constrain(YD, 0, max_Y);
#pragma warn-
 XY = (XR-XL+1);
 XY = XY*(YD-YU+1);
#pragma warn+
 ILI9341_SetCol(XL,XR);
 ILI9341_SetRow(YU, YD);
 ILI9341_WriteCommand(0x2C); // start to write to display ram
 for(i=0; i < XY; i++){ILI9341_WriteByte(color>>8); ILI9341_WriteByte(color&0xFF);}
 TFT_CSX_HIGH; // disable ILI9341 chip
}

void ILI9341_Init(void) {
//unsigned char orient
  TFT_RST_HIGH;
  delay_ms(1);
  ILI9341_WriteCommand(0x01);
  ILI9341_WriteByte(0x00);
  delay_ms(50);
//DISPLAYOFF
  ILI9341_WriteCommand(0x28);
  ILI9341_WriteByte(0x00);
//Power Control 1
  ILI9341_WriteCommand(0xC0);
  ILI9341_WriteByte(0x23);
//Power Control 2
  ILI9341_WriteCommand(0xC1);
  ILI9341_WriteByte(0x10);
//VCOM Control 1
  ILI9341_WriteCommand(0xC5);
  ILI9341_WriteByte(0x2B);
  ILI9341_WriteByte(0x2B);
//VCOM Control 2
  ILI9341_WriteCommand(0xC7);
  ILI9341_WriteByte(0xC0);
//-MEMCONTROL-----------------
  ILI9341_WriteCommand(ILI9341_MADCTL);
  ILI9341_WriteByte(ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
  max_X=TFTWIDTH-1; max_Y=TFTHEIGHT-1;
  ILI9341_SetCol(0,max_X); ILI9341_SetRow(0,max_Y);
//----------------------------
//COLMOD: Pixel Format Set
  ILI9341_WriteCommand(0x3A);
  ILI9341_WriteByte(0x55);
//Frame Rate Control (In Normal Mode/Full Colors)
  ILI9341_WriteCommand(0xB1);
  ILI9341_WriteByte(0x00);
  ILI9341_WriteByte(0x1B);
//ENTRYMODE
  ILI9341_WriteCommand(0xB7);
  ILI9341_WriteByte(0x07);
  ILI9341_WriteCommand(ILI9341_SLEEPOUT);
  ILI9341_WriteByte(0x00);
  delay_ms(150);
//Display On
  ILI9341_WriteCommand(ILI9341_DISPLAYON);
  ILI9341_WriteByte(0x00);
  delay_ms(500);
}
