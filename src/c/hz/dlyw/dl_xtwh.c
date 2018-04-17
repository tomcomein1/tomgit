/*=======================================================
               ����ҵ��ϵͳά��(dl_xtwh.c)
               
original programmer:chenbo
created date:20010824
=======================================================*/
#include"newtask.h"
#include"dlpublic.h"
#include"tools.h"

extern char sTcpipErrInfo[81];
extern long nSysLkrq;

void DatabaseBackup()
{
    long nRetVal;
    char sbuf[256],sFileName[21];
    
    clrscr();
    nRetVal=MessageBox("    ϵͳ���ݽ�������������ϵͳ�����ݣ��������ݵ����ݴ�����ļ���"
               "��bf+8λ����.dmp�������ļ���������ݿ�������ϣ������Զ������ļ�"
               "���Ƶ������ĵ�ǰĿ¼�¡�Ҫ������",0);
    
    if(nRetVal!=KEY_YES) return;
    
    clrscr();
    
    gotoxy(30,10);
    outtext("���ڱ��ݣ��������ĵȺ�...");
    
    
    if(AutoRunTask("dldj",TASK_SYSTEM_BACKUP," ","%ld%s",&nRetVal,sFileName))
    {
    	MessageBox(sTcpipErrInfo,1);
    	return;          
    }
    
    if(nRetVal)
    {
    	MessageBox(sFileName,1);
    	return;          
    }
    
    sprintf(sbuf,"    ϵͳ���ݲ����ɹ��������ļ���Ϊ��%s��"
                 "��Ҫ�������ص�������?",sFileName);
    	
    nRetVal=MessageBox(sbuf,0);
    if(nRetVal!=KEY_YES) return;
    
    clrscr();
    gotoxy(30,10);
    outtext("���������ļ����������ĵȺ�...");
    
    GetRemoteFile("dldj",sFileName,sFileName,1);
}
    

void DatabaseRestore()
{
    long nRetVal;
    char sbuf[256],sFileName[31]="bf";
    
    clrscr();
    nRetVal=MessageBox("    ���棡ִ��ϵͳ�ָ��Ժ����ݿ������е����ݶ�����ʧ��������"
                       "ִ��ϵͳ�ָ�֮ǰ��һ��ϵͳ���ݡ�Ҫ����ִ��ϵͳ�ָ���",0);

    if(nRetVal!=KEY_YES) return;
    clrscr();
    
    nRetVal=GetValue(20,10,"�����뱸���ļ���:%-30.30s",sFileName);
    if(nRetVal!=KEY_RETURN) return;
    
    gotoxy(30,10);
    clearline(10);
    outtext("���ڻָ����ݣ��������ĵȺ�...");
    if(AutoRunTask("dldj",TASK_SYSTEM_RESTORE,"%s","%ld%s",sFileName,&nRetVal,sbuf))
    {
    	MessageBox(sTcpipErrInfo,1);
    	return;          
    }
    
    if(nRetVal)
    {
    	MessageBox(sbuf,1);
    	return;          
    }
    
    clrscr();
    gotoxy(30,10);
    outtext("ϵͳ�ָ���ϣ������������...");
    keyb();
    
}

void CleanupHistory()
{

    long nRetVal,nMode,nDate;
    char sbuf[256],sConfirm[10]="no";
    

    clrscr();
    GetSysLkrq();
    nDate=nSysLkrq;
    
    nDate-=100;
    if(nDate%10000<100) nDate+=1200;
    nRetVal=MessageBox("    �����ʷ���ݰ��������ϵͳ�еĲ��Ϸ����ݡ�������õ�ҵ����ϸ���ݡ�"
                       "���ĳʱ����ǰ���������ݡ�Ϊ��֤���ݰ�ȫ������ִ�б�������ǰ����һ��ϵ"
                       "ͳ���ݡ�Ҫ������",0);

    if(nRetVal!=KEY_YES) return;
    clrscr();

    
    nRetVal=GetValue(20,8,"������������ݵĽ�ֹ����:%8d",&nDate);
    if(nRetVal!=KEY_RETURN) return;

    nMode=0;
    
    
    nRetVal=GetValue(20,10,"�Ƿ������ʱ����ǰ����������[yes/no]:%-3.3s",sConfirm);
    alltrim(sConfirm);
    if(!strcmp(sConfirm,"yes")) nMode|=1;
    if(nRetVal!=KEY_RETURN) return;

    nRetVal=GetValue(20,12,"�Ƿ������ʱ����ǰ��ҵ������[yes/no]:%-3.3s",sConfirm);
    alltrim(sConfirm);
    if(!strcmp(sConfirm,"yes")) nMode|=2;
    if(nRetVal!=KEY_RETURN) return;

    nRetVal=GetValue(20,14,"�Ƿ������ʱ����ǰ����ʱ����[yes/no]:%-3.3s",sConfirm);
    alltrim(sConfirm);
    if(!strcmp(sConfirm,"yes")) nMode|=4;
    if(nRetVal!=KEY_RETURN) return;
    
    
    clrscr();
    gotoxy(30,10);
    outtext("���������ʷ���ݣ��������ĵȺ�...");
    
    
    if(AutoRunTask("dldj",TASK_CLEANUP_HISTORY,"%ld%ld","%ld%s",nDate,nMode,&nRetVal,sbuf))
    {
    	MessageBox(sTcpipErrInfo,1);
    	return;          
    }
    
    MessageBox(sbuf,1);

}


void AccessAccount()
{
  
    char sJym[16],sJymc[51],sBuffer[100]="\0";
    long nCxms,nLkrq=nSysLkrq,nRet;
    
    clrscr();
    
    if(SelectJym(sJym,1)<=0) return;
    RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);
    
    if(MessageBox("    ����:�ֹ�����,ȷ�ϱ������Ѿ��ֹ����ˣ�"
    "���������һ��!Ҫ������?",0)!=KEY_YES) return;

    clrscr();
    nRet=GetValue(12,2,"������������ʳɹ���ֹ����:%8d",&nLkrq);
    if(nRet!=KEY_RETURN) return;
    nRet=0;
    
    switch(nCxms)
    {
        case 123:
        case 124:
        case 130:
        case 140:
        case 522:
        case 821:
        	nRet=RunSql("update dl$zwmx set ncsbz=2 where cjym=%s and ncsbz=1 and nlkrq<=%ld",
							sJym,nLkrq);
		if(nRet>0) 
			nRet=RunSql("update dl$jymcsfb set ccs='%ld' where cjym=%s and nbh=34",
							nLkrq,sJym);
	        break;
		
	case 521:
	case 222:
	case 333:
		nRet=RunSql("update dl$ywmx_%t set ncsbz=3 where ncsbz=0 and nlkrq<=%ld",sJym,nLkrq);
		if(nRet>0)
		nRet=RunSql("update dl$zwmx set ncsbz=2 where cjym=%s and nlkrq<=%ld",sJym,nLkrq);
		break;

        default:
        	sprintf(sBuffer,"�˽��ײ���Ҫ����,���������...");
		break;
    }//end switch
    
    if(nRet>0)
    {
	CommitWork();
	sprintf(sBuffer,"%ld��ǰ�ʵ����ɹ�,���������...",nLkrq);
    }
    else
    {
	RollbackWork();
        sprintf(sBuffer,"���ʿɵ���%ldǰ���ѳɹ�����!",nLkrq);
    }//end if
    MessageBox(sBuffer,1);
}
