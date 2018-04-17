/*=========================================================
函数：dsTaskCommitPurchase   TASK_COMMIT_PURCHASE
功能：操作员进货记录入库
参数：sCzydh:操作员代号
返回：0：成功，否则失败
      [返回RECORD]
      成功：000000|操作员代号|
      失败：XXXXXX|失败信息文字说明|
*========================================================*/
#include<stdio.h>
#include "t_hwgl.h"

extern long nSysLkrq;
extern long nSysSfm;


long dsTaskCommitPurchase(char * sCzydh)
{

    char sInfo[201],sSqlErr[81],sLsh[41];
    long nCount,nId,nRetVal;

    nId=OpenCursor("select cLsh from dl$hwjcb where nczlb=1 and njlzt=0 and \
                    cjhczydh=%s",sCzydh);

    if(nId<0)
    {
    	GetSqlErrInfo(sSqlErr);
    	sprintf(sInfo,"000011|打开游标出错:%s|",sSqlErr);
    	return ApplyToClient("%s",sInfo);
    }
    
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%s",sLsh);
    	if(nRetVal==0)
    	{
    		nRetVal=1;
    		break;
    	}
        //修改记录状态为已入库存
        if(nRetVal>0)
               nRetVal=RunSql("update dl$hwjcb set njlzt=1 where cLsh=%s",sLsh);
        //根据流水帐记录修改库存量
        if(nRetVal>0)  
               nRetVal=ProcessRecord(sLsh);
        if(nRetVal<=0) break;        	      	

    }
    CloseCursor(nId);
    if(nRetVal<=0)
    {
    	GetSqlErrInfo(sSqlErr);
    	RollbackWork();
    	sprintf(sInfo,"000011|处理流水数据出错:%s|",sSqlErr);
    	return ApplyToClient("%s",sInfo);
    }
    CommitWork();
    sprintf(sInfo,"000000|%s|",sCzydh);
    return ApplyToClient("%s",sInfo);

}


static long ProcessRecord(char * sLsh)
{
    
    char sJym[16],sFhczydh[21],sJhczydh[21];
    long nCzlb,nCount,identify,nstatus;
    long nRetVal=1;
    
    
    GetSysLkrq();
    if(RunSelect("select nczlb,cjym,nsl,cfhczydh,cjhczydh,\
                  to_number(substr(clxbz,1,1)),njlzt from \
                  dl$hwjcb where clsh=%s into %ld%s%ld%s%s%ld%ld",
                  sLsh,&nCzlb,sJym,&nCount,sFhczydh,sJhczydh,&identify,&nstatus)<=0) return -1;
    
    //检查库存记录是否存在，其不存在则建立一个空白记录
    if(nCzlb!=7&&nCzlb!=2&&nCzlb!=6)
          if(CheckOrInsertStock(sJhczydh,sJym,identify)<0) return -1;            
                 
    switch(nCzlb)
    {
    	case 1://进货,增加本操作员库存
    	     switch(nstatus)
    	     {
    	     	case 1:
                     return RunSql("update dl$hwkcb set nkcl=nkcl+%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz=%s and cjym=%s",
                            nCount,nSysLkrq,nSysSfm,sJhczydh,sJym);
             }
             break;

        case 3://发货、接货
             if(CheckOrInsertStock(NULL,sJym,identify)<0) return -1;
             switch(nstatus)
             {
             	case 1://确认发货，减少本操作员库存，增加公用库存[PUBLIC]
                   nRetVal=RunSql("update dl$hwkcb set nkcl=nkcl-%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz=%s and cjym=%s",
                            nCount,nSysLkrq,nSysSfm,sFhczydh,sJym);
                   if(nRetVal>0)
                      return RunSql("update dl$hwkcb set nkcl=nkcl+%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz='PUBLIC' and cjym=%s and substr(clxbz,1,1)='1'",
                            nCount,nSysLkrq,nSysSfm,sJym);
                   return nRetVal;
                   
                case 2://接受发货，增加本操作员库存，减少公用库存[PUBLIC]
                   nRetVal=RunSql("update dl$hwkcb set nkcl=nkcl+%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz=%s and cjym=%s",
                            nCount,nSysLkrq,nSysSfm,sJhczydh,sJym);
                   if(nRetVal>0)
                      return RunSql("update dl$hwkcb set nkcl=nkcl-%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz='PUBLIC' and cjym=%s and substr(clxbz,1,1)='1'",
                            nCount,nSysLkrq,nSysSfm,sJym);
                   return nRetVal;
             }                     
             break;

       case 2://向委托方退货
       case 6://报损
       case 7://售出
             return RunSql("update dl$hwkcb set nkcl=nkcl-%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz=%s and cjym=%s",
                            nCount,nSysLkrq,nSysSfm,sFhczydh,sJym);
                            
       case 8://客户退货
             return RunSql("update dl$hwkcb set nkcl=nkcl+%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz=%s and cjym=%s",
                            nCount,nSysLkrq,nSysSfm,sJhczydh,sJym);
                            
    }                        
        
}

static long CheckOrInsertStock(char * sCzydh,char * sJym,long identfy)
{

    char cLxbz[11]="0000000000";
    char sJsdh[10]=".";
    char sHwsyz[21];
    long nRetVal;
    
   
    if(sCzydh)
    {
    	 nRetVal=RunSelect("select * from dl$hwkcb where csyz=%s and cjym=%s",sCzydh,sJym);
    	 cLxbz[0]='0';  //私有的
    	 strcpy(sHwsyz,sCzydh);
    }
    else 
    {
    	 nRetVal=RunSelect("select * from dl$hwkcb where cjym=%s and \
    	                    substr(clxbz,1,1)='1'",sJym);
         cLxbz[0]='1';  //公有的
    	 strcpy(sHwsyz,"PUBLIC");
    }
    
    if(nRetVal>0) return 1;     //已存在库存记录

    RunSelect("select cjh from dl$czyb where cczydh=%s into %s",sHwsyz,sJsdh); 
    //否则插入一条空白记录
    return  RunSql("insert into dl$hwkcb(cjym,csyz,nkcl,nlkrq,nsfm,clxbz,cjh,nbl,dbl,cbl) \
                    values(%s,%s,0,%ld,%ld,%s,%s,0,0.0,'0')",
                    sJym,sHwsyz,nSysLkrq,nSysSfm,cLxbz,sJsdh);

}


/*=========================================================
函数：dsTaskCommitDistribute  TASK_COMMIT_DISTRIBUTE
功能：操作员分货记录入库
参数：sCzydh:操作员代号
返回：0：成功，否则失败
      [返回RECORD]
      成功：000000|操作员代号|
      失败：XXXXXX|失败信息文字说明|
*========================================================*/


long dsTaskCommitDistribute(char * sCzydh)
{
    char sInfo[201],sSqlErr[81],sLsh[41],sHwbz[21],sJym[16];
    long nCount,nId,nRetVal,identify;

    nId=OpenCursor("select cLsh,to_number(substr(clxbz,1,1)),chwbz,cjym from \
                    dl$hwjcb where nczlb=3 and njlzt=0 and cfhczydh=%s",sCzydh);

    if(nId<0)
    {
    	GetSqlErrInfo(sSqlErr);
    	sprintf(sInfo,"000011|打开游标出错:%s|",sSqlErr);
    	return ApplyToClient("%s",sInfo);
    }
    
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%s%ld%s%s",sLsh,&identify,sHwbz,sJym);
    	if(nRetVal==0)
    	{
    		nRetVal=1;
    		break;
    	}
    	
        //修改记录状态为已入库存
        if(nRetVal>0)
        {
               nRetVal=RunSql("update dl$hwjcb set njlzt=1 where cLsh=%s",sLsh);
               if(nRetVal>0&&identify)
                   nRetVal=RunSql("update dl$hwjcb set njlzt=3 where cjhczydh=%s \
                                   and chwbz=%s and cjym=%s and (nczlb=1 or nczlb=3)",
                                   sCzydh,sHwbz,sJym);
        }                           
               
        //根据流水帐记录修改库存量
        if(nRetVal>0)  
               nRetVal=ProcessRecord(sLsh);
        if(nRetVal<=0) break;        	      	

    }
    CloseCursor(nId);
    if(nRetVal<=0)
    {
    	GetSqlErrInfo(sSqlErr);
    	RollbackWork();
    	sprintf(sInfo,"000011|处理流水数据出错:%s|",sSqlErr);
    	return ApplyToClient("%s",sInfo);
    }
    CommitWork();
    sprintf(sInfo,"000000|%s|",sCzydh);
    return ApplyToClient("%s",sInfo);

}


/*=========================================================
函数：dsTaskAcceptDistribute  TASK_ACCEPT_DISTRIBUTE
功能：操作员接受分货
参数：sLsh:发货流水号
返回：0：成功，否则失败
      [返回RECORD]
      成功：000000|流水号|
      失败：XXXXXX|失败信息文字说明|
*========================================================*/

long dsTaskAcceptDistribute(char * sLsh)
{

     char sbuf[21],sTemp[101];
     long nRetVal;
     

     nRetVal=RunSql("update dl$hwjcb set njlzt=2 where clsh=%s and njlzt=1",sLsh);
     if(nRetVal>0)  nRetVal=ProcessRecord(sLsh);
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sbuf);
     	RollbackWork();
     	sprintf(sTemp,"000011|%s|",sbuf);
     }
     else
     {
     	CommitWork();
     	sprintf(sTemp,"000000|%s|",sLsh);
     }
     return ApplyToClient("%s",sTemp);	   	
      
}

/*=========================================================
函数：dsTaskSellGoods   TASK_SELL_GOODS
功能：销售入库任务
参数：sLsh:流水号
      sCzydh:操作员代号
      sHwbz:货物标志
      nCount:数量
返回：0：成功，否则失败
      [返回RECORD]
      成功：000000|流水号|
      失败：XXXXXX|失败信息文字说明|
*========================================================*/
long dsTaskSellGoods(char * sLsh,char * sCzydh,char * sHwbz,long nCount)
{

     char sJym[16],sbuf[81],sJymc[51]=".";
     char sJsdh[10],cLxbz[21]="00000000000000000000";
     long identify=0,nRetVal;
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     RunSelect("select to_number(substr(clxbz,5,1)),cjymc from dl$jym \
                where cjym=%s into %ld%s",sJym,&identify,sJymc);
     nRetVal=1;
     if(identify)
     {
     	   nRetVal=RunSql("update dl$hwjcb set njlzt=3 where cjym=%s and cjhczydh=%s \
                     and chwbz=%s and ((nczlb=1 and njlzt=1) or (nczlb=3 and njlzt=2))",
                     sJym,sCzydh,sHwbz);
           cLxbz[0]='1';
     }
        

     if(nRetVal>0)
         nRetVal=RunSql("insert into dl$hwjcb(cjym,cjymc,clsh,djhdj,nsl,chwbz,\
                        cfhczydh,cjhczydh,cfhjh,cjhjh,njhrq,njhsfm,nfhrq,nfhsfm,\
                        nczlb,njlzt,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,clxbz) \
                        values(%s,%s,%s,0,%ld,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,\
                        7,1,0,0,0,0,'0','0',%s)",
                        sJym,sJymc,sLsh,nCount,sHwbz,sCzydh,"客户",sJsdh,
                        ".",nSysLkrq,nSysSfm,nSysLkrq,nSysSfm,cLxbz);
     
     if(nRetVal>0) nRetVal=ProcessRecord(sLsh);
     
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sJymc);
     	RollbackWork();
     	sprintf(sbuf,"000011|%s|",sJymc);
     	return ApplyToClient("%s",sbuf);
     }
     
     CommitWork();
     sprintf(sbuf,"000000|%s|",sLsh);
     return ApplyToClient("%s",sbuf);

}


/*=========================================================
函数：dsTaskRecallGoods   TASK_RECALL_GOODS
功能：客户入库任务
参数：sLsh:流水号
      sCzydh:操作员代号
      sHwbz:货物标志
      nCount:数量
返回：0：成功，否则失败
      [返回RECORD]
      成功：000000|流水号|
      失败：XXXXXX|失败信息文字说明|
*========================================================*/
long dsTaskRecallGoods(char * sLsh,char * sCzydh,char * sHwbz,long nCount)
{

     char sJym[16],sbuf[81],sJymc[51]=".",sYlsh[41]=".";
     char sJsdh[10],cLxbz[21]="00000000000000000000";
     long identify=0,nRetVal,nCzlb,nJlzt;
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     RunSelect("select to_number(substr(clxbz,5,1)),cjymc from dl$jym \
                where cjym=%s into %ld%s",sJym,&identify,sJymc);

     if(identify)
     {
          
           nRetVal=RunSelect("select nczlb,clsh from dl$hwjcb where cjym=%s and cjhczydh=%s \
                      and chwbz=%s and ((nczlb=1 and njlzt=3) or (nczlb=3 and njlzt=3)) \
                      into %ld%s",sJym,sCzydh,sHwbz,&nCzlb,sYlsh);

           if(nCzlb==1) nJlzt=1;
           else nJlzt=2;
           
     	   if(nRetVal>0)
     	      nRetVal=RunSql("update dl$hwjcb set njlzt=%ld where clsh=%s",nJlzt,sYlsh);

           cLxbz[0]='1';
     }

     nRetVal=1;
     if(nRetVal>0)
         nRetVal=RunSql("insert into dl$hwjcb(cjym,cjymc,clsh,djhdj,nsl,chwbz,\
                        cfhczydh,cjhczydh,cfhjh,cjhjh,njhrq,njhsfm,nfhrq,nfhsfm,\
                        nczlb,njlzt,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,clxbz) \
                        values(%s,%s,%s,0,%ld,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,\
                        8,1,0,0,0,0,'0','0',%s)",
                        sJym,sJymc,sLsh,nCount,sHwbz,"客户",sCzydh,".",sJsdh,
                        nSysLkrq,nSysSfm,nSysLkrq,nSysSfm,cLxbz);
     
     if(nRetVal>0) nRetVal=ProcessRecord(sLsh);
     
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sJymc);
     	RollbackWork();
     	sprintf(sbuf,"000011|%s|",sJymc);
     	return ApplyToClient("%s",sbuf);
     }
     CommitWork();
     sprintf(sbuf,"000000|%s|",sLsh);
     return ApplyToClient("%s",sbuf);
}

/*=========================================================
函数：dsTaskWasteGoods   TASK_WASTE_GOODS
功能：货物报损
参数：sLsh:流水号
      sCzydh:操作员代号
      sHwbz:货物标志
      nCount:数量
返回：0：成功，否则失败
      [返回RECORD]
      成功：000000|流水号|
      失败：XXXXXX|失败信息文字说明|
*========================================================*/
long dsTaskWasteGoods(char * sLsh,char * sCzydh,char * sHwbz,long nCount)
{
     char sJym[16],sbuf[81],sJymc[51]=".";
     char sJsdh[10],cLxbz[21]="00000000000000000000";
     long identify=0,nRetVal;
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     RunSelect("select to_number(substr(clxbz,5,1)),cjymc from dl$jym \
                where cjym=%s into %ld%s",sJym,&identify,sJymc);
     nRetVal=1;
     if(identify)
     {
     	   nRetVal=RunSql("update dl$hwjcb set njlzt=3 where cjym=%s and cjhczydh=%s \
                     and chwbz=%s and ((nczlb=1 and njlzt=1) or (nczlb=3 and njlzt=2))",
                     sJym,sCzydh,sHwbz);
           cLxbz[0]='1';
     }
        

     if(nRetVal>0)
         nRetVal=RunSql("insert into dl$hwjcb(cjym,cjymc,clsh,djhdj,nsl,chwbz,\
                        cfhczydh,cjhczydh,cfhjh,cjhjh,njhrq,njhsfm,nfhrq,nfhsfm,\
                        nczlb,njlzt,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,clxbz) \
                        values(%s,%s,%s,0,%ld,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,\
                        6,1,0,0,0,0,'0','0',%s)",
                        sJym,sJymc,sLsh,nCount,sHwbz,sCzydh,"NONE",sJsdh,
                        ".",nSysLkrq,nSysSfm,nSysLkrq,nSysSfm,cLxbz);
     
     if(nRetVal>0) nRetVal=ProcessRecord(sLsh);
     
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sJymc);
     	RollbackWork();
     	sprintf(sbuf,"000011|%s|",sJymc);
     	return ApplyToClient("%s",sbuf);
     }
     
     CommitWork();
     sprintf(sbuf,"000000|%s|",sLsh);
     return ApplyToClient("%s",sbuf);
}

/*=========================================================
函数：dsTaskReturnGoods   TASK_RETURN_GOODS
功能：向委托方退货
参数：sLsh:流水号
      sCzydh:操作员代号
      sHwbz:货物标志
      nCount:数量
返回：0：成功，否则失败
      [返回RECORD]
      成功：000000|流水号|
      失败：XXXXXX|失败信息文字说明|
*========================================================*/
long dsTaskReturnGoods(char * sLsh,char * sCzydh,char * sHwbz,long nCount)
{
     char sJym[16],sbuf[81],sJymc[51]=".";
     char sJsdh[10],cLxbz[21]="00000000000000000000";
     long identify=0,nRetVal;
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     RunSelect("select to_number(substr(clxbz,5,1)),cjymc from dl$jym \
                where cjym=%s into %ld%s",sJym,&identify,sJymc);
     nRetVal=1;
     if(identify)
     {
     	   nRetVal=RunSql("update dl$hwjcb set njlzt=3 where cjym=%s and cjhczydh=%s \
                     and chwbz=%s and ((nczlb=1 and njlzt=1) or (nczlb=3 and njlzt=2))",
                     sJym,sCzydh,sHwbz);
           cLxbz[0]='1';
     }
        

     if(nRetVal>0)
         nRetVal=RunSql("insert into dl$hwjcb(cjym,cjymc,clsh,djhdj,nsl,chwbz,\
                        cfhczydh,cjhczydh,cfhjh,cjhjh,njhrq,njhsfm,nfhrq,nfhsfm,\
                        nczlb,njlzt,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,clxbz) \
                        values(%s,%s,%s,0,%ld,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,\
                        2,1,0,0,0,0,'0','0',%s)",
                        sJym,sJymc,sLsh,nCount,sHwbz,sCzydh,"NONE",sJsdh,
                        ".",nSysLkrq,nSysSfm,nSysLkrq,nSysSfm,cLxbz);
     
     if(nRetVal>0) nRetVal=ProcessRecord(sLsh);
     
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sJymc);
     	RollbackWork();
     	sprintf(sbuf,"000011|%s|",sJymc);
     	return ApplyToClient("%s",sbuf);
     }
     
     CommitWork();
     sprintf(sbuf,"000000|%s|",sLsh);
     return ApplyToClient("%s",sbuf);
}


