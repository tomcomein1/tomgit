#include"dlpublic.h"
#include"tools.h"
#include"tasktool.h"
#include"dl_mobile.h"
#include"newtask.h"
#include"dlprint.h"

extern char sTcpipErrInfo[81];
extern long nSysLkrq;
extern char sSysCzydh[7];

void MobileSignIn(long nType,char * sItem,char * sJym)
{
     long nRetVal;
     char sInfo[101];
     long nLkrq=0;
     
     clrscr();
     outtext("�������ƶ�ǩ��,��ȴ�......\n");
     
     nRetVal=AutoRunTask("dldj",TASK_MOBILE_SIGN_IN,"%s","%s",sJym,sInfo);
    
     if(nRetVal)
     {
        outtext(sTcpipErrInfo);
        keyb();
        return;
     }             
     if(!strncmp("000000",sInfo,6)) strcpy(sInfo,"ǩ���ɹ�������������ء�\n");
     outtext(sInfo);
     keyb();

}

//���ƶ�ǩ��
void MobileSignOut(long nType,char * sItem,char * sJym)
{
     char sResult[256];           //ǩ�����ؽ��
     long nRetVal;
     char sBankCode[9];
     TABLE * pTab; 
  
     pTab=CreateTable(7,
          "|���ƶ�ǩ��...                                           |");
     if(!pTab) return;

     FillTable(pTab,2,"%s","                 -----  ��  ��  ------\n    һ��ǩ�˺�,���վͲ��ܿ�չ�����Ѳ�ѯ������κ�ҵ��!\n");
     FillTable(pTab,4,"%s",
       "    �����ȷ����Ҫǩ��,�밴'1'��,������������.ǩ�˳ɹ���ϵͳ���Զ����ƶ������ж���.\n");
     FillTable(pTab,7,"%s","   ����������ѡ��....");
     clrscr();
     ShowTable(pTab,10,6);
     if(keyb()!='1')
     {
        DropTable(pTab);
        return;
     } 
     FillTable(pTab,7,"%-50.50s","�������ƶ�ǩ��,��ȴ�...");
     fflush(stdout);

     nRetVal=AutoRunTask("dldj",TASK_MOBILE_SIGN_OUT,"%s","%s",sJym,sResult);
     
     if(nRetVal)
     {
        FillTable(pTab,7,"%-50.50s",sTcpipErrInfo);
        DropTable(pTab);
        keyb();
        return;
     }             
     if(!strncmp("000000",sResult,6))
     {
        //ǩ�˳ɹ����������ƶ�������
        DropTable(pTab);
        MobileCheckAccount(0,NULL,sJym);
     }        
     else
     {
     	FillTable(pTab,7,"%-50.50s",sResult);
     	DropTable(pTab);
        keyb();
     }

}

void MobileCheckAccount(long ntype,char * sitem,char * sJym)  
{
      char sAnswer[256];
      long nRetVal;
            
      memset(sAnswer,0,sizeof(sAnswer));

      clrscr();

      
      outtext("\n���ں��ƶ�����˶�����,�����Ҫ���ϳ�ʱ��,�����ĵȴ�.. \n");
  
      nRetVal=AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sAnswer);
      if(nRetVal)
      {
          outtext(sTcpipErrInfo);
          keyb();
          return;
      }
      if(!strncmp(sAnswer,"000000",6)) strcpy(sAnswer,"���ʳɹ��������������...");
      outtext(sAnswer);
      keyb();
}


void MobileCompletePrint(long nType,char * sItem,char * sJym)
{
     PRINTFORM * ppf;
     char sResult[256],sLsh[41];           //ǩ�����ؽ��
     long nRetVal;
     char sBankCode[9];
     char sBeginTime[15];
     char sEndTime[15];
     char sPhone[12];
     long nMonth=0,nYear=0,nKey,nprint;
     
     
     memset(sPhone,'\0',sizeof(sPhone));              
     memset(sResult,'\0',sizeof(sResult));              

     nYear=nSysLkrq/10000;
     nMonth=(nSysLkrq%10000)/100;

     clrscr();
     gotoxy(1,1);
     outtext("                               ��ȫ��������ӡ                    \n");
     outtext("             ----------------------------------------------------\n");
          
     for(;;)
     {
    	gotoxy(10,23);
     	outtext("        ��������Ӧ��Ϣ����ESC������...                           ");

     	for(;;)
     	{
        	nKey=GetValue(24,10,"�������ֻ�����: %11s",sPhone);
         	if(nKey==KEY_ESC) return;
         	if(!IsValidPhoneNo(sPhone)) continue;
         	nKey=GetValue(24,12,"�������ӡ���: %4d",&nYear);
         	if(nKey==KEY_ESC) return;
         	nKey=GetValue(24,14,"�������ӡ�·�: %2d",&nMonth);
         	if(nKey==KEY_ESC) return;
         	if(nMonth<1 || nMonth>12) continue;
         	break;
	  	}
        
     	sprintf(sBeginTime,"%04ld%02ld20000000",
             nYear-(nMonth<=1),(((nMonth-1)<1)?12:nMonth-1));
     	sprintf(sEndTime,"%04ld%02ld21000000",nYear,nMonth);
     
     	gotoxy(10,23); 
     	outtext("            ���ڲ�ѯ��ȫ���������ݣ���ȴ�...                   ");
        
     	nRetVal=AutoRunTask("dldj",TASK_MOBILE_COMPLETE_PRINT,"%s%s%s%s","%s",
     	                    sJym,sPhone,sBeginTime,sEndTime,sResult);
                     
     	if(nRetVal<0)
     	{
         	gotoxy(10,23);
         	outtext(sTcpipErrInfo);
         	keyb();
         	continue;
     	}
     
     	if(strncmp(sResult,"000000",6))
     	{
         	gotoxy(10,23);
         	outtext("%50.50s",sResult);
         	keyb();
     		continue;
     	}
     	strncpy(sLsh,sResult+7,40);
        sLsh[40]='\0';
     	if(sLsh[0]=='.')
     	{
     		
         	gotoxy(10,23);
         	outtext("          ���û�û�и��µ�Ԥ������Ʊ...          ");
         	keyb();
         	clearline(23);
     		continue;
     	}
		
		PrintYdfp(sLsh); //�ƶ��¸�ʽ���ƶ�����
	}
}

long IsValidPhoneNo(char * sPhone)
{
     char sValid[6];
     int nRec;

     strcpy(sValid,"56789");
     
     alltrim(sPhone);
     
     for(nRec=0;nRec<strlen(sPhone);nRec++) 
          if(sPhone[nRec]>'9' || sPhone[nRec]<'0') return 0;
     if(strlen(sPhone)==11)
     {
          if(sPhone[0]=='0' && sPhone[1]=='8') return 1;
          if(sPhone[0]=='1' && sPhone[1]=='3' && strchr(sValid,sPhone[2])) return 1;
          return 0;
     }
     if(strlen(sPhone)==7) 
     {
          if(sPhone[0]=='9') return 1;
          else return 0;
     }
        
     return 0;

}
