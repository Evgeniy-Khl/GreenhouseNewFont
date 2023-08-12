#define KEY_1          1
#define KEY_2          2
#define KEY_3          4
#define KEY_4          8
#define KEY_5          16
#define KEY_6          32
#define KEY_7          64
#define KEY_7_4        72
#define KEY_7_6        96
#define KEY_8          128
#define KEY_7_8        192

void checkkey(unsigned char key){
 char byte, max = 5;
  if(key){
     Display=1;
     switch (displ_num){
       //-------------------------------- СТАН ДАТЧИКІВ и ТАЙМЕРІВ ------------------------------------------
       case 2:
         max = 8;
         switch (key){
            case KEY_1: if(--displ_num<0) displ_num=3; newSetButt=1; break;//  (белая 1)
            case KEY_6: displ_num = 0; newSetButt = 1; break;
            case KEY_2: if(++displ_num>3) displ_num=0; newSetButt=1; break;//  (белая 2)
            case KEY_3: if(--numSet<0) numSet = 0;     break;
            case KEY_7: if(++numSet>max) numSet = max; break;
            case KEY_4: 
                if(numSet<4){relaySet[numSet]++; if(relaySet[numSet]>2) relaySet[numSet]=0;}
                else {analogSet[numSet]+=10; if(analogSet[numSet]>100) analogSet[numSet]=100;} 
            break;
            case KEY_8: 
                if(numSet<4){relaySet[numSet]--; if(relaySet[numSet]<0) relaySet[numSet]=2;}
                else {analogSet[numSet]-=10; if(analogSet[numSet]<0) analogSet[numSet]=-1;}
            break;
         }
       break;
       //-------------------------------- СТАН ВЫХОДІВ ------------------------------------------------------
       case 3:  // displ_num
         switch (key){
            case KEY_1: if(--displ_num<0) displ_num=3; newSetButt=1; break;//  (белая 1)
            case KEY_6: displ_num = 0; newSetButt = 1; break;
            case KEY_3: if(--numMenu<0) numMenu = MENU-1; subMenu=0; numSet=0; NewnumMenu=1; break;
            case KEY_7: if(++numMenu>MENU-1) numMenu = 0; subMenu=0; numSet=0; NewnumMenu=1; break;
            case KEY_5: newSetButt = 1; // (зеленая)
                    switch (numMenu){
                        case 5:  displ_num=6; subMenu=0; break;
                        case 6:  displ_num=9; subMenu=0; break;   
                        default: displ_num=4;
                    };  
            break;
         }
       break;
       //-------------------------------- НАЛАШТУВАННЯ СИСТЕМИ ---------------------------------------------
       case 4:  // displ_num
        if(numMenu<2) max=6; else if(numMenu==4) max=4;
          switch (key){
            case KEY_1: displ_num = 3; newSetButt = 1; break;
            case KEY_6: displ_num = 0; newSetButt = 1; break;
            case KEY_3: if(--numSet<0) numSet=0; NewnumMenu=1; break;
            case KEY_7: if(++numSet>max) numSet=max; NewnumMenu=1; break;
            case KEY_5:  // (зеленая) 
              switch (numMenu){
                case 0: for (byte=0;byte<MAX_7;byte++) newval[byte] = set[0][byte]; break; // "Температура"
                case 1: for (byte=0;byte<MAX_7;byte++) newval[byte] = set[1][byte]; break; // "Влажность"
                case 2: for (byte=0;byte<MAX_6;byte++) newval[byte] = set[4][byte]; break; // "Таймер" 
                case 3: for (byte=0;byte<MAX_6;byte++) newval[byte] = rtcTodec(set[5][byte]); break; // "День Ночь"
                case 4: for (byte=0;byte<2;byte++) newval[byte] = rtcTodec(clock_buffer[byte+1]); 
                        for (byte=2;byte<5;byte++) newval[byte] = rtcTodec(clock_buffer[byte+2]);
                    break;     // "Время и Дата"
              };
              displ_num = 5; newSetButt = 1; 
            break;
          }
       break;
       //-------------------------------- РЕДАГУВАННЯ окремих пунктiв --------------------------------------
       case 5:  // displ_num
          switch (key){
            case KEY_1: displ_num = 4; newSetButt = 1; break;
            case KEY_6: displ_num = 0; newSetButt = 1; break;
            case KEY_4: ++newval[numSet];    
                 switch (numMenu) {
                    case 0:  // Температура 
                        switch (numSet) {
                            case 0: if(newval[numSet]>1200) newval[numSet]=1200; break; // День
                            case 1: if(newval[numSet]>1200) newval[numSet]=1200; break; // Ночь
                            case 2: if(newval[numSet]>500) newval[numSet]=500; break;   // Отклонение
                            case 3: if(newval[numSet]>100) newval[numSet]=100; break;   // Гистерезис
                            case 4: if(newval[numSet]>1)  newval[numSet]=1;    break;   // Режим
                            case 5: if(newval[numSet]>0) newval[numSet]=0;     break;   // Резерв
                            case 6: if(newval[numSet]>0)  newval[numSet]=0;    break;   // ВЫХОД
                        }; 
                    break;
                    case 1:  // Влажность 
                        switch (numSet) {
                            case 0: if(newval[numSet]>100) newval[numSet]=100; break; // День
                            case 1: if(newval[numSet]>100) newval[numSet]=100; break; // Ночь
                            case 2: if(newval[numSet]>90) newval[numSet]=90; break;   // Отклонение
                            case 3: if(newval[numSet]>50) newval[numSet]=50; break;   // Гистерезис
                            case 4: if(newval[numSet]>1)  newval[numSet]=1;  break;   // Режим
                            case 5: if(newval[numSet]>1) newval[numSet]=1;   break;   // Датчик DHT11/22
                            case 6: if(newval[numSet]>1)  newval[numSet]=1;  break;   // ВЫХОД
                        };
                    break;
                    case 2:  // Таймер 
                        switch (numSet) {
                            case 0: if(newval[numSet]>1200) newval[numSet]=1200; break;  // Включен
                            case 1: if(newval[numSet]>1) newval[numSet]=1; break;        // Размерность
                            case 2: if(newval[numSet]>1200) newval[numSet]=1200; break;  // Отключен
                            case 3: if(newval[numSet]>1) newval[numSet]=1; break;        // Размерность
                            case 4: if(newval[numSet]>14) newval[numSet]=14; break;      // Шаг
                            case 5: if(newval[numSet]>14) newval[numSet]=14; break;      // Смещение
                        };
                    break;
                    case 3:  // День Ночь 
                        switch (numSet) {
                            case 0: if(newval[numSet]>12) newval[numSet]=12; break;   // DayBeg
                            case 1: if(newval[numSet]>23) newval[numSet]=23; break;   // DayEnd
                            case 2: if(newval[numSet]>12) newval[numSet]=12; break;   // Light0Beg
                            case 3: if(newval[numSet]>12) newval[numSet]=12; break;   // Light0End
                            case 4: if(newval[numSet]>23) newval[numSet]=23; break;   // Light1Beg
                            case 5: if(newval[numSet]>23) newval[numSet]=23; break;   // Light1End
                        };
                    break;
                    case 4:  // Время и Дата 
                        switch (numSet) {
                            case 0: if(newval[numSet]>59) newval[numSet]=0;  break;   // минуты
                            case 1: if(newval[numSet]>23) newval[numSet]=0;  break;   // часы
                            case 2: if(newval[numSet]>31) newval[numSet]=1;  break;   // день
                            case 3: if(newval[numSet]>12) newval[numSet]=1;  break;   // месяц
                            case 4: if(newval[numSet]>59) newval[numSet]=22; break;   // год
                        };
                    break;
                  }; 
            break;
            case KEY_8: --newval[numSet];
                 switch (numMenu) {
                    case 0:  // Температура 
                        switch (numSet) {
                            case 0: if(newval[numSet]<-500) newval[numSet]=-500; break;   // День
                            case 1: if(newval[numSet]<-500) newval[numSet]=-500; break;   // Ночь
                            case 2: if(newval[numSet]<1) newval[numSet]=1;       break;   // Отклонение
                            case 3: if(newval[numSet]<1) newval[numSet]=1;       break;   // Гистерезис
                            case 4: if(newval[numSet]<0) newval[numSet]=0;       break;   // Режим
                            case 5: if(newval[numSet]<-1) newval[numSet]=-1;     break;   // Резерв
                            case 6: if(newval[numSet]<0)  newval[numSet]=0;      break;   // ВЫХОД                            
                        }; 
                    break;
                    case 1:  // Влажность 
                        switch (numSet) {
                            case 0: if(newval[numSet]<10) newval[numSet]=10; break;   // День
                            case 1: if(newval[numSet]<10) newval[numSet]=10; break;   // Ночь
                            case 2: if(newval[numSet]<1) newval[numSet]=1;   break;   // Отклонение
                            case 3: if(newval[numSet]<1) newval[numSet]=1;   break;   // Гистерезис
                            case 4: if(newval[numSet]<0) newval[numSet]=0;   break;   // Режим
                            case 5: if(newval[numSet]<0) newval[numSet]=0;   break;   // Датчик DHT11/22
                            case 6: if(newval[numSet]<1)  newval[numSet]=1;  break;   // ВЫХОД     
                        };
                    break;
                    case 2:  // Таймер 
                        switch (numSet) {
                            case 0: if(newval[numSet]<1) newval[numSet]=1; break;   // Включен
                            case 1: if(newval[numSet]<0) newval[numSet]=1; break;   // Размерность
                            case 2: if(newval[numSet]<1) newval[numSet]=1; break;   // Отключен
                            case 3: if(newval[numSet]<0) newval[numSet]=1; break;   // Размерность
                            case 4: if(newval[numSet]<0) newval[numSet]=0; break;   // Шаг
                            case 5: if(newval[numSet]<0) newval[numSet]=0; break;   // Смещение
                        };
                    break;
                    case 3:  // День Ночь 
                        switch (numSet) {
                            case 0: if(newval[numSet]<0) newval[numSet]=12; break;   // DayBeg
                            case 1: if(newval[numSet]<0) newval[numSet]=23; break;   // DayEnd
                            case 2: if(newval[numSet]<0) newval[numSet]=12; break;   // Light0Beg
                            case 3: if(newval[numSet]<0) newval[numSet]=12; break;   // Light0End
                            case 4: if(newval[numSet]<0) newval[numSet]=23; break;   // Light1Beg
                            case 5: if(newval[numSet]<0) newval[numSet]=23; break;   // Light1End
                        };
                    break;
                    case 4:  // Время и Дата 
                        switch (numSet) {
                            case 0: if(newval[numSet]<0)  newval[numSet]=59; break;   // минуты
                            case 1: if(newval[numSet]<0)  newval[numSet]=23; break;   // часы
                            case 2: if(newval[numSet]<1)  newval[numSet]=31; break;   // день
                            case 3: if(newval[numSet]<1)  newval[numSet]=12; break;   // месяц
                            case 4: if(newval[numSet]<22) newval[numSet]=59; break;   // год
                        };
                    break;
                  };
            break;
            case KEY_5: ILI9341_FillScreen(0, max_X, 0, max_Y, fillScreen);
                  ILI9341_WriteString(10,100,"ВИКОНУЮ ЗАПИС",Font_11x18,GREEN,fillScreen,2);
                  switch (numMenu){
                    case 0: set[0][numSet] = newval[numSet]; break; // "Температура"
                    case 1: set[1][numSet] = newval[numSet]; break; // "Влажность"
                    case 2: set[4][numSet] = newval[numSet]; break; // "Таймер"
                    case 3: set[5][numSet] = ByteToBcd2(newval[numSet]); break; // "День Ночь"
                    case 4: for (byte=0;byte<2;byte++) clock_buffer[byte+1] = ByteToBcd2(newval[byte]);
                            for (byte=2;byte<5;byte++) clock_buffer[byte+2] = ByteToBcd2(newval[byte]);
                            clock_buffer[0]=0;  
                            Clock_Ok=write_TWI(DS_SRTC,0,clock_buffer,7);  break; // "Время и Дата"
                  }
                  delay_ms(500);
                  displ_num = 4; newSetButt = 1; break;
          }
       break;
       //-------------------------------- ПЕРЕЛIК КОЕФІЦІЄНТIВ ---------------------------------------------
       case 6:  // displ_num
          switch (key){
            case KEY_1: displ_num = 3; newSetButt = 1; break;
            case KEY_6: displ_num = 0; newSetButt = 1; break;
            case KEY_3: if (--subMenu<0) subMenu = 0; NewnumMenu=1; break;
            case KEY_7: if (++subMenu>4) subMenu = 4; NewnumMenu=1; break;
            case KEY_5: displ_num = 7; newSetButt = 1; moduleEdit = 0; numSet = 0; break;
          }
        break;
       //-------------------------------- ПЕРЕЛIК ОКРЕМОГО КОЄФФ. чи МОДУЛЯ --------------------------------
        case 7:  // displ_num
          switch (key){
            case KEY_1: if(moduleEdit) displ_num = 9; else displ_num = 6; newSetButt = 1; break;  // модули или коэффициенты
            case KEY_6: displ_num = 3; newSetButt = 1; break;
            case KEY_3: if (--numSet<0) numSet = 3;    break;
            case KEY_7: if (++numSet>3) numSet = 0;    break;
            case KEY_5: if(moduleEdit) newval[numSet] = module[subMenu][numSet];
                        else newval[numSet] = limit[subMenu][numSet];
                        displ_num = 8; newSetButt = 1; 
            break;
          }
        break;
       //-------------------------------- РЕДАГУВАННЯ ОКРЕМОГО КОЄФФ. чи МОДУЛЯ ----------------------------
        case 8:  // displ_num
          switch (key){
            case KEY_1: displ_num = 7; newSetButt = 1; break;
            case KEY_6: displ_num = 3; newSetButt = 1; break;
            case KEY_4: ++newval[numSet];
                        if(newval[numSet]>1000) newval[numSet]=1000;
            break; 
            case KEY_8: --newval[numSet];
                        if(newval[numSet]<0) newval[numSet]=0;
            break; 
            case KEY_5: ILI9341_FillScreen(0, max_X, 0, max_Y, fillScreen);
                    ILI9341_WriteString(10,100,"ВИКОНУЮ ЗАПИС",Font_11x18,GREEN,fillScreen,2);
                    if(moduleEdit) module[subMenu][numSet] = newval[numSet];
                    else limit[subMenu][numSet] = newval[numSet];
                    displ_num = 7; newSetButt = 1; delay_ms(500);                     
            break;
          }
        break;
       //-------------------------------- ПЕРЕЛIК МОДУЛIВ --------------------------------------------------
        case 9:  // displ_num
          switch (key){
            case KEY_1: displ_num = 3; newSetButt = 1; break;
            case KEY_6: displ_num = 0; newSetButt = 1; break;
            case KEY_3: if (--subMenu<0) numMenu = 0; NewnumMenu=1; break;
            case KEY_7: if (++subMenu>3) numMenu = 3; NewnumMenu=1; break;
            case KEY_5: displ_num = 7; newSetButt = 1; moduleEdit = 1; numSet = 0; break;
          }
        break; 
       default:
         switch (key){
            case KEY_1: if(--displ_num<0) displ_num=3; newSetButt=1; break;//  (белая 1)
            case KEY_2: if(++displ_num>3) displ_num=0; newSetButt=1; break;//  (белая 2)
            case KEY_3:  break;//  (белая 3)
            case KEY_4:  break;//  (желтая)            
            case KEY_5:  break;//  (зеленая)       
            case KEY_6:  break;//  (черная)           
            case KEY_7:  break;//  (красная)             
            case KEY_8:  break;//  (синяя)
            case KEY_7_4:  break;// (красная + зеленая)
            case KEY_7_8:  break;// (красная + синяя)
            case KEY_7_6:  break;// (красная + черная)
         };
     };     
  }
  keynum=0;
}