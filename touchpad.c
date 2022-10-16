void touchpad(char byte){
  char max = MAX_MENU-1;
     switch (displ_num){
        //- Общий список Установок -
        case 3:
          switch (byte){
            case 0: displ_num = 0; newSetButt = 1; break;
            case 1: if (--numMenu<0) numMenu = 0;  break;
            case 2: if (++numMenu>MAX_MENU-1) numMenu = MAX_MENU-1; break;
            case 3: displ_num = 4; newSetButt = 1; break;
          }
          byte = 10;
        break;
        //- Установки пунктов -
        case 4:
          if (numMenu==4) max++;
          switch (byte){
            case 0: displ_num = 3; newSetButt = 1;  break;
            case 1: if (--numSet<0) numSet = 0;     break;
            case 2: if (++numSet>max) numSet = max; break;
            case 3:  
              switch (numMenu){
                case 0: for (byte=0;byte<MAX_5;byte++) newval[byte] = set[0][byte]; break; // "Температура"
                case 1: for (byte=0;byte<MAX_5;byte++) newval[byte] = set[1][byte]; break; // "Влажность"
                case 2: for (byte=0;byte<MAX_5;byte++) newval[byte] = set[4][byte]; break; // "Таймер" 
                case 3: for (byte=0;byte<MAX_6;byte++) newval[byte] = rtcTodec(set[5][byte]); break; // "День Ночь"
                case 4: for (byte=0;byte<MAX_6;byte++) newval[byte] = set[byte][6]; break; // "Прочее"
                case 5: for (byte=0;byte<2;byte++) newval[byte] = rtcTodec(clock_buffer[byte+1]); 
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
                        }; 
                    break;
                    case 1:  // Влажность 
                        switch (numSet) {
                            case 0: if(newval[numSet]>100) newval[numSet]=100; break; // День
                            case 1: if(newval[numSet]>100) newval[numSet]=100; break; // Ночь
                            case 2: if(newval[numSet]>90) newval[numSet]=90; break;   // Отклонение
                            case 3: if(newval[numSet]>50) newval[numSet]=50; break;   // Гистерезис
                            case 4: if(newval[numSet]>1)  newval[numSet]=1;  break;   // Режим
                        };
                    break;
                    case 2:  // Таймер 
                        switch (numSet) {
                            case 0: if(newval[numSet]>1200) newval[numSet]=1200; break;  // Включен
                            case 1: if(newval[numSet]>1) newval[numSet]=1; break;        // Размерность
                            case 2: if(newval[numSet]>1200) newval[numSet]=1200; break;  // Отключен
                            case 3: if(newval[numSet]>1) newval[numSet]=1; break;        // Размерность
                            case 4: if(newval[numSet]>14) newval[numSet]=14; break;      // Шаг
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
                    case 4:  // Прочее 
                        switch (numSet) {
                            case 0: newval[numSet]=0; break;   // Выход нагрев / охлаждение (0/4)
                            case 1: newval[numSet]=1; break;   // Выход полив / осушение    (1/5)
                            case 2: newval[numSet]=6; break;   // Выход грунт 1             (6)
                            case 3: newval[numSet]=7; break;   // Выход грунт 2             (7)
                            case 4: newval[numSet]=2; break;   // Выход таймера             (2)
                            case 5: newval[numSet]=3; break;   // Выход освещения           (3)
                        };
                    break;
                    case 5:  // Время и Дата 
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
                            case 2: if(newval[numSet]<1) newval[numSet]=1; break;   // Отклонение
                            case 3: if(newval[numSet]<1) newval[numSet]=1; break;   // Гистерезис
                            case 4: if(newval[numSet]<0) newval[numSet]=0; break;   // Режим                            
                        }; 
                    break;
                    case 1:  // Влажность 
                        switch (numSet) {
                            case 0: if(newval[numSet]<10) newval[numSet]=10; break;   // День
                            case 1: if(newval[numSet]<10) newval[numSet]=10; break;   // Ночь
                            case 2: if(newval[numSet]<1) newval[numSet]=1; break;   // Отклонение
                            case 3: if(newval[numSet]<1) newval[numSet]=1; break;   // Гистерезис
                            case 4: if(newval[numSet]<0) newval[numSet]=0; break;   // Режим     
                        };
                    break;
                    case 2:  // Таймер 
                        switch (numSet) {
                            case 0: if(newval[numSet]<1) newval[numSet]=1; break;   // Включен
                            case 1: if(newval[numSet]<0) newval[numSet]=1; break;   // Размерность
                            case 2: if(newval[numSet]<1) newval[numSet]=1; break;   // Отключен
                            case 3: if(newval[numSet]<0) newval[numSet]=1; break;   // Размерность
                            case 4: if(newval[numSet]<0) newval[numSet]=0; break;   // Шаг
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
                    case 4:  // Прочее 
                        switch (numSet) {
                            case 0: newval[numSet]=0; break;   // Выход нагрев / охлаждение (0/4)
                            case 1: newval[numSet]=1; break;   // Выход полив / осушение    (1/5)
                            case 2: newval[numSet]=6; break;   // Выход грунт 1             (6)
                            case 3: newval[numSet]=7; break;   // Выход грунт 2             (7)
                            case 4: newval[numSet]=2; break;   // Выход таймера             (2)
                            case 5: newval[numSet]=3; break;   // Выход освещения           (3)
                        };
                    break;
                    case 5:  // Время и Дата 
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
                  ILI9341_WriteString(5,100,"ВИКОНУЮ ЗАПИС",Font_11x18,GREEN,fillScreen,2);
                  switch (numMenu){
                    case 0: set[0][numSet] = newval[numSet]; break; // "Температура"
                    case 1: set[1][numSet] = newval[numSet]; break; // "Влажность"
                    case 2: set[4][numSet] = newval[numSet]; break; // "Таймер"
                    case 3: set[5][numSet] = ByteToBcd2(newval[numSet]); break; // "День Ночь"
                    case 4: set[numSet][6] = newval[numSet]; break; // "Прочее"
                    case 5: for (byte=0;byte<2;byte++) clock_buffer[byte+1] = ByteToBcd2(newval[byte]);
                            for (byte=2;byte<5;byte++) clock_buffer[byte+2] = ByteToBcd2(newval[byte]);
                            clock_buffer[0]=0;  
                            Clock_Ok=write_TWI(DS3231,0,clock_buffer,7);  break; // "Время и Дата"
                  }
                  delay_ms(500);
                  displ_num = 4; newSetButt = 1; break;
          }
          byte = 10;
        break;
    }
}

