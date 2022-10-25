#define TUNING    170

unsigned char txtTimer[20];

void temperature_check(void){
 int val;
 unsigned char item, byte, crc, try=0;
    for (item=0; item < ds18b20;){
        w1_init();                 // 1 Wire Bus initialization
        w1_write(0x55);            // Load MATCH ROM [55H] comand
        ptr_char = &familycode[item][0];
        for (byte=0; byte < 8; byte++) w1_write(*ptr_char++);
        ptr_char = ds.buffer;
        w1_write(0xBE);            // Read Scratchpad command [BE]
        for (byte=0; byte < 8; byte++) *ptr_char++ = w1_read(); // Read cont. byte
        crc = w1_read();           // Read CRC byte
        ptr_char = ds.buffer;
        if (w1_dow_crc8(ptr_char, 8)==crc){
            try = 0; val = ds.pvT;
            if (val<0) {val=-val; val = val*10/16; val=-val;} // перевод в дес€тичное значение !!
            else val = val*10/16; // перевод в дес€тичное значение !!
            //-----  оректировка датчика DS18B20 ----------
            if(ds.buffer[2]==TUNING) val +=(signed char)ds.buffer[3]; // корекци€ показаний датчика
        }
        else if (++try > 2) {val = 1990; try = 0;}// (199.0) если ошибка более X раз то больше не опрашиваем     
        t[item] = val; 
        if (try==0) item++;
    }
    w1_init();                    // 1 Wire Bus initialization
    w1_write(0xCC);               // Load Skip ROM [CCH] command
    w1_write(0x44);               // Load Convert T [44H] command
}

#define ID_SOIL1         0xF1   // идентификатор модул€ грунта #1
#define ID_SOIL2         0xF2   // идентификатор модул€ грунта #2
#define ID_PH            0xF4   // идентификатор сенсора pH
#define ID_CO2           0xF5   // идентификатор модул€ CO2
unsigned char module_check(unsigned char fc){
 unsigned char *p, try, byte;
    p = out.buffer;
    out.buffer[3]=w1_dow_crc8(p,3);// контрольна€ сумма
    for (try=0; try<3; try++){
        w1_init();            // 1 Wire Bus initialization
        w1_write(fc);         // Family code
        p = out.buffer;
        for (byte=0; byte<4; byte++) w1_write(*p++);
        delay_us(250);      // ожидаем пока модуль обработает информацию
        p = in.buffer;
        for (byte=0; byte<4; byte++) *p++ = w1_read();// Read 4 byte
        byte = w1_read();  // Read CRC byte #5
        p = in.buffer;
        if(byte==w1_dow_crc8(p,4)) break;
        delay_ms(10);
    };
    if(try>2) byte=0; else byte=1;
    return byte;
}

void soilModule_check(void){
 unsigned char item, fc, res;
    for (item=0; item < soilModule; item++){
        fc = ID_SOIL1 + item;
        res = module_check(fc);
        if(res){t[item] = in.val[0]; hum[item] = in.val[1];}
        else {t[item] = 1990; hum[item] = 1990;}    
    }    
}

//#define DATAREAD        0xA1    // Read Scratchpad
//unsigned char readCO2(void) // чтение модул€ —ќ2
//{
// unsigned char byte;
// static unsigned char try;
//  out.buffer[0]=DATAREAD;       // Function Command
//  out.buffer[1]=0x00;           // Data 1
//  out.buffer[2]=displCO2;       // Data 2 1->компрессор отключен; 2->подготовка к замеру; 3->выполнить замер;
//  byte = module_check(ID_CO2); // идентификатор блока
//  if(byte){                     // если блок ответил ...
//     pvCO2 = in.val[1];// CO2
//     try = 0;
//  }
//  else if(++try>5){pvCO2=0; error|=0x08;}// ќтказ модул€ CO2
//  return byte;
//}

//signed int LowPassF2(signed int t,unsigned char i)
//{
//float val;
//  val = A1*Told1[i]-A2*Told2[i]+A3*t;
//  Told2[i] = Told1[i];
//  Told1[i] = val;
//  return val;
//}

void setDAC(void){ // V = Vref x (255/256)
  unsigned char i, val;
    for (i=0; i<4; i++){
        switch (i){
            case 0: CSDAC1 = 0; break;
            case 1: CSDAC2 = 0; break;
            case 2: CSDAC3 = 0; break;
            case 3: CSDAC4 = 0; break;
        }
        SPCR = SPI_MOUD_FL;// SPI port enabled
        val = dacU[i]>>4;
        SPDR = val & 0x0F; // Load Register to DAC
        while (!(SPSR & (1<<SPIF)));     // ожидаем конца передачи по SPI Opcode
        val = dacU[i]<<4;
        SPDR = val & 0xF0;
        while (!(SPSR & (1<<SPIF)));     // ожидаем конца передачи по SPI Opcode
        PORTD|=0xF0;       // INSTRUCTION EXECUTED
    };
    SPCR = 0;             //disable SPI
}

unsigned char adapt(unsigned char val){
    val <<= 1; val += ZERO;
    return val;
}

unsigned char limitationOut(unsigned char val, unsigned char n){// ограничение управл€ющего сигнала
    if(val<limit[n][0]) val = limit[n][0];
    if(val>limit[n][1]) val = limit[n][1];
    return val;
}

// - Converts from 2 digit BCD to Binary. -----------
//char Bcd2ToByte(char Value)
//{
//  char tmp = 0;
//  tmp = ((Value & 0xF0) >> 0x4) * 10;
//  return (tmp + (Value & 0x0F));
//}
unsigned char rtcTodec(unsigned char rtc){
  unsigned char res;
    res = (rtc>>4)*10;
    res += (rtc&0x0F);
    return res;
}

//- Converts a 2 digit decimal to BCD format. ---------
char ByteToBcd2(char Value){
 char bcdhigh = 0;
  while (Value >= 10){
    bcdhigh++;
    Value -= 10;
  }
  return ((bcdhigh << 4U) | Value);
}

//unsigned char decToRtc(unsigned char dec){
//  unsigned char res;
//    res = (dec / 10)*16;
//    res <<= 4;
//    res += dec%10;
//    return res;
//}

unsigned char calcRtc(unsigned char rtc, signed char val){
  unsigned char res;
    res = rtcTodec(rtc);
    res += val;
    res = ByteToBcd2(res);
    return res;
}

// –абота таймера не прив€занна€ к RTC
void timerCheck(void){
 unsigned char byte, port, dimOn=set[4][1], dimOff=set[4][3];
    port = set[4][6];  // є выхода таймера
    byte = 1 << port;
    if(portOut&byte){
        if(--timerOn==0){
            if(dimOff) timerOff = (int)set[4][2]*60; 
            else timerOff = set[4][2];
            portOut &= ~byte; relOut[port] = 0; 
        }
        else {
            dimOff = timerOn/3600;
            port = (timerOn%3600)/60;
            byte = (timerOn%3600)%60;
            sprintf(txtTimer,"ON  залиш.%02u:%02u:%02u",dimOff,port,byte);
        }
    }
    else {
        if(--timerOff==0){
            if(dimOn) timerOn = (int)set[4][0]*60; 
            else timerOn = set[4][0];
            portOut |= byte; relOut[port] = 1; 
        }
        else {
            dimOff = timerOff/3600;
            port = (timerOff%3600)/60;
            byte = (timerOff%3600)%60;
            sprintf(txtTimer,"OFF залиш.%02u:%02u:%02u",dimOff,port,byte);
        }    
    }
}

// –абота таймера прив€занна€ к RTC
void timerRTC(unsigned char prg){
  unsigned char byte, port, dimOn=set[4][3];
    port = set[4][6];  // є выхода таймера
    byte = 1 << port;
    if(clock_buffer[0]<2){
        switch(prg){
            case 1: portOut |= byte; relOut[port] = 1; 
                    if(dimOn) timerOn = (int)set[4][0]*60; 
                    else timerOn = set[4][0];
                break;
            case 2: if(clock_buffer[1]%3==0){   
                        portOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    } break;
            case 3: if(clock_buffer[1]%5==0){   
                        portOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    } break;
            case 4: if(clock_buffer[1]%10==0){   
                        portOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    } break;
            case 5: if(clock_buffer[1]%15==0){   
                        portOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    } break;
            case 6: if(clock_buffer[1]%30==0){   
                        portOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    } break;
            case 7: if(clock_buffer[2]>=set[4][4]){
                        if(clock_buffer[1]==0){   
                            portOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 8: if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%2==0&&clock_buffer[2]==0){   
                            portOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 9: if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%3==0&&clock_buffer[2]==0){   
                            portOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 10:if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%4==0&&clock_buffer[2]==0){   
                            portOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 11:if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%6==0&&clock_buffer[2]==0){   
                            portOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 12:if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%8==0&&clock_buffer[2]==0){   
                            portOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 13:if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%12==0&&clock_buffer[2]==0){   
                            portOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 14:if(clock_buffer[2]==set[4][4]&&clock_buffer[2]==0){   
                        portOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    }    
            
                break;
        };
    }
    else  if(--timerOn==0){
        timerOn = 1;
        portOut &= ~byte; relOut[port] = 0;
        sprintf(txtTimer,"OFF режим роботи %u",prg);        
    }
    else {
        dimOn = timerOn/3600;
        port = (timerOn%3600)/60;
        byte = (timerOn%3600)%60;
        sprintf(txtTimer,"ON  залиш.%02u:%02u:%02u",dimOn,port,byte);
    }
}
//                ол-во
//#1    1 мин   1440	 аждую минуту
//#2    3 мин   480	0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57 мин. каждого часа
//#3    5 мин   288	0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55 мин. каждого часа
//#4    10 мин	144	0, 10, 20, 30, 40, 50 мин. каждого часа
//#5    15 мин	96	0, 15, 30, 45 мин. каждого часа
//#6    30 мин	48	0, 30 мин. каждого часа
//#7    1 час   24	 аждый час
//#8    2 часа	12	0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 часа каждого дн€ (+ сдвиг на стартовый час)
//#9    3 часа	8	0, 3, 6, 9, 12, 15, 18, 21 час каждого дн€ (+ сдвиг на стартовый час)
//#10   4 часа	6	0, 4, 8, 12, 16, 20 часов каждого дн€ (+ сдвиг на стартовый час)
//#11   6 часов	4	0, 6, 12, 18 часов каждого дн€ (+ сдвиг на стартовый час)
//#12   8 часов	3	0, 8, 16 часов каждого дн€ (+ сдвиг на стартовый час)
//#13   12 часов	2	0, 12 часов каждого дн€ (+ сдвиг на стартовый час)
//#14   24 часа	1	0 часов каждого дн€ (+ сдвиг на стартовый час)