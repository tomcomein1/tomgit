#include"dlywd.h"

extern long nSysLkrq;
//extern long nSysSfm;

/*============================================================*
函数：缴费查询服务端程序：TASK_PAY_CONFIRM
功能：根据用户的缴费取文将缴费确具重向,并将缴费成功或失败结果返回给客户端
参数：inbuf:输入参数字符串，它的格式如下（第个字段用‘|’隔开）
      
流水号|用户标志|用户名称|缴款方式|实缴总款额|操作员代号|时分秒|缴款月份数{|年度|月份|年度|月份|......}
char(40) char     char    LONG(2) FLOAT(10.2) char(4)   LONG(8)  LONG(2)

注：若缴款月份数为0，则表示所有月份都缴，若不为零则只缴其后面年度和月份的费用
返回：>0成功 
     [返回RECORD]
           成功：000000|流水号|实缴款额|
           失败：XXXXXX|错误信息|         

*============================================================*/
long dsTaskPayConfirm(char * inbuf)
{
     char sLsh[41];  //流水号
     char outbuf[2048];
     char sSqlErr[81];
     char sJsdh[10];     
     char sJym[16];
     char sYhbz[101]; //用户标志
     char sYhmc[51]; //用户名称
     char sCzydh[7]; //操作员代号
     long nMode;     //缴款方式 1：现金  2：绿卡
     long nCount,limited;
     long nCxms;
     double dSjk;  //实缴款，应缴款
     long nSfm=0;


     GetSysLkrq();
     
     FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|%d|",
                       sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSfm,&nCount);
     //组合交易码
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);

     if(!RunSelect("select ncxms from dl$jym where cjym=%s into %ld",
                                                                sJym,&nCxms))
     {
     	ApplyToClient("%s","000002|数据源无此交易|");
        return 0;
     }
     
     if(RunSelect("select ROWNUM from dl$zwmx where clsh=%s",sLsh)>0)
     {
     	ApplyToClient("%s","000003|流水号重复|");
        return 0;
     }	     
     
    /////////////////// 若是集中方式的预存款  //////////////////////////////////
     if(nCount>=0 && 
        RunSelect("select ROWNUM from dl$ywmx_%t where clsh=%s",sJym,sLsh)<=0)
     {
        switch(nCxms)
        {
            //代收
            case 130:
            case 110:
            case 140:
            case 141:
            case 610:
            case 210:
            case 220:
            case 230:
            case 821:
            case 521:
            case 522:
            case 123:
            	 ApplyToClient("%s","000010|数据源无此流水号|");
                 return 0;
                        
            case 111:
                 
                 limited=0;
                 RunSelect("select to_number(ccs) from dl$jymcsfb "
                 "where nbh=1 and cjym=%s into %ld",sJym,&limited);
                 
                 //限制号码
                 if(limited==1)
                   if(RunSelect("select * from dl$jymcsfb where cjym=%s "
                      "and nbh between 2 and 32 and %s like ccs",sJym,sYhbz)<=0)
                     	return ApplyToClient("%s","000017|您所输入的号码不在预定范围内...|");

                 if(InserBlankYwmx(inbuf)<0)
                 {
               	       GetSqlErrInfo(sSqlErr);
               	       RollbackWork();
               	       sprintf(outbuf,"000011|数据源数据库操作出错:%s|",sSqlErr);
               	       ApplyToClient("%s",outbuf);
                       return 0;
                 }	
                 break;

        }    
      }
      
      /////////////////////        进入缴款阶段      /////////////////////////
      if(nCount<0) nCxms=0; //若缴款月份数小于0则为调帐记录
      memset(outbuf,0,sizeof(outbuf));
      switch(nCxms)
      {
      	 //移动
      	 case 130:
      	      dsTaskMobilePayConfirm((char *)inbuf,outbuf);
      	      break;
      	 
      	 //集中方式     
      	 case 110:
      	 case 111:
      	 case 610:
      	 case 521:
      	 case 0:
      	      dsTaskGeneralPayConfirm(inbuf,outbuf);
      	      break;

         case 140:
              dsTaskTelcomPayConfirm(inbuf,outbuf);
              break;        
         case 141:
              //dsTaskYaTelPayConfirm(inbuf,outbuf);
              break;        
         //寻呼
         case 210:
         case 220:
         case 230:
              dsTaskBpUserPayConfirm(inbuf,outbuf);
              break;
         //保险
       	 case 821:
       	      dsTaskBxUserPayConfirm(inbuf,outbuf);
              break;
         case 123:
              dsTaskPowerPayConfirm(inbuf,outbuf);
              break;
         case 522:
              dsTaskTaxPayConfirm(inbuf,outbuf);
              break;
         
         default:
              sprintf(outbuf,"000016|无法鉴别的交易模式|");
              break;
 
      }

      if(strncmp(outbuf,"111111",6)==0)
      {
      	  RunSql("update dl$zwmx set nxzbz=0 where clsh=%s",sLsh);
          CommitWork();
      }    
      return ApplyToClient("%s",outbuf);
}

/*============================================================
函数：dsTaskGeneralPayConfirm
功能：一般集中式缴费请求
参数：inbuf:缴款输入参数序列
      outbuf：缴费结果返回字符串
返回：0成功，否则失败
*============================================================*/

long dsTaskGeneralPayConfirm(char *inbuf,char *outbuf)
{
      
     char sLsh[41];  //流水号
     char sYhbz[101]; //用户标志
     char sJym[16];  //交易码
     char sYhmc[51]; //用户名称
     char sCzydh[7]; //操作员代号
     char sJsdh[10]; //缴费局局号
     char sBuffer[51];
     char * sOffset;
     long nMode;     //缴款方式 1：现金  2：绿卡
     long nCount,nYsnd,nYsyf,nxh,nId;
     double dSjk,dYjzk,dSumSjk;  //实缴款，应缴款
     long nSfm=0;
     long nCxms=0;
     char sDump[20]=".",sFph[13]=".";

     sOffset=FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|%d|",
                       sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSfm,&nCount);
     
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);//程序模式
	 
	 if(nCxms==521) nCount=0;
     
     if(sJym[0]=='0'&&sJym[1]=='1'&&nCount>=0)
     {
        if(RunSelect("select ROWNUM from dl$ywmx_%t where nxzbz=0 and clsh=%s",sJym,sLsh)<=0)
        {
     	   strcpy(outbuf,"000002|缴费流水号不存在或该用户已缴费|");
     	   return -1;
        }
     
     ////////////  若为选择性缴费则只修改相应业务明细记录    ////////
        if(nCount>0)  //选择性缴费
        {
           while(nCount--)
           {

              sOffset=FetchStringToArg(sOffset,"%d|%d|",&nYsnd,&nYsyf);
              if(RunSql("update dl$ywmx_%t set nxzbz=1,dsjk=%lf,nlkrq=%ld,\
                         nsfm=%ld,cczydh=%s where clsh=%s and nysnd=%ld and \
                         nysyf=%ld",sJym,dSjk,nSysLkrq,
                         nSfm,sCzydh,sLsh,nYsnd,nYsyf)<0)
              {
              	
              	    GetSqlErrInfo(sYhbz);
              	    sprintf(outbuf,"000011|修改业务表出错：%s|",sYhbz);
              	    return -2;
              }
           }
           if(RunSql("update dl$ywmx_%t set nxzbz=0,clsh='0' \
                      where clsh=%s and nxzbz=0",sJym,sLsh)<0)
           {
              	    GetSqlErrInfo(sYhbz);
              	    sprintf(outbuf,"000011|修改业务表出错：%s|",sYhbz);
              	    return -2;
           }
        }
        else        //所有月份全部缴费
        {
           if(RunSql("update dl$ywmx_%t set nxzbz=1,dsjk=%lf,nlkrq=%ld,\
                      nsfm=%ld,cczydh=%s where clsh=%s",
                      sJym,dSjk,nSysLkrq,nSfm,sCzydh,sLsh)<0)
           {
             	
            GetSqlErrInfo(sYhbz);
            sprintf(outbuf,"000011|修改业务表出错：%s|",sYhbz);
            return -2;
           }
        }       	
      
        /////////////  验证实缴款是否正确   ////////////////     
     
        dYjzk=-1.0;
        RunSelect("select sum(dyjzk),count(*) from dl$ywmx_%t where clsh=%s \
               and nxzbz=1 into %lf%ld",sJym,sLsh,&dYjzk,&nCount);
                
        if(dYjzk<0.0)
        {
           GetSqlErrInfo(sYhbz);
           RollbackWork();
           sprintf(outbuf,"000011|金额验证出错：%s|",sYhbz);
           return -2;
        }
     
        if(dSjk<dYjzk)
        {
           RollbackWork();
           sprintf(outbuf,"000012|缴费金额不足|");
           return -2;
        }

     ///////////////  如果不只一个月记录则逐条修改业务明细实缴款记录   //////////
        if(nCount>1)
        {
           dSumSjk=dSjk;
           nId=OpenCursor("select distinct nysnd,nysyf,dyjzk from dl$ywmx_%t where clsh=%s \
                        and  nxzbz=1 order by nysnd,nysyf",sJym,sLsh);
           if(nId>0)
           {
             nxh=1;
             for(;;)
             {
                FetchCursor(nId,"%ld%ld%lf",&nYsnd,&nYsyf,&dYjzk);
             	if(nxh++>=nCount)  //最后一笔
             	{
             		RunSql("update dl$ywmx_%t set dsjk=%lf where clsh=%s and nysnd=%ld \
             		        and nysyf=%ld",sJym,dSumSjk,sLsh,nYsnd,nYsyf);
                        break;
                }
                else
                {
             		RunSql("update dl$ywmx_%t set dsjk=dyjzk where clsh=%s and nysnd=%ld \
             		        and nysyf=%ld",sJym,sLsh,nYsnd,nYsyf);
             		dSumSjk-=dYjzk;
             	}
             }//end for
           }
        }
     }//end if
     //////////////////  插入帐务明细表  //////////////////////
     GenerateLsh(3,sJsdh,sJym,sBuffer);
     nxh=atol(sBuffer);

     if(InsertZwmxTable(inbuf,TASK_PAY_CONFIRM,nxh)<0)
     {
        GetSqlErrInfo(sYhbz);
        RollbackWork();
        sprintf(outbuf,"000011|插入帐务明细表出错：%s|",sYhbz);
        return -3;
     }
     
     //据业务要求休改相应的字段值。
     
     alltrim(sFph);
     switch(nCxms)
     {
     	case 521:
     	  nId=OpenCursor("select cyhbz1 from dl$ywmx_%t where clsh=%s ",sJym,sLsh);
    	  if(nId<=0) { CloseCursor(nId); break;}
    	  
    	  for(;;)
		  {
			sDump[0]='\0';
			if(FetchCursor(nId,"%s",sDump)<=0)break;
    	
			sFph[0]='\0';
    		sOffset=FetchStringToArg(sOffset,"%s|",sFph);
     	  
     	    RunSql("update dl$ywmx_%t set cczydh=%s,dsjk=dsfmx5,cbl8=%s,cjh=%s where clsh=%s and cyhbz1=%s",
        	       sJym,sCzydh,sFph,sJsdh,sLsh,sDump);
          }
          CloseCursor(nId);
           
        break;
        
        default:
        RunSql("update dl$ywmx_%t set cczydh=%s,cjh=%s where clsh=%s",sJym,sCzydh,sJsdh,sLsh);
        break;
     }
     CommitWork();   //提交任务
     sprintf(outbuf,"000000|%s|%.2lf|",sLsh,dSjk);
     return 0;

}
/*===============================================================
函数：InsertZwmxTable
功能：统一插入帐务明细表
参数：inbuf:缴款参数确认模块[同dsTaskPayConfirm函数的定义]；

流水号|用户标志|用户名称|缴款方式|实缴总款额|操作员代号|时分秒|缴款月份数|年度|月份|年度|月份|
char(40) char     char    LONG(2) FLOAT(10.2) char(4)   LONG(8)  LONG(2)

      nmode:以哪一种方式，如：
            TASK_PAY_CONFIRM:一般集中式收费插入
            TASK_MOBILE_PAY_CONFIRM:移动插入帐务明细
            TASK_RECALL_CONFIRM:插入撤单记录
      nxh:与委托方的帐务流水号[由GenerateLsh产生]            
返回：>0成功，否则失败            
*===============================================================*/
//插入帐务明细记录
long InsertZwmxTable(char * inbuf,long nmode,long nxh)
{
     char sLsh[41],sRecallLsh[41];  //流水号
     char sYhbz[101]; //用户标志
     char sJym[16];  //交易码
     char sYhmc[51]; //用户名称
     char sCzydh[7]; //操作员代号
     char sJsdh[10]; //缴费局局号
     char sLxbz[21];
     double dSjkxj=0.0,dSjkcz=0.0,dSjkxyk=0.0; //实缴款现金、存折、信用卡
     long   nFkfs=1,nCxms;   //付缴方式1：现金  2：绿卡
     long nCount,nCsbz=0,nXzbz=2,nYsnd,nYsyf;
     double dSjk;  //实缴款，应缴款
     long nSfm=0;

     FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|%d|",
                       sLsh,sYhbz,sYhmc,&nFkfs,&dSjk,sCzydh,&nSfm,&nCount);
     strcpy(sRecallLsh,sLsh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     
     if(sJym[1]!='2')  nCount=1;
     RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);
     switch(nCxms)
     {
         case 130:
         case 140:
         case 141:
         case 210:
         case 220:
         case 230:
         case 821:
         case 1001:
         case 123:
         case 522:
              nCsbz=1;
              nXzbz=2;
              break;
         default:
              nCsbz=0;
              nXzbz=2;
	      break;
     }
     switch(nmode)
     {
     	
     	case TASK_PAY_CONFIRM:
     	case TASK_POWER_PAY_CONFIRM:
     	case TASK_MOBILE_PAY_CONFIRM:
             strcpy(sLxbz,"00000000000000000000");
             break;
        case TASK_RECALL_CONFIRM:
             dSjk=-dSjk;
             nCount=-nCount;
             strcpy(sLxbz,"00000000000000000010"); 
             strcpy(sRecallLsh,sYhmc);
             strcpy(sYhmc,".");
             break;
        default:
             strcpy(sLxbz,"00000000000000000000");
             break;

     }
     
     switch(nFkfs)  //根据付款方式判断是现金缴款还是信用卡
     {
     	case 1:
     	     dSjkxj=dSjk;
     	     break;
        case 2:
             dSjkcz=dSjk;
             break;
     }
     
        
     nYsnd=nSysLkrq/10000;
     nYsyf=(nSysLkrq%10000)/100;
     return RunSql("insert into dl$zwmx(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,nywlb,nxh,nsl,"
     				"dyjzk,dxzyjk,dsjk,ddlsxf,dybk,dytk,dsjkxj,dsjkcz,dsjkxyk,"
     				"nsfkfs,cjkdh,cjh,cczydh,nlkrq,nsfm,clxbz,"
     				"nxzbz,ncsbz,ncscs,njsbz,cwbjsdh,"
                    "cnbjsdh,cbl,nbl,cbl1,cbl2,cbl3,cbl4,cbl5,clsh,cylsh) "
                    "VALUES(%s,%ld,%ld,%s,%s,'0',%s,0,%ld,%ld,"
                    "%lf,0,%lf,%lf,0,0,%lf,%lf,%lf,"
                    "%ld,'000000000000000',%s,%s,%ld,%ld,%s,"
                    "%ld,%ld,0,0,'00000000000000000000',"
                    "'00000000000000000000','0',0,'0','0','0','0','0',%s,%s)",
                    sJym,nYsnd,nYsyf,sYhbz,sYhbz,sYhmc,nxh,nCount,
                    dSjk,dSjk,0.0,dSjkxj,dSjkcz,dSjkxyk,
                    nFkfs,sJsdh,sCzydh,nSysLkrq,nSfm,sLxbz,
                    nXzbz,nCsbz,sLsh,sRecallLsh);

}

/*===============================================================
函数：InserBlankYwmx
功能：在集中方式的预存款缴费时插入一条业务明细内容
参数：inbuf:缴款参数序列字符串，其定义请参见函数dsTaskPayConfirm
*===============================================================*/

//插入空白业务明细记录
static long InserBlankYwmx(char * inbuf)
{
     char sBuffer[51];
     char sLsh[41];
     char sYhbz[41]; //用户标志
     char sJym[16];  //交易码
     char sYhmc[51]; //用户名称
     char sCzydh[7];
     char sJsdh[10];
     long nMode;     //缴款方式 1：现金  2：绿卡
     double dSjk;
     long nxh,nYsnd,nYsyf;
     long nSfm=0;

     FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|",sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSfm);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     
     //按交易码获得帐务明细序号
     GenerateLsh(3,sJsdh,sJym,sBuffer);
     nxh=atol(sBuffer);

     nYsnd=nSysLkrq/10000;
     nYsyf=(nSysLkrq%10000)/100;
        
        
     return  RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,\
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
             VALUES(%s,%ld,%ld,%s,%s,'0',%s,0,%ld,0,0,0,0,0,0,0,0,0,0,\
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0',0,0,0,0,0,0,0,%lf,0,\
                    1,1,0,0,0,0,'0','0',%s,%s,%ld,%ld,'0',\
                    0,0,'0','0','0','0','0','0',%s,'0')",
                    sJym,sJym,nYsnd,nYsyf,sYhbz,sYhbz,sYhmc,
                    nxh,dSjk,sJsdh,sCzydh,nSysLkrq,nSfm,sLsh);

}


