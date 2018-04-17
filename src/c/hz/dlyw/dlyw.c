/***********************************************************************
** @Copyright...........2001.7
**filename       �ļ���: dlyw.c
**target           Ŀ��:
**create time  ��������: 2000.7
**original programmer          ԭʼ�����Ա: �²�
**edit history �޸���ʷ:
   
  200107 �²� 1.����һ�����Ա�����ݿ��в��ٽ����û����ڲ���Ա��������cpasswod
                �ֶ��Դ洢�ò���Ա�����롣�����¼������μ�����LoginDatabase;
              2.�����ۺ����Ĳ���Ա����ҵ�����Ա��
**************************************************************************/
#include "dlyw.h"
#include "dlpublic.h"
#include "tools.h"
#include "filechar.h"
#include "des.h"
#include "tasktool.h"
#include "newtask.h"
#include "exchfile.h"

#ifdef DLYWLIB
extern char zjusername[20];   
extern char zjpassword[20];
#endif

extern char sTcpipErrInfo[81];
extern double dDlywVer,dHelpVer;  //����ҵ�����������ĵ�����汾
extern char sSysIp[16];
extern char sSysJh[10];       //�����־ֺ�
extern char sSysJm[JM_JM_LEN+1];/*��ҵ�ؾ־���  cs2*/
extern char sSysJsdh[10];     //����Ա��������
extern char sSysJsmc[JM_JM_LEN+1];   //��������֧�־���
extern char sUserName[101];   //�û�����
extern char sPassWord[41];    //�û�����
extern long nSysLkrq;  //¼������
extern long nSysSfm;   //ϵͳʱ��  Сʱ ���� ��
extern long nSysAutoClose;
extern char sSysCzydh[7];//����Ա����
extern char sSysCzymc[9];//����Ա����
extern long nSysCzyqx;//����ԱȨ��
extern char sSysCzyqx[21];//����ԱȨ�޺��ֽ���
extern long nSysCzylb;//����Ա���0������Ĭ�ϣ���1���ۺ���  dlyw.ini:czylb=1;
extern char sSysDsHost[41]; // ���ݿ��ػ���������
extern char sSysDsService[41]; // ���ݿ��ػ����̶˿ں�
extern char sSysManager[7];//����ҵ��������û���ȱʡΪDLZ
extern char sSysPassword[41];//����ҵ��������û��Ŀ��ȱʡΪDDDD
extern char sSysDlywHost[41]; // ����ҵ���ػ���������
extern char sSysDlywService[41]; // �ػ����̶˿ں�
extern long nSysPcType;          //���0������ǰ̨��1��ͨѶ��
extern long nSysMode;
extern MENU * pQueryMenu;
#ifdef UNIX
extern long nTermType;
#endif

#ifndef DLYWLIB
void main(int argc,char *argv[])
#else
void dlywmain()
#endif
{
  long nRetVal;

  char value[100];
  
  #ifdef UNIX
  system("stty ixon ixoff -ixany");
  NoShowRead();
  screento80();
  #endif

  ReadDlywIni();
  
  #ifdef DLYWLIB    //���ۺ������
  nSysCzylb=1;
  strcpy(sUserName,zjusername);
  strcpy(sPassWord,zjpassword);
  #else             //����ϵͳ��ΪSHELL����
  if (argc==2)
  {
      if(strchr(argv[1],'/'))  FetchStringToArg(argv[1],"%s/%s",sUserName,sPassWord);
      else strcpy(sUserName,argv[1]);
  }
  #endif  //end ifdef DLYWLIB

  nSysCzylb=zhw_dec(sUserName, sPassWord);
  alltrim(sUserName);
  alltrim(sPassWord);  
  upper(sUserName);
  upper(sPassWord);
 
  clrscr();

  if(!strcmp(sUserName,"HELP"))
  {
  	ShowHelp("dlyw.hlp",NULL);
  	Finished();
  	exit(0);
  }
  
  //��¼���ݿ�
  nRetVal=LoginDatabase();

  if(nRetVal<=0)
  {
        if(nRetVal<0)
        {
             nRetVal=MessageBox("�޷���¼���ݿ⣬��Ҫ�õ��ⷽ��İ�����",0);
             if(nRetVal==KEY_YES)
             {
                clrscr();
                ShowHelp("dlyw.hlp",NULL/*"�Ҹ���ô��?"*/);
             }     
        }     
  	Finished();
  	exit(0);
  }

  strcpy(sSysCzydh,sUserName); 
  //ȡ�ô���ϵͳ����
  if(GetDlywInfo()<=0)
  {
        CloseDatabase();
        outtext("\nȡ�ô���ϵͳ��Ϣ����...");
  	Finished();
        exit(0);
  }

  CommitWork();

  dlywxt();     //����ҵ��ϵͳ���
  CloseDatabase();  
  Finished();
  exit(0);

}



/*===========================================================
������LoginDatabase
Ŀ�ģ���¼���ݿ�
˵������¼���ݿ�ķ����Ǹ���ȫ�ֱ���nSysCzylb�������ģ�����
      ȫ�ֱ�����ֵ���������ļ�dlyw.ini�е�������czylb��������
      Ĭ��Ϊ��0��
      
      ��¼���̣�
      
      ���ȷ�������ȡ��DLDJ���ݿ���DLZ�Ŀ��DES���ܡ�
      
      ��nSysCzylb=1ʱ�����ۺ�������Ա��
        [1] �Ըù��ŵ�¼���أ�dlyw�����ݿ⣬��ѯ�ۺ�������Ա
            ��Ϣ��͵�λ��Ϣ��(gg$ryxx,gg$dwxx)��ȡ������Ա
            ����(cxm)���ۺ����ֺ�(gg$ryxx.cdwdm)������ԱȨ��
            (cgzlbbm)���;�������(cdwmc)��
            
        [2] ��DLZ���ŵ�¼������(dldj)���ݿ⣬�Ȳ鿴�ֺű���
            �Ƿ��б��ۺ����ֺ�����Ӧ����ֺţ�(dl$jh.czhwjh,
            dl$jh.cjh)����û�У������ֺžֺż�¼��
        
        [3] ��ѯ������ֺţ�Ȼ���ڲ���Ա���в�ѯ�þֺ����Ƿ�
            �б�����Ա�ļ�¼�����������е�[5]�������û����
            �鿴�������Ƿ��б�����Ա�Ĺ��ţ�����У����������
            �ظ���������롣
            
        [4] �������Ա��û�б�����Ա�Ĺ��ţ����Զ��ڲ���Ա��
            �м��뱾����Ա�ļ�¼��
        
        [5] ˢ�±�����Ա����Ϣ����ȡ����Ա��������Ϣ����ҵ��
            ����Ա��ݽ���ϵͳ��        
     
      ��nSysCzylb=0ʱ���Ǵ������Ա
            ��DLZ���ŵ�¼������(dldj)���ݿ⣬�鿴����Ա����
            �Ƿ��иò���Ա�ļ�¼������У���Ƚ������Ƿ�һ
            ��(dl$czyb.cpassword)����һ����ȡ��ϵͳͳ��Ϣ��
            ����ϵͳ��
���أ�>0�ɹ���<0ʧ��,=0:��ʾ�û�������¼            
*==========================================================*/
static long LoginDatabase()
{
   
   long nRetVal=0,nDlZjjh=2;
   char sDlywPassWord[17];
   char sNewJsdh[10],sbuf[200];
   long nAutoRegiste=0;
      
   clrscr();
   if(nSysCzylb!=1) outtext("\n���ڽ��г�ʼ��...");

   if(AutoRunTask("dldj",TASK_GET_VALUE_FROM_FILE,
                 "%s%s%s","%ld%s",
                 "dlywd.ini","password=","dddd",
                 &nRetVal,sDlywPassWord))
   {
        printf("\nȡ�ô���ϵͳ��Կ����:%s",sTcpipErrInfo);
        keyb();
      	return -1;
   }

   alltrim(sDlywPassWord);
   strcpy(sSysPassword,sDlywPassWord);

   if(strlen(sDlywPassWord)==16)
   {  
       memset(sSysPassword,'\0',sizeof(sSysPassword));
       DlPasswordDec(sDlywPassWord,sSysPassword);
   }
       
   upper(sSysPassword);
   if(!strcmp(sUserName,"DLZ")) nSysCzylb=0;
   if(nSysCzylb!=1) //����
   {
        if(ConnectDatabase(sSysManager,sSysPassword)<0)
        {
               outtext("\n�޷����ӵ����ַ���������֪ͨϵͳ����Ա��");
               keyb();
               return -1;	
        }
        
        UpgradeProg();  //����Ƿ���Ҫ����

        nRetVal=3;
        while(nRetVal--)
        {
    	    clrscr();
  	    if(sUserName[0]=='\0')
  	    {
  	       if(GetValue(30,9,"�����빤��:%6s",sUserName)==KEY_ESC)
  	       {
  	       	  CloseDatabase();
  	          return 0;
  	       }
  	    }
  	
    	    if(sPassWord[0]=='\0')
    	    {
    	       if(GetValue(30,11,"���������:%8p",sPassWord)==KEY_ESC)
    	       {
    	       	  CloseDatabase();
    	       	  return 0;
    	       }
    	    }   	
    	    alltrim(sUserName);
    	    alltrim(sPassWord);
    	    upper(sUserName);
    	    upper(sPassWord);
            if(strcmp(sUserName,"DLZ"))
            {
    	       if(RunSelect("select * from dl$czyb where cczydh=%s \
    	          and cpassword=%s",sUserName,sPassWord)>0) return 1;  //successed
    	    }
    	    else if(!strcmp(sPassWord,sSysPassword)) return 1;  
    	    sUserName[0]='\0';
    	    sPassWord[0]='\0';
        }//end while
        CloseDatabase();
        return -1;
   }//����ϵͳ��¼���
   else  //�ۺ���
   {
	/***********************
        strcpy(sSysDsHost,"cent");
        if(ConnectDatabase(sUserName,sPassWord)<0)
        {
            MessageBox("    �޷��������ݿ⣬�����ȼ��bkfxd�Ƿ��������������ʹ�õ�"
            "�Ǵ���ϵͳ�Ĳ���Ա����ɾ�������ļ�dlyw.ini��czylb=1��������...",1);
            return -1;
        }
        
        //��ѯ�ۺ�������Ա��Ϣ
        nRetVal=RunSelect("select cxm,gg$ryxx.cdwdm,cgzlbbm,cdwmc \
                           from gg$ryxx,gg$dwxx where gg$ryxx.cdwdm=gg$dwxx.cdwdm \
                           and  cgzlbbm in ('YYY' ,'YYBZZ','ZJZ','ZJSUPER') \
                           and cgh=%s into %s%s%s%s",
                           sUserName,sSysCzymc,sSysJsdh,sSysCzyqx,sSysJsmc);
        
        CloseDatabase();//�رձ������ݿ�����
        if(nRetVal<=0)
        {
            outtext("\n��Ա��Ϣ����û��������Ϣ���ѯ����...");
            keyb();
            return -1;
        } 
	******************************/
	strncpy(sSysCzymc,getenv("HZCZYMC"),9);
	strncpy(sSysJsdh,getenv("HZJSDH"),9);
	strcpy(sSysJsmc,"�ۺ���֧��");
	
        strcpy(sSysDsHost,"dldj"); //���ӵ��������ݿ�
  
        if(nSysCzylb!=1) outtext("\n�����������ݿ�...");
        if(ConnectDatabase(sSysManager,sSysPassword)<0)
        {
            outtext("\n�޷����ӵ����ַ���������֪ͨϵͳ����Ա��");
            return -1;	
        }

        UpgradeProg();  //����Ƿ���Ҫ����

        alltrim(sSysJsdh);
        RunSelect("select to_number(ccs) from dl$dlywcs where nbh=8 into %ld",
                  &nAutoRegiste);
        //���Ҷ�Ӧ�Ĵ���ֺ�
        nRetVal=RunSelect("select cjh from dl$jh where czhwjh=%s and nlb=4 into %s",
                           sSysJsdh,sSysJsdh);
                           
        if(nRetVal<=0)
        {
            
            outtext("\n�Ҳ����ۺ����ֺ�'%s'��Ӧ�Ĵ���ֺ�,"
                    "���ڲ����Ӧ�ľֺ�...",sSysJsdh);
            //���ҳ��ۺ����ֺ�����Ӧ�Ĵ���ֺ�
        
            nRetVal=RunSelect("select cjh from dl$jh where nlb=3 and \
                               substr(czhwjh,1,4)=substr(%s,1,4) into %s",
                               sSysJsdh,sNewJsdh);
            if(nRetVal<=0)
            {
                CloseDatabase();
                outtext("\n�Ҳ����ۺ����ֺ�'%s'��Ӧ�Ĵ����ؾ־ֺ�,"
                         "��֪ͨϵͳά��Ա����þֺŵĶ�Ӧ...",sSysJsdh);
                keyb();
                return -1;
            }
            
            
            RunSelect("select max(to_number(substr(cjh,7,3))) from dl$jh \
                       where substr(cjh,1,6)=substr(%s,1,6) and nlb=4 into %ld",
                       sNewJsdh,&nDlZjjh);
            sprintf(sNewJsdh+6,"%03ld",nDlZjjh+1);
            printf("\n�·���ֺţ�%s-->%s",sNewJsdh,sSysJsdh);

            //����ֺż�¼
            nRetVal=RunSql("insert into dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) \
                            values(%s,%s,%s,4,'0000000000','.')",
                            sNewJsdh,sSysJsmc,sSysJsdh);
            if(nRetVal<=0)
            {
            	CloseDatabase();
            	HiError(nRetVal,"�����ֺű����!");
            	return -1;
            } 
            CommitWork();
            MessageBox("�����ڵ�֧�ֺ��ڴ���ϵͳ���Ѿ��Ǽǳɹ�������Ҫ����"
            "ҵ����֪ͨϵͳά��Ա�������ֵ�ʹ��Ȩ��",1);
            strcpy(sSysJsdh,sNewJsdh);
            clrscr();
            
        }
        nSysCzyqx=3; //�ۺ�������Ա����ȨΪҵ�����Ա
        nRetVal=RunSelect("select * from dl$czyb where cczydh=%s",
                          sUserName);
        if(nRetVal<=0) //û�иò���Ա��Ϣ���Զ�����
        {
            if(!nAutoRegiste)
            {
                MessageBox("    ���Ĺ����ڵ����ַ�������û�еǼǣ���֪ͨϵͳ����Ա����"
                           "���Ĺ��Ż����ۺ�������Ա�Զ��Ǽǹ��ܡ�",1);
            	return -1;
            }

            nRetVal=RunSql("insert into dl$czyb(cczydh,cczymc,nczyqx,cpassword,\
                            cjh,cbl,nbl,dbl,clxbz) values(%s,%s,3,%s,%s,\
                            '�ۺ�������Ա',0,0,'00000000000000000000')",
                            sUserName,sSysCzymc,sPassWord,sSysJsdh);
            if(nRetVal<0)
            {
                CloseDatabase();
            	outtext("\n����Ա�����޷��������Ĺ��ţ���֪ͨϵͳ����Ա...");
            	keyb();
            	return -1;
            }
            //RunSql("update dl$jh set cjm=%s and cjh=%s",sSysJsmc,sSysJsdh);
            CommitWork();
            return 1; //successed
        }
        else  //�иò���Ա����
        {
            
            nRetVal= RunSelect("select * from dl$czyb where cczydh=%s and cjh=%s",
                               sUserName,sSysJsdh);
            if(nRetVal>0)
            {
            	//RunSql("update dl$jh set cjm=%s and cjh=%s",sSysJsmc,sSysJsdh);
            	RunSql("update dl$czyb set cpassword=%s,cczymc=%s where cczydh=%s",
            	        sPassWord,sSysCzymc,sUserName);
            	CommitWork();
            	return 1; //successed        
            }
            CloseDatabase(); //����Ա�����ظ��������ֵĹ��������Աһ�£�
            outtext("\n����Ա�����ظ����˹��Ų��ܵ�¼...");
            keyb();
            return -1;
        }
   }//end else �ۺ���   
}
/*======================================================
������UpgradeProg
Ŀ�ģ���������
˵����
      ����汾�����dl$dlywcs��
      nbh            ����
      11              dlyw����İ汾��
      12              dlyw.hlp�ı��İ汾��
======================================================*/

void UpgradeProg()
{
    double dNewDlywVer=0.0,dNewHelpVer=0.0;
    long nflag,nRetVal;
    char smsg[256];
     
    if(nSysCzylb!=1) outtext("\n���ڼ��汾��Ϣ...");
     
    RunSelect("select to_number(ccs) from dl$dlywcs where nbh=11 into %lf",
               &dNewDlywVer);

    RunSelect("select to_number(ccs) from dl$dlywcs where nbh=12 into %lf",
               &dNewHelpVer);
    
    if(dNewHelpVer>dHelpVer)
    {
         outtext("\n���ڸ��°����ĵ��ļ�dlyw.hlp�������ĵȴ�...");
         ReplaceFile("dldj","dlyw.hlp");
    }
    
    
    if(dNewDlywVer>dDlywVer)
    {
        sprintf(smsg,
        "��ӭʹ�óɶ���־��˾�����Ĵ���ҵ��ϵͳ,������ʹ�õ�ϵͳ�汾Ϊ%.2lf��"
        "�����Ѿ����°汾���밴ȷ�ϼ���ʼ��������������������¼...",dDlywVer);
        nRetVal=MessageBox(smsg,0);
        clrscr();
        if(nRetVal!=KEY_YES)
        {
            CloseDatabase();
            Finished();
            exit(0);
        }    
            
       	#ifdef DOS
        outtext("\n���ڸ�������DLYWDOS.EXE�������ĵȴ�...");
        nflag=ReplaceFile("dldj","dlywdos.exe");
        #else
        outtext("\n���ڸ�������dlyw�������ĵȴ�...");
        nflag=ReplaceFile("dldj","dlyw");
        #endif
        switch(nflag)
        {
          case -9:
            strcpy(smsg,"�޷����ӵ�Զ��������dldj��...");
            break;
          case 0:
            strcpy(smsg,"ϵͳ�ѳɹ����������������˳������Ա�ʹ���µ�ϵͳ...");
            break;
          default:
            ReasonReceiveFileFromRemote(nflag,smsg);
            break;
        }
        clrscr();
        MessageBox(smsg,1);
        CloseDatabase();
        Finished();
        exit(0);
    }//end if
}

/*=======================================================
������GetDlywInfo
���ܣ�ȡ�ô���ϵͳ��Ϣ�Ͳ���Ա��Ϣ
���أ�>0�ɹ���<0����ʧ��,=0ϵͳ��Ҫ����
*======================================================*/

static long GetDlywInfo()
{
    
    char * sCzyqx[7]={"�����û�","ϵͳά��","�����鳤",
                      "ҵ�����Ա","��ѯͳ��","����","֧�ֳ�"};
    long nRetVal;

    if(nSysCzylb!=1) outtext("\n����ȡ�ô���ϵͳ��Ϣ...");
    nRetVal=RunSelect("select substr(ccs,1,9) from dl$dlywcs where nbh=1 into %s",sSysJh);
    if(nRetVal>0)
    nRetVal=RunSelect("select substr(ccs,1,20) from dl$dlywcs where nbh=2 into %s",sSysJm);
    if(nRetVal>0)
    nRetVal=RunSelect("select substr(ccs,1,16) from dl$dlywcs where nbh=3 into %s",sSysIp);
    if(nRetVal>0)
    nRetVal=RunSelect("select to_number(ccs) from dl$dlywcs \
                       where nbh=10 into %ld",&nSysAutoClose);
     
    if(!strcmp(sUserName,"DLZ"))
    {
    	nSysCzyqx=0;
    	strcpy(sSysCzyqx,sCzyqx[nSysCzyqx]);
    	strcpy(sSysCzymc,"DLZ");
        strcpy(sSysJsdh,sSysJh);
        strcpy(sSysJsmc,sSysJm);
    	return nRetVal;
    }
    
    if(nRetVal>0)
    nRetVal=RunSelect("select cczymc,cjh,nczyqx from dl$czyb where cczydh=%s \
                       into %s%s%ld",sSysCzydh,sSysCzymc,sSysJsdh,&nSysCzyqx);
                       
    if(nRetVal>0)
    nRetVal=RunSelect("select cjm from dl$jh where cjh=%s into %s",sSysJsdh,sSysJsmc);
    strcpy(sSysCzyqx,sCzyqx[nSysCzyqx]);
    
    return nRetVal;
}

/*=============================================================
����: ReadDlywIni
Ŀ��: ����������ͬ����ini�ļ��ж�ȡ��ʼ������
����: ��
*============================================================*/

static void ReadDlywIni()
{
  #ifdef UNIX
  char sTermFlag[21];
  char sModeFlag[21];
  strcpy(sSysDsHost,"dldj");
  strcpy(sSysDlywHost,"dldj");
  strcpy(sSysDsService,"bkfx");
  strcpy(sSysDlywService,"dlyw");
  strcpy(sTermFlag,"vt100");
  strcpy(sModeFlag,"term");
  nSysMode=UNIX_TERM;
  nTermType=TERM_ANSI;
  #else
  strcpy(sSysDsHost,"cent");
  strcpy(sSysDlywHost,"dldj");
  strcpy(sSysDsService,"6666");
  strcpy(sSysDlywService,"6667");
  #endif
  strcpy(sSysManager,"DLZ");
  strcpy(sSysPassword,"dddd");
  strcpy(sUserName,"\0");
  strcpy(sPassWord,"\0");
  nSysCzylb=1;
  nSysPcType=0;
  pQueryMenu=NULL;
  
  
  GetValueFromFile("dlyw.ini","czylb=%ld",&nSysCzylb);
  GetValueFromFile("dlyw.ini","dsservice=%s",sSysDsService);
  GetValueFromFile("dlyw.ini","dlywservice=%s",sSysDlywService);
  
  #ifdef UNIX
  GetValueFromFile("dlyw.ini","term=%s",sTermFlag);
  GetValueFromFile("dlyw.ini","mode=%s",sModeFlag);
  GetValueFromFile("dlyw.ini","pctype=%ld",&nSysPcType);
  
  if(!strcmp(sTermFlag,"ansi")) nTermType=TERM_ANSI;
  if(!strcmp(sTermFlag,"vt100")) nTermType=TERM_VT100;
  if(!strcmp(sTermFlag,"vt220")) nTermType=TERM_VT220;

  if(!strcmp(sModeFlag,"host")) nSysMode=UNIX_HOST;
  if(!strcmp(sModeFlag,"term")) nSysMode=UNIX_TERM;
  #endif
  
}

static void Finished()
{
  #ifndef DOS
  clrscr();
  fflush(stdin);
  fflush(stdout);
  showcursor();
  EndRead();
  #endif
}
