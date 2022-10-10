#define ON              1
#define OFF             0
#define UNCHANGED       2

// ch = 0 -> (����.) [0]Tset;  [1]dTalarm;  [2]hysteresis;  [3]mode=1 -> ������ / mode=0 -> ����������; [4]����� � 0 ��� 4
// ch = 1 -> (����.) [0]RHset; [1]dRHalarm; [2]hysteresis;  [3]mode=1 -> ������ / mode=0 -> ��������;   [4]����� � 1 ��� 5
// ch = 2 -> (�����) [0]Tset;  [1]dTalarm;  [2]hysteresis;  [3]mode=1 -> ������ / mode=0 -> ��������;   [4]����� � 4 ��� 0
// ch = 3 -> (�����) [0]RHset; [1]dTalarm;  [2]hysteresis;  [3]mode=1 -> ������ / mode=0 -> ��������;   [4]����� � 5 ��� 1
void RelaySensor(signed int val, unsigned char ch){
 char x=UNCHANGED, byte;
 signed int error;
    error = set[ch][0]-val;                  // ������ �������������
    if(abs(error)>set[ch][1])alarm[ch] = 1;  // maxError
    else alarm[ch] = 0;
    if(set[ch][3]){                          // mode=1 -> ������ / ����������
        if(error-set[ch][2] > 0) x = ON;     // ��������
        if(error < 0) x = OFF;               // ���������
    }
    else {                                   // mode=0 -> ���������� / ��������
        if(error+set[ch][2] < 0) x = ON;     // ��������
        if(error > 0) x = OFF;               // ���������
    }
    if(x<UNCHANGED) {
        byte = 1 << set[ch][4];              // � ����
        if(x) {portOut |= byte; relOut[ch] = 1;}
        else {portOut &= ~byte; relOut[ch] = 0;}
    }
}

unsigned char UpdatePI(signed int val, char i){// i-> ������ iPart[i]; time-> ������ ����� �������� ������ �������������
  signed int error;
  float pPart, Kp, Ki, Ud;
    Kp = (float) limit[i][1]/4;              // ����������������    limit[i][1]=20/4=5
    Ki = (float) limit[i][2]*100;            // ������������        limit[i][2]=200*100=20000
    if(set[i][3]) error = set[i][0]-val;   // mode=1 ������ �������������
    else error = val - set[i][0];           // mode=0 ������ �������������
    pPart = (float) Kp * error;   // ������ ���������������� �����
//---- ������� ����������� pPart -----------------------------
    if(pPart <0){pPart = 0; iPart[i] = 0;} else if(pPart > 100) pPart = 100; // ������� ����������� ????? if(pPart <=0)
    //--------------------------------------------------------------
    iPart[i] += (float) Kp / Ki  * error; // ���������� ������������ �����  
    Ud = pPart + iPart[i];                // ����� ���������� �� �����������
//------ ������� ����������� -----------------------------------
    if(Ud < 0) Ud = 0; else if(Ud > 100) Ud = 100;
    iPart[i] = Ud - pPart;                // "��������������" ��������
    error = Ud;                           // �������������� ������� �� float � signed int
    return error;
};

