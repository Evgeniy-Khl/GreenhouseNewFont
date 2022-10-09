unsigned char txt[10];

void fraction(signed int t){
  if (t<0) {t = -t; signchar = '-';} else signchar = ' ';
  frcval = t%10; intval = t/10;
}

char newButtonCheck(void){
 char res=0;
   if(checkTouch()){
     res = (checkButton(buttonCount));
     if (res < buttonCount) newButton = res; // была нажата кнопка
     else {newButton = 100; res=0;}          // нажатие не обнаружено
   }
   return res;
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
  if(newButtonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
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
      if(newButtonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
//--- Индикация RH ------
      sprintf(buff,"[%3u%%]",set[1][0]); // ЗАДАНИЕ RH
      ILI9341_WriteString(5,pointY+5,buff,Font_11x18,bordWindow,fillWindow,1);
      ILI9341_WriteString(5,pointY+25,"вологысть",Font_11x18,bordWindow,fillWindow,1);
      if (pvRH>100) sprintf(buff,"***%%",pvRH);
      else sprintf(buff,"%3u%% ",pvRH);
      ILI9341_WriteString(142,pointY,buff,Font_11x18,bordWindow,fillWindow,3); 
      // индикация тревоги alarm[1]
      switch (alarm[1]) {
        case 0: temp=GREEN; break;
        case 1: temp=RED;   break;
        default: temp=GREEN;
      };
      ILI9341_FillRectangle(280,pointY+2,30,45,temp);      
      if(newButtonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
//--- Индикация CO2 ----
//CO2module = 1;
//pHsensor = 1;
//pvCO2 = 2500;
//pvPH = 75;
      pointY += 55;
      if(CO2module){
        if(error&0x08){
          ILI9341_WriteString(10,pointY,"CO2 помилка модуля!",Font_11x18,YELLOW,RED,1);
        }
        else {
          sprintf(buff,"CO2   %4u",pvCO2);
          ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
          ILI9341_WriteString(250,pointY+10,"ppm",Font_11x18,bordWindow,fillWindow,1);
        }
      }
      else {
        ILI9341_WriteString(10,pointY,"CO2 модуль выдсутный.",Font_11x18,bordWindow,fillWindow,1);
      }
//--- Индикация рH-4502 -----
      pointY += 35;
      if(pHsensor){
        if(error&0x08){
          ILI9341_WriteString(10,pointY,"рН помилка модуля!",Font_11x18,YELLOW,RED,1);
        }
        else {
          temp = pvPH; fraction(temp);     // проверка знака температуры
          sprintf(buff,"рН   %2u.%u",intval,frcval);
          ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
          ILI9341_WriteString(230,pointY+10,"одиниць",Font_11x18,bordWindow,fillWindow,1);
        }
      }
      else {
        ILI9341_WriteString(10,pointY,"рН  модуль выдсутный.",Font_11x18,bordWindow,fillWindow,1);
      }
      if(newButtonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
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
    ILI9341_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    ILI9341_FillRectangle(buttons[num].x-3, TFTBUTTON, buttons[num].w+3, buttons[num].h, fillWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(WHITE, WHITE, BLACK, 0, "Повытря");
    drawButton(fillWindow, fillWindow, bordWindow, 1, "Грунт");
    drawButton(WHITE, WHITE, BLACK, 2, "Управл");
    drawButton(WHITE, WHITE, BLACK, 3, "Налашт");
    ILI9341_DrawVerticalLine(buttons[num].x-2, TFTBUTTON+8, buttons[num].h+5, bordWindow);
    ILI9341_DrawVerticalLine(buttons[num].x+buttons[num].w, TFTBUTTON+8, buttons[num].h+5, bordWindow);
    ILI9341_DrawHorizontalLine(buttons[num].x-1, max_Y-2, buttons[num].w, bordWindow);
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
      // нет индикации тревоги
      pointY += 35;
      if(newButtonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
    };
        
}

//------------------------------ РЕЛЕЙНЫЕ ВЫХОДЫ --------------------------------------------
void displ_2(void){
 char i, num=2;
 unsigned int fillWindow = GRAY1, bordWindow = BLACK, color_box; 
  pointY=7;
  if(newSetButt){
    newSetButt = 0;
    ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
    initializeButtons(4,1,25);// 4 колонки; одна строка; высота 25
    ILI9341_DrawRectangle(2, 2, max_X-5, TFTBUTTON+6, bordWindow);
    ILI9341_FillRectangle(buttons[num].x-3, TFTBUTTON, buttons[num].w+3, buttons[num].h, fillWindow);
    //---------- фон ------- рамкa --- текст - номер -текст ---------
    drawButton(WHITE, WHITE, BLACK, 0, "Повытря");
    drawButton(WHITE, WHITE, BLACK, 1, "Грунт");
    drawButton(fillWindow, fillWindow, bordWindow, 2, "Управл");
    drawButton(WHITE, WHITE, BLACK, 3, "Налашт");
    ILI9341_DrawVerticalLine(buttons[num].x-2, TFTBUTTON+8, buttons[num].h+5, bordWindow);
    ILI9341_DrawVerticalLine(buttons[num].x+buttons[num].w, TFTBUTTON+8, buttons[num].h+5, bordWindow);
    ILI9341_DrawHorizontalLine(buttons[num].x-1, max_Y-2, buttons[num].w, bordWindow);
    ILI9341_WriteString(90,pointY,"СТАН УПРАВЛЫННЯ",Font_11x18,bordWindow,fillWindow,1);
  }
//---- РЕЛЕЙНЫЕ ВЫХОДЫ ----
  pointY += 20;
  for (i=0;i<4;i++){
    sprintf(buff,"РЕЛЕ  N%u: ",i+1);
    switch (relay[i]){
      case 2: strcat(buff,"АВТ"); break;
      case 1: strcat(buff,"ON "); break;
      case 0: strcat(buff,"OFF"); break;
    };
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    if(relOut[i]) color_box=YELLOW; else color_box=BLACK;
    ILI9341_FillRectangle(150,pointY,45,18,color_box);
    pointY = pointY+22;
    if(newButtonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
  };
//---- АНАЛОГОВЫЕ ВЫХОДЫ ----
//  pointY = pointY+30;
  for (i=0;i<4;i++){
    sprintf(buff,"ВИХЫД N%u: ",i+1);
    if (analog[i]==-1) strcat(buff,"АВТ"); else strcat(buff,"РУЧ");
    sprintf(txt," %3u%% ",analogOut[i]);
    strcat(buff,txt);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    pointY = pointY+22;
    if(newButtonCheck()) return; //++++ проверим нажатие кнопки +++++++++++++++++++++++++
  };
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