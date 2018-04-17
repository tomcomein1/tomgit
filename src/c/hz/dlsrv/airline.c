//与民航的通信服务进程
//original programmer : chenbo

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
#include "dlsrv.h"
#include "pgw.h"
#include "mobile.h"


extern long nSysTime;

static void InsertChar(char * str,char ch);
long dsRecvDlywAndSendAir(int airsock);
static void dsProcessAirData(char * data);


void airmain()
{
  //struct sockaddr_in airaddr; 
  //int air,airlen;                         /* master server socket   */
  //int airsock;
  //long nRetVal;
     
  printf("\n与中国民航通信服务进程启动完毕...\n");
  dlsrvd("air");
  /*air = passiveTCP("air",5);
  for(;;) 
  {
        airlen=sizeof(airaddr);
        airsock=accept(air,(struct sockaddr *)&airaddr,&airlen);
        if(airsock<0) continue;
        nRetVal=dsRecvDlywAndSendAir(airsock);
        if(nRetVal!=SUCCESS) SendLongToSocket(airsock,nRetVal);
        close(airsock);
  }//end for
  */
}

long dsRecvDlywAndSendAir(int airsock)
{
	
   int  session,retcode,nbyte;
   long begintime;
   char buf[4096],sendbuf[1024],recvbuf[4096];
   char * offset;

   alarm(MAX_DLYW_TIME);
   if(GetStringFromSocket(airsock,buf))
   {
     	pgw_close(session);
   	return -11;
   }	
   offset = buf;     

   retcode = pgw_init();
   if(retcode!= SUCCESS) return retcode;

   retcode = pgw_open("dl_gs_8",&session);
   if(retcode!= SUCCESS) return retcode;

   /*每条发送的字符串以'\n'分隔*/

   while(strchr(offset,'\n'))
   {
   	memset(sendbuf,'\0',sizeof(sendbuf));
   	nbyte=0;
   	
   	while(offset[0]!='\n')
   	{
   	    sendbuf[nbyte]=offset[0];
   	    offset++;
   	    nbyte++;
   	}
   	
   	offset++;    
        retcode = pgw_write(session,sendbuf,&nbyte);
        printf("\nSEND:%s",sendbuf);
        fflush(stdout);
        if(retcode!=SUCCESS)
        {
       	    pgw_close(session);
       	    return retcode;
        }   	

        //reading
        memset(recvbuf,'\0',sizeof(recvbuf));
        GetNowTime();
        begintime=nSysTime;
    
        for(;;)
        {
            nbyte=4000;
            retcode = pgw_read(session,recvbuf,&nbyte);
            
            if(retcode==SUCCESS) break;
            if(retcode==ERR_NO_DATA)
            {
                GetNowTime();
                if(abs(nSysTime-begintime)>20)
                {
        	    pgw_close(session);
                    printf("\nTimeouted after 20 seconds..");
                    fflush(stdout);
        	    return -12;
                }
                continue;
            }
            pgw_close(session);
            return retcode;
    	}//end for
   }//end for
   
   pgw_close(session);
   dsProcessAirData(recvbuf);
   SendLongToSocket(airsock,0L);
   SendStringToSocket(airsock,recvbuf);
   printf("\nRECV:%s",recvbuf);
   fflush(stdout);
   return SUCCESS;
}

static void dsProcessAirData(char * data)
{
    
     char * offset;
     long count;
     
     offset=data;
     
     count=0;
     while(offset[0])
     {
     	if(offset[0]==0x0d)
     	{
     	     count=0;
     	     offset[0]='\n';
     	}
     	else if((unsigned char)offset[0]<' ') 
     	{
     	     offset[0]=' ';
     	     count++;
     	}
     	else count++;
     	offset++;
     	if(count==80)
     	{
     	     count=0;
     	     InsertChar(offset,'\n');
     	     offset++;
     	}
     }	     
}

static void InsertChar(char * str,char ch)
{
     char * offset;
     
     offset = str;
     while(offset[0]) offset++;
     offset++;
     
     while(offset>str)
     {
     	offset[0]=offset[-1];
     	offset--;
     }
     str[0]=ch;
}
