/*******************************************************
Project :  Почвомер
Version :  0.0
Date    : 21.10.2022
Chip type       : ATtiny13A
Clock frequency : 9,600000 MHz
Program size    : 403 words (806 bytes), 78,7% of FLASH
*******************************************************/

#include <tiny13a.h>
#include <delay.h>
#include <1wire.h>
#include <1wireslave.h>

#define ID              0xF1    // идентификатор блока
#define RESET           0xC1    // Generate Reset Pulse
#define DATAREAD        0xA1    // Read Scratchpad

#define LEDrst	PORTB.0
#define LEDcool	PORTB.2
#define ON      0
#define OFF     1

#define TIMING480    60	// 60*64*0.10417=400 us  (50*64*0.125=400 us)
#define PRESET48 	249	// (256 - n)*64*0.10417= 46,6 us  ( Waits 48 us )
#define PRESET120 	237	// (256 - n)*64*0.10417= 126,6 us ( Presence pulse 120 us )
#define PRESET260 	217	// (256 - n)*64*0.10417= 260,0 us ( Waiting 260 us )

// 1 Wire Bus functions
#asm
   .equ __w1_port=0x18 ;PORTB
   .equ __w1_bit=1
#endasm

// Declare your global variables here
union {unsigned char data[4]; signed int val[2];} out;
unsigned char buffer[4];
//eeprom unsigned char nil=255;   // НЕ используется, только для ускорения программирования.

bit TimeSlot;
bit Fall;
bit RstPuise;
bit Waiting;
bit PrsPuise;
bit Measur;

// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
    if(Fall==0){               // прищел спад
        #asm("wdr")
        TCNT0=0;               // 256*64*0.10417=1706 us max.
        MCUCR=0x03;            // INT1 Mode: Rising Edge
        Fall=1; Measur=1;
    }
    else {                     // пришел фронт
        if(TCNT0>=TIMING480){
            RstPuise=1; LEDrst=ON;
            TCNT0=PRESET48; TIFR0=0x02; TIMSK0=0x02;// Waits 48 us
            GIMSK=0;                                // INT0: Off; Interrupt on any change on pins PCINT0-5: Off
        }
        else {Fall=0; MCUCR=0x02;}                  // INT1 Mode: Falling Edge
    };
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    if(RstPuise){
        TCNT0=PRESET120;                   // Presence pulse 120 us
        #asm
        cbi  __w1_port,__w1_bit
        sbi  __w1_port-1,__w1_bit       ; set 1Wr
        #endasm
        RstPuise=0; PrsPuise=1;
    }
    else if(PrsPuise){
        TCNT0=PRESET260;                   // Waiting 260 us
        #asm
        cbi  __w1_port-1,__w1_bit	; relise 1Wr
        #endasm
        PrsPuise=0; Waiting=1;
    }
    else if(Waiting) {Waiting=0; TimeSlot=1; TIMSK0=0;}// TOV0 Off
}

// Bandgap Voltage Reference: Off
#define ADC_VREF_TYPE ((0<<REFS0) | (0<<ADLAR))

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input){
    ADMUX=adc_input | ADC_VREF_TYPE;
    // Delay needed for the stabilization of the ADC input voltage
    delay_us(10);
    // Start the AD conversion
    ADCSRA|=(1<<ADSC);
    // Wait for the AD conversion to complete
    while ((ADCSRA & (1<<ADIF))==0);
    ADCSRA|=(1<<ADIF);
    return ADCW;
}

#include "WR1com.c"

void main(void)
{
// Declare your local variables here

#include "init.c"

while (1)
 {
    if(TimeSlot) w1_handler();
    if(Measur){
        Measur = 0;
        out.val[0] = read_adc(3);
    }
 }
}
