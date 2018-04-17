// 用法：
//     dlywd [端口号] 启动服务进程，缺省端口号 dlyw
//     dlywd stop [端口号] 停止服务进程 缺省端口号 dlyw
// 编程：李宝东
// 日期：2000.2.22


#include "dlywd.h"
#include "netmain.h"
#include "task.h"
#include "newtask.h"

extern char sTcpipErrInfo[81];
extern long nSysSfm;
double  dDsVersion = 1.00;
double  dVersion=3.00;         //
long nSysLkrq=0;           
long nSysSfm=0;
long nMakeDate=20010530;
long nSysDebug;
long nMaxConnectNum=50;
long nMaxDelayTime=3000;  
char LKHOST[21];
char LKSVRNAME[21];
char sDefaultUser[7];
char sDefaultPass[17];
char sSysDsHost[21];
char sSysDsService[21];
char sSysDlywdPath[31];        //work path of dlywd
char sSysJsdh[10];
char sSysJh[10];
int g_nMaxWaitingTime = MAX_WAITING_TIME;
int g_nMinWaitingTime = MIN_WAITING_TIME;
int g_nBpType = 1; // 寻呼通讯类型
char* g_sBuffer = NULL;

PROCESSTAB childtab[MAX_CONNECT_NUM];  //子进程表

/*------------------------------------------------------------------------
 * main - Concurrent TCP server for DLYW service
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
  char service[21] = "dlyw";  /* service name or port number  */
  char shellcom[30];
  struct sockaddr_in fsin;  /* the address of a client  */
  int alen;     /* length of client's address */
  int msock;      /* master server socket   */
  int ssock;      /* slave server socket    */
  long  nTaskNo;
  long nRetVal;
  long nEncryptKey=0;
  long nposition=0;

  //确定DLYWD的工作目录
  char * str;
  strcpy(sSysDlywdPath,argv[0]);
  str=sSysDlywdPath;
  while(strchr(str,'/')) str++;
  str[0]='\0';
  if(strlen(sSysDlywdPath)==0) strcpy(sSysDlywdPath,"./");

  memset(childtab,0,sizeof(childtab));


  printf("DLYWD start... \n");
  printf("鸿志电子公司代理专用后台服务进程启动,版本号: %.2lf 制作日期: %ld\n",dVersion,nMakeDate);
  
  ReadIni();
  if(argc > 1)  strcpy(service, argv[1]);

  //将进程转移到后台以免阻塞
  switch (fork())
  {
    case 0:		/* child */
      break;
    case -1:
      errexit("fork: %s\n", strerror(errno));
    default:	/* parent */
      exit(0);
  }

  (void) signal(SIGCLD, cleanup);
  signal(SIGHUP,SIG_IGN);
      
  msock = passiveTCP(service, QLEN);

  if(access("log",0))
  {
    mkdir("log",S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);
    system("chmod 777 log");
  } 
  if(access("account",0))
  {
    mkdir("account",S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);
    system("chmod 777 account");
  } 
  while (1) 
  {
    signal(SIGCLD,cleanup);
    alen = sizeof(fsin);
    ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
    if (ssock < 0)  continue;
    
    //得到一个子进程登记表空间，非阻塞
    nposition=GetChildPosition(); 
    if(nposition<0)
    {
    	close(ssock);
    	continue;
    }

    if(childtab[nposition].pid=fork())
    {
    	close(ssock); 
    	childtab[nposition].nStartTime=nSysSfm;
        if(nSysDebug)
           printf("\nchild %ld created with pid %d and starting time is %ld",
                 nposition,childtab[nposition].pid,childtab[nposition].nStartTime);
    	continue;
    }
    close(msock);

    #ifdef CONNECT_IDENTIFICATION
    {
      unsigned char one, two, three, four;
      long nKeyOne, nKeyTwo, nKeyThree, nKeyFour;      
      char* addr;
      addr = (char*)&fsin.sin_addr.s_addr;
      one = (char)(*addr++);
      two = (char)(*addr++);
      three = (char)(*addr++);
      four = (char)(*addr);
      nKeyOne = one;
      nKeyTwo = two;
      nKeyThree = three;
      nKeyFour = four;

      if(SendToSocket(ssock,"%ld %ld %ld %ld", nKeyOne, nKeyTwo, nKeyThree, nKeyFour)) break;

      // 接收密钥
      if(GetLongFromSocket(ssock,&nTaskNo)) break;

      if(nTaskNo != (one + 81) * (one + 81) + (two + 81) * (two + 81) + 
        (three + 81) * (three + 81) + (four + 81) * (four + 81) + 
        (one / 81 + 1) * (two / 81 + 1) * (three / 81 + 1) * (four / 81 + 1)) break;
    }
    #endif

    // 接收是否退出标志
    if(GetLongFromSocket(ssock,&nTaskNo)) break;
    if(SendLongToSocket(ssock, 1)) break;
    if(SendDoubleToSocket(ssock, dDsVersion)) break;
    // 接收是否符合版本要求标志
    if(GetLongFromSocket(ssock,&nTaskNo)) break;
    // 0 不符合要求 1 符合要求
    if (nTaskNo == 0) break;
        
    while(1)
    {
       SetWaitingTime(g_nMaxWaitingTime);
       if(GetLongFromSocket(ssock,&nTaskNo))
       {
            close(ssock);
            errexit("\nerror[%d]: %s\n", errno,strerror(errno));
       }     

       switch(nTaskNo)
       { 
          case TASK_CONNECTDATABASE:
   	      GetDlzPassword();
              nRetVal=ConnectDatabase(sDefaultUser,sDefaultPass);
              if(SendLongToSocket(ssock,nRetVal)||nRetVal<0) break;
              continue;

          case TASK_AUTORUNTASK:
              dsTaskAutoRunTask(ssock,nTaskNo);
              continue;

          case TASK_QUERY_REMOTE:
              dsTaskQueryRemote(ssock,nTaskNo);
              break;

          case TASK_RUN_REMOTESQL:
              dsTaskRunSql(ssock,nTaskNo);
              break;

          case TASK_OPEN_REMOTECURSOR:
              dsTaskOpenRemoteCursor(ssock,nTaskNo);
              break;

          case TASK_OPEN_FILECURSOR:
              dsTaskOpenFileCursor(ssock,nTaskNo);
              CloseDatabase();
              exit(0);
              break;
              
          case TASK_CLOSEDATABASE:
              CloseDatabase();
              break;

          case TASK_NULL:
              break;
          default:
              continue;
       }//end switch
       break;
    }//end while任务接收循环
    break;
  }//end 后台狩护循环
  close(ssock);
  exit(0);            
}

/*==========================================================
函数：cleanup()
目的：去除僵死进程，并回收进程表空间
===========================================================*/
void cleanup(int sig)
{
    int status,pid;
    long nLoop;
    
    while((pid=waitpid(-1,&status,WNOHANG))>0) 
    {
       if(nSysDebug)
          printf("\ndlywd child process %d terminated with %d",pid,status);
       for(nLoop=0;nLoop<nMaxConnectNum;nLoop++)
       {
    	if(childtab[nLoop].pid==pid)
    	{
    		childtab[nLoop].pid=0;
    		childtab[nLoop].nStartTime=0;
                break;
    	}
      }
    }  
}

/*==========================================================
函数：ReadIni()
目的：取得配置文件dlywd.ini信息，配置条目有:
      dshost=BKFXD数据库解释进程和主机地址或别名[dldj]
      dsservice=BKFXD数据库解释进程的端口号[bkfx]
      debug=调试语句是否开始[1:开启  0：关闭]
      maxconnect=DLYWD的最多同时连接的数目[10~200]
      maxtime=DLYWD进程最多停留时间(mmss)[500~6000]
      password=DLZ用户的密码
===========================================================*/
long ReadIni()
{
  nMaxConnectNum=50;
  nMaxDelayTime=3000;
  strcpy(sDefaultUser, "DLZ");
  strcpy(sDefaultPass, "dddd");
  strcpy(sSysDsHost,"dldj");
  strcpy(sSysDsService,"bkfx");
  strcpy(LKHOST,"tbhost");
  strcpy(LKSVRNAME,"tbsrv");
  nSysDebug=0;
   
  GetValueFromFile("dlywd.ini","dshost=%s",sSysDsHost);
  GetValueFromFile("dlywd.ini","dsservice=%s",sSysDsService);
  GetValueFromFile("dlywd.ini","debug=%ld",&nSysDebug);
  GetValueFromFile("dlywd.ini","maxconnect=%ld",&nMaxConnectNum);
  GetValueFromFile("dlywd.ini","maxtime=%ld",&nMaxDelayTime);
  GetValueFromFile("dlywd.ini","tbhost=%s",LKHOST);
  GetValueFromFile("dlywd.ini","tbservice=%s",LKSVRNAME);
  
  if(nMaxConnectNum<MIN_CONNECT_NUM||nMaxConnectNum>MAX_CONNECT_NUM)
     nMaxConnectNum=50;
  
  if(nMaxDelayTime<MIN_DELAY_TIME||nMaxDelayTime>MAX_DELAY_TIME)
     nMaxDelayTime=3000;

  return 0;
}

/*==========================================================
函数：GetDlzPassword()
目的：取得DLZ密码，如果不是DDDD则将密文解密
===========================================================*/
long GetDlzPassword()
{
  char sNewDefaultPass[81];
  GetValueFromFile("dlywd.ini","password=%s",sNewDefaultPass);
 
  //解密password 
  if(strlen(sNewDefaultPass)==16)
  {
	  memset(sDefaultPass,'\0',sizeof(sDefaultPass));
	  DlPasswordDec(sNewDefaultPass,sDefaultPass);
  }	  
  else    strcpy(sDefaultPass,sNewDefaultPass);
  return 0;
}

/*==========================================================
函数：GetChildPosition()
目的：先去除超时的进程（>nMaxDelayTime);并分配一个子进程表空间，
返回：>=0成功，该值表示进程表中被分配的进程表空间的下标变量
===========================================================*/
long GetChildPosition()
{
  long nLoop,nDelay,noccupy;
  
  GetSysLkrq();
  
  for(nLoop=0;nLoop<nMaxConnectNum;nLoop++)
  {
  	nDelay=abs(nSysSfm-childtab[nLoop].nStartTime);
  	if(childtab[nLoop].pid>0 && nDelay>nMaxDelayTime)
  	{
  	    kill(childtab[nLoop].pid,SIGKILL);
            if(nSysDebug)
  	       printf("\ndlywd child process %d killed by it's parent "
  	              "because of timeout.", childtab[nLoop].pid);
  	    childtab[nLoop].pid=0;
  	    childtab[nLoop].nStartTime=0;
  	}
  }	    

  for(nLoop=0;nLoop<nMaxConnectNum;nLoop++)
  	if(!childtab[nLoop].pid) return nLoop;
  
  nDelay=999999;
  for(nLoop=0;nLoop<nMaxConnectNum;nLoop++)
  {
  	if(childtab[nLoop].nStartTime<nDelay)
  	{
  	    nDelay=childtab[nLoop].nStartTime;
  	    noccupy=nLoop;
  	}
  }

  kill(childtab[noccupy].pid,SIGKILL);
  if(nSysDebug)
       printf("\ndlywd child process %d killed by it's parent.", childtab[noccupy].pid);
  childtab[noccupy].pid=0;
  childtab[noccupy].nStartTime=0;
  return noccupy;	
}


