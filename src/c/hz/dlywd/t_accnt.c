#include"dlywd.h"
#include"t_accnt.h"

extern long nSysLkrq;
extern long nSysSfm;

/*============================================================
函数：dsTaskPayValidate    TASK_PAY_VALIDATE
功能：与支局（县局）单条收费记录验证
参数：inbuf:输入参数字符串
      流水号  char(40)
      流水号：[9位局号+15位交易码+8位日期+8位序号]
返回：0成功
      [返回RECORD]
      成功：000000|流水号|销帐标志|
      失败：XXXXXX|错误信息文字解释|
*============================================================*/

long dsTaskPayValidate(char * inbuf)
{
     
     long nXzbz;
     char sSqlErr[81];
     char outbuf[101];
     
     if(RunSelect("select nxzbz from dl$zwmx where clsh=%s into %ld",inbuf,&nXzbz)>0)
     {
     	if(nXzbz<2)
     	{
     		RunSql("update dl$zwmx set nxzbz=2 where clsh=%s",inbuf);
     	        CommitWork();
     	}
     	sprintf(outbuf,"000000|%s|%ld|",inbuf,nXzbz);
     }
     else
     {
     	GetSqlErrInfo(sSqlErr);
     	sprintf(outbuf,"000014|%s|",sSqlErr);
     }
     return ApplyToClient("%s",outbuf);

}

/*============================================================
函数：dsTaskCheckAccount    TASK_CHECK_ACCOUNT
功能：数据源与委托方的对帐
参数：inbuf:输入参数字符串
      交易码  char(15)
返回：0成功
      [返回RECORD]
      成功：000000|交费总笔数|交缴费金额|
      失败：XXXXXX|错误信息文字解释|
*============================================================*/
long dsTaskCheckAccount(char * inbuf)
{
     
     char outbuf[256];
     long nCxms;

     GetSysLkrq();
     
     if(!RunSelect("select ncxms from dl$jym where cjym=%s into %ld",inbuf,&nCxms))
     {
     	ApplyToClient("%s","000002|数据源无此交易|");
        return -1;
     }
     memset(outbuf,0,sizeof(outbuf));
     switch(nCxms)
     {
     	case 110://一般集中式的对帐
     	case 111:
     	case 610:
     	     dsGeneralCheckAccount(inbuf,outbuf);
     	     break;
     	     
     	case 130:
     	     dsMobileCheckAccount(inbuf,outbuf);
     	     break;

     	case 140:
     	     dsTelecomCheckAccount(inbuf,outbuf);
     	     break;

        case 210:
        case 220:
        case 230:
             dsTaskBpCheckAccount(inbuf,outbuf);
             break;
     }
     CommitWork();
     return ApplyToClient("%s",outbuf);
}


long dsGeneralCheckAccount(char * inbuf,char * outbuf)
{
     strcpy(outbuf,"000000|0|0|");
     return 0;
}



