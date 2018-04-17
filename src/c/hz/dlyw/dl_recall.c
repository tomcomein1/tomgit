/*==========================================================
           ��׼���ѳ��������ݲ���ģ��(dl_recall.c)
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
		"|  |�û���ʶ                |���׿��  |��������|����ʱ��|",
		"%-1.1s%-22.22s%10.2f%08d%06d%-6.6s%s");
    
	MergeTable(pdt->pTable,CreateTable(1,
	"|                                                            |"));
    
    
	strcpy(sJym,cjym); 
	RunSelect("select cjymc,ncxms,csjydz from dl$jym where cjym=%s into %s%ld%s",
				sJym,sJymc,&nCxms,sRemoteHost);
	clrscr();
	if(!ntype)
		outtext("                     ����%s\n",sJymc);
	else if(ntype==1)    outtext("                     ����%s����\n",sJymc);
	else if(ntype==2)    outtext("                     ����%s����\n",sJymc);
	outtext("                -------------------------------------\n");
    
	ShowDataTable(pdt,8,6);
    
	for(;;)
	{
		memset(sYhbz,'\0',sizeof(sYhbz));
		while(pdt->nCount) 
			DeleteDataNode(pdt,pdt->nCount);
		if(!ntype)
			ShowDatatableInfo(pdt,"������Ҫ�������ѵ��û���ʶ����绰�����...",0);
		else       
			ShowDatatableInfo(pdt,"�������ѽ��ѵ��û���ʶ����绰�����...",0);
		
		nRetVal=GetValue(10,4,"�������û���ʶ:%-22.22s",sYhbz);
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
				if(GetValue(10,5,"������֧�ֳ�����:%7s",sCzy)!=KEY_RETURN) continue;
				if(GetValue(34,5,"������֧�ֳ�����:%8p",sPass)!=KEY_RETURN) continue;
				upper(sCzy);upper(sPass);alltrim(sCzy);alltrim(sPass);
				if(RunSelect("select ROWNUM from dl$czyb where (nczyqx=2 or nczyqx=6) and cczydh=%s and cpassword=%s",sCzy,sPass)<=0)
				{
					clearline(5);gotoxy(10,5);
					outtext("����Ȩ���г���!");
					continue;
				}
			}//������������Ȩ����
		
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
			ShowDatatableInfo(pdt,"�����ڸ��û��ĽɷѼ�¼��ñʽ����Ѿ�����...",1);
			continue;
		}      
          
		nCurrent=1;
          
		for(;;)
		{
			if(!pdt->nCount) break;
			ImportData(pdt,nCurrent,"%s","+");
			ShowDatatableInfo(pdt,"ʹ�����¹���ѡ���¼����'1'ȷ��...",0);
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
          	
			if(!ntype)  //����
			{
				ShowDatatableInfo(pdt,"���Ҫ������(y/n)",0);
				nRetVal=keyb();
				if(nRetVal!='y' && nRetVal!='Y') 
					continue;
                        
				GenerateLsh(1,sSysJsdh,sJym,sLsh);

				GetSysLkrq();
				sprintf(sbuf,"%s|%s|%s|%ld|",sLsh,
								sRecallLsh,sSysCzydh,nSysSfm);
                         
				ShowDatatableInfo(pdt,"���ڳ����ɷѣ����Ժ�...",0);
				
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
				
				
				ShowDatatableInfo(pdt,"�������ѳɹ�...",1);
				DeleteDataNode(pdt,nCurrent);
				nCurrent=1;
				continue;
			}
			else
			{
				ShowDatatableInfo(pdt,"��׼���ô�ӡ�������س�����ʼ��ӡ...",0);
				if(keyb()!=KEY_RETURN)
					break;
				strcpy(sLsh,sRecallLsh);
				if(nCxms==1001)
				{
					AirPrintReceipt(sLsh);
					break;
				}  //��Ʊ����	
				
				if(ntype!=2) 
					PayPrint(2);
				else 
					PrintLtddfp(sLsh);
				
				switch(nCxms)
				{
					case 130:
					case 821:
						//�ƶ����������շѲ���[����]
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
