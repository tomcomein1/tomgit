#include"dlywd.h"
#include"t_mobile.h"

//extern long nSysSfm;
extern long nSysLkrq;
/*=========================================================
函数：dsTaskRecallConfirm   TASK_RECALL_CONFIRM
功能：撤单申请确认
参数：inbuf:撤单请求参数报文，具体内容如下
      交易流水号|被撤单流水号|操作员代号|时分秒|
返回：0成功
      [返回RECORD]
      成功：000000|撤单记录流水号|被撤单记录流水号|
      失败：XXXXXX|错误信息文字描述|
*========================================================*/

long dsTaskRecallConfirm(char * inbuf)
{

     char outbuf[256];//返回给客户端
     char sJym[16];
     char sJsdh[10];
     char sLsh[41],sRecallLsh[41];//撤单流水号和被撤单流水号
     char sHead[8],sCzsj[21],sSjh[13];
     long nCxms,nxh;
     
     GetSysLkrq();     
     FetchStringToArg(inbuf,"%s|%s|",sLsh,sRecallLsh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     
     if(strncmp(sLsh+9,sRecallLsh+9,15))
     {
     	ApplyToClient("%s","000013|撤消流水号与被撤消流水号中的交易码不一致|");
        return 0;
     }

     if(!RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms))
     {
     	ApplyToClient("%s","000002|数据源无此交易|");
        return 0;
     }
     
     if(RunSelect("select * from dl$zwmx where clsh=%s",sLsh)>0)
     {
     	ApplyToClient("%s","000003|流水号重复|");
        return 0;
     }	     

     if(!RunSelect("select * from dl$zwmx where cjym=%s and clsh=%s and ncsbz<2",sJym,sRecallLsh))
     {
     	ApplyToClient("%s","000010|数据源无此流水号的缴费记录或该笔缴费已对帐|");
        return 0;
     }	     
     GenerateLsh(3,sJsdh,sJym,outbuf);
     nxh=atol(outbuf);
     
     strcpy(outbuf,"000000");
     switch(nCxms)
     {
     	case 130: //移动
     	     dsMobileRecallConfirm(inbuf,nxh,outbuf);
     	     break;

        case 140://银海电信
             dsTelecomRecallConfirm(inbuf,nxh,outbuf);
             break;
                
        case 110://一般集中方式的缴费[注意：请不要映射预存款]
        case 610:
        case 520:
        case 521:
             dsGeneralRecallConfirm(inbuf,nxh,outbuf);
             break;
    
        case 210:
        case 220:
        case 230:
             dsTaskBpUserRecallConfirm(inbuf,nxh,outbuf);
             break;
        
        case 1001:
             dsAirRecall(inbuf,nxh,outbuf);
             break;     

		case 821:
	     	dsTaskBxRecallConfirm(inbuf,nxh,outbuf);
 	     	FetchStringToArg(outbuf,"%s|%s|%s|",sHead,sCzsj,sSjh);
	     	break;

		case 123:
	    	//dsTaskPowerRecallConfirm(inbuf,nxh,outbuf);
	     	break;
		
		case 522:
	     	dsTaxRecallConfirm(inbuf,nxh,outbuf);
	    	break;        
     } 
     
     //最后插入撤单帐务明细记录并修改业务明细消帐标志
     if(!strncmp(outbuf,"000000",6))
     {
        dsModifyZwmxOnRecall(inbuf,outbuf,nxh);
     	CommitWork();
     	sprintf(outbuf,"000000|%s|%s|",sLsh,sRecallLsh);
     }
     else 
     	RollbackWork();
     
     if(nCxms==821 && !strcmp(sHead,"000000") )
     {
        if(RunSql("update dl$zwmx set cbl1=%s,cbl3=%s where clsh=%s",
		sSjh,sCzsj,sLsh)>0)
	  CommitWork();
     }
     return ApplyToClient("%s",outbuf);

}

/*=========================================================
函数：dsModifyZwmxOnRecall
功能：撤单成功后插入撤单帐务明细记录并修改业务明细消帐标志
参数：inbuf:撤单请求参数报文，具体内容如下
      交易流水号|被撤单流水号|实缴款额|操作员代号|时分秒|
      outbuf:返回给客户端的成功或失败信息
返回：0成功，否则失败
*========================================================*/
long dsModifyZwmxOnRecall(char * inbuf,char * outbuf,long nxh)
{
     char sLsh[41],sRecallLsh[41];  //流水号
     char sBuffer[256];
     char sYhbz[101]; //用户标志
     char sJym[16];  //交易码
     char sYhmc[51]; //用户名称
     char sCzydh[7]; //操作员代号
     char sJsdh[10]; //缴费局局号
     long   nFkfs=1;   //付缴方式1：现金  2：绿卡
     long  nRecallXh;
     double dSjk;  //实缴款，应缴款
     long nSfm=0,nSl=0;
	
    
     FetchStringToArg(inbuf,"%s|%s|%s|%d|",sLsh,sRecallLsh,sCzydh,&nSfm);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);

     //GenerateLsh(3,sJsdh,sJym,sBuffer);
     //nxh=atol(sBuffer);

     if(RunSelect("select nxh,cyhbz,cyhmc,nsfkfs,dsjk,nsl from dl$zwmx where cjym=%s and clsh=%s \
                into %ld%s%s%ld%lf%ld",sJym,sRecallLsh,&nRecallXh,sYhbz,sYhmc,&nFkfs,&dSjk,&nSl)<=0)
     {
         GetSqlErrInfo(sBuffer);
         sprintf(outbuf,"000011|查询撤单原始记录出错:%s|",sBuffer);
         return -1;
     }
     
     sprintf(sBuffer,"%s|%s|%s|%ld|%.2lf|%s|%ld|%ld|",sLsh,sYhbz,sRecallLsh,nFkfs,
             dSjk,sCzydh,nSfm,nSl);
             
     //插入帐务明细撤单记录
     if(InsertZwmxTable(sBuffer,TASK_RECALL_CONFIRM,nxh)<0)
     {
         GetSqlErrInfo(sBuffer);
         sprintf(outbuf,"000011|插入帐务明细撤单记录出错:%s|",sBuffer);
         return -1;
     }    
     
     //修改原记录标志位，将CLXBZ第19位置2，将本记录第nbl置成被撤单流水号
     
     RunSql("update dl$zwmx set nbl=%ld where clsh=%s",nRecallXh,sLsh);
     RunSql("update dl$zwmx set clxbz='00000000000000000020' where clsh=%s",sRecallLsh);
     sprintf(outbuf,"000000|撤单成功|");
     return 0;

}


/*=========================================================
函数：dsGeneralRecallConfirm
功能：一般集中式缴费的撤单过程
参数：inbuf:撤单请求参数报文，具体内容如下
      交易流水号|被撤单流水号|操作员代号|时分秒|
      nxh:与委托方撤单记录序号
      outbuf:返回给客户端的成功或失败信息
返回：0成功，否则失败
*========================================================*/
long dsGeneralRecallConfirm(char * inbuf,long nxh,char * outbuf)
{
     
     char sLsh[41];
     char sRecallLsh[41];
     char sJym[16];
     char sJsdh[10];
     char sSqlErr[81];
     long nRetVal;
    
     
     FetchStringToArg(inbuf,"%s|%s|",sLsh,sRecallLsh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     
     nRetVal=RunSql("update dl$ywmx_%t set nxzbz=0,clsh='0',dsjk=0.0 where clsh=%s",
                    sJym,sRecallLsh);
     
     /*nRetVal=RunSql("update dl$ywmx_%t set nxzbz=0,dsjk=0 where clsh=%s",
                    sJym,sRecallLsh);*/
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sSqlErr);
     	sprintf(outbuf,"000011|数据库操作失败：%s|",sSqlErr);
     	return -1;
     }
     strcpy(outbuf,"000000|撤单成功|");
     return 0;	

}
