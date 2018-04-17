#include"dlpublic.h"
#include"tools.h"
#include"tasktool.h"
#include"newtask.h"

extern long nSysLkrq;           //系统录卡日期
extern long nSysSfm;            //系统时分秒
extern char sTcpipErrInfo[81];
extern char sRemoteHost[16];    //数据源地址

void B2iCheckAccount(long nId,char * item,char * sJym)
{
    
    char sAnswer[1024]=".";
    long nLkrq=0,nRetVal;
    
    GetSysLkrq();
    
    nRetVal=RunSelect("select * from dl$zwmx where ncsbz=1 and cjym=%s",sJym);
    
    /*if(nRetVal>0)
       RunSelect("select min(nlkrq) from dl$zwmx where ncsbz=1 and cjym=%s into %ld",
                      sJym,&nLkrq);
     */
     
    if(nRetVal<0) return;
    if(nRetVal==0)
    {
    	nRetVal=RunSelect("select * from dl$zwmx where ncsbz=2 and cjym=%s and nlkrq=%ld",
    	             sJym,nSysLkrq);
    	
    	if(nRetVal>0)
    	{
    		MessageBox("系统已经成功对帐！",1);
    		return;
    	}//endif
    }
    clrscr(); 
    nLkrq=nSysLkrq;
    //printf("\n\t你所对帐日期:[%ld]",nLkrq);    
    if(nLkrq==nSysLkrq||nRetVal==0)
    {
         if(MessageBox("  提醒：对帐后将终止今日业务，您确定要进行对帐码？",0)!=KEY_YES) return;
    }
    else
    {
         if(MessageBox("  尚有往日未对帐的记录，选择\"确认\"将先对往日帐，并不会影响本日业务。",0)!=KEY_YES) return;
    }    	          
    clrscr();
    outtext("\n正在和人寿方对帐，这可能需要较长时间，请耐心等待...\n");

    if(AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sAnswer))
    {
        outtext(sTcpipErrInfo);
        keyb();
        return;
    }
    if(!strncmp(sAnswer,"000000",6)) strcpy(sAnswer,"对帐成功！按任意键返回...");
    outtext(sAnswer);
    keyb();
}
