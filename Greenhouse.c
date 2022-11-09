/*******************************************************
Project : Greenhouse
Version : 0.0
Date    : 07.10.2022
Clock   : 16 MHz
Program size: 15690 words (31380 bytes), 95,8% of FLASH  08.11.2022
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
#define LOCK	        PORTB.1
#define BEEP		    PORTB.2
#define CSDAC1		    PORTD.4
#define CSDAC2		    PORTD.5
#define CSDAC3		    PORTD.6
#define CSDAC4	        PORTD.7
#define CONECT          PINB.3    // ���� bluetooth ��������� �� ����� +3,3�
#define MAX_DEVICES     4
#define MAX_MENU        7
#define MAX_4           4
#define MAX_5           5
#define MAX_6           6
#define MAX_7           7
#define MISTAKE         3
#define ZERO	        50

#define DATAREAD        0xA1    // Read Scratchpad

#define SPI_MOUD_FL	    0x51	  // SPI Type: Master, Clock Rate: 1000,000 kHz, Clock Phase: Cycle Half, Clock Polarity: Low, Data Order: MSB First
#define TWI_CLK_RATE    100000    // TWI clock rate [bps]
#define EEPROM_TWI_BUS_ADDRESS (0xA0 >> 1)// 7 bit TWI bus slave address of the AT24C16B 2kbyte EEPROM
#define ADC_VREF_TYPE   0x40
#define F_XTAL			16000L               //quartz crystal frequency [KHz]
#define INIT_TIMER0     TCNT0 =   0x100L-F_XTAL/1024L*5L    // 5 ms

#define DS3231          0xD0    // ������� ����������
#define CLOCK_BUFFER	8
#define SQWE_32768Hz    0x13
#define SQWE_1Hz        0x10
#define OUT_OFF         0x80

// Declare your global variables here
//flash float A1=1.8, A2=0.81, A3=0.01;  // ������� a=0.9 (A1=2a; A2=a^2; A3=(1-a)^2)
flash float A1=1.6, A2=0.64, A3=0.04;  // ������� a=0.8 (A1=2a; A2=a^2; A3=(1-a)^2)
//flash float A1=1.2, A2=0.36, A3=0.16;  // ������� a=0.6 (A1=2a; A2=a^2; A3=(1-a)^2)
unsigned char BeepT, displ_num, ok, portOut, newSetButt, ds18b20, soilModule, pointY, DHTexist, signchar, intval, frcval, error;
signed char numMenu, numSet, pauseEdit, moduleEdit/*, displCO2, timerCO2*/;
unsigned char relOut[4]={0}, analogOut[4]={0}, dacU[4]={ZERO}, buff[40], familycode[MAX_DEVICES][9], clock_buffer[CLOCK_BUFFER], alarm[4]={2,2,2,2};
unsigned int  max_X, max_Y, timerOn, timerOff, fillScreen = BLACK;
signed int pvT=1990, offsetT, pvRH=1990, offsetRH, pvCO2, pvPH, newval[MAX_7], t[MAX_DEVICES], hum[MAX_DEVICES];
unsigned char *ptr_char;
const char* setMenu[MAX_MENU]={"�����������","���������","������","���� ���","��� ����","����","������"};
const char* setName0[MAX_7]={"����","���","������.","������.","�����","������","�����"};
const char* setName1[MAX_6]={"���������","�����.","��������","�����.","����","��������"};
const char* setName2[MAX_6]={"���� �����.","��� �����.","��������� �","�������� �","��������� �","�������� �"};
const char* setName3[MAX_4]={"MIN","MAX","���.1","���.2"};
const char* setName7[MAX_5]={"�������","������","����","�����","���"};
//--------------- union declaration -----------------------------------------------
union {unsigned char buffer[8]; unsigned int pvT;} ds;          // ������ ���������� DS18B20
union {unsigned char buffer[4]; unsigned int val[2];} in;
union {unsigned char buffer[4]; unsigned int val[2];} out;
//---------------------------------------------------------------------------------
float Told1[8], Told2[8], iPart[4];

//-------------------------
eeprom signed char relaySet[4]={2,2,2,2};
eeprom signed char analogSet[4]={-1,-1,-1,-1};

eeprom signed int set[6][7]={
{ 230, 200,  50,  10,   1,  -1,   0},  // (����.) Tday;  Tnight;  dTalarm;  hysteresis;  mode=1(������)/mode=0(����������); ������;    ����� � ����1
{  60,  60,  10,   5,   0,   0,   1},  // (����.) RHday; RHnight; dRHalarm; hysteresis;  mode=1(����������)/mode=0(��������); DHT22=0; ����� � ����2
{ 200, 180,  50,  10,   1,  -1,   6},  // (�����) Tday;  Tnight;  dTalarm;  hysteresis;  mode=1(������)/mode=0(����������); ������;    ����� �
{ 400, 350, 100,  50,   1,  -1,   7},  // (�����) RHday; RHnight;  dTalarm;  hysteresis; mode=1(����������)/mode=0(��������); ������;  ����� �   
{  10,  0,   10,   1,   0,0x06,   2},  // tmOn; dimOn=0(���.)/dim=1(���.); tmOff; dimOff; HourStart; Programm;                         ����� � ����3
{0x07,0x20,0x05,0x09,0x18,0x23,   3}}; // DayBeg; DayEnd; Light0Beg; Light0End; Light1Beg; Light1End;                                  ����� � ����4

eeprom unsigned int limit[5][4]={
                    // min max  kP   kI 
                      {  0,100, 20, 100}, // 4
                      {  0,100, 21, 100}, // 5
                      {  0,100, 22, 100}, // 6
                      {  0,100, 23, 100}, // 7
                      {  0, 50,265, 720}}; // ����� �����������  t=0 -> V=1.32 -> ADC=270; t=25 -> V=2.51 -> ADC=514

eeprom unsigned char module[4][4]={
                      {  0,100,4,200}, // ����� ��������� 0%: 100% OutMIN; OutMAX; ������ �1
                      {  0,100,4,200}, // ����� ��������� 0%: 100% OutMIN; OutMAX; ������ �2
                      {  0,100,4,200}, // ����� ��������� 0%: 100% OutMIN; OutMAX; ������ �3
                      {  0,100,4,200}};// ����� ��������� 0%: 100% OutMIN; OutMAX; ������ �4
bit Night;
bit Sec;
bit Dht;
bit Clock_Ok;
bit pHsensor;
bit CO2module;       // ��������� ���������� ��2
bit typeS;           // DHT11/DHT22

//- prototypes ------
void display(void);

#include "fontsN.c"
#include "ili9341.c"
#include "twi.c"
#include "dht.c"
#include "buttons.c"
#include "proc.c"
#include "control.c"
#include "touchpad.c"
#include "displ.c"

// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void){
 static unsigned int count1;
 count1++;
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)  // �������� 5 mS ����������� ������� �����
{
  INIT_TIMER0;
  if(BeepT){--BeepT; BEEP = ON;}		// �������� ������  1
  else BEEP = OFF;			            // ��������� ������ 0      �����  1,9-2,44 us
}

// 0.25 Hz timer interrupt generated by Timer1 overflow;  Timer Period: 1,0486 s
interrupt [TIM1_OVF] void timer_comp_isr(void){
 Sec=1;
} 


void main(void){
// Declare your local variables here
signed char x, byte;
#include "init.c"

while (1){
//--------------------------- ������� 1 ������� ---------------------------------------------
    if(Sec){                     
        Sec=0;
        if(clock_buffer[2]>=set[5][0]&&clock_buffer[2]<set[5][1]) byte=0; else byte=1;
        if(byte!=Night){Night = byte; ok = 0;}      // ���� / ����
    // -- ������ ������� ----------    
        byte = set[4][4];                           // ����� ������� ���� 0 �� ������� ���� 1-14 �� �����������
        if(byte==0) timerCheck();                   // ������� ������
        else timerRTC(byte);                        // ������ �� ���������
    // -- ������ ��������� --------    
        if(clock_buffer[2]>=set[5][2]&&clock_buffer[2]<set[5][3]) x=1; else x=0; // Light0Beg; Light0End;
        if(x==0){if(clock_buffer[2]>=set[5][4]&&clock_buffer[2]<set[5][5]) x=2; else x=0;}//Light1Beg; Light1End;
        byte = set[5][6];  // � ������ �������
        if(x>0){
            relOut[byte] = 1;
            byte = 1 << byte;
            portOut |= byte;
            if(x==1) byte = rtcTodec(set[5][3])-1; else byte = rtcTodec(set[5][5])-1;            
            sprintf(txt,"ON  �����.%02u:59:59",byte);
        } 
        else {
            relOut[byte] = 0;
            byte = 1 << byte;
            portOut &= ~byte;
            if(Night) byte = rtcTodec(set[5][2]); else byte = rtcTodec(set[5][4]);
            sprintf(txt,"OFF �����.%02u:00:00",byte);
        }
    // -- ��������� ���������� ������� ---------            
        if(Dht){                                         // ������������ ������ ���������
            if(readDHT()) DHTexist = 3; 
            else if(DHTexist) DHTexist--;                // ������ ��������� ��������? 
            else {pvT = 1900; pvRH = 190;}
        }
    // -- ��������� ���������� ������ ---------
        if(ds18b20) temperature_check();
        else if(soilModule){
            out.buffer[0] = DATAREAD;
            for (x=0; x < soilModule; x++){
                byte = module_check(ID_SOIL1 + x);
                if(byte){
                    t[x] = LowPassF2(in.val[0],x);
                    hum[x] = LowPassF2(in.val[1],x+4);
                }
                else {t[x] = 1990; hum[x] = 1990;}    
            }
        } 
        // --------����� ����������� ������� ����� 0 � ����� 4 ---------
         if(Dht){RelaySensor(pvT,0); analogOut[0]=UpdatePI(pvT,0);}
         else if(ds18b20){
            RelaySensor((t[0]+t[1])/2,0);  // ������� ������
            analogOut[0]=UpdatePI((t[0]+t[1])/2,0);
         }
        // --------����� ��������� ������� ����� 1 � ����� 5 --------- 
        if(Dht){RelaySensor(pvRH,1); analogOut[1]=UpdatePI(pvRH,1);}
        // --------����� ����������� ������ ����� 6 ---------
        if(ds18b20) analogOut[2]=UpdatePI((t[0]+t[1])/2,2);  // ������� ������
        // --------����� ��������� ������ ����� 7 ---------
        //if(ds18b20) analogOut[3] = UpdatePI((t.point[0]+t.point[1])/2,3);  // ������� ������
// ================ ���������� ��������������� ����������� ======================================
        for(byte=0; byte<4; byte++){
            if(relaySet[byte]<2) relOut[byte]=relaySet[byte];
            if(analogSet[byte]>=0) analogOut[byte] = analogSet[byte]; 
            else analogOut[byte] = limitationOut(analogOut[byte], byte);            
            dacU[byte] = adapt(analogOut[byte]);// ��������� ��� ���
        }
        LOCK = OFF;
        PORTC = portOut<<2;                   // ������ ���������� �� �������� ������
        LOCK = ON;
//        setDAC();                           // ������ ���������� �� ���������� ������
    }
//------------------------- ����� ������� 1 ������� ---------------------------------------------
    display();
//sprintf(buff,"X%4u; Y%4u; D%u newB=%3u",point_X,point_Y, displ_num, newButton);
//ILI9341_WriteString(5,TFTBUTTON-10,buff,Font_11x18,WHITE,BLACK,1);
 }
}
