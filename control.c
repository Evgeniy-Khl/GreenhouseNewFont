void RelayControl(signed int val, unsigned char chanal){
 char x=UNCHANGED, byte, chRelay=chanal;
 signed int error;
 if(Dht==0 && chanal==1) chanal = 2;            // психрометр
  error = set[chanal][0]-val;                   //set[chanal][0] - SP
  if (abs(error)>set[chanal][1]){               //set[chanal][1] - maxError
    alarm[chRelay] = 1;
  }
  else alarm[chRelay] = 0;
  if (set[chanal][2]){                          //set[chanal][2] - mode=0 -> нагрев / mode=1 -> охлаждение;
   if (error > 0) x = ON;                       // включить
   if (error + set[chanal][3] < 0) x = OFF;     //set[chanal][3] - hysteresis отключить
  }
  else {
   if (error - set[chanal][3] > 0) x = ON;      //set[chanal][3] - hysteresis включить
   if (error < 0) x = OFF;                      // отключить
  }
  if (x<UNCHANGED) {
    byte = 1 << chRelay;
    if (x) {portOut |= byte; relOut[chRelay] = 1;}
    else {portOut &= ~byte;  relOut[chRelay] = 0;}
  }
}

