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
     outtext("正在向移动签到,请等待......\n");
     
     nRetVal=AutoRunTask("dldj",TASK_MOBILE_SIGN_IN,"%s","%s",sJym,sInfo);
    
     if(nRetVal)
     {
        outtext(sTcpipErrInfo);
        keyb();
        return;
     }             
     if(!strncmp("000000",sInfo,6)) strcpy(sInfo,"签到成功，按任意键返回。\n");
     outtext(sInfo);
     keyb();

}

//向移动签退
void MobileSignOut(long nType,char * sItem,char * sJym)
{
     char sResult[256];           //签到返回结果
     long nRetVal;
     char sBankCode[9];
     TABLE * pTab; 
  
     pTab=CreateTable(7,
          "|向移动签退...                                           |");
     if(!pTab) return;

     FillTable(pTab,2,"%s","                 -----  警  告  ------\n    一旦签退后,今日就不能开展除交费查询以外的任何业务!\n");
     FillTable(pTab,4,"%s",
       "    如果您确认需要签退,请按'1'键,或按其它键返回.签退成功后系统将自动与移动方进行对帐.\n");
     FillTable(pTab,7,"%s","   请输入您的选择....");
     clrscr();
     ShowTable(pTab,10,6);
     if(keyb()!='1')
     {
        DropTable(pTab);
        return;
     } 
     FillTable(pTab,7,"%-50.50s","正在向移动签退,请等待...");
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
        //签退成功后立即与移动方对帐
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

      
      outtext("\n正在和移动方面核对数据,这可能要花较长时间,请耐心等待.. \n");
  
      nRetVal=AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sAnswer);
      if(nRetVal)
      {
          outtext(sTcpipErrInfo);
          keyb();
          return;
      }
      if(!strncmp(sAnswer,"000000",6)) strcpy(sAnswer,"对帐成功！按任意键返回...");
      outtext(sAnswer);
      keyb();
}


void MobileCompletePrint(long nType,char * sItem,char * sJym)
{
     PRINTFORM * ppf;
     char sResult[256],sLsh[41];           //签到返回结果
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
     outtext("                               完全划拨单打印                    \n");
     outtext("             ----------------------------------------------------\n");
          
     for(;;)
     {
    	gotoxy(10,23);
     	outtext("        请输入相应信息，按ESC键返回...                           ");

     	for(;;)
     	{
        	nKey=GetValue(24,10,"请输入手机号码: %11s",sPhone);
         	if(nKey==KEY_ESC) return;
         	if(!IsValidPhoneNo(sPhone)) continue;
         	nKey=GetValue(24,12,"请输入打印年度: %4d",&nYear);
         	if(nKey==KEY_ESC) return;
         	nKey=GetValue(24,14,"请输入打印月份: %2d",&nMonth);
         	if(nKey==KEY_ESC) return;
         	if(nMonth<1 || nMonth>12) continue;
         	break;
	  	}
        
     	sprintf(sBeginTime,"%04ld%02ld20000000",
             nYear-(nMonth<=1),(((nMonth-1)<1)?12:nMonth-1));
     	sprintf(sEndTime,"%04ld%02ld21000000",nYear,nMonth);
     
     	gotoxy(10,23); 
     	outtext("            正在查询完全划拨单数据，请等待...                   ");
        
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
         	outtext("          该用户没有该月的预存款划拨发票...          ");
         	keyb();
         	clearline(23);
     		continue;
     	}
		
		PrintYdfp(sLsh); //移动新格式，移动处理
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
