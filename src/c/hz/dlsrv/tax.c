/*---------------------------------------------------------------------------
 *国税通信模块 
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
#include "mobile.h"
#include "netmain.h"


void taxmain()
{
	printf("\nTaxuserd started...\n国税数据通信进程启动完毕...\n");
	dlsrvd("tax");
}


long dsRecvDlywAndSendTax(int taxsock)
{

	long nLength;
	char * sbuf;
	char sBuffer[2048];
	char sJylx[9],sLen[7];
	char sFileName[20];
	int nsock,nflag;
   
	FILE *fp;
   
	SetWaitingTime(MAX_DLYW_TIME);
	if(GetLongFromSocket(taxsock,&nLength)) return 1;
   
//   printf("\n test:%ld",nLength);
   
	if(!(sbuf=(char*)malloc(nLength+1))) return 1;
	if(GetOneRecordFromSocket(taxsock,sbuf,(int)nLength)!=(int)nLength)
	{
		free(sbuf);
		return -1;
	}
	memset(sBuffer,'\0',sizeof(sBuffer));
	sbuf[nLength]='\0';

	printf("\n dlywdin:%ld,%s",taxsock,sbuf);
   
	SetWaitingTime(MAX_MOB_TIME);
	nsock=connectTCP("dl_gs_10","tax_recv");
   
	printf("\n out:%ld,%s",nsock,sbuf);
   
	if(nsock<0)
	{
		SendLongToSocket(taxsock,-2);
		free(sbuf);
		close(nsock);
		return -1;
	}

	if(SendRecordToSocket(nsock,sbuf,nLength))
	{
		SendLongToSocket(taxsock,-3);
		free(sbuf);
		close(nsock);
		return -1;
	}
	free(sbuf);

	if(GetOneRecordFromSocket(nsock,sBuffer,14)!=14)
	{
		SendLongToSocket(taxsock,-3);
		close(nsock);
		return -1;
	}

	printf("\n in:%ld,%s",nsock,sBuffer);
   
	FetchStringToArg(sBuffer,"%8s%6s",sJylx,sLen);
	nLength=atol(sLen);
   
	if(GetOneRecordFromSocket(nsock,sBuffer,nLength)!=nLength)
	{
		SendLongToSocket(taxsock,-3);
		close(nsock);
		return -1;
	}
   
	printf("\n in:%ld,%ld,%s",nsock,nLength,sBuffer);
   
	/*strcpy(sFileName,"test");
	if(!(fp=fopen(sFileName,"wb")))
	{
		RollbackWork();
		return -1;
	}
	fprintf(fp,"\n%s\n",sBuffer);
	fclose(fp);*/
   
	SetWaitingTime(MAX_DLYW_TIME);
	SendLongToSocket(taxsock,0);
	SendStringToSocket(taxsock,sBuffer);
	close(nsock);
	return 0;
}
