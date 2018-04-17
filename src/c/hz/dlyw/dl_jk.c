/*===========================================================
           代理业务系统操作员缴款模块
           

============================================================*/
#include "dlpublic.h"
#include "tools.h"
#include "newtask.h"

extern char sSysCzydh[7];
extern char sSysJsdh[10];
extern char sSysCzymc[9];
extern char sSysJsmc[JM_JM_LEN+1];
extern char sTcpipErrInfo[81];
extern long nSysLkrq;
static void PrintCzyJkd();

void ChargeConfirm()
{
     char sJkdh[128];
     long nRetVal;
     long nSequence=-1;
     
     clrscr();
    
     outtext("\n按回车键开始缴款，或按其它键放弃...");
     if(keyb()!=KEY_RETURN) return;
     
     if(RunSelect("select * from dl$zwmx where cczydh=%s and substr(clxbz,16,1)='0'",sSysCzydh)<=0)
     {
     	
     	outtext("\n没有要缴款的数据，按任意键返回...");
     	keyb();
     	return;
     }	

     GetSysLkrq();
     RunSelect("select dlnblsh.nextval from dual into %ld",&nSequence);
     nSequence%=10000000;
     sprintf(sJkdh,"%8ld%07ld",nSysLkrq,nSequence);

     nRetVal=RunSql("update dl$zwmx set clxbz=substr(clxbz,1,15)||'1'||substr(clxbz,17,4),"
                    "cjkdh=%s where cczydh=%s and substr(clxbz,16,1)='0'",
                    sJkdh,sSysCzydh);
     
     if(nRetVal<0)
     {
     	RollbackWork();
     	GetSqlErrInfo(sTcpipErrInfo);
     	outtext("\n缴款出错：%s",sTcpipErrInfo);
     	keyb();
     	return;
     }	                    
     CommitWork();
     outtext("\n缴款成功，缴款单号为：%s.\n按任意键开始打印缴款单,ESC键放弃...",sJkdh);
     if(keyb()==KEY_ESC) return;
   
     PrintCzyJkd(sJkdh);

}

static void PrintCzyJkd(char * sJkdh)
{
     DATATABLE * pdt;
     long nMaxDate,nMinDate,nRetVal,nId,nSl;
     double dSjk,dSjkxj;
     char sJym[16],sJymc[51];
     
     nRetVal=-1;
     RunSelect("select count(count(*)) from dl$zwmx where cjkdh=%s "
               "group by cjym into %ld", sJkdh,&nRetVal);

     pdt=CreateDataTable(nRetVal,
     "|业务名称                                |数  量|总交易金额|",
     "%-40.40s%6d%10.2f");
     MergeTable(pdt->pTable,CreateTable(1,
     "|现金|            |其它|            |合计金额|          |"));
     

     GetSysLkrq();
     RunSelect("select min(nlkrq),max(nlkrq),sum(dsjk),sum(dsjkxj) from dl$zwmx "
               "where cjkdh=%s into %ld%ld%lf%lf",
               sJkdh,&nMinDate,&nMaxDate,&dSjk,&dSjkxj);

     FillTable(pdt->pTable,pdt->nPage+2,"> %10.2f > %10.2f > %10.2f",
               dSjkxj,dSjk-dSjkxj,dSjk);
                               
     nId=OpenCursor("select cjym,sum(nsl),sum(dsjk) from dl$zwmx "
                    "where cjkdh=%s group by cjym",sJkdh);

     for(;;)
     {
     	if(FetchCursor(nId,"%s%ld%lf",sJym,&nSl,&dSjk)<=0) break;
     	RunSelect("select cjymc from dl$jym where cjym=%s into %s",sJym,sJymc);
     	ImportData(pdt,pdt->nCount+1,"%s %d %f",sJymc,nSl,dSjk);
     }	
     CloseCursor(nId);
     clrscr();
     setprinton();
     output("\n                        操作员缴款单\n\n");
     output("\n  工  号:%s     姓  名:%s    所属局名:%s",
            sSysCzydh,sSysCzymc,sSysJsmc);
     output("\n  缴款单号：%s    缴款时段：%ld至%ld\n",sJkdh,nMinDate,nMaxDate);
     setprintoff();
     OutputTable(pdt->pTable);
     DropDataTable(pdt);
     setprinton();
     output("  制作日期：%ld        签  名：\n",nSysLkrq);      
     setprintoff();
     keyb();
}

void ReprintCzyJkd()
{
     char sJkdh[16]="\0";
     long nRetVal,nLkrq;
     
     clrscr();
     
     GetSysLkrq();
     nLkrq=nSysLkrq;
     if(GetValue(20,10,"请输入缴款日期:%8d",&nLkrq)!=KEY_RETURN) return;
     sprintf(sJkdh,"%ld0000000",nLkrq);
     for(;;)
     {
        nRetVal=GetValue(20,12,"请输入缴款单号:%-15.15s",sJkdh);
        if(nRetVal==KEY_ESC) return;
        if(nRetVal!=KEY_RETURN) continue;
        if(RunSelect("select * from dl$zwmx where cjkdh=%s and cczydh=%s",
                    sJkdh,sSysCzydh)<=0)
        {
                savewin();
        	QueryLocal(
        	"  缴款单号             总交易款额      操作员代号    ",
        	"%s%lf%s",
        	"select cjkdh,sum(dsjk),min(cczydh) from dl$zwmx where substr(cjkdh,"
        	"1,8)='%ld' and substr(clxbz,16,1)='1' and cczydh=%s group by cjkdh ",
        	nLkrq,sSysCzydh);
        	popwin();
        	continue;
        }
        break;
     }   	
     gotoxy(20,20);
     outtext("按任意键开始打印缴款单，或按ESC键放弃...");
     if(keyb()==KEY_ESC) return;
     PrintCzyJkd(sJkdh);   	
}
