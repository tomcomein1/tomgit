#include"dlpublic.h"
#include"tools.h"
#include"tasktool.h"
#include"newtask.h"

extern long nSysLkrq;           //ϵͳ¼������
extern long nSysSfm;            //ϵͳʱ����
extern char sTcpipErrInfo[81];
extern char sRemoteHost[16];    //����Դ��ַ

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
    		MessageBox("ϵͳ�Ѿ��ɹ����ʣ�",1);
    		return;
    	}//endif
    }
    clrscr(); 
    nLkrq=nSysLkrq;
    //printf("\n\t������������:[%ld]",nLkrq);    
    if(nLkrq==nSysLkrq||nRetVal==0)
    {
         if(MessageBox("  ���ѣ����ʺ���ֹ����ҵ����ȷ��Ҫ���ж����룿",0)!=KEY_YES) return;
    }
    else
    {
         if(MessageBox("  ��������δ���ʵļ�¼��ѡ��\"ȷ��\"���ȶ������ʣ�������Ӱ�챾��ҵ��",0)!=KEY_YES) return;
    }    	          
    clrscr();
    outtext("\n���ں����ٷ����ʣ��������Ҫ�ϳ�ʱ�䣬�����ĵȴ�...\n");

    if(AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sAnswer))
    {
        outtext(sTcpipErrInfo);
        keyb();
        return;
    }
    if(!strncmp(sAnswer,"000000",6)) strcpy(sAnswer,"���ʳɹ��������������...");
    outtext(sAnswer);
    keyb();
}
