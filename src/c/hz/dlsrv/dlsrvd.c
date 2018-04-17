/*---------------------------------------------------------------------------
 * 电信接口通信模块 
 *---------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <time.h>
#include "netmain.h"
#include "mobile.h"

typedef struct processtab{
                          pid_t  pid;
                          long   nStartTime;
                         }PROCESSTAB;

#define MAX_CONNECT_NUM 200
long nMaxConnectNum=50;
long nMaxDelayTime=3000;
PROCESSTAB childtab[MAX_CONNECT_NUM];  //子进程表
extern void GetSysLkrq();
extern long nSysLkrq;
extern long nSysSfm;
static long GetChildPosition();
static void cleanup(int sig);


void dlsrvd(char *sDkmc)
{
	struct sockaddr_in addr;
	int nlen;
	int ssock;
	int nsock;
	
	long nposition=0,nRetVal;

	memset(childtab,0,sizeof(childtab));

	(void) signal(SIGCLD,cleanup);
	signal(SIGHUP,SIG_IGN);
	
	
	ssock=passiveTCP(sDkmc,10);
  
	for(;;) 
	{
		signal(SIGCLD,cleanup);
		nlen=sizeof(addr);
		nsock=accept(ssock,(struct sockaddr *)&addr,&nlen);
		if(nsock<0) continue;
    
		//得到一个子进程登记表空间，非阻塞
		nposition=GetChildPosition(); 
		if(nposition<0)
		{
			close(nsock);
			continue;
		}
		
		//父进程
		if(childtab[nposition].pid=fork())
		{
			close(nsock); 
			childtab[nposition].nStartTime=nSysSfm;
			//if(nSysDebug)
			printf("\nchild %ld created with pid %d and starting time is %ld\n",
					nposition,childtab[nposition].pid,childtab[nposition].nStartTime);
			continue;
		}
		
		//子进程
		close(ssock);

		if(strcmp(sDkmc,"telecom")==0)
			dsRecvDlywAndSendTele(nsock);
		if(strcmp(sDkmc,"tax")==0)
			dsRecvDlywAndSendTax(nsock);
		else if(strcmp(sDkmc,"bpuser")==0)
			dsRecvDlywAndSendBp(nsock);
		//else if(strcmp(sDkmc,"policy")==0)
		//	dsRecvDlywAndSendBx(nsock);
		else if(strcmp(sDkmc,"yzsd")==0)
			dsRecv185AndSendDlyw(nsock);
		else if(strcmp(sDkmc,"power")==0)
			dsRecvDlywAndSendPower(nsock);
		else if(strcmp(sDkmc,"air")==0)
		{	
			nRetVal=dsRecvDlywAndSendAir(nsock);
		        if(nRetVal!=0) SendLongToSocket(nsock,nRetVal);
		}
		
		close(nsock);
		exit(0);
	}//end for

}

/*==========================================================
函数：GetChildPosition()
目的：先去除超时的进程（>nMaxDelayTime);并分配一个子进程表空间，
返回：>=0成功，该值表示进程表中被分配的进程表空间的下标变量
===========================================================*/
static long GetChildPosition()
{
	long nLoop,nDelay,noccupy;
  
	GetSysLkrq();
  
	for(nLoop=0;nLoop<nMaxConnectNum;nLoop++)
	{
		nDelay=abs(nSysSfm-childtab[nLoop].nStartTime);
		if(childtab[nLoop].pid>0 && nDelay>nMaxDelayTime)
		{
			kill(childtab[nLoop].pid,SIGKILL);
			//if(nSysDebug)
			printf("\ndlsrv child process %d killed by it's parent "
  	              "because of timeout. \n", childtab[nLoop].pid);
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
	//if(nSysDebug)
	printf("\ndlsrv child process %d killed by it's parent.\n", childtab[noccupy].pid);
	childtab[noccupy].pid=0;
	childtab[noccupy].nStartTime=0;
  	return noccupy;	
}


/*==========================================================
函数：cleanup()
目的：去除僵死进程，并回收进程表空间
===========================================================*/
static void cleanup(int sig)
{
	int status,pid;
	long nLoop;
    
	while((pid=waitpid(-1,&status,WNOHANG))>0) 
	{
       //if(nSysDebug)
		printf("\ndlsrv child process %d terminated with %d \n",pid,status);
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


