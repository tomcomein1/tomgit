#include"newtask.h"
#include"tasktool.h"
extern char sTcpipErrInfo[81];

void CheckAccountWithBP(long ntype,char * item,char *sJym)
{
  char sBuffer[256]="012345";
  long nRetVal;
  
  clrscr();
  outtext("\n���ں�Ѱ�����ʣ���ȴ�...");
  AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sBuffer);
  if(strncmp(sBuffer,"000000",6))
  	outtext("\n���ʳ���%s",sTcpipErrInfo);
  else  outtext("\n���ʳɹ��������������...");
  keyb();  	

}

