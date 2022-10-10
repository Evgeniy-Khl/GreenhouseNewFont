void RelayControl(signed int val, unsigned char chanal){
 char x=UNCHANGED, byte, chRelay=chanal;
 signed int error;
 if(Dht==0 && chanal==1) chanal = 2;            // ����������
  error = set[chanal][0]-val;                   //set[chanal][0] - SP
  if (abs(error)>set[chanal][1]){               //set[chanal][1] - maxError
    alarm[chRelay] = 1;
  }
  else alarm[chRelay] = 0;
  if (set[chanal][2]){                          //set[chanal][2] - mode=0 -> ������ / mode=1 -> ����������;
   if (error > 0) x = ON;                       // ��������
   if (error + set[chanal][3] < 0) x = OFF;     //set[chanal][3] - hysteresis ���������
  }
  else {
   if (error - set[chanal][3] > 0) x = ON;      //set[chanal][3] - hysteresis ��������
   if (error < 0) x = OFF;                      // ���������
  }
  if (x<UNCHANGED) {
    byte = 1 << chRelay;
    if (x) {portOut |= byte; relOut[chRelay] = 1;}
    else {portOut &= ~byte;  relOut[chRelay] = 0;}
  }
}

