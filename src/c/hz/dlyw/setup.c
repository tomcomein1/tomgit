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
        outtext("\n正在链接数据库...");
        if(ConnectDatabase(sUserName,sPassWord)>=0)  break;
        sUserName[0]='\0';
        sPassWord[0]='\0';
  } //end while(nAttemptNo);
  
  if(nAttemptNo<=0)  //经过三次未成功
  {
        outtext("\n经过三次不能正确登录数据库，退出系统...");
        Finished();
        exit(0);
  }  	
  strcpy(sSysManager,sUserName);
  strcpy(sSysPassword,sPassWord);

  ///////////////////////////////////////////
  sprintf(sbuf,"代理业务系统安装程序(版本:%.2lf  制作日期:%ld)",dVersion,nMakeDate);
  pm=CreateMenu(sbuf,0);
  BindMenu(pm,"初始安装代理业务系统",NULL,InstallDlyw);
  BindMenu(pm,"卸载代理业务系统",NULL,UnInstallDlyw);
  AddMenu(pm, "  0.退出系统",NULL);
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
  outtext("\n按ENTER任意键开始安装，其它键返回...");
  if(keyb()!=KEY_RETURN) return;
  nRetVal=RunSelect("SELECT * from all_users WHERE username='DLZ'");
  if(nRetVal>0)
  {
  	outtext("\n错误：DLZ用户已经存在，请先卸载代理系统再安装...");
  	keyb();
  	return;
  }
  
  /*
     建立DLZ用户，其默认表空间为USERS，临时表空间为TEMP，
     在系统安装之前上述表空间应当已经存在。
  */   
  nRetVal=RunSql("CREATE USER DLZ IDENTIFIED BY DDDD DEFAULT TABLESPACE USERS \
                  TEMPORARY TABLESPACE TEMP");

  /*
     为提高ORACLE运行效率，让代理组不占用SYSTEM表空间，而TEMP和USERS表空间的占用
     是无限制的。
  
  */
  if(nRetVal>0)
       nRetVal=RunSql("ALTER USER DLZ QUOTA 0 ON SYSTEM QUOTA UNLIMITED ON USERS \
                       QUOTA UNLIMITED ON TEMP");                  
  //授予DBA权限给DLZ
  if(nRetVal>0) nRetVal=RunSql("GRANT DBA TO DLZ");
  
  
  if(nRetVal<0)
  {
  	ShowInstallError("建立代理组用户出错");
  	return;
  }
  outtext("\n建立DLZ用户成功，初始密码为'dddd',请在系统安装成功后修改它！");

  CloseDatabase();
  if(ConnectDatabase("DLZ","DDDD")<0)
  {
        outtext("\n无法以DLZ用户登录！");
        return;
  }

  
  
  nRetVal=CreateDlywTable();
  if(nRetVal<0)
  {
  	ShowInstallError("建立表失败");
  	outtext("\n系统安装不成功！\n");
  	return;
  }
  InitDljh();//插入局号信息
  outtext("\n系统基础表结构安装成功,按任意键开始下一步...");
  keyb();
  clrscr();
  for(;;)
  {
      clearline(9);
      nRetVal=GetValue(20,9,"请输入地区局局号:%-4.4s",sDjjh);
      if(nRetVal==KEY_ESC) return;
      if(nRetVal==KEY_RETURN)
      {
      	sprintf(sSysJh,"%s00000",sDjjh);
      	nRetVal=RunSelect("select cjm from  dl$jh where cjh=%s into %s",sSysJh,sSysJm);
      	if(nRetVal<=0) continue;
      
        gotoxy(60,9);
        outtext(sSysJm);
        gotoxy(10,23);
        outtext("         地区局局名输入是否正确(y/n)");
        nRetVal=keyb();
        clearline(23);
        if(nRetVal=='y'||nRetVal=='Y') break;
        continue;        
      }
  }    	
  
  GetValue(20,11,"请输入本局IP地址:%-15.15s",sSysIp);
  clrscr();
  InitDlcs(); //插入参数信息
  CommitWork();
  outtext("\n系统安装完毕。\n");
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
   outtext("\n警告！一旦执行卸载，代理系统中的所有数据都将丢失，按'Y'键继续...");
   nKey=keyb();
   if(nKey!='y'&&nKey!='Y') return;
    
   outtext("\n请在确认一次，按'C'键继续...");
   nKey=keyb();
   if(nKey!='c'&&nKey!='C') return;
   
   outtext("\n正在卸载代理业务系统，请稍候...");
   nKey=RunSql("drop user dlz cascade");
   if(nKey>0)   outtext("\n系统卸载完毕...");
   else         ShowInstallError("卸载失败");
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
                
       
        nRetVal=CreatePrint();     //打印单据
        if(nRetVal<0) nRetVal=CreatePrint();

        if(nRetVal>0) 
          if((nRetVal=CreateCzyb())<0)
              nRetVal=CreateCzyb();      //操作员

        if(nRetVal>0) 
          if((nRetVal=CreaetJh())<0)
              nRetVal=CreaetJh();        //局号

        if(nRetVal>0) 
          if((nRetVal=CreateJymkzfb())<0)  //交易开展附表
              nRetVal=CreateJymkzfb();
        
        if(nRetVal>0)
          if((nRetVal=CreateJymcsfb())<0)
              nRetVal=CreateJymcsfb();   //交易参数
        
        if(nRetVal>0)
          if((nRetVal=CreateJym())<0)
              nRetVal=CreateJym();       //交易码
        
        if(nRetVal>0)
          if((nRetVal=CreateZwmx())<0)
              nRetVal=CreateZwmx();      //帐务明细

        if(nRetVal>0) 
          if((nRetVal=CreateGs())<0)
              nRetVal=CreateGs();        //公司编号及名称

        if(nRetVal>0) 
          if((nRetVal=CreateDlywcs())<0)
              nRetVal=CreateDlywcs();    //代理系统参数

        if(nRetVal>0) 
          if((nRetVal=CreateYwzl())<0)
              nRetVal=CreateYwzl();      //业务种类

        if(nRetVal>0)
          if((nRetVal=CreateHwjcb())<0)
              nRetVal=CreateHwjcb();     //货物进出表

        if(nRetVal>0) 
          if((nRetVal=CreateHwkcb())<0)
              nRetVal=CreateHwkcb();     //货物库存表

        if(nRetVal>0)
          if((nRetVal=CreateLkzhgl())<0)
              nRetVal=CreateLkzhgl();    //绿卡帐号

        if(nRetVal>0) 
          if((nRetVal=CreateLkzjhd())<0)
              nRetVal=CreateLkzjhd();    //绿卡资金

        /*if(nRetVal>0)
           if((nRetVal=CreateAirCity())<0)   //航空城市对照表
               nRetVal=CreateAirCity();  

        if(nRetVal>0)
           if((nRetVal=CreateAirRebate())<0)//航空公司打折管理
               nRetVal=CreateAirRebate();*/

        if(nRetVal>0) 
          if((nRetVal=CreateRoute())<0)
              nRetVal=CreateRoute();     //路由表

        if(nRetVal>0) 
          if((nRetVal=CreateSequence())<0)
              nRetVal=CreateSequence();  //建立序列



        if(nRetVal<0)
            ShowInstallError("表建立失败");
       
        fflush(stdout); 
        CommitWork();
        return nRetVal;
}


void show_statue()
{
}

/*************************************
**
**函数: GetSysUserName
**目的: 接收用户名
**参数: 无
**返回值: 功能键
**
*************************************/
long GetSysUserName()
{
  int nFlag,nKey;
  TEXTBOX pTextBox;
  gotoxy(40,12);
  putspace(20);
  gotoxy(28,10);
  printf("操作员代号:");
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
**函数: GetSysPassWord
**目的: 接收口令
**参数: 无
**返回值: 功能键
**
*************************************/

long GetSysPassWord()
{
  int nFlag,nKey;
  TEXTBOX pTextBox;
  gotoxy(28,12);
  printf("操作员口令:");
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
     outtext("\n错误：%s,%s",str,sTcpipErrInfo);
     keyb();          

}

