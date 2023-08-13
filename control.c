#define ON              1
#define OFF             0
#define UNCHANGED       2

// ch = 0 -> (ВОЗД.) [0]Tday;  [1]Tnight;  [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> нагрев / mode=0 -> охлаждение; [5]выход № 0 или 4
// ch = 1 -> (ВОЗД.) [0]RHday; [1]RHnight; [2]dRHalarm; [3]hysteresis;  [4]mode=1 -> увлажн / mode=0 -> осушение;   [5]выход № 1 или 5
// ch = 2 -> (ГРУНТ) [0]Tday;  [1]Tnight;  [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> нагрев / mode=0 -> отключен;   [5]выход № 4 или 0
// ch = 3 -> (ГРУНТ) [0]RHday; [1]RHnight; [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> увлажн / mode=0 -> отключен;   [5]выход № 5 или 1
void RelaySensor(signed int val, unsigned char ch){
 char x=UNCHANGED, byte, port;                           
 signed int error;
    if(set[ch][4]) error = set[ch][Night]-val;  // mode=1(нагрев/увлажнение)   ошибка регулирования
    else error = val - set[ch][Night];          // mode=0(охлаждение/осушение) ошибка регулирования

    if(error-set[ch][3] > 0) x = ON;            // включить
    if(error < 0) x = OFF;                      // отключить

    if(x<UNCHANGED){
        port = set[ch][6];                      // № выхода
        byte = 1 << port;
        if(x){relayOut |= byte; relOut[port] = 1;}
        else {relayOut &= ~byte; relOut[port] = 0;}
    }
    else if(relOut[port]) relayOut |= byte;
    else relayOut &= ~byte;
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