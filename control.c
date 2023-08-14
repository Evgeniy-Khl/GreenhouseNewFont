#define ON              1
#define OFF             0
#define UNCHANGED       2

void UpdatePID(signed int err, unsigned char K, unsigned char Ti, char cn){
  signed int minP, maxP=analog[4][3]*60; // analog[4][3]=>Period  maxP =< 15000 (max_int)32767
  float pPart, Ud;
    minP = - maxP;
    pPart = (float) err * K;             // расчет пропорциональной части
    //---- функция ограничения pPart ---------------
    if (pPart < minP) pPart = minP;
    else if (pPart > maxP) pPart = maxP;         // функция ограничения    
    //----------------------------------------------
    iPart[cn] += (float)err * K / (unsigned int)(Ti*10);       // приращение интегральной части
    Ud = pPart + iPart[cn];                      // выход регулятора до ограничения
    //---- функция ограничения Ud ------------------
    if (Ud < minP) Ud = minP;
    else if (Ud > maxP) Ud = maxP;               // функция ограничения
    iPart[cn] = Ud - pPart;                      // "антинасыщяющая" поправка
    p[cn] = Ud;
//    return Ud;
}

unsigned char UpdatePI(signed int error, char i)// i-> индекс iPart[i]; time-> период между замерами ошибки регулирования
{
float pPart, Kp;//
  Kp = (float) analog[i][1]/4;   // Пропорциональный    analog[i][1]=20/4=5
  pPart = (float) Kp * error;    // расчет пропорциональной части
  //---- функция ограничения pPart -----------------------------
  if(pPart <0) pPart = 0; else if(pPart > 100) pPart = 100; // функция ограничения ????? if(pPart <=0)
  error = pPart;                           // преобразование формата из float к signed int
  return error;
};

// ch = 0 -> (ВОЗД.) [0]Tday;  [1]Tnight;  [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> нагрев / mode=0 -> охлаждение; [5]выход № 0 или 4
// ch = 1 -> (ВОЗД.) [0]RHday; [1]RHnight; [2]dRHalarm; [3]hysteresis;  [4]mode=1 -> увлажн / mode=0 -> осушение;   [5]выход № 1 или 5
// ch = 2 -> (ГРУНТ) [0]Tday;  [1]Tnight;  [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> нагрев / mode=0 -> отключен;   [5]выход № 4 или 0
// ch = 3 -> (ГРУНТ) [0]RHday; [1]RHnight; [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> увлажн / mode=0 -> отключен;   [5]выход № 5 или 1
void RelaySensor(signed int val, unsigned char ch){
 char x, byte, port;                           
 signed int error;
    x = set[ch][4]; // set[1][4] => mode=2(импульс увл)/mode=1(релейное увл)/mode=0(осуш);
    if(x) error = set[ch][Night] - val;
    else error = val - set[ch][Night];

    if(x<2){
        x=UNCHANGED;
        if(error-set[ch][3] > 0) x = ON;            // включить
        if(error < 0) x = OFF;                      // отключить
    }
    else if(ch==1){
        byte = analog[4][0];                    // analog[4][0]=>minRun
        port = analog[4][1];                    // analog[4][1]=>maxRun
        UpdatePID(error,analog[5][0],analog[5][1],0);// определение длительности ON
        valRun = p[0];
        if(valRun < byte) valRun = byte;
        else if(valRun > port) valRun = port;
        if(error<=0) valRun = 0;                // отключение впрыска по 2 каналу если сильный перелив        
        val = analog[4][3]*60;                  // analog[4][3]=>Period 
        if(val<60) val = 20;                    // если analog[4][3]=0 то Period=20сек.
        UpdatePID(error,analog[5][2],analog[5][3],1);// определение длительности OFF
        valPeriod = val - p[1]; 
        val = analog[4][2]*60;                  //analog[4][2]=>minPeriod
        if(valPeriod < val) valPeriod = val;    // minPeriod
        return;    
    }
    port = set[ch][6];                      // № выхода
    byte = 1 << port;
    if(x<UNCHANGED){        
        if(x){relayOut |= byte; relOut[port] = 1;}
        else {relayOut &= ~byte; relOut[port] = 0;}
    }
    else if(relOut[port]) relayOut |= byte;
    else relayOut &= ~byte;
}

void calcAn(signed int actualT)      // Расчетное задание АНАЛОГОВЫХ выходов № 1,2,3,4
{
 unsigned char i;
 signed int err; 
    for (i=0;i<4;i++){     
     if(i==3){                                          // Положение клапана горячей воды. 
        err = (set[0][Night] - analog[i][0]) - actualT; // (ВОЗД.) Tday;  Tnight;
     }
     else err = actualT - (set[0][Night] + analog[i][0]);// analog[i][0]-> dТ смещение температуры;;
     if(err>0){  
         err = UpdatePI(err,i);                         // расчет управляющего напряжения
         if(err>analog[i][3]) err = analog[i][3];
         if(err<analog[i][2]) err = analog[i][2];
     }
     else err=0;
     analogOut[i] =  LowPassF2(err, i);
    };
}