// Port B initialization
PORTB=0x00;// State7=0 State6=T State5=0 State4=T State3=T State2=0 State1=0 State0=0
DDRB=0xA7; // Func7=Out Func6=In Func5=Out Func4=In Func3=In Func2=Out Func1=Out Func0=Out

// Port D initialization
PORTD=0xF0;// State7=1 State6=1 State5=1 State4=1 State3=T State2=T State1=T State0=T
DDRD=0xF0; // Func7=Out Func6=Out Func5=Out Func4=Out Func3=In Func2=In Func1=In Func0=In

// Timer/Counter 0 initialization
TCCR0=0x05;// (F_XTAL/1024)
INIT_TIMER0;// 5 ms

// Timer/Counter 1 initialization
//TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10); // Clock value: 62,500 kHz
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (1<<CS12) | (0<<CS11) | (0<<CS10);// Timer Period: 1,0486 s


// Timer/Counter 2 initialization

// External Interrupt(s) initialization
// INT0: On Mode: Falling Edge INT1: Off INT2: Off
GICR|=0x40;
MCUCR=0x02;
//MCUCSR=0x00;
GIFR=0x40;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(1<<TOIE1) | (1<<TOIE0);// enable Timer0, Timer1 overflow interrupt

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
//UCSRA=0x00;
UCSRB=0xD8;
UCSRC=0x86;
//UBRRH=0x00;
UBRRL=0x67;

// Analog Comparator initialization
ACSR=0x80;// Analog Comparator: Off
//SFIOR=0x00;

// SPI initialization
// SPI disabled
SPCR=0x00;

// TWI initialization
//TWBR=0x0C;// Bit Rate: 400,000 kHz
TWBR=0x48;// Bit Rate: 100,000 kHz
//TWAR=0x00;// Two Wire Bus Slave Address: 0x0; General Call Recognition: Off
TWCR=0x44;// Generate Acknowledge Pulse: On; TWI Interrupt: Off
//TWSR=0x00;

// ADC initialization
// ADC Clock frequency: 125,000 kHz
// ADC Voltage Reference: AREF pin
ADMUX=ADC_VREF_TYPE & 0xff;
ADCSRA=0x84;

//setDAC();           // подать напряжение на аналоговые выходы

//------ 1 Wire Protocol Functions ----------------------------------------------------------------------
ds18b20 = w1_search(0xf0,familycode);// detect how many DS1820/DS18S20 devices are connected to the 1 Wire bus
if((ds18b20)>MAX_DS18B20) ds18b20=(MAX_DS18B20);
if(ds18b20){           // если датчики были найдены
   w1_init();      // 1 Wire Bus initialization
   w1_write(0xCC); // Load Skip ROM [CCH] command
   w1_write(0x44); // Load Convert T [44H] command
}
else {
    soilModule = module_check(ID_SOIL1);  // detect модуль грунта #1
    soilModule += module_check(ID_SOIL2); // detect модуль грунта #2
//    soilModule += module_check(ID_SOIL3); // detect модуль грунта #3
//    soilModule += module_check(ID_SOIL4); // detect модуль грунта #4
}

//------------------------ once per second --------------------------------------
//clock_buffer[0] = 0;// EOSC=0 BBSQW=0 CONV=0 RS2=0 RS1=0 INTCN=0 A2IE=0 A1IE=0
//clock_buffer[1] = 0;//  EN32kHz=0 BSY=0 A2F=0 A1F=0      
//Clock_Ok = write_TWI(DS3231,0x0E,clock_buffer,2);
Clock_Ok = read_TWI(DS_SRTC,0,clock_buffer,CLOCK_BUFFER);// чтение данных часовой микросхемы
//if(Clock_Ok && clock_buffer[7]!=SQWE_1Hz){
//    clock_buffer[0] &= 0x7F;  // Bit7-'CH'=0
//    for (x=1; x<7; x++) clock_buffer[x] = 1;
//    clock_buffer[7] = SQWE_1Hz;
//    Clock_Ok = read_TWI(DS_SRTC,0,clock_buffer,7);// чтение данных часовой микросхемы
//}

// Global enable interrupts
#asm("sei")
//while (1){BeepT=500; delay_ms(1000);};

InitInterface();
ILI9341_Init();
//TFT_SetOrientation(3);  //0
ILI9341_FillScreen(0, max_X, 0, max_Y, BLACK);
//---------
//sprintf(buff,"%02x:%02x:%02x   %02x.%02x.%02x  %02x",clock_buffer[2],clock_buffer[1],clock_buffer[0],clock_buffer[4],clock_buffer[5],clock_buffer[6],clock_buffer[7]);//час:мин дата.мес.год
//ILI9341_WriteString(10,200,buff,Font_11x18,WHITE,BLACK,1);
if(Clock_Ok && clock_buffer[7]!=SQWE_1Hz){   // SQWE_1Hz -> 0x10
    clock_buffer[0] &= 0x7F;  // Bit7-'CH'=0
    for (x=1; x<6; x++) clock_buffer[x] = 1;
    clock_buffer[6] = 0x23;
    clock_buffer[7] = SQWE_1Hz;
    Clock_Ok = write_TWI(DS_SRTC,0,clock_buffer,CLOCK_BUFFER);// чтение данных часовой микросхемы
}
//sprintf(buff,"%02x:%02x:%02x   %02x.%02x.%02x  %02x",clock_buffer[2],clock_buffer[1],clock_buffer[0],clock_buffer[4],clock_buffer[5],clock_buffer[6],clock_buffer[7]);//час:мин дата.мес.год
//ILI9341_WriteString(10,220,buff,Font_11x18,WHITE,BLACK,1);
//---------
pointY = 10;
ILI9341_WriteString(60, pointY, "www.graviton.com.ua", Font_11x18, WHITE, BLACK, 1);
pointY += 30;
ILI9341_WriteString(80, pointY, "Розумна", Font_11x18, WHITE, BLACK, 2);
pointY += 30;
ILI9341_WriteString(80, pointY, "теплиця", Font_11x18, WHITE, BLACK, 2);
pointY = pointY+45;
sprintf(buff,"Датчикыв температури %u",ds18b20);    // количество датчиков
ILI9341_WriteString(10, pointY, buff, Font_11x18, WHITE, BLACK, 1);
pointY = pointY+20;
sprintf(buff,"Датчик вологосты %u",Dht);                // датчик AM2301
ILI9341_WriteString(10, pointY, buff, Font_11x18, WHITE, BLACK, 1);
if(soilModule){
    out.buffer[0] = DATAREAD;
    for (x=0; x < soilModule; x++){
        byte = module_check(ID_SOIL1 + x);
        if(byte){
            Told1[x] = Told2[x] = t[x] = in.val[0]; 
            Told1[x+4] = Told2[x+4] = hum[x] = in.val[1];                   
        }    
    }
    pointY = pointY+20;
    sprintf(buff,"Модулыв грунту %u",soilModule);
    ILI9341_WriteString(10, pointY, buff, Font_11x18, WHITE, BLACK, 1);
}
Sec=1; Display=1; newSetButt=1; timerOn = 1; timerOff = 1;
temperature_check();
BeepT=50;
delay_ms(1000);
