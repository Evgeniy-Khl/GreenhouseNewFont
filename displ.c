unsigned char pointY, txt[20];

void fraction(signed int t){
  if (t<0) {t = -t; signchar = '-';} else signchar = ' ';
  frcval = t%10; intval = t/10;
}

//-------------------------------- СТАН ПОВІТРЯ. ----------------------------------------------------
void displ_0(void){
 unsigned char i, keystop=0;
 unsigned int fillWindow = LIGHTGREEN, bordWindow = BLACK, temp;
    if(newSetButt){newSetButt=0; keystop=1; ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);}// очистка экрана
//--- Индикация часов ------
    pointY = 3;
    if(Clock_Ok){
        if(Night) ILI9341_WriteString(5,pointY,"НЫЧ ",Font_11x18,WHITE,BLACK,1); else ILI9341_WriteString(5,pointY,"ДЕНЬ",Font_11x18,BLACK,WHITE,1);
        read_TWI(DS_SRTC,0,clock_buffer,7);// чтение данных часовой микросхемы
        sprintf(buff,"%02x:%02x   %02x.%02x.20%02x",clock_buffer[2],clock_buffer[1],clock_buffer[4],clock_buffer[5],clock_buffer[6]);//час:мин дата.мес.год
        if(Night){ILI9341_WriteString(70,pointY,buff,Font_11x18,WHITE,GRAY1,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,WHITE,BLACK,1);}// "БЕГУЩАЯ СТРОКА"
        else {ILI9341_WriteString(70,pointY,buff,Font_11x18,BLACK,GREEN1,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,BLACK,LIGHTGREEN,1);}
    }
    else {
        sprintf(buff,"Помилка часыв!"); 
        ILI9341_WriteString(70,pointY,buff,Font_11x18,YELLOW,RED,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,RED,YELLOW,1);// "БЕГУЩАЯ СТРОКА"
    }
    if(keynum&&!keystop){checkkey(keynum); return;}//***************************** проверим номер кнопки ***************************************
    pointY += 22;
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
    if(keynum&&!keystop){checkkey(keynum); return;}//***************************** проверим номер кнопки ***************************************
    pointY += 52;
//--- Индикация RH ------
    sprintf(buff,"[%3u%%]",set[1][Night]); // ЗАДАНИЕ RH
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    ILI9341_WriteString(5,pointY+16,"вологысть",Font_11x18,bordWindow,fillWindow,1);
    if(pvRH>100) sprintf(buff,"***%%",pvRH); else sprintf(buff,"%3u%%",pvRH);
    ILI9341_WriteString(142,pointY,buff,Font_11x18,bordWindow,fillWindow,2); 
    if(keynum&&!keystop){checkkey(keynum); return;}//***************************** проверим номер кнопки ***************************************
// --- СТАН УПРАВЛIННЯ -------
    pointY += 43;
    ILI9341_WriteString(70,pointY,"СТАН УПРАВЛЫННЯ",Font_11x18,bordWindow,fillWindow,1);
    ILI9341_FillScreen(0, max_X,135, max_Y, fillWindow);
    if(errors+noAutoRel+noAutoAna){
        BeepT=10;
        if(errors&0x01){pointY += 18;      ILI9341_WriteString(5,pointY,"Помилка датчика температури",Font_11x18,RED,fillWindow,1);}
        else if(errors&0x02){pointY += 18; ILI9341_WriteString(5,pointY,"Помилка датчика вологосты",Font_11x18,RED,fillWindow,1);}
        else if(errors&0x10){pointY += 18; ILI9341_WriteString(5,pointY,"Велике відхилення температур",Font_11x18,RED,fillWindow,1);}
        else if(errors&0x20){pointY += 18; ILI9341_WriteString(5,pointY,"Велике відхилення вологосты",Font_11x18,RED,fillWindow,1);}           
        if(noAutoRel){
            pointY += 18;
            sprintf(buff,"R-");
            sprintf(txt,"виходи: %u в ручному режимы",noAutoRel);
            strcat(buff,txt);
            ILI9341_WriteString(5,pointY,buff,Font_11x18,RED,fillWindow,1);
        }
        if(noAutoAna){
            pointY += 18;
            sprintf(buff,"U-");
            sprintf(txt,"виходи: %u в ручному режимы",noAutoAna);
            strcat(buff,txt);
            ILI9341_WriteString(5,pointY,buff,Font_11x18,RED,fillWindow,1);
        }                         
    }
    else {pointY += 25; ILI9341_WriteString(80,pointY,"Помилок немаэ",Font_11x18,bordWindow,fillWindow,1);}
    if(keynum&&!keystop){checkkey(keynum); return;}//***************************** проверим номер кнопки ***************************************      
}

//-------------------------------- СТАН ДАТЧИКІВ и ТАЙМЕРІВ ------------------------------------------
void displ_1(void){
 unsigned char i, num = 1;
 signed int temp;
 unsigned int fillWindow = BLUE1, bordWindow = BLACK;
    pointY = 7;
    if(newSetButt){
        newSetButt=0; 
        ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
        ILI9341_WriteString(100,pointY,"СТАН ДАТЧИКЫВ",Font_11x18,bordWindow,fillWindow,1);
    }
//--- Индикация t ГРУНТА ---
    pointY += 20;
    if(ds18b20){
        for (i=0;i<ds18b20;i++){
            sprintf(buff,"зона %u",i+1);
            ILI9341_WriteString(5,pointY+12,buff,Font_11x18,bordWindow,fillWindow,1);
            temp = t[i];
            if(temp>1250) sprintf(buff,"**.*");
            else {
                fraction(temp);     // проверка знака температуры
                sprintf(buff,"%2u.%u",intval,frcval); // T датчиков показываем с десятичным знаком 
            }
            ILI9341_WriteString(90,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
            pointY += 35;
        };
    }
    else if(soilModule){
        for (i=0;i<soilModule;i++){
            sprintf(buff,"зона %u",i+1);
            ILI9341_WriteString(5,pointY+12,buff,Font_11x18,bordWindow,fillWindow,1);
//            temp = map(t[i], limit[4][2], limit[4][3], limit[4][0], limit[4][1]);// температура
            if(temp>99) sprintf(buff,"**");
            else sprintf(buff,"%2u",temp); // T грунта в целых числах 
            ILI9341_WriteString(90,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
            sprintf(buff,"%3u ",t[i]);
            ILI9341_WriteString(135,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
            
//            temp = 100 - map(hum[i], module[i][0], module[i][1], 0, 100);// влажность
            if(temp>100) sprintf(buff,"^^^%%"); else if(temp<0) sprintf(buff,"---%%");
            else sprintf(buff,"%3u%%",temp); // RH грунта в целых числах
            ILI9341_WriteString(190,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
            sprintf(buff,"%3u",hum[i]);
            ILI9341_WriteString(275,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
            pointY += 35;
        };
        pointY += 20;
        for (i=soilModule;i<4;i++){
            sprintf(buff,"зона %u выдсутня!",i+1);
            ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
            pointY += 35;
        }
    }
//    else ILI9341_WriteString(10,pointY,"Модулы грунту выдсутны!",Font_11x18,bordWindow,fillWindow,1); 
    if(keynum){checkkey(keynum); return;}//***************************** проверим номер кнопки ***************************************
// --- Состояние ТАЙМЕРОВ -------
    pointY += 10;
    ILI9341_WriteString(90,pointY,"СТАН ТАЙМЕРЫВ",Font_11x18,bordWindow,fillWindow,1);
    pointY += 20;
    sprintf(buff,"таймер 1 ");
    strcat(buff,txtTimer);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    pointY += 20;
    sprintf(buff,"таймер 2 ");
    strcat(buff,txt);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    if(keynum){checkkey(keynum); return;}//***************************** проверим номер кнопки **************************************
}

//-------------------------------- СТАН ВЫХОДІВ ------------------------------------------------------
void displ_2(void){
 signed char i, x, keystop=0;
 unsigned int fillWindow = GRAY1, bordWindow = BLACK, color_box; 
    pointY=7;
    if(newSetButt){
        newSetButt=0; keystop=1; 
        ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
        ILI9341_WriteString(90,pointY,"СТАН УПРАВЛЫННЯ",Font_11x18,bordWindow,fillWindow,1);
    }
//---- РЕЛЕЙНЫЕ ВЫХОДЫ ----
    pointY += 20;
    for (i=0;i<4;i++){
        sprintf(buff,"РЕЛЕ  N%u: ",i+1);
        if(relaySet[i]<0) strcat(buff,"АВТ"); else strcat(buff,"РУЧ");
        if(i == numSet){bordWindow = WHITE; fillWindow = BLACK;} else {bordWindow = BLACK; fillWindow = GRAY1;}
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
        if(relOut[i]) color_box=YELLOW; else color_box=BLACK;
        ILI9341_FillRectangle(160,pointY,40,18,color_box);
        pointY = pointY+22;
    }
    if(keynum&&!keystop){checkkey(keynum); return;}//***************************** проверим номер кнопки ***************************************
//---- АНАЛОГОВЫЕ ВЫХОДЫ ----
    for (i=0;i<4;i++){
        sprintf(buff,"ВИХЫД N%u: ",i+1);
        if(analogSet[i]<0) strcat(buff,"АВТ"); else {strcat(buff,"РУЧ"); analogOut[i]=analogSet[i];}
        sprintf(txt," %3u%% ",analogOut[i]);
        strcat(buff,txt);
        if(i+4 == numSet){bordWindow = WHITE; fillWindow = BLACK;} else {bordWindow = BLACK; fillWindow = GRAY1;}
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
        pointY = pointY+22;    
    }
    if(keynum&&!keystop){checkkey(keynum); return;}//***************************** проверим номер кнопки ***************************************
}

//-------------------------------- НАЛАШТУВАННЯ СИСТЕМИ ---------------------------------------------
void displ_3(void){
 char item;
 unsigned int color_txt = BLACK, color_fon = LIGHTYELLOW; 
    pointY=7;
    if (newSetButt){
        ILI9341_FillScreen(0, max_X, 0, max_Y, color_fon);
        ILI9341_WriteString(50,pointY,"НАЛАШТУВАННЯ СИСТЕМИ",Font_11x18,color_txt,color_fon,1);
    }
    pointY += 25;
    if(NewnumMenu || newSetButt){
         NewnumMenu=0;
        for (item = 0; item < MENU; item++){
            sprintf(buff,"%s", setMenu[item]);   //  %10s
            if(item == numMenu){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = LIGHTYELLOW;}
            ILI9341_WriteString(20,pointY,buff,Font_11x18,color_txt,color_fon,1);    
            pointY += 25;
        }
    }
    newSetButt=0; 
    if(keynum) checkkey(keynum);//***************************** проверим номер кнопки ***************************************
}

//-------------------------------- НАЛАШТУВАННЯ окремих пунктiв -------------------------------------
void displ_4(void){
 char item, tmpv0, tmpv1;
 unsigned int color_txt = BLACK, color_fon = GREEN1;
 signed int temp;
    pointY=7;
    if (newSetButt){
        ILI9341_FillScreen(0, max_X, 0, max_Y, color_fon);
        sprintf(buff,"%s",setMenu[numMenu]);
        ILI9341_WriteString(80,pointY,buff,Font_11x18,color_txt,color_fon,1);
    }
    pointY += 25;
    if(NewnumMenu || newSetButt){
        switch (numMenu){
            case 0: //-------- "Температура" ---------
                for (item = 0; item < LIST0;item++){
                    if (item<4){
                        temp = set[0][item]; tmpv0 = temp%10; tmpv1 = temp/10;
                        sprintf(buff,"%7s = %2u.%u", setName0[item],tmpv1,tmpv0); // T с десятичным знаком
                    }
                    else if(item==4) {
                        sprintf(buff,"%7s = ", setName0[item]);
                        if(set[0][item]==1) strcat(buff,"НАГРЫВ"); else  strcat(buff,"ОХОЛОДЖЕННЯ"); 
                    }
                    else sprintf(buff,"%8s = %i", setName0[item],set[0][item]);
                    if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
                    ILI9341_WriteString(15,pointY,buff,Font_11x18,color_txt,color_fon,1);
                    pointY += 25;
                }
            break;
            case 1: //-------- "Влажность" -----------
                for (item = 0; item < LIST0;item++){
                    if(item<4) sprintf(buff,"%8s = %i%%", setName0[item],set[1][item]);
                    else if(item==4) {
                        sprintf(buff,"%8s = ", setName0[item]);
                        switch (set[1][item]){
                            case 2: strcat(buff,"ЫМПУЛЬСНИЙ"); break;
                            case 1: strcat(buff,"ПОСТОЯННИЙ"); break;
                            case 0: strcat(buff,"ОСУШЕННЯ"); break;   
                            default: strcat(buff,"ПОМИЛКА");
                        };
                    }
                    else sprintf(buff,"%8s = %i", setName0[item],set[1][item]);
                    if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
                    ILI9341_WriteString(15,pointY,buff,Font_11x18,color_txt,color_fon,1);
                    pointY += 25;
                }
            break;
            case 2: //-------- "Таймер" --------------
                for (item = 0; item < LIST1;item++){
                    if(item==1||item==3){
                        sprintf(buff,"%8s = ", setName1[item]);
                        if (set[4][item]==0) strcat(buff,"СЕКУНД"); else strcat(buff,"ХВИЛИН");
                    }
                    else sprintf(buff,"%8s = %i", setName1[item],set[4][item]);
                    if(item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
                    ILI9341_WriteString(15,pointY,buff,Font_11x18,color_txt,color_fon,1);
                    pointY += 25;
                }
            break;
            case 3: //-------- "День Ночь" -----------
                for (item = 0; item < LIST2;item++){
                    sprintf(buff,"%11s = %02i:00 год.", setName2[item],rtcTodec(set[5][item]));
                    if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
                    ILI9341_WriteString(5,pointY,buff,Font_11x18,color_txt,color_fon,1);
                    pointY += 25;
                }
            break;
            case 4: //-------- "Время и Дата" ---------------
                for (item = 0; item < LIST4;item++){
                    if (item<2) tmpv0 = clock_buffer[item+1];
                    else tmpv0 = clock_buffer[item+2]; 
                    sprintf(buff,"%8s = %x", setName7[item],tmpv0);
                    if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
                    ILI9341_WriteString(5,pointY,buff,Font_11x18,color_txt,color_fon,1);            
                    pointY += 25;
                }
            break;    
        };
    }
    newSetButt=0;
    if(keynum) checkkey(keynum);//***************************** проверим номер кнопки ***************************************    
}

//-------------------------------- РЕДАГУВАННЯ окремих пунктiв --------------------------------------
void displ_5(void){
 char tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, temp;
    pointY=7;
    if (newSetButt){
        newSetButt=0; 
        ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
        sprintf(buff,"РЕДАГУВАННЯ %s", setMenu[numMenu]);
        ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,1); 
    }
    pointY = pointY+50;
    switch (numMenu){
        case 0: //-------- "Температура" ---------
            if (numSet<4){
                temp = newval[numSet]; tmpv0 = temp%10; tmpv1 = temp/10;
                sprintf(buff,"%5s = %3u.%u", setName0[numSet],tmpv1,tmpv0); // T с десятичным знаком
                ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
            }
            else if(numSet==4) {
                sprintf(buff,"%7s = ", setName0[numSet]);
                if(newval[numSet]==1) strcat(buff,"НАГРЫВ     ");
                else strcat(buff,"ОХОЛОДЖЕННЯ");
                ILI9341_WriteString(15,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
            }
            else {
                sprintf(buff,"%7s = %i", setName0[numSet],newval[numSet]);
                ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
            }      
        break;
        case 1: //-------- "Влажность" -----------
            if(numSet<4){
                sprintf(buff,"%7s = %3i%%", setName0[numSet],newval[numSet]);
                ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
            }
            else if(numSet==4) {
                sprintf(buff,"%8s = ", setName0[numSet]);
                if(newval[numSet]==1) strcat(buff,"ЗВОЛОЖЕННЯ"); else strcat(buff,"ОСУШЕННЯ  ");
                ILI9341_WriteString(15,pointY,buff,Font_11x18,bordWindow,fillWindow,1); 
            }
            else {
                sprintf(buff,"%7s = %i", setName0[numSet],newval[numSet]);
                ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
            }
        break;
        case 2: //-------- "Таймер" --------------
            sprintf(buff,"%5s: %3i ", setName1[numSet], newval[numSet]);
            ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
        break;
        case 3: //-------- "День Ночь" -----------
            sprintf(buff,"%5s: %3i ", setName2[numSet], newval[numSet]);
            ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
        break;
        case 4: //-------- "Время и Дата" ---------------
            sprintf(buff,"%7s: %3u ", setName7[numSet], newval[numSet]);
            ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
        break;
    }
    if(keynum) checkkey(keynum);//***************************** проверим номер кнопки *************************************** 
}

//-------------------------------- ПЕРЕЛIК КОЕФІЦІЄНТIВ ---------------------------------------------
void displ_6(void){
 char item;
 unsigned int color_txt = BLACK, color_fon = GREEN1; 
    pointY=7;
    if(newSetButt){
        ILI9341_FillScreen(0, max_X, 0, max_Y, color_fon);
        ILI9341_WriteString(20,pointY,"НАЛАШТУВАННЯ КОЕФЫЦЫЭНТЫВ",Font_11x18,color_txt,color_fon,1);
    }
    pointY += 35;
    if(NewnumMenu || newSetButt){
        for (item = 0; item < 5; item++){
            sprintf(buff,"Набыр коефыцыэнтыв N%u",item+1);
            if(item == subMenu){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(20,pointY,buff,Font_11x18,color_txt,color_fon,1);    
            pointY += 25;
        }
    }
    newSetButt=0;
    if(keynum) checkkey(keynum);//***************************** проверим номер кнопки ***************************************
}

//-------------------------------- ПЕРЕЛIК ОКРЕМОГО КОЄФФ. чи МОДУЛЯ --------------------------------
void displ_7(void){
 char item, max=6;
 unsigned int color_txt = BLACK, color_fon = GREEN1, temp;
    pointY=7;
    if(moduleEdit) max=4;
    if(newSetButt){
        newSetButt=0; 
        ILI9341_FillScreen(0, max_X, 0, max_Y, color_fon);
        if(moduleEdit) sprintf(buff,"Модуль N%u",subMenu+1);   //- Установки отдельный значений модулей
        else sprintf(buff,"Набыр коефыцыэнтыв N%u",subMenu+1); //- Установки отдельный значений коэффициентов
        ILI9341_WriteString(20,pointY,buff,Font_11x18,color_txt,color_fon,1);
    }
    pointY += 25;
    
    for (item = 0; item < max;item++){
        if(moduleEdit) sprintf(buff,"КОФ.%i= %4i",item,module[subMenu][item]);
        else sprintf(buff,"КОФ.%i= %4i",item,analog[subMenu][item]);
        if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
        ILI9341_WriteString(5,pointY,buff,Font_11x18,color_txt,color_fon,1);
        pointY += 25;
    }
   if(keynum) checkkey(keynum);//***************************** проверим номер кнопки ***************************************   
}

//-------------------------------- РЕДАГУВАННЯ ОКРЕМОГО КОЄФФ. чи МОДУЛЯ ----------------------------
void displ_8(void){
 char tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, temp;
  pointY=7;
  if (newSetButt){
    newSetButt=0; ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
    if(moduleEdit) sprintf(buff,"РЕДАГУВАННЯ модуля N%u",numSet+1); 
    else sprintf(buff,"РЕДАГУВАННЯ коефыцыэнтыв N%u", numSet+1);
    ILI9341_WriteString(10,pointY,buff,Font_11x18,bordWindow,fillWindow,1); 
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    pointY = pointY+50;
    sprintf(buff,"КОФ.%i:%4i",numSet,newval[numSet]);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
    if(keynum) checkkey(keynum);//***************************** проверим номер кнопки *************************************** 
}

//-------------------------------- ПЕРЕЛIК МОДУЛIВ --------------------------------------------------
void displ_9(void){
 char item;
 unsigned int color_txt = BLACK, color_fon = GREEN1; 
    pointY=7;
    if (newSetButt){
        ILI9341_FillScreen(0, max_X, 0, max_Y, color_fon);
        ILI9341_WriteString(20,pointY,"НАЛАШТУВАННЯ МОДУЛЫВ",Font_11x18,color_txt,color_fon,1);
    }
    pointY += 35;
    if(NewnumMenu || newSetButt){;
        for (item = 0; item < 4; item++){
            sprintf(buff,"Модуль N%u",item+1);
            if(item == subMenu){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(20,pointY,buff,Font_11x18,color_txt,color_fon,1);    
            pointY += 25;
        }
    }
    newSetButt=0;
    if(keynum) checkkey(keynum);//***************************** проверим номер кнопки ***************************************
}

void display(void){
 switch (displ_num){
    case 0: displ_0(); break;//- СТАН ПОВІТРЯ. --
    case 1: displ_1(); break;//- СТАН ДАТЧИКІВ и ТАЙМЕРІВ -
    case 2: displ_2(); break;//- СТАН ВЫХОДІВ -
    case 3: displ_3(); break;//- НАЛАШТУВАННЯ СИСТЕМИ -
    case 4: displ_4(); break;//- НАЛАШТУВАННЯ окремих пунктiв -
    case 5: displ_5(); break;//- РЕДАГУВАННЯ окремих пунктiв -
    case 6: displ_6(); break;//- ПЕРЕЛIК КОЕФІЦІЄНТIВ -
    case 7: displ_7(); break;//- ПЕРЕЛIК ОКРЕМОГО КОЄФФ. чи МОДУЛЯ -
    case 8: displ_8(); break;//- РЕДАГУВАННЯ ОКРЕМОГО КОЄФФ. чи МОДУЛЯ -
    case 9: displ_9(); break;//- ПЕРЕЛIК МОДУЛIВ -
    default: displ_0(); break;
  }
}

