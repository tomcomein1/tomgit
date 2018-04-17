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


void telemain()
{
	printf("\nTeleuserd started...\n中国电信数据通信进程启动完毕...\n");
	dlsrvd("telecom");  
}

long dsRecvDlywAndSendTele(int telesock)
{
	char * sbuf;
	char sBuffer[10000];
	char sId[50]=".";
	int  nsock;
	long nLength;

	SetWaitingTime(MAX_DLYW_TIME);
	
//	if(GetStringFromSocket(telesock,sId)) return 1;
	if(GetLongFromSocket(telesock,&nLength)) return 1;
	if(!(sbuf=(char*)malloc(nLength+1))) return 1;
	memset(sbuf,0,nLength+1);
	if(GetOneRecordFromSocket(telesock,sbuf,(int)nLength)!=(int)nLength)
	{
		free(sbuf);
		return -1;
	}
	memset(sBuffer,'\0',sizeof(sBuffer));

	SetWaitingTime(MAX_MOB_TIME*2);
	nsock=connectTCP("dl_gs_4","tele_recv");
	
//	printf("\n /%s.../nsock=%ld \n",sId,nsock);
	
	if(nsock<0)
	{
		SendLongToSocket(telesock,-2);
		free(sbuf);
		return -1;
	}

//	printf("\n %s...dlsrv send \n",sId);

	if(SendRecordToSocket(nsock,sbuf,(int)nLength))
	{
		SendLongToSocket(telesock,-3);
		free(sbuf);
		close(nsock);
		return -1;
	}
	free(sbuf);
	
//	if(strncmp(sId,"account",7)==0)
		alarm(3600);
//	else alarm(40);
	
	nLength=read(nsock,sBuffer,10000);
	alarm(0);
	close(nsock);
	if(nLength<=0)
	{
		SendLongToSocket(telesock,-3);
		return -1;
	}
   
//	printf("\n %s...dlsrv read \n",sId);

	SetWaitingTime(MAX_DLYW_TIME);
	SendLongToSocket(telesock,0);
	SendLongToSocket(telesock,nLength);
	SendRecordToSocket(telesock,sBuffer,nLength);
	return 0;
}
