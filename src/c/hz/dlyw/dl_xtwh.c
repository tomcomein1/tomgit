/*=======================================================
               代理业务系统维护(dl_xtwh.c)
               
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
    nRetVal=MessageBox("    系统备份将备份整个代理系统的数据，并将备份的数据存放在文件中"
               "（bf+8位日期.dmp），该文件存放在数据库服务器上，并可自动将该文件"
               "复制到本机的当前目录下。要继续吗？",0);
    
    if(nRetVal!=KEY_YES) return;
    
    clrscr();
    
    gotoxy(30,10);
    outtext("正在备份，请您耐心等候...");
    
    
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
    
    sprintf(sbuf,"    系统备份操作成功，数据文件名为：%s，"
                 "需要将它下载到本地吗?",sFileName);
    	
    nRetVal=MessageBox(sbuf,0);
    if(nRetVal!=KEY_YES) return;
    
    clrscr();
    gotoxy(30,10);
    outtext("正在下载文件，请您耐心等候...");
    
    GetRemoteFile("dldj",sFileName,sFileName,1);
}
    

void DatabaseRestore()
{
    long nRetVal;
    char sbuf[256],sFileName[31]="bf";
    
    clrscr();
    nRetVal=MessageBox("    警告！执行系统恢复以后，数据库中现有的数据都将丢失，建议在"
                       "执行系统恢复之前作一次系统备份。要继续执行系统恢复吗？",0);

    if(nRetVal!=KEY_YES) return;
    clrscr();
    
    nRetVal=GetValue(20,10,"请输入备份文件名:%-30.30s",sFileName);
    if(nRetVal!=KEY_RETURN) return;
    
    gotoxy(30,10);
    clearline(10);
    outtext("正在恢复备份，请您耐心等候...");
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
    outtext("系统恢复完毕，按任意键返回...");
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
    nRetVal=MessageBox("    清除历史数据包括：清除系统中的不合法数据、清除无用的业务明细数据、"
                       "清除某时段以前的帐务数据。为保证数据安全，请在执行本功能以前先做一次系"
                       "统备份。要继续吗？",0);

    if(nRetVal!=KEY_YES) return;
    clrscr();

    
    nRetVal=GetValue(20,8,"请输入清除数据的截止日期:%8d",&nDate);
    if(nRetVal!=KEY_RETURN) return;

    nMode=0;
    
    
    nRetVal=GetValue(20,10,"是否清除此时段以前的帐务数据[yes/no]:%-3.3s",sConfirm);
    alltrim(sConfirm);
    if(!strcmp(sConfirm,"yes")) nMode|=1;
    if(nRetVal!=KEY_RETURN) return;

    nRetVal=GetValue(20,12,"是否清除此时段以前的业务数据[yes/no]:%-3.3s",sConfirm);
    alltrim(sConfirm);
    if(!strcmp(sConfirm,"yes")) nMode|=2;
    if(nRetVal!=KEY_RETURN) return;

    nRetVal=GetValue(20,14,"是否清除此时段以前的临时数据[yes/no]:%-3.3s",sConfirm);
    alltrim(sConfirm);
    if(!strcmp(sConfirm,"yes")) nMode|=4;
    if(nRetVal!=KEY_RETURN) return;
    
    
    clrscr();
    gotoxy(30,10);
    outtext("正在请除历史数据，请您耐心等候...");
    
    
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
    
    if(MessageBox("    敬告:手工调帐,确认被代理方已经手工调账，"
    "建议您检查一下!要继续吗?",0)!=KEY_YES) return;

    clrscr();
    nRet=GetValue(12,2,"请输入调整对帐成功截止日期:%8d",&nLkrq);
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
        	sprintf(sBuffer,"此交易不需要调整,任意键返回...");
		break;
    }//end switch
    
    if(nRet>0)
    {
	CommitWork();
	sprintf(sBuffer,"%ld以前帐调整成功,任意键返回...",nLkrq);
    }
    else
    {
	RollbackWork();
        sprintf(sBuffer,"无帐可调，%ld前帐已成功处理!",nLkrq);
    }//end if
    MessageBox(sBuffer,1);
}
