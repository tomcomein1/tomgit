/*=======================================================
filename:dl_jhgl.c 
target: 县局管理、支局管理和局号对照管理
orginal programmer:chenbo 陈博
*======================================================*/


#include"dlpublic.h"
#include"tools.h"

static long SaveYwzlData(DATATABLE * pdt);
static long LoadYwzlData(DATATABLE * pdt);
static long SaveCompanyData(DATATABLE * pdt);
static long LoadCompanyData(DATATABLE * pdt);

//////////////////////业务种类信息管理//////////////////////////////

void YwzlManage()
{
    DATATABLE * pdt;//录入界面数据表格对象
    long nKey,nCurrent,nwhere,nModifyFlag;
    char sOpt[2];
    long nYwbh,nTempYwbh;
    char sYwmc[21],sBz[21];
    
    clrscr();           
     
    pdt=CreateDataTable(8,
    "|  |业务编号|业务名称            |备注                      |",
    "%-1.1s%-4d%-20.20s%-20.20s%d%f%s");
    if(!pdt)
    {
	MessageBox("建立数据表格出错...",1);
	return;
    }

    AssignTableFields(pdt,"copt,nywbh,cywmc,cbz,nbl,dbl,clxbz");
    SetTableStyle(pdt,ST_ALLTRIM|ST_MESSAGELINE,1); 


    gotoxy(1,1);	
    outtext("                           业 务 种 类 管 理");
    ShowDataTable(pdt,8,3);
    if(LoadYwzlData(pdt))
    {
	 DropDataTable(pdt);
	 return;
    }

    //begin input
    nCurrent=1;//start with the first line
    nwhere=1;
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
            "请输入业务编号，不能与已有的编号重复。",0);
            break;

            case 3:
            ShowDatatableInfo(pdt,"请输入业务名称",0);
            break;
            
        }
         

	  //Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"copt,nywbh,cywmc,cbz");

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
                                ShowDataTable(pdt,8,3);
			        continue;
			}
			if(nKey==KEY_YES) SaveYwzlData(pdt);
		    }
		    DropDataTable(pdt);
		    return;
            default: 
                    DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
                    continue;
	}

 	ExportData(pdt,nCurrent,"copt,nywbh,cywmc,cbz",sOpt,&nYwbh,sYwmc,sBz);
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
					 nModifyFlag=SaveYwzlData(pdt);
					 keyb();
					 ShowDataTable(pdt,8,3);
				  }
				  continue;
			     default:
			          DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nwhere,&nModifyFlag);
			          continue;
		    }
		    break;
	   case 2:
                    nModifyFlag=1;
	            if(nYwbh<1) continue;

	            nKey=FindDataTable(pdt,1,"nywbh",nYwbh);
	            if(nKey==nCurrent) nKey=FindDataTable(pdt,nCurrent+1,"nywbh",nYwbh);
	            if(nKey>0)
              	    {
                          ShowDatatableInfo(pdt,
                          "该业务编号已经存在，请重新输入。",1);
                          continue;
                    }
                    nwhere=3;
		    continue;
	    case 3:
                    if(strlen(sYwmc)>=4) nwhere=4;
		    continue;
           case 4:
	            nwhere=2;
	            nCurrent++;
		    continue;
           		    
        }//end switch(nwhere);
   }//end for(;;)
}

static long SaveYwzlData(DATATABLE * pdt)
{
    long nYwbh,nBl,nId,nRetVal;
    double dBl;
    char sYwmc[21],sBz[21],sLxbz[21];
    
    ShowDatatableInfo(pdt,"正在入库，请稍候...",0);
    nRetVal=RunSql("delete from dl$ywzl");
    if(nRetVal<0)
    {
        MessageBox("数据库操作失败",2);
    	RollbackWork();
    	return -1; 
    }

    nRetVal=1;
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	
    	ExportData(pdt,nId,"nywbh,cywmc,cbz,nbl,dbl,clxbz",&nYwbh,sYwmc,sBz,&nBl,&dBl,sLxbz);
        alltrim(sYwmc);
        alltrim(sBz);
        if(sYwmc[0]=='.') continue;
        nRetVal=RunSql("insert into dl$ywzl(nywbh,cywmc,cbl,nbl,dbl,clxbz) \
                        values(%ld,%s,%s,%ld,%lf,%s)",
                        nYwbh,sYwmc,sBz,nBl,dBl,sLxbz);
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

static long LoadYwzlData(DATATABLE * pdt)
{
    long nYwbh,nBl,nId,nRetVal;
    double dBl;
    char sYwmc[21],sBz[21],sLxbz[21];

    nId=OpenCursor("select nywbh,cywmc,cbl,dbl,nbl,clxbz from dl$ywzl");

    if(nId<0)  return -1;
    
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%ld%s%s%lf%ld%s",&nYwbh,sYwmc,sBz,&dBl,&nBl,sLxbz);
    	if(nRetVal==0) break;
    	if(nRetVal<0)  return -1;
    	ImportData(pdt,pdt->nCount+1,"copt,nywbh,cywmc,cbz,nbl,dbl,clxbz",
    	               "+",nYwbh,sYwmc,sBz,nBl,dBl,sLxbz);
    }
    CommitWork();
    CloseCursor(nId);
    return 0;
}

//////////////////////公司信息管理//////////////////////////////

void CompanyManage()
{
    DATATABLE * pdt;//录入界面数据表格对象
    long nKey,nCurrent,nwhere,nModifyFlag;
    long nCompanybh;
    char sOpt[2],sGsmc[21],sBz[21];
    
    clrscr();           
     
    pdt=CreateDataTable(8,
    "|  |公司编号|公司名称            |备注                      |",
		"%-1.1s%-3d%-20.20s%-20.20s%d%f%s");
    if(!pdt)
    {
	MessageBox("建立数据表格出错...",1);
	return;
    }

    SetTableStyle(pdt,ST_ALLTRIM|ST_MESSAGELINE,1);
    AssignTableFields(pdt,"copt,ngsbh,cgsmc,cbz,nbl,dbl,clxbz");
    gotoxy(1,1);	
    outtext("                           委 托 公 司 管 理");
    ShowDataTable(pdt,8,3);
    if(LoadCompanyData(pdt))
    {
	 DropDataTable(pdt);
	 return;
    }
    //begin input
    nCurrent=1;//start with the first line
    nwhere=1;
    nModifyFlag=0;//the document is modified if the flag is nozero
    for(;;)
    {
	//auto append a new record
        if(AutoExtendTable(pdt,&nCurrent)>0) ImportData(pdt,nCurrent,"copt","+");

        switch(nwhere)
        {
            case 1:
            ShowDatatableInfo(pdt,"请输入功能控制，按M键显示功能键帮助...",0);
            break;
            case 2:
            ShowDatatableInfo(pdt,"请输入公司编号，不能与已有的编号重复...",0);
            break;

            case 3:
            ShowDatatableInfo(pdt,"请输入公司名称...",0);
            break;
            
            case 4:
            ShowDatatableInfo(pdt,"请输入相应备注信息(不必须)...",0);
            break;
        }
         

	//Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"copt,ngsbh,cgsmc,cbz");

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
                                ShowDataTable(pdt,8,3);
			        continue;
			}
			if(nKey==KEY_YES) SaveCompanyData(pdt);
		    }
		    DropDataTable(pdt);
	            return;
           default:
                    DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
                    continue;
	}

        //Export data from container which you inputed
	ExportData(pdt,nCurrent,"copt,ngsbh,cgsmc,cbz",sOpt,&nCompanybh,sGsmc,sBz);
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
					 nModifyFlag=SaveCompanyData(pdt);
					 keyb();
					 ShowDataTable(pdt,8,3);
				  } 
				  continue;
			 default:
			          DefaultTableChoice(pdt,sOpt[0],
  	                              &nCurrent,&nwhere,&nModifyFlag);
		    }
		    continue;
	   case 2:
                    nModifyFlag=1;
	            if(nCompanybh<1) continue;
	            nKey=FindDataTable(pdt,1,"ngsbh",nCompanybh);
	            if(nKey==nCurrent) nKey=FindDataTable(pdt,nCurrent+1,"ngsbh",nCompanybh);
	            if(nKey>0 && nKey!=nCurrent)
              	    {
                          ShowDatatableInfo(pdt,
                          "该公司编号已经存在，请重新输入。",1);
                          continue;
                    }
                    nwhere=3;
		    continue;
	   case 3:
      	            if(strlen(sGsmc)>=4) nwhere=4;
		    continue;
           case 4:
	            nwhere=2;
                    nCurrent++;
		    continue;
        }//end switch(nwhere);
   }//end for(;;)
}

static long SaveCompanyData(DATATABLE * pdt)
{
    long nCompanybh,nBl,nId,nRetVal;
    double dBl;
    char sGsmc[21],sBz[21],sLxbz[21];
    
    ShowDatatableInfo(pdt,"正在入库，请稍候...",0);
    nRetVal=RunSql("delete from dl$gs");
    if(nRetVal<0)
    {
        MessageBox("数据库操作失败",2);
    	RollbackWork();
    	return -1; 
    }

    nRetVal=1;
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	
    	ExportData(pdt,nId,"ngsbh,cgsmc,cbz,nbl,dbl,clxbz",&nCompanybh,sGsmc,sBz,&nBl,&dBl,sLxbz);
        alltrim(sGsmc);
        alltrim(sBz);
        if(sGsmc[0]=='.') continue;
        nRetVal=RunSql("insert into dl$gs(ngsbh,cGsmc,cbl,nbl,dbl,clxbz) \
                        values(%ld,%s,%s,%ld,%lf,%s)",
                        nCompanybh,sGsmc,sBz,nBl,dBl,sLxbz);
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

static long LoadCompanyData(DATATABLE * pdt)
{
    long nCompanybh,nBl,nId,nRetVal;
    double dBl;
    char sGsmc[21],sBz[21],sLxbz[21];

    nId=OpenCursor("select ngsbh,cGsmc,cbl,dbl,nbl,clxbz from dl$gs");

    if(nId<0)  return -1;
    
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%ld%s%s%lf%ld%s",&nCompanybh,sGsmc,sBz,&dBl,&nBl,sLxbz);
    	if(nRetVal==0) break;
    	if(nRetVal<0)  return -1;
    	ImportData(pdt,pdt->nCount+1,"copt,ngsbh,cgsmc,cbz,nbl,dbl,clxbz",
    	               "+",nCompanybh,sGsmc,sBz,nBl,dBl,sLxbz);
    }
    CommitWork();
    CloseCursor(nId);
    return 0;
}
