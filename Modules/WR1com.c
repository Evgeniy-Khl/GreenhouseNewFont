void w1_reply(void){    // w1_reply(unsigned char command)
 unsigned char *p,i,crc;    
    p = out.data;
//    if(command==EEPROMREAD) eeprom_read_block(p, ptr_to_eeprom, 2);
//    else if(command==EEPROMLOAD){
//        p = &buffer[1];
//        eeprom_write_block(p, ptr_to_eeprom, 2);
//    }
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
        if(byte==buffer[3]) w1_reply(); // w1_reply(buffer[0])
    }
}
