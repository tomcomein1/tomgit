#include"dlpublic.h"
#include"tools.h"
#include"filechar.h"
#include"setup.h"

extern char sTcpipErrInfo[81];
char sSysDsHost[41];
char sSysDsService[41];
char sUserName[31]="\0";
char sPassWord[31]="\0";
extern long nTermType;
char sSysJh[10]=".",sSysJm[21]=".",sSysIp[16]=".";
char sSysJsdh[10],sSysCzydh[7];
char sSysManager[7];
char sSysPassword[41];
long nSysAutoClose=0;

int main(int argc,char *argv[])
{

  MENU * pm;
  char sbuf[100];
  int nAttemptNo=3;
  long nMakeDate=20010710;
  double dVersion=3.00;

  NoShowRead();
  screento80();
  clrscr();
  ReadIni();
  
  if (argc==2)
  {
      if(strchr(argv[1],'/'))  FetchStringToArg(argv[1],"%s/%s",sUserName,sPassWord);
      else strcpy(sUserName,argv[1]);
      upper(sUserName);
      upper(sPassWord);
  }

  while(nAttemptNo--)
  {
  	clrscr();
  	if(sUserName[0]=='\0')
  	{
            if(GetSysUserName())
            {
               Finished();
               exit(0);
            }
  	}
  	
  	if(sPassWord[0]=='\0')
  	{
            if(GetSysPassWord())
            {
               Finished();
               exit(0);
            }
  	}
        upper(sUserName);
        upper(sPassWord);
        clrscr();
        outtext("\n�����������ݿ�...");
        if(ConnectDatabase(sUserName,sPassWord)>=0)  break;
        sUserName[0]='\0';
        sPassWord[0]='\0';
  } //end while(nAttemptNo);
  
  if(nAttemptNo<=0)  //��������δ�ɹ�
  {
        outtext("\n�������β�����ȷ��¼���ݿ⣬�˳�ϵͳ...");
        Finished();
        exit(0);
  }  	
  strcpy(sSysManager,sUserName);
  strcpy(sSysPassword,sPassWord);

  ///////////////////////////////////////////
  sprintf(sbuf,"����ҵ��ϵͳ��װ����(�汾:%.2lf  ��������:%ld)",dVersion,nMakeDate);
  pm=CreateMenu(sbuf,0);
  BindMenu(pm,"��ʼ��װ����ҵ��ϵͳ",NULL,InstallDlyw);
  BindMenu(pm,"ж�ش���ҵ��ϵͳ",NULL,UnInstallDlyw);
  AddMenu(pm, "  0.�˳�ϵͳ",NULL);
  while(ShowMenu(pm)!=0);
  DropMenu(pm);
  ///////////////////////////////////////////

  CloseDatabase();
  Finished();
  exit(0);
}

void InstallDlyw()
{
  long nRetVal;
  char sDjjh[5]=".";
   
  clrscr(); 
  outtext("\n��ENTER�������ʼ��װ������������...");
  if(keyb()!=KEY_RETURN) return;
  nRetVal=RunSelect("SELECT * from all_users WHERE username='DLZ'");
  if(nRetVal>0)
  {
  	outtext("\n����DLZ�û��Ѿ����ڣ�����ж�ش���ϵͳ�ٰ�װ...");
  	keyb();
  	return;
  }
  
  /*
     ����DLZ�û�����Ĭ�ϱ�ռ�ΪUSERS����ʱ��ռ�ΪTEMP��
     ��ϵͳ��װ֮ǰ������ռ�Ӧ���Ѿ����ڡ�
  */   
  nRetVal=RunSql("CREATE USER DLZ IDENTIFIED BY DDDD DEFAULT TABLESPACE USERS \
                  TEMPORARY TABLESPACE TEMP");

  /*
     Ϊ���ORACLE����Ч�ʣ��ô����鲻ռ��SYSTEM��ռ䣬��TEMP��USERS��ռ��ռ��
     �������Ƶġ�
  
  */
  if(nRetVal>0)
       nRetVal=RunSql("ALTER USER DLZ QUOTA 0 ON SYSTEM QUOTA UNLIMITED ON USERS \
                       QUOTA UNLIMITED ON TEMP");                  
  //����DBAȨ�޸�DLZ
  if(nRetVal>0) nRetVal=RunSql("GRANT DBA TO DLZ");
  
  
  if(nRetVal<0)
  {
  	ShowInstallError("�����������û�����");
  	return;
  }
  outtext("\n����DLZ�û��ɹ�����ʼ����Ϊ'dddd',����ϵͳ��װ�ɹ����޸�����");

  CloseDatabase();
  if(ConnectDatabase("DLZ","DDDD")<0)
  {
        outtext("\n�޷���DLZ�û���¼��");
        return;
  }

  
  
  nRetVal=CreateDlywTable();
  if(nRetVal<0)
  {
  	ShowInstallError("������ʧ��");
  	outtext("\nϵͳ��װ���ɹ���\n");
  	return;
  }
  InitDljh();//����ֺ���Ϣ
  outtext("\nϵͳ������ṹ��װ�ɹ�,���������ʼ��һ��...");
  keyb();
  clrscr();
  for(;;)
  {
      clearline(9);
      nRetVal=GetValue(20,9,"����������־ֺ�:%-4.4s",sDjjh);
      if(nRetVal==KEY_ESC) return;
      if(nRetVal==KEY_RETURN)
      {
      	sprintf(sSysJh,"%s00000",sDjjh);
      	nRetVal=RunSelect("select cjm from  dl$jh where cjh=%s into %s",sSysJh,sSysJm);
      	if(nRetVal<=0) continue;
      
        gotoxy(60,9);
        outtext(sSysJm);
        gotoxy(10,23);
        outtext("         �����־��������Ƿ���ȷ(y/n)");
        nRetVal=keyb();
        clearline(23);
        if(nRetVal=='y'||nRetVal=='Y') break;
        continue;        
      }
  }    	
  
  GetValue(20,11,"�����뱾��IP��ַ:%-15.15s",sSysIp);
  clrscr();
  InitDlcs(); //���������Ϣ
  CommitWork();
  outtext("\nϵͳ��װ��ϡ�\n");
  keyb();
  return;
  
}

void ReadIni()
{
   char sTermType[21]="ansi";
   strcpy(sSysDsHost,"dldj");
   strcpy(sSysDsService,"bkfx");
   
   GetValueFromFile("setup.ini","term=%s",sTermType);
   lower(sTermType);
   if(!strcmp(sTermType,"vt220"))       nTermType=TERM_VT220;
   else if(!strcmp(sTermType,"vt100"))  nTermType=TERM_VT100;
   else if(!strcmp(sTermType,"ansi"))   nTermType=TERM_ANSI;
   GetValueFromFile("setup.ini","dsservice=%s",sSysDsService);
}

void UnInstallDlyw()
{
   long nKey;
   clrscr();
   outtext("\n���棡һ��ִ��ж�أ�����ϵͳ�е��������ݶ�����ʧ����'Y'������...");
   nKey=keyb();
   if(nKey!='y'&&nKey!='Y') return;
    
   outtext("\n����ȷ��һ�Σ���'C'������...");
   nKey=keyb();
   if(nKey!='c'&&nKey!='C') return;
   
   outtext("\n����ж�ش���ҵ��ϵͳ�����Ժ�...");
   nKey=RunSql("drop user dlz cascade");
   if(nKey>0)   outtext("\nϵͳж�����...");
   else         ShowInstallError("ж��ʧ��");
   keyb();   
   CommitWork();
   

}

void Finished()
{
  clrscr();
  fflush(stdin);
  fflush(stdout);
  showcursor();
  EndRead();
}

long CreateDlywTable()
{
        long nRetVal;
                
       
        nRetVal=CreatePrint();     //��ӡ����
        if(nRetVal<0) nRetVal=CreatePrint();

        if(nRetVal>0) 
          if((nRetVal=CreateCzyb())<0)
              nRetVal=CreateCzyb();      //����Ա

        if(nRetVal>0) 
          if((nRetVal=CreaetJh())<0)
              nRetVal=CreaetJh();        //�ֺ�

        if(nRetVal>0) 
          if((nRetVal=CreateJymkzfb())<0)  //���׿�չ����
              nRetVal=CreateJymkzfb();
        
        if(nRetVal>0)
          if((nRetVal=CreateJymcsfb())<0)
              nRetVal=CreateJymcsfb();   //���ײ���
        
        if(nRetVal>0)
          if((nRetVal=CreateJym())<0)
              nRetVal=CreateJym();       //������
        
        if(nRetVal>0)
          if((nRetVal=CreateZwmx())<0)
              nRetVal=CreateZwmx();      //������ϸ

        if(nRetVal>0) 
          if((nRetVal=CreateGs())<0)
              nRetVal=CreateGs();        //��˾��ż�����

        if(nRetVal>0) 
          if((nRetVal=CreateDlywcs())<0)
              nRetVal=CreateDlywcs();    //����ϵͳ����

        if(nRetVal>0) 
          if((nRetVal=CreateYwzl())<0)
              nRetVal=CreateYwzl();      //ҵ������

        if(nRetVal>0)
          if((nRetVal=CreateHwjcb())<0)
              nRetVal=CreateHwjcb();     //���������

        if(nRetVal>0) 
          if((nRetVal=CreateHwkcb())<0)
              nRetVal=CreateHwkcb();     //�������

        if(nRetVal>0)
          if((nRetVal=CreateLkzhgl())<0)
              nRetVal=CreateLkzhgl();    //�̿��ʺ�

        if(nRetVal>0) 
          if((nRetVal=CreateLkzjhd())<0)
              nRetVal=CreateLkzjhd();    //�̿��ʽ�

        /*if(nRetVal>0)
           if((nRetVal=CreateAirCity())<0)   //���ճ��ж��ձ�
               nRetVal=CreateAirCity();  

        if(nRetVal>0)
           if((nRetVal=CreateAirRebate())<0)//���չ�˾���۹���
               nRetVal=CreateAirRebate();*/

        if(nRetVal>0) 
          if((nRetVal=CreateRoute())<0)
              nRetVal=CreateRoute();     //·�ɱ�

        if(nRetVal>0) 
          if((nRetVal=CreateSequence())<0)
              nRetVal=CreateSequence();  //��������



        if(nRetVal<0)
            ShowInstallError("����ʧ��");
       
        fflush(stdout); 
        CommitWork();
        return nRetVal;
}


void show_statue()
{
}

/*************************************
**
**����: GetSysUserName
**Ŀ��: �����û���
**����: ��
**����ֵ: ���ܼ�
**
*************************************/
long GetSysUserName()
{
  int nFlag,nKey;
  TEXTBOX pTextBox;
  gotoxy(40,12);
  putspace(20);
  gotoxy(28,10);
  printf("����Ա����:");
  pTextBox.x=40;
  pTextBox.y=10;
  pTextBox.nLen=6;
  pTextBox.sValue[0]='\0';
  pTextBox.nFlag=0;
  nFlag=0;
  while(!nFlag)
  {
    nKey=getstring(&pTextBox);
    switch(nKey)
    {
      case KEY_F1:
        break;
      case KEY_RIGHT:
      case KEY_RETURN:
        alltrim(pTextBox.sValue);
        if(pTextBox.sValue[0]!='\0')
        {
          strcpy(sUserName,pTextBox.sValue);
          nFlag=1;
        }
        break;
      case KEY_ESC:
        return 1;
      default:
        bell();
        break;
    }
  }
  return 0;
}

/*************************************
**
**����: GetSysPassWord
**Ŀ��: ���տ���
**����: ��
**����ֵ: ���ܼ�
**
*************************************/

long GetSysPassWord()
{
  int nFlag,nKey;
  TEXTBOX pTextBox;
  gotoxy(28,12);
  printf("����Ա����:");
  pTextBox.x=40;
  pTextBox.y=12;
  pTextBox.nLen=20;
  pTextBox.sValue[0]='\0';
  pTextBox.nFlag=0;
  nFlag=0;
  while(!nFlag)
  {
    nKey=getpassword(&pTextBox);
    switch(nKey)
    {
      case KEY_F1:
        break;
      case KEY_RIGHT:
      case KEY_RETURN:
        alltrim(pTextBox.sValue);
        if(pTextBox.sValue[0]!='\0')
        {
          strcpy(sPassWord,pTextBox.sValue);
          nFlag=1;
        }
        break;
      case KEY_ESC:
        return 1;
      default:
        bell();
        break;
    }
  }
  return 0;
}


void ShowInstallError(char * str)
{
     GetSqlErrInfo(sTcpipErrInfo);
     outtext("\n����%s,%s",str,sTcpipErrInfo);
     keyb();          

}

