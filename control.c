#define ON              1
#define OFF             0
#define UNCHANGED       2

// ch = 0 -> (����.) [0]Tday;  [1]Tnight;  [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> ������ / mode=0 -> ����������; [5]����� � 0 ��� 4
// ch = 1 -> (����.) [0]RHday; [1]RHnight; [2]dRHalarm; [3]hysteresis;  [4]mode=1 -> ������ / mode=0 -> ��������;   [5]����� � 1 ��� 5
// ch = 2 -> (�����) [0]Tday;  [1]Tnight;  [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> ������ / mode=0 -> ��������;   [5]����� � 4 ��� 0
// ch = 3 -> (�����) [0]RHday; [1]RHnight; [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> ������ / mode=0 -> ��������;   [5]����� � 5 ��� 1
void RelaySensor(signed int val, unsigned char ch){
 char x=UNCHANGED, byte, port;                           
 signed int error;
    if(set[ch][4]) error = set[ch][Night]-val;  // mode=1(������/����������)   ������ �������������
    else error = val - set[ch][Night];          // mode=0(����������/��������) ������ �������������

    if(error-set[ch][3] > 0) x = ON;            // ��������
    if(error < 0) x = OFF;                      // ���������

    if(x<UNCHANGED){
        port = set[ch][6];                      // � ������
        byte = 1 << port;
        if(x){relayOut |= byte; relOut[port] = 1;}
        else {relayOut &= ~byte; relOut[port] = 0;}
    }
    else if(relOut[port]) relayOut |= byte;
    else relayOut &= ~byte;
}

unsigned char UpdatePI(signed int error, char i)// i-> ������ iPart[i]; time-> ������ ����� �������� ������ �������������
{
float pPart, Kp;//
  Kp = (float) analog[i][1]/4;   // ����������������    analog[i][1]=20/4=5
  pPart = (float) Kp * error;    // ������ ���������������� �����
  //---- ������� ����������� pPart -----------------------------
  if(pPart <0) pPart = 0; else if(pPart > 100) pPart = 100; // ������� ����������� ????? if(pPart <=0)
  error = pPart;                           // �������������� ������� �� float � signed int
  return error;
};

void calcAn(signed int actualT)      // ��������� ������� ���������� ������� � 1,2,3,4
{
 unsigned char i;
 signed int err; 
    for (i=0;i<4;i++){     
     if(i==3){                                          // ��������� ������� ������� ����. 
        err = (set[0][Night] - analog[i][0]) - actualT; // (����.) Tday;  Tnight;
     }
     else err = actualT - (set[0][Night] + analog[i][0]);// analog[i][0]-> d� �������� �����������;;
     if(err>0){  
         err = UpdatePI(err,i);                         // ������ ������������ ����������
         if(err>analog[i][3]) err = analog[i][3];
         if(err<analog[i][2]) err = analog[i][2];
     }
     else err=0;
     analogOut[i] =  LowPassF2(err, i);
    };
}