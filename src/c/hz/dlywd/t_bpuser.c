//寻呼收费模块

#include "dlywd.h"

extern long nSysLkrq;
extern long nSysSfm;

//寻呼查询任务，参数请看t_query.c
long dsTaskBpUserQuery(char * inbuf,RECORD * Client)
{
    char sLsh[41],sWt[6],sYhbz[21];
    char sStation[10],sUser[10],sUserName[101],sTemp[50];
    char sBuffer[1024],sType[21],sKind[21],sDate[11];
    char sJym[16],sJsdh[10];
    long nOrder,nFlag,nXh;
    char * sOffset;
    double dYzfbz;
    double dsfmx[25];
    long   nValue[10],nYear,nMonth;
    
    memset(dsfmx,0,sizeof(dsfmx));
    memset(nValue,0,sizeof(nValue));
    
    FetchStringToArg(inbuf,"%s|%s|%20s",sLsh,sWt,sYhbz);
    FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
    alltrim(sYhbz);
    FetchStringToArg(sYhbz,"%s-%s\0",sStation,sUser);
   
    memset(sBuffer,'\0',sizeof(sBuffer));
    sprintf(sBuffer,"@|1|80|0001|%s|%s|#",sStation,sUser);
    if(dsBpSendAndRecv(-1,sBuffer,sBuffer))
    {
    	ImportRecord(Client,"%s%s","111111","向寻呼通讯机发送/接收数据包失败");
    	return 0;
    }
    alltrim(sBuffer);
    sOffset=sBuffer+2;  //discard the packet head
    sOffset=FetchStringToArg(sOffset,"%d|%d|",&nOrder,&nFlag);
    if(nFlag==0)  //成功
    {
        sOffset=FetchStringToArg(sOffset,"%s|%s|%s|%s|%s|%f|%s|",
                            sStation,sUser,sUserName,sType,sKind,&dYzfbz,sDate);
        GenerateLsh(3,sJsdh,sJym,sTemp);
        nXh=atol(sTemp);
        
        nYear=nSysLkrq/10000;
        nMonth=(nSysLkrq%10000)/100;
        if(strlen(sUserName)<2) strcpy(sUserName,".");
        
        nFlag=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,\
                        cyhbz1,cyhbz2,cyhmc,nywlb,nxh,\
                        dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,\
                        dsfmx8,dsfmx9,dsfmx10,\
                        dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,\
                        dsfmx17,dsfmx18,dsfmx19,dsfmx20,\
                        dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,\
                        njscz2,djscz3,djscz4,nsl,dyjzk,\
                        dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,\
                        dbl1,dbl2,cbl1,cbl2,cjh,cczydh,nlkrq,nsfm,clxbz,\
                        nxzbz,ncsbz,cbz,cbl3,cbl4,cbl5,cbl6,cbl7,clsh,cylsh) \
              VALUES(%s,%ld,%ld,%s,'0','0',%s,0,%ld,%lf,0,0,0,0,0,0,0,0,0,0,0,\
                     0,0,0,0,0,0,0,0,0,0,0,0,0,'0',0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,\
                     '0','0','0','0',%ld,%ld,'0',2,0,'0','0','0',%s,%s,%s,%s,'0')",
                     sJym,sJym,nYear,nMonth,sYhbz,
                     sUserName,nXh,dYzfbz,nSysLkrq,nSysSfm,sType,sKind,sDate,sLsh);
        if(nFlag<0)
        {
                RollbackWork();
        	GetSqlErrInfo(sTemp);
        	sprintf(sBuffer,"插入业务明细出错：%s",sTemp);
        	ImportRecord(Client,"%s%s","000011",sBuffer);
        	return 0;
        }
        CommitWork();
    	ImportRecord(Client,"%s%s%s%lf%ld%s%ld(2)%lf%lf[25]%ld[4]%s(4)","000000",sUserName,"0",0.00,1,
    	             sYhbz,nYear,nMonth,dYzfbz,dsfmx,nValue,sType,sKind,sDate,".");
    	             
    	return 0;
     }
     
     switch(nFlag)
     {
     	case 1:
     	     ImportRecord(Client,"%s%s","000011","该用户为免费用户");
     	     break;
     	     
     	case -1:
     	     ImportRecord(Client,"%s%s","000012","您所输入的号码不存在或查询失败");
     	     break;

      	default:
     	     ImportRecord(Client,"%s%s","000013","查询交易失败");
     	     break;
     }
     return 0;            

}
//寻呼发送数据包
static int dsBpSendAndRecv(int nsock,char * sinbuf,char * soutbuf)
{
     int nerror;
     long nRetVal;
     
     if(nsock<0) nsock=connectTCP("dl_tx_2","bpuser");
     if(nsock<0) return -1;

     nRetVal=strlen(sinbuf);
     SendLongToSocket(nsock,(long)nRetVal);
     if(SendRecordToSocket(nsock,sinbuf,(int)nRetVal))
     {
     	close(nsock);
     	return -2;
     }
     if(!soutbuf) return nsock;
     nerror=GetLongFromSocket(nsock,&nRetVal);
     if(!nerror&&!nRetVal)     
         nerror=GetStringFromSocket(nsock,soutbuf);
     close(nsock);
     return nerror+nRetVal;

}

//寻呼缴费确认，参数请参见t_pay.c
long dsTaskBpUserPayConfirm(char *inbuf,char *outbuf)
{
    char sLsh[41];  //流水号
    char sYhbz[101],sStation[10],sUser[10]; //用户标志
    char sJym[16];  //交易码
    char sYhmc[51]; //用户名称
    char sCzydh[7]; //操作员代号
    char sJsdh[10]; //缴费局局号
    char sBuffer[1024];
    char sDate[11];
    char * sOffset;
    long nMode,nFlag,nCxms;     //缴款方式 1：现金  2：绿卡
    long nCount,nxh,nId,nRetVal;
    double dSjk;

     
    sOffset=FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|%d|",
                       sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSysSfm,&nCount);
    FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
    FetchStringToArg(sYhbz,"%s-%s\0",sStation,sUser);

    GenerateLsh(3,sJsdh,sJym,sBuffer);
    nxh=atol(sBuffer);
    
    RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms); 
    sprintf(sDate,"%04ld/%02ld/%02ld",nSysLkrq/10000,(nSysLkrq%10000)/100,nSysLkrq%100);
    if(nCxms==220)  //万瑞
        sprintf(sBuffer,"@|2|%ld|80|0001|%s|%s|%ld|%s|#",nxh,sStation,sUser,nCount,sDate);
    else //长通,北纬模式
        sprintf(sBuffer,"@|2|%ld|80|0001|%s|%s|%ld|%s|#",nxh,sStation,sUser,(long)dSjk,sDate);
     
    if(dsBpSendAndRecv(-1,sBuffer,sBuffer))
    {
    	strcpy(outbuf,"111111|向寻呼通讯机发送/接收数据包失败|");
    	return 0;
    }
    alltrim(sBuffer);
    sOffset=sBuffer+2;  //discard the packet head
    sOffset=FetchStringToArg(sOffset,"%d|%d|%d|",&nFlag,&nFlag,&nFlag);

    if(nFlag==0)  //成功
    {
        FetchStringToArg(sOffset,"%s|%s|%f|",sStation,sUser,&dSjk);        
        sprintf(sBuffer,"%s|%s|%s|%ld|%.2lf|%s|%ld|0|",
                        sLsh,sYhbz,sYhmc,nMode,dSjk,sCzydh,nSysSfm);

        if(InsertZwmxTable(sBuffer,TASK_PAY_CONFIRM,nxh)<0)
        {
           GetSqlErrInfo(sYhbz);
           RollbackWork();
           sprintf(outbuf,"000011|插入帐务明细表出错：%s|",sYhbz);
           return -3;
        }

        sprintf(sBuffer,"@|5|%ld|80|0001|%s|%s|#",nxh,sStation,sUser);
        dsBpSendAndRecv(-1,sBuffer,sBuffer);
        if(strlen(sBuffer)>20)
        {
       	        sOffset=sBuffer;
       	        while(sOffset[0]!='#' && sOffset[0]) sOffset++;
        	strncpy(sDate,sOffset-11,10);
                sDate[10]='\0';
        }
        RunSql("update dl$ywmx_%t set dyjzk=%lf,dsjk=%lf,cczydh=%s,cbl7=%s where clsh=%s",
               sJym,dSjk,dSjk,sCzydh,sDate,sLsh);
        CommitWork();
        sprintf(outbuf,"000000|%s|%.2lf|",sLsh,dSjk);
        return 0;
    }
    RollbackWork(); 
    strcpy(outbuf,"000013|缴费交易失败|");
    return 0;            
}

//寻呼撤单任务，参数请参见t_recall.c
long dsTaskBpUserRecallConfirm(char * inbuf,long nxh,char * outbuf)
{
     char sLsh[41],sRecallLsh[41],sDate[11];  //流水号
     char sYhbz[101],sStation[10],sUser[10]; //用户标志
     char sJym[16];  //交易码
     char sCzydh[7]; //操作员代号
     char sJsdh[10]; //缴费局局号
     char sBuffer[1024];
     long nRecallXh,nFlag;
     char * sOffset;
     
     
     FetchStringToArg(inbuf,"%s|%s|",sLsh,sRecallLsh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     RunSelect("select nxh,cyhbz from dl$zwmx where cjym=%s and \
                clsh=%s into %ld%s",sJym,sRecallLsh,&nRecallXh,sYhbz);
     FetchStringToArg(sYhbz,"%s-%s\0",sStation,sUser);
     sprintf(sDate,"%ld/%02ld/%02ld",nSysLkrq/10000,(nSysLkrq%10000)/100,nSysLkrq%100);

     sprintf(sBuffer,"@|3|%ld|80|0001|%s|%s|%ld|%s|#",nxh,sStation,sUser,nRecallXh,sDate);

     if(dsBpSendAndRecv(-1,sBuffer,sBuffer))
     {
    	strcpy(outbuf,"111111|向寻呼通讯机发送/接收数据包失败|");
    	return 0;
     }
     alltrim(sBuffer);
     sOffset=sBuffer+2;  //discard the packet head
     sOffset=FetchStringToArg(sOffset,"%d|%d|%d|",&nxh,&nxh,&nFlag);
     if(nFlag==0)
     	strcpy(outbuf,"000000|撤单成功|");
     else
     	strcpy(outbuf,"111111|撤单失败|");
     
     return 0;
}

//寻呼发测试呼，
//参数：sJym:寻呼交易的交易码，sYhbz:寻呼用户号码[台号-机号]
long dsTaskBpTestCall(char * sJym,char * syhbz)
{
    char sBuffer[256],sStation[10],sUser[10];
    long nOrder,nFlag,nXh;
    char * sOffset;
     
    alltrim(syhbz);
    FetchStringToArg(syhbz,"%s-%s\0",sStation,sUser);
    
    if(RunSelect("select nxh from dl$zwmx where cjym=%s and \
                  cyhbz=%s and ncsbz=1 into %ld",sJym,syhbz,&nXh)<=0)
        return ApplyToClient("%ld",-2);
            	
                 	
    memset(sBuffer,'\0',sizeof(sBuffer));
    sprintf(sBuffer,"@|4|%ld|80|0001|%s|%s|#",nXh,sStation,sUser);
    if(dsBpSendAndRecv(-1,sBuffer,sBuffer))
    	return ApplyToClient("%ld",-1);

    alltrim(sBuffer);
    sOffset=sBuffer+2;  //discard the packet head
    FetchStringToArg(sOffset,"%d|%d|",&nOrder,&nFlag);
    
    return ApplyToClient("%ld",nFlag);
}
//寻呼对帐任务，参数请参见t_accnt.c
long dsTaskBpCheckAccount(char * inbuf,char * outbuf)
{
    long nId,nXh,nlkrq,nsize,nFlag,nFetched;
    char sYhbz[101],sStation[10],sUser[10]; //用户标志
    char sJsdh[10],sDate[11],sTemp[256]; //缴费局局号
    double dsjk;
    char * sBuffer;
    char * sOffset;
    

    sBuffer=(char *)malloc(1024);
    nsize=1024;
    sOffset=sBuffer;
    if(!sBuffer)
    {
    	strcpy(outbuf,"111111|数据源存储分配出错|");
    	return 0;
    }

    memset(sOffset,'\0',1024);
    
    sprintf(sDate,"%04ld/%02ld/%02ld",nSysLkrq/10000,(nSysLkrq%10000)/100,nSysLkrq%100);
    sprintf(sOffset,"@|6|80|%s|",sDate);
    while(sOffset[0]) sOffset++;     	

    nId=OpenCursor("select nxh,cyhbz,dsjk,nlkrq from dl$zwmx where cjym=%s and \
                    ncsbz=1 and substr(clxbz,19,1)='0'",inbuf);
    if(nId<0)
    {
        free(sBuffer);
    	GetSqlErrInfo(sYhbz);
    	sprintf(outbuf,"000011|数据源操作数据库出错：%s|",sYhbz);
    	return 0;
    }
    nFetched=0;
    for(;;)
    {
    	if(FetchCursor(nId,"%ld%s%lf%ld",&nXh,sYhbz,&dsjk,&nlkrq)<=0) break;
        sprintf(sDate,"%ld/%02ld/%02ld",nlkrq/10000,(nlkrq%10000)/100,nlkrq%100);
        alltrim(sYhbz);
        FetchStringToArg(sYhbz,"%s-%s\0",sStation,sUser);
        sprintf(sTemp,"$|%ld|%s|%s|%ld|0001|%s|%%|",nXh,sStation,sUser,(long)dsjk,sDate);
        if((nsize-(sOffset-sBuffer))<(strlen(sTemp)-2))  //缺页
        {
        	if(sBuffer=(char*)IncBufSize((void*)sBuffer,nsize,(long)1024))
        	        nsize+=1024;
        	else
        	{
        		free(sBuffer);
      	                strcpy(outbuf,"111111|数据源存储分配出错|");
      	                CloseCursor(nId);
      	                return 0;
      	        }
                memset(sOffset,'\0',1024);
      	}
        strcpy(sOffset,sTemp);
        while(sOffset[0]) sOffset++;
        nFetched=1;
    }//end for
    CloseCursor(nId);
    if(nFetched) sOffset-=2;
    strcpy(sOffset,"#\0");
    
    if(dsBpSendAndRecv(-1,sBuffer,sTemp))
    {
    	strcpy(outbuf,"111111|向寻呼通讯机发送/接收数据包失败|");
    	free(sBuffer);
    	return 0;
    }
    free(sBuffer);
    alltrim(sTemp);
    sOffset=sTemp+2;  //discard the packet head
    FetchStringToArg(sOffset,"%d|%d|",&nXh,&nFlag);
    
//    if(nFlag==0)
    {
        RunSelect("select count(*),sum(dsjk) from dl$zwmx where cjym=%s and ncsbz=1 \
                   into %ld%lf",inbuf,&nId,&dsjk);
        RunSql("update dl$zwmx set ncsbz=2 where cjym=%s and ncsbz=1",inbuf);
        RunSql("delete dl$ywmx_%t where nlkrq<=%ld",inbuf,nSysLkrq-100);
        RunSql("update dl$ywmx_%t set ncsbz=1 where ncsbz=-1",inbuf);
     	sprintf(outbuf,"000000|%ld|%.2lf|",nId,dsjk);
    } 	

//    else
//     	strcpy(outbuf,"111111|对帐失败|");
    return 0;
}



