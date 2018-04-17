/*===========================================================
                通讯机数据库访问模块
*==========================================================*/

#include"newtask.h"
#include"tasktool.h"


char sSysDsHost[41]="dldj";
char sSysDsService[41]="bkfx";
char sSysManager[7]="dlz";
char sSysPassWord[41]="dddd";
char sSysDlywHost[41]="dldj";
char sSysDlywService[41]="dlyw";

static int DlPasswordDec(char* sPassWord,char* sNewPassWord);
long Dlsrv_ConnectDatabase();
long GetManagerPassword();

//通讯机以DLZ链接数据库，返回>0成功
long Dlsrv_ConnectDatabase()
{
     	
     if(ConnectDatabase(sSysManager, sSysPassWord)>=0) return 1;

     if(!GetManagerPassword()) return 0;

     if(ConnectDatabase(sSysManager, sSysPassWord)>=0) return 1;
     return 0;
}

long GetManagerPassword()
{
     char sTempPassWord[41];
     long nRetVal;

     memset(sTempPassWord,'\0',sizeof(sTempPassWord));
     if(AutoRunTask("dldj",TASK_GET_VALUE_FROM_FILE,
                    "%s%s%s","%ld%s",
                    "dlywd.ini","password=","dddd",
                    &nRetVal,sTempPassWord)) return 0;

     if(strlen(sTempPassWord)==16)
          DlPasswordDec(sTempPassWord,sSysPassWord);
     else strcpy(sSysPassWord,sTempPassWord);
     
     return 1;
     
}

static int DlPasswordDec(char* sPassWord,char* sNewPassWord)
{

    char keyword[9]="19780616";
    char dest[9];

    ASCIItoBCD(sPassWord,16,dest);
    DesDecString(keyword,8,dest,8,sNewPassWord,0);
    return 1;
}
