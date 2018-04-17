/*===========================================================
           ����ҵ��ϵͳ����Ա�ɿ�ģ��
           

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
    
     outtext("\n���س�����ʼ�ɿ������������...");
     if(keyb()!=KEY_RETURN) return;
     
     if(RunSelect("select * from dl$zwmx where cczydh=%s and substr(clxbz,16,1)='0'",sSysCzydh)<=0)
     {
     	
     	outtext("\nû��Ҫ�ɿ�����ݣ������������...");
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
     	outtext("\n�ɿ����%s",sTcpipErrInfo);
     	keyb();
     	return;
     }	                    
     CommitWork();
     outtext("\n�ɿ�ɹ����ɿ��Ϊ��%s.\n���������ʼ��ӡ�ɿ,ESC������...",sJkdh);
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
     "|ҵ������                                |��  ��|�ܽ��׽��|",
     "%-40.40s%6d%10.2f");
     MergeTable(pdt->pTable,CreateTable(1,
     "|�ֽ�|            |����|            |�ϼƽ��|          |"));
     

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
     output("\n                        ����Ա�ɿ\n\n");
     output("\n  ��  ��:%s     ��  ��:%s    ��������:%s",
            sSysCzydh,sSysCzymc,sSysJsmc);
     output("\n  �ɿ�ţ�%s    �ɿ�ʱ�Σ�%ld��%ld\n",sJkdh,nMinDate,nMaxDate);
     setprintoff();
     OutputTable(pdt->pTable);
     DropDataTable(pdt);
     setprinton();
     output("  �������ڣ�%ld        ǩ  ����\n",nSysLkrq);      
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
     if(GetValue(20,10,"������ɿ�����:%8d",&nLkrq)!=KEY_RETURN) return;
     sprintf(sJkdh,"%ld0000000",nLkrq);
     for(;;)
     {
        nRetVal=GetValue(20,12,"������ɿ��:%-15.15s",sJkdh);
        if(nRetVal==KEY_ESC) return;
        if(nRetVal!=KEY_RETURN) continue;
        if(RunSelect("select * from dl$zwmx where cjkdh=%s and cczydh=%s",
                    sJkdh,sSysCzydh)<=0)
        {
                savewin();
        	QueryLocal(
        	"  �ɿ��             �ܽ��׿��      ����Ա����    ",
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
     outtext("���������ʼ��ӡ�ɿ����ESC������...");
     if(keyb()==KEY_ESC) return;
     PrintCzyJkd(sJkdh);   	
}
