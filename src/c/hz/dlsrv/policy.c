/*---------------------------------------------------------------------------
 * 保险(B2I)通信模块
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
#include "policy.h"

long dsRecvDlywAndSendBx(int bxsock);

void b2imain()
{
  struct sockaddr_in bxaddr;
  int bxlen;
  int bxsock;
  int bxuser;
  
  printf("\nB2Iuserd started...\n中国人寿保险数据通信进程启动完毕...\n");
  //dlsrvd("policy");
  signal(SIGHUP,SIG_IGN);  //SIG_IGN忽略参数signum所指的信号|SIG_DFL恢复^^^^
  bxuser=passiveTCP("policy",10);
  for(;;) 
  {
        bxlen=sizeof(bxaddr);
        bxsock=accept(bxuser,(struct sockaddr *)&bxaddr,&bxlen);
        if(bxsock<0) continue;
        dsRecvDlywAndSendBx(bxsock);
        close(bxsock);
  }//end for
  
}//end b2imain 

long dsRecvDlywAndSendBx(int bxsock)
{
   char sendbuf[1024];
   struct Ctl_Msg sendc,recvc;
   data_Pkg recvp;
   int  nsock,i,acc_flag=0;
   long nLength;
           
   SetWaitingTime(MAX_MOB_TIME);
   nsock=connectTCP("dl_gs_9","policy_recv");

   for(;;)
   {
        SetWaitingTime(MAX_DLYW_TIME);
        if(GetLongFromSocket(bxsock,&nLength)) return 1;
        if(nLength==0)
 	    break;

        //get data for dlywd
        memset(sendbuf,'\0',strlen(sendbuf));   
        if(GetOneRecordFromSocket(bxsock,sendbuf,(int)nLength)!=(int)nLength)
           return -1;
		
		////////////请求包值///////////////////
		memset(&sendc,0,CTL_PKG_LEN);
		if(acc_flag==0) FetchStringToArg(sendbuf,"%6s",sendc.TransCode);
		else strcpy(sendc.TransCode,"500401");
		sprintf(sendc.MsgLength,"%ld",nLength); //包长度
		
		strcpy(sendc.status,"0000");
		if(!strcmp(sendc.TransCode,"500401") )
		{
			 sendc.CtlChar.flags.morepkt=1;
			 strcpy(sendc.status,"8385");
		}
		else sendc.CtlChar.flags.morepkt=0;
		sendc.CtlChar.flags.pktype=0;
	
	if(acc_flag) strcpy(sendc.status,"0000");
	//////////////////////////////////////   
	/*{
		FILE *fp;
		fp=fopen("policy.log","ab");
		fwrite((unsigned char *)&sendc,CTL_PKG_LEN,1,fp);
		fwrite(sendbuf,nLength,1,fp);
		fwrite("\n---\n",5,1,fp);
		fclose(fp);
	}*/

   		SetWaitingTime(MAX_MOB_TIME);
        if(SendRecordToSocket(nsock,(char *)&sendc,CTL_PKG_LEN))
        {
          SendLongToSocket(bxsock,-3);
          close(nsock);
   	  return -1;
        }
        
        //send data to dl_gs_9(BX COMPANY)
		//printf("\nTEST:sendbuf=%s\n",sendbuf);
        if(strcmp(sendbuf,"5004019999") ) 
        {
	 	  printf("\nTEST:CODE=%s,SENDBUF_LEN=[%ld]",sendc.TransCode,nLength);	
          if(SendRecordToSocket(nsock,sendbuf,(int)nLength))
          {
            SendLongToSocket(bxsock,-3);
            close(nsock);
			return -1;
          }
          	//////-------------//////////////////
          	if(!strcmp(sendc.TransCode,"500501"))
			{
			nLength=ReadSocket(nsock,&recvc,CTL_PKG_LEN);
			printf("\nrecv.status=%s\n",recvc.status);
			if(strcmp(recvc.status,"0000") )
			{
			  SendRecordToSocket(bxsock,recvc.status,4); //送状态
			  close(nsock);
			  return -1;
			}
			printf("\nTEST:sencond send Reacall_ctr_data\n");
			strcpy(sendc.status,"9999");
			if(SendRecordToSocket(nsock,(char *)&sendc,CTL_PKG_LEN))
		    {
		         SendLongToSocket(bxsock,-3);
		         close(nsock);
		   		 return -1;
		    }
			}//------------------------///////////////

	     }//endif

     	if(!strcmp(sendc.TransCode,"500401") ) 
     	{
		memset(&recvc,0,CTL_PKG_LEN);
        	nLength=ReadSocket(nsock,&recvc,CTL_PKG_LEN);
   		    SetWaitingTime(MAX_DLYW_TIME);
		    SendRecordToSocket(bxsock,recvc.status,4);
        	printf("\nrecv.status=%s\n",recvc.status);
        	if(!strcmp(recvc.status,"0000"))
        	{ 
          		acc_flag=1;
        		alarm(0);
        		continue;
        	}
           	else
        	{
        		close(nsock);
	    		return -2;
        	}//endifelse
     	}//for policy account
   }//end for
   
   if(!strcmp(sendc.TransCode,"500401") )
   {		 
     sendc.CtlChar.flags.morepkt=0;
     if(SendRecordToSocket(nsock,(char *)&sendc,CTL_PKG_LEN))
     {
        close(nsock);
   	  	return -1;
     }
     close(nsock);
     return 0;
   }
   //Get buf from BX(dl_gs_9)
   alarm(0);
   SetWaitingTime(MAX_MOB_TIME);
   for(i=1;i<3;i++)
   {
      nLength=ReadSocket(nsock,&recvc,CTL_PKG_LEN);
	  printf("\n...status=%s,mor=%d",recvc.status,recvc.CtlChar.flags.morepkt);
      if(recvc.CtlChar.flags.morepkt==0) break;
      if(strcmp(recvc.status,"0000") )  break;
      nLength=ReadSocket(nsock,&recvp,DAT_PKG_LEN);
      if(nLength==DAT_PKG_LEN) break;
   }
   printf("\nTEST:RECV_LEN=%ld;SOCK=%d;I=%d",nLength,nsock,i);
   
   alarm(0);
   close(nsock);  //关掉保险端口
    
    //以下是接收到的数据，写入bxrecv文件中
   /*	{
	FILE *fp;
 	fp=fopen("bxrecv","wb");
	fwrite((unsigned char *)recvc,CTL_PKG_LEN,1,fp);
	fwrite("\n---\n",5,1,fp);
	fwrite((unsigned char *)recvp,nLength,1,fp);
	fclose(fp);
   	}
   */
   //send data to dlywd
   SetWaitingTime(MAX_DLYW_TIME);
   SendLongToSocket(bxsock,0);
   SendRecordToSocket(bxsock,recvc.status,4);
   if(strcmp(recvc.TransCode,"500101") && nLength==DAT_PKG_LEN)
   		SendRecordToSocket(bxsock,(char *)&recvp,nLength);
   
   printf("\n.......SUCCESSFUL!\n");
   return 0;
}

