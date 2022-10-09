#define dhtport PORTB.0
#define dhtddr DDRB.0
#define dhtpin PINB.0

unsigned char startDHT(void){
   unsigned char flag=0;
   dhtport = 0;
   dhtddr = 1;   // MCU pull down
   if(typeS) delay_ms(30); // MCU Sends out Start Signal to DHT and pull down voltage for at least 18ms to let DHT11 detect the signal.
   else delay_ms(10); // MCU Sends out Start Signal to DHT and pull down voltage for at least 18ms to let DHT21 detect the signal.
   dhtddr = 0;   // MCU pull up
   delay_us(60); // wait for DHT respond 20-40uS
   if(!dhtpin){
      while(!dhtpin) {flag++;} // low-voltage-level response signal & keeps it for 80us (flag=32 Response to low time finished)
      if(flag<10) return 0;
      else {flag=0; while(dhtpin) {flag++;}}  // hi-voltage-level response signal & keeps it for 80us (flag=55 Response to high time finished)
      if(flag<10) return 0;
      else return 1;
   }
   else return 0;
}

unsigned char readDHT(void){
 unsigned char i, j, flag=0, tem[5];
 unsigned int tempRH;
 static unsigned char err;
 if(startDHT()){
    for(i=0; i<5; i++){
       tem[i]=0;
       for(j=0; j<8; j++){
          tem[i]<<= 1;
          #asm("cli")     // Global enable interrupts
//        delay_us(30);   // When DHT is sending data to MCU, every bit of data begins with the 50us low-voltage-level
                          // and the length of the following high-voltage-level signal determines whether data bit is "0" or "1"         
          while(!dhtpin) {flag++;}// ожидаем фронт сигнала
          delay_us(32);   //26-28u voltage-length means data "0" / 70u voltage-length means data "1"
          flag=0;
          while(dhtpin) {flag++;}// ожидаем спад сигнала
          if(flag>10) tem[i]|= 1;// data "1"
          #asm("sei")     // Global enable interrupts
       };
    };
    flag=tem[0]+tem[1]+tem[2]+tem[3];
    if(flag==tem[4]){
      if(typeS) {tempRH =(int)tem[0]*10; pvT =(int)tem[2]*10;}            // Other = 1 -> DHT11
      else {tempRH =(int)tem[0]*256+tem[1]; pvT =(int)tem[2]*256+tem[3];} // Other = 0 -> DHT21
      pvT  += offsetT;
      pvRH = tempRH/10;
      pvRH += offsetRH;                  // коррекция датчика влажности
      err = 0;
      if(pvRH>100) pvRH=100; else if (pvRH<1) pvRH=1;
      return 1;
    }
    else if(++err>3) return 0;  // НЕ верная CRC датчика влажности  errors |=0x10;
 }
 else return 0;     // потерян датчик влажности
}
