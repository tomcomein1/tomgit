/*=======================================================================
            ����ҵ��ϵͳ�����ѯģ�飨dl_zwcx.c��
original programmer:chenbo
created date :20010820
*=======================================================================*/

#include"dlpublic.h"
#include"newtask.h"
#include"tasktool.h"
#include"tools.h"
#include"query.h"
#include"dl_zwcx.h"

MENU * pQueryMenu=NULL;
static char sQueryJym[16]=".";
static char sQueryJsdh[10]=".";
static char sQueryCzydh[7]=".";
static char sEndJsdh[10]=".";
static long nStartLkrq,nEndLkrq;
extern char sSysJsdh[10];
extern char sSysCzydh[7];
extern char sSysJh[10];
extern char sSysJm[21];
extern char sTcpipErrInfo[81];
extern long nSysCzyqx;
extern long nSysLkrq,nSysSfm;

void QueryAndStatistic()
{
     char sTempCzydh[7];
     long nTempCzyqx;
     GetSysLkrq();
     nStartLkrq=nSysLkrq;
     nEndLkrq=nSysLkrq;

/*     if(nSysCzyqx==2&&(!strncmp(sSysJsdh+4,"01",2)))//���ľִ����鳤
     {
     	//�����ѯ
     	strcpy(sTempCzydh,sSysCzydh);
     	while(1)
     	{
		if(GetValue(10,2,"��������Ҫ��ѯ�Ĳ���Ա:%7s",sSysCzydh)==KEY_ESC)
		{
			strcpy(sSysCzydh,sTempCzydh);
		 	return;
	     	}
	     	alltrim(sSysCzydh);
	     	upper(sSysCzydh);
		if(RunSelect("select * from dl$czyb where cczydh=%s and nczyqx not in(0,1)",sSysCzydh)<=0)
		{
			outtext("\n         �޴˲���Ա��Ϣ���޲�ѯ����");
			keyb();
			clearline(3);
			continue;
		}
		break;
	}
	nTempCzyqx=nSysCzyqx;
	RunSelect("select nczyqx from dl$czyb where cczydh=%s into %ld",sSysCzydh,&nSysCzyqx);
     }//end if
*/ 
     if(pQueryMenu==NULL) pQueryMenu=CreateQueryMenu();
     clrscr();
     outtext("                            �����ѯ��ͳ��");
     outtext(
     "\n-------------------------------------------------------------------------------");
     gotoxy(1,SCREEN_LINE-2);
     outtext(
     "-------------------------------------------------------------------------------");
     outtext(
     "\n ��ʹ�ù���ѡ��...");
     PopMenu(pQueryMenu,2,3,18);
     
     //strcpy(sSysCzydh,sTempCzydh);
     //nSysCzyqx=nTempCzyqx;       //����ԭ
}

static MENU * CreateQueryMenu()
{
     RECORD rec;
     MENU * pmn,* pmxj;
     MENUITEM * pmxjitem,* pmzjitem;
     char sJym[16],sJymc[51];
     long nId,nLoop;

     if(!(pmn=CreateMenu("QUERY",0))) return NULL;

     AddMenuEvent(pmn,MENU_EVENT_FOCUSED,RecordQueryJym);
     AddMenuEvent(pmn,MENU_EVENT_SELECTED,RecordQueryJym);
     AddMenu(pmn,"���н���",".");

     nId=OpenCursor("select cjym,cjymc from dl$jym");
     if(nId<0)
     {
     	HiError(nId,"���ݿ����!");
     	return NULL;
     }

     for(;;)
     {
     	if(FetchCursor(nId,"%s%s",sJym,sJymc)<=0) break;
        AddMenu(pmn,sJymc,sJym);
     }
     CloseCursor(nId);
     

     if(AutoRunTask("dldj",TASK_GET_ALL_JSDH,"%s","%r",sSysJsdh,&rec))
     {
     	outtext("\nȡ�þֺ����ݳ���:%s",sTcpipErrInfo);
     	DropMenu(pmn);
     	keyb();
     	return NULL;
     }

     if(nSysCzyqx==2&&(!strncmp(sSysJsdh+4,"01",2)))//���ľִ����鳤
     {
        pmxj=CreateXjMenu(rec);
        if(!pmxj) return NULL;
        pmxjitem=pmn->pPopMenu;
        while(pmxjitem)
        {
             pmxjitem->pSubMenu=pmxj;
             pmzjitem=pmxj->pPopMenu;
             nLoop=6;
             while(nLoop--) pmzjitem=pmzjitem->pNext;
             while(pmzjitem)
             {
             	 pmzjitem->pSubMenu=CreateZjMenu(pmzjitem->sAddition,rec);
             	 pmzjitem=pmzjitem->pNext;
             }
             pmxjitem=pmxjitem->pNext;
        }
        DropRecord(&rec);
     }
     else
     {
        pmzjitem=pmn->pPopMenu;
        while(pmzjitem)
        {
            pmzjitem->pSubMenu=CreateZjMenu(sSysJsdh,rec);
            pmzjitem=pmzjitem->pNext;
        }
     }
     return pmn;
}

static MENU * CreateXjMenu(RECORD rec)
{
     MENU * pmn;
     RECORD jhrec;
     char sJsdh[10],sJm[31];
     long nLb;

     jhrec=rec;
     if(!(pmn=CreateMenu("XJ",0))) return NULL;


     AddMenuEvent(pmn,MENU_EVENT_FOCUSED,RecordQueryJsdh);
     AddMenuEvent(pmn,MENU_EVENT_SELECTED,RecordQueryJsdh);

     BindMenu(pmn,"ȫ�����ϼ�",sSysJh,QueryAllZwmx);
     BindMenu(pmn,"���ؾֱ���",sSysJh,QueryXjZwmxForm);
     BindMenu(pmn,"��֧�ֱ���",sSysJh,QueryZwmxForm);
     BindMenu(pmn,"ȫ������ϸ",sSysJh,QueryZwmxDetail);
     BindMenu(pmn,"�ɿ��ѯ",sSysJh,DlywJkdCx);
     BindMenu(pmn,"[���ò�ѯʱ��]",".",SetQueryDate);
     for(;;)
     {
     	ExportRecord(&jhrec,"%s%s%ld",sJsdh,sJm,&nLb);
     	if(nLb==0) break;
     	if(nLb!=3) continue;
     	alltrim(sJsdh);
        AddMenu(pmn,sJm,sJsdh);
     }
     return pmn;

}


static MENU * CreateZjMenu(char * sXjJsdh,RECORD rec)
{
     MENU * pmn;
     RECORD jhrec;
     char sJsdh[10],sJm[31];
     long nLb;

     jhrec=rec;
     if(!(pmn=CreateMenu("XJ",0))) return NULL;

     AddMenuEvent(pmn,MENU_EVENT_FOCUSED,RecordQueryJsdh);
     AddMenuEvent(pmn,MENU_EVENT_SELECTED,RecordQueryJsdh);

     if(nSysCzyqx==2||nSysCzyqx==4||nSysCzyqx==5)//�����鳤����ѯͳ�ƣ�����
     {
          BindMenu(pmn,"ȫ�غϼ�",sXjJsdh,QueryAllZwmx);
          BindMenu(pmn,"��֧�ֱ���",sXjJsdh,QueryZwmxForm);
          BindMenu(pmn,"ȫ����ϸ",sXjJsdh,QueryZwmxDetail);
          BindMenu(pmn,"�ɿ��ѯ",sXjJsdh,DlywJkdCx);
          BindMenu(pmn,"[���ò�ѯʱ��]",".",SetQueryDate);
    
          for(;;)
          {
          	ExportRecord(&jhrec,"%s%s%ld",sJsdh,sJm,&nLb);
          	if(nLb==0) break;
          	if(nLb!=4 || strncmp(sJsdh,sXjJsdh,6)) continue;
          	alltrim(sJsdh);
                BindMenu(pmn,sJm,sJsdh,QueryZwmxDetail);
          }
     }
     else if(nSysCzyqx==6)//֧�ֳ�
     {
          BindMenu(pmn,"��֧�ֺϼ�",sSysJsdh,QueryAllZwmx);
          BindMenu(pmn,"��֧����ϸ",sSysJsdh,QueryZwmxDetail);
          BindMenu(pmn,"�ɿ��ѯ",sSysJsdh,DlywJkdCx);
          BindMenu(pmn,"[���ò�ѯʱ��]",".",SetQueryDate);
     }
     else if(nSysCzyqx==3)//ҵ�����Ա
     {
          strcpy(sQueryCzydh,sSysCzydh);
          BindMenu(pmn,"������Ա�ϼ�",sSysJsdh,QueryAllZwmx);
          BindMenu(pmn,"������Ա��ϸ",sSysJsdh,QueryZwmxDetail);
          BindMenu(pmn,"�ɿ��ѯ",sSysJsdh,DlywJkdCx);
          BindMenu(pmn,"[���ò�ѯʱ��]",".",SetQueryDate);
     }
     return pmn;
}

static void RecordQueryJym(MENU * pmn,long nId)
{
    strcpy(sQueryJym,GetAddition(pmn,nId));
}

static void RecordQueryJsdh(MENU * pmn,long nId)
{
    strcpy(sQueryJsdh,GetAddition(pmn,nId));
    strcpy(sEndJsdh,sQueryJsdh);
    if(!strncmp(sQueryJsdh+6,"000",3)) sprintf(sEndJsdh,"%-6.6s999",sQueryJsdh);
    if(!strncmp(sQueryJsdh+4,"00",2)) sprintf(sEndJsdh,"%-4.4s99999",sQueryJsdh);
    //if(!strncmp(sQueryJsdh+2,"00",2)) sprintf(sEndJsdh,"%-2.2s9999999",sQueryJsdh);  //��Ʊʡ��

}

static void QueryAllZwmx()
{

    QUERYTABLE * pqt;
    long nCount=0,nId,nRetVal,nZs=0;
    double dSjk=0.0,dJsfl=0.0,dSr=0.0,dZsr=0.0,dZk=0.0;
    char sJym[16]=".",sJymc[51]=".",cBl2[6]=".";
    char sWhere[41];
    char sTitle[81];
    
    sprintf(sTitle,"                 ����ϼƲ�ѯ (��ѯʱ��:%ld��%ld)",nStartLkrq,nEndLkrq);

    pqt=CreateQueryTable(sTitle,
                         "  ҵ������                                ���ױ���     �ܽ��׿��(Ԫ)  ����    ����        ��������",
                         "%-40.40s %8ld %12.2lf %-6.3lf %10.2lf %-2.2s",ST_LASTLINE|ST_VISUAL);
    if(!pqt) return;

    clrscr();
    strcpy(sWhere," ");

    if(sQueryJym[0]!='.')
      sprintf(sWhere,"and cjym='%s' ",sQueryJym);
    if(sQueryCzydh[0]!='.')
      sprintf(sWhere+strlen(sWhere)," and cczydh='%s'",sQueryCzydh);

    RunSelect("select sum(nsl),sum(dsjk) from dl$zwmx "
    		  "where nlkrq between %ld and %ld "
    		  "and cjh between %s and %s %t into %ld%lf",
    	      nStartLkrq,nEndLkrq,sQueryJsdh,sEndJsdh,sWhere,&nCount,&dSjk);

    nId=OpenCursor("select sum(nsl),sum(dsjk),cjym,cbl2 from dl$zwmx "
    		  "where nlkrq between %ld and %ld "
    		  "and cjh between %s and %s %t group by cjym,cbl2",
     	      nStartLkrq,nEndLkrq,sQueryJsdh,sEndJsdh,sWhere);

    if(sQueryJym[0]!='.')
    {	
	    RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=33 and \
    		cjym=%s into %lf",sQueryJym,&dJsfl);

      dSr=dSjk*dJsfl;
      ImportLastLine(pqt,".",nCount,dSjk,dJsfl,dSr,".");
    }
    else {nZs=nCount;dZk=dSjk;}

    if(nId<0)
    {
    	HiError(nId,"��ѯ����");
        DropQueryTable(pqt);
        return;
    }

    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%ld%lf%s%s",&nCount,&dSjk,sJym,cBl2);
        strcpy(sJymc,sJym);
        GetJymcByJym(sJym,sJymc);
	
	if(sQueryJym[0]=='.')
	{
	   RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=33 and \
   					cjym=%s into %lf",sJym,&dJsfl);
           dSr=dSjk*dJsfl;
           dZsr+=dSr;
        }
        
        if(ImportQueryTable(pqt,nRetVal,sJymc,nCount,dSjk,dJsfl,dSr,cBl2)<=0) break;
	if(sQueryJym[0]=='.')  ImportLastLine(pqt,".",nZs,dZk,0.0,dZsr,".");
    }
    CloseCursor(nId);
    DropQueryTable(pqt);

}

static void QueryXjZwmxForm()
{
    QUERYTABLE * pqt;
    long nCount=0,nId,nRetVal;
    double dSjk=0.0,dJsfl=0.0,dSr=0.0;
    char sJym[16]=".",sOldJym[16]=".",sJymc[51]=".",
         sJsdh[16],sOldJsdh[16]=".",sJm[51];
    char sWhere[41];

    char sTitle[81];

    sprintf(sTitle,"               ���ؾ����񱨱��ѯ (��ѯʱ��:%ld��%ld)",nStartLkrq,nEndLkrq);

    pqt=CreateQueryTable(sTitle,
                         "  ����                ҵ������                    ���ױ���     �ܽ��׿��(Ԫ)  ����    ����      ",
                         "%-24.24s %-40.40s %8ld %12.2lf %-6.3lf %10.2lf",ST_LASTLINE|ST_VISUAL);
    if(!pqt) return;

    clrscr();
    strcpy(sWhere," ");

    if(sQueryJym[0]!='.')
      sprintf(sWhere,"and cjym='%s' ",sQueryJym);

    RunSelect("select sum(nsl),sum(dsjk) from dl$zwmx "
    		  "where nlkrq between %ld and %ld "
    		  "and cjh between %s and %s %t into %ld%lf",
    	      nStartLkrq,nEndLkrq,sQueryJsdh,sEndJsdh,sWhere,&nCount,&dSjk);

    nId=OpenCursor("select sum(nsl),sum(dsjk),cjym,substr(cjh,1,6) from dl$zwmx "
                   "where nlkrq between %ld and %ld and cjh between %s and %s %t "
                   "group by substr(cjh,1,6),cjym",
     	           nStartLkrq,nEndLkrq,sQueryJsdh,sEndJsdh,sWhere);

    if(sQueryJym[0]!='.')
    	RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=33 and \
    		cjym=%s into %lf",sQueryJym,&dJsfl);
    		
    dSr=dSjk*dJsfl;
    ImportLastLine(pqt,".",".",nCount,dSjk,dJsfl,dSr);

    if(nId<0)
    {
    	HiError(nId,"��ѯ����");
        DropQueryTable(pqt);
        return;
    }

    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%ld%lf%s%s",&nCount,&dSjk,sJym,sJsdh);

    	sJsdh[6]='\0';
    	strcat(sJsdh,"000");

    	sJm[0]='\0';

        if(strcmp(sOldJym,sJym)) 
        {
             strcpy(sJymc,sJym);
             GetJymcByJym(sJym,sJymc);
             strcpy(sOldJym,sJym);
        }
        
        if(strcmp(sOldJsdh,sJsdh))
        {
             strcpy(sJm,sJsdh);
             GetJsmcByJsdh(sJsdh,sJm);
             strcpy(sOldJsdh,sJsdh);
        }     
    	
		if(sQueryJym[0]=='.')
			RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=33 and \
    				cjym=%s into %lf",sJym,&dJsfl);
    		
    	dSr=dSjk*dJsfl;
        if(ImportQueryTable(pqt,nRetVal,sJm,sJymc,nCount,dSjk,dJsfl,dSr)<=0) break;
    }
    CloseCursor(nId);
    DropQueryTable(pqt);

}

static void QueryZwmxForm()
{
    QUERYTABLE * pqt;
    long nCount=0,nId,nRetVal;
    double dSjk=0.0,dJsfl=0.0,dSr=0.0;
    char sJym[16]=".",sOldJym[16]=".",sJymc[51]=".",
         sJsdh[16],sOldJsdh[16]=".",sJm[51];
    char sWhere[41];

    char sTitle[81];

    sprintf(sTitle,"               ��֧�����񱨱��ѯ (��ѯʱ��:%ld��%ld)",nStartLkrq,nEndLkrq);

    pqt=CreateQueryTable(sTitle,
                         "  ����                    ҵ������                  �ܱ���     �ܽ��׿��(Ԫ)  ����    ����      ",
                         "%-22.22s %-26.26s %6ld %12.2lf %-6.3lf %10.2lf",ST_LASTLINE|ST_VISUAL);
    if(!pqt) return;

    clrscr();
    strcpy(sWhere," ");

    if(sQueryJym[0]!='.')
      sprintf(sWhere,"and cjym='%s' ",sQueryJym);

    RunSelect("select sum(nsl),sum(dsjk) from dl$zwmx "
    	      "where nlkrq between %ld and %ld "
    	      "and cjh between %s and %s %t into %ld%lf",
    	      nStartLkrq,nEndLkrq,sQueryJsdh,sEndJsdh,sWhere,&nCount,&dSjk);

    nId=OpenCursor("select sum(nsl),sum(dsjk),cjym,cjh from dl$zwmx "
    			   "where nlkrq between %ld "
    			   "and %ld and cjh between %s and %s %t group by cjh,cjym",
     	           nStartLkrq,nEndLkrq,sQueryJsdh,sEndJsdh,sWhere);

    if(sQueryJym[0]!='.')
    	RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=33 and \
    		cjym=%s into %lf",sQueryJym,&dJsfl);
    		
    dSr=dSjk*dJsfl;
    ImportLastLine(pqt,".",".",nCount,dSjk,dJsfl,dSr);

    if(nId<0)
    {
    	HiError(nId,"��ѯ����");
        DropQueryTable(pqt);
        return;
    }

    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%ld%lf%s%s",&nCount,&dSjk,sJym,sJsdh);
    	sJm[0]='\0';
    	
        if(strcmp(sOldJym,sJym)) 
        {
             strcpy(sJymc,sJym);
             GetJymcByJym(sJym,sJymc);
             strcpy(sOldJym,sJym);
        }
        
        if(strcmp(sOldJsdh,sJsdh))
        {
             strcpy(sJm,sJsdh);
             GetJsmcByJsdh(sJsdh,sJm);
             strcpy(sOldJsdh,sJsdh);
        }     
        
        if(sQueryJym[0]=='.')
        	RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=33 and \
    					cjym=%s into %lf",sJym,&dJsfl);
   
        dSr=dSjk*dJsfl;
        if(ImportQueryTable(pqt,nRetVal,sJm,sJymc,nCount,dSjk,dJsfl,dSr)<=0) break;
    }
    CloseCursor(nId);
    DropQueryTable(pqt);
}
static void QueryZwmxDetail()
{

    QUERYTABLE * pqt;
    long nCount=0,nId,nRetVal,nLkrq,nSfm,nXh,nXzbz=2;
    double dSjk=0.0;
    char sCzydh[7],sYhbz[21],sLxbz[21],sYhbz1[21];
    char sWhere[100],sOther[20],sYhmc[51],sJsdh[10],sJsmc[51];
    char sTitle[81];
    long nZsl=0;
    double dZk=0;
    
    sprintf(sTitle,"                ������ϸ��ѯ (��ѯʱ��:%ld��%ld)",nStartLkrq,nEndLkrq);

    pqt=CreateQueryTable(sTitle,
        "  ��ˮ��    �û���ʶ              ����    �����ܿ��  ����Ա  ����                 �û�����        ��ͬ��          ��������  ʱ��    ��ע  ",
        "%-8ld %-22.22s %4ld %10.2lf %-6.6s %-20.20s %-20.20s %-16.16s %8ld %06ld %-6.6s",
        ST_KEYWORD|ST_VISUAL|ST_LASTLINE);

    if(!pqt) return;

    clrscr();
    strcpy(sWhere," ");

    if(sQueryJym[0]!='.')
      sprintf(sWhere,"and cjym='%s' ",sQueryJym);
    if(sQueryCzydh[0]!='.')
      sprintf(sWhere+strlen(sWhere)," and cczydh='%s'",sQueryCzydh);

    //������ϸ����
    RunSelect("select sum(nsl),sum(dsjk) from dl$zwmx "
                   "where nlkrq between %ld and %ld and "
                   "cjh between %s and %s %t into %ld%lf",
                   nStartLkrq,nEndLkrq,sQueryJsdh,sEndJsdh,sWhere,&nZsl,&dZk);

    nId=OpenCursor("select nxh,cyhbz,cyhbz1,cyhmc,nsl,dsjk,cczydh,cjh,nlkrq,nsfm,clxbz,nxzbz from dl$zwmx "
                   "where  nlkrq between %ld and %ld and cjh between %s and %s %t order by nxh",
     	           nStartLkrq,nEndLkrq,sQueryJsdh,sEndJsdh,sWhere);

    if(nId<0)
    {
    	HiError(nId,"��ѯ����");
        DropQueryTable(pqt);
        return;
    }
  
    for(; ;)
    {
    	nRetVal=FetchCursor(nId,"%ld%s%s%s%ld%lf%s%s%ld%ld%s%ld",
    	        &nXh,sYhbz,sYhbz1,sYhmc,&nCount,&dSjk,sCzydh,sJsdh,&nLkrq,&nSfm,sLxbz,&nXzbz);

        if(sLxbz[18]=='0')
        {
           strcpy(sOther,"����");
           if(nXzbz==0) strcpy(sOther,"��ȷ��");
        }   
        else strcpy(sOther,"�ѳ���");
        GetJsmcByJsdh(sJsdh,sJsmc);
 	
 	ImportLastLine(pqt,0,".",nZsl,dZk,".",".",".",".",0,0,".");//����
        if(ImportQueryTable(pqt,nRetVal,nXh,sYhbz,nCount,dSjk,sCzydh,sJsmc,sYhmc,sYhbz1,nLkrq,nSfm,sOther)<=0) break;

    }
    CloseCursor(nId);
    DropQueryTable(pqt);
}

static void SetQueryDate()
{
     clearline(20);
     if(GetValue(2,20,"�������ѯʱ�Σ�%8d",&nStartLkrq)==KEY_ESC) return;
     if(GetValue(26,20," �� %8d",&nEndLkrq)==KEY_ESC) return;
     clearline(20);

}

static void DlywJkdCx()
{
     QUERYTABLE * pqt;

     char sJkdh[16],sCzydh[7],sWhere[100],sLxbz[21],sJsdh[10],sJsmc[51]=".";
     long nStartDate,nId,nRetVal,nCount;
     double dKe;

     clrscr();
     strcpy(sWhere," ");
     if(sQueryJym[0]!='.')
        sprintf(sWhere,"and cjym='%s' ",sQueryJym);
     if(sQueryCzydh[0]!='.')
        sprintf(sWhere+strlen(sWhere)," and cczydh='%s'",sQueryCzydh);

     nId=OpenCursor("select cjkdh,sum(dsjk),cczydh,min(nlkrq),"
                    "min(clxbz),count(*),min(cjh) from dl$zwmx "
                    "where nlkrq between %ld and "
                    "%ld and cjh between %s and %s %t group by cczydh,cjkdh", 
                    nStartLkrq,nEndLkrq,sQueryJsdh,sEndJsdh,sWhere);

     if(nId<0)
     {
     	HiError(nId,"��ѯ�������");
     	return;
     }
     pqt=CreateQueryTable(
     "                            ��  ��  ��  ��  ѯ",
     "  �ɿ��           ����  �ܽ��(Ԫ)  ����Ա  ����                  ��ʼ����",
     "%-15.15s %4ld %10.2lf %-6.6s  %-20.20s %-8ld",
     ST_KEYWORD|ST_VISUAL);

     nStartDate=0;

     for(;;)
     {
     	nRetVal=FetchCursor(nId,"%s%lf%s%ld%s%ld%s",
     	sJkdh,&dKe,sCzydh,&nStartDate,sLxbz,&nCount,sJsdh);
        if(sLxbz[15]=='0') strcpy(sJkdh,"δ�ɿ�");
        GetJsmcByJsdh(sJsdh,sJsmc);
        if(ImportQueryTable(pqt,nRetVal,sJkdh,nCount,dKe,sCzydh,sJsmc,nStartDate)<=0) break;
     }

     CloseCursor(nId);
     DropQueryTable(pqt);
     return;

}

static long GetJsmcByJsdh(char * sJsdh,char * sJsmc)
{
   
   MENU * pXjMenu,* pZjMenu;
   MENUITEM * pItem;
   long nIndex,nRetVal=1;
   char sXjjh[9],sXjmc[21],sZjmc[21];
   
   strcpy(sJsmc,sJsdh);
   memset(sXjmc,0,sizeof(sXjmc));
   memset(sZjmc,0,sizeof(sZjmc));
   sXjmc[0]='.';
   sZjmc[0]='.';
   
   if(strncmp(sJsdh+4,"00000",5) == 0)
   {
      strcpy(sZjmc,sSysJm);
      return 1;
   }

   sprintf(sXjjh,"%-6.6s000",sJsdh);
   if(pQueryMenu)
   {
      pXjMenu = pQueryMenu->pPopMenu->pSubMenu;
      nIndex = IsInMenu(pXjMenu,sXjjh,0);
      if(nIndex>0)
      {
      	   strncpy(sXjmc,GetMenu(pXjMenu,nIndex),20);
      	   alltrim(sXjmc);
      	   if(strncmp(sJsdh+6,"000",3))
      	   {
      	   	pItem = pXjMenu->pPopMenu;
      	   	while(--nIndex) pItem=pItem->pNext;
      	   	pZjMenu = pItem->pSubMenu;
      	   	nIndex = IsInMenu(pZjMenu,sJsdh,0);
      	   	if(nIndex>0)
      	   	{
       	            strncpy(sZjmc,GetMenu(pZjMenu,nIndex),20);
      	            alltrim(sZjmc);
      	        }
      	   }
      } 	            
   }
   
   if(sXjmc[0]=='.')
       nRetVal=RunSelect("select cjm from dl$jh where cjh=%s and nlb=3 into %s",sXjjh,sXjmc);
   
   if(sZjmc[0]=='.' && strncmp(sJsdh+6,"000",3))
       nRetVal=RunSelect("select cjm from dl$jh where cjh=%s and nlb=4 into %s",sJsdh,sZjmc);

   
   if(strncmp(sJsdh+6,"000",3)) sprintf(sJsmc,"%s%s",sXjmc,sZjmc);
   else strcpy(sJsmc,sXjmc);

   return nRetVal;

}

static long GetJymcByJym(char * sJym,char * sJymc)
{
   long nIndex;  

   strcpy(sJymc,sJym);
   if(pQueryMenu)
   {
      nIndex=IsInMenu(pQueryMenu,sJym,0);
      if(nIndex>0)
      {
      	   strncpy(sJymc,GetMenu(pQueryMenu,nIndex),50);
      	   alltrim(sJymc);
      	   return nIndex;
      }
   }   
   else return RunSelect("select cjymc from dl$jym where cjym=%s into %s",sJym,sJymc);
}
