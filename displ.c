unsigned char txt[20];

void fraction(signed int t){
  if (t<0) {t = -t; signchar = '-';} else signchar = ' ';
  frcval = t%10; intval = t/10;
}

unsigned char checkDisplNum(void){
  unsigned char i;
    if(point_Y>200){
        for (i=0; i<4; i++){if(contains(i)) break;}// проверка попадания новой координаты в область кнопки
        if(i<4){BeepT = 20; displ_num = i; newSetButt = 1; i=10;}
    }
    return i;
}

void checkSkipEdit(void){
  unsigned char i;
    if(point_Y>200){
        for (i=0; i<4; i++){if(contains(i)) break;}// проверка попадания новой координаты в область кнопки
        if(i<4){BeepT = 20; touchpad(i);}
    }
}


//-------------------------------- СТАН ПОВІТРЯ. ----------------------------------------
void displ_0(void){
 unsigned char i;
 unsigned int fillWindow = BLUE1, bordWindow = BLACK, temp;
    if(newSetButt){
        newSetButt = 0;
        ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
        initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
        //---------- фон ------- рамкa --- текст - номер -текст ---------
        drawButton(fillWindow, fillWindow, bordWindow, 0, "Перший");
        drawButton(WHITE, WHITE, BLACK, 1, "Другий");
        drawButton(WHITE, WHITE, BLACK, 2, "Управл");
        drawButton(WHITE, WHITE, BLACK, 3, "Налашт");
    }
//--- Индикация часов ------
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
    pointY = 3;
    if(Clock_Ok){
        if(Night) ILI9341_WriteString(5,pointY,"НЫЧ ",Font_11x18,WHITE,BLACK,1); else ILI9341_WriteString(5,pointY,"ДЕНЬ",Font_11x18,BLACK,WHITE,1);
        read_TWI(DS3231,0,clock_buffer,7);// чтение данных часовой микросхемы
        sprintf(buff,"%02x:%02x   %02x.%02x.20%02x",clock_buffer[2],clock_buffer[1],clock_buffer[4],clock_buffer[5],clock_buffer[6]);//час:мин дата.мес.год
        if(Night){ILI9341_WriteString(70,pointY,buff,Font_11x18,WHITE,GRAY1,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,WHITE,BLACK,1);}// "БЕГУЩАЯ СТРОКА"
        else {ILI9341_WriteString(70,pointY,buff,Font_11x18,BLACK,GRAY1,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,BLACK,WHITE,1);}
    }
    else {
        sprintf(buff,"Помилка часыв!"); 
        ILI9341_WriteString(70,pointY,buff,Font_11x18,YELLOW,RED,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,RED,YELLOW,1);// "БЕГУЩАЯ СТРОКА"
    }
    pointY += 22;
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
//--- Индикация t ВОЗДУХА -----
    temp = set[0][Night]; fraction(temp);     // проверка знака температуры
    sprintf(buff,"[%2u.%u]",intval,frcval);   // ЗАДАНИЕ T показываем с десятичным знаком
    ILI9341_WriteString(5,pointY+5,buff,Font_11x18,bordWindow,fillWindow,1);
    ILI9341_WriteString(5,pointY+25,"температура",Font_11x18,bordWindow,fillWindow,1);
    if (pvT>1250) sprintf(buff,"**.*");
    else {
        temp = pvT; fraction(temp);     // проверка знака температуры
        sprintf(buff,"%2u.%u",intval,frcval); // T датчиков показываем с десятичным знаком 
    }
    ILI9341_WriteString(140,pointY,buff,Font_11x18,bordWindow,fillWindow,3);
    // индикация тревоги alarm[0]
    switch (alarm[0]) {
        case 0: temp=GREEN; break;
        case 1: temp=RED;   break;
        default: temp=fillWindow;
    }; 
    ILI9341_FillRectangle(280,pointY+2,30,45,temp);
    pointY += 55;
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
//--- Индикация RH ------
    sprintf(buff,"[%3u%%]",set[1][Night]); // ЗАДАНИЕ RH
    ILI9341_WriteString(5,pointY+5,buff,Font_11x18,bordWindow,fillWindow,1);
    ILI9341_WriteString(5,pointY+25,"вологысть",Font_11x18,bordWindow,fillWindow,1);
    if(pvRH>100) sprintf(buff,"***%%",pvRH); else sprintf(buff,"%3u%%",pvRH);
    ILI9341_WriteString(142,pointY,buff,Font_11x18,bordWindow,fillWindow,3); 
    // индикация тревоги alarm[1]
    switch (alarm[1]) {
        case 0: temp=GREEN; break;
        case 1: temp=RED;   break;
        default: temp=fillWindow;
    };
    ILI9341_FillRectangle(280,pointY+2,30,45,temp);
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
// --- Состояние ТАЙМЕРОВ -------
    pointY += 60;
    ILI9341_WriteString(90,pointY,"СТАН ТАЙМЕРЫВ",Font_11x18,bordWindow,fillWindow,1);
    pointY += 20;
    sprintf(buff,"таймер 1 ");
    strcat(buff,txtTimer);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    pointY += 20;
    sprintf(buff,"таймер 2 ");
    strcat(buff,txt);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);      
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
}

//-------------------------------- СТАН ГРУНТУ. -------------------------------------------
void displ_1(void){
 unsigned char i, num = 1;
 unsigned int fillWindow = YELLOW2, bordWindow = BLACK, temp;
    pointY = 7;
    if(newSetButt){
        newSetButt = 0;
        ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
        initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
        //---------- фон ------- рамкa --- текст - номер -текст ---------
        drawButton(WHITE, WHITE, BLACK, 0, "Перший");
        drawButton(fillWindow, fillWindow, bordWindow, 1, "Другий");
        drawButton(WHITE, WHITE, BLACK, 2, "Управл");
        drawButton(WHITE, WHITE, BLACK, 3, "Налашт");
        ILI9341_WriteString(100,pointY,"СТАН ГРУНТУ",Font_11x18,bordWindow,fillWindow,1);
    }
//--- Индикация t ГРУНТА ---
    pointY += 20;
    for (i=0;i<ds18b20;i++){
        sprintf(buff,"зона %u",i+1);
        ILI9341_WriteString(5,pointY+12,buff,Font_11x18,bordWindow,fillWindow,1);
        temp = t.point[i];
        if(temp>1250) sprintf(buff,"**.*");
        else {
            fraction(temp);     // проверка знака температуры
            sprintf(buff,"%2u.%u  ---%%",intval,frcval); // T датчиков показываем с десятичным знаком 
        }
        ILI9341_WriteString(90,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
        pointY += 35;
    };
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
    pointY += 10;
//--- Модуль грунта ---
    if(Soilmodule){
        if(error&0x08) ILI9341_WriteString(10,pointY,"Помилка модуля грунту!",Font_11x18,YELLOW,RED,1);
        else {
            sprintf(buff,"грунт   ---%%");
            ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
        }
    }
    else ILI9341_WriteString(10,pointY,"Модуль грунту выдсутный.",Font_11x18,bordWindow,fillWindow,1);
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
    pointY += 30;
//--- Модуль CO2 ----
//CO2module = 1;
//pHsensor = 1;
//pvCO2 = 2500;
//pvPH = 75;
    if(CO2module){
        if(error&0x08) ILI9341_WriteString(10,pointY,"CO2 помилка модуля!",Font_11x18,YELLOW,RED,1);
        else {
            sprintf(buff,"CO2   %4u",pvCO2);
            ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
            ILI9341_WriteString(250,pointY+10,"ppm",Font_11x18,bordWindow,fillWindow,1);
        }
    }
    else ILI9341_WriteString(10,pointY,"CO2 модуль выдсутный.",Font_11x18,bordWindow,fillWindow,1);
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
//--- Модуль рH-4502 -----
    pointY += 30;
    if(pHsensor){
        if(error&0x08) ILI9341_WriteString(10,pointY,"рН помилка модуля!",Font_11x18,YELLOW,RED,1);
        else {
          temp = pvPH; fraction(temp);
          sprintf(buff,"рН   %2u.%u",intval,frcval);
          ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
          ILI9341_WriteString(230,pointY+10,"одиниць",Font_11x18,bordWindow,fillWindow,1);
        }
    }
    else ILI9341_WriteString(10,pointY,"рН  модуль выдсутный.",Font_11x18,bordWindow,fillWindow,1);
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
}

//------------------------------ РЕЛЕЙНЫЕ ВЫХОДЫ --------------------------------------------
void displ_2(void){
 signed char i,x;
 unsigned int fillWindow = GRAY1, bordWindow = BLACK, color_box; 
    pointY=7;
    if(newSetButt){
        newSetButt = 0;
        ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
        initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
        //---------- фон ------- рамкa --- текст - номер -текст ---------
        drawButton(WHITE, WHITE, BLACK, 0, "Перший");
        drawButton(WHITE, WHITE, BLACK, 1, "Другий");
        drawButton(fillWindow, fillWindow, bordWindow, 2, "Управл");
        drawButton(WHITE, WHITE, BLACK, 3, "Налашт");
        //--
        initializePlus(pointY+25, 8, 16);
        for (i=0;i<plusCount;i++) drawPlus(i, fillWindow);
        //--
        ILI9341_WriteString(90,pointY,"СТАН УПРАВЛЫННЯ",Font_11x18,bordWindow,fillWindow,1);
    }
//=========================== проверим кнопки управления реле ==================================
    if(checkTouch()){
    //sprintf(buff,"X%4u; Y%4u",point_X,point_Y);
    //ILI9341_WriteString(5,TFTBUTTON-60,buff,Font_11x18,WHITE,BLACK,1);
        if(point_X>220&&point_Y<100){
            for (i=0; i<8; i++){if(containsPlus(i)) break;}// проверка верхней половины кнопок
            if(i<8){
                BeepT = 20; 
                if(i%2) x=-1; else x=1;   // кнопка "+" или кнопка "-"
                i /= 2;
                relaySet[i]+=x; if(relaySet[i]>2) relaySet[i]=0; else if(relaySet[i]<0) relaySet[i]=2;
            }
        }
        else checkDisplNum();
    }
//---- РЕЛЕЙНЫЕ ВЫХОДЫ ----
    pointY += 20;
    for (i=0;i<4;i++){
        sprintf(buff,"РЕЛЕ  N%u: ",i+1);
        if(relaySet[i]==2) strcat(buff,"АВТ"); else strcat(buff,"РУЧ");
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
        if(relOut[i]) color_box=YELLOW; else color_box=BLACK;
        ILI9341_FillRectangle(160,pointY,40,18,color_box);
        pointY = pointY+22;
    }
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
//========================= проверим кнопки управления 0-10v ===================================
    if(checkTouch()){
//sprintf(buff,"X%4u; Y%4u",point_X,point_Y);
//ILI9341_WriteString(5,TFTBUTTON-60,buff,Font_11x18,WHITE,BLACK,1);
        if(point_X>220&&point_Y>100&&point_Y<200){
            for (i=8; i<16; i++){if(containsPlus(i)) break;}// проверка нижней половины кнопок
            sprintf(buff,"i=%2u",i);
            ILI9341_WriteString(5,TFTBUTTON-30,buff,Font_11x18,WHITE,BLACK,1);
            if(i<16){
                BeepT = 20; 
                if(i%2) x=-1; else x=1;   // кнопка "+" или кнопка "-"
                i /= 2; i -= 4; analogSet[i]+=x*10; // шаг изменения 10 ед.
                if(analogSet[i]>100) analogSet[i]=100; else if(analogSet[i]==9) analogSet[i]=10; else if(analogSet[i]<-1) analogSet[i]=-1;
//sprintf(buff,"i=%2u; x=%2i",i,x);
//ILI9341_WriteString(5,TFTBUTTON-45,buff,Font_11x18,WHITE,BLACK,1);                 
            }
        }
        else checkDisplNum();
    }
//---- АНАЛОГОВЫЕ ВЫХОДЫ ----
    for (i=0;i<4;i++){
        sprintf(buff,"ВИХЫД N%u: ",i+1);
        if(analogSet[i]==-1) strcat(buff,"АВТ"); else {strcat(buff,"РУЧ"); analogOut[i]=analogSet[i];}
        sprintf(txt," %3u%% ",analogOut[i]);
        strcat(buff,txt);
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
        pointY = pointY+22;    
    }
   if(checkTouch()) if(checkDisplNum()==10) return;//***************************** проверим нажатие кнопки ***************************************
}

void drawButtEdit(unsigned int fW){
    newSetButt = 0;
    ILI9341_FillScreen(0, max_X, 0, max_Y, fW);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Выдм.");
    drawButton(GREEN, BLACK, BLACK, 1, "+");
    drawButton(GREEN, BLACK, BLACK, 2, "-");
    drawButton(MAGENTA, BLACK, BLACK, 3, "Збер.");
}

void drawButtSkip(unsigned int fW){
    newSetButt = 0;
    ILI9341_FillScreen(0, max_X, 0, max_Y, fW);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(BLUE, WHITE, WHITE, 0, "Вихыд");
    drawButton(GREEN, BLACK, BLACK, 1, "<-");
    drawButton(GREEN, BLACK, BLACK, 2, "->");
    drawButton(MAGENTA, BLACK, BLACK, 3, "Кор.");
}

//--- Установки Общий список ----
void displ_3(void){
 char item;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, color_txt = BLACK, color_fon = GREEN1; 
  pointY=7;
  if (newSetButt){
    drawButtSkip(fillWindow);
    ILI9341_WriteString(50,pointY,"НАЛАШТУВАННЯ СИСТЕМИ",Font_11x18,bordWindow,fillWindow,1);
  }
  pointY += 25;
  for (item = 0; item < MAX_MENU; item++){
    sprintf(buff,"%s", setMenu[item]);   //  %10s
    if(item == numMenu){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
    ILI9341_WriteString(20,pointY,buff,Font_11x18,color_txt,color_fon,1);    
    pointY += 25;
  }
  if(checkTouch()) checkSkipEdit();//***************************** проверим нажатие кнопки ***************************************
}

//- Установки пунктов -
void displ_4(void){
 char item, tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, color_txt = BLACK, color_fon = GREEN1, temp;
  pointY=7;
  if (newSetButt){
    drawButtSkip(fillWindow);
    sprintf(buff,"%s",setMenu[numMenu]);
    ILI9341_WriteString(80,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
  }
  pointY += 25;
  switch (numMenu){
    case 0: //-------- "Температура" ---------
        for (item = 0; item < MAX_5;item++){
            if (item<4){
                temp = set[0][item]; tmpv0 = temp%10; tmpv1 = temp/10;
                sprintf(buff,"%7s = %2u.%u", setName0[item],tmpv1,tmpv0); // T с десятичным знаком
            }
            else {
                sprintf(buff,"%7s = ", setName0[item]);
                if(set[0][item]==1) strcat(buff,"НАГРЫВ");
                else  strcat(buff,"ОХОЛОДЖЕННЯ"); 
            }
            if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(15,pointY,buff,Font_11x18,color_txt,color_fon,1);
            pointY += 25;
        }
    break;
    case 1: //-------- "Влажность" -----------
        for (item = 0; item < MAX_5;item++){
            if(item<4) sprintf(buff,"%8s = %i%%", setName0[item],set[1][item]);
            else {
                sprintf(buff,"%8s = ", setName0[item]);
                if(set[1][item]==1) strcat(buff,"ЗВОЛОЖЕННЯ");
                else  strcat(buff,"ОСУШЕННЯ"); 
            }
            if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(15,pointY,buff,Font_11x18,color_txt,color_fon,1);
            pointY += 25;
        }
    break;
    case 2: //-------- "Таймер" --------------
        for (item = 0; item < MAX_6;item++){
            if(item==1||item==3){
                sprintf(buff,"%8s = ", setName1[item]);
                if (set[4][item]==0) strcat(buff,"СЕКУНД");
                else strcat(buff,"ХВИЛИН");
            }
            else sprintf(buff,"%8s = %i", setName1[item],set[4][item]);
            if(item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(15,pointY,buff,Font_11x18,color_txt,color_fon,1);
            pointY += 25;
        }
    break;
    case 3: //-------- "День Ночь" -----------
        for (item = 0; item < MAX_6;item++){
            sprintf(buff,"%11s = %02i:00 год.", setName2[item],rtcTodec(set[5][item]));
            if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(5,pointY,buff,Font_11x18,color_txt,color_fon,1);
            pointY += 25;
        }
    break;
    case 4: //-------- "Время и Дата" ---------------
        for (item = 0; item < MAX_5;item++){
            if (item<2) tmpv0 = clock_buffer[item+1];
            else tmpv0 = clock_buffer[item+2]; 
            sprintf(buff,"%8s = %x", setName7[item],tmpv0);
            if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(5,pointY,buff,Font_11x18,color_txt,color_fon,1);            
            pointY += 25;
        }
    break;    
  };
  if(checkTouch()) checkSkipEdit();//***************************** проверим нажатие кнопки ***************************************   
}

//- РЕДАКТИРОВАНИЕ -
void displ_5(void){
 char tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, temp;
  pointY=7;
  if (newSetButt){
    drawButtEdit(fillWindow);
    pauseEdit = 3;
    sprintf(buff,"РЕДАГУВАННЯ %s", setMenu[numMenu]);
    ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,1); 
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  pointY = pointY+50;
   switch (numMenu){
    case 0: //-------- "Температура" ---------
      if (numSet<4){
        temp = newval[numSet]; tmpv0 = temp%10; tmpv1 = temp/10;
        sprintf(buff,"%5s = %2u.%u", setName0[numSet],tmpv1,tmpv0); // T с десятичным знаком
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
      }
      else {
        sprintf(buff,"%7s = ", setName0[numSet]);
        if(newval[numSet]==1) strcat(buff,"НАГРЫВ     ");
        else strcat(buff,"ОХОЛОДЖЕННЯ");
        ILI9341_WriteString(15,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
      }      
    break;
    case 1: //-------- "Влажность" -----------
      if(numSet<4){
        sprintf(buff,"%7s = %i%%", setName0[numSet],newval[numSet]);
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
      }
      else {
        sprintf(buff,"%8s = ", setName0[numSet]);
        if(newval[numSet]==1) strcat(buff,"ЗВОЛОЖЕННЯ");
        else  strcat(buff,"ОСУШЕННЯ  ");
        ILI9341_WriteString(15,pointY,buff,Font_11x18,bordWindow,fillWindow,1); 
      }
      
    break;
    case 2: //-------- "Таймер" --------------
        sprintf(buff,"%5s: %i ", setName1[numSet], newval[numSet]);
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
    break;
    case 3: //-------- "День Ночь" -----------
        sprintf(buff,"%5s: %i ", setName2[numSet], newval[numSet]);
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
    break;
    case 4: //-------- "Время и Дата" ---------------
        sprintf(buff,"%7s: %u ", setName7[numSet], newval[numSet]);
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
    break;
   }
   if(--pauseEdit==0){pauseEdit=1; if(checkTouch()) checkSkipEdit();}//***************************** проверим нажатие кнопки *************************************** 
}

//--- Установки Общий коэффициентов ----
void displ_6(void){
 char item;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, color_txt = BLACK, color_fon = GREEN1; 
  pointY=7;
  if (newSetButt){
    drawButtSkip(fillWindow);
    ILI9341_WriteString(20,pointY,"НАЛАШТУВАННЯ КОЕФЫЦЫЭНТЫВ",Font_11x18,bordWindow,fillWindow,1);
  }
  pointY += 35;
  for (item = 0; item < 4; item++){
    sprintf(buff,"Набыр коефыцыэнтыв N%u",item+1);
    if(item == numMenu){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
    ILI9341_WriteString(20,pointY,buff,Font_11x18,color_txt,color_fon,1);    
    pointY += 25;
  }
  if(checkTouch()) checkSkipEdit();//***************************** проверим нажатие кнопки ***************************************
}

//- Установки пунктов -
void displ_7(void){
 char item, tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, color_txt = BLACK, color_fon = GREEN1, temp;
    pointY=7;
    if (newSetButt){
    drawButtSkip(fillWindow);
//    sprintf(buff,"%s",setName3[numMenu]);
    sprintf(buff,"Набыр коефыцыэнтыв N%u",numMenu+1);
    ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    }
    pointY += 25;
    for (item = 0; item < MAX_4;item++){
        sprintf(buff,"%7s = %i", setName3[item],limit[numMenu][item]);
        if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
        ILI9341_WriteString(5,pointY,buff,Font_11x18,color_txt,color_fon,1);
        pointY += 25;
    }
   if(checkTouch()) checkSkipEdit();//***************************** проверим нажатие кнопки ***************************************   
}

//- РЕДАКТИРОВАНИЕ -
void displ_8(void){
 char tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, temp;
  pointY=7;
  if (newSetButt){
    drawButtEdit(fillWindow);
    pauseEdit = 3;
    sprintf(buff,"РЕДАГУВАННЯ коефыцыэнтыв N%u", numMenu+1);
    ILI9341_WriteString(10,pointY,buff,Font_11x18,bordWindow,fillWindow,1); 
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    pointY = pointY+50;
    sprintf(buff,"%7s:%3i", setName3[numSet], newval[numSet]);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
    if(--pauseEdit==0){pauseEdit=1; if(checkTouch()) checkSkipEdit();}//***************************** проверим нажатие кнопки *************************************** 
}

void display(void){
 switch (displ_num){
    case 0: displ_0(); break;
    case 1: displ_1(); break;
    case 2: displ_2(); break;
    case 3: displ_3(); break;
    case 4: displ_4(); break;
    case 5: displ_5(); break;
    case 6: displ_6(); break;
    case 7: displ_7(); break;
    case 8: displ_8(); break;
    default: displ_0(); break;
  }
}

