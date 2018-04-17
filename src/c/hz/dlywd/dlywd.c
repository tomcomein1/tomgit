// �÷���
//     dlywd [�˿ں�] ����������̣�ȱʡ�˿ں� dlyw
//     dlywd stop [�˿ں�] ֹͣ������� ȱʡ�˿ں� dlyw
// ��̣����
// ���ڣ�2000.2.22


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
int g_nBpType = 1; // Ѱ��ͨѶ����
char* g_sBuffer = NULL;

PROCESSTAB childtab[MAX_CONNECT_NUM];  //�ӽ��̱�

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

  //ȷ��DLYWD�Ĺ���Ŀ¼
  char * str;
  strcpy(sSysDlywdPath,argv[0]);
  str=sSysDlywdPath;
  while(strchr(str,'/')) str++;
  str[0]='\0';
  if(strlen(sSysDlywdPath)==0) strcpy(sSysDlywdPath,"./");

  memset(childtab,0,sizeof(childtab));


  printf("DLYWD start... \n");
  printf("��־���ӹ�˾����ר�ú�̨�����������,�汾��: %.2lf ��������: %ld\n",dVersion,nMakeDate);
  
  ReadIni();
  if(argc > 1)  strcpy(service, argv[1]);

  //������ת�Ƶ���̨��������
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
    
    //�õ�һ���ӽ��̵ǼǱ�ռ䣬������
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

      // ������Կ
      if(GetLongFromSocket(ssock,&nTaskNo)) break;

      if(nTaskNo != (one + 81) * (one + 81) + (two + 81) * (two + 81) + 
        (three + 81) * (three + 81) + (four + 81) * (four + 81) + 
        (one / 81 + 1) * (two / 81 + 1) * (three / 81 + 1) * (four / 81 + 1)) break;
    }
    #endif

    // �����Ƿ��˳���־
    if(GetLongFromSocket(ssock,&nTaskNo)) break;
    if(SendLongToSocket(ssock, 1)) break;
    if(SendDoubleToSocket(ssock, dDsVersion)) break;
    // �����Ƿ���ϰ汾Ҫ���־
    if(GetLongFromSocket(ssock,&nTaskNo)) break;
    // 0 ������Ҫ�� 1 ����Ҫ��
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
    }//end while�������ѭ��
    break;
  }//end ��̨����ѭ��
  close(ssock);
  exit(0);            
}

/*==========================================================
������cleanup()
Ŀ�ģ�ȥ���������̣������ս��̱�ռ�
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
������ReadIni()
Ŀ�ģ�ȡ�������ļ�dlywd.ini��Ϣ��������Ŀ��:
      dshost=BKFXD���ݿ���ͽ��̺�������ַ�����[dldj]
      dsservice=BKFXD���ݿ���ͽ��̵Ķ˿ں�[bkfx]
      debug=��������Ƿ�ʼ[1:����  0���ر�]
      maxconnect=DLYWD�����ͬʱ���ӵ���Ŀ[10~200]
      maxtime=DLYWD�������ͣ��ʱ��(mmss)[500~6000]
      password=DLZ�û�������
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
������GetDlzPassword()
Ŀ�ģ�ȡ��DLZ���룬�������DDDD�����Ľ���
===========================================================*/
long GetDlzPassword()
{
  char sNewDefaultPass[81];
  GetValueFromFile("dlywd.ini","password=%s",sNewDefaultPass);
 
  //����password 
  if(strlen(sNewDefaultPass)==16)
  {
	  memset(sDefaultPass,'\0',sizeof(sDefaultPass));
	  DlPasswordDec(sNewDefaultPass,sDefaultPass);
  }	  
  else    strcpy(sDefaultPass,sNewDefaultPass);
  return 0;
}

/*==========================================================
������GetChildPosition()
Ŀ�ģ���ȥ����ʱ�Ľ��̣�>nMaxDelayTime);������һ���ӽ��̱�ռ䣬
���أ�>=0�ɹ�����ֵ��ʾ���̱��б�����Ľ��̱�ռ���±����
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


