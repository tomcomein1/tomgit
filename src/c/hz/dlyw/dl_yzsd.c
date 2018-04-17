#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <time.h>
#include "netmain.h"
#include "dl_pay.h"
#include "newtask.h"

#define MAX_DLYW_TIME 40

extern char sSysJsdh[10];       //本支局代号
extern char sRemoteHost[16];    //数据源地址
extern long nSysLkrq;
extern long nSysSfm;

static long dsRecv185AndSendDlyw(int yzsock);
static void dsRunQueryDlyw(int nsock,long ncxms,char * inbuf);
static void dsRunPayDlyw(int nsock,long ncxms,char * inbuf);

void yzsdmain()
{
  struct sockaddr_in yzaddr;
  int yzlen;
  int yzsock;
  int yzuser;
  
  printf("\nYzsd_userd started...\n");
  signal(SIGHUP,SIG_IGN);
  yzuser=passiveTCP("yzsd",10);
  
  for(;;) 
  {
        yzlen=sizeof(yzaddr);
        yzsock=accept(yzuser,(struct sockaddr *)&yzaddr,&yzlen);
        if(yzsock<0) continue;
        dsRecv185AndSendDlyw(yzsock);
        //close(yzsock);
  }//end for

}

static long dsRecv185AndSendDlyw(int yzsock)
{
	char sBuffer[1000];
	long nPkglen,nLength;
	char * sOffset,sHand[10];
	long ncxms,nFlag;
	
	memset(sBuffer,'\0',sizeof(sBuffer));
		
	//SetWaitingTime(MAX_DLYW_TIME);
	
	nLength=GetStringFromSocket(yzsock,sBuffer);

	printf("\nTEST:SOCKET=%d,nLength=%ld\n",yzsock,nLength);
	printf("\nTEST:RECV_BUF[%ld]=%s\n",strlen(sBuffer),sBuffer);
	if(!sBuffer) 
	{
	   write(yzsock,"0001|收到包信息出错|",20);
	   return -1;
	}
	
//	sOffset=FetchStringToArg(sBuffer,"%d|",&nPkglen);
//	printf("\nTEST:fist_field=%ld\n",nPkglen);
	sOffset=FetchStringToArg(sBuffer,"%d|%d|%s|",&nFlag,&ncxms,sHand);

	printf("\nTEST:FLAG=%ld,SJYM=%ld,JYLX=%s\n",nFlag,ncxms,sHand);
    upper(sHand);
    
    if(!strcmp(sHand,"QUERY") )
    	dsRunQueryDlyw(yzsock,ncxms,sOffset);
    else if(!strcmp(sHand,"PAY") )
    	dsRunPayDlyw(yzsock,ncxms,sOffset);
	else if(!strcmp(sHand,"CANCELL") )
		;//dsRunCancellDlyw(yzsock,ncxms,sOffset);
    else return -1;
 
	return 0;
}

static void dsRunQueryDlyw(int nsock,long ncxms,char * inbuf)
{
   	char sYhbz1[23],sCzydh[10],sLsh[41];
   	char sJym[15],sPacket[257],outbuf[1000];
   	char sWt[6]="00000",sJkny[60]="\0",sTemp[20];
   	long nWt=1,iLoop,nJls=0;
   	double dSjk=0.0;
   	int nPkglen=0;
   	char sRet[7];//返回代码
	RECORD   record;
	YWMX rYwmx[YWMX_ELEMENT_LEN+1];
	long nBl3[YWMX_ELEMENT_LEN+1];
	long nBl4[YWMX_ELEMENT_LEN+1];

	FetchStringToArg(inbuf,"%s|%s|%f|%s|",sLsh,sYhbz1,&dSjk,sCzydh);
	printf("\nTEST:LSH=%s,YHBZ=%s,SJK=%lf,CZY=%s\n",sLsh,sYhbz1,dSjk,sCzydh);
	
	RunSelect("select cjym from dl$jym where ncxms=%ld into %s",ncxms,sJym);
	if(GenerateLsh(1,sSysJsdh,sJym,sLsh)==0)  return;
    CommitWork(); 
	
	sWt[nWt-1]='1';
	
	if(ncxms==821)
    {
    	sprintf(sPacket,
            "%-40.40s|%5.5s|%-22.22s|%-6.6s|",
            sLsh,sWt,sYhbz1,sCzydh);
    }
    else
    {
        sprintf(sPacket,
            "%-40.40s|%5.5s|%-20.20s|%-6.6s|",
            sLsh,sWt,sYhbz1,sCzydh);
    }
    printf("\nTEST:PACKET[%ld]=%s\n",strlen(sPacket),sPacket);
	outtext(" 正在查询信息,请稍侯......                    ");
 
	if(AutoRunTask(sRemoteHost,TASK_PAY_QUERY,"%s","%r",sPacket,&record)<0)
	{      
		HintError(TASK_ERR,"查询用户信息网络出错!"); 
		return;
	}
	
	memset(sPacket,'\0',sizeof(sPacket));
	ExportRecord(&record,"%s",sRet);
	alltrim(sRet);
	if(strcmp(sRet,"000000")==0)
	{
		ExportRecord(&record,"%s%s%lf%ld",rYwmx[0].cYhmc,rYwmx[0].cYhbz1,&rYwmx[0].dBl2,&nJls);
		for(iLoop=1;iLoop<nJls;iLoop++)
		{
			strcpy(rYwmx[iLoop].cYhmc,rYwmx[0].cYhmc);
			rYwmx[iLoop].dBl2=rYwmx[0].dBl2;
		}
		for(iLoop=0;iLoop<nJls;iLoop++)
		{
			ExportRecord(&record,"%s%ld%ld%lf[25]%lf%ld(4)%s(4)",
				rYwmx[iLoop].cYhbz,&rYwmx[iLoop].nYsnd,&rYwmx[iLoop].nYsyf,
				rYwmx[iLoop].dSfmx,&rYwmx[iLoop].dYjzk,&rYwmx[iLoop].nBl1,
				&rYwmx[iLoop].nBl2,&nBl3[iLoop],&nBl4[iLoop],rYwmx[iLoop].cBl1,
				rYwmx[iLoop].cBl2,rYwmx[iLoop].cBl3,rYwmx[iLoop].cBl4);
			
			sprintf(sTemp,"%ld%02ld,",
					rYwmx[iLoop].nYsnd,rYwmx[iLoop].nYsyf);
			strcat(sJkny,sTemp);
		}
		DropRecord(&record);
	}
	else
	{
		ExportRecord(&record,"%s",sPacket);
		printf("\nTEST:%s\n",sPacket);
		DropRecord(&record);
		return;
	}

	memset(outbuf,'\0',sizeof(outbuf));
	GetSysLkrq();

	sprintf(outbuf,"0000|%ld|QUERY|%s|%s|%.2lf|%s|%s|%8ld|%6ld|",
				ncxms,sLsh,rYwmx[iLoop].cYhbz,rYwmx[0].dYjzk,sCzydh,sJkny,nSysLkrq,nSysSfm);
				
	//strcpy(outbuf,"0000|130|QUERY|2711010000100300112711012003091200436086|13808109370|120.00|DL185|200307|20030912|160000|");
	nPkglen=strlen(outbuf);
	sprintf(outbuf,"%04ld|%s",nPkglen,outbuf);
	printf("\nTEST:outbuf[%ld]=%s\n",strlen(outbuf),outbuf);
	SetWaitingTime(MAX_DLYW_TIME);
    SendStringToSocket(nsock,outbuf);
	//close(nsock);
	return;
}

static void dsRunPayDlyw(int nsock,long ncxms,char * inbuf)
{
	char sYhbz1[23],sCzydh[10],sLsh[41];
   	char sJym[15],sPacket[257],outbuf[1000];
   	char sWt[6]="00000",sJkny[60]="\0",sTemp[20];
   	char sDate[60];
   	double dSjk=0.0;

	FetchStringToArg(inbuf,"%s|%s|%f|%s|",sLsh,sYhbz1,&dSjk,sCzydh);

	/*sprintf(sPacket,"%-40.40s|%s|%s|2|%.2lf|%s|%ld|%ld|%s|%s|",
			sLsh,sYhbz1,sYhmc,
			dSjk,sCzydh,nSysSfm,nJkyfs,sDate);*/
}
