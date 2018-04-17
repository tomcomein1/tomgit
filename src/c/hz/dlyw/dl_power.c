#include"dlpublic.h"
#include"tools.h"
#include"tasktool.h"
#include"dl_power.h"
#include"newtask.h"
#include"dlprint.h"
#include"query.h"

extern char sTcpipErrInfo[81];
extern long nSysLkrq,nSysCzyqx;
extern char sSysCzydh[7];
extern char sSysCzymc[12];
extern char sSysJsdh[10],sSysJm[22];

void PowerSignIn(long nType,char * sItem,char * sJym)
{
     long nRetVal;
     char sInfo[101];
     long nLkrq=0;
     
     clrscr();
     outtext("正在向供电局签到,请等待......\n");
     
     nRetVal=AutoRunTask("dldj",TASK_POWER_SIGN_IN,"%s","%s",sJym,sInfo);
    
     if(nRetVal)
     {
        outtext(sTcpipErrInfo);
        keyb();
        return;
     }             
     if(!strncmp("000000",sInfo,6)) strcpy(sInfo,"签到成功，按任意键返回。\n");
     outtext(sInfo);
     keyb();

}

//向移动签退
void PowerSignOut(long nType,char * sItem,char * sJym)
{
     char sResult[256];           //签到返回结果
     long nRetVal;
     char sBankCode[9];
     TABLE * pTab; 
  
     pTab=CreateTable(7,
          "|向电业局签退...                                           |");
     if(!pTab) return;

     FillTable(pTab,2,"%s","                 -----  警  告  ------\n    一旦签退后,今日就不能开展除交费查询以外的任何业务!\n");
     FillTable(pTab,4,"%s",
       "    如果您确认需要签退,请按'1'键,或按其它键返回.签退成功后系统将自动与供电方进行对帐.\n");
     FillTable(pTab,7,"%s","   请输入您的选择....");
     clrscr();
     ShowTable(pTab,10,6);
     if(keyb()!='1')
     {
        DropTable(pTab);
        return;
     } 
     FillTable(pTab,7,"%-50.50s","正在向电业局签退,请等待...");
     fflush(stdout);

     nRetVal=AutoRunTask("dldj",TASK_POWER_SIGN_OUT,"%s","%s",sJym,sResult);
     
     if(nRetVal)
     {
        FillTable(pTab,7,"%-50.50s",sTcpipErrInfo);
        DropTable(pTab);
        keyb();
        return;
     }             
     if(!strncmp("000000",sResult,6))
     {
        //签退成功后立即与对方对帐
        DropTable(pTab);
        PowerCheckAccount(0,NULL,sJym);
     }        
     else
     {
     	FillTable(pTab,7,"%-50.50s",sResult);
     	DropTable(pTab);
        keyb();
     }

}

void PowerCheckAccount(long ntype,char * sitem,char * sJym)  
{
    char sAnswer[1024]=".";
    long nLkrq=0,nRetVal;
    
    GetSysLkrq();
    
    nRetVal=RunSelect("select * from dl$zwmx where ncsbz=1 and cjym=%s",sJym);
    
    if(nRetVal>0)
       RunSelect("select min(nlkrq) from dl$zwmx where ncsbz=1 and cjym=%s into %ld",
                      sJym,&nLkrq);

    if(nRetVal<0) return;
    
    if(nRetVal==0)
    {
    	nRetVal=RunSelect("select * from dl$zwmx where ncsbz=2 and cjym=%s and nlkrq=%ld",
    	             sJym,nSysLkrq);
    	
    	if(nRetVal>0)
    	{
    		MessageBox("系统已经成功对帐！",1);
    		return;
    	}
    }
                          
    clrscr();
    if(nLkrq==0) nLkrq=nSysLkrq;
    printf("\n            你对帐的日期：[%ld]",nLkrq);
    if(nLkrq==nSysLkrq||nRetVal==0)
    {
         if(MessageBox("  提醒：对帐后将终止今日业务，您确定要进行对帐码？",0)!=KEY_YES) return;
    }
    else
    {
         if(MessageBox("  尚有往日未对帐的记录，选择\"确认\"将先对往日帐，并不会影响本日业务。",0)!=KEY_YES) return;
    }    	          
    clrscr();
    outtext("\n正在和电业局对帐，这可能需要较长时间，请耐心等待...\n");
    
    if(AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sAnswer))
    {
        outtext(sTcpipErrInfo);
        keyb();
        return;
    }
    if(!strncmp(sAnswer,"000000",6)) strcpy(sAnswer,"对帐成功！按任意键返回...");
    outtext(sAnswer);
    keyb();
}

void QueryPowerbyJh(long ntype,char * cjymc,char * cjym)
{

    QUERYTABLE * pqt;
    long nCount=0,nId,nId1,nRetVal,nRetVal1,nLkrq,nSfm,nXzbz=2;
    double dSjk=0.0,dZje=0.0;
    char sCzydh[7],sYhbz[21],sLxbz[21],sYhbz1[21];
    char sJh[10];
    char sTitle[81];
    long nZsl=0,nXh=0,nZbs=0;
    double dZk=0;
    long nKey,nLoop,nQssj,nJzsj;
    char sDfjh[8]=".",sWhere[80]=" ";

    clrscr();
    
    nQssj=nJzsj=nSysLkrq;
    
    while(1)
	{
		switch(nLoop)
		{
			case 1:
			  nKey=GetValue(26,8,"请输入起始时间:%-8ld",&nQssj);
			  break;
			case 2:
			  nKey=GetValue(26,10,"请输入截止时间:%-8ld",&nJzsj);
			  break;
			case 3:
			  nKey=GetValue(26,12,"请输入电业局号:%-6.6s",sDfjh);
			  break;
			
			default:
			  nLoop=1;
			  continue;
		}// end switch
		/*********控制*************/
		if(nKey==KEY_ESC)	return;
		if(nKey==KEY_LEFT || nKey==KEY_UP)
		{
			nLoop--;	continue;
		}
		if(nKey==KEY_RIGHT || nKey==KEY_DOWN)
		{
			nLoop++;	continue;
		}
		if(nKey==KEY_RETURN && nLoop>=3)	break;

		nLoop++;
	}// end while
    
    clrscr();

    sprintf(sTitle,"                    代收电费划帐对照表");

    pqt=CreateQueryTable(sTitle,
    	"供电局局号   供电局局名    金额         笔数  日期      ",
    	"%-8.8s %-12.12s %-12.2lf %-4ld %8ld",
        ST_KEYWORD|ST_VISUAL|ST_LASTLINE);

    if(!pqt) return;
    clrscr();

    alltrim(sDfjh);
    if(strlen(sDfjh)>5)  sprintf(sWhere,"and substr(cyhbz,1,6)=%s",sDfjh);
    else strcpy(sWhere,"  ");
    //新增明细汇总
    RunSelect("select sum(nsl),sum(dsjk) from dl$zwmx where cjym=%s and "
    			"nlkrq>=%ld and nlkrq<=%ld %t into %ld,%lf",
    			cjym,nQssj,nJzsj,sWhere,&nZbs,&dZje);
    
	ImportLastLine(pqt,".",".",dZje,nZbs,nJzsj);
    if(strlen(sDfjh)<=5)
    {
	    nId1=OpenCursor("select substr(cyhbz,1,6),sum(nsl),sum(dsjk) from dl$zwmx "
	                   "where cjym=%s and nlkrq>=%ld and nlkrq<=%ld "
	                   " group by substr(cyhbz,1,6) ",
	                   cjym,nQssj,nJzsj);
	    if(nId1<0)
	    {
	    	HiError(nId1,"查询出错");
			DropQueryTable(pqt);
	        return;
	    }
	    
	    for(;;)
	    {
	    	nRetVal1=FetchCursor(nId1,"%s%ld%lf",sJh,&nXh,&dSjk);
	    	if(nRetVal1<0)
	    		break;
	
		    if(ImportQueryTable(pqt,nRetVal1,sJh,sJh,dSjk,nXh,nJzsj)<=0) break;
		    
		}
		CloseCursor(nId1);
    }
    else
    {
    	for(;nQssj<=nJzsj;nQssj++)
    	{
    		nXh=0; dSjk=0;
    		nId=RunSelect("select sum(nsl),sum(dsjk) from dl$zwmx "
	                   "where cjym=%s and nlkrq=%ld "
	                   "and substr(cyhbz,1,6)=%s into %ld,%lf",
	                   cjym,nQssj,sDfjh,&nXh,&dSjk);
			if(nId<0) return;
			
			ImportQueryTable(pqt,1,sDfjh,sDfjh,dSjk,nXh,nQssj);
    	}
    	ImportQueryTable(pqt,0);
    }
	DropQueryTable(pqt);
	return;
}
/////////////////////////////////////////////////////
void QueryPower_Info(long number,char *item,char *sJym)
{
    QUERYTABLE * pqt;
    long nId,nRetVal,nKey,nStartLkrq=0,nEndLkrq;
    char sYhmc[40],sTemp[3],cBt[100],sWhere[70]=".";
    char sYhbz[20],ydlb[10],gdsm[14],rq[14];
    double dSjk,dzk=0.0;
    
    clrscr();
    while(1)
    {
      clrscr();
      sYhmc[0]='\0';
      gotoxy(28,3);
      outtext("〖请输入纳税人的名称〗");
      gotoxy(8,5);
      outtext("注:支持模糊查询，如果输入(全部)则可查询所有未收用户信息。");
      nKey=GetValue(8,7,"%-60.60s",sYhmc);
      if(nKey==KEY_ESC) return;
      alltrim(sYhmc);
      if(strlen(sYhmc)<2)
      {
      	 QueryLocal("用户名称                                           用户编码          ",
      	           "%s%s","select distinct cyhmc,cyhbz from dl$ywmx_%t ",sJym);
      	 continue;
      }//help
      if(nKey==KEY_RETURN) break;
    }

    nEndLkrq=nSysLkrq;
    if(GetValue(8,8,"请输入查询时段：%8d",&nStartLkrq)==KEY_ESC) return;
    if(GetValue(32,8," 至 %8d",&nEndLkrq)==KEY_ESC) return;
     

    sprintf(cBt,"    %s邮政局  〖%s〗",sSysJm,sYhmc);
    pqt=CreateQueryTable(cBt,"用户名称              用户编码          实缴金额    用电类别  供电所名    滞纳金收取日",
                             "%-20.20s              %-16.16s          %-12.2lf     %-6.6s    %-10.10s    %-12.12s",ST_VISUAL|ST_LASTLINE);
    if(!pqt) return;
    
    sprintf(sWhere,"%s%",sYhmc);
    if(!strcmp(sYhmc,"全部"))
      nId=OpenCursor("select distinct cyhmc,cyhbz,dsfmx1,cbl1,cbl2,cbl4 "
                   "from dl$ywmx_%t where (nxzbz=0 OR nxzbz=3) "
                   "and nlkrq between %ld and %ld",
                   sJym,nStartLkrq,nEndLkrq);
    else 
      nId=OpenCursor("select distinct cyhmc,cyhbz,dsfmx1,cbl1,cbl2,cbl4 "
                   "from dl$ywmx_%t where nxzbz=0 and (cyhmc=%s or cyhmc like %s ) "
                   "and nlkrq between %ld and %ld",
                   sJym,sYhmc,sWhere,nStartLkrq,nEndLkrq);
    if(nId<0)
    {
        DropQueryTable(pqt);
	return;
    }
    while(1)
    {
    	nRetVal=FetchCursor(nId,"%s%s,%lf%s%s%s",
    	        sYhmc,sYhbz,&dSjk,ydlb,gdsm,rq);
    	if(nRetVal<0) break;
        ImportLastLine(pqt,".","总金额:",dzk,".",".",".");
    	if(ImportQueryTable(pqt,nRetVal,sYhmc,sYhbz,dSjk,ydlb,gdsm,rq)<=0) break;
	dzk+=dSjk;
    }
    CloseCursor(nId);
    DropQueryTable(pqt);
}

void QueryZnj_Info(long number,char *item,char *sJym)
{
    QUERYTABLE * pqt;
    long nId,nRetVal,nKey,nStartLkrq=0,nEndLkrq;
    char sTemp[3],cBt[100];
    char sJh[10],sWhere[20];
    double dSjk,dZnj;
    
    clrscr();
    nStartLkrq=nEndLkrq=nSysLkrq;
    if(GetValue(8,8,"请输入查询时段：%8d",&nStartLkrq)==KEY_ESC) return;
    if(GetValue(32,8," 至 %8d",&nEndLkrq)==KEY_ESC) return;


    sprintf(cBt,"    %s[%s]邮政局滞纳金信息查询",sSysJm,sSysCzymc);
    pqt=CreateQueryTable(cBt,"局号        实缴金额    滞纳金    ",
                             "%-9.9s      %-12.2lf    %-10.2lf",ST_VISUAL);
    if(!pqt) return;
    
    sWhere[0]='\0';
    if(nSysCzyqx==2) strcpy(sWhere," ");
    else sprintf(sWhere," and cjh=%s ",sSysJsdh);
    	
    nId=OpenCursor("select cjh,sum(dsfmx1),sum(dxzyjk) "
                   "from dl$ywmx_%t where nxzbz=1  "
                   "%t and nlkrq between %ld and %ld group by cjh",
                   sJym,sWhere,nStartLkrq,nEndLkrq);
    if(nId<0)
    {
        DropQueryTable(pqt);
	return;
    }
    
    while(1)
    {
    	nRetVal=FetchCursor(nId,"%s,%lf%lf",sJh,&dSjk,&dZnj);
    	if(nRetVal<0) break;
    	
    	if(ImportQueryTable(pqt,nRetVal,sJh,dSjk,dZnj)<=0) break;
    }
    CloseCursor(nId);
    DropQueryTable(pqt);
}
