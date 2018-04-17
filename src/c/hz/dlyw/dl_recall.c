/*==========================================================
           标准交费撤消及单据补打模块(dl_recall.c)
*=========================================================*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "dlpublic.h"
#include "newtask.h"
#include "tasktool.h"
#include "tools.h"

extern char sSysCzydh[7];
extern char sSysJsdh[10];
extern char sRemoteHost[21];
extern char sJym[16];
extern char sJymc[51];
extern long nSysSfm;
extern long nSysLkrq;
extern long nCxms;
extern long nSysCzyqx;
extern char sTcpipErrInfo[81];
extern char sLsh[41];
extern char sSysJh[10];

void RecallOrReprint(long ntype,char * cjym);

void PayRecall(long ntype,char * cjymc,char * cjym)
{
    RecallOrReprint(0,cjym);
}

void ReprintBill(long ntype,char * cjymc,char * cjym)
{
    RecallOrReprint(1,cjym);
}

void RecallOrReprint(long ntype,char * cjym)
{  
	long nRetVal,nLkrq,nSfm,nId,nCurrent;
	char sRecallLsh[41],sYhbz[21],sbuf[128],sCzydh[7],sOpt[3],sTemp[22];
	double dSjk;
	char cBz[2],sPass[10]="\0",sCzy[8]="\0";
	DATATABLE * pdt;
    
    
	pdt=CreateDataTable(6,
		"|  |用户标识                |交易款额  |交易日期|交易时间|",
		"%-1.1s%-22.22s%10.2f%08d%06d%-6.6s%s");
    
	MergeTable(pdt->pTable,CreateTable(1,
	"|                                                            |"));
    
    
	strcpy(sJym,cjym); 
	RunSelect("select cjymc,ncxms,csjydz from dl$jym where cjym=%s into %s%ld%s",
				sJym,sJymc,&nCxms,sRemoteHost);
	clrscr();
	if(!ntype)
		outtext("                     撤消%s\n",sJymc);
	else if(ntype==1)    outtext("                     补打%s单据\n",sJymc);
	else if(ntype==2)    outtext("                     代打%s单据\n",sJymc);
	outtext("                -------------------------------------\n");
    
	ShowDataTable(pdt,8,6);
    
	for(;;)
	{
		memset(sYhbz,'\0',sizeof(sYhbz));
		while(pdt->nCount) 
			DeleteDataNode(pdt,pdt->nCount);
		if(!ntype)
			ShowDatatableInfo(pdt,"请输入要撤消交费的用户标识，如电话号码等...",0);
		else       
			ShowDatatableInfo(pdt,"请输入已交费的用户标识，如电话号码等...",0);
		
		nRetVal=GetValue(10,4,"请输入用户标识:%-22.22s",sYhbz);
		if(nCxms==222)
		{
		    if(!strncmp(sSysJh,"2715",4)) 
		    	strcpy(sTemp,sYhbz);
		    else sprintf(sTemp,"511112%s",sYhbz);
		}
		else if(nCxms==124 || nCxms==123)
		{  
		  if(strstr(sSysJh,"2708") && nCxms==123)
	       	    strcpy(sTemp,sYhbz);
	     	  else sprintf(sTemp,"6205%s",sYhbz);
		}
		if(nCxms==222 || nCxms==124 || nCxms==123) strcpy(sYhbz,sTemp);
		
		if(nRetVal==KEY_ESC)
		{
			DropDataTable(pdt);
			return;
		}	
		if(nRetVal!=KEY_RETURN) 
			continue;

		if(strstr(sSysJh,"2719"))
			if(nSysCzyqx!=6 && nSysCzyqx!=2 && !ntype)
			{
				clearline(5);
				if(GetValue(10,5,"请输入支局长工号:%7s",sCzy)!=KEY_RETURN) continue;
				if(GetValue(34,5,"请输入支局长密码:%8p",sPass)!=KEY_RETURN) continue;
				upper(sCzy);upper(sPass);alltrim(sCzy);alltrim(sPass);
				if(RunSelect("select ROWNUM from dl$czyb where (nczyqx=2 or nczyqx=6) and cczydh=%s and cpassword=%s",sCzy,sPass)<=0)
				{
					clearline(5);gotoxy(10,5);
					outtext("你无权进行撤销!");
					continue;
				}
			}//新增西昌撤销权控制
		
		upper(sYhbz);alltrim(sYhbz);
		if(ntype!=1 && ntype!=2)
				nId=OpenCursor("select clsh,dsjk,nlkrq,nsfm from dl$zwmx "
							"where (cyhbz=%s or cyhbz1=%s or cyhbz2=%s) and ncsbz<=1 "
							"and substr(clxbz,19,1)='0' and cjym=%s and cczydh=%s "
							"and nlkrq=%ld order by -nlkrq,-nsfm",
							sYhbz,sYhbz,sYhbz,sJym,sSysCzydh,nSysLkrq);
		else if(ntype==1)
				nId=OpenCursor("select clsh,dsjk,nlkrq,nsfm from dl$zwmx "
							"where (cyhbz=%s or cyhbz1=%s or cyhbz2=%s) "
							"and substr(clxbz,19,1)='0' "
							"and substr(clxbz,2,1)='0' "
							"and ncsbz<=1 "
							"and (cczydh=%s or cczydh='DL185' or cczydh='DL183')"
							"and cjym=%s order by -nlkrq,-nsfm",
							sYhbz,sYhbz,sYhbz,sSysCzydh,sJym);
		else if(ntype==2)
				nId=OpenCursor("SELECT clsh,dyjzk,nlkrq,nsfm from dl$ywmx_%t "
								"where (cyhbz=%s or cyhbz1=%s or cyhbz2=%s) "
								"and nywlb=2",sJym,sYhbz,sYhbz,sYhbz);

		if(nId<0)
		{
			GetSqlErrInfo(sbuf);
			ShowDatatableInfo(pdt,sbuf,1);
			continue;
		}
		
		for(;;)
		{
			if(FetchCursor(nId,"%s%lf%ld%ld",
								sRecallLsh,&dSjk,&nLkrq,&nSfm)<=0) break;
			
			ImportData(pdt,pdt->nCount+1,"%s%s%f%d%d%s","+",
								sYhbz,dSjk,nLkrq,nSfm,sRecallLsh);
		}
		CloseCursor(nId);
		
		if(!pdt->nCount)
		{
			ShowDatatableInfo(pdt,"不存在该用户的缴费记录或该笔交易已经对帐...",1);
			continue;
		}      
          
		nCurrent=1;
          
		for(;;)
		{
			if(!pdt->nCount) break;
			ImportData(pdt,nCurrent,"%s","+");
			ShowDatatableInfo(pdt,"使用上下光标键选择记录，按'1'确认...",0);
			nRetVal=FetchData(pdt,nCurrent,1,"%-1.1s");

			if(nRetVal==KEY_ESC) break;
          	
			if(nRetVal==KEY_UP)
			{
				if(nCurrent>1)
					nCurrent--;
				continue;
			}	
          	
			if(nRetVal==KEY_DOWN)
			{
				if(nCurrent<pdt->nCount) 
					nCurrent++;
				continue;
			}	
          	
			if(nRetVal!=KEY_RETURN) 
				continue;
          	
			ExportData(pdt,nCurrent,"%s > > > > %s",sOpt,sRecallLsh);
          	
			if(sOpt[0]!='1') continue;
          	
			if(!ntype)  //撤消
			{
				ShowDatatableInfo(pdt,"真的要撤消吗(y/n)",0);
				nRetVal=keyb();
				if(nRetVal!='y' && nRetVal!='Y') 
					continue;
                        
				GenerateLsh(1,sSysJsdh,sJym,sLsh);

				GetSysLkrq();
				sprintf(sbuf,"%s|%s|%s|%ld|",sLsh,
								sRecallLsh,sSysCzydh,nSysSfm);
                         
				ShowDatatableInfo(pdt,"正在撤消缴费，请稍候...",0);
				
				if(AutoRunTask(sRemoteHost,TASK_RECALL_CONFIRM,"%s","%s",sbuf,sbuf))
				{
					ShowDatatableInfo(pdt,sTcpipErrInfo,1);
					continue;
				}
                        
				if(strncmp(sbuf,"000000",6))
				{
					ShowDatatableInfo(pdt,sbuf,1);
					continue;
				}
				
				
				ShowDatatableInfo(pdt,"撤消交费成功...",1);
				DeleteDataNode(pdt,nCurrent);
				nCurrent=1;
				continue;
			}
			else
			{
				ShowDatatableInfo(pdt,"请准备好打印机，按回车键开始打印...",0);
				if(keyb()!=KEY_RETURN)
					break;
				strcpy(sLsh,sRecallLsh);
				if(nCxms==1001)
				{
					AirPrintReceipt(sLsh);
					break;
				}  //机票补打	
				
				if(ntype!=2) 
					PayPrint(2);
				else 
					PrintLtddfp(sLsh);
				
				switch(nCxms)
				{
					case 130:
					case 821:
						//移动新增语音收费补打[德阳]
						if(RunSql("update dl$zwmx set clxbz=substr(clxbz,1,1)"
									"||'1'||substr(clxbz,3,19) where clsh=%s",sLsh)>0)
							CommitWork();
						break;
					default: break;
				}
			}//end else
		}//end for
	}//end for
}
