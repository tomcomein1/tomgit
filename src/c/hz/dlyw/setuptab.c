/*==============================================================
           setuptab.c:安装模块[表建立]
           
程序员：陈博

日期       修改人      原因


*==============================================================*/

long CreaetJh()        //局号
{
    long nRetVal;
    outtext("\n正在建立局号表(dl$jh)...");
    nRetVal=RunSql("CREATE TABLE dl$jh( \
               cjh VARCHAR2(9) NOT NULL primary key, \
               cjm VARCHAR2(20) NOT NULL, \
               czhwjh varchar(9) NOT NULL, \
               nlb NUMBER(1)   NOT NULL, \
               clxbz VARCHAR2(10) NOT NULL, \
               cbz VARCHAR2(20) NOT NULL)");
    if(nRetVal>0)
       return RunSql("create index dl$jh_cjh_idx on dl$jh(nlb)");
           	               
}


long CreatePrint()
{
    outtext("\n正在建立打印单据格式表(dl$print)...");
    return RunSql("create table dl$print (\
                      CNAME       VARCHAR2(51)  NOT NULL,\
                      CTABLE      VARCHAR2(51)  NOT NULL,\
                      NCOL        NUMBER(4)     NOT NULL,\
                      NROW        NUMBER(4)     NOT NULL,\
                      CVALUE      VARCHAR2(150) NOT NULL,\
                      NFLAG       NUMBER(2)     NOT NULL,\
                      NLENGTH     NUMBER(4)     NOT NULL,\
                      NPRECISION  NUMBER(2)     NOT NULL)");
}
                

long  CreateCzyb()      //操作员
{
    outtext("\n正在建立操作员表(dl$czyb)...");
    return RunSql("create table dl$czyb(\
                   cczydh varchar2(6) not null,\
                   cczymc varchar2(8) not null,\
                   nczyqx number(2) not null,\
                   cpassword varchar(8) not null,\
                   cjh varchar(9) not null,\
                   cbl varchar2(20) not null,\
                   nbl number(10) not null,\
                   dbl number(12,2) not null,\
                   clxbz varchar2(20) not null,\
                   constraint dl$czyb_uni unique (cczydh))");

}

long  CreateJymkzfb()   //交易开展
{

    outtext("\n正在建立交易开展表(dl$jymkzfb)...");
    return RunSql("create table dl$jymkzfb(\
                   cjym varchar(15) not null,\
                   cjh varchar(9) not null,\
                   nsyzt number(2) not null,\
                   cxx varchar2(80) not null,\
                   clxbz varchar2(20) not null,\
                   constraint dl$jymkzfb_uni unique(cjym,cjh))");

}


long  CreateJymcsfb()   //交易参数
{
    outtext("\n正在建立交易码参数附表(dl$jymcsfb)...");
    return RunSql("create table dl$jymcsfb(\
                   cjym varchar(15) not null,\
                   nbh  number(4)    not null,\
                   ccs  varchar2(50) not null,\
                   constraint dl$jymcsfb_uni unique(cjym,nbh))");
}


long  CreateJym()       //交易码
{
    outtext("\n正在形成交易码表(dl$jym)...");
    return RunSql("create table dl$jym(\
                   cjym   varchar(15)  not null,\
                   cjymc  varchar2(51) not null,\
                   ncxms  number(10)   not null,\
                   csjydz varchar2(21) not null,\
                   cbl    varchar2(40) not null,\
                   nbl    number(10)   not null,\
                   dbl    number(12,2) not null,\
                   clxbz  varchar2(20) not null,\
                   constraint dl$jym_uni unique(cjym))");
}


long  CreateZwmx()      //帐务明细
{
  long nRetVal;
  outtext("\n正在形成帐务明细表(dl$zwmx)...");
  nRetVal=RunSql("CREATE TABLE dl$zwmx( \
               cjym  VARCHAR2(15) NOT NULL, \
               nysnd NUMBER(4) NOT NULL,\
               nysyf NUMBER(4) NOT NULL,\
               cyhbz  VARCHAR2(22) NOT NULL, \
               cyhbz1  VARCHAR2(22) NOT NULL, \
               cyhbz2  VARCHAR2(20) NOT NULL, \
               cyhmc  VARCHAR2(80) NOT NULL, \
               nywlb NUMBER(1) NOT NULL,\
               nxh   NUMBER(10) NOT NULL,\
               nsl      NUMBER(8) NOT NULL,\
               dyjzk    NUMBER(11,2) NOT NULL,\
               dxzyjk   NUMBER(11,2) NOT NULL,\
               dsjk     NUMBER(11,2) NOT NULL,\
               ddlsxf   NUMBER(11,2) NOT NULL,\
               dybk     NUMBER(11,2) NOT NULL,\
               dytk     NUMBER(11,2) NOT NULL,\
               dsjkxj   NUMBER(11,2) NOT NULL,\
               dsjkcz   NUMBER(11,2) NOT NULL,\
               dsjkxyk   NUMBER(11,2) NOT NULL,\
               nsfkfs    NUMBER(2)    NOT NULL,\
               cjkdh     VARCHAR2(15) NOT NULL,\
               cjh       VARCHAR2(9)  NOT NULL,\
               cczydh    VARCHAR2(6)  NOT NULL,\
               nlkrq     NUMBER(8)    NOT NULL,\
               nsfm      NUMBER(8)    NOT NULL,\
               clxbz     VARCHAR2(20) NOT NULL, \
               nxzbz     NUMBER(1)    NOT NULL,\
               ncsbz     NUMBER(1)    NOT NULL,\
               ncscs     NUMBER(4)    NOT NULL,\
               njsbz     NUMBER(1)    NOT NULL,\
               cwbjsdh   VARCHAR2(20) NOT NULL,\
               cnbjsdh   VARCHAR2(20) NOT NULL,\
               cbl       VARCHAR2(20) NOT NULL,\
               nbl       NUMBER(8)    NOT NULL,\
               cbl1       VARCHAR2(20) NOT NULL,\
               cbl2       VARCHAR2(20) NOT NULL,\
               cbl3       VARCHAR2(20) NOT NULL,\
               cbl4       VARCHAR2(20) NOT NULL,\
               cbl5       VARCHAR2(20) NOT NULL,\
               cylsh      VARCHAR2(40) NOT NULL ,\
               clsh      VARCHAR2(40) NOT NULL  primary key)");
               
//    if(nRetVal>0) nRetVal=RunSql("CREATE index dl$zwmx_lsh_idx on dl$zwmx(clsh)");
    if(nRetVal>0) nRetVal=RunSql("CREATE index dl$zwmx_jym_idx on dl$zwmx(cjym)");
    if(nRetVal>0) nRetVal=RunSql("CREATE index dl$zwmx_jh_idx on dl$zwmx(cjh)");
    if(nRetVal>0) nRetVal=RunSql("CREATE index dl$zwmx_lkrq_idx on dl$zwmx(nlkrq)");
    if(nRetVal>0) nRetVal=RunSql("CREATE index dl$zwmx_yhbz_idx on dl$zwmx(cyhbz,cyhbz1,cyhbz2)");
  
  return nRetVal;

}


long  CreateLkZwmx()      //绿卡帐务明细
{
  long nRetVal;
  outtext("\n正在形成绿卡帐务表(dl$lkzwmx)...");

  nRetVal=RunSql("CREATE TABLE dl$lkzwmx(\
        cLkzh   VARCHAR2(19) NOT NULL,\
        cYhbz   VARCHAR2(20) NOT NULL,\
        cDllsh  VARCHAR2(40) NOT NULL,\
        cLkjyjh VARCHAR2(15) NOT NULL,\
        cDljyjh VARCHAR2(15) NOT NULL,\
        cCzydh  VARCHAR2(6)  NOT NULL,\
        cCzymc  VARCHAR2(12) NOT NULL,\
        cLxbz   VARCHAR2(10) NOT NULL,\
        dJyje   NUMBER(11,2) NOT NULL,\
        nLklsh  NUMBER(10)   NOT NULL,\
        nDzbz   NUMBER(1)    NOT NULL,\
        nDzrq   NUMBER(8)    NOT NULL,\
        nSfm    NUMBER(6)    NOT NULL,\
        nLkrq   NUMBER(8)    NOT NULL)");
  return nRetVal;
}

                   
long  CreateGs()        //公司编号及名称
{
    outtext("\n正在形成公司表(dl$gs)...");
    return RunSql("create table dl$gs(\
                   ngsbh number(3) not null,\
                   cgsmc varchar2(20) not null,\
                   cbl varchar2(20) not null,\
                   nbl number(10) not null,\
                   dbl number(12,2) not null,\
                   clxbz varchar2(20) not null,\
                   constraint dl$gs_uni unique(ngsbh))");

}

long  CreateDlywcs()    //代理系统参数
{
    outtext("\n正在形成代理业务参数表(dl$dlywcs)...");
    return RunSql("create table dl$dlywcs(\
                   nbh number(3) not null primary key,\
                   ccs varchar2(51) not null)");

}
long  CreateYwzl()      //业务种类
{

    outtext("\n正在形成代理业务种类(dl$ywzl)...");
    return RunSql("create table dl$ywzl(\
                   nywbh number(4) not null,\
                   cywmc varchar2(20) not null,\
                   cbl varchar2(20) not null,\
                   nbl number(10) not null,\
                   dbl number(12,2) not null,\
                   clxbz varchar2(20) not null,\
                   constraint dl$ywzl_uni unique(nywbh))");

}

long  CreateRoute()     //路由表
{
    outtext("\n正在形成路由表(dl$route)...");
    return RunSql("create table dl$route(\
                   sdestination varchar2(20) not null,\
                   sgateway     varchar2(20) not null)");
                   
}

long  CreateHwjcb()     //货物进出表
{
    outtext("\n正在形成货物进出表(dl$hwjcb)...");
    return RunSql("CREATE TABLE dl$hwjcb( \
	CJYM                    VARCHAR2(15)   NOT NULL,\
	CJYMC                   VARCHAR2(50)   NOT NULL,\
	CLSH                    VARCHAR2(40)   NOT NULL  primary key,\
	DJHDJ                   NUMBER(10, 2)  NOT NULL ,\
	NSL                     NUMBER(10)     NOT NULL    ,\
	CHWBZ                   VARCHAR2(40)   NOT NULL ,\
	CFHczydh                  VARCHAR2(20)    NOT NULL , \
	CJHczydh                 VARCHAR2(20)    NOT NULL  ,\
	CFHJH                   VARCHAR2(9)    NOT NULL    ,\
	CJHJH                   VARCHAR2(9)    NOT NULL   ,\
	NJHRQ                   NUMBER(8)      NOT NULL   ,\
	NJHSFM                  NUMBER(6)      NOT NULL   ,\
	NFHRQ                   NUMBER(8)      NOT NULL   ,\
	NFHSFM                  NUMBER(6)      NOT NULL   ,\
	NCZLB                   NUMBER(2)      NOT NULL   ,\
	NJLZT                   NUMBER(2)      NOT NULL   ,\
    	CLXBZ                   VARCHAR2(20)   NOT NULL   ,\
	NBL1                    NUMBER(10)     NOT NULL   ,\
	NBL2                    NUMBER(10)     NOT NULL   ,\
	DBL1                    NUMBER(10, 2)  NOT NULL   ,\
	DBL2                    NUMBER(10, 2)  NOT NULL  ,\
	CBL1                    VARCHAR2(20)   NOT NULL  ,\
	CBL2                    VARCHAR2(40)   NOT NULL )");

}
long  CreateHwkcb()     //货物库存表
{

    outtext("\n正在形成货物库存表(dl$hwkcb)...");
    RunSql("CREATE TABLE dl$hwkcb( \
	CJYM                    VARCHAR2(15)   NOT NULL  ,\
	CSYZ                    VARCHAR2(20)   NOT NULL  ,\
	CJH                     VARCHAR2(10)   NOT NULL  ,\
	NKCL                    NUMBER(10)     NOT NULL,\
	NLKRQ                   NUMBER(8)      NOT NULL,\
	NSFM                    NUMBER(6)      NOT NULL, \
	CLXBZ                   VARCHAR2(10)   NOT NULL,\
	NBL                     NUMBER(10)     NOT NULL ,\
	DBL                     NUMBER(10, 2)  NOT NULL ,\
	CBL                     VARCHAR2(40)   NOT NULL )");
}

long  CreateLkzhgl()    //绿卡帐号
{
    long nRetVal;
    outtext("\n正在形成绿卡帐号管理表表(dl$lkzhgl)...");
    
    nRetVal=RunSql("CREATE TABLE dl$lkzhgl( \
               clkzh  VARCHAR2(40) NOT NULL ,\
               cdw    VARCHAR2(20) NOT NULL ,\
               cbl    VARCHAR2(20) NOT NULL ,\
               clxbz  VARCHAR2(10) NOT NULL ,\
               cbz    VARCHAR2(50) NOT NULL ,\
               DLKYE  NUMBER(11,2) NOT NULL ,\
               NLKRQ  NUMBER(8)    NOT NULL ,\
               NSFM   NUMBER(6)    NOT NULL ,\
               CCZYDH VARCHAR2(6)  NOT NULL ,\
               CJH    VARCHAR2(9)  NOT NULL)");
    if(nRetVal>0)
       nRetVal=RunSql("CREATE UNIQUE INDEX dl$lkzhgl_idx ON \
                       dl$lkzhgl(clkzh,cdw,cjh)");
    return nRetVal;                   

}

long  CreateLkzjhd()    //绿卡资金
{
    long nRetVal;
    outtext("\n正在形成绿卡资金核对表(dl$lkzjhd)");

    nRetVal=RunSql("CREATE TABLE dl$lkzjhd( \
               clkzh  VARCHAR2(40) NOT NULL ,\
               cdw    VARCHAR2(20) NOT NULL ,\
               dscye  NUMBER(11,2) NOT NULL ,\
               dbcye  NUMBER(11,2) NOT NULL ,\
               dlkzk  NUMBER(11,2) NOT NULL ,\
               ddlzk  NUMBER(11,2) NOT NULL ,\
               dbcce  NUMBER(11,2) NOT NULL ,\
               dscce  NUMBER(11,2) NOT NULL ,\
               dzce    NUMBER(11,2) NOT NULL ,\
               cbl    VARCHAR2(20) NOT NULL ,\
               clxbz  VARCHAR2(10) NOT NULL ,\
               cbz    VARCHAR2(50) NOT NULL ,\
               Nscjzrq  NUMBER(8)    NOT NULL ,\
               Nbcjzrq  NUMBER(8)    NOT NULL ,\
               Nccqq  NUMBER(8)    NOT NULL ,\
               NLKRQ  NUMBER(8)    NOT NULL ,\
               NSFM   NUMBER(6)    NOT NULL ,\
               CCZYDH VARCHAR2(6)  NOT NULL ,\
               CJH    VARCHAR2(9)  NOT NULL)");
    if(nRetVal>0)
          RunSql("CREATE UNIQUE INDEX dl$lkzjgl_idx ON \
                  dl$lkzjgl(clkzh,nbcjzrq)");
    return nRetVal;                  
}


long CreateSequence()
{
    long nRetVal;
    outtext("\n正在建立序列...");
    
    nRetVal=RunSql("CREATE SEQUENCE DLNBLSH INCREMENT BY 1 START WITH 1 \
       MAXVALUE 99999999 CYCLE ");
    if(nRetVal>0) nRetVal=RunSql("CREATE SEQUENCE DLWTLSH INCREMENT BY 1 \
       START WITH 1 MAXVALUE 99999999 CYCLE ");
    if(nRetVal>0) nRetVal=RunSql("CREATE SEQUENCE DLNKLSH INCREMENT BY 1 \
       START WITH 1 MAXVALUE 99999999 CYCLE ");
    if(nRetVal>0) nRetVal=RunSql("CREATE SEQUENCE lklsh INCREMENT BY 1 \
       START WITH 1 MAXVALUE 999999 MINVALUE 1 ");
    return nRetVal;   
}


//==============*****************=================//
/*long CreateAirCity()   //航空城市对照表
{
   long nRetVal;
   
   outtext("\n正在建立航空城市对照表(DL$AIR_CITY)...");
   nRetVal=RunSql("create table DL$AIR_CITY("
                  "cname     varchar(40) not null,"
                  "ccode     varchar(4)  not null primary key,"
                  "csearch    varchar(10) not null,"
                  "nweight    number(1) not null)");
   if(nRetVal>0)
      nRetVal=RunSql("create unique index dl$air_city_idx on dl$air_city(csearch)");
   
   return nRetVal;   

}

long CreateAirRebate()
{
   long nRetVal;
   
   outtext("\n正在建立机票折扣管理表(DL$AIR_REBATE)...");
   nRetVal=RunSql("create table DL$AIR_REBATE("
                  "ccompany   varchar(2)  not null,"
                  "croute     varchar(6)  not null,"
                  "cberth     varchar(2)  not null,"
                  "drebate    number(6,2) not null," 
                  "nstdate   number(8)   not null,"
                  "nendate   number(8)   not null,"
                  "nflag      number(2)   not null)");
   if(nRetVal>0)
       nRetVal=RunSql("create unique index dl$air_rebate_idx on dl$air_rebate(ccompany,croute,cberth)");
   
   return nRetVal;   

}

long CreateAirCompany()
{
   long nRetVal;
   
   outtext("\n正在建立航空公司管理表(DL$AIR_COMPANY)...");
   nRetVal=RunSql("create table DL$AIR_COMPANY("
                  "ccompany   varchar(2)  not null,"
                  "cname      varchar(21) not null,"
                  "dsxf       number(8,2) not null)");
   if(nRetVal>0)
      nRetVal=RunSql("create unique index dl$air_company_idx on dl$air_company(ccompany)");
   
   return nRetVal;   

}*/

/*
   create table dl$czyb(cczydh varchar2(6) not null,cczymc varchar2(8) not null,
                      nczyqx number(2) not null, cpassword varchar(8) not null,
                      cjh varchar(9) not null,cbl varchar2(20) not null,
                      nbl number(10) not null,dbl number(12,2) not null,
                      clxbz varchar2(20) not null,
                      constraint dl$czyb_uni unique (cczydh));
*/
