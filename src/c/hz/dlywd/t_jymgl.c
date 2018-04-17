
#include "dlywd.h"
/*===================================================
函数：dsTaskCreateDsJym      [TASK_CREATE_DS_JYM]
参数： 
      sJym:交易码
      sJymc:交易名称
      sSjydz:数据源地址
      nCxms:程序模式
返回：
      1：成功
      -1：失败及失败原因
*===================================================*/
long dsTaskCreateDsJym(char * sJym,char * sJymc,char * sSjydz,long nCxms)
{
     
     long nRetVal,nId;
     char sbuf[201];
     
     nRetVal=RunSelect("select * from dl$jym where cjym=%s",sJym);
     if(nRetVal>0)
     	return ApplyToClient("%ld%s",-1,"交易码建立失败：该交易码已经存在...");
     
     nRetVal=RunSql("insert into dl$jym(cjym,cjymc,ncxms,csjydz,cbl,nbl,dbl,clxbz)"
                    " values(%s,%s,%ld,%s,'.',1,0,'00000000000000000000')",
                    sJym,sJymc,nCxms,sSjydz);
     
     if(nRetVal>0)
     nRetVal=RunSql("CREATE TABLE dl$ywmx_%t( \
               cjym  VARCHAR2(15) NOT NULL,\
               nxh   NUMBER(10) NOT NULL,\
               nysnd NUMBER(4) NOT NULL,\
               nysyf NUMBER(4) NOT NULL,\
               cyhbz    VARCHAR2(20) NOT NULL, \
               cyhbz1   VARCHAR2(20) NOT NULL, \
               cyhbz2   VARCHAR2(20) NOT NULL, \
               cyhmc    VARCHAR2(80) NOT NULL, \
               nywlb    NUMBER(1)    NOT NULL,\
               dsfmx1   NUMBER(11,2) NOT NULL,\
               dsfmx2   NUMBER(11,2) NOT NULL,\
               dsfmx3   NUMBER(11,2) NOT NULL,\
               dsfmx4   NUMBER(11,2) NOT NULL,\
               dsfmx5   NUMBER(11,2) NOT NULL,\
               dsfmx6   NUMBER(11,2) NOT NULL,\
               dsfmx7   NUMBER(11,2) NOT NULL,\
               dsfmx8   NUMBER(11,2) NOT NULL,\
               dsfmx9   NUMBER(11,2) NOT NULL,\
               dsfmx10  NUMBER(11,2) NOT NULL,\
               dsfmx11  NUMBER(11,2) NOT NULL,\
               dsfmx12  NUMBER(11,2) NOT NULL,\
               dsfmx13  NUMBER(11,2) NOT NULL,\
               dsfmx14  NUMBER(11,2) NOT NULL,\
               dsfmx15  NUMBER(11,2) NOT NULL,\
               dsfmx16  NUMBER(11,2) NOT NULL,\
               dsfmx17  NUMBER(11,2) NOT NULL,\
               dsfmx18  NUMBER(11,2) NOT NULL,\
               dsfmx19  NUMBER(11,2) NOT NULL,\
               dsfmx20  NUMBER(11,2) NOT NULL,\
               dsfmx21  NUMBER(11,2) NOT NULL,\
               dsfmx22  NUMBER(11,2) NOT NULL,\
               dsfmx23  NUMBER(11,2) NOT NULL,\
               dsfmx24  NUMBER(11,2) NOT NULL,\
               dsfmx25  NUMBER(11,2) NOT NULL,\
               cjscz1   VARCHAR2(10) NOT NULL,\
               njscz2   NUMBER(8) NOT NULL,\
               djscz3   NUMBER(11,2) NOT NULL,\
               djscz4   NUMBER(11,2) NOT NULL,\
               nsl      NUMBER(8) NOT NULL,\
               dyjzk    NUMBER(11,2) NOT NULL,\
               dxzyjk   NUMBER(11,2) NOT NULL,\
               dsjk     NUMBER(11,2) NOT NULL,\
               dwjk     NUMBER(11,2) NOT NULL,\
               ddlsxf   NUMBER(11,2) NOT NULL,\
               nyfkfs   NUMBER(2) NOT NULL,\
               nsfkfs   NUMBER(2) NOT NULL,\
               nbl1     NUMBER(8) NOT NULL,\
               nbl2     NUMBER(8) NOT NULL,\
               dbl1     NUMBER(11,2) NOT NULL,\
               dbl2     NUMBER(11,2) NOT NULL,\
               cbl1     VARCHAR2(20) NOT NULL,\
               cbl2     VARCHAR2(20) NOT NULL,\
               cjh      VARCHAR2(9) NOT NULL,\
               cczydh   VARCHAR2(6) NOT NULL,\
               nlkrq    NUMBER(8) NOT NULL,\
               nsfm     NUMBER(8) NOT NULL,\
               clxbz    VARCHAR2(10) NOT NULL, \
               nxzbz    NUMBER(1) NOT NULL,\
               ncsbz    NUMBER(1) NOT NULL,\
               cbz      VARCHAR2(100) NOT NULL, \
               cbl3      VARCHAR2(20) NOT NULL, \
               cbl4      VARCHAR2(20) NOT NULL, \
               cbl5      VARCHAR2(20) NOT NULL, \
               cbl6      VARCHAR2(20) NOT NULL, \
               cbl7      VARCHAR2(20) NOT NULL, \
     	       cylsh      VARCHAR2(40) NOT NULL, \
               clsh      VARCHAR2(40) NOT NULL)",sJym);
     if(nRetVal>0)
       nRetVal=RunSql("CREATE index dl$yhbz_%t_idx on dl$ywmx_%t(cyhbz,nxzbz)",
       sJym,sJym);

     if(nRetVal>0)
       nRetVal=RunSql("CREATE index dl$lsh_%t_idx on dl$ywmx_%t(clsh)",
       sJym,sJym);
     
     if((nCxms==521 ||nCxms==522)&&nRetVal>0)
     {
     	    nRetVal=RunSql("alter table dl$ywmx_%t "
     	          "ADD  (cbl8      VARCHAR2(50) NOT NULL,"
                  "cbl9      VARCHAR2(50) NOT NULL,"
                  "cbl10      VARCHAR2(50) NOT NULL,"
                  "cbl11      VARCHAR2(50) NOT NULL,"
                  "cbl12      VARCHAR2(50) NOT NULL,"
                  "cbl13      VARCHAR2(50) NOT NULL,"
                  "cbl14     VARCHAR2(50) NOT NULL,"
	              "cbl15     VARCHAR2(50) NOT NULL,"	              
   	       	      "cbl16     VARCHAR2(50) NOT NULL,"
   	              "cbl17     VARCHAR2(50) NOT NULL,"
                  "cbl18     VARCHAR2(50) NOT NULL)",sJym);
     	       
            nRetVal=RunSql("CREATE TABLE DL$KHHGL( \
               	cjsdh  VARCHAR2(9)  NOT NULL,\
                ckhh  VARCHAR2(40)  NOT NULL,\
                cskgk VARCHAR2(30) NOT NULL,\
                czh   VARCHAR2(30) NOT NULL)");
     }//endif
	 if(nCxms==140)
	 {
	 		nRetVal=RunSql("CREATE TABLE DL$DXFP( \
               	cfpmc VARCHAR2(30) NOT NULL,\
               	dfpje NUMBER(10,2) NOT NULL,\
                nxh   NUMBER(8) NOT NULL,\
               	nlkrq NUMBER(8) NOT NULL,\
               	fpzs NUMBER(2) NOT NULL)");
	 }//end dxfp
	 
     for(nId=1;nId<=50;nId++)
     {
         if(nRetVal>0)
           nRetVal=RunSql("insert into dl$jymcsfb(cjym,nbh,ccs) values(%s,%ld,'0')",
                   sJym,nId);
         else break;
     }
     
     if(nRetVal>0)              
     {
     	 CommitWork();
     	 return ApplyToClient("%ld%s",1,"建立代收交易码成功...");
     }
     
     GetSqlErrInfo(sbuf);
     RollbackWork();
     return ApplyToClient("%ld%s",-1,sbuf);	 
}


/*===================================================
函数：dsTaskCreateSpJym      [TASK_CREATE_SP_JYM]
参数： 
      sJym:交易码
      sJymc:交易名称
      nMode:表示该交易码的属性
            MODE_CUSTOMER:具有客户信息属性 CLXBZ[1]
            MODE_IDENTIFIED:具有商品编号属性 CLXBZ[2]
            
返回：
      1：成功
      -1：失败及失败原因
*===================================================*/
long dsTaskCreateSpJym(char * sJym,char * sJymc,long nMode)
{
     
     long nRetVal,nId;
     char sbuf[201],sLxbz[21]="00000000000000000000";
     
     nRetVal=RunSelect("select * from dl$jym where cjym=%s",sJym);
     if(nRetVal>0)
     	return ApplyToClient("%ld%s",-1,"交易码建立失败：该交易码已经存在...");
     
     if(nMode&MODE_CUSTOMER) sLxbz[0]='1';
     if(nMode&MODE_IDENTIFIED) sLxbz[1]='1';
     nRetVal=RunSql("insert into dl$jym(cjym,cjymc,ncxms,csjydz,cbl,nbl,dbl,clxbz)"
                    " values(%s,%s,0,'dldj','.',2,0,%s)",
                    sJym,sJymc,sLxbz);
 
     for(nId=1;nId<=20;nId++)
     {
         if(nRetVal>0)
           nRetVal=RunSql("insert into dl$jymcsfb(cjym,nbh,ccs) values(%s,%ld,'0')",
                   sJym,nId);
         else break;
     }

     if(nRetVal>0)              
     {
     	 CommitWork();
     	 return ApplyToClient("%ld%s",1,"建立代售交易码成功...");
     }
     
     GetSqlErrInfo(sbuf);
     RollbackWork();
     return ApplyToClient("%ld%s",-1,sbuf);	 
}

/*===================================================
函数：dsTaskOpenJym      [TASK_OPEN_JYM]
参数： 
      sJym:交易码
      sJsdh:被开入或被禁止的局号
      sWhy:被禁止的提示语
      nMode: 0：开放  1：禁止
            
返回：
      1：成功
      -1：失败及失败原因
*===================================================*/
long dsTaskOpenJym(char * sJym,char * sJsdh,char * sWhy,long nMode)
{
     
     char sJh[10],sOpenJh[10],sbuf[101];
     long nId,nLen,nRetVal;
     
     strcpy(sJh,sJsdh);
     alltrim(sJh);
     nLen=strlen(sJh);
     
     nId=OpenCursor("select cjh from dl$jh where substr(cjh,1,%ld)=substr(%s,1,%ld)",
                     nLen,sJh,nLen);

     if(nId<0)
     {
     	GetSqlErrInfo(sbuf);
     	return ApplyToClient("%ld%s",-1,sbuf);
     }
     
     for(;;)
     {
     	if(FetchCursor(nId,"%s",sOpenJh)<=0) break;
     	
     	RunSql("delete dl$jymkzfb where cjym=%s and cjh=%s",sJym,sOpenJh);
        nRetVal=RunSql("insert into dl$jymkzfb(cjym,cjh,nsyzt,cxx,clxbz) "
                "values(%s,%s,%ld,%s,'00000000000000000000')",
                sJym,sOpenJh,nMode,sWhy);
        if(nRetVal<0) break;
     }
     CloseCursor(nId);
     if(nRetVal>0)
     {
     	CommitWork();
     	if(nMode==0)
     	  return ApplyToClient("%ld%s",1,"交易码开放成功...");
     	else
     	  return ApplyToClient("%ld%s",1,"交易码禁止成功...");
     }
     GetSqlErrInfo(sbuf);
     RollbackWork();
     return ApplyToClient("%ld%s",-1,sbuf);

}


long dsTaskDeleteJym(char * sJym)
{
	long nRetVal,nId;
	char sbuf[201];
     
	nRetVal=RunSelect("select * from dl$jym where cjym=%s",sJym);
	if(nRetVal<=0)
		return ApplyToClient("%ld%s",-1,"该交易码不存在...");

	RunSql("delete dl$jymkzfb where cjym=%s",sJym);
	nRetVal=RunSql("delete dl$jym where cjym=%s",sJym);
	if(nRetVal>0)
		nRetVal=RunSql("delete dl$jymcsfb where cjym=%s",sJym);
	
	
	if(sJym[0]=='0'&&sJym[1]=='1'&&nRetVal>=0)
	{
		nRetVal=RunSql("drop table dl$ywmx_%t",sJym);
		if(sJym[3]=='1'){
			RunSql("drop table dl$xjkzb");
			RunSql("drop table dl$khhgl");
		}
		if(sJym[4]=='4')	RunSql("drop table dl$dxfp");
	}
	if(sJym[0]=='0'&&sJym[1]=='3'&&nRetVal>=0)
		nRetVal=RunSql("drop table dl$dbmx_%t",sJym);
     
	if(nRetVal>=0)
	{
		CommitWork();
		return ApplyToClient("%ld%s",1,"交易码删除成功...");
	}
	GetSqlErrInfo(sbuf);
	RollbackWork();
	return ApplyToClient("%ld%s",-1,sbuf);
}

/*===================================================
函数：dsTaskCreateSpJym      [TASK_CREATE_DB_JYM]
参数： 
      sJym:交易码
      sJymc:交易名称
           
返回：
      1：成功
      -1：失败及失败原因
*===================================================*/
long dsTaskCreateDbJym(char * sJym,char * sJymc,char * sSjydz,long nCxms)
{
     
     long nRetVal,nId;
     char sbuf[201],sLxbz[21]="00000000000000000000";
     
     nRetVal=RunSelect("select * from dl$jym where cjym=%s",sJym);
     if(nRetVal>0)
     	return ApplyToClient("%ld%s",-1,"交易码建立失败：该交易码已经存在...");
     
     nRetVal=RunSql("insert into dl$jym(cjym,cjymc,ncxms,csjydz,cbl,nbl,dbl,clxbz)"
                    " values(%s,%s,%ld,%s,'.',3,0,%s)",
                    sJym,sJymc,nCxms,sSjydz,sLxbz);
 
     for(nId=1;nId<=50;nId++)
     {
         if(nRetVal>0)
           nRetVal=RunSql("insert into dl$jymcsfb(cjym,nbh,ccs) values(%s,%ld,'0')",
                   sJym,nId);
         else break;
     }
     if(nRetVal>0)              
     {
         nRetVal=RunSql("CREATE TABLE dl$dbmx_%t( "
                        "clsh      VARCHAR2(40) NOT NULL,"
                        "nlkrq     number(8)    not null,"
                        "nsfm      number(8)    not null,"
                        "cczydh    varchar(6)   not null,"
                        "cvalue1   varchar2(40)  not null,"
                        "cvalue2   varchar2(40)  not null,"
                        "cvalue3   varchar2(40)  not null,"
                        "cvalue4   varchar2(40)  not null,"
                        "cvalue5   varchar2(40)  not null,"
                        "cvalue6   varchar2(50)  not null,"
                        "cvalue7   varchar2(50)  not null,"
                        "cvalue8   varchar2(50)  not null,"
                        "cvalue9   varchar2(50)  not null,"
                        "cvalue10   varchar2(50) not null,"
                        "cvalue11   varchar2(50)  not null,"
                        "cvalue12   varchar2(50)  not null,"
                        "cvalue13   varchar2(50)  not null,"
                        "cvalue14   varchar2(50)  not null,"
                        "cvalue15   varchar2(50) not null,"
                        "nvalue1   number(12)   not null,"
                        "nvalue2   number(12)   not null,"
                        "nvalue3   number(12)   not null,"
                        "nvalue4   number(12)   not null,"
                        "nvalue5   number(12)   not null,"
                        "nvalue6   number(12)   not null,"
                        "nvalue7   number(12)   not null,"
                        "nvalue8   number(12)   not null,"
                        "nvalue9   number(12)   not null,"
                        "nvalue10  number(12)   not null,"
						"nvalue11  number(12)   not null,"
                        "nvalue12  number(12)   not null,"
                        "nvalue13  number(12)   not null,"
                        "nvalue14  number(12)   not null,"
                        "nvalue15  number(12)   not null,"
                        "dvalue1   number(12,2) not null,"
                        "dvalue2   number(12,2) not null,"
                        "dvalue3   number(12,2) not null,"
                        "dvalue4   number(12,2) not null,"
                        "dvalue5   number(12,2) not null,"
                        "dvalue6   number(12,2) not null,"
                        "dvalue7   number(12,2) not null,"
                        "dvalue8   number(12,2) not null,"
                        "dvalue9   number(12,2) not null,"
                        "dvalue10  number(12,2) not null)",
                         sJym);
         if(nRetVal>0)
            nRetVal=RunSql("CREATE index dl$lsh_%t_idx on dl$dbmx_%t(clsh)",
                    sJym,sJym);
     }

     if(nRetVal>0)              
     {
     	 CommitWork();
     	 return ApplyToClient("%ld%s",1,"建立代办交易码成功...");
     }
     
     GetSqlErrInfo(sbuf);
     RollbackWork();
     return ApplyToClient("%ld%s",-1,sbuf);	 
}

//TOM新增异地交易
long dsTaskGetYdJym(char * sJym)
{
	long nId;
	char sJh[7]=".";
        char sbuf[128];
        char Jym[16],Jymc[51];
        long nYwjb=0;

	memset(sbuf,'\0',strlen(sbuf));
	nId=RunSelect("select cjym,cjymc from dl$jym where substr(cjym,1,9)=substr(%s,1,9) into %s%s",sJym,Jym,Jymc);
	if(nId<=0)
	{
	   strcpy(sbuf," 异地还无此交易！");
	   return ApplyToClient("%ld%s",0,sbuf);
        }
        RunSelect("select to_number(ccs) from dl$jymcsfb where cjym=%s and nbh=25 into %ld",sJym,&nYwjb);
        sprintf(sbuf,"%s|%s|%ld|",Jym,Jymc,nYwjb);
        return 	ApplyToClient("%ld%s",1,sbuf);
	
}
//新增异地交易开放检测
long dsTaskCheckOpen(char * sJym,char * sJh)
{
     long nId,nLen;

     alltrim(sJh);
     nLen=strlen(sJh);
     
     nId=RunSelect("select * from dl$jymkzfb where cjym=%s and substr(cjh,1,%ld)=substr(%s,1,%ld) and nsyzt=0",
                     sJym,nLen,sJh,nLen);
      
     if(nId<0) nId=0;
     return ApplyToClient("%ld",nId);
     
}

