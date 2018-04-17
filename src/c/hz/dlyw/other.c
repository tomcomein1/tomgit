
#include<stdarg.h>
#include"dlpublic.h"
#include"newtask.h"
#include"other.h"
#include"tools.h"

extern char sSysCzydh[7];
extern char sSysJh[10];
extern char sSysJsdh[10];
extern char sSysIp[16];
extern char sSysJsmc[JM_JM_LEN+1];//本人所属支局局名
extern char sSysCzymc[CZYB_CZYMC_LEN+1];//操作员名称
extern char sSysCzyqx[21];//操作员权限汉字解释
extern long nSysCzyqx;
extern double dDlywVer;
extern long nMakeDate;
extern long nSysPcType;
extern long nSysLkrq;
extern char sTcpipErrInfo[81];
extern char sPassWord[41];    //用户口令
extern char sSysPassword[41];//代理业务表所在用户的口令，缺省为DDDD
extern char sSysDlywHost[41];
extern char sSysDlywService[41];

/*===========================================================
函数: long GenerateLsh(long nLb,char* cJh,char* cJym,char* cLsh);
目的: 产生流水号
参数: nLb  流水号类别
返回值: 0 产生失败 1	成功
生成日期：2001.2.23
原始编程人员：txy
修改历史
日期	       修改人员	 原因
局号9位+15交易码+YYYYMMDD+8位序号 (SR)
===========================================================*/

long GenerateLsh(long nLb,char *cjh,char *cjym,char *clsh)
{
  long nRetVal,nValue;
  char sSequence[41];

  GetSysLkrq();
  switch(nLb)
  {
    case 1:
    case 2:
         strcpy(sSequence,"DLNBLSH");
         break;
    case 3:
         sprintf(sSequence,"DLWTLSH");
         break;
  }

  nRetVal=RunSelect("SELECT %t.nextval FROM dual INTO %ld",sSequence,&nValue);
  if(nRetVal<=0)
  {
        HintError(DB_ERR,"从序列%t取数出错",sSequence);
	return 0;
  }//endif     
  switch(nLb)
  {
    case 1:
         sprintf(clsh,"%-9.9s%-15.15s%08ld%08ld",cjh,cjym,nSysLkrq,nValue);
         break;
    case 2:
         sprintf(clsh,"%-9.9s%08ld%08ld",cjh,nSysLkrq,nValue);
         break;
    case 3:
         sprintf(clsh,"%08ld",nValue);
         break;
  }
  return 1;  

}



void SystemBackup()
{
  char sBackupFileName[101];
  long nRetVal;
  
  outtext("\n按回车键任意键开始进行系统备份，或其它键返回...");
  if(keyb()!=KEY_RETURN) return;
  
  outtext("\n正在备份数据，请稍候...");
  if(AutoRunTask("dldj",TASK_SYSTEM_BACKUP," ","%ld%s",
                 &nRetVal,sBackupFileName))
  {
           HintError(TASK_ERR,"系统备份任务失败！");
           return ;
  }
  
  if(nRetVal)
  {
           HintError(TASK_ERR,sBackupFileName);
           return ;
  }  
  
  if(GetRemoteFile("dldj",sBackupFileName,sBackupFileName,1))
  {
           HintError(TASK_ERR,"取得备份文件%s到本机出错",sBackupFileName);
           return;
  }
  outtext("\n系统备份成功，备份文件名(当前目录)：%s",sBackupFileName);
  keyb();
}


void HintError(int nType,char *sInfo, ...)
{
  va_list pvar;
  char sMessage[256];
  
  va_start(pvar,sInfo);
  vsprintf(sMessage,sInfo,pvar);
  va_end(pvar);
  
  savewin();
  MessageBox(sMessage,1);
  popwin();

}


void show_statue()
{ 
        ReverseChar();
        GetSysLkrq();
        /*gotoxy(1,23);
		outtext("%80s"," ");
		gotoxy(1,23);
		outtext("成都人民南路三段十七号附一号华西美庐A座11B 电话: 028-85498102  传真：85498102");*/
        
        gotoxy(1,24);
        outtext("%80s"," ");
        gotoxy(1,24); 
	    outtext("| 状态栏 | %s | %s(%s) | 软件版本号: %.2lf-%06ld |鸿志",
	        sSysJsmc,sSysCzymc,sSysCzyqx,dDlywVer,nMakeDate-(nMakeDate/1000000)*1000000);
		if(nSysPcType==1)
        { 
			gotoxy(71,24);
        	outtext("| 通讯机 ");
		}
        gotoxy(80,24);
        outtext("|");
        ShowChar();
}


/*----------------------------------------------------------------*
函数：long SelectJym;
功能：动态生成交易码菜单，并返回用户所选择的交易码
参数：sValue:被选择的交易码值，nlx- 0/1/2/3/4:全部/代收/代售/代办/只选择脱机业务
返回：>0被选中，<0:功能键的相反数
*-----------------------------------------------------------------*/

long SelectJym(char * sValue,long nlx)
{
    MENU * pMenu,* pmn;
    long nId,nSyzt;
    char sTemp[81],sJym[16],sJymc[51];
    
    pMenu=CreateMenu("业务",0);
    
    if(pMenu==NULL) return NULL;

    if(nlx==4)
       nId=OpenCursor("select cjym,cjymc from dl$jym where nbl=1 and substr(cjym,9,1)='0'");
    else if(nlx)
       nId=OpenCursor("select cjym,cjymc from dl$jym where nbl=%ld",nlx);
    else       
       nId=OpenCursor("select cjym,cjymc from dl$jym");

    if(nId<0)
    {
             	DropMenu(pMenu);
             	HiError(nId,"Error!");
             	return NULL;
    }	                
    
    for(;;)
    {
       	 if(FetchCursor(nId,"%s%s",sJym,sJymc)<=0) break;
         nSyzt=-1;
         RunSelect("select nsyzt from dl$jymkzfb where cjym=%s and cjh=%s into %ld",
                   sJym,sSysJsdh,&nSyzt);
         switch(nSyzt)
         {
         	case 0:          
         	    sprintf(sTemp,"[√]%s",sJymc);
     	            break;
            	case 1:
         	    sprintf(sTemp,"[Ｘ]%s",sJymc);
                    break;
                default:
         	    sprintf(sTemp,"[？]%s",sJymc);
                    break;
         }
         AddMenu(pMenu,sTemp,sJym);
    }//end for
    CloseCursor(nId);
    clrscr();     
    gotoxy(16,10);
    outtext("请选择业务代码:");
    nId=PopMenu(pMenu,32,10,12);
    if(nId>0) strcpy(sValue,GetAddition(pMenu,nId));
    DropMenu(pMenu);
    return nId;
}



void ChangeUserPassword()
{
   char sNewPassword[17]="\0";
   char sNewPasswordConfirm[17]="\0";
   long nRetVal;
   char sStatement[100];   
   
   clrscr();
   
   gotoxy(30,7);
   outtext("操作员工号：%s",sSysCzydh);
   if(GetValue(30,9, "请输入新口令:%8p",sNewPassword)==KEY_ESC) return;
   alltrim(sNewPassword);
   upper(sNewPassword);
   if(sNewPassword[0]<'A' || sNewPassword[0]>'Z')
   {
        gotoxy(20,18);
   	outtext("口令的第一个字符必须为'A'-'Z'的字母，此口令无效...");
   	keyb();
   	return;
   }     
   if(GetValue(30,11,"请再确认一次:%8p",sNewPasswordConfirm)==KEY_ESC) return;
   alltrim(sNewPasswordConfirm);
   upper(sNewPasswordConfirm);
   
      
   gotoxy(30,18);
   if(strcmp(sNewPassword,sNewPasswordConfirm))
   {
   	outtext("两次输入的口令不符，口令更改失败...");
   	keyb();
   	return;
   }
   if(nSysCzyqx!=0)
   {
       RunSql("update dl$czyb set cpassword=%s where cczydh=%s",sNewPassword,sSysCzydh);
   }
   else
   {
       DlPasswordEnc(sNewPassword,sNewPasswordConfirm);
       sNewPasswordConfirm[16]='\0';
       if(AutoRunTask("dldj",TASK_PUT_VALUE_TO_FILE,"%s%s%s","%ld",
          "dlywd.ini","password=",sNewPasswordConfirm,&nRetVal))
       {
        	   	
        	   	outtext("更改dlz口令出错:%s",sTcpipErrInfo);
        	   	keyb();
           		return;
       }
       sprintf(sStatement,"GRANT CONNECT TO DLZ IDENTIFIED BY %s",sNewPassword);
       nRetVal=RunSql(sStatement);
       if(nRetVal<0)
       {
       	  HiError(nRetVal,"修改密码错误");
       	  return;
       }	  
       strcpy(sSysPassword,sNewPassword);
   }
   strcpy(sPassWord,sNewPassword);
   CommitWork();
   outtext("口令更改成功...");
   keyb();
   return;
}

int DlPasswordEnc(char* sPassWord,char * sNewPassWord)
{
    char keyword[9]="19780616";
    char dest[9];
    int  len;
    DesEncString(keyword,8,sPassWord,8,dest,&len);
    BCDtoASCII(dest,8,sNewPassWord);
    return 1;

}

int DlPasswordDec(char* sPassWord,char* sNewPassWord)
{

    char keyword[9]="19780616";
    char dest[9];

    upper(sPassWord);
    ASCIItoBCD(sPassWord,16,dest);
    DesDecString(keyword,8,dest,8,sNewPassWord,0);
    return 1;
}

long FindRoute(char * sDestination,char * sGateway)
{
     lower(sSysDlywHost);

     if(strcmp(sSysDlywHost,"localhost"))
     {
     	strcpy(sGateway,sSysDlywHost);
     	return 1;
     }	
     if(RunSelect("select lower(sgateway) from dl$route where \
        sdestination=%s into %s",sDestination,sGateway)>0) return 1;
        
     if(RunSelect("select lower(sgateway) from dl$route where \
        upper(sdestination)='DEFAULT' into %s",sGateway)>0) return 1;
        
     return 0;   

}
