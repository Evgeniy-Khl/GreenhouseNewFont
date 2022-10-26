// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=(1<<CLKPCE);
CLKPR=(0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Port B initialization 
DDRB=(0<<DDB5)|(0<<DDB4)|(0<<DDB3)|(0<<DDB2)|(0<<DDB1)|(1<<DDB0);          // Function: Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=Out 
PORTB=(0<<PORTB5)|(0<<PORTB4)|(0<<PORTB3)|(0<<PORTB2)|(0<<PORTB1)|(0<<PORTB0);// State: Bit5=T  Bit4=T  Bit3=T  Bit2=T  Bit1=T  Bit0=0

// Timer/Counter 0 initialization
TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
TCCR0B=(0<<WGM02) | (0<<CS02) | (1<<CS01) | (1<<CS00);// Clock value: 150,000 kHz;  Timer Period: 1,7067 ms
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (1<<TOIE0);

// External Interrupt(s) initialization
// INT0: On
// INT0 Mode: Falling Edge
// Interrupt on any change on pins PCINT0-5: Off
GIMSK=(1<<INT0) | (0<<PCIE);
MCUCR=(1<<ISC01) | (0<<ISC00);
GIFR=(1<<INTF0) | (0<<PCIF);

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIS1) | (0<<ACIS0);
// Digital input buffer on AIN0: On
// Digital input buffer on AIN1: On
DIDR0=(0<<AIN0D) | (0<<AIN1D);

// ADC initialization
// ADC Clock frequency: 150,000 kHz
// ADC Bandgap Voltage Reference: Off
// ADC Auto Trigger Source: ADC Stopped
// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: Off
DIDR0|=(0<<ADC0D) | (0<<ADC2D) | (1<<ADC3D) | (0<<ADC1D);
ADMUX=ADC_VREF_TYPE;       // ((0<<REFS0) | (0<<ADLAR))
ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (0<<ADPS0);
ADCSRB=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

delay_ms(1000);
// 1 Wire Bus initialization
w1_init();
word = read_adc(ADC_PB3);// влажность
if(word<100){limitRH[0]=450; limitRH[1]=850;} // если вход датчика закорочен то сброс лимитов

// Watchdog Timer initialization
// Watchdog Timer Prescaler: OSC/1024k
// Watchdog timeout action: Reset
#pragma optsize-
WDTCR=(0<<WDTIF) | (0<<WDTIE) | (1<<WDP3) | (1<<WDCE) | (1<<WDE) | (0<<WDP2) | (0<<WDP1) | (1<<WDP0);
WDTCR=(0<<WDTIF) | (0<<WDTIE) | (1<<WDP3) | (0<<WDCE) | (1<<WDE) | (0<<WDP2) | (0<<WDP1) | (1<<WDP0);
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Global enable interrupts
#asm("sei")