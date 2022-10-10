unsigned char txt[10];

void fraction(signed int t){
  if (t<0) {t = -t; signchar = '-';} else signchar = ' ';
  frcval = t%10; intval = t/10;
}

void checkDisplNum(void){
  unsigned char i;
    if(point_Y>200){
        for (i=0; i<4; i++){if(contains(i)) break;}// проверка попадания новой координаты в область кнопки
        if(i<4){BeepT = 20; displ_num = i; newSetButt = 1;}
    }
}

//void drawButtEdit(unsigned int fW){
//    newSetButt = 0;
//    ILI9341_FillScreen(0, max_X, 0, max_Y, fW);
//    initializeButtons(2,1,25);// 2 колонки; одна строка; высота 25
//    //---------- фон ------- рамкa --- текст - номер -текст ---------
//    drawButton(BLUE, WHITE, WHITE, 0, "Отмена");
//    drawButton(MAGENTA, BLACK, BLACK, 1, "Сохран.");
//}

//-------------------------------- СТАН ПОВІТРЯ. ----------------------------------------
void displ_0(void){
 unsigned char i;
 unsigned int fillWindow = BLUE1, bordWindow = BLACK, temp;
    if(newSetButt){
        newSetButt = 0;
        ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
        initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
        //---------- фон ------- рамкa --- текст - номер -текст ---------
        drawButton(fillWindow, fillWindow, bordWindow, 0, "Повытря");
        drawButton(WHITE, WHITE, BLACK, 1, "Грунт");
        drawButton(WHITE, WHITE, BLACK, 2, "Управл");
        drawButton(WHITE, WHITE, BLACK, 3, "Налашт");
    }
//--- Индикация часов ------
    pointY = 3;
    if(Clock_Ok){
        read_TWI(DS3231,0,clock_buffer,7);// чтение данных часовой микросхемы
        sprintf(buff,"%02x:%02x   %02x.%02x.20%02x",clock_buffer[2],clock_buffer[1],clock_buffer[4],clock_buffer[5],clock_buffer[6]);//час:мин дата.мес.год
        ILI9341_WriteString(70,pointY,buff,Font_11x18,BLACK,WHITE,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,WHITE,BLACK,1);// "БЕГУЩАЯ СТРОКА"
    }
    else {
        sprintf(buff,"Помилка часыв!"); 
        ILI9341_WriteString(70,pointY,buff,Font_11x18,YELLOW,RED,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,RED,YELLOW,1);// "БЕГУЩАЯ СТРОКА"
    }
    pointY += 22;
    if(checkTouch()) checkDisplNum();//***************************** проверим нажатие кнопки ***************************************
//--- Индикация t ВОЗДУХА -----
    temp = set[0][0]; fraction(temp);     // проверка знака температуры
    sprintf(buff,"[%2u.%u]",intval,frcval); // ЗАДАНИЕ T показываем с десятичным знаком
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
        default: temp=GREEN;
    }; 
    ILI9341_FillRectangle(280,pointY+2,30,45,temp);
    pointY += 55;
    if(checkTouch()) checkDisplNum();//***************************** проверим нажатие кнопки ***************************************
//--- Индикация RH ------
    sprintf(buff,"[%3u%%]",set[1][0]); // ЗАДАНИЕ RH
    ILI9341_WriteString(5,pointY+5,buff,Font_11x18,bordWindow,fillWindow,1);
    ILI9341_WriteString(5,pointY+25,"вологысть",Font_11x18,bordWindow,fillWindow,1);
    if (pvRH>100) sprintf(buff,"***%%",pvRH); else sprintf(buff,"%3u%% ",pvRH);
    ILI9341_WriteString(142,pointY,buff,Font_11x18,bordWindow,fillWindow,3); 
    // индикация тревоги alarm[1]
    switch (alarm[1]) {
        case 0: temp=GREEN; break;
        case 1: temp=RED;   break;
        default: temp=GREEN;
    };
    ILI9341_FillRectangle(280,pointY+2,30,45,temp);
    if(checkTouch()) checkDisplNum();//***************************** проверим нажатие кнопки ***************************************      
//--- Индикация CO2 ----
//CO2module = 1;
//pHsensor = 1;
//pvCO2 = 2500;
//pvPH = 75;
    pointY += 55;
    if(CO2module){
    if(error&0x08) ILI9341_WriteString(10,pointY,"CO2 помилка модуля!",Font_11x18,YELLOW,RED,1);
    else {
        sprintf(buff,"CO2   %4u",pvCO2);
        ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
        ILI9341_WriteString(250,pointY+10,"ppm",Font_11x18,bordWindow,fillWindow,1);
    }
    }
    else ILI9341_WriteString(10,pointY,"CO2 модуль выдсутный.",Font_11x18,bordWindow,fillWindow,1);
//--- Индикация рH-4502 -----
    pointY += 35;
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
    if(checkTouch()) checkDisplNum();//***************************** проверим нажатие кнопки ***************************************
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
        drawButton(WHITE, WHITE, BLACK, 0, "Повытря");
        drawButton(fillWindow, fillWindow, bordWindow, 1, "Грунт");
        drawButton(WHITE, WHITE, BLACK, 2, "Управл");
        drawButton(WHITE, WHITE, BLACK, 3, "Налашт");
        ILI9341_WriteString(100,pointY,"СТАН ГРУНТУ",Font_11x18,bordWindow,fillWindow,1);
    }
//--- Индикация t ГРУНТА ---
    pointY += 20;
    for (i=0;i<ds18b20;i++){
        sprintf(buff,"датчик %u",i+1);
        ILI9341_WriteString(5,pointY+12,buff,Font_11x18,bordWindow,fillWindow,1);
        temp = t.point[i];
        if(temp>1250) sprintf(buff,"**.*");
        else {
            fraction(temp);     // проверка знака температуры
            sprintf(buff,"%2u.%u",intval,frcval); // T датчиков показываем с десятичным знаком 
        }
        ILI9341_WriteString(120,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
        pointY += 35;
    };
    if(checkTouch()) checkDisplNum();//***************************** проверим нажатие кнопки ***************************************
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
        drawButton(WHITE, WHITE, BLACK, 0, "Повытря");
        drawButton(WHITE, WHITE, BLACK, 1, "Грунт");
        drawButton(fillWindow, fillWindow, bordWindow, 2, "Управл");
        drawButton(WHITE, WHITE, BLACK, 3, "Налашт");
        //--
        initializePlus(pointY+25, 8, 16);
        for (i=0;i<plusCount;i++) drawPlus(i, fillWindow);
        //--
        ILI9341_WriteString(90,pointY,"СТАН УПРАВЛЫННЯ",Font_11x18,bordWindow,fillWindow,1);
    }
//***************************** проверим кнопки управления реле ***************************************
    if(checkTouch()){
//sprintf(buff,"X%4u; Y%4u",point_X,point_Y);
//ILI9341_WriteString(5,TFTBUTTON-60,buff,Font_11x18,WHITE,BLACK,1);
      if(point_X>220&&point_Y<100){
        for (i=0; i<8; i++){if(containsPlus(i)) break;}// проверка верхней половины кнопок
        if(i<8){
            BeepT = 20; 
            if(i%2) x=-1; else x=1;   // кнопка "+" или кнопка "-"
            i /= 2;
            relay[i]+=x; if(relay[i]>2) relay[i]=0; else if(relay[i]<0) relay[i]=2;}
      }
      else checkDisplNum();
    }
//---- РЕЛЕЙНЫЕ ВЫХОДЫ ----
    pointY += 20;
    for (i=0;i<4;i++){
    sprintf(buff,"РЕЛЕ  N%u: ",i+1);
    switch (relay[i]){
      case 2: strcat(buff,"АВТ"); break;
      case 1: strcat(buff,"ON "); relOut[i]=1; break;
      case 0: strcat(buff,"OFF"); relOut[i]=0; break;
    };
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    if(relOut[i]) color_box=YELLOW; else color_box=BLACK;
    ILI9341_FillRectangle(160,pointY,40,18,color_box);
    pointY = pointY+22;
    }

//***************************** проверим кнопки управления 0-10v ***************************************
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
            i /= 2; i -= 4;
//sprintf(buff,"i=%2u; x=%2i",i,x);
//ILI9341_WriteString(5,TFTBUTTON-45,buff,Font_11x18,WHITE,BLACK,1);
            analog[i]+=x*10; if(analog[i]>100) analog[i]=100; else if(analog[i]==9) analog[i]=10; else if(analog[i]<-1) analog[i]=-1;}
      }
      else checkDisplNum();
    }
//---- АНАЛОГОВЫЕ ВЫХОДЫ ----
    for (i=0;i<4;i++){
    sprintf(buff,"ВИХЫД N%u: ",i+1);
    if(analog[i]==-1) strcat(buff,"АВТ"); else {strcat(buff,"РУЧ"); analogOut[i]=analog[i];}
    sprintf(txt," %3u%% ",analogOut[i]);
    strcat(buff,txt);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    pointY = pointY+22;    
    };
   if(checkTouch()) checkDisplNum();//***************************** проверим нажатие кнопки ***************************************
}

void displ_3(void){
 char item, X_left;
 signed char x;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, temp;
  pointY=7;
  if (newSetButt){
//    drawButtEdit(fillWindow);
    initializePlus(pointY+25, 4, 30);
    for (item=0;item<plusCount;item++) drawPlus(item, fillWindow);
    sprintf(buff,"ЗАДАНИЯ ФАЗЫ %u", pointY);
    ILI9341_WriteString(50,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
}

void display(void){
 switch (displ_num){
    case 0: displ_0(); break;
    case 1: displ_1(); break;
    case 2: displ_2(); break;
//    case 3: displ_3(); break;
//    case 4: displ_4(); break;
//    case 5: displ_5(); break;
//    case 6: displ_6(); break;
//    case 7: displ_7(); break;
//    case 8: displ_8(); break;
//    case 9: displ_9(); break;
    default: displ_0(); break;
  }
}