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
#include "yatelecom.h"
#include "filechar.h"

static long dsRecvDlywAndSendYatele(int nsock);
static void StrReturnStr(char * buf,yadx_Pkg * p,long nFlag);

void yatelemain()
{
  struct sockaddr_in yaaddr;
  int yalen;
  int yasock;
  int yauser;
  
  printf("\nYAuserd started...\nYA固定电话通信进程启动完毕...\n");
  signal(SIGHUP,SIG_IGN);  //SIG_IGN忽略参数signum所指的信号|SIG_DFL恢复^^^^
  yauser=passiveTCP("telecom",10);
  
  for(;;) 
  {
        yalen=sizeof(yaaddr);
        yasock=accept(yauser,(struct sockaddr *)&yaaddr,&yalen);
        if(yasock<0) continue;
        dsRecvDlywAndSendYatele(yasock);
        close(yasock);
  }//end for
  
}//end b2imain 

static long dsRecvDlywAndSendYatele(int nsock)
{
    yadx_Pkg sendp,recvp;
    char inbuf[YADX_PKG_LEN+1];
    char outbuf[YADX_PKG_LEN+2048];
    char sFp[2048]="\0";
    long nLength;
    int dxsock;
	
   SetWaitingTime(MAX_DLYW_TIME);
   if(GetLongFromSocket(nsock,&nLength)) return 1;
   
   memset(inbuf,0,YADX_PKG_LEN+1);
   
   if(GetOneRecordFromSocket(nsock,inbuf,(int)nLength)!=(int)nLength)
   {
	 SendLongToSocket(nsock,-2);
	 return -1;
   }
   printf("\nTEST:inbuf[%ld]=%s\n",nLength,inbuf);
   memset(&sendp,'\0',YADX_PKG_LEN);
 
   StrReturnStr(inbuf,&sendp,1);

   SetWaitingTime(MAX_MOB_TIME);
   dxsock=connectTCP("dl_gs_4","tele_recv");
   printf("\nTET:dxsock=%d|send date to dx...\n",dxsock);
   if(dxsock<0)
   {
   	SendLongToSocket(nsock,-2);
   	return -1;
   }
   
   printf("TEST:JYFS=%d\n",sendp.jyfs);
   if(SendRecordToSocket(dxsock,(char *)&sendp,YADX_PKG_LEN))
   {
     printf("SEND  PKG DEFALUT\n");
     SendLongToSocket(nsock,-3);
     close(dxsock);
     return -1;
   }
   printf("\ndxsock=%d,PKG_LEN=%ld\n",dxsock,sizeof(sendp));
   memset(&recvp,'\0',YADX_PKG_LEN);

   SetWaitingTime(MAX_MOB_TIME);
   nLength=GetOneRecordFromSocket(dxsock,&recvp,YADX_PKG_LEN);
   if(nLength<=0)
   {
	printf("RECV PKG DEFALUT\n");
	close(dxsock);
   	SendLongToSocket(nsock,-3);
   	return -1;
   }

   printf("\nTEST.recv date for second..\n");
   if(recvp.lsh!=YADX_PKG_LEN)
   {
	memset(sFp,'\0',strlen(sFp));
    	nLength=ReadSocket(dxsock,sFp,(int)(recvp.lsh-YADX_PKG_LEN));
    	if(nLength<=0)
    	{
   	SendLongToSocket(nsock,-3);
   	return -1;
    	}
   }
   alarm(0);
   close(dxsock);
   
   memset(outbuf,0,YADX_PKG_LEN+2048);
   StrReturnStr(outbuf,&recvp,0);
   strcat(outbuf,"#");
   strcat(outbuf,sFp);
   nLength=strlen(outbuf);
   //printf("\nTEST:outbuf[%ld]=%s\n",strlen(outbuf),outbuf);

   SetWaitingTime(MAX_DLYW_TIME);
   SendLongToSocket(nsock,0);
   SendLongToSocket(nsock,nLength);
   SendRecordToSocket(nsock,outbuf,nLength);
   
   return 0;
}


static void StrReturnStr(char * buf,yadx_Pkg * p,long nFlag)
{
	if(nFlag)
	{
		FetchStringToArg(buf,
		"%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%f|%f|%f|%s|%d|%s|%d|%d|%d|%s|%s|%f|",
		&p->lsh,p->zh1,p->zh2,p->mm1,p->mm2,p->sfzh,p->dwbh,p->sfzsy,p->sffsy,p->yhm,
		&p->qfhj,&p->dsfhj,&p->znj,p->bz,&p->jyfs,p->dsjg,&p->sfrq,&p->sfsj,&p->czy,p->xym,
		p->xymms,&p->zje);
		if(p->lsh==0) p->lsh=YADX_PKG_LEN;
	}
	else
	{
		char yhm[17];
		char* str;
		strcpy(yhm,p->yhm);
		printf("用户名:%s",yhm);
		if(str=strchr(yhm, '['))
		{
			*str = 0;
			*(str+1)=0;
		} 
		printf("\n用户名:%s", yhm);
		//替换p->yhm为yhm
		sprintf(buf,
		"%ld|%s|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%lf|%lf|%s|%ld|%s|%ld|%ld|%ld|%s|%s|%lf|",
		p->lsh,p->zh1,p->zh2,p->mm1,p->mm2,p->sfzh,p->dwbh,p->sfzsy,p->sffsy,yhm,
		p->qfhj,p->dsfhj,p->znj,p->bz,p->jyfs,p->dsjg,p->sfrq,p->sfsj,p->czy,p->xym,
		p->xymms,p->zje);
	}
	printf("\n%ld|%s|%s|%s|%s|%s|%s|%s|%s|%s|%lf|%lf|%f|%s|%d|%s|%ld|%ld|%d|%s|%s|%lf|\n",
		p->lsh,p->zh1,p->zh2,p->mm1,p->mm2,p->sfzh,p->dwbh,p->sfzsy,p->sffsy,p->yhm,
		p->qfhj,p->dsfhj,p->znj,p->bz,p->jyfs,p->dsjg,p->sfrq,p->sfsj,p->czy,p->xym,
		p->xymms,p->zje);
}
