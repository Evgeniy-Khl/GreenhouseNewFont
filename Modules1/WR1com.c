void w1_reply(unsigned char command){
 unsigned char *p,i,crc;
    p = out.data;
    if(command==EEPROMREAD) eeprom_read_block(p, ptr_to_eeprom, 4);
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
        if(byte==buffer[3]) w1_reply(buffer[0]);
    }
    TimeSlot=0; Fall=0; LEDrst=OFF; // ÂÀÆÍÎ íà ýòîì ìåñòå !!!
    TIMSK0=0;                       // TOV0 Off TOV1 Off
    MCUCR=0x02;                     // INT1 Mode: Falling Edge;
    GIFR=GIMSK=(1<<INT0); GIMSK=GIMSK=(1<<INT0); // INT0: On; Interrupt on any change on pins PCINT0-5: Off
}
