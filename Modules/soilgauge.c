/*******************************************************
Project :  Почвомер
Version :  0.1 
Date    : 01.11.2022
Chip type       : ATtiny13A
Clock frequency : 9,600000 MHz
Program size    : 395 words (790 bytes), 77,1% of FLASH [0xE4F3]-#1; [0xE382]-#2  06.02.2023
*******************************************************/

#include <tiny13a.h>
#include <delay.h>
#include <1wire.h>
#include <1wireslave.h>
//#include <eeprom.h>

#define ID              0xF2    // идентификатор блока
//#define DATAREAD        0xA1    // Read Scratchpad
//#define EEPROMREAD      0xB1    // Read EEPROM
//#define EEPROMLOAD      0xC1    // Load EEPROM

#define LEDrst    PORTB.0
#define LEDlim    PINB.2
#define ON      0
#define OFF     1
#define ADC_PB4 2
#define ADC_PB3 3

#define TIMING480    60     // 60*64*0.10417=400 us  (72->480 us)
#define PRESET48     249    // (256 - n)*64*0.10417= 46,6 us  ( Waits 48 us )
#define PRESET120    237    // (256 - n)*64*0.10417= 126,6 us ( Presence pulse 120 us )
#define PRESET260    217    // (256 - n)*64*0.10417= 260,0 us ( Waiting 260 us )

// 1 Wire Bus functions
#asm
   .equ __w1_port=0x18 ;PORTB
   .equ __w1_bit=1
#endasm

// Declare your global variables here
unsigned int counter, humid;
union {unsigned char data[4]; signed int val[2];} out;
unsigned char buffer[4];
//unsigned char eeprom *ptr_to_eeprom;
//eeprom unsigned char limitRH[2]={100, 100};

bit TimeSlot;
bit Fall;
bit RstPuise;
bit Waiting;
bit PrsPuise;
bit Measur;

//unsigned int map(unsigned char x, unsigned char in_min, unsigned char in_max){
// unsigned int val;
//    val = (x - in_min)*100; val /= (in_max - in_min);
//    return val;
//}

// Pin change interrupt service routine
interrupt [PC_INT0] void pin_change_isr(void)
{
    counter++;
}

// External Interrupt 0 service routine
interrupt [EXT_INT0] void ext_int0_isr(void)
{
    if(Fall==0){               // прищел спад
        #asm("wdr")
        TCNT0=0;               // 256*64*0.10417=1706 us max.
        MCUCR=0x03;            // INT0 Mode: Rising Edge        
        Fall=1;  
    }
    else {                     // пришел фронт
        if(TCNT0>=TIMING480){
            RstPuise=1; LEDrst=ON;
            TCNT0=PRESET48; TIFR0=0x02; TIMSK0=0x02;// Overflow Interrupt Enable; Waits 48 us
            GIMSK=0;                                // INT0: Off; Interrupt on any change on pins PCINT0-5: Off            
        }
        else{
            GIFR=(1<<INT0);                         // INT0: On; 
            GIMSK=(1<<INT0);
            Fall=0; MCUCR=0x02;                     // INT0 Mode: Falling Edge
        }
    };
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    if(RstPuise){
        TCNT0=PRESET120;                // Presence pulse 120 us
        #asm
        cbi  __w1_port,__w1_bit
        sbi  __w1_port-1,__w1_bit       ; set 1Wr
        #endasm
        RstPuise=0; PrsPuise=1;
    }
    else if(PrsPuise){
        TCNT0=PRESET260;                // Waiting 260 us
        #asm
        cbi  __w1_port-1,__w1_bit    ; relise 1Wr
        #endasm
        PrsPuise=0; Waiting=1;
    }
    else if(Waiting) {Waiting=0; TimeSlot=1; TIMSK0=0;}// Overflow Interrupt Desable
    else if(Measur){
        Measur=0; TIMSK0=0; GIMSK=0;    // TIMSK0 -> Interrupt Mask Register
        humid = counter;                // количество импульсов
        TCCR0B=(0<<WGM02)|(0<<CS02)|(1<<CS01)|(1<<CS00);// clk/64 (From prescaler) Clock value: 150,000 kHz;  Timer Period: 1,7067 ms
    }
}

// Bandgap Voltage Reference: Off
//#define ADC_VREF_TYPE ((0<<REFS0) | (0<<ADLAR))
#define ADC_VREF_TYPE ((0<<REFS0) | (1<<ADLAR))

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
//return ADCW;
return ADCH;// Read the 8 most significant bits
}


#include "WR1com.c"

void main(void)
{
// Declare your local variables here

#include "init.c"
//ptr_to_eeprom=&limitRH[0];

while (1)
 {
    if(TimeSlot){
        w1_handler();
        TimeSlot=0; Fall=0; LEDrst=OFF;         // ВАЖНО на этом месте !!!        
        out.val[0] = read_adc(ADC_PB4);         // температура  106 uS
        //--- измерение влажности ---
        TCCR0B=(0<<WGM02)|(1<<CS02)|(0<<CS01)|(0<<CS00);// clk/256 (From prescaler) Clock value: 37,500 kHz;  Timer Period: 6,8267 ms
        Measur=1; counter=0;
        TCNT0=0; TIFR0=0x02; TIMSK0=0x02;       // 256*64*0.10417=1706 us max.
        GIFR=(1<<PCIE); GIMSK=(1<<PCIE);        // Interrupt on any change on pins PCINT0-5: On; PCINT3
        while(Measur)
//        out.val[1] = map(humid, limitRH[0], limitRH[1]);
        out.val[1] = humid;
        MCUCR=0x02;                     // INT1 Mode: Falling Edge;
        GIFR=GIMSK=(1<<INT0);           // INT0: On; Interrupt on any change on pins PCINT0-5: Off
//        if(LEDlim==0){
//            if(humid>limitRH[1]) limitRH[1] = humid;      // расширение границ диапазона вверх
//            else if(humid<limitRH[0]) limitRH[0] = humid; // расширение границ диапазона вниз
//        }        
    }
 }
}
