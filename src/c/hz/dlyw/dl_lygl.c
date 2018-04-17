/*=======================================================
filename:dl_lygl.c 
target:ʵ��·�ɱ��¼��������޸� 
orginal programmer:yuanlianhai  Ԭ����
*======================================================*/

#include"dlpublic.h"
#include"tools.h"
#include"setting.h"

static long SaveRouteData(DATATABLE * pdt);
static long LoadRouteData(DATATABLE * pdt);

//////////////////////·����Ϣ����/////////////////////////////
void RouteManage()
{
    DATATABLE * pdt;//¼��������ݱ�����
    long nKey;
    long nCurrent;//��ĵ�ǰ��
    long nwhere; //��ĵ�ǰ��
    long nModifyFlag; //�޸ı�־
    char sDest[21],sGate[21],sOpt[2];
    
    pdt=CreateDataTable(8,
    "|  |Ŀ�ĵ�                        |����                          |",
    "%-1.1s%-20.20s%-20.20s");

    AssignTableFields(pdt,"copt,cdestination,cgateway");
    SetTableStyle(pdt,ALL_STYLE^ST_NOTCONFIRM^ST_DOUBLELINE,1); 

    clrscr();           
    gotoxy(1,1);	
    outtext("                             ·  ��  ��  ��");
    
    ShowDataTable(pdt,6,3);
    ShowDatatableInfo(pdt,"������ȡ·�ɱ���Ϣ�����Ժ�...  ",0);
    if(LoadRouteData(pdt))
    {
	 DropDataTable(pdt);
	 return;
    }
    //begin input
    nCurrent=1;//start with the first line
    nwhere=1;//start with the first column
    nModifyFlag=0;//the document is modified if the flag is nozero
    for(;;)
    {
	//auto append a new record
        if(AutoExtendTable(pdt,&nCurrent)>0)
		ImportData(pdt,nCurrent,"copt","+");
        
        switch(nwhere)
        {
            case 1:
            ShowDatatableInfo(pdt,"�����빦�ܿ��ƣ���M����ʾ���ܼ�����...",0);
            break;
            case 2:
            ShowDatatableInfo(pdt,
            "������Ŀ�ĵ�ַ�����������е�Ŀ�ĵ��ظ���",0);
            break;
            case 3:
            ShowDatatableInfo(pdt,
            "�����뵽��Ŀ�ĵ���Ҫ���������ء�",0);
            break;
        }
         

	//Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"copt,cdestination,cgateway");

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
                                ShowDataTable(pdt,6,3);
			        continue;
			}
			if(nKey==KEY_YES) SaveRouteData(pdt);
		    }
		    DropDataTable(pdt);
	            return;
           default:
                    DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
                    continue;
	}
        //Export data from container which you inputed
	ExportData(pdt,nCurrent,"copt,cdestination,cgateway",sOpt,sDest,sGate);
	ImportData(pdt,nCurrent,"copt","+");

	switch(nwhere)
	{
	   case 1: //��������
		    switch(sOpt[0])
		    {
                             case 's':   //save as temporary record
			     case 'S':
				 if(nModifyFlag)
				 {
					 nModifyFlag=SaveRouteData(pdt);
					 keyb();
					 ShowDataTable(pdt,6,3);
				 }
				 continue;
			     default:
			         DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nwhere,&nModifyFlag);
			         continue;
		    }
		    break;
	   case 2:
	            nKey=FindDataTable(pdt,1,"cdestination",sDest);
	            if(nKey==nCurrent) nKey=FindDataTable(pdt,nCurrent+1,"cdestination",sDest);
	            if(nKey>0)
              	    {
                          ShowDatatableInfo(pdt,"��Ŀ�ĵ��Ѿ����ڣ�����������",1);
                          break;
                    }
                    nwhere++;
		    continue;
           case 3:
                    nModifyFlag=1;
	            nwhere=2;
	            nCurrent++;
		    continue;
           		    
        }//end switch(nwhere);
   }//end for(;;)
}

static long SaveRouteData(DATATABLE * pdt)
{

    long nRetVal=0,nId=0;
    char  sDest[21],sGate[21];
    sDest[0]='\0';
    sGate[0]='\0';
 
    ShowDatatableInfo(pdt,"������⣬���Ժ�...",0);
    nRetVal=RunSql("delete from dl$route");

    if(nRetVal<0)
    {
        MessageBox("���ݿ����ʧ��",2);
    	RollbackWork();
    	return -1; 
    }

    nRetVal=1;
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	
    	ExportData(pdt,nId,"cdestination,cgateway",sDest,sGate);
        alltrim(sDest);
        alltrim(sGate);
        if(sGate[0]=='.') continue;
        nRetVal=RunSql("insert into dl$route(sdestination,sgateway) \
                        values(%s,%s)",sDest,sGate);
        if(nRetVal<0) break;                        
    }

    if(nRetVal<0)
    {
        HiError(nRetVal,"���ݿ����ʧ��");
    	RollbackWork();
    	return -1;
    }
    else
    {
    	CommitWork();
        ShowDatatableInfo(pdt,"�������...",0);
        return 0;
    }

}

static long LoadRouteData(DATATABLE * pdt)
{
    long nId=0;
    char sDest[21],sGate[21];
    sDest[0]='\0';
    sGate[0]='\0';

    nId=OpenCursor("select sdestination,sgateway from dl$route \
                    order by sdestination");

    if(nId<0)  
    {
       ShowDatatableInfo(pdt," ��ȡ���ݴ��������´����ݿ�",1);
       return -1;
    }
    
    for(;;)
    {
    	if(FetchCursor(nId,"%s%s",sDest,sGate)<=0) break;
    	ImportData(pdt,pdt->nCount+1,"copt,cdestination,cgateway","+",sDest,sGate);
    }
    CloseCursor(nId);
    return 0;
}
