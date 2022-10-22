void w1_reply(void){
 unsigned char *p,i,crc;
    p = out.data;
    crc=w1_dow_crc8(p,4);
    for (i=0;i<4;i++) w1_write_slave(*p++);
    w1_write_slave(crc);
}

void w1_handler(void){
 unsigned char byte,*p;
    byte=w1_read_slave();
    if(byte==ID){
        #asm("wdr")
        p = buffer;
        for (byte=0; byte<4; byte++) *p++ = w1_read_slave(); // Read cont. byt
        p = buffer;
        byte=w1_dow_crc8(p,3);
        if(byte==buffer[3]) w1_reply();
    }
    TimeSlot=0; Fall=0; LEDrst=OFF; LEDcool=ON; // ÂÀÆÍÎ íà ýòîì ìåñòå !!!
    TIMSK0=0;                       // TOV0 Off TOV1 Off
    MCUCR=0x02;                     // INT1 Mode: Falling Edge;
    GIFR=0x60; GIMSK=0x60;          // INT0: On; Interrupt on any change on pins PCINT0-5: On
}
