/*=======================================================
 与电信的代收费接口
=======================================================*/

#include "string.h"
#include"dlywd.h"

extern long nSysLkrq,nSysSfm;

long dsTaskTelecomPayQuery(char * inbuf,RECORD * Client)
{
   
   char sUserName[16],sPassWord[16],sYhbz[101],sWt[6];
   char sAccount[21]="\0",sAccount97[21]="\0",sTeleCode[21]="\0";
   char sLsh[41],sCzydh[7],sJsdh[41],sJym[16];
   char sBuffer[TOTAL_PACKAGE_SIZE],* sOffset;
   double dSfmx[50],dYjzk;
   char   sSfmx[50],sYhmc[51],sDate[21],sDump[21];
   long nXh,nResult,nLoop,nMxCount,nYear,nMonth,nFys,nRetVal,nValue[4],nIndex,range;
   static char * sField[50]={
   "月租","区内","区间","长途","国际",
   "电报","800","会议","新业务","设备出租",
   "代维","电路出租","数据通信","Inter","主叫因特网",
   "滞纳","信息","赠长话","优惠","....","...."};
     long nSign=0;
   
   
   
   FetchStringToArg(inbuf,"%s|%s|%s|%s|",sLsh,sWt,sYhbz,sCzydh);
   FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
   
   
   if(sWt[0]=='1') strncpy(sTeleCode,sYhbz,20);
   else if(sWt[1]=='1') strncpy(sAccount,sYhbz+20,20);
   else if(sWt[2]=='1') strncpy(sAccount97,sYhbz+40,20);
   
   RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=1 into %s",sJym,sUserName);
   RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=2 into %s",sJym,sPassWord);
   RunSelect("select dlwtlsh.nextval from dual into %ld",&nXh);

   memset(nValue,0,sizeof(nValue));
      
   alltrim(sAccount);
   alltrim(sAccount97);
   alltrim(sTeleCode);
  /* sprintf(sBuffer,"CXR|%06ld|%s|%s|%s|%s|%s|#",
           nXh%1000000,sUserName,sPassWord,sAccount,sAccount97,sTeleCode);*/
   sprintf(sBuffer,"CXR|%06ld|%s|%s|%s|%s|%s|%ld|#",
           nXh%1000000,sUserName,sPassWord,sAccount,sAccount97,sTeleCode,nSign);
   
   if(dsTelecomSendAndRecv(sBuffer,sBuffer))
   {
   	ImportRecord(Client,"%s%s","111111","与邮政通信机通信失败");
   	return 0;
   }

   if(strncmp(sBuffer,"CXA",3))
   {
   	ImportRecord(Client,"%s%s","000011","查询出错:电信方返回数据错误!");
   	return 0;
   }
   
   nResult=-3;
   sOffset=FetchStringToArg(sBuffer+4,"%d|%d|%s|%s|%s|",
   &nXh,&nResult,sAccount,sAccount97,sTeleCode);
   if(sOffset==NULL) nResult=-3;
   switch(nResult)
   {
      case -1:
   	ImportRecord(Client,"%s%s","000012","没有该用户的信息");
   	return 0;
      case 0:
   	ImportRecord(Client,"%s%s","000013","该用户已缴费");
   	return 0;
      case -2:
   	ImportRecord(Client,"%s%s","000014","欠费月份数超过12个月，须在电信局缴纳");
   	return 0;
      case -3:   	
   	ImportRecord(Client,"%s%s","000015",sBuffer);
   	return 0;
   }
  
   sOffset=FetchStringToArg(sOffset,"%s|%f|%s|",sYhmc,&dYjzk,sDump);
   ImportRecord(Client,"%s%s%s%lf%ld","000000",sYhmc,"0",0.00,nResult);

   for(nLoop=0;nLoop<nResult;nLoop++)
   {
   	memset(dSfmx,0,sizeof(dSfmx));
   	sOffset=FetchStringToArg(sOffset,"%s|%d|",sDate,&nMxCount);
   	for(nFys=0;nFys<nMxCount;nFys++)
   	{
   	   sOffset=FetchStringToArg(sOffset,"%s|",sSfmx);
   	   for(nIndex=0;nIndex<21;nIndex++)
   	       if(strstr(sSfmx,sField[nIndex])!=NULL) break;
   	   sOffset=FetchStringToArg(sOffset,"%f|",&dSfmx[nIndex]);

	RunSql("INSERT INTO dl$dxfp(cfpmc,dfpje,nxh,nlkrq,nfpzs) \
                        values(%s,%lf,%ld,%ld,%ld)",
                        sSfmx,dSfmx[nIndex],nXh,nSysLkrq,nLoop+1);

   	   if(nIndex==21)
   	   {
   	   	dSfmx[24]+=dSfmx[21];
   	   	dSfmx[21]=0.0;
   	   }
        }
   	   
        sOffset=FetchStringToArg(sOffset,"%s|%f|%f|%f|",sDump,&dYjzk,&dSfmx[19],&dSfmx[20]);
   	FetchStringToArg(sDate,"%4d%2d",&nYear,&nMonth);

        if(!sOffset)
        {
                RollbackWork();
        	GetSqlErrInfo(sBuffer);
        	sprintf(Client->sdata,"%s|数据格式解释出错|","000011");
        	return 0;
        }

        for(nIndex=0;nIndex<=2;nIndex++)
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
              VALUES(%s,%ld,%ld,%s,%s,%s,%s,0,%ld,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,\
              %lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,\
              '0',%ld,0,0,0,%lf,0,%lf,0,0,1,1,0,0,0,0,'0','0','0',%s,%ld,%ld,'0',2,0,'0',\
              '0','0','0','0','0',%s,'0')",
                     sJym,sJym,nYear,nMonth,sTeleCode,sAccount,sAccount97,
                     sYhmc,nXh,dSfmx[0],dSfmx[1],dSfmx[2],dSfmx[3],dSfmx[4],dSfmx[5],
                     dSfmx[6],dSfmx[7],dSfmx[8],dSfmx[9],dSfmx[10],dSfmx[11],dSfmx[12],
                     dSfmx[13],dSfmx[14],dSfmx[15],dSfmx[16],dSfmx[17],dSfmx[18],dSfmx[19],
                     dSfmx[20],dSfmx[21],dSfmx[22],dSfmx[23],dSfmx[24],nLoop+1,dYjzk,dYjzk,
                     sCzydh,nSysLkrq,nSysSfm,sLsh);
	     if(nRetVal>0) break;
	     if(nIndex==0) sYhmc[20]='\0';
	     if(nIndex==1) strcpy(sYhmc,".....");
	}
	if(nRetVal<0)
        {
                RollbackWork();
        	GetSqlErrInfo(sBuffer);
        	sprintf(Client->sdata,"%s|插入业务明细出错：%s|","000011",sBuffer);
        	return 0;
        }
    	ImportRecord(Client,"%s%ld(2)%lf[25]%lf%ld[4]%s(4)",
    	             sTeleCode,nYear,nMonth,dSfmx,dYjzk,nValue,".",".",".",".");
        sOffset+=2;  //jump '%|'
   }//end for
   CommitWork();
   return 0;
}

long dsTaskTelcomPayConfirm(char *inbuf,char *outbuf)
{
      
   char sLsh[41];  //流水号
   char sYhbz[101]; //用户标志
   char sJym[16];  //交易码
   char sYhmc[51]; //用户名称
   char sCzydh[7]; //操作员代号
   char sJsdh[10]; //缴费局局号
   char sBuffer[1024],sAccount[21],sAccount97[21],sUserName[21],sPassWord[21],sDate[11],cBl1[21];
   char * sOffset;
   long nMode;     //缴款方式 1：现金  2：绿卡
   long nCount,nYear,nMonth,nXh,nId,nMxCount,nResult,nLoop,nRetVal;
   double dSjk;  //实缴款，应缴款
  long nSign=0;

   sOffset=FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|%d|",
                      sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSysSfm,&nCount);
   FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     
   RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=1 into %s",sJym,sUserName);
   RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=2 into %s",sJym,sPassWord);
   RunSelect("select dlwtlsh.nextval from dual into %ld",&nXh);
   RunSelect("select cyhbz1,cyhbz2 from dl$ywmx_%t where clsh=%s into %s%s",sJym,sLsh,sAccount,sAccount97);

   /*sprintf(sBuffer,"XZR|%06ld|%s|%s|%s|%.2lf|#",nXh%1000000,sUserName,sPassWord,sAccount,dSjk);*/
   sprintf(sBuffer,"XZR|%06ld|%s|%s|%s|%.2lf|%ld|#",nXh%1000000,sUserName,sPassWord,sAccount,dSjk,nSign);
   
   if(InsertZwmxTable(inbuf,TASK_PAY_CONFIRM,nXh)<0)
   {
        GetSqlErrInfo(sYhbz);
        RollbackWork();
        sprintf(outbuf,"000011|插入帐务明细表出错：%s|",sYhbz);
        return 0;
   }
   RunSql("update dl$zwmx set cyhbz1=%s,cyhbz2=%s where clsh=%s",sAccount,sAccount97,sLsh);

   if(dsTelecomSendAndRecv(sBuffer,sBuffer))
   {
   	strcpy(outbuf,"111011|与通信机通信失败..|");
	RollbackWork();
        //CommitWork();
   	return -1;
   }

   if(strncmp(sBuffer,"XZA",3))
   {
   	sprintf(outbuf,"000011|缴费出错:%s|",sBuffer);
        RollbackWork(); 
   	return 0;
   }
   
   sOffset=FetchStringToArg(sBuffer+4,"%d|%d|%s|",&nXh,&nResult,sAccount);
   if(sOffset==NULL) nResult=-4;
   switch(nResult)
   {
      case 1:
        CommitWork();
        sprintf(outbuf,"000000|%s|%.2lf|",sLsh,dSjk);  //正确
        return 0;
      case -1:
   	strcpy(outbuf,"000012|没有该用户的信息|");
        break;
      case 0:
   	strcpy(outbuf,"000013|该用户已缴费");
        break;
      case -2:
        strcpy(outbuf,"000014|金额出错|");
        break;
      case -3:
        strcpy(outbuf,"000014|操作员无权限|");
        break;
      default:   	
   	sprintf(outbuf,"000015|无法鉴别:%-20.20s|",sBuffer);
        break;
   }
   /*{
	FILE *fp;
	fp=fopen("telecom.log","ab");
	sprintf(sBuffer,"%ld|%s|%s|%ld|\n",nSysLkrq,sYhbz,sAccount,nResult);
	fwrite(sBuffer,strlen(sBuffer),1,fp);
	fclose(fp);
   }*/
   RollbackWork();
   return 0;

}

long dsTelecomRecallConfirm(char * inbuf,long nxh,char * outbuf)
{
   
   char sLsh[41],sRecallLsh[41],sDate[11];  //流水号
   char sTeleCode[21],sAccount[21],sAccount97[21],sUserName[21],sPassWord[21]; //用户标志
   char sJym[16];  //交易码
   char sCzydh[7]; //操作员代号
   char sJsdh[10]; //缴费局局号
   char sBuffer[1024];
   char * sOffset;
   double dSjk;
   long nResult,nXzbz=2;
   long nSign=0;

   FetchStringToArg(inbuf,"%s|%s|%s|%d|",sLsh,sRecallLsh,sCzydh,&nSysSfm);
   FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);

   
   RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=1 into %s",sJym,sUserName);
   RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=2 into %s",sJym,sPassWord);
   RunSelect("select cyhbz,cyhbz1,cyhbz2 from dl$ywmx_%t where clsh=%s into %s%s%s",
   sJym,sRecallLsh,sTeleCode,sAccount,sAccount97);
   RunSelect("select dsjk,nxzbz from dl$zwmx where clsh=%s into %lf%ld",sRecallLsh,&dSjk,&nXzbz);
   

   sprintf(sBuffer,"FXR|%06ld|%s|%s|%s|%s|%s|%.2lf|#",
                    nxh%1000000,sUserName,sPassWord,sAccount,sAccount97,sTeleCode,dSjk);
   
   if(nXzbz==2) dsTelecomSendAndRecv(sBuffer,sBuffer);
   
/*   
   if(dsTelecomSendAndRecv(sBuffer,sBuffer) && nXzbz!=0)
   {
   	strcpy(outbuf,"111111|与邮政通信机通信失败|");
   	return 0;
   }
   if(strncmp(sBuffer,"FXA",3) && nXzbz!=0)
   {
   	strcpy(outbuf,"000011|查询出错，错误原因未知|");
   	return 0;
   }

   if(nXzbz!=0)
   {
        sOffset=FetchStringToArg(sBuffer+4,"%d|%d|%s|",&nxh,&nResult,sAccount);
        if(sOffset==NULL) nResult=-4;
        switch(nResult)
        {
          case 1:  //成功
             break;
          case -1:
   	     strcpy(outbuf,"000012|没有该用户的信息|");
   	     return 0;
          case 0:
   	     strcpy(outbuf,"000013|无法撤消|");
   	     return 0;
          case -2:
             strcpy(outbuf,"000014|金额出错|");
   	     return 0;
          case -3:
             strcpy(outbuf,"000014|操作员无权限|");
   	     return 0;
          default:
   	     sprintf(outbuf,"000015|无法鉴别:%-20.20s|",sBuffer);
   	     return 0;
        }
   }     
*/  
   strcpy(outbuf,"000000|撤销成功|");
   return 0;
}

long dsTelecomCheckAccount(char * inbuf,char * outbuf)
{

   char * buf;
   long bufsize=0,nId,nCount=0.0,nXh,buflen=0,nStatus,nLkrq=0;
   char sBuffer[1024],sUserName[21],sPassWord[21],sAccount[21];
   char sDescribe[512];
   double dSjk,dSum=0.0;
    long nSign=0;
   
   GetSysLkrq();   

   buf=(char*)malloc(1024);
   bufsize=1024;
   memset(buf,0,1024);
   
   RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=1 into %s",inbuf,sUserName);
   RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=2 into %s",inbuf,sPassWord);
   RunSelect("select dlwtlsh.nextval from dual into %ld",&nXh);
   RunSelect("select min(nlkrq) from dl$zwmx where ncsbz=1 and cjym=%s into %ld",inbuf,&nLkrq);
   
   RunSelect("select count(*),sum(dsjk) from dl$zwmx "
   "where substr(clxbz,19,1)='0' and nlkrq=%ld and cjym=%s into %ld%lf",
             nLkrq,inbuf,&nCount,&dSum);
   
   sprintf(buf,"DAY|%06ld|%s|%s|%ld|%ld|%.2lf|\n",
                nXh%1000000,sUserName,sPassWord,nLkrq,nCount,dSum);

   buflen=strlen(buf);
   nId=OpenCursor("select cyhbz1,dsjk from dl$zwmx where "
   "substr(clxbz,19,1)='0' and nlkrq=%ld and cjym=%s",nLkrq,inbuf);
   
   if(nId<0)
   {
   	GetSqlErrInfo(buf);
   	sprintf(outbuf,"000111|%s|",buf);
   	return 0;
   }
   
   for(;;)
   {
   	if(FetchCursor(nId,"%s%lf",sAccount,&dSjk)<=0) break;
   	sprintf(sBuffer,"%s|%.2lf|%ld\n",sAccount,dSjk,nSign);
   	
   	if(strlen(sBuffer)>=(bufsize-buflen))
   	{
   	    buf=(char *)IncBufSize((void *)buf,bufsize,1024);
   	    bufsize+=1024;
   	}
   	strcat(buf,sBuffer);
   	buflen+=strlen(sBuffer);
   }	    
   strcat(buf,"#");
   CloseCursor(nId);
   if(dsTelecomSendAndRecv(buf,sBuffer))
   {
   	free(buf);
   	strcpy(outbuf,"111111|与邮政通信机通信失败|");
   	return 0;
   }
   free(buf);
   if(strncmp(sBuffer,"DAY",3))
   {
   	sprintf(outbuf,"000011|对帐出错：%-20.20s|",sBuffer);
   	return 0;
   }
   
   FetchStringToArg(sBuffer+4,"%d|%d|%s|",&nXh,&nStatus,sDescribe);
   switch(nStatus)
   {
      case 220:
   	RunSql("update dl$zwmx set ncsbz=2 where nlkrq=%ld and cjym=%s",nLkrq,inbuf);
   	CommitWork();
   	if(nLkrq!=nSysLkrq)
   	   sprintf(outbuf,"000015|对帐成功，对帐日期为:%ld|",nLkrq);
        else   	   
   	   sprintf(outbuf,"000000|%ld|%.2lf|",nCount,dSum);

	if(RunSql("delete dl$dxfp where nlkrq<%ld",nLkrq-2)>0) CommitWork();

   	return 0;
     
      default:
        sprintf(outbuf,"000011|%s|",sBuffer);
        return 0;
   }
}

static long dsTelecomSendAndRecv(char * inbuf,char * outbuf)
{
   int nsock;
   long nLength,nRetVal;
  
   nLength=strlen(inbuf);
   printf("\nsend=%s",inbuf);
   TelecomEncrypt(inbuf,nLength);
   nsock=connectTCP("dl_tx_4","telecom");
   if(nsock<0) return -1;
   
   if(SendLongToSocket(nsock,nLength)) 
   {
   	close(nsock);
   	return -2;
   }

   if(SendRecordToSocket(nsock,inbuf,nLength)) 
   {
   	close(nsock);
   	return -2;
   }
   
   if(!outbuf)
   {
   	close(nsock);
   	return 0;
   }
   
   nRetVal=-1;
   GetLongFromSocket(nsock,&nRetVal);
   if(nRetVal)
   {
   	close(nsock);
   	return -2;
   }
   if(GetLongFromSocket(nsock,&nLength))   
   {
   	close(nsock);
   	return -2;
   }
   if(GetOneRecordFromSocket(nsock,outbuf,nLength)!=nLength)   
   {
   	close(nsock);
   	return -2;
   }
   
   close(nsock);
   TelecomDecrypt(outbuf,nLength);
   outbuf[nLength]='\0';
   printf("\nrecv=%s",outbuf);
   return 0;
}


/*加密子程序*/
static void TelecomEncrypt(char *buf,long buflen)
{
  unsigned char charbefore,charafter;
  unsigned short int shiwei[16]={12,1,13,0,2,4,6,3,7,10,8,11,15,9,5,14};
  unsigned short int gewei[16]= {13,6,4,1,11,9,7,5,3,12,14,2,15,8,0,10};
  long loop;
  long highbyte,lowbyte;
  for(loop=0;loop<buflen;loop++)
  {
    charbefore=buf[loop];
    highbyte=(charbefore & 0xF0)/16;
    lowbyte=(charbefore & 0x0F);
    charafter=shiwei[highbyte]*16+gewei[lowbyte];
    buf[loop]=charafter;
  }
}

/*解密子程序*/
static void TelecomDecrypt(char *buf,long buflen)
{
  unsigned char charbefore,charafter;
  unsigned short int shiwei[16]={3,1,4,7,5,14,6,7,10,13,9,11,0,2,15,12};
  unsigned short int gewei[16]= {14,3,11,8,2,7,1,6,13,5,15,4,9,0,10,12};
  long loop;
  long highbyte,lowbyte;
  for(loop=0;loop<buflen;loop++)
  {
    charbefore=buf[loop];
    highbyte=(charbefore & 0xF0)/16;
    lowbyte=(charbefore & 0x0F);
    charafter=shiwei[highbyte]*16+gewei[lowbyte];
    buf[loop]=charafter;
  }
}

