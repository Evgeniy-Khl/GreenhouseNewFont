#define ON              1
#define OFF             0
#define UNCHANGED       2

// ch = 0 -> (ВОЗД.) [0]Tday;  [1]Tnight;  [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> нагрев / mode=0 -> охлаждение; [5]выход № 0 или 4
// ch = 1 -> (ВОЗД.) [0]RHday; [1]RHnight; [2]dRHalarm; [3]hysteresis;  [4]mode=1 -> увлажн / mode=0 -> осушение;   [5]выход № 1 или 5
// ch = 2 -> (ГРУНТ) [0]Tday;  [1]Tnight;  [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> нагрев / mode=0 -> отключен;   [5]выход № 4 или 0
// ch = 3 -> (ГРУНТ) [0]RHday; [1]RHnight; [2]dTalarm;  [3]hysteresis;  [4]mode=1 -> увлажн / mode=0 -> отключен;   [5]выход № 5 или 1
void RelaySensor(signed int val, unsigned char ch){
 char x=UNCHANGED, byte;                           
 signed int error;
    byte = set[ch][2];                       // уставка аварии
    if(set[ch][4]) error = set[ch][Night]-val;  // mode=1(нагрев/увлажнение)   ошибка регулирования
    else error = val - set[ch][Night];         // mode=0(охлаждение/осушение) ошибка регулирования
    
    if(abs(error)<(byte/2)) ok |= (1<<ch);   // достигли заданного значения
    if(abs(error)>byte) byte |= (1<<ch); else byte = 0;// авария
    if(ok&byte) alarm[ch] = 1; else if(ok&(1<<ch)) alarm[ch] = 0; else alarm[ch] = 2;// авария или норма или еще не вышли на заданное значение

    if(error-set[ch][3] > 0) x = ON;     // включить
    if(error < 0) x = OFF;               // отключить

    if(x<UNCHANGED) {
        byte = 1 << set[ch][6];              // № реле
        if(x) {portOut |= byte; relOut[ch] = 1;}
        else {portOut &= ~byte; relOut[ch] = 0;}
    }
}

unsigned char UpdatePI(signed int val, char i){// i-> индекс iPart[i]; time-> период между замерами ошибки регулирования
  signed int error;
  float pPart, Kp, Ki, Ud;
    Kp = (float) limit[i][2]/4;               // Пропорциональный    limit[i][1]=20/4=5
    Ki = (float) limit[i][3]*10;             // Интегральный        limit[i][2]=200*10=2000
    if(set[i][4]) error = set[i][Night]-val;  // mode=1(нагрев/увлажнение) ошибка регулирования
    else error = val - set[i][Night];         // mode=0(охлаждение/осушение) ошибка регулирования
    pPart = (float) Kp * error;               // расчет пропорциональной части
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

