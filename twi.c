#include "twi.h"
#define BITRATE      100     //Bit Rate: 66,000 kHz -> TWBR=0x14;/Bit Rate: 102,000 kH -> TWBR=0x0A;/Bit Rate: 115,000 kHz -> TWBR=0x08;

unsigned char wait_int(void)    //Wait for TWINT flag set.
{
 unsigned char count=0, val=0;
  while (!(TWCR & 0x80)){if(++count>200) {TWBR=0; TWCR=0; val=1; TWBR=BITRATE; break;}};
  return val;
}

unsigned char read_TWI(char devaddr, int address, char *data, int byte)
{
 unsigned char ok=1;
 int tmpint;
  TWCR = 0xA4;//(1<<TWINT)|(1<<TWSTA)|(TWEN) - Send START condition
  if(wait_int()){ok=0; goto EXIT_read_clock;};
  if((TWSR & 0xF8) != START_OK){ok=0; goto EXIT_read_clock;};// This indicates that the START condition has been transmitted
  TWDR = devaddr+0;//Load SLA_W into TWDR register. Clear TWINT bit in TWCR to start transmission of address
  TWCR = 0x84;//(1<<TWINT) | (1<<TWEN)
  if (wait_int()){ok=0; goto EXIT_read_clock;};
  if ((TWSR & 0xF8) != MT_SLA_ACK){ok=0; goto EXIT_read_clock;};// This indicates that the ACKNOWLEDGE  has been riceived
  if ((devaddr&0xF0)==0xA0)// если Serial EEPROM то вместо FIRST WORD ADDRESS посылаем "0"
   {
    ok = address>>8;
    TWDR = ok;//Load DATA into TWDR register. Clear TWINT bit in TWCR to start transmission of address
    ok = 1;
    TWCR = 0x84;//(1<<TWINT) | (1<<TWEN)
    if (wait_int()){ok=0; goto EXIT_read_clock;};
    if ((TWSR & 0xF8) != MT_DATA_ACK){ok=0; goto EXIT_read_clock;};       
   }
  TWDR = address;//Load DATA into TWDR register. Clear TWINT bit in TWCR to start transmission of address
  TWCR = 0x84;//(1<<TWINT) | (1<<TWEN)
  if (wait_int()){ok=0; goto EXIT_read_clock;};
  if ((TWSR & 0xF8) != MT_DATA_ACK){ok=0; goto EXIT_read_clock;};
  TWCR = 0xA4;//(1<<TWINT)|(1<<TWSTA)|(TWEN) - Send REPEATED START condition
  if (wait_int()){ok=0; goto EXIT_read_clock;};
  if ((TWSR & 0xF8) != REPIT_START_OK){ok=0; goto EXIT_read_clock;};
  TWDR = devaddr+1;//Load SLA_R into TWDR register. Clear TWINT bit in TWCR to start transmission of address
  TWCR = 0x84;//(1<<TWINT) | (1<<TWEN)
  if (wait_int()){ok=0; goto EXIT_read_clock;};
  if ((TWSR & 0xF8) != MR_SLA_ACK){ok=0; goto EXIT_read_clock;};
  if(byte>1)
   {
    for (tmpint=0; tmpint<byte-1; tmpint++)
     {
      TWCR = 0xC4;//(1<<TWINT) | (1<<TWEN) | (1<<TWEA) *!*!*!если нет ответа в предышем сеансе то TWINT не устанавливается
      if (wait_int()){ok=0; goto EXIT_read_clock;};
      if ((TWSR & 0xF8) != MR_DATA_ACK){ok=0; goto EXIT_read_clock;};
      *data++ = TWDR;//Read DATA into TWDR register.
     };
   }; 
  TWCR = 0x84;//(1<<TWINT) | (1<<TWEN)
  if (wait_int()){ok=0; goto EXIT_read_clock;};
  if ((TWSR & 0xF8) != MR_DATA_NOT_ACK){ok=0; goto EXIT_read_clock;};
  *data = TWDR;//Read DATA into TWDR register.
EXIT_read_clock:
  TWCR = 0x94;//(1<<TWINT)|(1<<TWEN)|(1<<TWSTO) - Transmit STOP condition
  return ok;
}

unsigned char write_TWI(char devaddr, int address, char *data, char byte)
{
 unsigned char i, ok=1;
  TWCR = 0xA4;//(1<<TWINT)|(1<<TWSTA)|(TWEN) - Send START condition
  while (!(TWCR & 0x80));//Wait for TWINT flag set. This indicates that the START condition has been transmitted
  if ((TWSR & 0xF8) != START_OK){ok=0; goto EXIT_write_clock;};
  TWDR = devaddr+0;//Load SLA_W into TWDR register. Clear TWINT bit in TWCR to start transmission of address
  TWCR = 0x84;//(1<<TWINT) | (1<<TWEN)
  while (!(TWCR & 0x80));//Wait for TWINT flag set. This indicates that the START condition has been transmitted
  if ((TWSR & 0xF8) != MT_SLA_ACK){ok=0; goto EXIT_write_clock;};
  if ((devaddr&0xF0)==0xA0)// если Serial EEPROM то вместо FIRST WORD ADDRESS посылаем "0"
   {
    ok = address>>8;
    TWDR = ok;//Load DATA into TWDR register. Clear TWINT bit in TWCR to start transmission of address
    ok = 1;
    TWCR = 0x84;//(1<<TWINT) | (1<<TWEN)
    while (!(TWCR & 0x80));//Wait for TWINT flag set. This indicates that the START condition has been transmitted
    if ((TWSR & 0xF8) != MT_DATA_ACK){ok=0; goto EXIT_write_clock;};      
   }
  TWDR = address;//Load DATA into TWDR register. Clear TWINT bit in TWCR to start transmission of address
  TWCR = 0x84;//(1<<TWINT) | (1<<TWEN)
  while (!(TWCR & 0x80));//Wait for TWINT flag set. This indicates that the START condition has been transmitted
  if ((TWSR & 0xF8) != MT_DATA_ACK){ok=0; goto EXIT_write_clock;};
  for (i=0; i<byte; i++)
  {
    TWDR = *data++;//Load DATA into TWDR register.
    TWCR = 0x84;//(1<<TWINT) | (1<<TWEN)
    //if (wait_int()) goto EXIT_write_clock;
    while (!(TWCR & 0x80));//Wait for TWINT flag set. This indicates that the START condition has been transmitted
    if ((TWSR & 0xF8) != MT_DATA_ACK){ok=0; goto EXIT_write_clock;};
  }
EXIT_write_clock:
  TWCR = 0x94;//(1<<TWINT)|(1<<TWEN)|(1<<TWSTO) - Transmit STOP condition 
  return ok;
}
/*
void wait_clock(unsigned char m)    //Wait for TWINT flag set. This indicates that the START condition has been transmitted
{
 unsigned char count=0, err=0;
 do
   {
     if (m) err = write_clock(clock_buffer);
     else err = read_clock();
     count++;
   } while (sec==1 && count<5);
}
*/