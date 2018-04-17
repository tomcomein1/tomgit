/*=======================================================
filename:dl_lygl.c 
target:实现路由表记录的输入和修改 
orginal programmer:yuanlianhai  袁连海
*======================================================*/

#include"dlpublic.h"
#include"tools.h"
#include"setting.h"

static long SaveRouteData(DATATABLE * pdt);
static long LoadRouteData(DATATABLE * pdt);

//////////////////////路由信息管理/////////////////////////////
void RouteManage()
{
    DATATABLE * pdt;//录入界面数据表格对象
    long nKey;
    long nCurrent;//表的当前行
    long nwhere; //表的当前列
    long nModifyFlag; //修改标志
    char sDest[21],sGate[21],sOpt[2];
    
    pdt=CreateDataTable(8,
    "|  |目的地                        |网关                          |",
    "%-1.1s%-20.20s%-20.20s");

    AssignTableFields(pdt,"copt,cdestination,cgateway");
    SetTableStyle(pdt,ALL_STYLE^ST_NOTCONFIRM^ST_DOUBLELINE,1); 

    clrscr();           
    gotoxy(1,1);	
    outtext("                             路  由  管  理");
    
    ShowDataTable(pdt,6,3);
    ShowDatatableInfo(pdt,"正在提取路由表信息，请稍候...  ",0);
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
            ShowDatatableInfo(pdt,"请输入功能控制，按M键显示功能键帮助...",0);
            break;
            case 2:
            ShowDatatableInfo(pdt,
            "请输入目的地址，不能与已有的目的地重复。",0);
            break;
            case 3:
            ShowDatatableInfo(pdt,
            "请输入到达目的地需要经过的网关。",0);
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
			nKey=MessageBox("您的修改没有保存，要保存吗？",0);
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
	   case 1: //操作方法
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
                          ShowDatatableInfo(pdt,"该目的地已经存在，请重新输入",1);
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
 
    ShowDatatableInfo(pdt,"正在入库，请稍候...",0);
    nRetVal=RunSql("delete from dl$route");

    if(nRetVal<0)
    {
        MessageBox("数据库操作失败",2);
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
        HiError(nRetVal,"数据库操作失败");
    	RollbackWork();
    	return -1;
    }
    else
    {
    	CommitWork();
        ShowDatatableInfo(pdt,"保存完毕...",0);
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
       ShowDatatableInfo(pdt," 读取数据错误，请重新打开数据库",1);
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
