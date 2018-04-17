/*---------------------------------------------------------------------------
 * ���Žӿ�ͨ��ģ�� 
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
PROCESSTAB childtab[MAX_CONNECT_NUM];  //�ӽ��̱�
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
    
		//�õ�һ���ӽ��̵ǼǱ�ռ䣬������
		nposition=GetChildPosition(); 
		if(nposition<0)
		{
			close(nsock);
			continue;
		}
		
		//������
		if(childtab[nposition].pid=fork())
		{
			close(nsock); 
			childtab[nposition].nStartTime=nSysSfm;
			//if(nSysDebug)
			printf("\nchild %ld created with pid %d and starting time is %ld\n",
					nposition,childtab[nposition].pid,childtab[nposition].nStartTime);
			continue;
		}
		
		//�ӽ���
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
������GetChildPosition()
Ŀ�ģ���ȥ����ʱ�Ľ��̣�>nMaxDelayTime);������һ���ӽ��̱�ռ䣬
���أ�>=0�ɹ�����ֵ��ʾ���̱��б�����Ľ��̱�ռ���±����
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
������cleanup()
Ŀ�ģ�ȥ���������̣������ս��̱�ռ�
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


