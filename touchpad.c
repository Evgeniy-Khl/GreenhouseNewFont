void touchpad(char byte){
  char max = MAX_SET-1;
     switch (displ_num){
        case 0: if(byte<buttonCount){displ_num = byte; newSetButt = 1;} break;
        //- АНАЛОГОВЫЕ ВЫХОДЫ -
        case 1: if(byte<buttonCount){displ_num = byte; newSetButt = 1;} 
                if(displ_num==3) displ_num = 6;
        break;
        //- РЕЛЕЙНЫЕ ВЫХОДЫ -
        case 2: if(byte<buttonCount){displ_num = byte; newSetButt = 1;} 
                if(displ_num==3) displ_num = 7;
        break;
        //- Общий список Установок -
        case 3:
          switch (byte){
            case 0: displ_num = 0; newSetButt = 1; break;
            case 1: if (--numMenu<0) numMenu = 0;    break;
            case 2: if (++numMenu>MAX_MENU-1) numMenu = MAX_MENU-1;    break;
            case 3: displ_num = 4; newSetButt = 1; break;
          }
          byte = 10;
        break;
        //- Установки пунктов -
        case 4:
          if (numMenu==4) max++;
          switch (byte){
            case 0: displ_num = 3; newSetButt = 1; break;
            case 1: if (--numSet<0) numSet = 0;    break;
            case 2: if (++numSet>max) numSet = max;    break;
            case 3: displ_num = 5; newSetButt = 1; 
              switch (numMenu){
                case 0: for (byte=0;byte<MAX_SET;byte++) newval[byte] = set[0][byte]; break; // "Температура"
                case 1: for (byte=0;byte<MAX_SET;byte++){                                     // "Влажность"
                            if (Dht) newval[byte] = set[1][byte];
                            else newval[byte] = set[2][byte];
                        } 
                  break;
                case 2: for (byte=0;byte<MAX_SET;byte++) newval[byte] = set[3][byte]; break; // "Таймер"
                case 3: for (byte=0;byte<MAX_SET;byte++) newval[byte] = set[4][byte]; break; // "Настройки"
                case 4: for (byte=0;byte<2;byte++) newval[byte] = Bcd2ToByte(clock_buffer[byte+1]); 
                        for (byte=2;byte<5;byte++) newval[byte] = Bcd2ToByte(clock_buffer[byte+2]);
                    break;     // "Время и Дата"
              }; 
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
                            case 0: if(newval[numSet]>1200) newval[numSet]=1200; break;   // Задание
                            case 1: if(newval[numSet]>500) newval[numSet]=500; break;   // Отклонение
                            case 2: if(newval[numSet]>100) newval[numSet]=100; break;   // Гистерезис
                            case 3: if(newval[numSet]>1)  newval[numSet]=1;    break;   // Режим
                        }; 
                    break;
                    case 1:  // Влажность 
                        if (Dht){
                            switch (numSet) {
                                case 0: if(newval[numSet]>100) newval[numSet]=100; break;   // Задание
                                case 1: if(newval[numSet]>90) newval[numSet]=90; break;   // Отклонение
                                case 2: if(newval[numSet]>50) newval[numSet]=50; break;   // Гистерезис
                                case 3: if(newval[numSet]>1)  newval[numSet]=1;  break;   // Режим
                            };
                        }
                        else {
                            switch (numSet) {
                                case 0: if(newval[numSet]>1200) newval[numSet]=1200; break;   // Задание
                                case 1: if(newval[numSet]>500) newval[numSet]=500; break;   // Отклонение
                                case 2: if(newval[numSet]>100) newval[numSet]=100; break;   // Гистерезис
                                case 3: if(newval[numSet]>1)  newval[numSet]=1;  break;   // Режим
                            };
                        }
                    break;
                    case 2:  // Таймер 
                        switch (numSet) {
                            case 0: if(newval[numSet]>3600) newval[numSet]=3600; break;   // Включен
                            case 1: if(newval[numSet]>3600) newval[numSet]=3600; break;   // Отключен
                            case 2: if(newval[numSet]>1) newval[numSet]=1; break;   // Размерность
                            case 3: if(newval[numSet]>100) newval[numSet]=100; break;   // Шаг
                        };
                    break;
                    case 3:  // Настройки 
                        switch (numSet) {
                            case 0:    break;   //
                            case 1:    break;   //
                            case 2:    break;   //
                            case 3:    break;   //
                            case 4:    break;   //
                        };
                    break;
                    case 4:  // Время и Дата 
                        switch (numSet) {
                            case 0:    break;   //
                            case 1:    break;   //
                            case 2:    break;   //
                            case 3:    break;   //
                            case 4:    break;   //
                        };
                    break;
                  }; 
            break;
            case 2: --newval[numSet];
                 switch (numMenu) {
                    case 0:  // Температура 
                        switch (numSet) {
                            case 0: if(newval[numSet]<-500) newval[numSet]=-500; break;   // Задание
                            case 1: if(newval[numSet]<1) newval[numSet]=1; break;   // Отклонение
                            case 2: if(newval[numSet]<1) newval[numSet]=1; break;   // Гистерезис
                            case 3: if(newval[numSet]<0) newval[numSet]=0; break;   // Режим                            
                        }; 
                    break;
                    case 1:  // Влажность 
                        switch (numSet) {
                            case 0: if(newval[numSet]<10) newval[numSet]=10; break;   // Задание
                            case 1: if(newval[numSet]<1) newval[numSet]=1; break;   // Отклонение
                            case 2: if(newval[numSet]<1) newval[numSet]=1; break;   // Гистерезис
                            case 3: if(newval[numSet]<0) newval[numSet]=0; break;   // Режим     
                        };
                    break;
                    case 2:  // Таймер 
                        switch (numSet) {
                            case 0: if(newval[numSet]<1) newval[numSet]=1; break;   // Включен
                            case 1: if(newval[numSet]<1) newval[numSet]=1; break;   // Отключен
                            case 2: if(newval[numSet]<0) newval[numSet]=0; break;   // Размерность
                            case 3: if(newval[numSet]<1) newval[numSet]=1; break;   // Шаг
                        };
                    break;
                    case 3:  // Настройки 
                        switch (numSet) {
                            case 0:    break;   //
                            case 1:    break;   //
                            case 2:    break;   //
                            case 3:    break;   //
                        };
                    break;
                    case 4:  // Время и Дата 
                        switch (numSet) {
                            case 0:    break;   //
                            case 1:    break;   //
                            case 2:    break;   //
                            case 3:    break;   //
                        };
                    break;
                  };
            break;
            case 3: ILI9341_FillScreen(0, max_X, 0, max_Y, fillScreen);
                  ILI9341_WriteString(5,100,"ВЫПОЛНЯЮ  ЗАПИСЬ",Font_11x18,GREEN,fillScreen,1);
                  switch (numMenu){
                    case 0: set[0][numSet] = newval[numSet]; break; // "Температура"
                    case 1: if (Dht) set[1][numSet] = newval[numSet];  // "Влажность"
                            else set[2][numSet] = newval[numSet]; 
                      break;
                    case 2: set[3][numSet] = newval[numSet]; break; // "Таймер"
                    case 3: set[4][numSet] = newval[numSet]; break; // "Настройки"
                    case 4: for (byte=0;byte<2;byte++) clock_buffer[byte+1] = ByteToBcd2(newval[byte]);
                            for (byte=2;byte<5;byte++) clock_buffer[byte+2] = ByteToBcd2(newval[byte]);
                            if (numSet==0) clock_buffer[0] = 0;  
                            Clock_Ok = write_TWI(DS3231,0,clock_buffer,7);
                      break; // "Время и Дата"                    
                  }
                  delay_ms(500);
                  displ_num = 4; newSetButt = 1; break;
          }
          byte = 10;
        break;
        //- УСТАНОВКИ АНАЛОГОВЫЕ ВЫХОДЫ -
        case 6:
          switch (byte){
               case 0: displ_num = 1; newSetButt = 1; break;
            case 1: if (--numSet<0) numSet = 0;    break;
            case 2: if (++numSet>MAX_SET-1) numSet = MAX_SET-1;    break;
            case 3: displ_num = 8; newSetButt = 1; 
                for (byte=0;byte<MAX_SET;byte++) newval[byte] = analogSet[byte]; break;
            break;
          }
          byte = 10;
        break;
        //- УСТАНОВКИ РЕДЛЕЙНЫЕ ВЫХОДЫ -
        case 7:
          switch (byte){
            case 0: displ_num = 2; newSetButt = 1; break;
            case 1: if (--numSet<0) numSet = 0;    break;
            case 2: if (++numSet>MAX_SET-1) numSet = MAX_SET-1;    break;
            case 3: displ_num = 9; newSetButt = 1; 
                for (byte=0;byte<MAX_SET;byte++) newval[byte] = relaySet[byte]; break;
            break;
          }
          byte = 10;
        break;
        //- РЕДАКТИР. АНАЛОГОВЫЕ ВЫХОДЫ -
        case 8:
          switch (byte){
            case 0: displ_num = 6; newSetButt = 1; break;
            case 1: if(++newval[numSet]>100) newval[numSet]=100; break;
            case 2: if(--newval[numSet]<-1) newval[numSet] = -1; break;
            case 3: ILI9341_FillScreen(0, max_X, 0, max_Y, fillScreen);
                  ILI9341_WriteString(5,100,"ВЫПОЛНЯЮ  ЗАПИСЬ",Font_11x18,GREEN,fillScreen,1);
                  analogSet[numSet] = newval[numSet];
                  delay_ms(1000);
                  displ_num = 6; newSetButt = 1; break;
          }
          byte = 10;
        break;
        //- РЕДАКТИР. РЕЛЕЙНЫЕ ВЫХОДЫ -
        case 9:
          switch (byte){
               case 0: displ_num = 6; newSetButt = 1; break;
            case 1: if(++newval[numSet]>2) newval[numSet]= 2; break;
            case 2: if(--newval[numSet]<0) newval[numSet]= 0; break;
            case 3: ILI9341_FillScreen(0, max_X, 0, max_Y, fillScreen);
                  ILI9341_WriteString(5,100,"ВЫПОЛНЯЮ  ЗАПИСЬ",Font_11x18,GREEN,fillScreen,1);
                  relaySet[numSet] = newval[numSet];
                  delay_ms(1000);
                  displ_num = 7; newSetButt = 1; break;
          }
          byte = 10;
        break;
        default: displ_num = 0; newSetButt = 1;
     }
}

