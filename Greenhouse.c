/*******************************************************
Project        : Greenhouse
Version        : 0.2
Date           : 10.08.2023
Chip type      : ATmega32
Clock frequency: 16,000000 MHz
Program size   : 14850 words (29700 bytes), 90,6% of FLASH  13.08.2023
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
#define CONECT          PINB.3    // ���� bluetooth ��������� �� ����� +3,3�
#define MAX_DS18B20     2
#define MENU            7
#define LIST0           7
#define LIST1           6
#define LIST2           6
//#define LIST3           5
#define LIST4           5
#define ZERO	        50

#define DATAREAD        0xA1    // Read Scratchpad
#define SPI_MOUD_DSP    0xD2      // Mode 0; Clock Rate: 250,000 kHz; SPI Interrupt Enable.
#define SPI_MOUD_FL	    0x51	  // SPI Type: Master, Clock Rate: 1000,000 kHz, Clock Phase: Cycle Half, Clock Polarity: Low, Data Order: MSB First
#define ADC_VREF_TYPE   0x40
#define F_XTAL			16000L               //quartz crystal frequency [KHz]
#define INIT_TIMER0     TCNT0 =   0x100L-F_XTAL/1024L*5L    // 5 ms

#define DS_SRTC         0xD0    // Serial Real Time Clock (������� ����������)
#define CLOCK_BUFFER	8
#define SQWE_32768Hz    0x13
#define SQWE_1Hz        0x10
#define OUT_OFF         0x80

// Declare your global variables here
//flash float A1=1.8, A2=0.81, A3=0.01;  // ������� a=0.9 (A1=2a; A2=a^2; A3=(1-a)^2)
//flash float B1=1.6, B2=0.64, B3=0.04;  // ������� a=0.8 (A1=2a; A2=a^2; A3=(1-a)^2)
flash float A1=1.2, A2=0.36, A3=0.16;  // ������� a=0.6 (A1=2a; A2=a^2; A3=(1-a)^2)
unsigned char BeepT, keynum, keycount, keywait, displwait, relayOut, ds18b20, signchar, intval, frcval, errors, noAutoRel, noAutoAna;
signed char numMenu, subMenu, numSet, displ_num, moduleEdit, soilModule, DHTexist;
// ����� relOut[8] ���������� ������� �� 8 �����!
unsigned char relOut[8]={0}, analogOut[4]={0}, dacU[4]={ZERO}, buff[40], familycode[MAX_DS18B20][9], clock_buffer[CLOCK_BUFFER];
unsigned int  max_X, max_Y, timerOn, timerOff, timerCount, rhCount;
signed int pvT=1990, pvRH=1990, offsetRH, valRun, valPeriod, newval[7], t[MAX_DS18B20], hum[MAX_DS18B20], p[2];

const char* setMenu[MENU]={"�����������","���������","������","���� ���","��� ����","����","������"};
const char* setName0[LIST0]={"����","���","������","������","�����","������","�����"};
const char* setName1[LIST1]={"���������","�����","��������","�����","����","��������"};
const char* setName2[LIST2]={"���� �����","��� �����","��������� �","�������� �","��������� �","�������� �"};
//const char* setName3[LIST3]={"���.1","���.2","���.3","���.4","���.2"};
const char* setName7[LIST4]={"�������","������","����","�����","���"};
//--------------- union declaration -----------------------------------------------
union {unsigned char buffer[8]; unsigned int pvT;} ds;          // ������ ���������� DS18B20
union {unsigned char buffer[4]; unsigned int val[2];} in;
union {unsigned char buffer[4]; unsigned int val[2];} out;
//---------------------------------------------------------------------------------
float Told1[8], Told2[8], iPart[2];

//-------------------------
eeprom signed char relaySet[4]={-1,-1,-1,-1};
eeprom signed char analogSet[4]={-1,-1,-1,-1};

eeprom signed int set[6][7]={
 {240, 200,  30,  10,   1,  -1,   0},  // (����.)  Tday;  Tnight;  dTalarm; hysteresis; mode=1(������)/mode=0(����������); ������; ����� �
 {100,  90,  10,   5,   2,   0,   1},  // (����.) RHday; RHnight; dRHalarm; hysteresis; mode=2(�������)/mode=1(���)/mode=0(����); ���������; ����� �
 {200, 180,  50,  10,   1,  -1,   4},  // (�����)  Tday;  Tnight;  dTalarm; hysteresis; mode=1(������)/mode=0(����������); ������; ����� � 4=>0x10
 {400, 350, 100,  50,   1,  -1,   5},  // (�����) RHday; RHnight;  dTalarm; hysteresis; mode=1(����������)/mode=0(��������); ������; ����� � 5=>0x20  
 { 10,  0,   10,   1,   0,0x06,   6},  //   tmOn; dimOn=0(���.)/dim=1(���.); tmOff;    dimOff; HourStart;  Programm; ����� � 6=>0x40
 {0x07,0x20,0x05,0x09,0x18,0x23,   7}}; // DayBeg;           DayEnd;      Light0Beg; Light0End; Light1Beg; Light1End; ����� � 8=>0x80

eeprom unsigned char analog[6][4]={
                //     dT min max  kP 
                      {20,  0, 100, 20}, // 0 - ���������� ����� I   ��������� ����������
                      {10,  0, 100, 21}, // 1 - ���������� ����� II  ��������� �������� �������� ����������
                      {15,  0, 100, 22}, // 2 - ���������� ����� III ��������� �������� ��������� ����������
                      { 5,  0, 100, 23}, // 3 - ���������� ����� IV  ��������� ������� ������� ����.
                      { 1, 30,   1,  5}, // minRun=1, maxRun=30, minPeriod=1*60, Period=5*60;
                      { 5,100,  30,100}};// K1; Ti1*10; K2; Ti2*10;
                      
eeprom unsigned int module[4][2]={
                      { 10,600}, // ����� ��������� 100%: 0% OutMIN; OutMAX; ������ �1
                      { 10,600}, // ����� ��������� 100%: 0% OutMIN; OutMAX; ������ �2
                      { 10,600}, // ����� ��������� 100%: 0% OutMIN; OutMAX; ������ �3
                      { 10,600}};// ����� ��������� 100%: 0% OutMIN; OutMAX; ������ �4

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
bit Display;      // �������� �������
bit Dht;
bit Clock_Ok;
bit pHsensor;
bit CO2module;       // ��������� ���������� ��2
bit typeS;           // DHT11/DHT22
bit NewnumMenu;
bit newSetButt;

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
 if(timerCount) --timerCount;
 if(rhCount) --rhCount;
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)  // �������� 5 mS ����������� ������� �����
{
  RCK = 1;
  INIT_TIMER0;
  if(BeepT){--BeepT; BEEP = ON;} else BEEP = OFF;		    // �������� ������  1; ��������� ������ 0      �����  1,9-2,44 us
  if(keywait) --keywait;
  else {SCK = 1; SCK = 0; SPCR=SPI_MOUD_DSP; RCK = 0; SPDR=0xFF;}  // ����������� �������� �������� ���������� + ������ �������� �� SPI
}

// 0.25 Hz timer interrupt generated by Timer1 overflow;  Timer Period: 1,0486 s
interrupt [TIM1_OVF] void timer_comp_isr(void){
  Sec=1; Display=1;
}

// SPI interrupt service routine
interrupt [SPI_STC] void spi_isr(void){
 unsigned char data;
  SPCR=0;           // SPI disabled
  data = ~SPDR;     // ������ ����������
  if(data) keycount++; else keycount=0;
  if(keycount>2){keynum=data; BeepT=25; keywait=255; Display=1;}  
} 


void main(void){
// Declare your local variables here
signed char x, byte;
int temp;
#include "init.c"

while (1){
//--------------------------- ������� 1 ������� ---------------------------------------------
    if(Sec){                     
        Sec=0; noAutoRel=0; noAutoAna=0; errors=0;
        for (byte=0; byte<4; byte++) if(relaySet[byte]>=0)  noAutoRel++; // �������� - ��� �� ��������� � �������������� ������
        for (byte=0; byte<4; byte++) if(analogSet[byte]>=0) noAutoAna++; // �������� - ��� �� ��������� � �������������� ������
        if(displ_num){
            if(displwait) --displwait; else {displ_num=0; newSetButt=1;}     // ������� ������� � �������� ������
        }
        if(clock_buffer[2]>=set[5][0]&&clock_buffer[2]<set[5][1]) Night=0; else Night=1;   // set[5][0]=> DayBeg; set[5][1]=> DayEnd;
    // -- ������ ������� ----------    
        byte = set[4][4];                           // ����� ������� ���� 0 �� ������� ���� 1-14 �� �����������
        if(byte==0) timerCheck();                   // ������� ������
        else timerRTC(byte);                        // ������ �� ���������
    // -- ������ ��������� --------    
        if(clock_buffer[2]>=set[5][2]&&clock_buffer[2]<set[5][3]) x=1; else x=0;            // Light0Beg; Light0End;
        if(x==0){if(clock_buffer[2]>=set[5][4]&&clock_buffer[2]<set[5][5]) x=2; else x=0;}  //Light1Beg; Light1End;
        byte = set[5][6];  // � ������ �������
        if(x>0){
            relOut[byte] = 1;
            byte = 1 << byte;
            relayOut |= byte;
            if(x==1) byte = rtcTodec(set[5][3])-1; else byte = rtcTodec(set[5][5])-1;            
            sprintf(txt,"ON  �����.%02u:59:59",byte);
        } 
        else {
            relOut[byte] = 0;
            byte = 1 << byte;
            relayOut &= ~byte;
            if(Night) byte = rtcTodec(set[5][2]); else byte = rtcTodec(set[5][4]);
            sprintf(txt,"OFF �����.%02u:00:00",byte);
        }
    // -- ��������� ����������� ---------
        if(ds18b20) temperature_check();
        else if(soilModule){
            out.buffer[0] = DATAREAD;
            for (x=0; x < soilModule; x++){
                byte = module_check(ID_SOIL1 + x);
                if(byte){
                    t[x] = LowPassF2(in.val[0],x);
                    hum[x] = LowPassF2(in.val[1],x+4);
                    //t[x] = in.val[0];                  // ������ ��� ����������
                    //hum[x] =in.val[1];                 // ������ ��� ����������
                }
                else {t[x] = 1990; hum[x] = 1990;}    
            }
            //pvT = mean(soilModule);                    // !!! ������� ������
        }
        // -- ��������� ��������� ---------            
        if(Dht){                                         // ������������ ������ ���������
            if(readDHT()) DHTexist = 3; 
            else if(DHTexist) DHTexist--;                // ������ ��������� ����������. 
            else {pvT = 1900; pvRH = 190;}
        }
        else {pvT = t[0]; pvRH = tableRH(t[0],t[1]);}    // !!! ������� ������ � soilModule � mean() !!!!!!!
        // --------����� ����������� ������� ����� x � ����� x ---------
         RelaySensor(pvT,0); analogOut[0]=UpdatePI(pvT,0);
        // --------����� ��������� ������� ����� x � ����� x --------- 
        RelaySensor(pvRH,1);
        // --------����� ����������� ������ ����� x ---------
        //if(ds18b20) analogOut[2]=UpdatePI((t[0]+t[1])/2,2);  // ������� ������
        // --------����� ��������� ������ ����� x ---------
        //if(ds18b20) analogOut[3] = UpdatePI((t.point[0]+t.point[1])/2,3);  // ������� ������
        byte = set[0][2];                           // ������� ���������� �����������
        temp = pvT - set[0][Night];
        if(abs(temp)>byte) errors |= 0x10;          // ������� ���������� �����������
        byte = set[1][2];                           // ������� ���������� ���������
        temp = pvRH - set[1][Night];
        if(abs(temp)>byte) errors |= 0x20;          // ������� ���������� ���������
                
        for (byte=0;byte<4;byte++) if(analogSet[byte]>=0) dacU[byte]=adapt(analogSet[byte]); else dacU[byte] = adapt(analogOut[byte]);         
    //------ �������� �� ������ ���������� --------------------------------------------------------        
        for (byte=0;byte<4;byte++){
            if(relaySet[byte]==1) {relOut[byte]=ON;  relayOut |= (1<<(byte));} // ������ On
            if(relaySet[byte]==0) {relOut[byte]=OFF; relayOut &= ~(1<<(byte));}// ������ Off
        }        
    //------------------------ ���������� ����� ���������� --------------------------------------
        if(set[1][4]==2 && rhCount==0){
            byte = set[1][6];               // � ������ �����������
            if(relayOut&(1<<byte)){
                relOut[byte]=OFF;
                relayOut &= ~(1<<(byte));   // ���� 1 ��...
                rhCount = valPeriod;    
            }
            else if(valRun){
                relOut[byte]=ON;
                relayOut |= (1<<(byte));    // ���� 0 ��...
                rhCount = valRun;          
            }
        }

        byte = PORTA & 0x0F;
        x = (relayOut<<4)&0xF0;
        x |= byte;
//        PORTA = x;  // PORA.4->��. I -- PORA.7->��. IV
//        LOCK=1; delay_us(1); LOCK=0;
        //setDAC();                           // ������ ���������� �� ���������� ������
    }
//------------------------- ����� ������� 1 ������� ---------------------------------------------
    if(Display){
      Display=0; display();
      //----------
      sprintf(buff,"P1 %6i P2 %6i",p[0],p[1]);
      ILI9341_WriteString(5,180,buff,Font_11x18,WHITE,BLACK,1);
      sprintf(buff,"Out %02x RhC%5u R%3u P%5u",relayOut,rhCount,valRun,valPeriod);
      ILI9341_WriteString(5,200,buff,Font_11x18,WHITE,BLACK,1);
      sprintf(buff,"Dn%2u nM%2u sM%2u nS%2u Er0x%02x",displ_num,numMenu,subMenu,numSet,errors);
      ILI9341_WriteString(5,220,buff,Font_11x18,WHITE,BLACK,1);
      //----------
    }

 }
}
