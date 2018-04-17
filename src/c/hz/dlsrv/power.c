/*---------------------------------------------------------------------------
 * 电费(POWER)通信模块
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

static unsigned long processid;                  //进程号
long dsRecvDlywAndSendPower(int psock);

void powermain()
{
  //struct sockaddr_in paddr;
  //int plen;
  //int psock;
  //int puser;
  
  printf("\nPoweruserd started...\n中国电费数据通信进程启动完毕...\n");
  dlsrvd("power");
  /*signal(SIGHUP,SIG_IGN);  //SIG_IGN忽略参数signum所指的信号|SIG_DFL恢复^^^^
 
  puser=passiveTCP("power",10);
  for(;;) 
  {
        plen=sizeof(paddr);
        psock=accept(puser,(struct sockaddr *)&paddr,&plen);
        if(psock<0) continue;
        dsRecvDlywAndSendPower(psock);
        close(psock);
  }//end for
  */
}

long dsRecvDlywAndSendPower(int psock)
{
  long nOpt,nLength,npkgCount,nSpace,nLoop;
  long nsendpkg;
  int  nOutSocket;
  unsigned short sequence,datalen;
  char sDzzh[8]="\0",sDzmm[8]="\0",sFile[21],sFileName[30],sJym[16];
  char sSendAdd[10];    //发起地标识，由移动给出
  char * sOperCode[10]={"000001","000002","100001",   //交易码
                       "300001","300002","400001",
                       "200001","200002","500001","600001"};    //银海[123]

  char * sOperCodeZ[10]={"900031","900032","100030",
  			"200030","210030","600030",
  			"100031","400030"};   //众望[124]

  char sAnswer[TOTAL_PACKAGE_SIZE];
  char * sOffset;
  MOBILEPACKAGE pkg;
  
  memset((void *)&pkg,' ',252);
  SetWaitingTime(MAX_DLYW_TIME);
  
  if(GetFromSocket(psock, "%ld%s%ld",&nOpt,sSendAdd,&nLength))
  {
  	 SendToSocket(psock,"%ld%s",0,"8101");
  	 close(psock);
	 return 1;
  }
  if(GetStringFromSocket(psock,pkg.datatrans))
  {
  	 SendToSocket(psock,"%ld%s",0,"8101");
  	 close(psock);
  	 return 1;
  }
 

  //连接移动通信服务
  nOutSocket=connectTCP("dl_gs_6","power_recv");
  printf("\nOutSocket=%ld\n",nOutSocket);
  if(nOutSocket<0)
  {
  	SendToSocket(psock,"%ld%s",0,"8001");
  	return 1;
  }

  /////////////////请求包赋值////////////////////////  
  processid++;
  processid%=10000000;
  datalen=(unsigned short)(nLength); 
  pkg.morepkt='0';
  //pkg.morepkt=(nOpt!=TASK_POWER_PAY_COMPARE)?'0':'1';
  pkg.pktype='1';                         //数据包
  pkg.unitend='1';                        //此类已结束
  pkg.save='\0';                          //未定义
  pkg.sequence=htons(1);                  //包序号[网络字节序]
  pkg.length=htons(datalen);              //包有效数据长度[网络字节序]
  pkg.msgtype=htonl(processid);           //进程号[网络字节序]
  memcpy(pkg.code,sOperCode[nOpt-TASK_POWER_SIGN_IN],6);//交易码
  memcpy(pkg.d_add,"110223300",9);        //目的标识
  memcpy(pkg.l_add,sSendAdd,9);           //发起地标识

  ///////////将数据包发送给电信通讯机////////////
    if(nOpt==TASK_POWER_PAY_COMPARE)  //发送对帐文件
    {
	if(Dlsrv_ConnectDatabase()==0)
	{
		printf("\n连接数据库失败...");
		close(nOutSocket);
		return -1;
	}
        RunSelect("select cjym from dl$jym where ncxms=123 into %s",sJym);   //yh
        //RunSelect("select cjym from dl$jym where ncxms=124 into %s",sJym); //zw
	RunSelect("select RTRIM(ccs) from dl$jymcsfb where cjym=%s and nbh=1 into %s",sJym,sDzzh);
	RunSelect("select RTRIM(ccs) from dl$jymcsfb where cjym=%s and nbh=2 into %s",sJym,sDzmm);
	RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=4 into %s",sJym,sFile);
	sprintf(sFileName,"account/%s",sFile);
	CloseDatabase();

  	alltrim(sDzzh);alltrim(sDzmm);
  	if(SendFileByFtp("dl_gs_6",sDzzh,sDzmm,sFile,sFileName)==0){
	  printf("\nFile send successful for FTP...\n");
	}//FTP传送成功 
    	else{
          close(nOutSocket);
          return 1;
	}
     }
  //////////////FTP...OVER...//////////////////////
  alarm(0);

  SetWaitingTime(MAX_MOB_TIME);
  printf("\nsendbuf=%s\n",pkg.datatrans);  
  if(SendRecordToSocket(nOutSocket,(char *)&pkg,252))
  {
  	SendToSocket(psock,"%ld%s",0,"8001");
        close(nOutSocket);
  	return 1;
  }

  if(nOpt==TASK_POWER_PAY_CONFIRM || nOpt==TASK_POWER_RECALL_CONFIRM)   //最后缴费确认不接收返回包
  {	
  	close(nOutSocket);
  	return 0;   
  }
  //get power date
  sequence=1;  
  memset(sAnswer,0,TOTAL_PACKAGE_SIZE);
  sOffset=sAnswer;

  for(;;)
  {
	alarm(0);
  	if(nOpt==TASK_POWER_PAY_COMPARE)
          SetWaitingTime(300);
	else
          SetWaitingTime(MAX_MOB_TIME);

        memset(&pkg,0,sizeof(MOBILEPACKAGE));
  	if(GetOneRecordFromSocket(nOutSocket,(char *)&pkg, 252)<=0)
  	{
           close(nOutSocket);
           return 1;
  	}	
        pkg.length=ntohs(pkg.length);	
        pkg.sequence=ntohs(pkg.sequence);
        pkg.msgtype=ntohl(pkg.msgtype);
  		memcpy(sOffset,pkg.datatrans,pkg.length);
  		if(pkg.length==252)
  		     sOffset+=(pkg.length-36);
  		else sOffset+=pkg.length;
      
        /*printf("\npkg.more=%c\npkg.unitend=%c\npkg.type=%c\
                \npkg.seq=%d\npkg.length=%ld\npkg.data=%s\npkg.id=%ld\n",
                pkg.morepkt,pkg.unitend,pkg.pktype,
                pkg.sequence,pkg.length,pkg.datatrans,pkg.msgtype);*/
        
        if(sequence<pkg.sequence-1)
        {
                strcpy(sAnswer,"8004");
                break;
        }
        sequence=pkg.sequence;

        //if(pkg.morepkt=='0'&&pkg.unitend=='1')    break; //接收完毕
	if(pkg.morepkt=='0')    break; //接收完毕
        
        if((sOffset-sAnswer)>(TOTAL_PACKAGE_SIZE-216))
        {
                strcpy(sAnswer,"8005");
                break;
        }
  }//end for
  close(nOutSocket);

  alarm(0);
  //send to dlyw  		
  //if((processid==pkg.msgtype) && pkg.msgtype)
  //{
    SetWaitingTime(MAX_DLYW_TIME);
    SendLongToSocket(psock,0);
    printf("\nTEST:sAnswer=%s\n",sAnswer);
    SendStringToSocket(psock,sAnswer);
    return 0;  		
  //}
  //return -1;	
}
