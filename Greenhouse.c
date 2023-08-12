/*******************************************************
Project : Greenhouse
Version : 0.2
Date    : 10.08.2023
Clock   : 16 MHz
Program size: 14474 words (28948 bytes), 88,3% of FLASH  10.08.2023
*******************************************************/

#include <mega32.h>
#include <stdio.h>
#include <twi.h>
#include <delay.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <1wire.h>
#include "fontsN.h"
#include "tft.h"
#include "9341.h"
//*********************************
#define RCK		        PORTB.0
#define LOCK	        PORTB.1
#define BEEP		    PORTB.2
#define SCK		        PORTB.7
#define CSDAC1		    PORTD.4
#define CSDAC2		    PORTD.5
#define CSDAC3		    PORTD.6
#define CSDAC4	        PORTD.7
#define CONECT          PINB.3    // если bluetooth подключен то здесь +3,3В
#define MAX_DS18B20     2
#define MENU            7
#define LIST0           7
#define LIST1           6
#define LIST2           6
#define LIST3           4
#define LIST4           5
#define MAX_4           4
#define MAX_5           5
#define MAX_6           6
#define MAX_7           7
#define MISTAKE         3
#define ZERO	        50

#define DATAREAD        0xA1    // Read Scratchpad
#define SPI_MOUD_DSP    0xD2      // Mode 0; Clock Rate: 250,000 kHz; SPI Interrupt Enable.
#define SPI_MOUD_FL	    0x51	  // SPI Type: Master, Clock Rate: 1000,000 kHz, Clock Phase: Cycle Half, Clock Polarity: Low, Data Order: MSB First
#define TWI_CLK_RATE    100000    // TWI clock rate [bps]
#define EEPROM_TWI_BUS_ADDRESS (0xA0 >> 1)// 7 bit TWI bus slave address of the AT24C16B 2kbyte EEPROM
#define ADC_VREF_TYPE   0x40
#define F_XTAL			16000L               //quartz crystal frequency [KHz]
#define INIT_TIMER0     TCNT0 =   0x100L-F_XTAL/1024L*5L    // 5 ms

#define DS_SRTC         0xD0    // Serial Real Time Clock (Часовая микросхема)
#define CLOCK_BUFFER	8
#define SQWE_32768Hz    0x13
#define SQWE_1Hz        0x10
#define OUT_OFF         0x80

// Declare your global variables here
//flash float A1=1.8, A2=0.81, A3=0.01;  // порядок a=0.9 (A1=2a; A2=a^2; A3=(1-a)^2)
//flash float B1=1.6, B2=0.64, B3=0.04;  // порядок a=0.8 (A1=2a; A2=a^2; A3=(1-a)^2)
flash float A1=1.2, A2=0.36, A3=0.16;  // порядок a=0.6 (A1=2a; A2=a^2; A3=(1-a)^2)
unsigned char BeepT, keynum, keycount, keywait, relayOut, newSetButt, ds18b20, signchar, intval, frcval, errors, noAutoRel, noAutoAna;
signed char numMenu, subMenu, numSet, displ_num, moduleEdit;

unsigned char ok, portOut, soilModule, DHTexist;
unsigned char relOut[4]={0}, analogOut[4]={0}, dacU[4]={ZERO}, buff[40], familycode[MAX_DS18B20][9], clock_buffer[CLOCK_BUFFER], alarm[4]={2,2,2,2};
unsigned int  max_X, max_Y, timerOn, timerOff, fillScreen = BLACK;
signed int pvT=1990, offsetT, pvRH=1990, offsetRH, pvCO2, pvPH, newval[MAX_7], t[MAX_DS18B20], hum[MAX_DS18B20];
unsigned char *ptr_char;
const char* setMenu[MENU]={"Температура","Вологысть","Таймер","День Ныч","Час Дата","Ынше","Модуль"};
const char* setName0[LIST0]={"ДЕНЬ","НЫЧ","Выдхил","Гыстер","Режим","Резерв","Вихыд"};
const char* setName1[LIST1]={"Увымкнено","Розмір","Вимкнено","Розмір","Крок","ЗМЫЩЕННЯ"};
const char* setName2[LIST2]={"День почат","Ныч почат","Увымкнено Р","Вимкнено Р","Увымкнено В","Вимкнено В"};
const char* setName3[LIST3]={"MIN","MAX","КОФ.1","Коф.2"};
const char* setName7[LIST4]={"Хвилини","Години","День","Мысяц","Рык"};
//--------------- union declaration -----------------------------------------------
union {unsigned char buffer[8]; unsigned int pvT;} ds;          // буффер микросхемы DS18B20
union {unsigned char buffer[4]; unsigned int val[2];} in;
union {unsigned char buffer[4]; unsigned int val[2];} out;
//---------------------------------------------------------------------------------
float Told1[8], Told2[8], iPart[4];

//-------------------------
eeprom signed char relaySet[4]={-1,-1,-1,-1};
eeprom signed char analogSet[4]={-1,-1,-1,-1};

eeprom signed int set[6][7]={
{ 230, 200,  50,  10,   1,  -1,   0},  // (ВОЗД.) Tday;  Tnight;  dTalarm;  hysteresis;  mode=1(нагрев)/mode=0(охлаждение); резерв;    выход № РЕЛЕ1
{  60,  60,  10,   5,   0,   0,   1},  // (ВОЗД.) RHday; RHnight; dRHalarm; hysteresis;  mode=1(увлажнение)/mode=0(осушение); DHT22=0; выход № РЕЛЕ2
{ 200, 180,  50,  10,   1,  -1,   6},  // (ГРУНТ) Tday;  Tnight;  dTalarm;  hysteresis;  mode=1(нагрев)/mode=0(охлаждение); резерв;    выход №
{ 400, 350, 100,  50,   1,  -1,   7},  // (ГРУНТ) RHday; RHnight;  dTalarm;  hysteresis; mode=1(увлажнение)/mode=0(осушение); резерв;  выход №   
{  10,  0,   10,   1,   0,0x06,   2},  // tmOn; dimOn=0(сек.)/dim=1(мин.); tmOff; dimOff; HourStart; Programm;                         выход № РЕЛЕ3
{0x07,0x20,0x05,0x09,0x18,0x23,   3}}; // DayBeg; DayEnd; Light0Beg; Light0End; Light1Beg; Light1End;                                  выход № РЕЛЕ4

eeprom unsigned char limit[5][4]={
                    // min max  kP   kI 
                      {  0,100, 20, 100}, // 4
                      {  0,100, 21, 100}, // 5
                      {  0,100, 22, 100}, // 6
                      {  0,100, 23, 100}, // 7
                      {  0, 30, 66, 138}}; // Грунт температура  t=0 -> V=1.32 -> ADC=270; t=25 -> V=2.51 -> ADC=514

eeprom unsigned int module[4][2]={
                      { 10,600}, // Грунт влажность 100%: 0% OutMIN; OutMAX; модуль №1
                      { 10,600}, // Грунт влажность 100%: 0% OutMIN; OutMAX; модуль №2
                      { 10,600}, // Грунт влажность 100%: 0% OutMIN; OutMAX; модуль №3
                      { 10,600}};// Грунт влажность 100%: 0% OutMIN; OutMAX; модуль №4

eeprom unsigned char tabRH[420]={
    95,90,86,81,77,72,68,64,60,56,52,48,44,40,36,32,29,25,22,18,
    95,91,86,82,77,73,69,65,61,57,53,49,45,42,38,34,31,27,24,20,
    95,91,87,82,78,74,70,66,62,58,54,50,47,43,40,36,33,29,26,23,
    96,91,87,83,79,75,71,67,63,59,55,52,48,45,41,38,34,31,28,25,
    96,91,87,83,79,75,71,67,64,60,56,53,49,46,43,39,36,33,30,26,
    96,92,88,84,80,76,72,68,65,61,57,54,51,47,44,41,37,34,31,28,
    96,92,88,84,80,76,73,69,65,62,58,55,52,48,45,42,39,36,33,30,
    96,92,88,84,80,77,73,70,66,63,59,56,53,50,46,43,40,37,34,32,
    96,92,88,85,81,77,74,70,67,64,60,57,54,51,48,45,42,39,36,33,
    96,92,89,85,81,78,74,71,68,64,61,58,55,52,49,46,43,40,37,35,
    96,92,89,85,82,78,75,71,68,65,62,59,56,53,50,47,44,41,39,36,
    96,93,89,85,82,79,75,72,69,66,63,60,57,54,51,48,45,42,40,37,
    96,93,89,86,82,79,76,73,69,66,63,60,57,55,52,49,46,44,41,38,
    96,93,89,86,83,79,76,73,70,67,64,61,58,55,53,50,47,45,42,40,
    96,93,90,86,83,80,77,74,71,68,65,62,59,56,54,51,48,46,43,41,
    97,93,90,87,83,80,77,74,71,68,65,62,60,57,54,52,49,47,44,42,
    97,93,90,87,84,80,77,74,72,69,66,63,60,58,55,53,50,48,45,43,
    97,93,90,87,84,81,78,75,72,69,66,64,61,58,56,53,51,48,46,44,
    97,93,90,87,84,81,78,75,72,70,67,64,62,59,57,54,52,49,47,45,
    97,94,90,87,84,81,79,76,73,70,67,65,62,60,57,55,52,50,48,46,
    97,94,91,88,85,82,79,76,73,71,68,65,63,60,58,56,53,51,49,46
};

bit Night;
bit Sec;
bit Display;      // обновить дисплай
bit Dht;
bit Clock_Ok;
bit pHsensor;
bit CO2module;       // подключен измеритель СО2
bit typeS;           // DHT11/DHT22
bit NewnumMenu;

//- prototypes ------
void display(void);

#include "fontsN.c"
#include "ili9341.c"
#include "twi.c"
#include "dht.c"
//#include "buttons.c"
#include "proc.c"
#include "control.c"
#include "keypad.c"
#include "displ.c"

// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void){
 static unsigned int count1;
 count1++;
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)  // интервал 5 mS отсчитывает текущее время
{
  RCK = 1;
  INIT_TIMER0;
  if(BeepT){--BeepT; BEEP = ON;} else BEEP = OFF;		    // включить серену  1; отключить серену 0      конец  1,9-2,44 us
  if(keywait) --keywait;
  else {SCK = 1; SCK = 0; SPCR=SPI_MOUD_DSP; RCK = 0; SPDR=0xFF;}  // паралельная загрузка значения клавиатуры + запуск передачи по SPI
}

// 0.25 Hz timer interrupt generated by Timer1 overflow;  Timer Period: 1,0486 s
interrupt [TIM1_OVF] void timer_comp_isr(void){
  Sec=1; Display=1;
}

// SPI interrupt service routine
interrupt [SPI_STC] void spi_isr(void){
 unsigned char data;
  SPCR=0;           // SPI disabled
  data = ~SPDR;     // данные клавиатуры
  if(data) keycount++; else keycount=0;
  if(keycount>2){keynum=data; BeepT=25; keywait=255; Display=1;}  
} 


void main(void){
// Declare your local variables here
signed char x, byte;
#include "init.c"

while (1){
//--------------------------- функция 1 секунда ---------------------------------------------
    if(Sec){                     
        Sec=0;
        if(clock_buffer[2]>=set[5][0]&&clock_buffer[2]<set[5][1]) byte=0; else byte=1;
        if(byte!=Night){Night = byte; ok = 0;}      // день / ночь
    // -- работа таймера ----------    
        byte = set[4][4];                           // режим таймера если 0 то простой если 1-14 то программный
        if(byte==0) timerCheck();                   // простой таймер
        else timerRTC(byte);                        // таймер по программе
    // -- работа освещения --------    
        if(clock_buffer[2]>=set[5][2]&&clock_buffer[2]<set[5][3]) x=1; else x=0; // Light0Beg; Light0End;
        if(x==0){if(clock_buffer[2]>=set[5][4]&&clock_buffer[2]<set[5][5]) x=2; else x=0;}//Light1Beg; Light1End;
        byte = set[5][6];  // № выхода таймера
        if(x>0){
            relOut[byte] = 1;
            byte = 1 << byte;
            portOut |= byte;
            if(x==1) byte = rtcTodec(set[5][3])-1; else byte = rtcTodec(set[5][5])-1;            
            sprintf(txt,"ON  вимкн.%02u:59:59",byte);
        } 
        else {
            relOut[byte] = 0;
            byte = 1 << byte;
            portOut &= ~byte;
            if(Night) byte = rtcTodec(set[5][2]); else byte = rtcTodec(set[5][4]);
            sprintf(txt,"OFF увымк.%02u:00:00",byte);
        }
    // -- измерение температуры ---------
        if(ds18b20){
            temperature_check();
//            if(ds18b20>1) pvT = mean(ds18b20-1); else pvT = t[0]; 
        }
        else if(soilModule){
            out.buffer[0] = DATAREAD;
            for (x=0; x < soilModule; x++){
                byte = module_check(ID_SOIL1 + x);
                if(byte){
                    t[x] = LowPassF2(in.val[0],x);
                    hum[x] = LowPassF2(in.val[1],x+4);
//                    t[x] = in.val[0];
//                    hum[x] =in.val[1];
                }
                else {t[x] = 1990; hum[x] = 1990;}    
            }
        }
        // -- измерение влажности ---------            
        if(Dht){                                         // присутствует датчик влажности
            if(readDHT()) DHTexist = 3; 
            else if(DHTexist) DHTexist--;                // датчик влажности работает? 
            else {pvT = 1900; pvRH = 190;}
        }
        else {pvT = t[0]; pvRH = tableRH(t[0],t[1]);} 
        // --------КАНАЛ температура воздуха ВЫХОД 0 и ВЫХОД 4 ---------
         if(Dht){RelaySensor(pvT,0); analogOut[0]=UpdatePI(pvT,0);}
         else if(ds18b20){
            RelaySensor((t[0]+t[1])/2,0);  // средняя грунта
            analogOut[0]=UpdatePI((t[0]+t[1])/2,0);
         }
        // --------КАНАЛ влажность воздуха ВЫХОД 1 и ВЫХОД 5 --------- 
        if(Dht){RelaySensor(pvRH,1); analogOut[1]=UpdatePI(pvRH,1);}
        // --------КАНАЛ температура грунта ВЫХОД 6 ---------
        if(ds18b20) analogOut[2]=UpdatePI((t[0]+t[1])/2,2);  // средняя грунта
        // --------КАНАЛ влажность грунта ВЫХОД 7 ---------
        //if(ds18b20) analogOut[3] = UpdatePI((t.point[0]+t.point[1])/2,3);  // средняя грунта
// ================ Управление исполнительными механизмами ======================================
        for(byte=0; byte<4; byte++){
            if(relaySet[byte]<2) relOut[byte]=relaySet[byte];
            if(analogSet[byte]>=0) analogOut[byte] = analogSet[byte]; 
            else analogOut[byte] = limitationOut(analogOut[byte], byte);            
            dacU[byte] = adapt(analogOut[byte]);// конверсия для ЦАП
        }
//        LOCK = OFF;
//        PORTC = portOut<<2;                   // подать напряжение на цифровые выходы
//        LOCK = ON;
//        setDAC();                           // подать напряжение на аналоговые выходы
    }
//------------------------- КОНЕЦ функция 1 секунда ---------------------------------------------
    if(Display){
      Display=0; display();
      //----------
//      sprintf(buff,"rOut 0x%02x",relayOut&0xF0);
//      ILI9341_WriteString(5,200,buff,Font_11x18,WHITE,BLACK,1);
      sprintf(buff,"Dn%2u nM%2u sM%2u nS%2u Er%3u",displ_num,numMenu,subMenu,numSet,errors);
      ILI9341_WriteString(5,220,buff,Font_11x18,WHITE,BLACK,1);
      //----------
    }

 }
}
