/***********************************************************************
**filename	文件名:dl_sell.c
**target	目的:代售系统中货物的销售及客户退货
**create time	生成日期: 2001.3.19
**original programmer	编码员:陈博[chenbo]
**edit  history	修改历史:
**date     editer    reson
** 
************************************************************************/
#include"dlpublic.h"
#include"query.h"
#include"tools.h"
#include"dl_buy.h"
#include"dl_give.h"
#include"tasktool.h"

extern long nSysSfm;
extern long nSysLkrq;
extern char sSysJsdh[10];
extern char sSysCzydh[7];
extern char sTcpipErrInfo[81];
extern char sSysJm[JM_JM_LEN+1];/*局名  cs2*/
extern char sSysSyz[21];

long nPrintDssj(char *sLsh);
void SellGoods();
void RecallGoods();
void WastingGoods();

void SellGoods()
{
      
     DIALOG * pd;
     RECORD rec;
     MENU * pmsp,* pmident;
     char sJym[16],sJymc[51],sHwbz[21],sInfo[81],sLsh[41];
     char sLicence[31],sUserName[31],sAddress[51],sbuf[256];
     long nCount,nKey,nwhere,nId,identify;
     long detail,nfinished,nsum,nTest,nRetVal;
     double dMoney=0.0,dPrice=0.0;

     nRetVal=0;
     sprintf(sSysSyz,"%s.%s",sSysJsdh,sSysCzydh);
     RunSelect("select count(*) from dl$hwjcb where \
                     ((nczlb=3 and njlzt=0) or (nczlb=2 and njlzt=0)) \
                     and cfhczydh=%s into %ld",sSysSyz,&nRetVal);
     
     if(nRetVal>0)
     {
     	clrscr();
     	if(MessageBox("    您尚有发货或退货记录没有正式入库，请先将它们正式入库后才能销售！\
以免引起库存混乱。要查看发货或退货明细吗?",0)==KEY_YES)
        {
        	QueryLocal( "商品名称               货品标识        类别    数量  接货操作员        日期   ",
        	            "%s%s%ld[3:发货;2:退货;]%ld%s%ld",
        	            "select cjymc,chwbz,nczlb,nsl,cjhczydh,nfhrq from dl$hwjcb \
        	             where  ((nczlb=3 and njlzt=0) or (nczlb=2 and njlzt=0)) \
                             and cfhczydh=%s",sSysSyz);
        }                     
     	return;
     }
    
     ///////////////////////////////////////////////////////////////////     
     pmsp=CreateGoodsMenu();
     if(!(pd=CreateDialog(60,21,FRAME))) return;
     AddDialog(pd,"static",STATIC_TEXT,"                 〖  商  品  代  售  〗\n");
     AddDialog(pd,"set1",SEPARATOR,"__\n");
     
     AddDialog(pd,"cspmc",INPUT,"商品名称:%-46.46s\n\n",sJymc);           //3
     AddDialog(pd,"nspsl",INPUT,"商品数量:%8d",&nCount);                  //4
     AddDialog(pd,"cspbs",INPUT,"         商品标识:%-20.20s\n",sHwbz);    //5
     
     AddDialog(pd,"sep2",SEPARATOR,"__\n");

     AddDialog(pd,"cyhmc",INPUT,"用户名称:%-26.26s\n\n",sUserName);        //7
     AddDialog(pd,"czjhm",INPUT,"证件号码:%-30.30s\n\n",sLicence);         //8
     AddDialog(pd,"cyhdz",INPUT,"用户地址或联系方式:%-24.24s\n",sAddress); //9

     AddDialog(pd,"sep3",SEPARATOR,"__");
     AddDialog(pd,"dspdj",OUTPUT,"商品单价:%10.2f",&dPrice);
     AddDialog(pd,"dyszk",OUTPUT,"        应收总款:%10.2f",&dMoney);
     //////////////////////////////////////////////////////////////////
     clrscr();
     ShowDialog(pd,10,2);
     for(;;)
     {
     	nwhere=1;   //start
        dMoney=0.0;
        nCount=0;
        nsum=0;
        detail=0;  //录入用户信息
        identify=0;//录入货品标识
        nfinished=0;
        strcpy(sJymc,".");
        strcpy(sHwbz,".");
        strcpy(sUserName,".");
        strcpy(sLicence,".");
        strcpy(sAddress,".");
        for(;;)
        {
                if(nfinished) break;
                RefreshDialog(pd,0);
                switch(nwhere)
                {
                   case 1:
                        SetDialogInfo(pd,"请输入商品名称，或按回车键选择商品..");
                        break;

                   case 2:
                        sprintf(sInfo,"请输入商品数量，目前库存量为:%ld",nsum);
                        SetDialogInfo(pd,sInfo);
                        break;

                   case 3:
                        SetDialogInfo(pd,"请输入货品标识，或按回车键选择货品标识..");
                        break;

                   case 4:
                   case 5:
                   case 6:
                        SetDialogInfo(pd,"请输入客户的个人相关信息...");
                        break;


                }
        	nKey=ActiveDialog(pd,nwhere,"cspmc,nspsl,cspbs,cyhmc,czjhm,cyhdz");

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
        	if(nKey==KEY_ESC)
        	{
        	   if(nwhere==1) break;
        	   nwhere=1;
        	   continue;
        	}
        	
                switch(nwhere)
                {
                   case 1:
                        nId=0;
                        alltrim(sJymc);
                        nId=atol(sJymc);
                        if(nId<=0||nId>pmsp->nTotal)
                        {
                             savewin();
                             nId=PopMenu(pmsp,20,8,16);
                             popwin();
                             if(nId<0)
                             {
  	                             RefreshDialog(pd,0);
                                     continue;
                             }
                                
                        }     
                        strcpy(sJymc,GetMenu(pmsp,nId));
                        strcpy(sJym,GetAddition(pmsp,nId));
                        RefreshDialog(pd,0);

                        SetDialogInfo(pd,"正在查询该商品的库存，请稍候...");
                        RunSelect("select nkcl from dl$hwkcb where \
                                   cjym=%s and csyz=%s into %ld",sJym,sSysSyz,&nsum);
                        if(nsum<=0)
                        {
                              SetDialogInfo(pd,"该商品已无库存，不能销售...");
                              keyb();
                              continue;
                        }

                        RunSelect("select to_number(substr(clxbz,1,1)),\
                                   to_number(substr(clxbz,2,1)) from dl$jym \
                                   where cjym=%s into %ld%ld",sJym,&detail,&identify);
                        RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=1 \
                                   and cjym=%s into %lf",sJym,&dPrice);
                        
                        nwhere=2;
                        if(identify)
                        {
                            nCount=1;
                            nsum=1;
                            nwhere=3;
                        }
                        continue;

                   case 2:
                        if(nCount<1||nCount>nsum) continue;
                        if(identify) nwhere=5;
                        else nfinished=1;   //input finished
                        continue;

                   case 3:
                        alltrim(sHwbz);
                        nTest=0;
                        SetDialogInfo(pd,"正在检查商品标识，请稍候...");

                        if(sHwbz[0]!='.')  
                           RunSelect("select count(*) from dl$hwjcb \
                                         where cjym=%s and cjhczydh=%s and chwbz=%s \
                                         and ((nczlb=1 and njlzt=1) or \
                                         (nczlb=3 and njlzt=2)) into %ld",
                                         sJym,sSysSyz,sHwbz,&nTest);
                        if(nTest<=0||sHwbz[0]=='.')
                        {                 
                             SetDialogInfo(pd,"商品标识不存在！正在形成商品标识菜单，请稍候...");
                             if(!(pmident=CreateIdentMenu(sJym))) continue;
                             SetDialogInfo(pd,"请从菜单中选择一个商品标识，或按相应数字键...");
                             savewin();
                             nId=PopMenu(pmident,46,10,14);
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
                        if(!detail) nfinished=1;
                        else nwhere=4;
                        continue;
                   
                   case 4:
                        nwhere=5;
                        continue;

                   case 5:
                        nwhere=6;
                        continue;

                   case 6:
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
        RefreshDialog(pd,"dyszk");
        
        SetDialogInfo(pd,"请选择缴费方式：1.现金 2.绿卡 3.转帐支票 ESC放弃...");
        
        for(;;)
        {
        	nKey=keyb();
        	if(nKey==KEY_ESC||nKey=='1'||nKey=='2'||nKey=='3') break;
        }
        if(nKey==KEY_ESC) continue;
        //产生流水号
        GenerateLsh(1,sSysJsdh,sJym,sLsh);
        
        //开始缴款
//      流水号|用户标志|用户名称|缴款方式|实缴总款额|操作员代号|时分秒|缴款月份数|年度|月份|年度|月份|......
        
        alltrim(sHwbz);
        alltrim(sUserName);
        alltrim(sAddress);
        alltrim(sLicence);
        
        sprintf(sInfo,"%s@%s@%s",sLicence,sUserName,sAddress);
        sInfo[50]='\0';
        
        sprintf(sbuf,"%s|%s|%s|%ld|%.2lf|%s|%ld|%ld|",sLsh,sHwbz,sInfo,
                                    nKey-'0',dMoney,sSysCzydh,nSysSfm,nCount);

        //数据源缴款
        SetDialogInfo(pd,"正在向数据源缴费,请稍候...");
        nRetVal=AutoRunTask("dldj",TASK_PAY_CONFIRM,"%s","%r",sbuf,&rec); 
        if(nRetVal)
        {
        	SetDialogInfo(pd,sTcpipErrInfo);
        	keyb();
        	continue;
        }
        memset(sbuf,0,sizeof(sbuf));
        ExportRecord(&rec,"%s",sInfo);
        if(strncmp(sInfo,"000000",6))
        {
        	ExportRecord(&rec,"%s",sbuf);
        	DropRecord(&rec);
        	SetDialogInfo(pd,sbuf);
        	keyb();
        	continue;
        }

        //SetDialogInfo(pd,"正在修改库存量,请稍候...");
        nRetVal=AutoRunTask("dldj",TASK_SELL_GOODS,"%s%s%s%ld","%r",
                                        sLsh,sSysSyz,sHwbz,nCount,&rec);

        if(nRetVal)
        {
        	SetDialogInfo(pd,sTcpipErrInfo);
        	keyb();
        }
        else
        {
                ExportRecord(&rec,"%s",sInfo);
                if(strncmp(sInfo,"000000",6))
                {
        	      ExportRecord(&rec,"%s",sbuf);
        	      DropRecord(&rec);
        	      SetDialogInfo(pd,sbuf);
        	      keyb();
        	}      
                SetDialogInfo(pd,"请准备好打印机，按'1'键开始打印单据,ESC返回...");
                for(;;)
                {
                	nKey=keyb();
                	if(nKey==KEY_ESC||nKey=='1') break;
                }
                if(nKey=='1')  nPrintDssj(sLsh);
                continue;  //successfully

        }

        //修改库存不成功的处理,向数据源撤销本次缴款
        SetDialogInfo(pd,"正在撤消本次缴费，请等待...");
        GenerateLsh(1,sSysJsdh,sJym,sInfo);//生成撤单流水号 
    
        // 交易流水号|被撤单流水号|实缴款额|操作员代号|时分秒|
        sprintf(sbuf,"%s|%s|%s|%ld|",sInfo,sLsh,sSysCzydh,nSysSfm);
        nRetVal=AutoRunTask("dldj",TASK_RECALL_CONFIRM,"%s","%r",sbuf,sbuf); 
        if(nRetVal) 
        {
                SetDialogInfo(pd,sTcpipErrInfo);
                keyb();
                continue;
        }
       	if(strncmp(sbuf,"000000",6))
       	{
       		SetDialogInfo(pd,sbuf+7);
       		keyb();
       	}
     }   
}


void RecallGoods()
{
      
     DIALOG * pd;
     RECORD rec;
     MENU * pmsp;
     char sJym[16],sJymc[51],sHwbz[21],sInfo[81],sLsh[41],sbuf[256];
     long nCount,nKey,nwhere,nId,identify;
     long nfinished,nTest,nRetVal;
     double dMoney=0.0,dPrice=0.0;

     sprintf(sSysSyz,"%s.%s",sSysJsdh,sSysCzydh);

     ///////////////////////////////////////////////////////////////////     
     pmsp=CreateGoodsMenu();
     if(!(pd=CreateDialog(60,15,FRAME))) return;
     AddDialog(pd,"static",STATIC_TEXT,"                 〖  客  户  退  货  〗\n");
     AddDialog(pd,"sep1",SEPARATOR,"__\n");
     
     AddDialog(pd,"cspmc",INPUT,"商品名称:%-46.46s\n\n",sJymc);           //3
     AddDialog(pd,"nspsl",INPUT,"商品数量:%8d",&nCount);                  //4
     AddDialog(pd,"cspbs",INPUT,"         商品标识:%-20.20s\n",sHwbz);    //5
     
     AddDialog(pd,"sep2",SEPARATOR,"__\n");

     AddDialog(pd,"dspdj",OUTPUT,"商品单价:%10.2f",&dPrice);
     AddDialog(pd,"dytzk",OUTPUT,"        应退总款:%10.2f",&dMoney);
     //////////////////////////////////////////////////////////////////
     clrscr();
     
     for(;;)
     {
     	nwhere=1;//star
        dMoney=0.0;
        nCount=0;
        identify=0;//录入货品标识
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
                        SetDialogInfo(pd,"请输入商品名称，或按回车键选择商品..");
                        break;

                   case 2:
                        SetDialogInfo(pd,"请输入退货数量...");
                        break;

                   case 3:
                        SetDialogInfo(pd,"请输入货品标识...");
                        break;


                }
                	    
        	nKey=ActiveDialog(pd,nwhere,"cspmc,nspsl,cspbs");

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
                        RunSelect("select to_number(substr(clxbz,2,1)) from dl$jym \
                                   where cjym=%s into %ld%lf",sJym,&identify);

                        RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=1 \
                                   and cjym=%s into %lf",sJym,&dPrice);
                                   
                        nwhere=2;
                        if(identify)
                        {
                            nCount=1;
                            nwhere=3;
                        }
                        RefreshDialog(pd,0);
                        continue;

                   case 2:
                        if(nCount<1) continue;
                        if(identify) nwhere=5;
                        else nfinished=1;   //input finished
                        continue;

                   case 3:
                        alltrim(sHwbz);
                        nTest=0;
                        SetDialogInfo(pd,"正在检查商品标识，请稍候...");

                        RunSelect("select count(*) from dl$hwjcb \
                                        where cjym=%s and chwbz=%s and nczlb=7 into %ld",
                                        sJym,sHwbz,&nTest);
                        if(nTest<=0)
                        {                 
                             SetDialogInfo(pd,"不存在该商品的销售记录...");
                             keyb();
                             continue;
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
        
        SetDialogInfo(pd,"按'1'键确认退货，ESC键返回...");
        
        for(;;)
        {
        	nKey=keyb();
        	if(nKey==KEY_ESC||nKey=='1') break;
        }
        if(nKey==KEY_ESC) continue;
        //产生流水号
        GenerateLsh(1,sSysJsdh,sJym,sLsh);
        
        //开始缴款
//      流水号|用户标志|用户名称|缴款方式|实缴总款额|操作员代号|时分秒|缴款月份数|年度|月份|年度|月份|......
        
        alltrim(sHwbz);
        
        sprintf(sbuf,"%s|%s|RECALL|%ld|%.2lf|%s|%ld|%ld|",sLsh,sHwbz,
                                   nKey-'0',-dMoney,sSysCzydh,nSysSfm,-nCount);

        //数据源缴款
        SetDialogInfo(pd,"正在向数据源缴款,请稍候...");
        nRetVal=AutoRunTask("dldj",TASK_PAY_CONFIRM,"%s","%r",sbuf,&rec); 
        if(nRetVal)
        {
        	SetDialogInfo(pd,sTcpipErrInfo);
        	keyb();
        	continue;
        }
        memset(sbuf,0,sizeof(sbuf));
        ExportRecord(&rec,"%s",sInfo);
        
        
        if(strncmp(sInfo,"000000",6))
        {
        	ExportRecord(&rec,"%s",sbuf);
        	DropRecord(&rec);
        	SetDialogInfo(pd,sbuf);
        	keyb();
        	continue;
        }
        
        DropRecord(&rec);
        
        //SetDialogInfo(pd,"正在修改库存量,请稍候...");
        nRetVal=AutoRunTask("dldj",TASK_RECALL_GOODS,"%s%s%s%ld","%r",
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
        else   //成功
        {
                DropRecord(&rec);
        	SetDialogInfo(pd,"缴款并修改库存成功...");
        	sleep(1);
        	continue;
        }

        //修改库存不成功的处理,向数据源撤销本次缴款
        SetDialogInfo(pd,"正在撤消本次缴费，请等待...");
        GenerateLsh(1,sSysJsdh,sJym,sInfo);//生成撤单流水号 
    
        // 交易流水号|被撤单流水号|实缴款额|操作员代号|时分秒|
        sprintf(sbuf,"%s|%s|%.2lf|%s|%ld|",sInfo,sLsh,dMoney,sSysCzydh,nSysSfm);
        nRetVal=AutoRunTask("dldj",TASK_RECALL_CONFIRM,"%s","%r",sbuf,sbuf); 
        if(nRetVal) 
        {
          	SetDialogInfo(pd,sTcpipErrInfo);
                keyb();
                continue;
        }
  	if(strncmp(sbuf,"000000",6))
       	{
       		SetDialogInfo(pd,sbuf+7);
       		keyb();
       	}
       	continue;
     	//成功后的处理
   }//end for(;;)

}


void WastingGoods()
{

     DIALOG * pd;
     RECORD rec;
     MENU * pmsp,* pmident;
     char sJym[16],sJymc[51],sHwbz[21],sInfo[81],sLsh[41],sbuf[256];
     long nCount,nKey,nwhere,nId,identify;
     long nfinished,nTest,nRetVal,nKcl;
     double dMoney=0.0,dPrice=0.0;

     sprintf(sSysSyz,"%s.%s",sSysJsdh,sSysCzydh);
     ///////////////////////////////////////////////////////////////////     
     pmsp=CreateGoodsMenu();
     if(!(pd=CreateDialog(60,15,FRAME))) return;
     AddDialog(pd,"static",STATIC_TEXT,"                 〖  商  品  报  损  〗\n");
     AddDialog(pd,"sep1",SEPARATOR,"__\n");
     
     AddDialog(pd,"cspmc",INPUT,"商品名称:%-46.46s\n\n",sJymc);           //3
     AddDialog(pd,"nspsl",INPUT,"商品数量:%8d",&nCount);                  //4
     AddDialog(pd,"cspbs",INPUT,"         商品标识:%-20.20s\n",sHwbz);    //5
     
     AddDialog(pd,"sep2",SEPARATOR,"__\n");

     AddDialog(pd,"dspdj",OUTPUT,"商品单价:%10.2f",&dPrice);
     AddDialog(pd,"dbszj",OUTPUT,"        报损总款:%10.2f",&dMoney);
     //////////////////////////////////////////////////////////////////
     clrscr();
     
     for(;;)
     {
     	nwhere=1;//star
        dMoney=0.0;
        nCount=0;
        identify=0;//录入货品标识
        memset(sJymc,0,sizeof(sJymc));
        memset(sHwbz,0,sizeof(sHwbz));
        sJymc[0]='.'; 
        sHwbz[0]='.'; 
        
        ShowDialog(pd,10,3);
        RefreshDialog(pd,0);
        nfinished=0;
        for(;;)
        {
                if(nfinished) break;
                switch(nwhere)
                {
                   case 1:
                        SetDialogInfo(pd,"请输入商品名称，或按回车键选择商品..");
                        break;

                   case 2:
                        SetDialogInfo(pd,"请输入报损数量...");
                        break;

                   case 3:
                        SetDialogInfo(pd,"请输入货品标识...");
                        break;


                }
                	    
        	nKey=ActiveDialog(pd,nwhere,"cspmc,nspsl,cspbs");

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
                        RunSelect("select to_number(substr(clxbz,2,1)) from dl$jym \
                                   where cjym=%s into %ld%lf",sJym,&identify);

                        RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=1 \
                                   and cjym=%s into %lf",sJym,&dPrice);

                        nwhere=2;
                        if(identify)
                        {
                            nCount=1;
                            nwhere=3;
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
                        	sprintf(sbuf,"该商品的库存量为:%ld",nKcl);
                        	SetDialogInfo(pd,sbuf);
                        	keyb();
                        	continue;
                        }
                        if(identify) nwhere=3;
                        else nfinished=1;   //input finished
                        continue;

                   case 3:
                        alltrim(sHwbz);
                        nTest=0;
                        SetDialogInfo(pd,"正在检查商品标识，请稍候...");

                        if(sHwbz[0]!='.')  
                           RunSelect("select count(*) from dl$hwjcb \
                                         where cjym=%s and cjhczydh=%s and chwbz=%s \
                                         and ((nczlb=1 and njlzt=1) or \
                                         (nczlb=3 and njlzt=2)) into %ld",
                                         sJym,sSysSyz,sHwbz,&nTest);
                        if(nTest<=0||sHwbz[0]=='.')
                        {                 
                             SetDialogInfo(pd,"商品标识不存在！正在形成商品标识菜单，请稍候...");
                             if(!(pmident=CreateIdentMenu(sJym))) continue;
                             SetDialogInfo(pd,"请从菜单中选择一个商品标识，或按相应数字键...");
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
        RefreshDialog(pd,"dbszk");
        
        SetDialogInfo(pd,"按'1'键确认报损或按ESC键返回...");
        
        for(;;)
        {
        	nKey=keyb();
        	if(nKey==KEY_ESC||nKey=='1') break;
        }
        if(nKey==KEY_ESC) continue;
        
        GenerateLsh(1,sSysJsdh,sJym,sLsh);//生成撤单流水号 

        SetDialogInfo(pd,"正在修改库存量,请稍候...");
        nRetVal=AutoRunTask("dldj",TASK_WASTE_GOODS,"%s%s%s%ld","%r",
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
        else   //成功
        {
                DropRecord(&rec);
        	SetDialogInfo(pd,"报损并修改库存成功...");
        	sleep(1);
        	continue;
        }

   }//end for(;;)

}

long nPrintDssj(char *sLsh)
{
   TABLE * pt;
   long nId;
   long nId1;
   long nSl;//数量
   long year;
   long month;
   long day;
   double dYjke;//应缴款额
   double dXj;//现金
   double dCz;//存折
   double dXyk;//信用卡
   double dSjk;//实缴款
   double dDj;//单价
   char cCzydh[13];//操作员代号
   char cYhmc[31];//用户名称
   char cJym[JYM_JYM_LEN+1];//交易码
   char cSpmc[31];//代售商品名称
   char cSpdw[11];//商品单位
   char buf[64];
   
   nId=RunSelect("select nsl,dyjzk,dsjkxj,dsjkcz,dsjkxyk,dsjk,cyhmc,cczydh,cjym \
                  from dl$zwmx where clsh=%s into %ld%lf%lf%lf%lf%lf%s%s%s",
                  sLsh,&nSl,&dYjke,&dXj,&dCz,&dXyk,&dSjk,cYhmc,cCzydh,cJym);
   if(nId<=0)
   {
   	return(-1);
   }
   nId1=RunSelect("select dbl,cgsmc,cfwm from dl$jym where cjym=%s into %lf%s%s",
                   cJym,&dDj,cSpmc,cSpdw);
   if(nId1<=0)
   {
   	return(-1);
   }
   
   pt=CreateTable(5,"|品名及规格                  |单位  |数量  |单价    |金额    |备注          |");
   sprintf(buf,"现金:%.2lf\n支票:%.2lf\n绿卡:%.2lf\n合计:%.2lf",dXj,dCz,dXyk,dYjke);
   FillTable(pt,2,"> > > > > %s ",buf);
   MergeTable(pt,CreateTable(1,
                    "|金额大写|                                            |                        |"));
   FillTable(pt,2,"%s%s%d%.2f%.2f",cSpmc,cSpdw,nSl,dDj,dYjke);
   to_Chinese(dYjke,buf);
   FillTable(pt,6,"> %s %.2f",buf,dYjke);
   year=nSysLkrq/10000;
   month=(nSysLkrq%10000)/100;
   day=(nSysLkrq%10000)%100;
   setprinton();
   output("                           四 川 省 邮 政 代 售 收 据\n\n");
   output("                                                            %4ld年%2ld月%2ld日填制\n",year,month,day);
   output("   购货单位:%-20.20s      NO.%s\n",cYhmc,sLsh);
   //ShowTable(pt,1,2);                 
   setprintoff();
   OutputTable(pt);//先填写，调用此函数向打印机输出
   setprinton();

   output("   销货单位:%-30.30s         操作员:%s\n",sSysJm,cCzydh);
   eject();
   setprintoff();
   DropTable(pt);
}




