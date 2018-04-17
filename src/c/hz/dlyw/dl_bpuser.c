#include"newtask.h"
#include"tasktool.h"
extern char sTcpipErrInfo[81];

void CheckAccountWithBP(long ntype,char * item,char *sJym)
{
  char sBuffer[256]="012345";
  long nRetVal;
  
  clrscr();
  outtext("\n正在和寻呼对帐，请等待...");
  AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sBuffer);
  if(strncmp(sBuffer,"000000",6))
  	outtext("\n对帐出错：%s",sTcpipErrInfo);
  else  outtext("\n对帐成功，按任意键返回...");
  keyb();  	

}

