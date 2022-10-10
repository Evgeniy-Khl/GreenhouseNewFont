#define ON              1
#define OFF             0
#define UNCHANGED       2

// ch = 0 -> (ВОЗД.) [0]Tset;  [1]dTalarm;  [2]hysteresis;  [3]mode=1 -> нагрев / mode=0 -> охлаждение; [4]выход № 0 или 4
// ch = 1 -> (ВОЗД.) [0]RHset; [1]dRHalarm; [2]hysteresis;  [3]mode=1 -> увлажн / mode=0 -> осушение;   [4]выход № 1 или 5
// ch = 2 -> (ГРУНТ) [0]Tset;  [1]dTalarm;  [2]hysteresis;  [3]mode=1 -> нагрев / mode=0 -> отключен;   [4]выход № 4 или 0
// ch = 3 -> (ГРУНТ) [0]RHset; [1]dTalarm;  [2]hysteresis;  [3]mode=1 -> увлажн / mode=0 -> отключен;   [4]выход № 5 или 1
void RelaySensor(signed int val, unsigned char ch){
 char x=UNCHANGED, byte;
 signed int error;
    error = set[ch][0]-val;                  // ошибка регулирования
    if(abs(error)>set[ch][1])alarm[ch] = 1;  // maxError
    else alarm[ch] = 0;
    if(set[ch][3]){                          // mode=1 -> нагрев / увлажнение
        if(error-set[ch][2] > 0) x = ON;     // включить
        if(error < 0) x = OFF;               // отключить
    }
    else {                                   // mode=0 -> охлаждение / осушение
        if(error+set[ch][2] < 0) x = ON;     // включить
        if(error > 0) x = OFF;               // отключить
    }
    if(x<UNCHANGED) {
        byte = 1 << set[ch][4];              // № реле
        if(x) {portOut |= byte; relOut[ch] = 1;}
        else {portOut &= ~byte; relOut[ch] = 0;}
    }
}

unsigned char UpdatePI(signed int val, char i){// i-> индекс iPart[i]; time-> период между замерами ошибки регулирования
  signed int error;
  float pPart, Kp, Ki, Ud;
    Kp = (float) limit[i][1]/4;              // Пропорциональный    limit[i][1]=20/4=5
    Ki = (float) limit[i][2]*100;            // Интегральный        limit[i][2]=200*100=20000
    if(set[i][3]) error = set[i][0]-val;   // mode=1 ошибка регулирования
    else error = val - set[i][0];           // mode=0 ошибка регулирования
    pPart = (float) Kp * error;   // расчет пропорциональной части
//---- функция ограничения pPart -----------------------------
    if(pPart <0){pPart = 0; iPart[i] = 0;} else if(pPart > 100) pPart = 100; // функция ограничения ????? if(pPart <=0)
    //--------------------------------------------------------------
    iPart[i] += (float) Kp / Ki  * error; // приращение интегральной части  
    Ud = pPart + iPart[i];                // выход регулятора до ограничения
//------ функция ограничения -----------------------------------
    if(Ud < 0) Ud = 0; else if(Ud > 100) Ud = 100;
    iPart[i] = Ud - pPart;                // "антинасыщяющая" поправка
    error = Ud;                           // преобразование формата из float к signed int
    return error;
};

