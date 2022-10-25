#define YP_DIR          DDRA.TFT_WRX   // TFT_WRX=6
#define YP_PORT         PORTA.TFT_WRX  // TFT_WRX=6
#define YP_PIN          TFT_WRX        // = 6

#define YM_DIR          DDRC.7
#define YM_PORT         PORTC.7
  
#define XP_DIR          DDRC.6
#define XP_PORT         PORTC.6

#define XM_DIR          DDRA.TFT_DCX   // TFT_DCX=5
#define XM_PORT         PORTA.TFT_DCX  // TFT_DCX=5
#define XM_PIN          TFT_DCX        // = 5

#define MINPRESSURE     10
#define MAXPRESSURE     1000
#define INPUT           0
#define OUTPUT          1
#define LOW             0
#define HIGH            1

#define TS_MINX        125
#define TS_MAXX        907
#define TS_MINY        130
#define TS_MAXY        935

unsigned char buttonCount, plusCount;
unsigned int point_X, point_Y, point_Z, bottom_Y, left_X;
struct {int x,y; char w,h;} buttons[4];// Global structure located in RAM
struct {int x,y; char w,h;} plus[16];  // Global structure located in RAM

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
{
adc_input |= (ADC_VREF_TYPE & 0xff);
ADMUX=adc_input;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=0x40;
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}

void initializePlus(char y, char row, char h){// высота кнопки
 char i,j,indx=0;
 unsigned int x;
    if(h<14) h=14;
    if(y>3)  y-=4;  // Y начало контура кнопки
    for (j=0; j<row; j++){
    x = TFTWIDTH-90;// X начало 1 кнопки
    for (i=0; i<2; i++){
        plus[indx].x = (int)x+i*45;// интервал между кнопками
        plus[indx].w = 40;
        plus[indx].h = h;
        plus[indx].y = y;
        indx++;
    }
    y += (h+6);// интервал между кнопками
    }
    left_X = x;// лева€ граница до которой можно закрашивать экран
    plusCount = row*2;// обшее количество кнопок
}

void initializeButtons(char col, char row, char h){// высота кнопки
 char i,j,indx, w;
 unsigned int x,y;
    switch (col){                  // ширина кнопки зависит от кол-ва кнопок в строке
        case 4:  w = max_X/4-6; break;  // 72
        case 3:  w = max_X/3-6; break;  // 100
        case 2:  w = max_X/2-9; break;  // 150 
        default: w = max_X-6;
    };
    if(h<20) h=20;
    y = max_Y - h - 4;      // начало контура кнопки
    indx = 0;
    for (j=0; j<row; j++){
        x = 4;// начало 1 кнопки
        for (i=0; i<col; i++){
            buttons[indx].x = (int)x+i*(w+5);// интервал между кнопками
            buttons[indx].w = w;
            buttons[indx].h = h;
            buttons[indx].y = y;
            indx++;
        }
        y -= (h+5);// интервал между кнопками
    }
    bottom_Y = y;// верхн€€ граница меню до которой можно закрашивать экран
    buttonCount = col * row;// обшее количество кнопок
}
//---------------- цвет фона ------- цвет рамки -------- цвет текста --- номер --- текст ---------
void drawButton(unsigned int fill, unsigned int bord, unsigned int text, char b, char flash *str){
 int x, y, w, h;
   w = buttons[b].w;      // ширина кнопки
   h = buttons[b].h;      // высота кнопки
   x = buttons[b].x;      // начало контура кнопки
   y = buttons[b].y;      // начало контура кнопки
   //TFT_FillRoundRect(x, y, w, h, r, fill);
   ILI9341_FillRectangle(x, y, w, h, fill);
   //TFT_DrawRoundRect(x, y, w, h, r, bord);
   ILI9341_DrawRectangle(x, y, w, h, bord);
#pragma warn-
   x = x + w/2 - strlenf(str)*5;
   y = y + h/2 - 8;
#pragma warn+  
   sprintf(buff,str); ILI9341_WriteString(x, y, buff, Font_11x18, text, fill, 1);
}

void drawPlus(char i, unsigned int fill){
 int x, y, w, h;
   w = plus[i].w;      // ширина кнопки
   h = plus[i].h;      // высота кнопки
   x = plus[i].x;      // начало контура кнопки
   y = plus[i].y;      // начало контура кнопки
   if(i%2) ILI9341_DrawRectangle(x, y, w, h, BLUE);
   else ILI9341_DrawRectangle(x, y, w, h, RED); 
#pragma warn-
   x = x + w/2 - 5;
   y = y + h/2 - 8;
#pragma warn+  
   if(i%2) ILI9341_WriteString(x, y, "-", Font_11x18, BLUE, fill, 1);
   else ILI9341_WriteString(x, y, "+", Font_11x18, RED, fill, 1);
}

//x:    переменна€, значение которой необходимо преобразовать
//in_min:  нижний предел текущего диапазона переменной value
//in_max: верхний предел текущего диапазона переменной value
//out_min:    нижний предел нового диапазона переменной value
//out_max:   верхний предел нового диапазона переменной value
unsigned int map(unsigned int x, unsigned int in_min, unsigned int in_max, char out_min, unsigned int out_max){
 long val;
    val = (x - in_min); val *= (out_max - out_min); val /= (in_max - in_min); val += out_min;
    return val;
}

char contains(char b){// проверка попадани€ пересчитаной координаты в область кнопки.
 int beg, end;
    beg = buttons[b].x-3;
    end = beg + buttons[b].w;
    if ((point_X < beg)||(point_X > end)) return 0;
    beg = buttons[b].y-3;
    end = beg + buttons[b].h;
    if ((point_Y < beg)||(point_Y > end)) return 0;
    return 1;
 }
 
char containsPlus(char b)// проверка попадани€ пересчитаной координаты в область кнопки.
 {
 int beg, end;
   beg = plus[b].x-3;
   end = beg + plus[b].w+3;
   if ((point_X < beg)||(point_X > end)) return 0;
   beg = plus[b].y-3;
   end = beg + plus[b].h+3;
   if ((point_Y < beg)||(point_Y > end)) return 0;
   return 1;
 }

char checkTouch(void)// вычисление кординаты касани€ резистивной матрицы (point_Y, point_X)
{
char i;
int val, samples[2];
//------- Set X+ to ground --------------------
  XP_DIR = OUTPUT; // DDRC.6 ??????????????????
  XP_PORT = LOW;   // DDRC.6
//------- Set Y- to VCC -----------------------
  YM_DIR = OUTPUT; // DDRC.7 ??????????????????
  YM_PORT = HIGH;  // DDRC.7 ??????????????????
//------- Hi-Z X- and Y+ ----------------------
  YP_DIR = INPUT;  // TFT_WRX
  YP_PORT = LOW;   // TFT_WRX
  XM_DIR = INPUT;  // TFT_DCX
  XM_PORT = LOW;   // TFT_DCX
  val = read_adc(YP_PIN);  // y
  val-= read_adc(XM_PIN);  //(y-x)
  val = 1023-val;          // z
  point_Z = val; 
  
  if(val < 200){
    InitInterface();
//sprintf(buff,"слабое нажатие v=%3u",val);
//ILI9341_WriteString(5,TFTBUTTON-45,buff,Font_11x18,WHITE,BLACK,1); 
    return 0;         // если слабое нажатие то выход
  }
  else{                // иначе определ€ем координаты
     YP_DIR = INPUT;   // TFT_WRX
     YP_PORT = LOW;    // TFT_WRX
     YM_DIR = INPUT;   // DDRC.7
     YM_PORT = LOW;    // DDRC.7
     XP_DIR = OUTPUT;  // DDRC.6
     XP_PORT = HIGH;   // DDRC.6
     XM_DIR = OUTPUT;  // TFT_DCX
     XM_PORT = LOW;    // TFT_DCX
     do{
        for (i=0; i<2; i++){samples[i] = read_adc(YP_PIN);}
        val = abs(samples[0]-samples[1]);
     } while (val>4);
    //point_Y = (1023-(samples[0]+samples[1])/2);  // Orientation=0
     point_X = (samples[0]+samples[1])/2;           // Orientation=3

     XP_DIR = INPUT;  // DDRC.6
     XP_PORT = LOW;   // DDRC.6
     XM_DIR = INPUT;  // TFT_DCX
     XM_PORT = LOW;   // TFT_DCX
     YP_DIR = OUTPUT; // TFT_WRX
     YP_PORT = HIGH;  // TFT_WRX
     YM_DIR = OUTPUT; // DDRC.7
     YM_PORT = LOW;   // DDRC.7
     do{
        for (i=0; i<2; i++){samples[i] = read_adc(XM_PIN);}
        val = abs(samples[0]-samples[1]);
     } while (val>4);
     //point_X = (1023-(samples[0]+samples[1])/2);   // Orientation=0
     point_Y = (1023-(samples[0]+samples[1])/2);     // Orientation=3
     
     InitInterface();
     point_X = map(point_X, TS_MINX, TS_MAXX, 0, max_X);// пересчет координаты резистивной матрицы на пиксельную метрицу
     point_Y = map(point_Y, TS_MINY, TS_MAXY, 0, max_Y);// пересчет координаты резистивной матрицы на пиксельную метрицу
//ILI9341_FillRectangle(point_X, point_Y, 5, 5, RED);
//sprintf(buff,"X%4u; Y%4u",point_X,point_Y);
//ILI9341_WriteString(5,TFTBUTTON-15,buff,Font_11x18,WHITE,BLACK,1);
     return 1;
  }
}

