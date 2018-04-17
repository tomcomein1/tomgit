/*=======================================================
filename:dl_jhgl.c 
target: �ؾֹ���֧�ֹ���;ֺŶ��չ���
orginal programmer:chenbo �²�
*======================================================*/


#include"dlpublic.h"
#include"tools.h"
#include"setting.h"

extern long nSysCzyqx;
extern char sSysJh[10];
extern char sSysJsdh[10];
static long SaveXianJuData(DATATABLE * pdt);
static long LoadXianJuData(DATATABLE * pdt);
static long SaveZhiJuData(DATATABLE * pdt);
static long LoadZhiJuData(DATATABLE * pdt);

//////////////////////֧����Ϣ����//////////////////////////////
void ZhiJuJuHao()
{
    DATATABLE * pdt;//¼��������ݱ�����
    long nXjbh,nZjbh,nKey,nTempbh,nCurrent,nwhere,nModifyFlag;
    char sDlJh[10],sOpt[2],sZhwJh[9],sDlJm[21],sBz[19];
    char sXjmc[21];
    
    clrscr();           
    RunSelect("select cjm from dl$jh where nlb=3 and substr(cjh,1,6)=\
               substr(%s,1,6) into %s",sSysJsdh,sXjmc);
     
    pdt=CreateDataTable(8,
    "|  |֧�ֺ�|����ֺ�  |�ۺ����ֺ�|����                |��ע            |",
		"%-1.1s%-3d%-9.9s%-8.8s%-20.20s%-16.6s%s");
    if(!pdt)
    {
	MessageBox("�������ݱ�����...",1);
	return;
    }
    
    SetTableStyle(pdt,ALL_STYLE^ST_NOTCONFIRM^ST_DOUBLELINE,1);
    AssignTableFields(pdt,"copt,nzjbh,cdljh,czhwjh,cdljm,cbz,clxbz");

    
    ShowDataTable(pdt,1,3);
    ShowDatatableInfo(pdt,"������ȡ֧����Ϣ�����Ժ�...  ",0);

    if(LoadZhiJuData(pdt))
    {
	 DropDataTable(pdt);
	 return;
    }

    gotoxy(1,1);	
    outtext("                          %s��֧�ֹ���",sXjmc);
    ShowDataTable(pdt,1,3);
    //begin input
    nCurrent=1;//start with the first line
    nwhere=1;
    nModifyFlag=0;//the document is modified if the flag is nozero
    for(;;)
    {
	//auto append a new record
        if(AutoExtendTable(pdt,&nCurrent)>0)
		ImportData(pdt,nCurrent,"copt,clxbz","+","0000000000");
        
        switch(nwhere)
        {
            case 1:
            ShowDatatableInfo(pdt,
            "�����빦�ܿ��ƣ���M����ʾ���ܼ�����...",0);
            break;
            case 2:
            ShowDatatableInfo(pdt,
            "�������ؾֺţ����������еľ��ظ���",0);
            break;
            case 3:
            ShowDatatableInfo(pdt,
            "��������ؾ�����Ӧ���ۺ����ֺţ���61810000��",0);
            break;
            
        }
         

	//Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"copt,nzjbh,czhwjh,cdljm,cbz");
        //Export data from container which you inputed

        switch(nKey)
	{
	   case KEY_RETURN:
	 	    break;
	   case KEY_ESC:
		    if(nwhere!=1)
		    {
				nwhere=1;
				continue;
		    }
	    	    if(nModifyFlag)
		    {
			savewin();
			nKey=MessageBox("�����޸�û�б��棬Ҫ������",0);
			popwin();
			if(nKey==KEY_ESC) continue;
			if(nKey==KEY_YES) SaveZhiJuData(pdt);
			
		    }
		    DropDataTable(pdt);
   	            return;
	   default:
	            DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
	            continue;
	}

	ExportData(pdt,nCurrent,"copt,nzjbh,cdljh,czhwjh,cdljm,cbz",
	               sOpt,&nZjbh,sDlJh,sZhwJh,sDlJm,sBz);
	ImportData(pdt,nCurrent,"copt","+");
	switch(nwhere)
	{
	   case 1: //��������
		    switch(sOpt[0])
		    {
                             case 's':   //save as temporary record
			     case 'S':
				 if(nModifyFlag)
					 nModifyFlag=SaveZhiJuData(pdt);
				 continue;
			     default:
                                 DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nwhere,&nModifyFlag);
               	                 continue;
		    }
		    break;
	   //�ؾֱ�ż��
	   case 2:
	            //�ϳɴ���ֺ�
                    nModifyFlag=1;
	            if(nZjbh<1) continue;
	            sprintf(sDlJh,"%-6.6s%03ld",sSysJsdh,nZjbh);
                    nKey=FindDataTable(pdt,1,"nzjbh",nZjbh);
                    if(nKey==nCurrent) nKey=FindDataTable(pdt,nCurrent+1,"nzjbh",nXjbh);
                    if(nKey>0)
	            {
                          ShowDatatableInfo(pdt,"��֧�־ֺ��Ѿ����ڣ�����������",1);
                          continue;
                    }
                    nwhere++;
                    break;
           case 3:
	   case 4:
                    nwhere++;
		    continue;
           case 5:
	            nwhere=2;
	            nCurrent++;
		    continue;
           		    
        }//end switch(nwhere);
	ImportData(pdt,nCurrent,"copt,nzjbh,cdljh,czhwjh,cdljm,cbz",
	               sOpt,nZjbh,sDlJh,sZhwJh,sDlJm,sBz);
   }//end for(;;)
}

static long SaveZhiJuData(DATATABLE * pdt)
{

    long nId,nRetVal;
    char sDlJh[10],sZhwJh[9],sDlJm[21],sBz[19],sLxbz[11];
    
    ShowDatatableInfo(pdt,"������⣬���Ժ�...",0);
    nRetVal=RunSql("delete from dl$jh where nlb=4 and substr(cjh,1,6)=\
                    substr(%s,1,6)",sSysJsdh);
    if(nRetVal<0)
    {
        MessageBox("���ݿ����ʧ��",2);
    	RollbackWork();
    	return -1; 
    }

    nRetVal=1;
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	
    	ExportData(pdt,nId,"cdljh,czhwjh,cdljm,cbz,clxbz",sDlJh,sZhwJh,sDlJm,sBz,sLxbz);
        if(sDlJm[0]=='.') continue;
        nRetVal=RunSql("insert into dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) \
                        values(%s,%s,%s,4,%s,%s)",sDlJh,sDlJm,sZhwJh,sLxbz,sBz);
        if(nRetVal<0) break;                        
    }

    if(nRetVal<0)
    {
    	RollbackWork();
        HiError(nRetVal,"���ݿ����ʧ��");
    	return -1;
    }
    else
    {
    	CommitWork();
        ShowDatatableInfo(pdt,"�������...",0);
        return 0;
    }

}

static long LoadZhiJuData(DATATABLE * pdt)
{
    long nId,nRetVal,nJh;
    char sDlJh[10],sZhwJh[9],sDlJm[21],sBz[19],sLxbz[11];
    nId=OpenCursor("select cjh,cjm,czhwjh,clxbz,cbz from dl$jh where nlb=4 and \
                    substr(cjh,1,6)=substr(%s,1,6) order by cjh",sSysJsdh);

    if(nId<0)  return -1;
    
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%s%s%s%s%s",sDlJh,sDlJm,sZhwJh,sLxbz,sBz);
    	if(nRetVal==0) break;
    	if(nRetVal<0)  return -1;
        nJh=(sDlJh[6]-'0')*100+(sDlJh[7]-'0')*10+(sDlJh[8]-'0');
    	ImportData(pdt,pdt->nCount+1,"copt,nzjbh,cdljh,czhwjh,cdljm,cbz,clxbz",
    	               "+",nJh,sDlJh,sZhwJh,sDlJm,sBz,sLxbz);
    }
    CommitWork();
    CloseCursor(nId);
    return 0;
}



////////////////////////////////�ؾֹ�����////////////////////////////////////


void XianJuJuHao()
{
    DATATABLE * pdt;//¼��������ݱ�����
    long nXjbh,nKey,nCurrent,nwhere,nModifyFlag;
    char sDlJh[10],sOpt[2],sZhwJh[9],sDlJm[21],sBz[19];
    
     
    pdt=CreateDataTable(8,
    "|  |�ر��|����ֺ�  |�ۺ����ֺ�|����                |��ע            |",
		"%-1.1s%-2d%-9.9s%-8.8s%-20.20s%-16.6s%s");
    if(!pdt)
    {
	MessageBox("�������ݱ�����...",1);
	return;
    }

    SetTableStyle(pdt,ALL_STYLE^ST_NOTCONFIRM^ST_DOUBLELINE,1);
    AssignTableFields(pdt,"copt,nxjbh,cdljh,czhwjh,cdljm,cbz,clxbz");

    if(LoadXianJuData(pdt))
    {
	 DropDataTable(pdt);
	 return;
    }

    clrscr();           
    outtext("                          ����ҵ��ϵͳ�ؾֹ���");
    ShowDataTable(pdt,1,3);
    //begin input
    nCurrent=1;//start with the first line
    nwhere=1;
    nModifyFlag=0;//the document is modified if the flag is nozero
  
    for(;;)
    {
	//auto append a new record
        
        if(AutoExtendTable(pdt,&nCurrent)>0)
		ImportData(pdt,nCurrent,"copt,clxbz,nxjbh","+","0000000000",0);
        
        switch(nwhere)
        {
            case 1:
            ShowDatatableInfo(pdt,
            "�����빦�ܿ��ƣ���M����ʾ���ܼ�����...",0);
            break;
            case 2:
            ShowDatatableInfo(pdt,
            "�������ؾֺţ����������еľ��ظ���",0);
            break;
            case 3:
            ShowDatatableInfo(pdt,
            "��������ؾ�����Ӧ���ۺ����ֺţ���61810000��",0);
            break;
            
        }
         

	//Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"copt,nxjbh,czhwjh,cdljm,cbz");
        //Export data from container which you inputed

        switch(nKey)
	{
	   case KEY_RETURN:
	 	    break;
	   case KEY_ESC:
		    if(nwhere!=1)
		    {
				nwhere=1;
				continue;
		    }
	    	    if(nModifyFlag)
		    {
			nKey=MessageBox("�����޸�û�б��棬Ҫ������",0);
			if(nKey==KEY_ESC)
			{
                                ShowDataTable(pdt,1,3);
			        continue;
			}
			if(nKey==KEY_YES) SaveXianJuData(pdt);
		    }
		    DropDataTable(pdt);
		    return;
	   default:
	            DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
	            continue;	    
		    
	}

	ExportData(pdt,nCurrent,"copt,nxjbh,cdljh,czhwjh,cdljm,cbz",
	               sOpt,&nXjbh,sDlJh,sZhwJh,sDlJm,sBz);
	ImportData(pdt,nCurrent,"%s","+");

	switch(nwhere)
	{
	   case 1: //��������
		    switch(sOpt[0])
		    {
                             case 's':   //save as temporary record
			     case 'S':
				 if(nModifyFlag)
				 {
					 nModifyFlag=SaveXianJuData(pdt);
					 keyb();
					 ShowDataTable(pdt,1,3);
				 }
				 continue;
			     default:
       	                         DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nwhere,&nModifyFlag);
   		                 continue;
		    }
		    break;
	   //�ؾֱ�ż��
	   case 2:
	            //�ϳɴ���ֺ�
                    nModifyFlag=1;
	            if(nXjbh<1) continue;
	            sprintf(sDlJh,"%-4.4s%02ld000",sSysJh,nXjbh);
                    nKey=FindDataTable(pdt,1,"nxjbh",nXjbh);
                    if(nKey==nCurrent) nKey=FindDataTable(pdt,nCurrent+1,"nxjbh",nXjbh);
                    if(nKey>0)
	            {
                          ShowDatatableInfo(pdt,"���ؾ־ֺ��Ѿ����ڣ�����������",1);
                          continue;
                    }
                    nwhere++;
		    break;
	   case 3:
           case 4:
                    nwhere++;
		    continue;
           case 5:
	            nwhere=2;
	            nCurrent++;
		    continue;
           		    
        }//end switch(nwhere);
	ImportData(pdt,nCurrent,"copt,nxjbh,cdljh,czhwjh,cdljm,cbz",
	               sOpt,nXjbh,sDlJh,sZhwJh,sDlJm,sBz);
   }//end for(;;)
}

static long SaveXianJuData(DATATABLE * pdt)
{

    long nId,nRetVal;
    char sDlJh[10],sZhwJh[9],sDlJm[21],sBz[19],sLxbz[11];
    
    ShowDatatableInfo(pdt,"������⣬���Ժ�...",0);
    nRetVal=RunSql("delete from dl$jh where nlb=3 and substr(cjh,1,4)=\
                    substr(%s,1,4)",sSysJh);
    if(nRetVal<0)
    {
        MessageBox("���ݿ����ʧ��",2);
    	RollbackWork();
    	return -1; 
    }

    nRetVal=1;
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	
    	ExportData(pdt,nId,"cdljh,czhwjh,cdljm,cbz,clxbz",sDlJh,sZhwJh,sDlJm,sBz,sLxbz);
        alltrim(sDlJm);
        alltrim(sBz);
        if(sDlJm[0]=='.') continue;
        nRetVal=RunSql("insert into dl$jh(cjh,cjm,czhwjh,nlb,clxbz,cbz) \
                        values(%s,%s,%s,3,%s,%s)",sDlJh,sDlJm,sZhwJh,sLxbz,sBz);
        if(nRetVal<0) break;                        
    }

    if(nRetVal<0)
    {
    	RollbackWork();
        HiError(nRetVal,"���ݿ����ʧ��");
    	return -1;
    }
    else
    {
    	CommitWork();
        ShowDatatableInfo(pdt,"�������...",0);
        return 0;
    }

}

static long LoadXianJuData(DATATABLE * pdt)
{
    long nId,nRetVal,nJh;
    char sDlJh[10],sZhwJh[9],sDlJm[21],sBz[19],sLxbz[11];
    nId=OpenCursor("select cjh,cjm,czhwjh,clxbz,cbz from dl$jh where nlb=3 and \
                    substr(cjh,1,4)=substr(%s,1,4) order by cjh",sSysJh);

    if(nId<0)
    {
    	HintError(nId,"��ѯ���ݿ����!");
    	return -1;
    }

    
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%s%s%s%s%s",sDlJh,sDlJm,sZhwJh,sLxbz,sBz);
    	if(nRetVal==0) break;
    	if(nRetVal<0)  return -1;

        nJh=(sDlJh[4]-'0')*10+sDlJh[5]-'0';
    	ImportData(pdt,pdt->nCount+1,"copt,nxjbh,cdljh,czhwjh,cdljm,cbz,clxbz",
    	               "+",nJh,sDlJh,sZhwJh,sDlJm,sBz,sLxbz);
    }
    CloseCursor(nId);
    CommitWork();
    return 0;
}


void XianJuHaoDuiZhao()
{
    
    char sTitle[56],sXjjh[10],sXjjm[21],sZhwJh[9];
    long nId;
    SETTABLE * pst;
    
    pst=CreateSetTable(8);
    if(!pst) return;
    nId=OpenCursor("select cjh,cjm from dl$jh where nlb=3 and \
                    substr(cjh,1,4)=substr(%s,1,4) order by cjh",sSysJh);
    if(nId<0)
    {
    	DropSetTable(pst);
    	HiError(nId,"����");                    
    	return;
    }

    for(;;)
    {
    	if(FetchCursor(nId,"%s%s",sXjjh,sXjjm)<=0) break;
    	sprintf(sTitle,"%s(%s)",sXjjh,sXjjm);
    	AddSetItem(pst,sTitle,"czhwjh",8,
    	           "from dl$jh where nlb=3 and cjh=%s",sXjjh);
    }
    CloseCursor(nId);
    LoadDefaultSet(pst);
    clrscr();
    outtext("\n                           �ؾ־ֺŶ�������");
    BeginSetting(pst,4);
    DropSetTable(pst);
}


void ZhiJuHaoDuiZhao()
{
    
    char sTitle[56],sZjjh[10],sZjjm[21],sZhwJh[9],sXjjm[21];
    long nId;
    SETTABLE * pst;
    
    pst=CreateSetTable(8);
    if(!pst) return;


    clrscr();           
    RunSelect("select cjm from dl$jh where nlb=3 and substr(cjh,1,6)=\
               substr(%s,1,6) into %s",sSysJsdh,sXjjm);

    nId=OpenCursor("select cjh,cjm from dl$jh where nlb=4 and \
                    substr(cjh,1,6)=substr(%s,1,6) order by cjh",sSysJsdh);
    if(nId<0)
    {
    	DropSetTable(pst);
    	HiError(nId,"����");                    
    	return;
    }

    for(;;)
    {
    	if(FetchCursor(nId,"%s%s",sZjjh,sZjjm)<=0) break;
    	sprintf(sTitle,"%s(%s)",sZjjh,sZjjm);
    	AddSetItem(pst,sTitle,"czhwjh",8,
    	           "from dl$jh where nlb=4 and cjh=%s",sZjjh);
    }
    CloseCursor(nId);
    LoadDefaultSet(pst);
    clrscr();
    outtext("\n                      %s��֧�־ֺŶ�������",sXjjm);
    BeginSetting(pst,4);
    DropSetTable(pst);
}
