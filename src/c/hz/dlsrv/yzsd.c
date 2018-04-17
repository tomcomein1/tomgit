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
#include "newtask.h"
#include "tasktool.h"
#include "dlpublic.h"
#include "filechar.h"

#define YWMX_ELEMENT_LEN 20
int dsRecv185AndSendDlyw(int nsock);
static long dsRunQueryDlyw(char * inbuf,long ncxms,char * outbuf);
static long dsRunPayDlyw(char * inbuf,long ncxms,char * outbuf);
long GenerateLsh(long nLb,char *cjh,char *cjym,char *clsh);
static char sSysJsdh[10]="271101999";
extern long nSysLkrq;
extern long nSysSfm;

void yzsdmain()
{
  
  //struct sockaddr_in yzaddr;
  //int yzlen;
  //int yzsock;
  //int yzuser;
  
  printf("\nYzsd started...\n邮政速递通信进程启动完毕...\n");
  dlsrvd("yzsd");
  /*signal(SIGHUP,SIG_IGN);
  yzuser=passiveTCP("yzsd",10);
  
  for(;;) 
  {
        yzlen=sizeof(yzaddr);
        yzsock=accept(yzuser,(struct sockaddr *)&yzaddr,&yzlen);
      	if (yzsock <0 ) continue;
      	dsRecv185AndSendDlyw(yzsock);
	close(yzsock);
  }//end for
  */
}

int dsRecv185AndSendDlyw(int nsock)
{
	char buf[513],* offset;
	char outbuf[513]="\0",sHand[10];
	char sBuffer[513]="\0";
	int nLength=256,nLen,nFlag,ncxms;
	int i,n;
	
	printf("\nSOCKET=%d\n",nsock);
	memset(buf,0,strlen(buf));

	SetWaitingTime(2);
	n=ReadSocket(nsock,buf,nLength);
	alarm(0);
	if(n<=0)
		return n;
	
	if(nSysLkrq>20040731)
	{
		strcpy(outbuf,"1144|0|0|0|0|0|0|0|0|0|#");
		n=strlen(outbuf)+4;
		sprintf(sBuffer,"%d|%s",n,outbuf);
		SetWaitingTime(4);
		n=write(nsock,sBuffer,nLength*2);
		return 0;
	}//user time ctrl@
	//alltrim(buf);
	offset=FetchStringToArg(buf,"%d|",&nLen);
	buf[nLen]='\0';
	printf("Recv_buf[%ld]=%s\n",strlen(buf),buf);
	
	offset=FetchStringToArg(offset,"%d|%d|%s|",&nFlag,&ncxms,sHand);
	alltrim(sHand);

        for(i=0;i<2;i++)
         if(Dlsrv_ConnectDatabase()==0)
         {
       	  	printf("\n连接数据库失败,等待10秒种...");
           	fflush(stdout);
           	sleep(10);
         }
         else break;

	if(i>=2) {printf("\nERROR: Connection default!~"); 
		return -1; 
	}

	if(!strcmp(sHand,"QUERY") )
          if(dsRunQueryDlyw(offset,ncxms,outbuf)){
	     sprintf(sBuffer,"%s|0|0|0|0|0|0|0|0|0|#",outbuf); 
	     strcpy(outbuf,sBuffer);
	   }
	if(!strcmp(sHand,"PAY")  )
	  if(dsRunPayDlyw(offset,ncxms,outbuf)){
	     sprintf(sBuffer,"%s|0|0|0|0|0|0|0|0|0|#",outbuf); 
	     strcpy(outbuf,sBuffer);
	   }
	
	n=strlen(outbuf)+4;
	sprintf(sBuffer,"%d|%s",n,outbuf);
	printf("\nSocket=%d\nSend_buffer[%s]\n",nsock,sBuffer);

	SetWaitingTime(4);
	n=write(nsock,sBuffer,nLength*2);
   	CloseDatabase();
	
	printf("\nSuccessful:Send_byte=%d\n",n);
	return 0;
}

static long dsRunQueryDlyw(char * inbuf,long ncxms,char * outbuf)
{
	char sJym[16],sPacket[513];
   	char sYhbz[23],sCzydh[10],sLsh[41];
   	double dSjk;
   	long iLoop,nJls=0;
   	int nPkglen=0;
   	char sRet[7],sJkny[60]="\0",sTemp[20],sPol[120]="\0",temp[60];
	RECORD   record;
	YWMX rYwmx[YWMX_ELEMENT_LEN+1];
	long nBl3[YWMX_ELEMENT_LEN+1];
	long nBl4[YWMX_ELEMENT_LEN+1];

	
	FetchStringToArg(inbuf,"%s|%s|%f|%s|",sLsh,sYhbz,&dSjk,sCzydh);
	
	RunSelect("select cjym from dl$jym where ncxms=%ld into %s",ncxms,sJym);
	if(GenerateLsh(1,sSysJsdh,sJym,sLsh)==0)  return -1;
	CommitWork(); 
	alltrim(sYhbz);
	
    	if(ncxms==821)
    	{
    	  sprintf(sPacket,"%-40.40s|100000|%-22.22s|%-6.6s|",
    		sLsh,sYhbz,sCzydh);
    	}
    	else
    	{
          sprintf(sPacket,"%-40.40s|10000|%-20.20s|%-6.6s|",
            	sLsh,sYhbz,sCzydh);
    	}//end if
    	
    	printf("\nNow is query message,please waiting......                    ");
 	if(AutoRunTask("dldj",TASK_PAY_QUERY,"%s","%r",sPacket,&record)<0)
	{      
		printf("\nERROR:查询网络出错!");
		strcpy(outbuf,"1114");
		return -1;
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
		sprintf(sPol,"%ld|%.2lf|!",nJls,rYwmx[0].dBl2);
		for(iLoop=0;iLoop<nJls;iLoop++)
		{
			ExportRecord(&record,"%s%ld%ld%lf[25]%lf%ld(4)%s(4)",
				rYwmx[iLoop].cYhbz,&rYwmx[iLoop].nYsnd,&rYwmx[iLoop].nYsyf,
				rYwmx[iLoop].dSfmx,&rYwmx[iLoop].dYjzk,&rYwmx[iLoop].nBl1,
				&rYwmx[iLoop].nBl2,&nBl3[iLoop],&nBl4[iLoop],rYwmx[iLoop].cBl1,
				rYwmx[iLoop].cBl2,rYwmx[iLoop].cBl3,rYwmx[iLoop].cBl4);
		
			sprintf(sTemp,"%04ld%02ld,",
					rYwmx[iLoop].nYsnd,rYwmx[iLoop].nYsyf);
			strcat(sJkny,sTemp);
			if((iLoop>0) && (ncxms==821))
			{
				temp[0]='\0';
				sprintf(temp,"%s|%ld|%ld|%.2lf|%s|",
					rYwmx[iLoop].cYhbz,rYwmx[iLoop].nBl2,rYwmx[iLoop].nBl1,
					rYwmx[iLoop].dYjzk,rYwmx[iLoop].cBl4);
				strcat(sPol,temp);
			}//endif
		}
		DropRecord(&record);
	}
	else
	{
		ExportRecord(&record,"%s",sPacket);
		printf("\nError_number:%s\n",sPacket);
		DropRecord(&record);
		strcpy(outbuf,sRet+2);
		return -1;
	}
	memset(outbuf,'\0',sizeof(outbuf));
	GetSysLkrq(); 
	
	sprintf(outbuf,"0000|%ld|QUERY|%s|%s|%.2lf|%s|%s|%8ld|%6ld|%s|#",
			ncxms,sLsh,rYwmx[0].cYhbz,rYwmx[0].dYjzk,sCzydh,sJkny,nSysLkrq,nSysSfm,rYwmx[0].cYhmc);
       
    if(ncxms==821)
    {
    	strcat(sPol,"%");
		printf("Year_month:%s,Policy_message:%s\n",sJkny,sPol);
    	strcat(outbuf,sPol);
    }       
    return 0;
}

static long dsRunPayDlyw(char * inbuf,long ncxms,char * outbuf)
{
 	char sbuf[513];
	char sPacket[513],sJym[16];
	char sLsh[41],sYhbz[23],sCzydh[7],sDate[80],sYhmc[81];
	double dSjk;
	long nLkrq,nSfm,nJkyfs=0;
  
	FetchStringToArg(inbuf,"%s|%s|%f|%s|%s|%d|%d|",
		sLsh,sYhbz,&dSjk,sCzydh,sDate,&nLkrq,&nSfm);

	RunSelect("select cjym from dl$jym where ncxms=%ld into %s",ncxms,sJym);
	RunSelect("select cyhmc,nysyf from dl$ywmx_%t where clsh=%s \
		into %s%ld",sJym,sLsh,sYhmc,&nJkyfs);

	sprintf(sPacket,"%-40.40s|%s|%s|2|%.2lf|%s|%ld|%ld|%s|0|",
					sLsh,sYhbz,sYhmc,
					dSjk,sCzydh,nSysSfm,nJkyfs,sDate);
	//printf("\nSend_Packet=%s\n",sPacket);
	if(RunSelect("select ROWID from dl$zwmx where clsh=%s and substr(clxbz,19,1)='0' ",sLsh)>0)
	{
		strcpy(outbuf,"0000|0|0|0|0|0|0|0|0|0|#"); 
		return 0;
	}//检查已如帐记录,返回成功标志

	if(AutoRunTask("dldj",TASK_PAY_CONFIRM,"%s","%s",sPacket,sbuf)<0)
	{      
		printf("\nERROR:缴费网络出错!");
		strcpy(outbuf,"8888");
		return -1;
	}

	printf("\nsbuf=%s\n",sbuf);
	if(strncmp(sbuf,"000000",6))
	{
		printf("\nTask pay is ERROR:\n"); 
		strncpy(outbuf,sbuf+2,4);
		return -1;
	}
	
	strcpy(outbuf,"0000|0|0|0|0|0|0|0|0|0|#"); 
	return 0;

}

long GenerateLsh(long nLb,char *cjh,char *cjym,char *clsh)
{
  long nRetVal,nValue;
  char sSequence[41];

  GetSysLkrq();
  switch(nLb)
  {
    case 1:
    case 2:
         strcpy(sSequence,"DLNBLSH");
         break;
    case 3:
         sprintf(sSequence,"DLWTLSH");
         break;
  }

  nRetVal=RunSelect("SELECT %t.nextval FROM dual INTO %ld",sSequence,&nValue);
  if(nRetVal<=0)
  {
        printf("从序列%t取数出错",sSequence);
	return 0;
  }//endif     
  switch(nLb)
  {
    case 1:
         sprintf(clsh,"%-9.9s%-15.15s%08ld%08ld",cjh,cjym,nSysLkrq,nValue);
         break;
    case 2:
         sprintf(clsh,"%-9.9s%08ld%08ld",cjh,nSysLkrq,nValue);
         break;
    case 3:
         sprintf(clsh,"%08ld",nValue);
         break;
  }
  return 1;  

}
