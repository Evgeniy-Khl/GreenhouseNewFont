unsigned char txt[20];

void fraction(signed int t){
  if (t<0) {t = -t; signchar = '-';} else signchar = ' ';
  frcval = t%10; intval = t/10;
}

unsigned char checkDisplNum(void){
  unsigned char i;
    if(point_Y>200){
        for (i=0; i<4; i++){if(contains(i)) break;}// �������� ��������� ����� ���������� � ������� ������
        if(i<4){BeepT = 20; displ_num = i; newSetButt = 1; i=10;}
    }
    return i;
}

void checkSkipEdit(void){
  unsigned char i;
    if(point_Y>200){
        for (i=0; i<4; i++){if(contains(i)) break;}// �������� ��������� ����� ���������� � ������� ������
        if(i<4){BeepT = 20; touchpad(i);}
    }
}


//-------------------------------- ���� ��²���. ----------------------------------------
void displ_0(void){
 unsigned char i;
 unsigned int fillWindow = BLUE1, bordWindow = BLACK, temp;
    if(newSetButt){
        newSetButt = 0;
        ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
        initializeButtons(4,1,25);// 4 �������; ���� ������; ������ 25
        //---------- ��� ------- ����a --- ����� - ����� -����� ---------
        drawButton(fillWindow, fillWindow, bordWindow, 0, "������");
        drawButton(WHITE, WHITE, BLACK, 1, "������");
        drawButton(WHITE, WHITE, BLACK, 2, "������");
        drawButton(WHITE, WHITE, BLACK, 3, "������");
    }
//--- ��������� ����� ------
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
    pointY = 3;
    if(Clock_Ok){
        if(Night) ILI9341_WriteString(5,pointY,"��� ",Font_11x18,WHITE,BLACK,1); else ILI9341_WriteString(5,pointY,"����",Font_11x18,BLACK,WHITE,1);
        read_TWI(DS3231,0,clock_buffer,7);// ������ ������ ������� ����������
        sprintf(buff,"%02x:%02x   %02x.%02x.20%02x",clock_buffer[2],clock_buffer[1],clock_buffer[4],clock_buffer[5],clock_buffer[6]);//���:��� ����.���.���
        if(Night){ILI9341_WriteString(70,pointY,buff,Font_11x18,WHITE,GRAY1,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,WHITE,BLACK,1);}// "������� ������"
        else {ILI9341_WriteString(70,pointY,buff,Font_11x18,BLACK,GRAY1,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,BLACK,WHITE,1);}
    }
    else {
        sprintf(buff,"������� �����!"); 
        ILI9341_WriteString(70,pointY,buff,Font_11x18,YELLOW,RED,1); ILI9341_WriteString(70,pointY,buff,Font_11x18,RED,YELLOW,1);// "������� ������"
    }
    pointY += 22;
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
//--- ��������� t ������� -----
    temp = set[0][Night]; fraction(temp);     // �������� ����� �����������
    sprintf(buff,"[%2u.%u]",intval,frcval);   // ������� T ���������� � ���������� ������
    ILI9341_WriteString(5,pointY+5,buff,Font_11x18,bordWindow,fillWindow,1);
    ILI9341_WriteString(5,pointY+25,"�����������",Font_11x18,bordWindow,fillWindow,1);
    if (pvT>1250) sprintf(buff,"**.*");
    else {
        temp = pvT; fraction(temp);     // �������� ����� �����������
        sprintf(buff,"%2u.%u",intval,frcval); // T �������� ���������� � ���������� ������ 
    }
    ILI9341_WriteString(140,pointY,buff,Font_11x18,bordWindow,fillWindow,3);
    // ��������� ������� alarm[0]
    switch (alarm[0]) {
        case 0: temp=GREEN; break;
        case 1: temp=RED;   break;
        default: temp=fillWindow;
    }; 
    ILI9341_FillRectangle(280,pointY+2,30,45,temp);
    pointY += 55;
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
//--- ��������� RH ------
    sprintf(buff,"[%3u%%]",set[1][Night]); // ������� RH
    ILI9341_WriteString(5,pointY+5,buff,Font_11x18,bordWindow,fillWindow,1);
    ILI9341_WriteString(5,pointY+25,"���������",Font_11x18,bordWindow,fillWindow,1);
    if(pvRH>100) sprintf(buff,"***%%",pvRH); else sprintf(buff,"%3u%%",pvRH);
    ILI9341_WriteString(142,pointY,buff,Font_11x18,bordWindow,fillWindow,3); 
    // ��������� ������� alarm[1]
    switch (alarm[1]) {
        case 0: temp=GREEN; break;
        case 1: temp=RED;   break;
        default: temp=fillWindow;
    };
    ILI9341_FillRectangle(280,pointY+2,30,45,temp);
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
// --- ��������� �������� -------
    pointY += 60;
    ILI9341_WriteString(90,pointY,"���� ��������",Font_11x18,bordWindow,fillWindow,1);
    pointY += 20;
    sprintf(buff,"������ 1 ");
    strcat(buff,txtTimer);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    pointY += 20;
    sprintf(buff,"������ 2 ");
    strcat(buff,txt);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);      
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
}

//-------------------------------- ���� ������. -------------------------------------------
void displ_1(void){
 unsigned char i, num = 1;
 unsigned int fillWindow = YELLOW2, bordWindow = BLACK, temp;
    pointY = 7;
    if(newSetButt){
        newSetButt = 0;
        ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
        initializeButtons(4,1,25);// 4 �������; ���� ������; ������ 25
        //---------- ��� ------- ����a --- ����� - ����� -����� ---------
        drawButton(WHITE, WHITE, BLACK, 0, "������");
        drawButton(fillWindow, fillWindow, bordWindow, 1, "������");
        drawButton(WHITE, WHITE, BLACK, 2, "������");
        drawButton(WHITE, WHITE, BLACK, 3, "������");
        ILI9341_WriteString(100,pointY,"���� ������",Font_11x18,bordWindow,fillWindow,1);
    }
//--- ��������� t ������ ---
    pointY += 20;
    for (i=0;i<ds18b20;i++){
        sprintf(buff,"���� %u",i+1);
        ILI9341_WriteString(5,pointY+12,buff,Font_11x18,bordWindow,fillWindow,1);
        temp = t.point[i];
        if(temp>1250) sprintf(buff,"**.*");
        else {
            fraction(temp);     // �������� ����� �����������
            sprintf(buff,"%2u.%u  ---%%",intval,frcval); // T �������� ���������� � ���������� ������ 
        }
        ILI9341_WriteString(90,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
        pointY += 35;
    };
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
    pointY += 10;
//--- ������ ������ ---
    if(Soilmodule){
        if(error&0x08) ILI9341_WriteString(10,pointY,"������� ������ ������!",Font_11x18,YELLOW,RED,1);
        else {
            sprintf(buff,"�����   ---%%");
            ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
        }
    }
    else ILI9341_WriteString(10,pointY,"������ ������ ���������.",Font_11x18,bordWindow,fillWindow,1);
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
    pointY += 30;
//--- ������ CO2 ----
//CO2module = 1;
//pHsensor = 1;
//pvCO2 = 2500;
//pvPH = 75;
    if(CO2module){
        if(error&0x08) ILI9341_WriteString(10,pointY,"CO2 ������� ������!",Font_11x18,YELLOW,RED,1);
        else {
            sprintf(buff,"CO2   %4u",pvCO2);
            ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
            ILI9341_WriteString(250,pointY+10,"ppm",Font_11x18,bordWindow,fillWindow,1);
        }
    }
    else ILI9341_WriteString(10,pointY,"CO2 ������ ���������.",Font_11x18,bordWindow,fillWindow,1);
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
//--- ������ �H-4502 -----
    pointY += 30;
    if(pHsensor){
        if(error&0x08) ILI9341_WriteString(10,pointY,"�� ������� ������!",Font_11x18,YELLOW,RED,1);
        else {
          temp = pvPH; fraction(temp);
          sprintf(buff,"��   %2u.%u",intval,frcval);
          ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
          ILI9341_WriteString(230,pointY+10,"�������",Font_11x18,bordWindow,fillWindow,1);
        }
    }
    else ILI9341_WriteString(10,pointY,"��  ������ ���������.",Font_11x18,bordWindow,fillWindow,1);
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
}

//------------------------------ �������� ������ --------------------------------------------
void displ_2(void){
 signed char i,x;
 unsigned int fillWindow = GRAY1, bordWindow = BLACK, color_box; 
    pointY=7;
    if(newSetButt){
        newSetButt = 0;
        ILI9341_FillScreen(0, max_X, 0, max_Y, fillWindow);
        initializeButtons(4,1,25);// 4 �������; ���� ������; ������ 25
        //---------- ��� ------- ����a --- ����� - ����� -����� ---------
        drawButton(WHITE, WHITE, BLACK, 0, "������");
        drawButton(WHITE, WHITE, BLACK, 1, "������");
        drawButton(fillWindow, fillWindow, bordWindow, 2, "������");
        drawButton(WHITE, WHITE, BLACK, 3, "������");
        //--
        initializePlus(pointY+25, 8, 16);
        for (i=0;i<plusCount;i++) drawPlus(i, fillWindow);
        //--
        ILI9341_WriteString(90,pointY,"���� ����������",Font_11x18,bordWindow,fillWindow,1);
    }
//=========================== �������� ������ ���������� ���� ==================================
    if(checkTouch()){
    //sprintf(buff,"X%4u; Y%4u",point_X,point_Y);
    //ILI9341_WriteString(5,TFTBUTTON-60,buff,Font_11x18,WHITE,BLACK,1);
        if(point_X>220&&point_Y<100){
            for (i=0; i<8; i++){if(containsPlus(i)) break;}// �������� ������� �������� ������
            if(i<8){
                BeepT = 20; 
                if(i%2) x=-1; else x=1;   // ������ "+" ��� ������ "-"
                i /= 2;
                relaySet[i]+=x; if(relaySet[i]>2) relaySet[i]=0; else if(relaySet[i]<0) relaySet[i]=2;
            }
        }
        else checkDisplNum();
    }
//---- �������� ������ ----
    pointY += 20;
    for (i=0;i<4;i++){
        sprintf(buff,"����  N%u: ",i+1);
        if(relaySet[i]==2) strcat(buff,"���"); else strcat(buff,"���");
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
        if(relOut[i]) color_box=YELLOW; else color_box=BLACK;
        ILI9341_FillRectangle(160,pointY,40,18,color_box);
        pointY = pointY+22;
    }
    if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
//========================= �������� ������ ���������� 0-10v ===================================
    if(checkTouch()){
//sprintf(buff,"X%4u; Y%4u",point_X,point_Y);
//ILI9341_WriteString(5,TFTBUTTON-60,buff,Font_11x18,WHITE,BLACK,1);
        if(point_X>220&&point_Y>100&&point_Y<200){
            for (i=8; i<16; i++){if(containsPlus(i)) break;}// �������� ������ �������� ������
            sprintf(buff,"i=%2u",i);
            ILI9341_WriteString(5,TFTBUTTON-30,buff,Font_11x18,WHITE,BLACK,1);
            if(i<16){
                BeepT = 20; 
                if(i%2) x=-1; else x=1;   // ������ "+" ��� ������ "-"
                i /= 2; i -= 4; analogSet[i]+=x*10; // ��� ��������� 10 ��.
                if(analogSet[i]>100) analogSet[i]=100; else if(analogSet[i]==9) analogSet[i]=10; else if(analogSet[i]<-1) analogSet[i]=-1;
//sprintf(buff,"i=%2u; x=%2i",i,x);
//ILI9341_WriteString(5,TFTBUTTON-45,buff,Font_11x18,WHITE,BLACK,1);                 
            }
        }
        else checkDisplNum();
    }
//---- ���������� ������ ----
    for (i=0;i<4;i++){
        sprintf(buff,"����� N%u: ",i+1);
        if(analogSet[i]==-1) strcat(buff,"���"); else {strcat(buff,"���"); analogOut[i]=analogSet[i];}
        sprintf(txt," %3u%% ",analogOut[i]);
        strcat(buff,txt);
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
        pointY = pointY+22;    
    }
   if(checkTouch()) if(checkDisplNum()==10) return;//***************************** �������� ������� ������ ***************************************
}

void drawButtEdit(unsigned int fW){
    newSetButt = 0;
    ILI9341_FillScreen(0, max_X, 0, max_Y, fW);
    initializeButtons(4,1,25);// 4 �������; ���� ������; ������ 25
    //---------- ��� ------- ����a --- ����� - ����� -����� ---------
    drawButton(BLUE, WHITE, WHITE, 0, "����.");
    drawButton(GREEN, BLACK, BLACK, 1, "+");
    drawButton(GREEN, BLACK, BLACK, 2, "-");
    drawButton(MAGENTA, BLACK, BLACK, 3, "����.");
}

void drawButtSkip(unsigned int fW){
    newSetButt = 0;
    ILI9341_FillScreen(0, max_X, 0, max_Y, fW);
    initializeButtons(4,1,25);// 4 �������; ���� ������; ������ 25
    //---------- ��� ------- ����a --- ����� - ����� -����� ---------
    drawButton(BLUE, WHITE, WHITE, 0, "�����");
    drawButton(GREEN, BLACK, BLACK, 1, "<-");
    drawButton(GREEN, BLACK, BLACK, 2, "->");
    drawButton(MAGENTA, BLACK, BLACK, 3, "���.");
}

//--- ��������� ����� ������ ----
void displ_3(void){
 char item;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, color_txt = BLACK, color_fon = GREEN1; 
  pointY=7;
  if (newSetButt){
    drawButtSkip(fillWindow);
    ILI9341_WriteString(50,pointY,"������������ �������",Font_11x18,bordWindow,fillWindow,1);
  }
  pointY += 25;
  for (item = 0; item < MAX_MENU; item++){
    sprintf(buff,"%s", setMenu[item]);   //  %10s
    if(item == numMenu){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
    ILI9341_WriteString(20,pointY,buff,Font_11x18,color_txt,color_fon,1);    
    pointY += 25;
  }
  if(checkTouch()) checkSkipEdit();//***************************** �������� ������� ������ ***************************************
}

//- ��������� ������� -
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
    case 0: //-------- "�����������" ---------
        for (item = 0; item < MAX_5;item++){
            if (item<4){
                temp = set[0][item]; tmpv0 = temp%10; tmpv1 = temp/10;
                sprintf(buff,"%7s = %2u.%u", setName0[item],tmpv1,tmpv0); // T � ���������� ������
            }
            else {
                sprintf(buff,"%7s = ", setName0[item]);
                if(set[0][item]==1) strcat(buff,"������");
                else  strcat(buff,"�����������"); 
            }
            if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(15,pointY,buff,Font_11x18,color_txt,color_fon,1);
            pointY += 25;
        }
    break;
    case 1: //-------- "���������" -----------
        for (item = 0; item < MAX_5;item++){
            if(item<4) sprintf(buff,"%8s = %i%%", setName0[item],set[1][item]);
            else {
                sprintf(buff,"%8s = ", setName0[item]);
                if(set[1][item]==1) strcat(buff,"����������");
                else  strcat(buff,"��������"); 
            }
            if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(15,pointY,buff,Font_11x18,color_txt,color_fon,1);
            pointY += 25;
        }
    break;
    case 2: //-------- "������" --------------
        for (item = 0; item < MAX_6;item++){
            if(item==1||item==3){
                sprintf(buff,"%8s = ", setName1[item]);
                if (set[4][item]==0) strcat(buff,"������");
                else strcat(buff,"������");
            }
            else sprintf(buff,"%8s = %i", setName1[item],set[4][item]);
            if(item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(15,pointY,buff,Font_11x18,color_txt,color_fon,1);
            pointY += 25;
        }
    break;
    case 3: //-------- "���� ����" -----------
        for (item = 0; item < MAX_6;item++){
            sprintf(buff,"%11s = %02i:00 ���.", setName2[item],rtcTodec(set[5][item]));
            if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
            ILI9341_WriteString(5,pointY,buff,Font_11x18,color_txt,color_fon,1);
            pointY += 25;
        }
    break;
    case 4: //-------- "����� � ����" ---------------
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
  if(checkTouch()) checkSkipEdit();//***************************** �������� ������� ������ ***************************************   
}

//- �������������� -
void displ_5(void){
 char tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, temp;
  pointY=7;
  if (newSetButt){
    drawButtEdit(fillWindow);
    pauseEdit = 3;
    sprintf(buff,"����������� %s", setMenu[numMenu]);
    ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,1); 
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  pointY = pointY+50;
   switch (numMenu){
    case 0: //-------- "�����������" ---------
      if (numSet<4){
        temp = newval[numSet]; tmpv0 = temp%10; tmpv1 = temp/10;
        sprintf(buff,"%5s = %2u.%u", setName0[numSet],tmpv1,tmpv0); // T � ���������� ������
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
      }
      else {
        sprintf(buff,"%7s = ", setName0[numSet]);
        if(newval[numSet]==1) strcat(buff,"������     ");
        else strcat(buff,"�����������");
        ILI9341_WriteString(15,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
      }      
    break;
    case 1: //-------- "���������" -----------
      if(numSet<4){
        sprintf(buff,"%7s = %i%%", setName0[numSet],newval[numSet]);
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
      }
      else {
        sprintf(buff,"%8s = ", setName0[numSet]);
        if(newval[numSet]==1) strcat(buff,"����������");
        else  strcat(buff,"��������  ");
        ILI9341_WriteString(15,pointY,buff,Font_11x18,bordWindow,fillWindow,1); 
      }
      
    break;
    case 2: //-------- "������" --------------
        sprintf(buff,"%5s: %i ", setName1[numSet], newval[numSet]);
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
    break;
    case 3: //-------- "���� ����" -----------
        sprintf(buff,"%5s: %i ", setName2[numSet], newval[numSet]);
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
    break;
    case 4: //-------- "����� � ����" ---------------
        sprintf(buff,"%7s: %u ", setName7[numSet], newval[numSet]);
        ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
    break;
   }
   if(--pauseEdit==0){pauseEdit=1; if(checkTouch()) checkSkipEdit();}//***************************** �������� ������� ������ *************************************** 
}

//--- ��������� ����� ������������� ----
void displ_6(void){
 char item;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, color_txt = BLACK, color_fon = GREEN1; 
  pointY=7;
  if (newSetButt){
    drawButtSkip(fillWindow);
    ILI9341_WriteString(20,pointY,"������������ ������������",Font_11x18,bordWindow,fillWindow,1);
  }
  pointY += 35;
  for (item = 0; item < 4; item++){
    sprintf(buff,"����� ������������ N%u",item+1);
    if(item == numMenu){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
    ILI9341_WriteString(20,pointY,buff,Font_11x18,color_txt,color_fon,1);    
    pointY += 25;
  }
  if(checkTouch()) checkSkipEdit();//***************************** �������� ������� ������ ***************************************
}

//- ��������� ������� -
void displ_7(void){
 char item, tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, color_txt = BLACK, color_fon = GREEN1, temp;
    pointY=7;
    if (newSetButt){
    drawButtSkip(fillWindow);
//    sprintf(buff,"%s",setName3[numMenu]);
    sprintf(buff,"����� ������������ N%u",numMenu+1);
    ILI9341_WriteString(20,pointY,buff,Font_11x18,bordWindow,fillWindow,1);
    }
    pointY += 25;
    for (item = 0; item < MAX_4;item++){
        sprintf(buff,"%7s = %i", setName3[item],limit[numMenu][item]);
        if (item == numSet){color_txt = WHITE; color_fon = BLACK;} else {color_txt = BLACK; color_fon = GREEN1;}
        ILI9341_WriteString(5,pointY,buff,Font_11x18,color_txt,color_fon,1);
        pointY += 25;
    }
   if(checkTouch()) checkSkipEdit();//***************************** �������� ������� ������ ***************************************   
}

//- �������������� -
void displ_8(void){
 char tmpv0, tmpv1;
 unsigned int fillWindow = GREEN1, bordWindow = BLACK, temp;
  pointY=7;
  if (newSetButt){
    drawButtEdit(fillWindow);
    pauseEdit = 3;
    sprintf(buff,"����������� ������������ N%u", numMenu+1);
    ILI9341_WriteString(10,pointY,buff,Font_11x18,bordWindow,fillWindow,1); 
  }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    pointY = pointY+50;
    sprintf(buff,"%7s:%3i", setName3[numSet], newval[numSet]);
    ILI9341_WriteString(5,pointY,buff,Font_11x18,bordWindow,fillWindow,2);
    if(--pauseEdit==0){pauseEdit=1; if(checkTouch()) checkSkipEdit();}//***************************** �������� ������� ������ *************************************** 
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

