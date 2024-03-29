#define TUNING    170

unsigned char txtTimer[20];

void temperature_check(void){
 int val;
 unsigned char *ptr_char, item, byte, crc, try=0;
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
            if (val<0) {val=-val; val = val*10/16; val=-val;} // ������� � ���������� �������� !!
            else val = val*10/16; // ������� � ���������� �������� !!
            //----- ������������ ������� DS18B20 ----------
            if(ds.buffer[2]==TUNING) val +=(signed char)ds.buffer[3]; // �������� ��������� �������
        }
        else if (++try > 2) {val = 1990; try = 0; errors |= (1<<item);}// (199.0) ���� ������ ����� X ��� �� ������ �� ����������     
        t[item] = val; 
        if (try==0) item++;
    }
    w1_init();                    // 1 Wire Bus initialization
    w1_write(0xCC);               // Load Skip ROM [CCH] command
    w1_write(0x44);               // Load Convert T [44H] command
}

#define ID_SOIL1         0xF1   // ������������� ������ ������ #1
#define ID_SOIL2         0xF2   // ������������� ������ ������ #2
#define ID_SOIL3         0xF3   // ������������� ������ ������ #3
#define ID_SOIL4         0xF4   // ������������� ������ ������ #4
unsigned char module_check(unsigned char fc){
 unsigned char *p, try, byte;
    p = out.buffer;
    out.buffer[3]=w1_dow_crc8(p,3);// ����������� �����
    for (try=0; try<3; try++){
        w1_init();            // 1 Wire Bus initialization
        w1_write(fc);         // Family code
        p = out.buffer;
        for (byte=0; byte<4; byte++) w1_write(*p++);
        delay_us(250);      // ������� ���� ������ ���������� ����������
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

signed int LowPassF2(signed int t,unsigned char i)
{
float val;
  val = A1*Told1[i]-A2*Told2[i]+A3*t;
  Told2[i] = Told1[i];
  Told1[i] = val;
  return val;
}

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
        while (!(SPSR & (1<<SPIF)));     // ������� ����� �������� �� SPI Opcode
        val = dacU[i]<<4;
        SPDR = val & 0xF0;
        while (!(SPSR & (1<<SPIF)));     // ������� ����� �������� �� SPI Opcode
        PORTD|=0xF0;       // INSTRUCTION EXECUTED
    };
    SPCR = 0;             //disable SPI
}

unsigned char adapt(unsigned char val){
    val <<= 1; val += ZERO;
    return val;
}

//unsigned char limitationOut(unsigned char val, unsigned char n){// ����������� ������������ �������
//    if(val<limit[n][1]) val = limit[n][1];
//    if(val>limit[n][2]) val = limit[n][2];
//    return val;
//}

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

// ������ ������� ����������� � xternal Interrupt 0
void timerCheck(void){
 unsigned char byte, port, dimOn=set[4][1], dimOff=set[4][3];
    port = set[4][6];  // � ������ �������
    byte = 1 << port;
    if(relayOut&byte){
        if(timerCount==0){
            if(dimOff) timerCount = (int)set[4][2]*60; 
            else timerCount = set[4][2];
            relayOut &= ~byte; relOut[port] = 0; 
        }
        else {
            dimOff = timerCount/3600;
            port = (timerCount%3600)/60;
            byte = (timerCount%3600)%60;
            sprintf(txtTimer,"ON  �����.%02u:%02u:%02u",dimOff,port,byte);
        }
    }
    else {
        if(timerCount==0){
            if(dimOn) timerCount = (int)set[4][0]*60; 
            else timerCount = set[4][0];
            relayOut |= byte; relOut[port] = 1; 
        }
        else {
            dimOff = timerCount/3600;
            port = (timerCount%3600)/60;
            byte = (timerCount%3600)%60;
            sprintf(txtTimer,"OFF �����.%02u:%02u:%02u",dimOff,port,byte);
        }    
    }
}

// ������ ������� ����������� � RTC
void timerRTC(unsigned char prg){
  unsigned char byte, port, dimOn=set[4][3];
    port = set[4][6];  // � ������ �������
    byte = 1 << port;
    if(clock_buffer[0]<2){
        switch(prg){
            case 1: relayOut |= byte; relOut[port] = 1; 
                    if(dimOn) timerOn = (int)set[4][0]*60; 
                    else timerOn = set[4][0];
                break;
            case 2: if(clock_buffer[1]%3==0){   
                        relayOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    } break;
            case 3: if(clock_buffer[1]%5==0){   
                        relayOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    } break;
            case 4: if(clock_buffer[1]%10==0){   
                        relayOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    } break;
            case 5: if(clock_buffer[1]%15==0){   
                        relayOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    } break;
            case 6: if(clock_buffer[1]%30==0){   
                        relayOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    } break;
            case 7: if(clock_buffer[2]>=set[4][4]){
                        if(clock_buffer[1]==0){   
                            relayOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 8: if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%2==0&&clock_buffer[2]==0){   
                            relayOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 9: if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%3==0&&clock_buffer[2]==0){   
                            relayOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 10:if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%4==0&&clock_buffer[2]==0){   
                            relayOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 11:if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%6==0&&clock_buffer[2]==0){   
                            relayOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 12:if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%8==0&&clock_buffer[2]==0){   
                            relayOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 13:if(clock_buffer[2]>=set[4][4]){
                        clock_buffer[2]-=set[4][4];
                        if(clock_buffer[2]%12==0&&clock_buffer[2]==0){   
                            relayOut |= byte; relOut[port] = 1; 
                            if(dimOn) timerOn = (int)set[4][0]*60; 
                            else timerOn = set[4][0];
                        }
                    } break;
            case 14:if(clock_buffer[2]==set[4][4]&&clock_buffer[2]==0){   
                        relayOut |= byte; relOut[port] = 1; 
                        if(dimOn) timerOn = (int)set[4][0]*60; 
                        else timerOn = set[4][0];
                    }    
            
                break;
        };
    }
    else  if(--timerOn==0){
        timerOn = 1;
        relayOut &= ~byte; relOut[port] = 0;
        sprintf(txtTimer,"OFF ����� ������ %u",prg);        
    }
    else {
        dimOn = timerOn/3600;
        port = (timerOn%3600)/60;
        byte = (timerOn%3600)%60;
        sprintf(txtTimer,"ON  �����.%02u:%02u:%02u",dimOn,port,byte);
    }
}
//               ���-��
//#1    1 ���   1440	������ ������
//#2    3 ���   480	0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57 ���. ������� ����
//#3    5 ���   288	0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55 ���. ������� ����
//#4    10 ���	144	0, 10, 20, 30, 40, 50 ���. ������� ����
//#5    15 ���	96	0, 15, 30, 45 ���. ������� ����
//#6    30 ���	48	0, 30 ���. ������� ����
//#7    1 ���   24	������ ���
//#8    2 ����	12	0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 ���� ������� ��� (+ ����� �� ��������� ���)
//#9    3 ����	8	0, 3, 6, 9, 12, 15, 18, 21 ��� ������� ��� (+ ����� �� ��������� ���)
//#10   4 ����	6	0, 4, 8, 12, 16, 20 ����� ������� ��� (+ ����� �� ��������� ���)
//#11   6 �����	4	0, 6, 12, 18 ����� ������� ��� (+ ����� �� ��������� ���)
//#12   8 �����	3	0, 8, 16 ����� ������� ��� (+ ����� �� ��������� ���)
//#13   12 �����	2	0, 12 ����� ������� ��� (+ ����� �� ��������� ���)
//#14   24 ����	1	0 ����� ������� ��� (+ ����� �� ��������� ���)

unsigned char tableRH(signed int maxT, signed int minT)
 {
  signed int dT;
   if (maxT>199 && maxT<410) // maxT> 19.9 � maxT< 41.0
    {
     dT = (maxT-minT)*16/10;
     if (dT<0) dT = 240;        // ������ ����� ��� ������� dT >>=3; ����������� -> dT>20
     maxT /=10;
     dT >>=3;
     if (dT>20) dT = 255;
     else if (dT==0) dT = 100;
     else {maxT -= 20; maxT *= 20; maxT += (dT-1); dT = tabRH[maxT];};
    }
   else dT = 255;
   return dT;
 }
 
//signed int mean(char item){
// unsigned char i, x=0;
// signed int tt=0;
//    for (i=0; i<item; i++){if(t[i] < 850) {tt += t[i]; x++;}};
//    if(x) tt /= x; else {tt = 1900; errors |= 0x08;}  // 0x08 - ������ �������� �������� �������� �����. �������
//    return tt;
//}