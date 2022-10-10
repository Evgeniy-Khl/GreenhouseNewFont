// Port D initialization
PORTD=0x00;// State7=0 State6=T State5=T State4=T State3=T State2=T State1=T State0=T
DDRD=0x00;// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In

// Timer/Counter 0 initialization

// Timer/Counter 1 initialization

TCCR1A=0x00;// initialize Timer1 overflow interrupts in Mode 0 (Normal)
TCCR1B=(1<<CS12)|(1<<CS10);// clkio/1024; Clock value: 15,625 kHz
TCNT1H=0;
TCNT1L=0;// timer overflow interrupts will occur with 0.25Hz frequency

// Timer/Counter 2 initialization

// External Interrupt(s) initialization
// INT0: On Mode: Falling Edge INT1: Off INT2: Off
GICR|=0x40;
MCUCR=0x02;
MCUCSR=0x00;
GIFR=0x40;

// Timer(s)/Counter(s) Interrupt(s) initialization
//TIMSK=0x10;
TIMSK=1<<TOIE1;// enable Timer1 overflow interrupt

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=0x00;
UCSRB=0xD8;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x67;

// Analog Comparator initialization
ACSR=0x80;// Analog Comparator: Off
SFIOR=0x00;

// TWI initialization
//TWBR=0x0C;// Bit Rate: 400,000 kHz
TWBR=0x48;// Bit Rate: 100,000 kHz
TWAR=0x00;// Two Wire Bus Slave Address: 0x0; General Call Recognition: Off
TWCR=0x44;// Generate Acknowledge Pulse: On; TWI Interrupt: Off
TWSR=0x00;

// ADC initialization
// ADC Clock frequency: 125,000 kHz
// ADC Voltage Reference: AREF pin
ADMUX=ADC_VREF_TYPE & 0xff;
ADCSRA=0x84;

offsetT = 0;// set[4][0];
offsetRH = 0;// set[4][1];
typeS = 0;// set[4][2];
//setDAC();           // подать напряжение на аналоговые выходы
delay_ms(1000);
Dht = readDHT();    // detect DHT-21/11
//------ 1 Wire Protocol Functions ----------------------------------------------------------------------
ds18b20 = w1_search(0xf0,familycode);// detect how many DS1820/DS18S20 devices are connected to the 1 Wire bus
if((ds18b20+Dht)>MAX_DEVICES) ds18b20=(MAX_DEVICES-Dht);
if(ds18b20)           // если датчики были найдены
 {
   w1_init();      // 1 Wire Bus initialization
   w1_write(0xCC); // Load Skip ROM [CCH] command
   w1_write(0x44); // Load Convert T [44H] command
 }
//------------------------ once per second --------------------------------------
clock_buffer[0] = 0;// EOSC=0 BBSQW=0 CONV=0 RS2=0 RS1=0 INTCN=0 A2IE=0 A1IE=0
clock_buffer[1] = 0;//  EN32kHz=0 BSY=0 A2F=0 A1F=0      
Clock_Ok = write_TWI(DS3231,0x0E,clock_buffer,2);

// Global enable interrupts
#asm("sei")

InitInterface();
ILI9341_Init();
//TFT_SetOrientation(3);  //0
ILI9341_FillScreen(0, max_X, 0, max_Y, fillScreen);
pointY = 10;
ILI9341_WriteString(60, pointY, "www.graviton.com.ua", Font_11x18, WHITE, fillScreen, 1);
pointY += 30;
ILI9341_WriteString(80, pointY, "Розумна", Font_11x18, WHITE, fillScreen, 2);
pointY += 30;
ILI9341_WriteString(80, pointY, "теплиця", Font_11x18, WHITE, fillScreen, 2);
pointY = pointY+45;
sprintf(buff,"Датчикыв температури %u",ds18b20+Dht);    // количество датчиков
ILI9341_WriteString(10, pointY, buff, Font_11x18, WHITE, fillScreen, 1);
pointY = pointY+20;
sprintf(buff,"Датчик вологосты %u",Dht);                // датчик AM2301
ILI9341_WriteString(10, pointY, buff, Font_11x18, WHITE, fillScreen, 1);
//  pointY = pointY+20;

newSetButt = 1;
temperature_check();
//display();