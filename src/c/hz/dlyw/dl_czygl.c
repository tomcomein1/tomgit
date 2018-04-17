/*=======================================================
filename:dl_czygl.c
target:代理系统操作员管理
orginal programmer:chenbo 陈博
*======================================================*/

#include<string.h>
#include"dlpublic.h"
#include"tools.h"
#include"setting.h"
#include"query.h"

extern long nSysCzyqx;
extern char sSysJh[10];
extern char sSysJsdh[10];
static long SaveUserData(DATATABLE * pdt);
static long LoadUserData(DATATABLE * pdt);

//////////////////////支局信息管理//////////////////////////////
void UserManage()
{
    DATATABLE * pdt;//录入界面数据表格对象
    long nCurrent,nwhere,nModifyFlag,nKey;
    char sOpt[2];

    char sCzydh[7],sCzymc[10],sCzyqx[13],
         sJsdh[10],sBz[13],sPassword[9];
    long nBl,nFlag,nJhbh,nCzyqx;
    double dBl;
    clrscr();
    if(nSysCzyqx==0)
    {
          pdt=CreateDataTable(8,
          "|  |工号  |姓名    |权限|权限名称    |县局号|代理局所号|备注        |",
	  "%-1.1s%-6.6s%-8.8s%-1d%-12.12s%-3d%-9.9s%-12.12s%d%s%d%f%s");
    }
    else
    {
          pdt=CreateDataTable(8,
          "|  |工号  |姓名    |权限|权限名称    |支局号|代理局所号|备注        |",
	  "%-1.1s%-6.6s%-8.8s%-1d%-12.12s%-3d%-9.9s%-12.12s%d%s%d%f%s");
    }
    if(!pdt)
    {
	MessageBox("建立数据表格出错...",1);
	return;
    }

    AssignTableFields(pdt,
    "copt,cczydh,cczymc,nczyqx,cqxmc,nzjbh,cjsdh,cbz,nflag,cpassword,nbl,dbl,clxbz");
    SetTableStyle(pdt,ST_MESSAGELINE|ST_ALLTRIM,1);

    gotoxy(1,1);
    outtext("                           操 作 人 员 管 理");

    ShowDataTable(pdt,1,3);
    ShowDatatableInfo(pdt,"正在提取操作员信息，请稍候...  ",0);

    if(LoadUserData(pdt))
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
           ImportData(pdt,nCurrent,"copt,clxbz,nflag","+","00000000000000000000",0);

        switch(nwhere)
        {
            case 1:
            ShowDatatableInfo(pdt,"请输入功能控制，按M键显示功能键帮助...",0);
            break;
            case 2:
            ShowDatatableInfo(pdt,
            "请输入操作员工号，不能与原有工号重复。",0);
            break;
            case 3:
            ShowDatatableInfo(pdt,
            "请输入操作员姓名。",0);
            break;
            case 4:
            if(nSysCzyqx==0)
              ShowDatatableInfo(pdt,"1:系统维护",0);
            else 
              ShowDatatableInfo(pdt,"2:代理组长  3:业务操作员  4:查询统计  5:帐务  6:支局长",0);
            break;
            case 5:
            if(nSysCzyqx==0)
              ShowDatatableInfo(pdt,"请输入县局编号，或按回车查询。",0);
            else
              ShowDatatableInfo(pdt,"请输入支局编号，或按回车查询。",0);
            break;

        }

	//Fetch data into container
        if(nSysCzyqx==6 && nwhere>3 && nwhere<6) nwhere=3;

        nKey=FetchData(pdt,nCurrent,nwhere,"copt,cczydh,cczymc,nczyqx,nzjbh,cbz");

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
                                ShowDataTable(pdt,1,3);
			        continue;
			}
			if(nKey==KEY_YES) SaveUserData(pdt);
		    }
		    DropDataTable(pdt);
	            return;

           default:
                    DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
                    continue;
	}

        //Export data from container which you inputed
	ExportData(pdt,nCurrent,
	"copt,cczydh,cczymc,nczyqx,cqxmc,nzjbh,cjsdh,cbz,nflag,cpassword",
        sOpt,sCzydh,sCzymc,&nCzyqx,sCzyqx,&nJhbh,sJsdh,sBz,&nFlag,sPassword);
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
				     nModifyFlag=SaveUserData(pdt);
				     keyb();
				     ShowDataTable(pdt,1,3);
				 }
				 continue;
			     default:
			         DefaultTableChoice(pdt,sOpt[0],
			                      &nCurrent,&nwhere,&nModifyFlag);
			         continue;
		    }
		    break;

	   case 2:
                    upper(sCzydh);
                    if(strlen(sCzydh)<4) continue;

	            nKey=FindDataTable(pdt,1,"cczydh",sCzydh);
	            if(nKey==nCurrent) 
	               nKey=FindDataTable(pdt,nCurrent+1,"cczydh",sCzydh);

	            if(nKey>0)
	            {
                        ShowDatatableInfo(pdt,"该操作员工号已经存在，请重新输入",1);
                        continue;
                    }
                    if(RunSelect("select * from dl$czyb where cczydh=%s",
                       sCzydh)>0 && nFlag==0)
                    {
                          ShowDatatableInfo(pdt,
                          "该操作员工号已经存在，请重新输入",1);
                          continue;
                    }                     	
                    if(!nFlag)
                    {
                    	  strcpy(sCzymc,sCzydh);
                    	  strcpy(sPassword,sCzydh);
                    }
                    nwhere++;
                    break;
	   case 3:
                    nModifyFlag=1;
	            if(strlen(sCzymc)<4) continue;
	            if(nSysCzyqx==6)
	            {
	            	nwhere+=3;
	            	nCzyqx=3;
	            	strcpy(sCzyqx,"业务操作员");
	            	nJhbh=atol(sSysJsdh+6);
	            	strcpy(sJsdh,sSysJsdh);
	            	break;
	            }
	            nwhere++;
	            continue;
           case 4:
                    if(nSysCzyqx==0)
                    {
                      if(nCzyqx==1) strcpy(sCzyqx,"系统维护");
                      else continue;
                    }
                    else if(nSysCzyqx==1)
                    {
                      if(nCzyqx==3) strcpy(sCzyqx,"业务操作员");
                      else if(nCzyqx==2) strcpy(sCzyqx,"代理组长");
                      else if(nCzyqx==4) strcpy(sCzyqx,"查询统计");
                      else if(nCzyqx==5) strcpy(sCzyqx,"帐务");
                      else if(nCzyqx==6) strcpy(sCzyqx,"支局长");
                      else continue;
                    }
                    nwhere++;
                    break;
           case 5:
                    if(nSysCzyqx==0)
                    {
                       sprintf(sJsdh,"%-4.4s%02ld000",sSysJh,nJhbh);
                       if(RunSelect("select * from dl$jh where cjh=%s and nlb in(2,3)",sJsdh)<=0)
                       {
                          savewin();
                       	  QueryLocal("  县局编号   代理局号       局名             ", 
                       	  "%ld%s%s",
                       	  "select to_number(substr(cjh,5,2)),cjh,cjm from dl$jh \
                       	  where substr(cjh,1,4)=substr(%s,1,4) and nlb in(2,3)",sSysJh);
                          popwin();
                          continue;
                       }
                    }
                    else if(nSysCzyqx==1)
                    {
                       
                       if(nCzyqx==3 || nCzyqx==6)
                       {
                         if(nJhbh==0) continue;
                       }
                       else if(nJhbh!=0) continue;
                       
                       sprintf(sJsdh,"%-6.6s%03ld",sSysJsdh,nJhbh);
                       if(RunSelect("select * from dl$jh where cjh=%s",sJsdh)<=0)
                       {
                          savewin();
                       	  QueryLocal("  支局编号   代理局号       局名             ",
                       	  "%ld%s%s",
                       	  "select to_number(substr(cjh,7,3)),cjh,cjm from dl$jh \
                       	  where substr(cjh,1,6)=substr(%s,1,6) and nlb=4",sSysJsdh);
                          popwin();
                          continue;
                       }
                    }
                    nwhere++;
                    break;
           case 6:
                    nwhere=2;
                    nCurrent++;
                    continue;         
        }//end switch(nwhere);
	ImportData(pdt,nCurrent,"cczydh,nczyqx,cqxmc,nzjbh,cjsdh,cczymc,cpassword",
	           sCzydh,nCzyqx,sCzyqx,nJhbh,sJsdh,sCzymc,sPassword);
   }//end for(;;)
}

static long SaveUserData(DATATABLE * pdt)
{
    long nId,nRetVal,nJh,nCzyqx,nBl;
    char sCzydh[7],sCzymc[9],sJsdh[10],sBz[13],sLxbz[21],sPassWord[9];
    double dBl;

    if(nSysCzyqx==0)
       nRetVal=RunSql("delete dl$czyb where nczyqx=1");
    else if(nSysCzyqx==1)    
       nRetVal=RunSql("delete dl$czyb where nczyqx in(2,3,4,5,6) and \
                      substr(cjh,1,6)=substr(%s,1,6)",sSysJsdh);
    else
       nRetVal=RunSql("delete dl$czyb where nczyqx=3 and cjh=%s",sSysJsdh);
    

    if(nRetVal<0)
    {
    	RollbackWork();
        HiError(nRetVal,"数据库操作失败");
    	return -1;
    }
    
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	ExportData(pdt,nId,
  	    "cczydh,cczymc,nczyqx,cjsdh,cbz,cpassword,nbl,dbl,clxbz",
  	     sCzydh,sCzymc,&nCzyqx,sJsdh,sBz,sPassWord,&nBl,&dBl,sLxbz);
    	if(sJsdh[0]=='.') continue;
        nRetVal=RunSql("insert into dl$czyb(cczydh,cczymc,nczyqx,\
                        cpassword,cjh,cbl,nbl,dbl,clxbz) \
                        values(%s,%s,%ld,%s,%s,%s,%ld,%lf,%s)",
                        sCzydh,sCzymc,nCzyqx,sPassWord,sJsdh,sBz,nBl,dBl,sLxbz);
        if(nRetVal<=0) break;
    }
    if(nRetVal<=0)
    {
    	RollbackWork();
        HiError(nRetVal,"数据库操作失败");
    	return -1;
    }
    else
    {
    	CommitWork();
        ShowDatatableInfo(pdt,"保存完毕...",0);
        return 0;
    }

}

static long LoadUserData(DATATABLE * pdt)
{

    char * sCzyqx[7]={"超级用户","系统维护","代理组长",
                      "业务操作员","统计","帐务","支局长"};

    long nId,nRetVal,nJh,nCzyqx,nBl;
    char sCzydh[7],sCzymc[9],sJsdh[10],sBz[13],sLxbz[21],sPassWord[9];
    double dBl;

    if(nSysCzyqx==0)
      nId=OpenCursor("select cczydh,cczymc,nczyqx,cpassword,cjh,cbl,nbl,dbl,clxbz \
                     from dl$czyb where nczyqx=1");
    else if(nSysCzyqx==1)
      nId=OpenCursor("select cczydh,cczymc,nczyqx,cpassword,cjh,cbl,nbl,dbl,clxbz \
                     from dl$czyb where nczyqx in(2,3,4,5,6) and substr(cjh,1,6)=\
                     substr(%s,1,6)",sSysJsdh);
    else
      nId=OpenCursor("select cczydh,cczymc,nczyqx,cpassword,cjh,cbl,nbl,dbl,clxbz \
                     from dl$czyb where nczyqx=3 and cjh=%s",sSysJsdh);

    if(nId<0)  return -1;

    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%s%s%ld%s%s%s%ld%lf%s",
    	               sCzydh,sCzymc,&nCzyqx,sPassWord,
    	               sJsdh,sBz,&nBl,&dBl,sLxbz);
    	               
    	if(nRetVal==0) break;
    	if(nRetVal<0)  return -1;
        
        if(nSysCzyqx==0)
           nJh=(sJsdh[4]-'0')*10+(sJsdh[5]-'0');
        else           
           nJh=(sJsdh[6]-'0')*100+(sJsdh[7]-'0')*10+(sJsdh[8]-'0');
    	
    	ImportData(pdt,pdt->nCount+1,
        "copt,cczydh,cczymc,nczyqx,cqxmc,nzjbh,cjsdh,cbz,nflag,cpassword,nbl,dbl,clxbz",
    	 "+",sCzydh,sCzymc,nCzyqx,sCzyqx[nCzyqx],nJh,
    	 sJsdh,sBz,1,sPassWord,nBl,dBl,sLxbz);
    }
    CommitWork();
    CloseCursor(nId);
    return 0;
}

