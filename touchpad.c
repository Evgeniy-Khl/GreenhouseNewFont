void touchpad(char byte){
  char max = 5;
     switch (displ_num){
        //- Общий список Установок -
        case 3:
          switch (byte){
            case 0: displ_num = 0; newSetButt = 1; break;
            case 1: if (--numMenu<0) numMenu = 0;  break;
            case 2: if (++numMenu>5) numMenu = 5; break;
            case 3: 
                    if(numMenu<5) displ_num = 4; else {displ_num = 6; numMenu = 0;}
                    newSetButt = 1; 
            break;
          }
          byte = 10;
        break;
        //- Установки пунктов -
        case 4:
        if(numMenu<2) max=6; else if(numMenu==4) max=4;
          switch (byte){
            case 0: displ_num = 3; newSetButt = 1;  break;
            case 1: if (--numSet<0) numSet = 0;     break;
            case 2: if (++numSet>max) numSet = max; break;
            case 3:  
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
          byte = 10;
        break;
        //- РЕДАКТИРОВАНИЕ -
        case 5:
          switch (byte){
            case 0: displ_num = 4; newSetButt = 1; break;
            case 1: ++newval[numSet];    
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
            case 2: --newval[numSet];
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
            case 3: ILI9341_FillScreen(0, max_X, 0, max_Y, fillScreen);
                  ILI9341_WriteString(10,100,"ВИКОНУЮ ЗАПИС",Font_11x18,GREEN,fillScreen,2);
                  switch (numMenu){
                    case 0: set[0][numSet] = newval[numSet]; break; // "Температура"
                    case 1: set[1][numSet] = newval[numSet]; break; // "Влажность"
                    case 2: set[4][numSet] = newval[numSet]; break; // "Таймер"
                    case 3: set[5][numSet] = ByteToBcd2(newval[numSet]); break; // "День Ночь"
                    case 4: for (byte=0;byte<2;byte++) clock_buffer[byte+1] = ByteToBcd2(newval[byte]);
                            for (byte=2;byte<5;byte++) clock_buffer[byte+2] = ByteToBcd2(newval[byte]);
                            clock_buffer[0]=0;  
                            Clock_Ok=write_TWI(DS3231,0,clock_buffer,7);  break; // "Время и Дата"
                  }
                  delay_ms(500);
                  displ_num = 4; newSetButt = 1; break;
          }
          byte = 10;
        break;
        //- Общий список коэффициентов -
        case 6:
          switch (byte){
            case 0: displ_num = 3; newSetButt = 1; break;
            case 1: if (--numMenu<0) numMenu = 5;  break;
            case 2: if (++numMenu>5) numMenu = 0;  break;
            case 3: displ_num = 7; newSetButt = 1; break;
          }
          byte = 10;
        break;
        //- Установки отдельный значений коэффициентов -
        case 7:
          switch (byte){
            case 0: displ_num = 6; newSetButt = 1; break;
            case 1: if (--numSet<0) numSet = 5;    break;
            case 2: if (++numSet>5) numSet = 0;    break;
            case 3:  
              switch (numMenu){
                case 0: for (byte=0;byte<3;byte++) newval[byte] = limit[0][byte]; break; // "Канал 0"
                case 1: for (byte=0;byte<3;byte++) newval[byte] = limit[1][byte]; break; // "Канал 1"
                case 2: for (byte=0;byte<3;byte++) newval[byte] = limit[2][byte]; break; // "Канал 2" 
                case 3: for (byte=0;byte<3;byte++) newval[byte] = limit[3][byte]; break; // "Канал 3"
                case 4: for (byte=0;byte<3;byte++) newval[byte] = limit[4][byte]; break; // Грунт температура 
                case 5: for (byte=0;byte<3;byte++) newval[byte] = limit[5][byte]; break; // Грунт влажность
              }
                displ_num = 8; newSetButt = 1; 
            break;
          }
          byte = 10;
        break;
        //- РЕДАКТИРОВАНИЕ коэффициентов -
        case 8:
          switch (byte){
            case 0: displ_num = 7; newSetButt = 1; break;
            case 1: ++newval[numSet];    
                switch (numSet) {
                    case 0: if(newval[numSet]>100) newval[numSet]=100; break; // MIN
                    case 1: if(newval[numSet]>100) newval[numSet]=100; break; // MAX
                    case 2: if(newval[numSet]>500) newval[numSet]=500; break; // Пропорциональный
                    case 3: if(newval[numSet]>1000) newval[numSet]=1000; break; // Интегральный
                };
            break; 
            case 2: --newval[numSet];
                switch (numSet) {
                    case 0: if(newval[numSet]<0) newval[numSet]=0; break;   // MIN
                    case 1: if(newval[numSet]<0) newval[numSet]=0; break;   // MAX
                    case 2: if(newval[numSet]<4) newval[numSet]=4; break;   // Пропорциональный
                    case 3: if(newval[numSet]<10) newval[numSet]=10; break; // Интегральный                            
                };
            break; 
            case 3: ILI9341_FillScreen(0, max_X, 0, max_Y, fillScreen);
                  ILI9341_WriteString(10,100,"ВИКОНУЮ ЗАПИС",Font_11x18,GREEN,fillScreen,2);
                  switch (numMenu){
                    case 0: limit[0][numSet] = newval[numSet]; break; // MIN
                    case 1: limit[1][numSet] = newval[numSet]; break; // MAX
                    case 2: limit[2][numSet] = newval[numSet]; break; // Пропорциональный
                    case 3: limit[3][numSet] = newval[numSet]; break; // Интегральный
                  }
                  delay_ms(500);
                  displ_num = 7; newSetButt = 1;
            break;
          }
          byte = 10;
        break;
    }
}

