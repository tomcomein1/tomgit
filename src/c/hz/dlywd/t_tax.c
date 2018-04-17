#include"dlywd.h"

extern long nSysLkrq;
extern long nSysSfm;

static SWMX *swmx;

//////////////////////新国税查询[ncxms=521]////////////////////////////////
long dsTaskTaxQuery(char * inbuf,RECORD * Client)
{
     char sBzmap[6],sJym[16],sLsh[41],sYhbz[101],sJsdh[10];
     char sYhmc[51],sYhbz1[41];//用户名称与合同号
     char sWhere[400];
     char sBz[31],jjlx[21],zssz[11],zssm[14];
     char sCzydh[7],sHdbz[2];
     long nmap,nId,ncount,nysnd,nysyf,nRetVal;
     double dsfmx[26],dZnj;
     long nValue[4],nDates,nCxms;

     FetchStringToArg(inbuf,"%s|%s|%s|%s|",sLsh,sBzmap,sYhbz,sCzydh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
    
     memset(dsfmx,0,sizeof(dsfmx));
     memset(nValue,0,sizeof(nValue));
     sYhbz[7]='\0';
     
     memset(sWhere,'\0',strlen(sWhere));
     sprintf(sWhere," where nxzbz=0 and nywlb=0 and cyhbz='%s' ",sYhbz);
   
     nRetVal=RunSelect("select count(*),min(cyhmc),min(cyhbz1) \
                        from dl$ywmx_%t %t into %ld%s%s",
                        sJym,sWhere,&ncount,sYhmc,sYhbz1);

     if(nRetVal<0)
     {
         GetSqlErrInfo(sWhere);
         ImportRecord(Client,"%s%s","000011",sWhere);
         return 0;
     }

     if(ncount<=0)
     {
         ImportRecord(Client,"%s%s","000001","该用户没有欠费或没有该用户信息");
         return 0;     
     }

     
     nId=OpenCursor("select cyhbz,nysnd,nysyf,cbl2,cbl5,cbl13,cbl7,\
                      dsfmx1,dsfmx2,dsfmx3,dsfmx5,dyjzk,\
                      to_number(to_date(%ld,'YYYYMMDD')-to_date(njscz2,'YYYYMMDD')) \
                      from dl$ywmx_%t %t order by -nysnd,-nysyf",
                      nSysLkrq,sJym,sWhere);
     
     if(nId<0)
     {
     	GetSqlErrInfo(sWhere);
     	ImportRecord(Client,"%s%s","200001",sWhere);
     	return 0;
     }
     
     ImportRecord(Client,"%s%s%s%lf%ld","000000",sYhmc,sYhbz1,0.0,ncount);
     while(ncount--)
     {
     	nRetVal=FetchCursor(nId,"%s%ld%ld%s%s%s%s"
     				"%lf%lf%lf%lf,%lf%ld",sYhbz,
    	                 &nysnd,&nysyf,jjlx,zssz,zssm,sHdbz,
    	                 &dsfmx[0],&dsfmx[1],&dsfmx[2],&dsfmx[3],
                         &dsfmx[25],&nDates);

        dZnj=0.0;
     	
     	ImportRecord(Client,"%s%ld(2)%lf[26]%ld[4]%s(4)",sYhbz,nysnd,nysyf,
     			    dsfmx,nValue,jjlx,zssz,zssm,sHdbz);

        nRetVal=RunSql("update dl$ywmx_%t set clsh=%s,cczydh=%s %t \
                        and nysnd=%ld and nysyf=%ld",
                        sJym,sLsh,sCzydh,sWhere,nysnd,nysyf);
        if(nRetVal<0) break;

     }
     CloseCursor(nId);
     if(nRetVal<0)
     {
     	GetSqlErrInfo(sWhere);
     	Client->scurrent=Client->sdata;
     	Client->nsize=0;
     	ImportRecord(Client,"%s%s","000011",sWhere);
        RollbackWork();
     }
     else CommitWork();
     return 0;
}

//国税发送数据包[ncxms=522]
static int dsTaxSendAndRecv(int nsock,char * sinbuf,char * soutbuf)
{
     int nerror;
     long nRetVal;
     
     if(nsock<0) nsock=connectTCP("dl_tx_10","tax");
     if(nsock<0) return -1;

     nRetVal=strlen(sinbuf);
     SendLongToSocket(nsock,(long)nRetVal);
     printf("sendbuf[%ld]=%s\n",nRetVal,sinbuf);
     
     if(SendRecordToSocket(nsock,sinbuf,(int)nRetVal)!=0)
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

static void GetTaxErrorInfo(char * sError,char * sInfo)
{
    char sErrCode[5]; 
    int nLen;
    char * sBegin;
    char sMsgTab[]= "1111:101111|对帐日期非当天日期|;"
                    "3333:103333|金额不对|;"
                    "5555:105555|申报序号不存在/信息文件不存在|;"
                    "6666:106666|该申报序号未打印税票|;"
                    "1001:101001|国税分配申报序号出错|;"
                    "1002:101002|交易码不存在|;"
                    "1003:101003|国税数据库操作出错|;"
                    "1004:101004|税票打印类型不存在|;"
                    "1005:101005|该纳税人并未登记或并不属于该银行|;"
                    "2001:102001|未找到纳税、税票信息|;"
                    "3001:103001|重复交易(银行流水不唯一)|;"
                    "3002:103002|税票不正常（票据类型或税票号为空）|;"
                    "3003:103003|国税局无该纳税人未申报记录|;"
                    "3004:103004|该税票已经被使用|;"
                    "3005:103005|非法税票状态|;"
                    "3006:103006|企业税票已打印|;"
                    "4001:104001|需抹帐的流水不存在|;"
                    "4002:104002|该笔流水已被取消(已抹帐，不能再次抹帐)|;"
                    "4003:104003|隔日无法抹帐|;"
                    "4004:104004|申报序号与税票信息不一致|;"
                    "4005:104005|汇总对帐税票总数不一致|;"
                    "4006:104006|汇总对帐汇总金额不一致|;"
                    "4007:104007|税票打印类型不是银行代征|;";

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

//立费查询
long dsTaskTax1Query(char * inbuf,RECORD * Client)
{
     char sBzmap[6],sJym[16],sLsh[41],sYhbz[101],sJsdh[10];
     char sYhmc[51],sYhbz1[41];//用户名称与合同号
     char sBuffer[1024],sBuffer1[1024];
     char sFh[5];
     char sBz[31],jjlx[21],zssz[11],zssm[11];
     char sCzydh[7],sBankCode[11],sKjny[7];
     long i,nmap,nId,nCount,nysnd,nysyf,nRetVal;
     double dsfmx[26],dZnj;
     long nValue[4],nDates,nCxms;
    
     FetchStringToArg(inbuf,"%s|%s|%s|%s|",sLsh,sBzmap,sYhbz,sCzydh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
    
     memset(dsfmx,0,sizeof(dsfmx));
     memset(nValue,0,sizeof(nValue));
     /*strncpy(sYhbz1,sYhbz+5,2);
     sYhbz[5]='\0';
     sYhbz1[2]='\0';*/
     
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=2 into %s",
     		sJym,sBankCode);
     
     if(((nSysLkrq%10000)/100)==1)
	sprintf(sKjny,"%4ld12",(nSysLkrq/10000)-1);
     else
	sprintf(sKjny,"%4ld%02ld",nSysLkrq/10000,(nSysLkrq%10000)/100-1);
     
     memset(sBuffer,0,sizeof(sBuffer));
     sprintf(sBuffer,"%08ld%06ld%-20.20s%-10.10s%-2.2s%-6.6s",
                     10004,38,sYhbz,sBankCode,"02",sKjny);
                          
    if(dsTaxSendAndRecv(-1,sBuffer,sBuffer))
    {
    	GetTaxErrorInfo(sBuffer,sBuffer);
	ApplyToClient("%s",sBuffer);
        return 0;
    }
    
	FetchStringToArg(sBuffer,"%4s",sFh);
	
	printf("\ntest:%s,%s",sFh,sBuffer);
	
  	if(atol(sFh)!=0)
  	{
  		GetTaxErrorInfo(sFh,sBuffer);
  		ApplyToClient("%s",sBuffer);
        	return 0;
    	}
      	alltrim(sYhbz);
    
    if(dsTaxYwmxRk(sLsh,sCzydh,sBuffer,&nCount)!=1)
    {
    	ApplyToClient("%s",sBuffer);
        return 0;
    }
    
    ImportRecord(Client,"%s%s%s%lf%ld","000000",swmx[0].cqymc,".",0.0,nCount);
    
    for(i=0;i<nCount;i++)
    {
    	dsfmx[0]=swmx[i].dkssl;
		dsfmx[1]=swmx[i].dxssr;
		dsfmx[2]=swmx[i].dsl;
		dsfmx[3]=swmx[i].dsjje;
		dsfmx[25]=swmx[i].dsjje;
    
		printf("\ntest:%lf,%lf,%s,%s,%s",
					swmx[i].dxssr,swmx[i].dsjje,swmx[i].czclx,swmx[i].cszmc,swmx[i].cpmmc);
       
		ImportRecord(Client,"%s%ld(2)%lf[26]%ld[4]%s(4)",
						swmx[i].cnsrbm,swmx[i].nysnd,swmx[i].nysyf,
						dsfmx,nValue,
						swmx[i].czclx,swmx[i].cszmc,swmx[i].cpmmc,swmx[i].chdbz);

		/*  nRetVal=RunSql("update dl$ywmx_%t set clsh=%s,cczydh=%s %t \
                        and nysnd=%ld and nysyf=%ld",
                        sJym,sLsh,sCzydh,sWhere,swmx.nysnd,swmx.nysyf);
		*/
	}
	return 0;
    
}

//交费提交
long dsTaskTaxPayConfirm(char * inbuf,char * outbuf)
{
    char sLsh[41]=".";  //流水号
    char sYhbz[101]=".",sStation[10]=".",sUser[10]="."; //用户标志
    char sJym[16]=".";  //交易码
    char sYhmc[51]="."; //用户名称
    char sCzydh[7]="."; //操作员代号
    char sJsdh[10]="."; //缴费局局号
    char sJh[7]=".";
    char sSjh[10],sSbxh[20]=".";
    char sBuffer[1024]=".";
    char * sOffset;
    long nMode,nFlag,nCxms;     //缴款方式 1：现金  2：绿卡
    long i=0,nCount=0,nCount1=0,nxh,nId,nRet;
    long nLkrq,nSfm;
    double dSjk;
    char sBankCode[11]=".",sKhh[41]=".";
    char sFh[5]=".";
    
    SWMX swmx;
     
    sOffset=FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|%d|",
                       sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSysSfm,&nCount);

    FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
    FetchStringToArg(inbuf,"%6s",sJh);
    GenerateLsh(3,sJsdh,sJym,sBuffer);
    nxh=atol(sBuffer);
 
    RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=2 into %s",
					sJym,sBankCode);
    RunSelect("select count(*) from dl$ywmx_%t where clsh=%s into %ld",
					sJym,sLsh,&nCount1);
    if(nCount!=nCount1)
    {
	strcpy(outbuf,"101111|业务明细表记录错误|");
	return -1;
    }
	
    nId=OpenCursor("select cbl6,cbl16,cbl1,nlkrq,nsfm,dsfmx1,dsfmx2,dsfmx3,dsfmx5 from dl$ywmx_%t where clsh=%s ",sJym,sLsh);
    if(nId<1)
    {
    	strcpy(outbuf,"100111|提取数据失败|");
	return -1;
    }
    
    for(;;)
    {
	memset(&swmx,0,sizeof(SWMX));
	sSbxh[0]='\0';sKhh[0]='\0';
	nRet=FetchCursor(nId,"%s%s%s%ld%ld,%lf%lf%lf%lf",sSbxh,sKhh,swmx.czspl,&nLkrq,&nSfm,
    				&swmx.dkssl,&swmx.dxssr,&swmx.dsl,&swmx.dsjje);
    	if(nRet<0)
    	{
    		CloseCursor(nId);
    		strcpy(outbuf,"111000|fetch数据失败|");
		return -1;
	}
	if(nRet==0 || swmx.dsjje==0) break;
		
	sSjh[0]='\0';
    	sOffset=FetchStringToArg(sOffset,"%s|",sSjh);

    	if(RunSql("update dl$ywmx_%t set cbl8=%s where clsh=%s and cbl6=%s ",
    								sJym,sSjh,sLsh,sSbxh)>=0) CommitWork();
    	
    	//形成缴费提交报文
		sBuffer[0]='\0';
		sprintf(sBuffer,"%08ld%06ld%-12.12s%-10.10s%-2.2s%-2.2s%8ld%06ld"
				"%-20.20s%-9.3lf%-11.2lf%-10.3lf%-11.2lf"
    				"%-40.40s%-20.20s%-20.20s",
                    	10001,181,sSbxh,sBankCode,"02",swmx.czspl,nLkrq,nSfm,
                    	sSjh,swmx.dkssl,swmx.dxssr,swmx.dsl,swmx.dsjje,
                    	sKhh,".",sJh);
        
        if(dsTaxSendAndRecv(-1,sBuffer,sBuffer))
    	{
    		CloseCursor(nId);
		strcpy(outbuf,"111111|国税方收费未知,请重新查询..|");
        	CommitWork();
   		return -1;
    	}
    
    	FetchStringToArg(sBuffer,"%4s",sFh);
    	if(atol(sFh)!=0)
  	{
  		CloseCursor(nId);
  		GetTaxErrorInfo(sFh,outbuf);
		RollbackWork();
       	 	return 0;
    	}
    }
    CloseCursor(nId);

    //插入帐务明细记录
    if(InsertZwmxTable(inbuf,TASK_PAY_CONFIRM,nxh)<0)
    {
	RollbackWork();
	GetSqlErrInfo(outbuf);
       	return 0;
    }
    //修改业务明细
    if(RunSql("update dl$ywmx_%t set nxzbz=1,dsjk=dsfmx5 where clsh=%s ",sJym,sLsh)<0)
    {
	RollbackWork();
	GetSqlErrInfo(outbuf);
       	return 0;
    }
    CommitWork();

    sprintf(outbuf,"000000|%s|%.2lf|",sLsh,dSjk);  //正确
    return 0;
}

long dsTaxRecallConfirm(char * inbuf,long nxh,char * outbuf)
{
	char sLsh[41],sRecallLsh[41];  //流水号
	char sJym[16];  //交易码
	char sCzydh[7]; //操作员代号
	char sJsdh[10]; //缴费局局号
	char sSjh[21];
	char sBuffer[1024];
   
	double dSjk;
	long nResult,nXzbz=2,nId=0,nRet=0;
   
	char sBankCode[11];
	char sFh[5];
    
	SWMX swmx;
	
	memset(&swmx,0,sizeof(SWMX));
	strcpy(outbuf,"111111|撤销失败|");
	
	FetchStringToArg(inbuf,"%s|%s|%s|%d|",sLsh,sRecallLsh,sCzydh,&nSysSfm);
	FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
	
	RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=2 into %s",
				sJym,sBankCode);

	//每张发票都需要撤销
	nId=OpenCursor("select cbl6,cbl1,cbl8 "
					"from dl$ywmx_%t where clsh=%s and dsjk!=0",
					sJym,sRecallLsh);
	if(nId<1)
	{
		strcpy(outbuf,"111001|打开游标失败|");
		return 0;
	}
		
    for(;;)
    {
    	swmx.csbxh[0]='\0';
    	swmx.czspl[0]='\0';
    	sSjh[0]='\0';
    	nRet=FetchCursor(nId,"%s,%s,%s",swmx.csbxh,swmx.czspl,sSjh);
    	if(nRet<0)
    	{
    		CloseCursor(nId);
    		strcpy(outbuf,"111111|fetch游标失败|");
    		return 0;
    	}
    	if(nRet==0) break;

		sBuffer[0]='\0';
		sprintf(sBuffer,"%08ld%06ld%-12.12s%-10.10s%-2.2s%-2.2s%-20.20s",
                    10003,46,swmx.csbxh,sBankCode,"02",swmx.czspl,sSjh);
		if(dsTaxSendAndRecv(-1,sBuffer,sBuffer))
		{
			CloseCursor(nId);
			strcpy(outbuf,"111111|与邮政通信机通信失败|");
			return 0;
		}
	
		sFh[0]='\0';
		FetchStringToArg(sBuffer,"%4s",sFh);
		if(atol(sFh)!=0)
		{
			CloseCursor(nId);
			GetTaxErrorInfo(sFh,outbuf);
			return 0;
		}
	}
	CloseCursor(nId);

    //修改业务明细
	if(dsGeneralRecallConfirm(inbuf,nxh,outbuf)<0)
	{
		RollbackWork();
		GetSqlErrInfo(sBuffer);
		ApplyToClient("%s",sBuffer);
        	return 0;
	}

	strcpy(outbuf,"000000|撤销成功|");
	return 0;
}



long dsTaxCheckAccount(char * inbuf,char * outbuf)
{
	long nCount=0,nLkrq=0;
	char sBuffer[1024]=".";
	char sBankCode[11],sJh[7]=".",sFh[5]=".";
	char sTemp[100]=".";
	
	double dSjk,dSum=0.0;
	double dSjk0=0,dSjk1=0;
	long nId,nRetVal;
   
	GetSysLkrq();
	outbuf[0]='\0';

	RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=2 into %s",
     		inbuf,sBankCode);
		
	//FetchStringToArg(inbuf,"%9s%6s",sTemp,sJh);
	RunSelect("select min(nlkrq) from dl$zwmx where ncsbz=1 and cjym=%s into %ld",inbuf,&nLkrq);
   
	nId=OpenCursor("select distinct substr(cjh,1,6) from dl$zwmx where cjym=%s and \
					nlkrq=%ld and ncsbz=1 order by substr(cjh,1,6) ",inbuf,nLkrq);
	
	if(nId<0) { CloseCursor(nId); strcpy(outbuf,"111100|打开游标错|");  return -1;}
	
	while(1)
	{		 		
		nRetVal=FetchCursor(nId,"%s",sJh);
		if(nRetVal<=0) break;
		
		nCount=0;dSum=0;
		if(RunSelect("select count(distinct cbl6),sum(dsjk) from dl$ywmx_%t "
					"where nlkrq=%ld and nxzbz=1 and dsjk>0.01 and substr(cjh,1,6)=%s "
					" into %ld%lf",
					inbuf,nLkrq,sJh,&nCount,&dSum)<0)
		{
			strcpy(outbuf,"111000|查询业务明细出错|");
			return 0;
		}
	   
		sprintf(sBuffer,"%08ld%06ld%-10.10s%-20.20s%8ld%-2.2s%8d%16.2lf",
	                    10006,64,sBankCode,sJh,nLkrq,"02",nCount,dSum);
	    
	    if(dsTaxSendAndRecv(-1,sBuffer,sBuffer))
	    {
	    	strcpy(outbuf,"111111|与邮政通信机通信失败|");
   		return 0;
	    }
		
		FetchStringToArg(sBuffer,"%4s",sFh);
		printf("\n对帐数据:局号=%s,数量=%ld,总款=%.2lf,返回值:%s",sJh,nCount,dSum,sFh);
	    if(atol(sFh)!=0)  sprintf(outbuf,"%s[%s]",outbuf,sJh);
	    else {
	    	RunSql("update dl$zwmx set ncsbz=2 where cjym=%s and nlkrq=%ld and substr(cjh,1,6)=%s",inbuf,nLkrq,sJh);
	    	RunSql("update dl$jymcsfb set ccs='%ld' where cjym=%s and nbh=34",nLkrq,inbuf);
	 	}
	 	
    }//end while
    CloseCursor(nId);
    if(strlen(outbuf)>5) sprintf(outbuf,"%s|对账失败局(日期:%ld)|",outbuf,nLkrq);
    else sprintf(outbuf,"000015|对帐成功(对帐日期:%ld)|",nLkrq);

    CommitWork();
    return 0;
}

static long dsTaxYwmxRk(char *sLsh,char *sCzydh,char * sBuffer,long *nCount)
{

    long i,nYwlb,nYsnd,nYsyf,nXh,nId=1,nKey,nKm,nXjrq,nSkrq;
    long nSzgs;
    char sFh[10],sSbxh[13];
    char sDump[81],sKssl[12];
    char sZsjg[31],sDylx[3],sZspl[3],sHdbz[2],sSwjg[31],sDsgx[31];
    char sZclx[31],sDjh[21],sQydz[51],sJz[2],sSzmc[31],sPmgs[3],sPmmc[41];
    char sKmbm[8]=".",sKmmc[41]=".",sJkgk[31]=".";
    char sKhh[41]=".",sZh[31]=".";
    double dKssl=0,dXssr,dSl,dDqje,dSjje;
    char sYhmc[81]=".",sYhbz1[61]=".";
    char sJsdh[10],sJym[16];
    char sTemp[41]=".",sXh[6]=".";
    char * offset;
    
    GetSysLkrq();
    
    if(swmx) free(swmx);
    swmx=NULL;

    alltrim(sBuffer);alltrim(sLsh);
    
    FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
	FetchStringToArg(sLsh,"%35s%5s",sTemp,sXh);
	nXh=atol(sXh);
	
	offset=FetchStringToArg(sBuffer,
			"%4s%2s%1s%30s%30s%30s%30s%20s%20s%50s%50s"
			"%4d%2d%30s%8d%8d%1d",
			sFh,sZspl,sHdbz,sSwjg,sDsgx,sZclx,sZsjg,sYhbz1,sDjh,sYhmc,sQydz,
			&nYsnd,&nYsyf,sJkgk,&nXjrq,&nSkrq,&nSzgs);

	swmx=(SWMX *)malloc(nSzgs*sizeof(SWMX));
	
	if(RunSelect("select ckhh,cskgk,czh from dl$khhgl where cjsdh=%s into %s,%s,%s",
					sJsdh,sKhh,sJkgk,sZh)<0)
	{
		strcpy(sBuffer,"111102|查询dl$khhgl表出错!");
		return 0;
	}
	else
	{
		strcpy(sKhh,".");
		strcpy(sJkgk,".");
		strcpy(sZh,".");
	}

	for(i=0;i<nSzgs;i++)
	{
		offset=FetchStringToArg(offset,
			"%12s%30s"
			"%6s%40s%30d%1s%1s%40s%9s"
			"%11f%10f%11f%11f",
			sSbxh,sSzmc,
			sKmbm,sKmmc,&nKm,sJz,sPmgs,sPmmc,sKssl,
			&dXssr,&dSl,&dDqje,&dSjje);
		
		/*printf("\nmx:%ld,%s,%s,%s,%s,%s,%s,%s,%s,%s,"
				"\n%ld,%ld,%s,%ld,%ld,"     
				"\n%ld,%s,%s,"
				"\n%s,%s,%ld,%s,%s,%s,%s,"
				"\n%lf,%lf,%lf,%lf",
				nXh,sZspl,sHdbz,sSwjg,sDsgx,sZclx,sZsjg,sDjh,sYhmc,sQydz,
				nYsnd,nYsyf,sJkgk,nXjrq,nSkrq,
				nSzgs,sSbxh,sSzmc,
				sKmbm,sKmmc,nKm,sJz,sPmgs,sPmmc,sKssl,
				dXssr,dSl,dDqje,dSjje);*/

		//if(strncmp(sSzmc,"01",2) && strncmp(sSzmc,"02",2) ) continue;
		strcpy(sDylx,".");
		sSzmc[19]='\0';
		if(sSwjg[0]=='\0') strcpy(sSwjg,".");
		if(sDsgx[0]=='\0') strcpy(sDsgx,".");
		if(sZclx[0]=='\0') strcpy(sZclx,".");
		if(sZsjg[0]=='\0') strcpy(sZsjg,".");
		if(sQydz[0]=='\0') strcpy(sQydz,".");
		
		if(strlen(sKmbm)<1) strcpy(sKmbm,".");
		if(strlen(sKmmc)<1) strcpy(sKmmc,".");
		if(sDsgx[0]=='\0') strcpy(sDsgx,".");
		if(sZsjg[0]=='\0') strcpy(sZsjg,".");
		if(sSbxh[0]=='\0') strcpy(sSbxh,".");
		dKssl=atol(sKssl);
        
	//	nSysLkrq=20030401;
		
		nId=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,"
				"nywlb,nxh,"
				"dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,"
				"dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,dsfmx18,dsfmx19,dsfmx20,"
				"dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,njscz2,djscz3,djscz4,nsl,"
				"dyjzk,dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,dbl1,dbl2,"
				"cbl1,cbl2,cbl3,cbl4,cbl5,cbl6,cbl7,cbl8,"
				"cbl9,cbl10,cbl11,cbl12,cbl13,cbl14,cbl15,cbl16,cbl17,cbl18,"
				"cjh,cczydh,nlkrq,nsfm,clxbz,nxzbz,ncsbz,cbz,cylsh,clsh) "
			"VALUES(%s,%ld,%ld,%s,%s,%s,%s,%ld,%ld,"
				"%lf,%lf,%lf,%lf,%lf,"
				"0,0,0,0,0,"
				"0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'.',%ld,0,0,1,%lf,0,0,%lf,0,0,%ld,%ld,%ld,"
				"0,0,"
				"%s,%s,%s,'.',%s,%s,%s,'.',"
				"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,"
				"%s,%s,%ld,%ld,'0000000000',0,0,'.','.',%s)",
				sJym,sJym,nYsnd,nYsyf,sYhbz1,sYhbz1,sDylx,sYhmc,
				0,nXh,
				dKssl,dXssr,dSl,dDqje,dSjje,
				nSysLkrq,dSjje,dSjje,nKm,nXjrq,nSkrq,
				sZspl,sZclx,sDjh,sSzmc,sSbxh,sHdbz,
				sKmmc,sKmbm,sZsjg,sPmgs,sPmmc,sSwjg,sQydz,sKhh,sZh,sJkgk,
				sJsdh,sCzydh,nSysLkrq,nSysSfm,sLsh);
        
		GetSysLkrq();
		
		if(nId<0)
		{
			strcpy(sBuffer,"111111|插入业务明细出错");
			RollbackWork();
			return 0;
		}
		
		strcpy(swmx[i].chdbz,sHdbz);
		strcpy(swmx[i].cqymc,sYhmc);
		strcpy(swmx[i].cnsrbm,sYhbz1);
		swmx[i].nysnd=nYsnd;
		swmx[i].nysyf=nYsyf;
	
		swmx[i].dkssl=dKssl;
		swmx[i].dxssr=dXssr;
		swmx[i].dsl=dSl;
		swmx[i].ddqje=dDqje;
		swmx[i].dsjje=dSjje;

		strcpy(swmx[i].czclx,sZclx);
		strcpy(swmx[i].cszmc,sSzmc);
		strcpy(swmx[i].cpmmc,sPmmc);
		
	}
	
	CommitWork();
	*nCount=nSzgs;
	return 1;
}

/********
参数说明：
  入口参数：
    sourdate:以整型表示8位日期，是基准日期点
    numberday:间隔天数；
    flag:以基准日期为准，向前向后确定目标日期，-1表示比基准日期小（历史日期），
        1是表示未来日期；

**********/

long GetRandomDate(long sourdate,long numberday,long flag)
{
	char sdescdate[10];
	long id=0;
	
	sdescdate[0]='\0';
	
	id=RunSelect("select to_char(to_date(to_char(%ld),'yyyymmdd')+%ld*%ld,'yyyymmdd') "
					"from dual into %s",sourdate,numberday,flag,sdescdate);
	alltrim(sdescdate);
	
	if( id<=0)  return -1;
	return atol(sdescdate);
}	   

//地税查询
long dsTaskTerraQuery(char * inbuf,RECORD * Client)
{
     char sBzmap[6],sJym[16],sLsh[41],sYhbz[101],sJsdh[10];
     char sYhmc[61],sYhbz1[41],sCzydh[8];//用户名称与合同号
     char sWhere[400];
     char zclx[21],sz[21],spbz[9];
     long nmap,nId,ncount,nysnd,nysyf,nRetVal;
     double dsfmx[26],dZnj;
     long nValue[4],nDates,nCxms;

     FetchStringToArg(inbuf,"%s|%s|%s|%s|",sLsh,sBzmap,sYhbz,sCzydh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
    
     memset(dsfmx,0,sizeof(dsfmx));
     memset(nValue,0,sizeof(nValue));
     sYhbz[20]='\0';
     alltrim(sYhbz);
     
     memset(sWhere,'\0',strlen(sWhere));
     sprintf(sWhere," where nxzbz=0 and nywlb=0 and cyhbz='%s' ",sYhbz);
   
     nRetVal=RunSelect("select count(*),min(cyhmc),min(cyhbz2) \
                        from dl$ywmx_%t %t into %ld%s%s",
                        sJym,sWhere,&ncount,sYhbz1,sYhmc);

     if(nRetVal<0)
     {
         GetSqlErrInfo(sWhere);
         ImportRecord(Client,"%s%s","000011",sWhere);
         return 0;
     }

     if(ncount<=0)
     {
         ImportRecord(Client,"%s%s","000001","该用户没有欠费或没有该用户信息");
         return 0;     
     }

     
     nId=OpenCursor("select cyhbz,nysnd,nysyf,cbl1,cbl3,cbl5,\
                      dsfmx1,dsfmx2,dsfmx3,dsfmx5,dyjzk,\
                      to_number(to_date(%ld,'YYYYMMDD')-to_date(njscz2,'YYYYMMDD')) \
                      from dl$ywmx_%t %t order by -nysnd,-nysyf",
                      nSysLkrq,sJym,sWhere);
     
     if(nId<0)
     {
     	GetSqlErrInfo(sWhere);
     	ImportRecord(Client,"%s%s","200001",sWhere);
     	return 0;
     }
     
     ImportRecord(Client,"%s%s%s%lf%ld","000000",sYhmc,sYhbz1,0.0,ncount);
     while(ncount--)
     {
     	nRetVal=FetchCursor(nId,"%s%ld%ld%s%s%s"
     				"%lf%lf%lf%lf,%lf%ld",sYhbz,
    	                 &nysnd,&nysyf,zclx,sz,spbz,
    	                 &dsfmx[0],&dsfmx[1],&dsfmx[2],&dsfmx[3],
                         &dsfmx[25],&nDates);

        dZnj=0.0;
     	sz[10]='\0';
     	ImportRecord(Client,"%s%ld(2)%lf[26]%ld[4]%s(4)",sYhbz,nysnd,nysyf,
     			    dsfmx,nValue,zclx,sz,'.',spbz);

        nRetVal=RunSql("update dl$ywmx_%t set clsh=%s,cczydh=%s %t \
                        and nysnd=%ld and nysyf=%ld",
                        sJym,sLsh,sCzydh,sWhere,nysnd,nysyf);
        if(nRetVal<0) break;

     }
     CloseCursor(nId);
     if(nRetVal<0)
     {
     	GetSqlErrInfo(sWhere);
     	Client->scurrent=Client->sdata;
     	Client->nsize=0;
     	ImportRecord(Client,"%s%s","000011",sWhere);
        RollbackWork();
     }
     else CommitWork();
     return 0;
}
