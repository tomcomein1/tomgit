#include"dlywd.h"

extern long nSysDebug;
extern long nSysLkrq;
extern long nSysSfm;
/*============================================================
函数：缴费查询服务端程序：TASK_PAY_QUERY
功能：根据用户要求和指定的搜索条件并根据程序模式（ncxms）将查询
      请求重定向，并将查询结果组织成统一的格式返回给客户端
参数：inbuf:输入参数字符串
      流水号|检索条件位图|检索条件内容|操作员代号|
    char(40)  char(5)      char(<=100)
      流水号：[9位局号+15位交易码+8位日期+8位序号]
返回：>0成功
      [返回RECORD]
      成功：000000|用户名称|合同号|预存款额|明细记录数|{用户标志|年度|月份|25项收费明细...
           |本条应收|4项长整型数据...|4项字符串型数据|}....
      失败：XXXXXX|错误信息文字解释|
*============================================================*/
long dsTaskPayQuery(char * inbuf)
{
	
     RECORD Client;    //返回客户端数据记录
     
     long nCxms,nRetVal;      //程序模式
     char sJym[16];//交易码
     char sLsh[41];
     char sMap[6];
     char sJsdh[10];
     	
     GetSysLkrq();
     FetchStringToArg(inbuf,"%s|%s|",sLsh,sMap);
     

     if(!strchr(sMap,'1'))
     {
     	ApplyToClient("%s","000001|没有指明检索条件|");
        return 0;
     }

     //组合交易码
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);

          
     if(!RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms))
     {
     	ApplyToClient("%s","000002|数据源无此交易|");
        return 0;
     }


     Client=CreateRecord();
     if(Client.nsize<1) 
     {
     	ApplyToClient("%s","000004|服务端存储分配出错|");
     	return 0;
     }	
             	
     switch(nCxms)
     {
        
        //集中方式
        case 110://联通代收
        case 610://天然气代收
          dsGeneralPayQuery(inbuf,&Client);
          break;
        case 111://联通预存款
          break;
        
        case 130://中国移动
          dsMobilePayQuery(inbuf,&Client);
          break;

        case 140: //银海电信
          dsTaskTelecomPayQuery(inbuf,&Client);
          break;

        case 210://寻呼
        case 220:
        case 230:
          dsTaskBpUserQuery(inbuf,&Client);
          break;
        case 821:
          dsTaskBxUserQuery(inbuf,&Client);
          break;
        
        case 521://国税代收
          dsTaskTaxQuery(inbuf,&Client);
          break;
        case 522:
          dsTaskTax1Query(inbuf,&Client);
          break;
        case 123://电费代收
          dsTaskPowerQuery(inbuf,&Client);
          break;
        case 141:
        //  dsTaskYaTelePayQuery(inbuf,&Client);
          break;
     }
     
     nRetVal=ApplyToClient("%r",Client);
     if(nSysDebug) printf("\nRECORD:\n%s\n",Client.sdata);
     DropRecord(&Client);
     return nRetVal;

}

/*==================================================================
函数：dsTaskGeneralPayQuery
功能：对于集中式收费的统一查询
*=================================================================*/

long dsGeneralPayQuery(char * inbuf,RECORD * Client)
{
     char sBzmap[6],sJym[16],sLsh[41],sYhbz[101],sJsdh[10];
     char sYhmc[51],sYhbz1[41];//用户名称与合同号
     char sWhere[1000];
     char sBz[31];
     char sCzydh[7];
     long nmap,nId,ncount,nysnd,nysyf,nRetVal;
     double dsfmx[26],dZnj;
     long nValue[4],nDates,nCxms;
     char * sql,*smap,* pointer;     

     FetchStringToArg(inbuf,"%s|%s|%s|%s|",sLsh,sBzmap,sYhbz,sCzydh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
    
     RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);
     smap=sBzmap;
     sql=sWhere;
     memset(sWhere,'\0',sizeof(sWhere));
     memset(nValue,0,sizeof(nValue));     

     strcpy(sql," where nxzbz=0 and nywlb=0 ");
     while(sql[0]) sql++;
     nmap=0;
     pointer=sYhbz;          
     //形成查询条件表达式
     while(smap[0])
     {
       	nmap++;

       	if(smap[0]=='0')
        {
             smap++; 
             pointer+=20;
             continue;
        }

        memset(sBz,'\0',sizeof(sBz));
        strncpy(sBz,pointer,20);
        alltrim(sBz);
      	switch(nmap)
       	{
       	     case 1:
       	        sprintf(sql," and cyhbz='%s'",sBz);
       	        break;
       	     case 2:
       	        sprintf(sql," and cyhbz1='%s'",sBz);
       	        break;
       	     case 3:
       	        sprintf(sql," and cyhbz2='%s'",sBz);
       	        break;
       	}
        while(sql[0]) sql++;
        pointer+=20;
        smap++;
     }	

     ncount=0;
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

     
     nId=OpenCursor("select cyhbz,nysnd,nysyf,dsfmx1,dsfmx2,dsfmx3,dsfmx4,\
                      dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,dsfmx11,\
                      dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,\
                      dsfmx18,dsfmx19,dsfmx20,dsfmx21,dsfmx22,dsfmx23,dsfmx24,\
                      dsfmx25,dyjzk,\
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
     	nRetVal=FetchCursor(nId,"%s%ld%ld%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf\
     	                 %lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%ld",sYhbz,
    	                 &nysnd,&nysyf,&dsfmx[0],&dsfmx[1],&dsfmx[2],&dsfmx[3],
                         &dsfmx[4],&dsfmx[5],&dsfmx[6],
                         &dsfmx[7],&dsfmx[8],&dsfmx[9],
     	                 &dsfmx[10],&dsfmx[11],&dsfmx[12],&dsfmx[13],&dsfmx[14],
     	                 &dsfmx[15],&dsfmx[16],&dsfmx[17],&dsfmx[18],&dsfmx[19],
     	                 &dsfmx[20],&dsfmx[21],&dsfmx[22],&dsfmx[23],&dsfmx[24],
     	                 &dsfmx[25],&nDates);

        dZnj=0.0;
        switch(nCxms)   //对各种类型交易的不同处理
        {
            
            case 610:   //对滞纳金的处理
            {
              double dMyZnjBl,dSyZnjBl;
              if(nDates>0)
              {                
                  RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=10 and cjym=%s \
                            into %lf",sJym,&dMyZnjBl);  //民用滞纳金比例
                  RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=11 and cjym=%s \
                            into %lf",sJym,&dSyZnjBl);  //商用滞纳金比例
                  dZnj=(dsfmx[5]*dMyZnjBl+dsfmx[2]*dSyZnjBl)*nDates;
                  if(dZnj>0.0) dsfmx[25]+=dZnj;
                  else dZnj=0.0;
                  dsfmx[6]=dZnj;
              }    
            }  
            break;      
        }//end switch
             	
     	ImportRecord(Client,"%s%ld(2)%lf[26]%ld[4]%s(4)",sYhbz,nysnd,nysyf,
     			    dsfmx,nValue,".", "." , "." , ".");

        nRetVal=RunSql("update dl$ywmx_%t set clsh=%s,cczydh=%s,dxzyjk=%lf %t \
                        and nysnd=%ld and nysyf=%ld",
                        sJym,sLsh,sCzydh,dZnj,sWhere,nysnd,nysyf);
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

/*
函数：缴费查询服务端程序：TASK_PUBLIC_QUERY
功能：根据用户要求和指定的搜索条件并将查询
      请求重定向，并将查询结果组织成统一的格式返回给客户端
参数：inbuf:输入参数字符串
      交易码|程序分支|条件(用于打开游标的SQL条件包)|
    
返回：>0成功
      [返回RECORD]
      成功：000000{五个双精度数据|五个整型数据|十个字串数据}....
      失败：XXXXXX|错误信息文字解释|

*/
long dsTaskPublicQuery(char * inbuf)
{
	 RECORD Client;    //返回客户端数据记录
     
     long nRetVal,nCxms;
     char sJym[16];//交易码
     char sJsdh[10];
     long nType;   //程序走向
     
     FetchStringToArg(inbuf,"%s|%d|",sJym,&nType);
     
     if(!RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms))
     {
     	ApplyToClient("%s","000002|数据源无此交易|");
        return 0;
     }

     Client=CreateRecord();
     if(Client.nsize<1) 
     {
     	ApplyToClient("%s","000004|服务端存储分配出错|");
     	return 0;
     }	
             	
     /*switch(nType)
     {
     	case 1002:   //一般[nCxms+n]用于调用相应交易的查询
     		dsTaskQuery_AirTicket(inbuf,&Client);
     		break;
		
		case 1004:
			dsTaskQuery_AirSd(inbuf,&Client);
			break;
			     	
		case 1006:
			dsTaskQuery_AirTp(inbuf,&Client);
			break;

        default:
			printf("\n未定义程序分支...\n");
        	break;
     }
	*/
     nRetVal=ApplyToClient("%r",Client);
     if(nSysDebug) printf("\nRECORD:\n%s\n",Client.sdata);
     DropRecord(&Client);
     return nRetVal;
}
