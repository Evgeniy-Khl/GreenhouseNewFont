#define ON              1
#define OFF             0
#define UNCHANGED       2

void UpdatePID(signed int err, unsigned char K, unsigned char Ti, char cn){
  signed int minP, maxP=analog[4][3]*60; // analog[4][3]=>Period  maxP =< 15000 (max_int)32767
  float pPart, Ud;
    minP = - maxP;
    pPart = (float) err * K;             // ������ ���������������� �����
    //---- ������� ����������� pPart ---------------
    if (pPart < minP) pPart = minP;
    else if (pPart > maxP) pPart = maxP;         // ������� �����������    
    //----------------------------------------------
    iPart[cn] += (float)err * K / (unsigned int)(Ti*10);       // ���������� ������������ �����
    Ud = pPart + iPart[cn];                      // ����� ���������� �� �����������
    //---- ������� ����������� Ud ------------------
    if (Ud < minP) Ud = minP;
    else if (Ud > maxP) Ud = maxP;               // ������� �����������
    iPart[cn] = Ud - pPart;                      // "��������������" ��������
    p[cn] = Ud;
//    return Ud;
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

// ch = 0 -> (����.) [0]Tday;  [1]Tnight;  [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> ������ / mode=0 -> ����������; [5]����� � 0 ��� 4
// ch = 1 -> (����.) [0]RHday; [1]RHnight; [2]dRHalarm; [3]hysteresis;  [4]mode=1 -> ������ / mode=0 -> ��������;   [5]����� � 1 ��� 5
// ch = 2 -> (�����) [0]Tday;  [1]Tnight;  [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> ������ / mode=0 -> ��������;   [5]����� � 4 ��� 0
// ch = 3 -> (�����) [0]RHday; [1]RHnight; [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> ������ / mode=0 -> ��������;   [5]����� � 5 ��� 1
void RelaySensor(signed int val, unsigned char ch){
 char x, byte, port;                           
 signed int error;
    x = set[ch][4]; // set[1][4] => mode=2(������� ���)/mode=1(�������� ���)/mode=0(����);
    if(x) error = set[ch][Night] - val;
    else error = val - set[ch][Night];

    if(x<2){
        x=UNCHANGED;
        if(error-set[ch][3] > 0) x = ON;            // ��������
        if(error < 0) x = OFF;                      // ���������
    }
    else if(ch==1){
        byte = analog[4][0];                    // analog[4][0]=>minRun
        port = analog[4][1];                    // analog[4][1]=>maxRun
        UpdatePID(error,analog[5][0],analog[5][1],0);// ����������� ������������ ON
        valRun = p[0];
        if(valRun < byte) valRun = byte;
        else if(valRun > port) valRun = port;
        if(error<=0) valRun = 0;                // ���������� ������� �� 2 ������ ���� ������� �������        
        val = analog[4][3]*60;                  // analog[4][3]=>Period 
        if(val<60) val = 20;                    // ���� analog[4][3]=0 �� Period=20���.
        UpdatePID(error,analog[5][2],analog[5][3],1);// ����������� ������������ OFF
        valPeriod = val - p[1]; 
        val = analog[4][2]*60;                  //analog[4][2]=>minPeriod
        if(valPeriod < val) valPeriod = val;    // minPeriod
        return;    
    }
    port = set[ch][6];                      // � ������
    byte = 1 << port;
    if(x<UNCHANGED){        
        if(x){relayOut |= byte; relOut[port] = 1;}
        else {relayOut &= ~byte; relOut[port] = 0;}
    }
    else if(relOut[port]) relayOut |= byte;
    else relayOut &= ~byte;
}

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