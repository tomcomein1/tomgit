//void DaiBanYw(){}
void SpecialOperation(){}
#include "dlpublic.h"
#include "tools.h"
#include "malloc.h"
#include "query.h"
#include "des.h"
#include "zhw_exec.h"


void qaction(char * msg);
void qaction(char * msg)
{
     savewin();
     MessageBox(msg,1);
     popwin();
 
}


void ToolsTest()
{

     char buffer[100];
     char sDest[16];
     int len=8;
     


     
     clrscr();
     fflush(stdout);
     //zhw_exec("DLZ","DDDD","../dlyw/dlyw");
     zhw_exec("main","DL001","DDDD","ABCD","61008001");
     keyb();
    

     
     

/*
     long nRetVal;
     char sErrInfo[100];
     
     
     
     
     
     nRetVal=RunSql("insert into dl$dlywcs(nbh,ccs) values(1,'abc')");
     
     clrscr();
     
     if(nRetVal<0)
     {
     	GetSqlErrInfo(sErrInfo);
     	outtext("\nval=%ld,\n%s",nRetVal,sErrInfo);
     }
     keyb(); 



     QUERYTABLE * pqt;
     long nId,nbh,nRetVal,nRec=1;
     char sJh[16],sJm[21];
     
     printf("\nint=%d,long=%d,uint=%d,short=%d",
     sizeof(int),sizeof(long),sizeof(unsigned int),sizeof(short int));keyb();
     pqt=CreateQueryTable("                            What's your name?",
                          "  ÐòºÅ                       ¾ÖÃû       ",
                          "%-4d                         %-9.9s ",
                          ST_KEYWORD|ST_VISUAL|ST_LASTLINE);
     
     clrscr();

     
     
     for(;;)
     {
     	nRec++;
     	sprintf(sJh,"jh%-ld",nRec);
     	if(ImportQueryTable(pqt,2000-nRec,nRec,sJh)<=0) break;
     	for(nRetVal=0;nRetVal<10;nRetVal++) for(nId=0;nId<32000;nId++);
     }
     DropQueryTable(pqt);
*/
}


