
#include<stdarg.h>
#include"dlpublic.h"
#include"newtask.h"
#include"other.h"
#include"tools.h"

extern char sSysCzydh[7];
extern char sSysJh[10];
extern char sSysJsdh[10];
extern char sSysIp[16];
extern char sSysJsmc[JM_JM_LEN+1];//��������֧�־���
extern char sSysCzymc[CZYB_CZYMC_LEN+1];//����Ա����
extern char sSysCzyqx[21];//����ԱȨ�޺��ֽ���
extern long nSysCzyqx;
extern double dDlywVer;
extern long nMakeDate;
extern long nSysPcType;
extern long nSysLkrq;
extern char sTcpipErrInfo[81];
extern char sPassWord[41];    //�û�����
extern char sSysPassword[41];//����ҵ��������û��Ŀ��ȱʡΪDDDD
extern char sSysDlywHost[41];
extern char sSysDlywService[41];

/*===========================================================
����: long GenerateLsh(long nLb,char* cJh,char* cJym,char* cLsh);
Ŀ��: ������ˮ��
����: nLb  ��ˮ�����
����ֵ: 0 ����ʧ�� 1	�ɹ�
�������ڣ�2001.2.23
ԭʼ�����Ա��txy
�޸���ʷ
����	       �޸���Ա	 ԭ��
�ֺ�9λ+15������+YYYYMMDD+8λ��� (SR)
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
        HintError(DB_ERR,"������%tȡ������",sSequence);
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
  
  outtext("\n���س����������ʼ����ϵͳ���ݣ�������������...");
  if(keyb()!=KEY_RETURN) return;
  
  outtext("\n���ڱ������ݣ����Ժ�...");
  if(AutoRunTask("dldj",TASK_SYSTEM_BACKUP," ","%ld%s",
                 &nRetVal,sBackupFileName))
  {
           HintError(TASK_ERR,"ϵͳ��������ʧ�ܣ�");
           return ;
  }
  
  if(nRetVal)
  {
           HintError(TASK_ERR,sBackupFileName);
           return ;
  }  
  
  if(GetRemoteFile("dldj",sBackupFileName,sBackupFileName,1))
  {
           HintError(TASK_ERR,"ȡ�ñ����ļ�%s����������",sBackupFileName);
           return;
  }
  outtext("\nϵͳ���ݳɹ��������ļ���(��ǰĿ¼)��%s",sBackupFileName);
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
		outtext("�ɶ�������·����ʮ�ߺŸ�һ�Ż�����®A��11B �绰: 028-85498102  ���棺85498102");*/
        
        gotoxy(1,24);
        outtext("%80s"," ");
        gotoxy(1,24); 
	    outtext("| ״̬�� | %s | %s(%s) | ����汾��: %.2lf-%06ld |��־",
	        sSysJsmc,sSysCzymc,sSysCzyqx,dDlywVer,nMakeDate-(nMakeDate/1000000)*1000000);
		if(nSysPcType==1)
        { 
			gotoxy(71,24);
        	outtext("| ͨѶ�� ");
		}
        gotoxy(80,24);
        outtext("|");
        ShowChar();
}


/*----------------------------------------------------------------*
������long SelectJym;
���ܣ���̬���ɽ�����˵����������û���ѡ��Ľ�����
������sValue:��ѡ��Ľ�����ֵ��nlx- 0/1/2/3/4:ȫ��/����/����/����/ֻѡ���ѻ�ҵ��
���أ�>0��ѡ�У�<0:���ܼ����෴��
*-----------------------------------------------------------------*/

long SelectJym(char * sValue,long nlx)
{
    MENU * pMenu,* pmn;
    long nId,nSyzt;
    char sTemp[81],sJym[16],sJymc[51];
    
    pMenu=CreateMenu("ҵ��",0);
    
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
         	    sprintf(sTemp,"[��]%s",sJymc);
     	            break;
            	case 1:
         	    sprintf(sTemp,"[��]%s",sJymc);
                    break;
                default:
         	    sprintf(sTemp,"[��]%s",sJymc);
                    break;
         }
         AddMenu(pMenu,sTemp,sJym);
    }//end for
    CloseCursor(nId);
    clrscr();     
    gotoxy(16,10);
    outtext("��ѡ��ҵ�����:");
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
   outtext("����Ա���ţ�%s",sSysCzydh);
   if(GetValue(30,9, "�������¿���:%8p",sNewPassword)==KEY_ESC) return;
   alltrim(sNewPassword);
   upper(sNewPassword);
   if(sNewPassword[0]<'A' || sNewPassword[0]>'Z')
   {
        gotoxy(20,18);
   	outtext("����ĵ�һ���ַ�����Ϊ'A'-'Z'����ĸ���˿�����Ч...");
   	keyb();
   	return;
   }     
   if(GetValue(30,11,"����ȷ��һ��:%8p",sNewPasswordConfirm)==KEY_ESC) return;
   alltrim(sNewPasswordConfirm);
   upper(sNewPasswordConfirm);
   
      
   gotoxy(30,18);
   if(strcmp(sNewPassword,sNewPasswordConfirm))
   {
   	outtext("��������Ŀ�������������ʧ��...");
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
        	   	
        	   	outtext("����dlz�������:%s",sTcpipErrInfo);
        	   	keyb();
           		return;
       }
       sprintf(sStatement,"GRANT CONNECT TO DLZ IDENTIFIED BY %s",sNewPassword);
       nRetVal=RunSql(sStatement);
       if(nRetVal<0)
       {
       	  HiError(nRetVal,"�޸��������");
       	  return;
       }	  
       strcpy(sSysPassword,sNewPassword);
   }
   strcpy(sPassWord,sNewPassword);
   CommitWork();
   outtext("������ĳɹ�...");
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
