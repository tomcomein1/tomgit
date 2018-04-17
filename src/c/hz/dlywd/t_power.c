//与电费的相关的后台任务模块
#include"dlywd.h"

extern long nSysLkrq;
extern long nSysSfm;

long dsTaskPowerSignIn(char * sJym)
{
     char sAuthentication[17],sTransKey[17];
     char sBankCode[9],sSendAdd[10],sMACkey[17];
     char sDataTrans[256],sTemp[100];
     int ndeslen;     
     long nRetVal;
     
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
 
     memset(sDataTrans,0,sizeof(sDataTrans));
     strcpy(sDataTrans,sBankCode);
     
     //发送签到包
     if(dsPowerSendAndRecv(-1,sDataTrans,TASK_MOBILE_SIGN_IN,sSendAdd,sDataTrans)<0)
     	return ApplyToClient("%s","111111|向通讯机发送/接收数据出错|");

     if(strncmp(sDataTrans,"0000",4))
     {
        GetPowerErrorInfo(sDataTrans,sDataTrans);
        ApplyToClient("%s",sDataTrans);
        return 0;
     }
         
     return ApplyToClient("%s","000000|");
     
}

long dsTaskPowerSignOut(char * sJym)
{
     char sAuthentication[17],sTransKey[17];
     char sBankCode[9],sSendAdd[10];
     char sDataTrans[256],sTemp[100];
     int ndeslen;     
     long nRetVal;
     
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
 
     memset(sDataTrans,0,sizeof(sDataTrans));
     strcpy(sDataTrans,sBankCode);
     
     //发送签到包
     if(dsPowerSendAndRecv(-1,sDataTrans,TASK_MOBILE_SIGN_OUT,sSendAdd,sDataTrans)<0)
     	return ApplyToClient("%s","111111|向通讯机发送/接数据收出错");
 
     if(strncmp(sDataTrans,"0000",4))
     {
        GetPowerErrorInfo(sDataTrans,sDataTrans);
        ApplyToClient("%s",sDataTrans);
        return 0;
     }
     return ApplyToClient("%s","000000|");

}

long dsTaskPowerQuery(char * inbuf,RECORD * Client)
{

     char sBzmap[6],sJym[16],sLsh[41],sYhbz[101],sJsdh[10],sCzydh[7];
     char sBuffer[TOTAL_PACKAGE_SIZE];
     char sBankCode[9];   //银行代码（dl$jymcsfb,nbh=36)
     char sSendAdd[10];
     CHARGEDETAIL det;
     MOBILEANSWER ans;
  
     FetchStringToArg(inbuf,"%s|%s|%s|%s|",sLsh,sBzmap,sYhbz,sCzydh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);

     sYhbz[11]='\0';
     memset(sBuffer,'\0',TOTAL_PACKAGE_SIZE);
   
     //正面的函数提供按交易码进行互斥操作，CommitWork/RollbackWork解锁
          
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
   
     sprintf(sBuffer,"%-14.14s%-12.12s",sYhbz,sBankCode);

     if(dsPowerSendAndRecv(-1,sBuffer,TASK_POWER_PAY_QUERY,sSendAdd,sBuffer)<0)
     {
     	ImportRecord(Client,"%s%s","111111","向通讯机发送/接数据收出错");
        return 0;
     }
          
     if(strncmp(sBuffer,"0000",4))
     {
        GetPowerErrorInfo(sBuffer,sBuffer);
        ImportRecord(Client,"%s",sBuffer);      
        return 0;
     }
     
     //PowerFormatTransfer(sBuffer,TASK_POWER_PAY_QUERY);
     //return PowerInsertYwmx(sLsh,sCzydh,sYhbz,sJym,sBuffer,Client);
     //return MobileInsertYwmx(sLsh,sCzydh,sYhbz,sJym,sBuffer,Client);
          
}

/*===============================================================
函数：PowerInsertYwmx
功能：移动将查询结果放入业务明细表
参数： slsh:流水号
      sPhone:电话号码
      sJym:交易码
      sCzydh:查询操作员代号
      sDetail:移动返回的详细内容
      Client:返回给客户端的RECORD结构

*===============================================================*/

static long PowerInsertYwmx(char * slsh,char * sCzydh,char * sPhone,char * sJym,char * sDetail,RECORD * Client)
{
   CHARGEDETAIL det;
   MOBILEANSWER ans;
   
   char sCode[]="ABDGHIGKLab";
   char * curstat[]={"正常","冒高","欠停","报停","挂失","预销","预拆","强开","强关","销号","局拆"};
   char sSqlErr[51],sSequence[11],sJsdh[10],sStatus[5];
   char * sOffset;
   long nRec,nRetVal=1,nLoop;
   long nId=1,nxh=-1;
   double dSjk,dYjzk,dAddition,dTotalSjk,dTemp;
   long   nValue[4];
      
   memset(&det,0,sizeof(CHARGEDETAIL));
   memset(&ans,0,sizeof(MOBILEANSWER));
   memset(nValue,0,sizeof(nValue));

   dTotalSjk=0.0; 
   sOffset=sDetail;
   sOffset=FetchStringToArg(sOffset,            //解读汇总头
                "%4s%15s%2s%2s%59s%12s%1d%1s%12f%12f%12f%12f%4s%4s%4d",
                 ans.sReturn,ans.sBargain,ans.sRegion,ans.sCountry,
                 ans.sCompany,ans.sUserName,&ans.nWay,ans.sStatus,
                 &ans.dwczhf,&ans.dxyd,&ans.dyck,&ans.dbsyzf,ans.smljqs,
                 ans.sszjqs,&ans.nCount);    //将符串的值分别装入变量中

   strncpy(sStatus,curstat[strchr(sCode,ans.sStatus[0])-sCode],4);
   sStatus[4]='\0';
   FetchStringToArg(slsh,"%9s",sJsdh);
   ImportRecord(Client,"%s%s%s%lf%ld","000000",ans.sCompany,ans.sBargain,ans.dyck,((ans.nCount==0)?1:ans.nCount));
   nId=((ans.nCount==0)?0:1);

   det.nYear=(nSysLkrq/10000);
   det.nMonth=(nSysLkrq%10000)/100;
   dAddition=0.0;
   //if(ans.nCount>0 && ans.nWay==9) dAddition=(ans.dwczhf+50-ans.dyck);//预存话费用户
   if(ans.nCount>0 && ans.nWay==9) dAddition=(ans.dwczhf-ans.dyck);//新预存话费用户
   nValue[0]=ans.nWay;   //长整型保留字段
   
   for(;nId<=ans.nCount;nId++)
   {
        if(ans.nCount>0)
        {
            //将明细内容解析
            sOffset=FetchStringToArg(sOffset,
            "%4d%2d%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f",
            &det.nYear,&det.nMonth,&det.dFee,&det.dBchf,&det.dZnj,&det.dYhk,
            &det.dYckhb,&det.dXjk,&det.dYzf,&det.dTff,&det.dBwf,&det.dMyf,
            &det.dCtf,&det.dFjf,&det.dQtf,&det.dXxf,&det.dPzf,&det.dNhf,&det.dBsf); 
        }
        //并将明细内容填充到返回结构中
        //用户实缴款=本月欠费+滞纳金+补收月租费(只加第一个月)+预存用户新增(只加最后一个月)
        dSjk=det.dFee+det.dZnj+ans.dbsyzf;
        dTemp=dSjk;
        if(nId==ans.nCount) dSjk+=dAddition;
        //应缴总款(发票单大写)=本次通话费-充值卡划拨+滞纳金+补收月租费-优惠款
        dYjzk=det.dBchf-det.dXjk+det.dZnj+ans.dbsyzf-det.dYhk;
        if(dYjzk<0.0) dYjzk=0.0;
        if((dSjk-(long)dSjk)>0.0) dSjk=(double)((long)dSjk+1);
        //导入客户端RECORD结构
        ImportRecord(Client,"%s%ld(2)%lf(26)%ld[4]%s(4)",sPhone,
                         det.nYear,det.nMonth,det.dYzf,det.dPzf,
                         det.dBwf,det.dMyf,det.dCtf,det.dFjf,det.dXxf,
                         0.0,0.0,det.dZnj,det.dNhf,det.dBsf,
                         det.dQtf,det.dYhk,det.dYckhb,det.dXjk,det.dTff,
                         ans.dbsyzf,dSjk,ans.dwczhf,ans.dxyd,0.0,0.0,
                         0.0,(ans.nWay==9)?dYjzk:dTemp,
                         dSjk,nValue,sStatus, "." , "." , ".");

        GenerateLsh(3,sJsdh,sJym,sSequence);
        nxh=atol(sSequence);
      
        //插入业务明细表结构
        nLoop=2;
        dTotalSjk+=dSjk;
        //加入2次业务明细插入循环，以保证移动数所包存在非法字符时也能正确插入
        while(nLoop--)
        {
           nRetVal=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,\
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
                 VALUES(%s,%ld,%ld,%s,%s,'0',%s,0,%ld,\
                	%lf,%lf,%lf,%lf,%lf,%lf,%lf,0,0,%lf,\
                  	%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,0,0,\
                  	0,0,0,0,0,'0',0,0,0,0,%lf,0,%lf,0,0,\
                  	1,1,%ld,0,0,0,'0','0','0',%s,%ld,%ld,'0',\
                  	2,0,'0','0','0','0',%s,%s,%s,'0')",
           sJym,sJym,det.nYear,det.nMonth,sPhone,ans.sBargain,
           ans.sCompany,nxh,det.dYzf,det.dPzf,det.dBwf,det.dMyf,
           det.dCtf,det.dFjf,det.dXxf,det.dZnj,det.dNhf,det.dBsf,
           det.dQtf,det.dYhk,det.dYckhb,det.dXjk,det.dTff,ans.dbsyzf,
           dYjzk,((nId==ans.nCount)?dTotalSjk:0.0),ans.nWay,sCzydh,nSysLkrq,nSysSfm,ans.sRegion,ans.sCountry,slsh);
           if(nRetVal>0) break;
           strcpy(ans.sCompany,".");
        }//end while
        
        if(nRetVal<0) break;
        ans.dbsyzf=0.0;  //补收月租费只算在第一个月内
   }//end for;

   if(nRetVal>0)  CommitWork();  
   else
   {
   	RollbackWork();
   	GetSqlErrInfo(sSqlErr);
   	sprintf(Client->sdata,"101007|%s|",sSqlErr);
   }
   return nRetVal;
} 

long dsTaskPowerPayConfirm(char * inbuf,char * outbuf)
{
     char sTemp[10000],sMAC[17],sMACkey[17],sPrName[20];     
     char sLsh[41];  //流水号
     char sYhbz[101]; //用户标志
     char sJym[16];  //交易码
     char sYhmc[51]; //用户名称
     char sCzydh[7]; //操作员代号
     char sJsdh[10]; //缴费局局号
     char sSendAdd[10],sBankCode[9];
     char sBuffer[10000];
     char sRegion[3],sCountry[3];  //地区代码与市县代码
     char * sOffset;
     long nMode;     //缴款方式 1：现金  2：绿卡
     long nxh,nTotal,nCount;
     int  nsock;
     double dSjk,dTemp;
     long nSfm=0;


     sOffset=FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|",
                         sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSfm);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
    
     sYhbz[11]='\0'; 
     RunSelect("select distinct cbl6,cbl7 from dl$ywmx_%t \
                where clsh=%s into %s%s",sJym,sLsh,sRegion,sCountry);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
       
     RunSql("update dl$ywmx_%t set dsjk=%lf where clsh=%s and dSjk>0.0",sJym,dSjk,sLsh);

     ASCIItoBCD(sMAC,16,sMACkey);
     
     //得到帐务明细外部流水号
     GenerateLsh(3,sJsdh,sJym,sBuffer);
     nxh=atol(sBuffer);
     
     //生成MAC验证序列
     sprintf(sTemp,"%8.8s%08ld%11.11s%12ld%08ld%06ld",
                    sBankCode,nxh,sYhbz,(long)dSjk*100,nSysLkrq,nSfm);
     sprintf(sBuffer,
             "b000%-2.2s%-2.2s%-8.8s%08ld%-11.11s%12ld%08ld%06ld%-16.16s",
             sRegion,sCountry,sBankCode,nxh,sYhbz,(long)dSjk*100,
             nSysLkrq,nSfm,sTemp);

     //将数据发往移动进行验证
     
     if(dsPowerSendAndRecv(-1,sBuffer,TASK_POWER_PAY_CHECK,sSendAdd,sTemp)<0)
     {
        RollbackWork();
     	strcpy(outbuf,"111111|向通讯机发送/接数据收出错|");
        return -1;
     }
     
     if(strncmp(sTemp,"0000",4))
     {
     	GetPowerErrorInfo(sTemp,outbuf);
     	RollbackWork();
     	return -2;
     }

     if(InsertZwmxTable(inbuf,TASK_POWER_PAY_CONFIRM,nxh)<0)
     {
        GetSqlErrInfo(sTemp);
        sprintf(outbuf,"000011|插入帐务明细表出错：%s|",sTemp);
        return -3;
     }
     CommitWork();

     nsock=dsPowerSendAndRecv(-1,sBuffer,TASK_POWER_PAY_CONFIRM,
                                          sSendAdd,NULL);
     if(nsock>0) close(nsock);
     sprintf(outbuf,"000000|%s|%.2lf|",sLsh,dSjk);
     return 0;
   
}

/*==================================================================
函数：dsPowerSendAndRecv
功能：向通讯机发送包并等待接收;  
参数：nsock:事先建好的socket连接，若nsock<0则由其内部自动建立
      sinbuf:发送包内容
      nopt:交易任务号
      soutstr:返回字符串区域
返回：<0失败；=0成功 >0:建立成功的socket句柄
说明：若soutbuf!=NULL则需要接收返回数据包，否则不需要
*=================================================================*/
static int dsPowerSendAndRecv(int nsock,char * sinbuf,long nopt,char * sSendAdd,char * soutbuf)
{
     int nerror;
     long nRetVal,nLength;
     
     if(nsock<0) nsock=connectTCP("dl_tx_11","power");
     if(nsock<0) return -1;

     nerror=SendToSocket(nsock,"%ld%s%ld",nopt,sSendAdd,(long)strlen(sinbuf));
		
     if(!nerror) nerror=SendStringToSocket(nsock,sinbuf);
     if(nerror)
     {
     	close(nsock);
     	return -2;
     }
     if(!soutbuf) return nsock;
     nerror=GetLongFromSocket(nsock,&nRetVal);
     if(!nerror&&!nRetVal)
        nerror=GetStringFromSocket(nsock,soutbuf);
     
     close(nsock);
     return nerror;	
}

/////////////////////////////////////////////////////////
static void GetPowerErrorInfo(char * sError,char * sInfo)
{
    char sErrCode[5]; 
    int nLen;
    char * sBegin;
    char sMsgTab[]= "0001:100001|表示失败/不能收取/不能删帐/文件传输失败/对帐失败|;"
                    "0002:100002|表示用户不存在|;"
                    "0003:100003|表示没有欠费/没有可以删的帐|;"
                    "0004:100004|表示其它人正在收取|;"
                    "0005:100005|表示没有对帐清单|;"
                    "0006:100006|表示没有签到|;"
                    "0007:100007|表示上日没有对帐|;"
                    "0040:100040|不能补打发票|;"
                    "0050:100050|该用户已是批量代扣用户|;"
                    "0051:100051|增加（删除）代扣用户失败|;"
                    "0052:100052|没有该代扣用户的用电档案|;"
                    "0060:100060|无批量代扣数据|;"
                    "0020:100020|用户已缴费|;"
                    "8001:108001|无法与移动通讯机建立连接|;"
                    "8004:108004|通讯机接收移动方数据包丢失|;"
                    "8005:108005|接收缓冲区溢出|;";

    strncpy(sErrCode,sError,4);
    sErrCode[4]='\0';
    sBegin=strstr(sMsgTab,sErrCode);
    
    if(sBegin&&sBegin[4]==':')
    {
           nLen=strchr(sBegin,';')-sBegin-5;    
           strncpy(sInfo,sBegin+5,nLen);
           sInfo[nLen]='\0';
    }       
    else   sprintf(sInfo,"109999|未知的错误[%s]|",sErrCode);

}
