/***********************************************************************
** @Copyright...........2001.7
**filename       文件名: dlyw.c
**target           目的:
**create time  生成日期: 2000.7
**original programmer          原始编程人员: 陈博
**edit history 修改历史:
   
  200107 陈博 1.对于一般操作员在数据库中不再建立用户，在操作员表中增加cpasswod
                字段以存储该操作员的密码。具体登录过程请参见函数LoginDatabase;
              2.所有综合网的操作员都是业务操作员。
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
extern double dDlywVer,dHelpVer;  //代理业务程序与帮助文档程序版本
extern char sSysIp[16];
extern char sSysJh[10];       //地区局局号
extern char sSysJm[JM_JM_LEN+1];/*现业县局局名  cs2*/
extern char sSysJsdh[10];     //操作员局所代号
extern char sSysJsmc[JM_JM_LEN+1];   //本人所属支局局名
extern char sUserName[101];   //用户名称
extern char sPassWord[41];    //用户口令
extern long nSysLkrq;  //录卡日期
extern long nSysSfm;   //系统时间  小时 分钟 秒
extern long nSysAutoClose;
extern char sSysCzydh[7];//操作员代号
extern char sSysCzymc[9];//操作员名称
extern long nSysCzyqx;//操作员权限
extern char sSysCzyqx[21];//操作员权限汉字解释
extern long nSysCzylb;//操作员类别：0：代理（默认），1：综合网  dlyw.ini:czylb=1;
extern char sSysDsHost[41]; // 数据库守护进程主机
extern char sSysDsService[41]; // 数据库守护进程端口号
extern char sSysManager[7];//代理业务表所在用户，缺省为DLZ
extern char sSysPassword[41];//代理业务表所在用户的口令，缺省为DDDD
extern char sSysDlywHost[41]; // 代理业务守护进程主机
extern char sSysDlywService[41]; // 守护进程端口号
extern long nSysPcType;          //类别：0：代理前台；1：通讯机
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
  
  #ifdef DLYWLIB    //与综合网混编
  nSysCzylb=1;
  strcpy(sUserName,zjusername);
  strcpy(sPassWord,zjpassword);
  #else             //代理系统作为SHELL调用
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
  
  //登录数据库
  nRetVal=LoginDatabase();

  if(nRetVal<=0)
  {
        if(nRetVal<0)
        {
             nRetVal=MessageBox("无法登录数据库，想要得到这方面的帮助吗？",0);
             if(nRetVal==KEY_YES)
             {
                clrscr();
                ShowHelp("dlyw.hlp",NULL/*"我该怎么办?"*/);
             }     
        }     
  	Finished();
  	exit(0);
  }

  strcpy(sSysCzydh,sUserName); 
  //取得代理系统参数
  if(GetDlywInfo()<=0)
  {
        CloseDatabase();
        outtext("\n取得代理系统信息出错...");
  	Finished();
        exit(0);
  }

  CommitWork();

  dlywxt();     //代理业务系统入口
  CloseDatabase();  
  Finished();
  exit(0);

}



/*===========================================================
函数：LoginDatabase
目的：登录数据库
说明：登录数据库的方法是根据全局变量nSysCzylb来决定的，而该
      全局变量的值是由配置文件dlyw.ini中的配置项czylb来决定。
      默认为：0；
      
      登录过程：
      
      首先发起任务取得DLDJ数据库中DLZ的口令并DES解密。
      
      当nSysCzylb=1时，是综合网操作员。
        [1] 以该工号登录本地（dlyw）数据库，查询综合网的人员
            信息表和单位信息表(gg$ryxx,gg$dwxx)，取出操作员
            名称(cxm)、综合网局号(gg$ryxx.cdwdm)、操作员权限
            (cgzlbbm)，和局所名称(cdwmc)。
            
        [2] 以DLZ工号登录地区局(dldj)数据库，先查看局号表中
            是否有本综合网局号所对应代理局号，(dl$jh.czhwjh,
            dl$jh.cjh)，若没有，则插入局号局号记录。
        
        [3] 查询出代理局号，然后在操作员表中查询该局号中是否
            有本操作员的记录，如果有则进行第[5]步，如果没有则
            查看其它局是否有本操作员的工号，如果有，则表明工号
            重复，不予进入。
            
        [4] 如果操作员表没有本操作员的工号，则自动在操作员表
            中加入本操作员的记录。
        
        [5] 刷新本操作员的信息，提取操作员的其它信息，以业务
            操作员身份进入系统。        
     
      当nSysCzylb=0时，是代理操作员
            以DLZ工号登录地区局(dldj)数据库，查看操作员表中
            是否有该操作员的记录；如果有，则比较密码是否一
            致(dl$czyb.cpassword)，若一致则取得系统统信息并
            进入系统。
返回：>0成功，<0失败,=0:表示用户放弃登录            
*==========================================================*/
static long LoginDatabase()
{
   
   long nRetVal=0,nDlZjjh=2;
   char sDlywPassWord[17];
   char sNewJsdh[10],sbuf[200];
   long nAutoRegiste=0;
      
   clrscr();
   if(nSysCzylb!=1) outtext("\n正在进行初始化...");

   if(AutoRunTask("dldj",TASK_GET_VALUE_FROM_FILE,
                 "%s%s%s","%ld%s",
                 "dlywd.ini","password=","dddd",
                 &nRetVal,sDlywPassWord))
   {
        printf("\n取得代理系统密钥出错:%s",sTcpipErrInfo);
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
   if(nSysCzylb!=1) //代理
   {
        if(ConnectDatabase(sSysManager,sSysPassword)<0)
        {
               outtext("\n无法连接地区局服务器，请通知系统管理员！");
               keyb();
               return -1;	
        }
        
        UpgradeProg();  //检查是否需要升级

        nRetVal=3;
        while(nRetVal--)
        {
    	    clrscr();
  	    if(sUserName[0]=='\0')
  	    {
  	       if(GetValue(30,9,"请输入工号:%6s",sUserName)==KEY_ESC)
  	       {
  	       	  CloseDatabase();
  	          return 0;
  	       }
  	    }
  	
    	    if(sPassWord[0]=='\0')
    	    {
    	       if(GetValue(30,11,"请输入口令:%8p",sPassWord)==KEY_ESC)
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
   }//代理系统登录完毕
   else  //综合网
   {
	/***********************
        strcpy(sSysDsHost,"cent");
        if(ConnectDatabase(sUserName,sPassWord)<0)
        {
            MessageBox("    无法连接数据库，请首先检查bkfxd是否已启动，如果您使用的"
            "是代理系统的操作员，请删除配置文件dlyw.ini中czylb=1的配置项...",1);
            return -1;
        }
        
        //查询综合网操作员信息
        nRetVal=RunSelect("select cxm,gg$ryxx.cdwdm,cgzlbbm,cdwmc \
                           from gg$ryxx,gg$dwxx where gg$ryxx.cdwdm=gg$dwxx.cdwdm \
                           and  cgzlbbm in ('YYY' ,'YYBZZ','ZJZ','ZJSUPER') \
                           and cgh=%s into %s%s%s%s",
                           sUserName,sSysCzymc,sSysJsdh,sSysCzyqx,sSysJsmc);
        
        CloseDatabase();//关闭本地数据库链接
        if(nRetVal<=0)
        {
            outtext("\n人员信息表中没有您的信息或查询出错...");
            keyb();
            return -1;
        } 
	******************************/
	strncpy(sSysCzymc,getenv("HZCZYMC"),9);
	strncpy(sSysJsdh,getenv("HZJSDH"),9);
	strcpy(sSysJsmc,"综合网支局");
	
        strcpy(sSysDsHost,"dldj"); //链接地区局数据库
  
        if(nSysCzylb!=1) outtext("\n正在链接数据库...");
        if(ConnectDatabase(sSysManager,sSysPassword)<0)
        {
            outtext("\n无法连接地区局服务器，请通知系统管理员！");
            return -1;	
        }

        UpgradeProg();  //检查是否需要升级

        alltrim(sSysJsdh);
        RunSelect("select to_number(ccs) from dl$dlywcs where nbh=8 into %ld",
                  &nAutoRegiste);
        //查找对应的代理局号
        nRetVal=RunSelect("select cjh from dl$jh where czhwjh=%s and nlb=4 into %s",
                           sSysJsdh,sSysJsdh);
                           
        if(nRetVal<=0)
        {
            
            outtext("\n找不到综合网局号'%s'对应的代理局号,"
                    "正在插入对应的局号...",sSysJsdh);
            //查找出综合网局号所对应的代理局号
        
            nRetVal=RunSelect("select cjh from dl$jh where nlb=3 and \
                               substr(czhwjh,1,4)=substr(%s,1,4) into %s",
                               sSysJsdh,sNewJsdh);
            if(nRetVal<=0)
            {
                CloseDatabase();
                outtext("\n找不到综合网局号'%s'对应的代理县局局号,"
                         "请通知系统维护员加入该局号的对应...",sSysJsdh);
                keyb();
                return -1;
            }
            
            
            RunSelect("select max(to_number(substr(cjh,7,3))) from dl$jh \
                       where substr(cjh,1,6)=substr(%s,1,6) and nlb=4 into %ld",
                       sNewJsdh,&nDlZjjh);
            sprintf(sNewJsdh+6,"%03ld",nDlZjjh+1);
            printf("\n新分配局号：%s-->%s",sNewJsdh,sSysJsdh);

            //插入局号记录
            nRetVal=RunSql("insert into dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) \
                            values(%s,%s,%s,4,'0000000000','.')",
                            sNewJsdh,sSysJsmc,sSysJsdh);
            if(nRetVal<=0)
            {
            	CloseDatabase();
            	HiError(nRetVal,"新增局号表出错!");
            	return -1;
            } 
            CommitWork();
            MessageBox("您所在的支局号在代理系统中已经登记成功，若需要办理"
            "业务请通知系统维护员开放您局的使用权限",1);
            strcpy(sSysJsdh,sNewJsdh);
            clrscr();
            
        }
        nSysCzyqx=3; //综合网操作员的限权为业务操作员
        nRetVal=RunSelect("select * from dl$czyb where cczydh=%s",
                          sUserName);
        if(nRetVal<=0) //没有该操作员信息，自动加入
        {
            if(!nAutoRegiste)
            {
                MessageBox("    您的工号在地区局服务器上没有登记，请通知系统管理员增加"
                           "您的工号或开启综合网操作员自动登记功能。",1);
            	return -1;
            }

            nRetVal=RunSql("insert into dl$czyb(cczydh,cczymc,nczyqx,cpassword,\
                            cjh,cbl,nbl,dbl,clxbz) values(%s,%s,3,%s,%s,\
                            '综合网操作员',0,0,'00000000000000000000')",
                            sUserName,sSysCzymc,sPassWord,sSysJsdh);
            if(nRetVal<0)
            {
                CloseDatabase();
            	outtext("\n操作员表中无法新增您的工号，请通知系统管理员...");
            	keyb();
            	return -1;
            }
            //RunSql("update dl$jh set cjm=%s and cjh=%s",sSysJsmc,sSysJsdh);
            CommitWork();
            return 1; //successed
        }
        else  //有该操作员工号
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
            CloseDatabase(); //操作员工号重复（其它局的工号与操作员一致）
            outtext("\n操作员工号重复，此工号不能登录...");
            keyb();
            return -1;
        }
   }//end else 综合网   
}
/*======================================================
函数：UpgradeProg
目的：程序升级
说明：
      程序版本存放在dl$dlywcs中
      nbh            意义
      11              dlyw程序的版本号
      12              dlyw.hlp文本的版本号
======================================================*/

void UpgradeProg()
{
    double dNewDlywVer=0.0,dNewHelpVer=0.0;
    long nflag,nRetVal;
    char smsg[256];
     
    if(nSysCzylb!=1) outtext("\n正在检查版本信息...");
     
    RunSelect("select to_number(ccs) from dl$dlywcs where nbh=11 into %lf",
               &dNewDlywVer);

    RunSelect("select to_number(ccs) from dl$dlywcs where nbh=12 into %lf",
               &dNewHelpVer);
    
    if(dNewHelpVer>dHelpVer)
    {
         outtext("\n正在更新帮助文档文件dlyw.hlp，请耐心等待...");
         ReplaceFile("dldj","dlyw.hlp");
    }
    
    
    if(dNewDlywVer>dDlywVer)
    {
        sprintf(smsg,
        "欢迎使用成都鸿志公司开发的代理业务系统,您正在使用的系统版本为%.2lf，"
        "现在已经有新版本，请按确认键开始升级，或按其它键放弃登录...",dDlywVer);
        nRetVal=MessageBox(smsg,0);
        clrscr();
        if(nRetVal!=KEY_YES)
        {
            CloseDatabase();
            Finished();
            exit(0);
        }    
            
       	#ifdef DOS
        outtext("\n正在更换程序DLYWDOS.EXE，请耐心等待...");
        nflag=ReplaceFile("dldj","dlywdos.exe");
        #else
        outtext("\n正在更换程序dlyw，请耐心等待...");
        nflag=ReplaceFile("dldj","dlyw");
        #endif
        switch(nflag)
        {
          case -9:
            strcpy(smsg,"无法连接到远程主机“dldj”...");
            break;
          case 0:
            strcpy(smsg,"系统已成功升级，并将立即退出程序以便使用新的系统...");
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
函数：GetDlywInfo
功能：取得代理系统信息和操作员信息
返回：>0成功，<0否则失败,=0系统需要升级
*======================================================*/

static long GetDlywInfo()
{
    
    char * sCzyqx[7]={"超级用户","系统维护","代理组长",
                      "业务操作员","查询统计","帐务","支局长"};
    long nRetVal;

    if(nSysCzylb!=1) outtext("\n正在取得代理系统信息...");
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
函数: ReadDlywIni
目的: 从与主程序同名的ini文件中读取初始化设置
参数: 无
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
