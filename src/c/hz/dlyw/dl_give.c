/***********************************************************************
**filename	文件名:dl_give.c
**target	目的:代售系统中货物的分发
**create time	生成日期: 2001.3.19
**original programmer	编码员:陈博[chenbo]
**edit  history	修改历史:
**date     editer    reson
** 
************************************************************************/

#include "dlpublic.h"
#include "dl_buy.h"
#include "dl_give.h"
#include "tasktool.h"

extern char sSysJh[10];
extern char sSysJsdh[10];
extern char sSysCzydh[7];
extern long nSysLkrq;
extern long nSysSfm;
extern char sTcpipErrInfo[81];
extern char sSysSyz[21];

void DistributeGoods()
{
    DATATABLE * pdt;//录入界面数据表格对象
    MENU * pmsp,* pmczy,* pmident;//货品名称，公司名称，货物标识
    char sOpt[3],sJym[16],sJymc[51],sIdent[19],sAccepter[21],smsg[51];
    char sTemp[21];
    long nCount,nCurrent,nId,nKey,npx,npy,identify,nsum,nsl,nRec;
    long nwhere,nModifyFlag;
    double dPrice;
    

    // initialize
    GetSysLkrq();
    sprintf(sSysSyz,"%s.%s",sSysJsdh,sSysCzydh);

    clrscr();

    pdt=CreateDataTable(8,
          "|  |货品名称                |数量|货品标识            |接货人          |",
		"%-1.1s%-24.24s%-4d%-16.16s%-16.16s%s%d");
    if(!pdt)
    {
	MessageBox("建立数据表格出错...",1);
	return;
    }

    MergeTable(pdt->pTable,CreateTable(1,
          "|                                                                          |"));

    
    gotoxy(1,1);
    outtext("                          货物进出管理(发  货)");
    ShowDataTable(pdt,1,3);
    
    ShowDatatableInfo(pdt,"正在提取本操作员的发货信息，请稍候...  ",0);
    pmsp=CreateGoodsMenu();         //商品名称帮助菜单
    pmczy=CreateCzyMenu();

    nKey=pmsp&&pmczy;
    if(nKey) nKey=(!LoadDistrubuteData(pdt));
    if(!nKey)
    {
	 DropMenu(pmsp);
	 DropMenu(pmczy);
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
	switch(nwhere)
	{
	    case 1:
            ShowDatatableInfo(pdt,
            "A:增加  -:删除  S:保存  H:最前  E:最后  U:上翻  D:下翻  I:入库存  ESC:返回",0);
	    break;
	
	}
	
	if(nCurrent>pdt->nCount)
        {
            if(pdt->nCount) ExportData(pdt,pdt->nCount,"> %s",sJymc);
	    if(!strchr(sJymc,'.')||(!pdt->nCount))
		ImportData(pdt,nCurrent,"%s%s%d%s%s%s%d","+",".",0,".",".",".",0);
            nwhere=1;
            nCurrent=pdt->nCount;
	}
        

	//Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"%-1.1s%-24.24s%-4d%-16.16s%-16.16s");
        //Export data from container which you inputed
	ExportData(pdt,nCurrent,"%s%s%d%s%s%s%d",
	               sOpt,sJymc,&nCount,sIdent,sAccepter,sJym,&identify);
        upper(sAccepter);
	ImportData(pdt,nCurrent,"%s > > > %s","+",sAccepter);
	
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
				nKey=MessageBox("您的发货记录没有保存，要保存吗？",0);
				if(nKey==KEY_ESC)
				{
                                        ShowDataTable(pdt,1,3);
				        continue;
				}
				if(nKey==KEY_YES) SaveDistributeData(pdt);
			    }
			    DropMenu(pmsp);
                            DropMenu(pmczy);
			    DropDataTable(pdt);
	                    GetSysLkrq();
			    return;
		   default:continue;
		}

		switch(nwhere)
		{
		   case 1: //操作方法
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
							 nModifyFlag=SaveDistributeData(pdt);
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
                                                 nModifyFlag=CommitDistributeData(pdt);
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
			    break;

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

                                 //检查该商品是否需要唯一标识
                                 RunSelect("select to_number(substr(clxbz,2,1)) from \
                                            dl$jym where cjym=%s into %ld",sJym,&identify);
			         
			         ImportData(pdt,nCurrent,"> %s %d > > %s%d",
			                                    sJymc,identify,sJym,identify);
			    
			    }

                            nsum=0;
                            RunSelect("select nkcl from dl$hwkcb where cjym=%s and csyz=%s into %ld",
                            sJym,sSysSyz,&nsum);
                            sprintf(smsg,"该类商品的库存量为:%ld",nsum);
                            ShowDatatableInfo(pdt,smsg,0);
                            nwhere=3;
			    continue;

		   case 3:
                            ShowDatatableInfo(pdt,
                            "                                    ",0);
                            nRec=nsum;
                            for(nId=1;nId<=pdt->nCount;nId++)
                            {
                                      if(nId==nCurrent) continue;
                                      ExportData(pdt,nId,"> > %d > > %s",&nsl,sTemp);
                                      alltrim(sTemp);
                                      if(!strcmp(sTemp,sJym)) nRec-=nsl;
                            }
                            if(nRec<nCount||nRec<=0)
                            {
                                      sprintf(smsg,
                                      "对不起！该类商品的库存不足;库存量为：%ld",nsum);
                                      ShowDatatableInfo(pdt,smsg,1);
                                      continue;
                            }
			    if(identify) nwhere=4;//next field
			    else nwhere=5;
                            continue;    
		   case 4:
                            nRec=0;
                            nsl=0;
                            alltrim(sIdent);
                            //启动帮助菜单
                            if(sIdent[0]=='.')
                            {
                                ShowDatatableInfo(pdt,"正在提取数据\n请稍候... ",0);
                                pmident=CreateIdentMenu(sJym);
                                ShowDatatableInfo(pdt,"                                   ",0);
                                if(pmident)
                                {
      		                     nId=PopMenu(pmident,35,3,20);
				     ShowDataTable(pdt,1,3);
				     if(nId<=0) continue;
			        }
			        else continue;
			        strcpy(sIdent,GetMenu(pmident,nId));
			        DropMenu(pmident);
			        ImportData(pdt,nCurrent,"> > > %s",sIdent);
			        nsl=1;
			    }   
                            nRec=0;
                            for(nId=1;nId<=pdt->nCount;nId++)
                            {
                            	if(nId==nCurrent) continue;
                            	ExportData(pdt,nId," > > > %s > %s",sTemp,sJymc);
                            	alltrim(sTemp);
                            	alltrim(sJymc);
                            	if(!strcmp(sJymc,sJym))
                            	    if(nRec=(strcmp(sTemp,sIdent)==0)) break;
                            }

                            if(nRec)
                            {
                            	ShowDatatableInfo(pdt,"该商品标识已经分配!",1);
                            	continue;
                            }
                            
                            if(nsl==0)
                            {
                                  nCount=0;
                                  RunSelect("select count(*) from dl$hwjcb \
                                    where cjym=%s and chwbz=%s and ((nczlb=1 and njlzt=1) or \
                                    (nczlb=3 and njlzt=2)) into %ld",sJym,sIdent,&nCount);
                                  if(nCount<=0)
                                  {
                            	        ShowDatatableInfo(pdt,"该商品标识已经分配或没有该商品标识！",1);
                            	        continue;
                            	  }      
                            }
			    nwhere=5;
			    continue;

		   case 5:
                            alltrim(sAccepter);
                            if(IsInMenu(pmczy,sAccepter,0)<=0)
                            {
		                 nId=0;
                                 nId=atol(sAccepter);
			         if(nId<=0||nId>pmczy->nTotal)
			         {
				     getxy(&npx,&npy);
				     nId=PopMenu(pmczy,45,npy,12);
				     ShowDataTable(pdt,1,3);
				     if(nId<=0) continue;
			         }
			         strcpy(sAccepter,GetAddition(pmczy,nId));
			         ImportData(pdt,nCurrent,"> > > > %s",sAccepter);
			    }
                            if(!strcmp(sAccepter,sSysSyz)) continue;
			    nCurrent++;
			    nModifyFlag=1;
			    continue;
	           		    
	}//end switch(nwhere);
   }//end for(;;)
}


static long CommitDistributeData(DATATABLE * pdt)
{
    RECORD rec;
    long nRetVal;
    char sbuf[81];
    
    
    if(MessageBox("    一旦发货记录入库存，将再也不能修改这些发货记录，要继续吗？",0)!=KEY_YES) return 1;
    if(SaveDistributeData(pdt))
    {
    	keyb(); 
    	return 1;
    }
    ShowDatatableInfo(pdt,"正在提交，请等待...",0);
    
    nRetVal=AutoRunTask("dldj",TASK_COMMIT_DISTRIBUTE,"%s","%r",sSysSyz,&rec);
    if(nRetVal) return -1;
    
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
    ShowDatatableInfo(pdt,"发货记录入库完毕...",0);
    return 0;

}
static long SaveDistributeData(DATATABLE * pdt)
{

    char sJym[16],sJymc[51],sLsh[41],sAccepter[21];
    char cLxbz[21],sIdent[21],sjhjh[10];
    long nCount,nId,nRetVal,identify;
    
    nRetVal=RunSql("delete from dl$hwjcb where nczlb=3 and njlzt=0 and \
                                cfhczydh=%s",sSysSyz);
    if(nRetVal<0)
    {
        MessageBox("数据库操作失败",2);
    	RollbackWork();
    	return -1; 
    }

    nRetVal=1;
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	
    	ExportData(pdt,nId,"> %s%d%s%s%s%d",
    	           sJymc,&nCount,sIdent,sAccepter,sJym,&identify);

        alltrim(sIdent);
        alltrim(sAccepter);
        alltrim(sJymc);
        alltrim(sJym);
        
        if(strchr(sJymc,'.')) break;
        
        GenerateLsh(1,sSysJsdh,sJym,sLsh);

        strcpy(cLxbz,"00000000000000000000");
        if(identify) cLxbz[0]='1';

        strncpy(sjhjh,sAccepter,9);
        sjhjh[9]='\0';
        nRetVal=RunSql("insert into dl$hwjcb(cjym,cjymc,clsh,djhdj,nsl,chwbz,\
                        cfhczydh,cjhczydh,cfhjh,cjhjh,njhrq,njhsfm,nfhrq,nfhsfm,\
                        nczlb,njlzt,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,clxbz) \
                        values(%s,%s,%s,0,%ld,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,\
                        3,0,0,0,0,0,'0','0',%s)",
                        sJym,sJymc,sLsh,nCount,sIdent,sSysSyz,sAccepter,sSysJsdh,
                        sjhjh,nSysLkrq,nSysSfm,nSysLkrq,nSysSfm,cLxbz);
        if(nRetVal<0) break;                        
    }

    if(nRetVal<0)
    {
    	RollbackWork();
        MessageBox("数据库操作失败...",2);
    	return -1;
    }
    else
    {
    	CommitWork();
        ShowDatatableInfo(pdt,"发货记录保存完毕...",0);
        return 0;
    }


}

static long LoadDistrubuteData(DATATABLE * pdt)
{
    char sJym[16],sJymc[51],sIdent[21],sAccepter[21];
    long nCount,nId,identify;

    nId=OpenCursor("select cjym,cjymc,chwbz,nsl,cjhczydh,\
                    to_number(substr(clxbz,1,1)) from dl$hwjcb \
                    where nczlb=3 and njlzt=0 and cfhczydh=%s \
                    order by njhsfm",sSysSyz);

    if(nId<0)  return -1;
    
    for(;;)
    {
    	if(FetchCursor(nId,"%s%s%s%ld%s%ld",sJym,sJymc,sIdent,
    	                   &nCount,sAccepter,&identify)<=0) break;
    	
    	ImportData(pdt,pdt->nCount+1,"%s%s%d%s%s%s%d","+",sJymc,nCount,
    	               sIdent,sAccepter,sJym,identify);
    }
    CloseCursor(nId);
    return 0;
}

MENU * CreateCzyMenu()
{
    
    char sczydh[7],sczymc[10],sTemp[21],sJh[10],sSyz[21];
    long nId;
    MENU * pmh;

    if(!(pmh=CreateMenu("CZYDH",0))) return NULL;

    if((nId=OpenCursor("select distinct cczydh,cczymc,cjh from dl$czyb \
                        where nczyqx in(2,3,6)"))<=0)
    {
    	DropMenu(pmh);
    	return NULL;
    }
                                        

    for(;;)
    {
	if(FetchCursor(nId,"%s%s%s",sczydh,sczymc,sJh)<=0) break;
        sprintf(sSyz,"%s.%-6.6s",sJh,sczydh);
        sprintf(sTemp,"%15s  %s",sSyz,sczymc); 
	
	AddMenu(pmh,sTemp,sSyz);
    }
    CloseCursor(nId);
    return pmh;

}

MENU * CreateIdentMenu(char * sJym)
{
    MENU * pmh;
    char sIdent[21];
    long nId;
                            	
    if(!(pmh=CreateMenu("HWBZ",0))) return NULL;
    nId=OpenCursor("select distinct chwbz from dl$hwjcb where cjym=%s \
                    and cjhczydh=%s and ((nczlb=1 and njlzt=1) or \
                    (nczlb=3 and njlzt=2))",sJym,sSysSyz);
    if(nId<0)
    {
    	DropMenu(pmh);
    	return NULL;
    }

    for(;;)
    {
        if(FetchCursor(nId,"%s",sIdent)<=0) break;
        AddMenu(pmh,sIdent,NULL);
    }
    CloseCursor(nId);
    return pmh;
}




