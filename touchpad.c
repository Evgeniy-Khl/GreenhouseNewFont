void touchpad(char byte){
  char max = MAX_MENU-1;
     switch (displ_num){
        //- ����� ������ ��������� -
        case 3:
          switch (byte){
            case 0: displ_num = 0; newSetButt = 1; break;
            case 1: if (--numMenu<0) numMenu = 0;  break;
            case 2: if (++numMenu>MAX_MENU-1) numMenu = MAX_MENU-1; break;
            case 3: displ_num = 4; newSetButt = 1; break;
          }
          byte = 10;
        break;
        //- ��������� ������� -
        case 4:
          if (numMenu==4) max++;
          switch (byte){
            case 0: displ_num = 3; newSetButt = 1;  break;
            case 1: if (--numSet<0) numSet = 0;     break;
            case 2: if (++numSet>max) numSet = max; break;
            case 3:  
              switch (numMenu){
                case 0: for (byte=0;byte<MAX_5;byte++) newval[byte] = set[0][byte]; break; // "�����������"
                case 1: for (byte=0;byte<MAX_5;byte++) newval[byte] = set[1][byte]; break; // "���������"
                case 2: for (byte=0;byte<MAX_5;byte++) newval[byte] = set[4][byte]; break; // "������" 
                case 3: for (byte=0;byte<MAX_6;byte++) newval[byte] = rtcTodec(set[5][byte]); break; // "���� ����"
                case 4: for (byte=0;byte<MAX_6;byte++) newval[byte] = set[byte][6]; break; // "������"
                case 5: for (byte=0;byte<2;byte++) newval[byte] = rtcTodec(clock_buffer[byte+1]); 
                        for (byte=2;byte<5;byte++) newval[byte] = rtcTodec(clock_buffer[byte+2]);
                    break;     // "����� � ����"
              };
              displ_num = 5; newSetButt = 1; 
            break;
          }
          byte = 10;
        break;
        //- �������������� -
        case 5:
          switch (byte){
               case 0: displ_num = 4; newSetButt = 1; break;
            case 1: ++newval[numSet];    
                 switch (numMenu) {
                    case 0:  // ����������� 
                        switch (numSet) {
                            case 0: if(newval[numSet]>1200) newval[numSet]=1200; break; // ����
                            case 1: if(newval[numSet]>1200) newval[numSet]=1200; break; // ����
                            case 2: if(newval[numSet]>500) newval[numSet]=500; break;   // ����������
                            case 3: if(newval[numSet]>100) newval[numSet]=100; break;   // ����������
                            case 4: if(newval[numSet]>1)  newval[numSet]=1;    break;   // �����
                        }; 
                    break;
                    case 1:  // ��������� 
                        switch (numSet) {
                            case 0: if(newval[numSet]>100) newval[numSet]=100; break; // ����
                            case 1: if(newval[numSet]>100) newval[numSet]=100; break; // ����
                            case 2: if(newval[numSet]>90) newval[numSet]=90; break;   // ����������
                            case 3: if(newval[numSet]>50) newval[numSet]=50; break;   // ����������
                            case 4: if(newval[numSet]>1)  newval[numSet]=1;  break;   // �����
                        };
                    break;
                    case 2:  // ������ 
                        switch (numSet) {
                            case 0: if(newval[numSet]>1200) newval[numSet]=1200; break;  // �������
                            case 1: if(newval[numSet]>1) newval[numSet]=1; break;        // �����������
                            case 2: if(newval[numSet]>1200) newval[numSet]=1200; break;  // ��������
                            case 3: if(newval[numSet]>1) newval[numSet]=1; break;        // �����������
                            case 4: if(newval[numSet]>14) newval[numSet]=14; break;      // ���
                        };
                    break;
                    case 3:  // ���� ���� 
                        switch (numSet) {
                            case 0: if(newval[numSet]>12) newval[numSet]=12; break;   // DayBeg
                            case 1: if(newval[numSet]>23) newval[numSet]=23; break;   // DayEnd
                            case 2: if(newval[numSet]>12) newval[numSet]=12; break;   // Light0Beg
                            case 3: if(newval[numSet]>12) newval[numSet]=12; break;   // Light0End
                            case 4: if(newval[numSet]>23) newval[numSet]=23; break;   // Light1Beg
                            case 5: if(newval[numSet]>23) newval[numSet]=23; break;   // Light1End
                        };
                    break;
                    case 4:  // ������ 
                        switch (numSet) {
                            case 0: newval[numSet]=0; break;   // ����� ������ / ���������� (0/4)
                            case 1: newval[numSet]=1; break;   // ����� ����� / ��������    (1/5)
                            case 2: newval[numSet]=6; break;   // ����� ����� 1             (6)
                            case 3: newval[numSet]=7; break;   // ����� ����� 2             (7)
                            case 4: newval[numSet]=2; break;   // ����� �������             (2)
                            case 5: newval[numSet]=3; break;   // ����� ���������           (3)
                        };
                    break;
                    case 5:  // ����� � ���� 
                        switch (numSet) {
                            case 0: if(newval[numSet]>59) newval[numSet]=0;  break;   // ������
                            case 1: if(newval[numSet]>23) newval[numSet]=0;  break;   // ����
                            case 2: if(newval[numSet]>31) newval[numSet]=1;  break;   // ����
                            case 3: if(newval[numSet]>12) newval[numSet]=1;  break;   // �����
                            case 4: if(newval[numSet]>59) newval[numSet]=22; break;   // ���
                        };
                    break;
                  }; 
            break;
            case 2: --newval[numSet];
                 switch (numMenu) {
                    case 0:  // ����������� 
                        switch (numSet) {
                            case 0: if(newval[numSet]<-500) newval[numSet]=-500; break;   // ����
                            case 1: if(newval[numSet]<-500) newval[numSet]=-500; break;   // ����
                            case 2: if(newval[numSet]<1) newval[numSet]=1; break;   // ����������
                            case 3: if(newval[numSet]<1) newval[numSet]=1; break;   // ����������
                            case 4: if(newval[numSet]<0) newval[numSet]=0; break;   // �����                            
                        }; 
                    break;
                    case 1:  // ��������� 
                        switch (numSet) {
                            case 0: if(newval[numSet]<10) newval[numSet]=10; break;   // ����
                            case 1: if(newval[numSet]<10) newval[numSet]=10; break;   // ����
                            case 2: if(newval[numSet]<1) newval[numSet]=1; break;   // ����������
                            case 3: if(newval[numSet]<1) newval[numSet]=1; break;   // ����������
                            case 4: if(newval[numSet]<0) newval[numSet]=0; break;   // �����     
                        };
                    break;
                    case 2:  // ������ 
                        switch (numSet) {
                            case 0: if(newval[numSet]<1) newval[numSet]=1; break;   // �������
                            case 1: if(newval[numSet]<0) newval[numSet]=1; break;   // �����������
                            case 2: if(newval[numSet]<1) newval[numSet]=1; break;   // ��������
                            case 3: if(newval[numSet]<0) newval[numSet]=1; break;   // �����������
                            case 4: if(newval[numSet]<0) newval[numSet]=0; break;   // ���
                        };
                    break;
                    case 3:  // ���� ���� 
                        switch (numSet) {
                            case 0: if(newval[numSet]<0) newval[numSet]=12; break;   // DayBeg
                            case 1: if(newval[numSet]<0) newval[numSet]=23; break;   // DayEnd
                            case 2: if(newval[numSet]<0) newval[numSet]=12; break;   // Light0Beg
                            case 3: if(newval[numSet]<0) newval[numSet]=12; break;   // Light0End
                            case 4: if(newval[numSet]<0) newval[numSet]=23; break;   // Light1Beg
                            case 5: if(newval[numSet]<0) newval[numSet]=23; break;   // Light1End
                        };
                    break;
                    case 4:  // ������ 
                        switch (numSet) {
                            case 0: newval[numSet]=0; break;   // ����� ������ / ���������� (0/4)
                            case 1: newval[numSet]=1; break;   // ����� ����� / ��������    (1/5)
                            case 2: newval[numSet]=6; break;   // ����� ����� 1             (6)
                            case 3: newval[numSet]=7; break;   // ����� ����� 2             (7)
                            case 4: newval[numSet]=2; break;   // ����� �������             (2)
                            case 5: newval[numSet]=3; break;   // ����� ���������           (3)
                        };
                    break;
                    case 5:  // ����� � ���� 
                        switch (numSet) {
                            case 0: if(newval[numSet]<0)  newval[numSet]=59; break;   // ������
                            case 1: if(newval[numSet]<0)  newval[numSet]=23; break;   // ����
                            case 2: if(newval[numSet]<1)  newval[numSet]=31; break;   // ����
                            case 3: if(newval[numSet]<1)  newval[numSet]=12; break;   // �����
                            case 4: if(newval[numSet]<22) newval[numSet]=59; break;   // ���
                        };
                    break;
                  };
            break;
            case 3: ILI9341_FillScreen(0, max_X, 0, max_Y, fillScreen);
                  ILI9341_WriteString(5,100,"������� �����",Font_11x18,GREEN,fillScreen,2);
                  switch (numMenu){
                    case 0: set[0][numSet] = newval[numSet]; break; // "�����������"
                    case 1: set[1][numSet] = newval[numSet]; break; // "���������"
                    case 2: set[4][numSet] = newval[numSet]; break; // "������"
                    case 3: set[5][numSet] = ByteToBcd2(newval[numSet]); break; // "���� ����"
                    case 4: set[numSet][6] = newval[numSet]; break; // "������"
                    case 5: for (byte=0;byte<2;byte++) clock_buffer[byte+1] = ByteToBcd2(newval[byte]);
                            for (byte=2;byte<5;byte++) clock_buffer[byte+2] = ByteToBcd2(newval[byte]);
                            clock_buffer[0]=0;  
                            Clock_Ok=write_TWI(DS3231,0,clock_buffer,7);  break; // "����� � ����"
                  }
                  delay_ms(500);
                  displ_num = 4; newSetButt = 1; break;
          }
          byte = 10;
        break;
    }
}

