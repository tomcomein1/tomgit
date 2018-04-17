/*---------------------------------------------------------------------------
 * 移动代收费通信模块
 * original programmer:陈博   [chenbo]
 * date:2001.09.18
 * 说明：本通讯模块采用异步方式与移动进行通信，通过select调用和超时控制技术来
 *       达到非阻塞的循环服务器效果。
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


MOBILEMSG process[MAX_PROCEDURE];         //交换登记表
unsigned long processid;                  //进程号
long nSysTime;                            //系统时间

void mobilemain()
{
  struct sockaddr_in dladdr;
  struct sockaddr_in mobaddr; 
  int dllen,moblen;                       /* length of client's address */
  int dlsock,mobsock;                     /* master server socket   */
  int dlyw,mobile;                        /* passive socket number connectted by dlywd or mobile */
  int nfds,nret;
  struct timeval waittime;
  fd_set rfds;

  signal(SIGHUP,SIG_IGN);
 
  
  dlyw=passiveTCP("mobile",5);           /* create passive SAP(server access point) for dlywd  */
  mobile=passiveTCP("dlyw_recv",5);      /* create passive SAP(server access point) for mobile server */
  
  printf("\nMobiled started...\n移动服务网数据交换进程启动完毕...\n");

  processid=1;
  memset(process,0,sizeof(process));
  memset(&waittime,0,sizeof(waittime));
  nfds=getdtablesize();

  FD_ZERO(&rfds);
  for(;;) 
  {

       FD_SET(dlyw,&rfds);
       FD_SET(mobile,&rfds);
        
       alarm(0);
       waittime.tv_sec=2;
       nret=select(nfds,&rfds,(fd_set *)0,(fd_set *)0,&waittime);
       
       CleanupSpace();
       if(nret<0) continue;
       
       //代理方请求发送数据
       if(FD_ISSET(dlyw,&rfds))
       {
            dllen=sizeof(dladdr);
            dlsock=accept(dlyw,(struct sockaddr *)&dladdr,&dllen);
            if(dlsock<0) continue;
            dsRecvDlywAndSendMob(dlsock);
       
       }
       //移动方返回数据
       if(FD_ISSET(mobile,&rfds))
       {
            moblen=sizeof(mobaddr);
            mobsock=accept(mobile,(struct sockaddr *)&mobaddr,&moblen);
            if(mobsock<0) continue;
            dsRecvMobAndSendDlyw(mobsock);
       }
  }//end for
}

static long dsRecvDlywAndSendMob(int dlsock)
{
  long nOpt,nLength,npkgCount,nSpace,nLoop;
  long nsendpkg;
  int  nOutSocket;
  unsigned short sequence,datalen;
  char sSendAdd[10];    //发起地标识，由移动给出
  char * sOperCode[9]={"900001","900002","100012",   //交易码
                       "200010","210010","600001",
                       "100013","400010","100011"};

  MOBILEPACKAGE pkg;

  memset((void *)&pkg,0,252);
  SetWaitingTime(MAX_DLYW_TIME);
  
  if(GetFromSocket(dlsock, "%ld%s%ld",&nOpt,sSendAdd,&nLength))
  {
  	 SendToSocket(dlsock,"%ld%s",0,"8101");
  	 close(dlsock);
	 return 1;
  }
  if(GetStringFromSocket(dlsock,pkg.datatrans))
  {
  	 SendToSocket(dlsock,"%ld%s",0,"8101");
  	 close(dlsock);
  	 return 1;
  }
 
  nSpace=getptablespace();
  if(nOpt!=TASK_MOBILE_PAY_CONFIRM && nSpace<0)  //最后缴费确认不接收返回包
  {
       SendToSocket(dlsock,"%ld%s",0,"8006");
       close(dlsock);
       return 1;
  }

  //连接移动通信服务
  nOutSocket=connectTCP("dl_gs_3","mobile_recv");
 
  if(nOutSocket<0)
  {
  	SendToSocket(dlsock,"%ld%s",0,"8001");
        close(dlsock);
  	return 1;
  }

  /////////////////请求包赋值////////////////////////  
  processid++;
  processid%=10000000;
  datalen=(unsigned short)(nLength); 
  pkg.morepkt=(nOpt!=TASK_MOBILE_PAY_COMPARE)?'0':'1';
  pkg.pktype='1';                         //数据包
  pkg.unitend='1';                        //此类已结束
  pkg.save='\0';                          //未定义
  pkg.sequence=htons(1);                  //包序号[网络字节序]
  pkg.length=htons(datalen);              //包有效数据长度[网络字节序]
  pkg.msgtype=htonl(processid);           //进程号[网络字节序]
  memcpy(pkg.code,sOperCode[nOpt-TASK_MOBILE_SIGN_IN],6);//交易码
  memcpy(pkg.d_add,"110223300",9);        //目的标识
  memcpy(pkg.l_add,sSendAdd,9);           //发起地标识

  ///////////将数据包发送给移动通讯机////////////
  
  SetWaitingTime(MAX_MOB_TIME);
 		
  if(SendRecordToSocket(nOutSocket,(char *)&pkg,252))
  {
  	SendToSocket(dlsock,"%ld%s",0,"8001");
        close(nOutSocket);
        close(dlsock);
  	return 1;
  }
  
  ////////////////发送明细帐文件//////////////////
  if(nOpt==TASK_MOBILE_PAY_COMPARE)
  {
     sequence=1;
     if(GetLongFromSocket(dlsock,&npkgCount)||npkgCount<=0)
     {
       SendToSocket(dlsock,"%ld%s",0,"8001");
       close(nOutSocket);
       close(dlsock);
       return 1;
     }      
     
     for(nsendpkg=1;nsendpkg<=npkgCount;nsendpkg++)
     {
       GetLongFromSocket(dlsock,&nLength);
       GetOneRecordFromSocket(dlsock,pkg.datatrans,nLength);

       sequence++;
       datalen=(unsigned short)nLength;
       pkg.morepkt=(nsendpkg<npkgCount)?'1':'0'; 
       pkg.pktype='3'; 
       pkg.unitend=(nsendpkg<npkgCount)?'0':'1';
       pkg.sequence=htons(sequence);          //包序号[网络字节序]
       pkg.length=htons(datalen);             //包有效数据长度[网络字节序]

       //将明细文件包发往移动通讯机 
       SetWaitingTime(MAX_MOB_TIME);
       if(SendRecordToSocket(nOutSocket,(char *)&pkg,252))
       {
           SendToSocket(dlsock,"%ld%s",0,"8001");
           close(nOutSocket);
           close(dlsock);
           return 1;
       }      
     }//end for
  }//end if
  close(nOutSocket);

  if(nOpt==TASK_MOBILE_PAY_CONFIRM)  //最后缴费确认不接收返回包
  {
  	close(dlsock);
  	return 0;   
  }
  
  GetNowTime();
  /* cleanup zombie slot */
  for(nLoop=0;nLoop<MAX_PROCEDURE;nLoop++)
     if(process[nLoop].processid && process[nLoop].socket==dlsock) 
     	process[nLoop].processid=0;

  /* registe table slot */
  process[nSpace].processid=processid;
  process[nSpace].socket=dlsock;
  process[nSpace].starttime=nSysTime;

  return 0;

}

static long dsRecvMobAndSendDlyw(int mobsock)
{

  char sAnswer[TOTAL_PACKAGE_SIZE];
  char * sOffset;
  long nLoop;
  unsigned int sequence=1;
  MOBILEPACKAGE pkg;
  FILE *fp;
  
  
  memset(sAnswer,0,TOTAL_PACKAGE_SIZE);
  sOffset=sAnswer;

  for(;;)
  {
        SetWaitingTime(MAX_MOB_TIME);
        memset(&pkg,0,sizeof(MOBILEPACKAGE));
  	if(GetOneRecordFromSocket(mobsock,(char *)&pkg, 252)<=0)
  	{
                close(mobsock);
                return 1;
  	}	
        pkg.length=ntohs(pkg.length);	
        pkg.sequence=ntohs(pkg.sequence);
        pkg.msgtype=ntohl(pkg.msgtype);
  	memcpy(sOffset,pkg.datatrans,pkg.length);
  	sOffset+=pkg.length;
      
        printf("\npkg.more=%c\npkg.unitend=%c\npkg.type=%c\
                \npkg.seq=%d\npkg.length=%ld\npkg.data=%s\npkg.id=%ld\n",
                pkg.morepkt,pkg.unitend,pkg.pktype,
                pkg.sequence,pkg.length,pkg.datatrans,pkg.msgtype);
        
        if(sequence<pkg.sequence-1)
        {
                strcpy(sAnswer,"8004");
                break;
        }
        sequence=pkg.sequence;

        if(pkg.morepkt=='0'&&pkg.unitend=='1')    break; //接收完毕

        if((sOffset-sAnswer)>(TOTAL_PACKAGE_SIZE-216))
        {
                strcpy(sAnswer,"8005");
                break;
        }         	
  }//end for

  close(mobsock);
	
  /* 将接收到的数据发给DLYWD */
  for(nLoop=0;nLoop<MAX_PROCEDURE;nLoop++)
  {
  	if((process[nLoop].processid==pkg.msgtype) && pkg.msgtype)
  	{
                /* 
                   A registered table slot matched.then,send the data
                   which received from mobile to dlywd server, and
                   clean up the registered table slot.
                */
  		process[nLoop].processid=0;
                SetWaitingTime(MAX_DLYW_TIME);
  		SendLongToSocket(process[nLoop].socket,0);
                SendStringToSocket(process[nLoop].socket,sAnswer);
          	close(process[nLoop].socket);
                process[nLoop].socket=0;
  		return 0;
  	}
  }
  /* no registered table slot matched */
  return -1;

}


static long getptablespace()
{

  long nLoop;
  
  for(nLoop=0;nLoop<MAX_PROCEDURE;nLoop++)
  	if(!process[nLoop].processid) return nLoop;

  return -1;

}

static void CleanupSpace()
{
  long nLoop,nDelay;
  
  GetNowTime();
  
  for(nLoop=0;nLoop<MAX_PROCEDURE;nLoop++)
  {
  	nDelay=abs(nSysTime-process[nLoop].starttime);
  	if(process[nLoop].processid>0 && nDelay>MAX_DELAY_TIME*6)
  	{
            SendToSocket(process[nLoop].socket,"%ld%s",0,"8007");
  	    close(process[nLoop].socket);
  	    process[nLoop].processid=0;
  	}
  }	    
}

void GetNowTime()
{
        struct tm now_time;
        time_t now;
        time(&now);
        now_time=*localtime(&now);
        nSysTime=now_time.tm_hour*3600+now_time.tm_min*60+now_time.tm_sec;
}

