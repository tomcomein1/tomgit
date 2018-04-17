/***********************************************************************
**filename	�ļ���:dl_buy.c
**target	Ŀ��:����ϵͳ�л��������ӻ�
**create time	��������: 2001.3.19
**original programmer	����Ա:�²�[chenbo]
**edit  history	�޸���ʷ:
**date     editer    reson
** 
************************************************************************/
#include "dlpublic.h"
#include "dl_buy.h"
#include "tasktool.h"

extern char sSysJh[10];
extern char sSysJsdh[10];
extern char sSysCzydh[7];
extern long nSysLkrq;
extern long nSysSfm;
extern char sTcpipErrInfo[81];
char sSysSyz[21];

void PurchaseGoods()
{
    DATATABLE * pdt;//¼��������ݱ�����
    MENU * pmsp,* pmgs,* pmjbr;//��Ʒ���ƣ���˾���ƣ������˰����˵�
    char sOpt[3],sJym[16],sJymc[51],sIdent[21],sTransActor[7];
    char sJymTemp[16],sIdentTemp[21];
    long nCount,nCurrent,nId,nKey,npx,npy,identify;
    long nwhere,nModifyFlag;
    double dPrice;
    

    // initialize
    GetSysLkrq();

    sprintf(sSysSyz,"%s.%s",sSysJsdh,sSysCzydh);
    clrscr();
     
    pdt=CreateDataTable(8,
    "|  |��Ʒ����                  |��������|��Ʒ��ʶ        |����  |������|",
		"%-1.1s%-26.26s%8.2f%-16.16s%-6d%-6.6s%15s%d");
    if(!pdt)
    {
	MessageBox("�������ݱ�����...",1);
	return;
    }

    MergeTable(pdt->pTable,CreateTable(1,
    "|                                                                          |"));

    
    gotoxy(1,1);
    outtext("                          �����������(��  ��)");
    ShowDataTable(pdt,1,3);
    ShowDatatableInfo(pdt,"������ȡ������Ա�Ľ�����Ϣ�����Ժ�...  ",0);
    pmsp=CreateGoodsMenu();         //��Ʒ���ư����˵�
    pmjbr=CreateTransActorMenu();   //�����˰����˵�

    nKey=pmsp&&pmgs&&pmjbr;
    if(LoadPurchaseData(pdt))
    {
	 DropMenu(pmjbr);
	 DropMenu(pmsp);
	 DropDataTable(pdt);
	 GetSysLkrq();
	 return;
    }
	
    //begin input
    nCurrent=1;//start with the first line
    nwhere=1;
    nModifyFlag=0;//the document is modified if the flag is nozero
    sJymc[0]='.';
    for(;;)
    {
	//auto append a new record
	if(nCurrent>pdt->nCount)
        {
            if(pdt->nCount) ExportData(pdt,pdt->nCount,"> %s",sJymc);
	    if(!strchr(sJymc,'.')||(!pdt->nCount))
		ImportData(pdt,nCurrent,"%s%s%f%s%d%s%s%d","+",".",0.0,".",0,".",".",0);
            nwhere=1;
            nCurrent=pdt->nCount;
	}
        
        switch(nwhere)
        {
            case 1:
            ShowDatatableInfo(pdt,
            "A:����  -:ɾ��  S:����  H:��ǰ  E:���  U:�Ϸ�  D:�·�  I:����  ESC:����",0);
            break;
        }
        
         

	//Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"%-1.1s%-26.26s%8.2f%-16.16s%-6d%-6.6s");
        //Export data from container which you inputed
	ExportData(pdt,nCurrent,"%s%s%f%s%d%s%s%d",
	               sOpt,sJymc,&dPrice,sIdent,&nCount,sTransActor,sJym,&identify);
	ImportData(pdt,nCurrent,"%s","+");

        switch(nKey)
	{
  		   case KEY_RETURN:
		 	    break;
		   case KEY_LEFT:
			    if(nwhere>1) nwhere--;
                            if(!identify&&nwhere==4) nwhere=3;
				continue;
		   case KEY_UP:
			    if(nwhere!=1) continue;
			    if(nCurrent>1) nCurrent--;
				continue;
		   case KEY_DOWN:
			    if(nwhere!=1) continue;
			    nCurrent++;
				continue;
		   case KEY_ESC:
			    if(nwhere!=1)
			    {
					nwhere=1;
					continue;
			    }
		    	    if(nModifyFlag)
			    {
				nKey=MessageBox("���Ľ�����¼û�б��棬Ҫ������",0);
				if(nKey==KEY_ESC)
				{
                                        ShowDataTable(pdt,1,3);
				        continue;
				}
				if(nKey==KEY_YES) SavePurchaseData(pdt);
			    }
			    DropMenu(pmjbr);
			    DropMenu(pmsp);
			    DropDataTable(pdt);
	                    GetSysLkrq();
			    return;
		   default:continue;
		}

		switch(nwhere)
		{
		   case 1: //��������
			    switch(sOpt[0])
			    {
				     case 'a':   //append record
				     case 'A':
						 nCurrent=pdt->nCount+1;
						 continue;
                                     case 's':   //save as temporary record
				     case 'S':
						 if(nModifyFlag)
						 {
							 nModifyFlag=SavePurchaseData(pdt);
							 keyb();
							 ShowDataTable(pdt,1,3);
						 }
						 continue;
				     
				     case '-':   //delete record
						 DeleteDataNode(pdt,nCurrent);
  						 nModifyFlag=1;
						 RefreshDataTable(pdt);
						 continue;
                                     
                                     case 'i':   //save as official record
                                     case 'I':
                                                 if(!pdt->nCount) continue;
                                                 nModifyFlag=CommitPurchaseData(pdt);
                                                 ShowDataTable(pdt,1,3);
                                                 continue;

                                     case 'h':   //goto head
                                     case 'H':
                                                 nCurrent=1;
                                                 continue;
                                     case 'E':   //goto end
                                     case 'e':
                                                 nCurrent=pdt->nCount;
                                                 continue;
                                     case 'u':  //page up
                                     case 'U':
                                                 nCurrent-=pdt->nPage;
                                                 if(nCurrent<1) nCurrent=1;
                                                 continue;
                                     case 'd':  //page down
                                     case 'D': 
                                                 nCurrent+=pdt->nPage;
                                                 if(nCurrent>pdt->nCount)
                                                         nCurrent=pdt->nCount;
                                                 continue;
				     case '+':
						 nwhere=2;
						 continue;
				     default:continue;
			    }
		   case 2:
                            alltrim(sJymc);
                            if(IsInMenu(pmsp,sJymc,1)<=0)
                            {
		                 nId=0;
			         nId=sJymc[0]-'0';
			         if(nId<=0||nId>pmsp->nTotal)
			         {
				     getxy(&npx,&npy);
				     nId=PopMenu(pmsp,15,npy,12);
				     ShowDataTable(pdt,1,3);
				     if(nId<=0) continue;
			         }
			         strcpy(sJymc,GetMenu(pmsp,nId));
			         strcpy(sJym,GetAddition(pmsp,nId));
			         RunSelect("select to_number(substr(clxbz,2,1)) from dl$jym \
			                    where cjym=%s into %ld",sJym,&identify);
			         ImportData(pdt,nCurrent,"> %s > > %d > %s%d",
			                                     sJymc,identify,sJym,identify);
			                                     
			    }
  	                    nModifyFlag=1;//the document has been modified
			    nwhere=3;//next field
			    continue;
		   case 3:
			    if(dPrice<0.0) continue;
			    nwhere=4+(identify==0);
			    continue;
                   case 4:
                            alltrim(sIdent);
                            if(sIdent[0]=='.') continue;
                            //����Ƿ�����ظ�����Ʒ��ʶ
                            for(nKey=1;nKey<=pdt->nCount;nKey++)
                            {
                            	if(nKey==nCurrent) continue;
 	                        ExportData(pdt,nKey,"> > > %s > > %s >",sIdentTemp,sJymTemp);
 	                        alltrim(sIdentTemp);
 	                        if(!strcmp(sIdent,sIdentTemp)&&!strcmp(sJymTemp,sJym)) break;
 	                    }
 	                    if(nKey<=pdt->nCount) continue; //detected contradiction,invalid input
 	                    
			    nwhere=5;
			    continue;
		   case 5:
			    if(nCount>0) nwhere=6;
			    continue;
	           case 6:
                            alltrim(sTransActor);
	                    if(strchr(sTransActor,'.')||strlen(sTransActor)<2)
	                    {
	                    	getxy(&npx,&npy);
         		        nId=PopMenu(pmjbr,55,npy,12);
				ShowDataTable(pdt,1,3);
  			        if(nId<=0) continue;
			        strcpy(sTransActor,GetMenu(pmjbr,nId));
			        ImportData(pdt,nCurrent,"> > > > > %s",sTransActor);
			    }
			    nCurrent++;
			    continue;
	           		    
	}//end switch(nwhere);
   }//end for(;;)
}

static long SavePurchaseData(DATATABLE * pdt)
{

    char sJym[16],sJymc[51],sLsh[41];
    char cLxbz[21],sIdent[21],sTransActor[7];
    long nCount,nId,nRetVal,identify;
    double dPrice;
    
    ShowDatatableInfo(pdt,"������⣬���Ժ�...",0);
    nRetVal=RunSql("delete from dl$hwjcb where nczlb=1 and njlzt=0 and \
                                cjhczydh=%s",sSysSyz);
    if(nRetVal<0)
    {
        MessageBox("���ݿ����ʧ��",2);
    	RollbackWork();
    	return -1; 
    }

    nRetVal=1;
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	
    	ExportData(pdt,nId,"> %s%f%s%d%s%s%d",sJymc,&dPrice,
    	             sIdent,&nCount,sTransActor,sJym,&identify);


        alltrim(sJym);
        alltrim(sIdent);
        alltrim(sJymc);
        alltrim(sTransActor);
        
        if(strchr(sJymc,'.')) break;
        GenerateLsh(1,sSysJsdh,sJym,sLsh);
        strcpy(cLxbz,"00000000000000000000");
        if(identify) cLxbz[0]='1';

        nRetVal=RunSql("insert into dl$hwjcb(cjym,cjymc,clsh,djhdj,nsl,chwbz,\
                        cfhczydh,cjhczydh,cfhjh,cjhjh,njhrq,njhsfm,nfhrq,nfhsfm,\
                        nczlb,njlzt,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,clxbz) \
                        values(%s,%s,%s,%lf,%ld,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,\
                        1,0,0,0,0,0,'0','0',%s)",
                        sJym,sJymc,sLsh,dPrice,nCount,sIdent,sTransActor,sSysSyz,
                        sSysJsdh,".",nSysLkrq,nSysSfm,nSysLkrq,nSysSfm,cLxbz);
        if(nRetVal<0) break;                        
    }

    if(nRetVal<0)
    {
    	RollbackWork();
        MessageBox("���ݿ����ʧ��",2);
    	return -1;
    }
    else
    {
    	CommitWork();
        ShowDatatableInfo(pdt,"������¼�������...",0);
        return 0;
    }

}

static long LoadPurchaseData(DATATABLE * pdt)
{
    char sJym[16],sJymc[51],sIdent[17],sTransActor[7];
    long nCount,nId,identify;
    double dPrice;

    nId=OpenCursor("select cjym,cjymc,djhdj,chwbz,nsl,cfhczydh,\
                          to_number(substr(clxbz,1,1)) from dl$hwjcb where nczlb=1 and \
                          njlzt=0 and cjhczydh=%s order by njhsfm",sSysSyz);

    if(nId<0)  return -1;
    
    for(;;)
    {
    	if(FetchCursor(nId,"%s%s%lf%s%ld%s%ld",sJym,sJymc,&dPrice,sIdent,
    	                   &nCount,sTransActor,&identify)<=0) break;
    	
    	ImportData(pdt,pdt->nCount+1,"%s%s%f%s%d%s%s%d","+",sJymc,dPrice,
    	             sIdent,nCount,sTransActor,sJym,identify);
    }
    CloseCursor(nId);
    return 0;
}

static long CommitPurchaseData(DATATABLE * pdt)
{
    
    RECORD rec;
    long nRetVal;
    char sbuf[81];
    
    
    if(MessageBox("    һ��������¼���棬����Ҳ�����޸���Щ������¼��Ҫ������",0)!=KEY_YES) return 1;
    if(SavePurchaseData(pdt))
    {
    	keyb();
    	return 1;
    }

    ShowDatatableInfo(pdt,"�������棬��ȴ�...",0);
    
    nRetVal=AutoRunTask("dldj",TASK_COMMIT_PURCHASE,"%s","%r",sSysSyz,&rec);
    if(nRetVal)
    {
          MessageBox(sTcpipErrInfo,1);
          return -1;
    }
    ExportRecord(&rec,"%s",sbuf);
    if(strncmp(sbuf,"000000",6))
    {
    	ExportRecord(&rec,"%s",sbuf);
        DropRecord(&rec);
    	MessageBox(sbuf,1);
    	return 1;
    }
    DropRecord(&rec);
    while(pdt->nCount) DeleteDataNode(pdt,pdt->nCount);
    ShowDatatableInfo(pdt,"��������������!",0);
    return 0;

}


MENU * CreateTransActorMenu()
{
    MENU * pmh;
    char sTransActor[7];
    long nId;

    if(!(pmh=CreateMenu("ACTOR",0))) return NULL;
    nId=OpenCursor("select distinct cfhczydh from dl$hwjcb where nczlb=1");
    if(nId<0)
    {
    	DropMenu(pmh);
    	return NULL;
    }
    
    for(;;)
    {
    	
    	if(FetchCursor(nId,"%s",sTransActor)<=0) break;
    	AddMenu(pmh,sTransActor,NULL);
    }
    CloseCursor(nId);
    return pmh;

}

MENU * CreateGoodsMenu()
{
    MENU * pmh;
    char sJym[16],sJymc[51];
    long nId;
   
    if(!(pmh=CreateMenu("SELECT",0))) return NULL;

    if((nId=OpenCursor("select distinct cjym,cjymc from dl$jym \
                                        where substr(cjym,1,2)='02'"))<0)
    {
    	DropMenu(pmh);
    	return NULL;
    }
                                        

    for(;;)
    {
	if(FetchCursor(nId,"%s%s",sJym,sJymc)<=0) break;
	AddMenu(pmh,sJymc,sJym);
    }
    CloseCursor(nId);
    return pmh;

}


void AcceptGoods()
{
    DATATABLE * pdt;//¼��������ݱ�����
    char sOpt[3];
    long nCurrent,nKey;
    long nwhere;

    clrscr();
    sprintf(sSysSyz,"%s.%s",sSysJsdh,sSysCzydh);
    MessageBox("    ������ȡ������Ա�Ľӻ���Ϣ�����Ժ�...  ",2);

    pdt=CreateDataTable(8,
          "|  |��Ʒ����                  |����  |������            |��������|",
		"%-1.1s%-24.24s%-4d%-16.16s%d%s");
    if(!pdt)
    {
	MessageBox("�������ݱ�����...",1);
	return;
    }

    MergeTable(pdt->pTable,CreateTable(1,
          "|-:���ӻ� +:�ӻ� H:��ǰ E:��� U:�Ϸ� D:�·� I:���� ESC:����       |"));

    
    if(LoadAcceptData(pdt))
    {
	 DropDataTable(pdt);
	 return;
    }

    if(pdt->nCount<1)
    {
    	 MessageBox("    û�нӻ���¼...",1);
    	 DropDataTable(pdt);
    	 return;
    }
    	
    
    //begin input
    gotoxy(1,1);
    outtext("                          �����������(��  ��)");
    ShowDataTable(pdt,5,3);
    nCurrent=1;//start with the first line
    nwhere=1;
    for(;;)
    {
	//Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"%-1.1s");
        //Export data from container which you inputed
	ExportData(pdt,nCurrent,"%s",sOpt);
	
	if(sOpt[0]=='i'||sOpt[0]=='I') ImportData(pdt,nCurrent,"%s","+");
	else if(sOpt[0]!='+') ImportData(pdt,nCurrent,"%s","-");

        switch(nKey)
	{
	        case KEY_RETURN:break;
		case KEY_UP:
			    if(nCurrent>1) nCurrent--;
			    continue;
		case KEY_DOWN:
			    if(nCurrent<pdt->nCount) nCurrent++;
			    continue;
		case KEY_ESC:
			    DropDataTable(pdt);
			    return;
		default:continue;
        }
        if(sOpt[0]=='i'||sOpt[0]=='I')
        {
        	CommitAcceptData(pdt);
        	ShowDataTable(pdt,5,3);
        }
        
        if(pdt->nCount<1)
        {
        	DropDataTable(pdt);
        	return;
        }

    }//end for(;;)
}



static long LoadAcceptData(DATATABLE * pdt)
{
    char sJymc[51],sIdent[21],sFhczydh[7],sLsh[41];
    long nCount,nId,nFhrq;

    nId=OpenCursor("select cjymc,nsl,cfhczydh,nfhrq,clsh from dl$hwjcb \
                    where nczlb=3 and njlzt=1 and cjhczydh=%s order by nfhrq,nfhsfm",
                    sSysSyz);

    if(nId<0)  return -1;
    
    for(;;)
    {
    	if(FetchCursor(nId,"%s%ld%s%ld%s",sJymc,&nCount,sFhczydh,&nFhrq,sLsh)<=0) break;
    	ImportData(pdt,pdt->nCount+1,"%s%s%d%s%d%s","-",sJymc,nCount,
    	               sFhczydh,nFhrq,sLsh);
    }
    CloseCursor(nId);
    return 0;
}

static long CommitAcceptData(DATATABLE * pdt)
{
    char sOpt[3],sLsh[41],sRet[7];
    long nId,nRetVal;
    RECORD rec;
    extern char sTcpipErrInfo[81];
   
    if(MessageBox("    һ��ѡ��ӻ����󣬾Ͳ������ٶԽӻ����ݽ����޸ġ�\n\
��ȷ�������õ��Ļ�Ʒʵ��Ӧ��ӻ���¼����һ�£�Ҫ������",0)!=KEY_YES) return 1;

    ShowDataTable(pdt,1,3);
    
    MessageBox("������⣬���Ժ�...",2);
    for(nId=1;nId<=pdt->nCount;nId++)
    {
        ExportData(pdt,nId,"%s > > > > %s",sOpt,sLsh);
    	if(sOpt[0]!='+') continue;
    	nRetVal=AutoRunTask("dldj",TASK_ACCEPT_DISTRIBUTE,"%s","%r",sLsh,&rec);
    	if(nRetVal<0)
    		MessageBox(sTcpipErrInfo,1);
        else
        {
        	ExportRecord(&rec,"%s",sRet);
        	if(strncmp(sRet,"000000",6))
        	{
        		ExportRecord(&rec,"%s",sTcpipErrInfo);
                        ImportData(pdt,nId,"%s","-");
        		MessageBox(sTcpipErrInfo,1);
        	}
        	else    DeleteDataNode(pdt,nId);
        	DropRecord(&rec);
        }
    }    
    return 0;     	
}
//��ί�з��˻�
void ReturnGoods()
{

     DIALOG * pd;
     RECORD rec;
     MENU * pmsp,* pmident;
     char sJym[16],sJymc[51],sHwbz[21],sInfo[81],sLsh[41],sbuf[256];
     long nCount,nKey,nwhere,nId,identify;
     long nfinished,nTest,nRetVal,nKcl;
     double dMoney=0.0,dPrice=0.0;

     ///////////////////////////////////////////////////////////////////     
     sprintf(sSysSyz,"%s.%s",sSysJsdh,sSysCzydh);
     pmsp=CreateGoodsMenu();
     if(!(pd=CreateDialog(60,15,FRAME))) return;
     AddDialog(pd,"static",STATIC_TEXT,"               ��  ��  ί  ��  ��  ��  ��  ��\n");
     AddDialog(pd,"set1",SEPARATOR,"__\n");
     
     AddDialog(pd,"cspmc",INPUT,"��Ʒ����:%-46.46s\n\n",sJymc);           //3
     AddDialog(pd,"nspsl",INPUT,"��Ʒ����:%8d",&nCount);                  //4
     AddDialog(pd,"nspbs",INPUT,"         ��Ʒ��ʶ:%-20.20s\n",sHwbz);    //5
     
     AddDialog(pd,"sep2",SEPARATOR,"__\n");

     AddDialog(pd,"dspdj",OUTPUT,"��Ʒ����:%10.2f",&dPrice);
     AddDialog(pd,"dytzk",OUTPUT,"        Ӧ���ܿ�:%10.2f",&dMoney);
     //////////////////////////////////////////////////////////////////
     clrscr();
     
     for(;;)
     {
     	nwhere=1;//start
        dMoney=0.0;
        nCount=0;
        identify=0;//¼���Ʒ��ʶ
        memset(sJymc,0,sizeof(sJymc));
        memset(sHwbz,0,sizeof(sHwbz));
        sJymc[0]='.'; 
        sHwbz[0]='.'; 
        
        ShowDialog(pd,10,3);
        RefreshDialog(pd,NULL);
        nfinished=0;
        for(;;)
        {
                if(nfinished) break;
                switch(nwhere)
                {
                   case 1:
                        SetDialogInfo(pd,"��������Ʒ���ƣ��򰴻س���ѡ����Ʒ..");
                        break;

                   case 2:
                        SetDialogInfo(pd,"�����뱨������...");
                        break;

                   case 3:
                        SetDialogInfo(pd,"�������Ʒ��ʶ...");
                        break;


                }
                	    
        	nKey=ActiveDialog(pd,nwhere,"cspmc,nspsl,nspbs");

        	switch(nKey)
        	{
        	   case KEY_RETURN:
        	   case KEY_ESC:
        	        break;
        	   
        	   case KEY_LEFT:
        	   case KEY_UP:
        	        if(nwhere>1) nwhere--;
        	        continue;
                   default:continue;
                }   	             
        	if(nKey==KEY_ESC) break;
                switch(nwhere)
                {
                   case 1:
                        nId=0;
                        alltrim(sJymc);
                        nId=atol(sJymc);
                        if(nId<=0||nId>pmsp->nTotal)
                        {    
                             savewin();
                             nId=PopMenu(pmsp,20,9,16);
                             popwin();
                             if(nId<0)
                             {
  	                             RefreshDialog(pd,0);
                                     continue;
                             }
                                
                        }     
                        strcpy(sJymc,GetMenu(pmsp,nId));
                        strcpy(sJym,GetAddition(pmsp,nId));
                        RunSelect("select to_number(substr(clxbz,5,1)),dbl from dl$jym \
                                   where cjym=%s into %ld%lf",sJym,&identify,&dPrice);
                        nwhere=4;
                        if(identify)
                        {
                            nCount=1;
                            nwhere=5;
                        }
                        RefreshDialog(pd,0);
                        continue;

                   case 2:
                        if(nCount<1) continue;
                        nKcl=0;
                        RunSelect("select nkcl from dl$hwkcb where csyz=%s \
                                                and cjym=%s into %ld",sSysSyz,sJym,&nKcl);
                        if(nCount>nKcl)
                        {
                        	sprintf(sbuf,"����Ʒ�Ŀ����Ϊ:%ld",nKcl);
                        	SetDialogInfo(pd,sbuf);
                        	keyb();
                        	continue;
                        }
                        if(identify) nwhere=5;
                        else nfinished=1;   //input finished
                        continue;

                   case 3:
                        alltrim(sHwbz);
                        nTest=0;
                        SetDialogInfo(pd,"���ڼ����Ʒ��ʶ�����Ժ�...");

                        if(sHwbz[0]!='.')  
                           RunSelect("select count(*) from dl$hwjcb \
                                         where cjym=%s and cjhczydh=%s and chwbz=%s \
                                         and ((nczlb=1 and njlzt=1) or \
                                         (nczlb=3 and njlzt=2)) into %ld",
                                         sJym,sSysSyz,sHwbz,&nTest);
                        if(nTest<=0||sHwbz[0]=='.')
                        {                 
                             SetDialogInfo(pd,"��Ʒ��ʶ�����ڣ������γ���Ʒ��ʶ�˵������Ժ�...");
                             if(!(pmident=CreateIdentMenu(sJym))) continue;
                             SetDialogInfo(pd,"��Ӳ˵���ѡ��һ����Ʒ��ʶ������Ӧ���ּ�...");
                             savewin();
                             nId=PopMenu(pmident,46,11,14);
                             popwin();
                             if(nId<=0)
                             {
                         	       DropMenu(pmident);
                                       RefreshDialog(pd,0); 
                        	       continue;
                             }
                             strcpy(sHwbz,GetMenu(pmident,nId));
                             DropMenu(pmident);
                        }
                        RefreshDialog(pd,0); 
                        nfinished=1;
                        continue;
                }
        	
        	break;
        }//end input/for(;;)
        
        if(nKey==KEY_ESC)
        {
        	DropDialog(pd);
        	DropMenu(pmsp);
        	break;
        }
       
        dMoney=dPrice*nCount;
        RefreshDialog(pd,"dytzk");
        
        SetDialogInfo(pd,"��'1'��ȷ���˻���ESC������...");
        
        for(;;)
        {
        	nKey=keyb();
        	if(nKey==KEY_ESC||nKey=='1') break;
        }
        if(nKey==KEY_ESC) continue;
        
        GenerateLsh(1,sSysJsdh,sJym,sLsh);//���ɳ�����ˮ�� 

        SetDialogInfo(pd,"�����޸Ŀ����,���Ժ�...");
        nRetVal=AutoRunTask("dldj",TASK_RETURN_GOODS,"%s%s%s%ld","%r",
                                        sLsh,sSysSyz,sHwbz,nCount,&rec);

        if(nRetVal)
        {
        	SetDialogInfo(pd,sTcpipErrInfo);
        	keyb();
        }
        ExportRecord(&rec,"%s",sInfo);
        if(strncmp(sInfo,"000000",6))
        {
        	ExportRecord(&rec,"%s",sbuf);
        	DropRecord(&rec);
        	SetDialogInfo(pd,sbuf);
        	keyb();
        }
        else   //�ɹ�
        {
                DropRecord(&rec);
        	SetDialogInfo(pd,"�˻����޸Ŀ��ɹ�...");
        	sleep(1);
        	continue;
        }

   }//end for(;;)

}




