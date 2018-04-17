/*---------------------------------------------------------------------------
 * 寻呼通信模块 
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

long dsRecvDlywAndSendBp(int bpsock);

void bpmain()
{
  //struct sockaddr_in bpaddr;
  //int bplen;
  //int bpsock;
  //int bpuser;
  
  printf("\nBpuserd started...\n寻呼数据通信进程启动完毕...\n");
  dlsrvd("bpuser");
  /*signal(SIGHUP,SIG_IGN);
  bpuser=passiveTCP("bpuser",10);
  
  for(;;) 
  {
        bplen=sizeof(bpaddr);
        bpsock=accept(bpuser,(struct sockaddr *)&bpaddr,&bplen);
        if(bpsock<0) continue;
        dsRecvDlywAndSendBp(bpsock);
  }//end for
  */
}


long dsRecvDlywAndSendBp(int bpsock)
{

   long nLength;
   char * sbuf;
   char sBuffer[256];
   int nsock,nflag;
   
   SetWaitingTime(MAX_DLYW_TIME);
   if(GetLongFromSocket(bpsock,&nLength)) return 1;
   if(!(sbuf=(char*)malloc(nLength+1))) return 1;
   if(GetOneRecordFromSocket(bpsock,sbuf,(int)nLength)!=(int)nLength)
   {
   	free(sbuf);
        close(bpsock);
   	return -1;
   }
   memset(sBuffer,'\0',sizeof(sBuffer));
   sbuf[nLength]='\0';

   SetWaitingTime(MAX_MOB_TIME);
   nsock=connectTCP("dl_gs_2","bp_recv");
   if(nsock<0)
   {
   	SendLongToSocket(bpsock,-2);
   	free(sbuf);
        close(nsock);
        close(bpsock);
   	return -1;
   }

   if(SendRecordToSocket(nsock,sbuf,nLength))
   {
   	SendLongToSocket(bpsock,-3);
   	free(sbuf);
        close(bpsock);
        close(nsock);
   	return -1;
   }
   free(sbuf);

   if(GetRecordFromSocket(nsock,sBuffer,'#'))
   {
   	SendLongToSocket(bpsock,-3);
        close(bpsock);
        close(nsock);
   	return -1;
   }

   SetWaitingTime(MAX_DLYW_TIME);
   SendLongToSocket(bpsock,0);
   SendStringToSocket(bpsock,sBuffer);
   close(bpsock);
   close(nsock);
   return 0;
}

