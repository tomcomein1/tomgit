/***********************************************************************
** @Copyright........... 2000.7
**filename       文件名: d02.c
**target           目的: 移动缴费菜单
**create time  生成日期: 2000.7
**original programmer          原始编程人员: 陈博
                              
**edit history 修改历史 :       改编程人：林之栋
**date日期    editer修改人员     reason原因 
**************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "dlpublic.h"
#include "tools.h"
#include "dl_yw.h"
#include "dl_mobile.h"
#include "dl_power.h"
#include "newtask.h"
#include "query.h"

char  sJym[16];               //交易码
long  nCxms;                  //程序模式
char  sJymc[51];              //交易名称
char  sRemoteHost[21];        //远方主机名

extern long nSysLkrq;
extern long nSysSfm;
extern long nSysCzyqx;
extern long nInstType;
extern char sSysCzydh[8];
extern char sSysIp[20];
extern char sSysJh[10];
extern char sSysJsdh[JM_JH_LEN+1];
extern char sTcpipErrInfo[81];

/*==============================================================
函数：AddPayMenuItem
功能：根据不同的程序模式映射相应的功能，一般来说，前三项分别为：
      代收费(DlywPay)、交费撤消(PayRecall)、补打单据(ReprintBill)，
      后面的功能则是本业务的特殊功能。
*==============================================================*/

void AddPayMenuItem(MENU * pmn)
{
    switch(nCxms)
    {
        case 130:  //新移动代收费
	if(strcmp(sSysCzydh,"DL185") )
	{
            	BindMenu(pmn,"移动手机费代收",sJym,DlywPay);
	   	BindMenu(pmn,"本次交费撤消",sJym,PayRecall);
        	BindMenu(pmn,"代打完全划拨发票",sJym,MobileCompletePrint);
        	BindMenu(pmn,"向移动签到",sJym,MobileSignIn);
	}
   	BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
  	if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0) 
   	{ 
        		BindMenu(pmn,"向移动签退",sJym,MobileSignOut);
        		BindMenu(pmn,"与移动对帐",sJym,MobileCheckAccount);
			BindMenu(pmn,"查询对帐情况",sJym,QueryAccountData);
   	}
                break;
                        
        case 110: //联通代收
                BindMenu(pmn,"联通手机费代收",sJym,DlywPay);
                BindMenu(pmn,"本次交费撤消",sJym,PayRecall);
                BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
                BindMenu(pmn,"代打预存款票据",sJym,UnicomPrint);
                break;

        case 140: //电信代收
	case 141: //YA
                BindMenu(pmn,"固定电话代收费",sJym,DlywPay);
		BindMenu(pmn,"[多号]电话费代收",sJym,MulDlywPay);
                BindMenu(pmn,"本次交费撤消",sJym,PayRecall);
                BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
                if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0)
                   BindMenu(pmn,"与电信对帐",sJym,TelecomCheckAccount);
                break;


        case 111:  //联通预存
                BindMenu(pmn,"联通手机费预缴",sJym,DlywPay);
 	        BindMenu(pmn,"本次交费撤消",sJym,PayRecall);
                BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
                break;
        
        case 210: //寻呼
        case 220:
        case 230:
                BindMenu(pmn,"寻呼服务费代收",sJym,DlywPay);
                BindMenu(pmn,"本次交费撤消",sJym,PayRecall);
                BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
  		if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0) 
                    BindMenu(pmn,"与寻呼对帐",sJym,CheckAccountWithBP);
                break;
         
        case 610: // 天然气代收
                BindMenu(pmn,"天然气费代收",sJym,DlywPay);
                BindMenu(pmn,"本次交费撤消",sJym,PayRecall);
                BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
                BindMenu(pmn,"查询出库功能",sJym,PostGasToFile);
                break;

        case 1001://民航
                BindMenu(pmn,"查票与订票",sJym,AirTicket);
                //BindMenu(pmn,"查询机票",sJym,QueryAirTicket);
                BindMenu(pmn,"补打订票单据",sJym,ReprintBill);
                BindMenu(pmn,"机票回款",sJym,AirTicketPay);
                BindMenu(pmn,"机票送达",sJym,AirTicketDeliver);
                BindMenu(pmn,"退票申请",sJym,AirReturnTicketBeg);
		BindMenu(pmn,"取消锁票或出票",sJym,OutAirTicket);
		if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,4)==0)
                {
                	BindMenu(pmn,"机票打印确正",sJym,CheckAirTicket);
			BindMenu(pmn,"退票申请处理",sJym,AirDoReturnTicketBeg);
			BindMenu(pmn,"监控订票情况",sJym,CheckPurTicket);
			BindMenu(pmn,"机票订单处理",sJym,AirDdProduct);			                	
                }//省局
		BindMenu(pmn,"综合查询",sJym,QueryAirAcc);
		BindMenu(pmn,"业务报表",sJym,MenuYwbb);
                break;
       
        case 821://人寿保险
		if(strcmp(sSysCzydh,"DL185") )
		{
                BindMenu(pmn,"人寿保险代收",sJym,DlywPay);
                BindMenu(pmn,"本次交费冲正",sJym,PayRecall);
		}
                BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
                if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0)
                   BindMenu(pmn,"人寿保险对帐",sJym,B2iCheckAccount);
                break;
       
        case 521:
        case 522:
                BindMenu(pmn,"国税代收",sJym,DlywPay);
                BindMenu(pmn,"本次交费撤消",sJym,PayRecall);
                BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
		BindMenu(pmn,"查询纳税人信息",sJym,QueryTax_Info);
                if(nSysCzyqx==2||nSysCzyqx==6)
                   BindMenu(pmn,"汇总专用缴款书",sJym,TaxPrintBill);
                if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0)
                {   
                   if(nCxms==522) BindMenu(pmn,"向国税局对帐",sJym,TaxCheckAccount);
                   else BindMenu(pmn,"汇总日文件",sJym,HzDayTaxFile);
                   //BindMenu(pmn,"纳税人基资料入库",NULL,InsertJzzl);
                }
		BindMenu(pmn,"纳税人旧号码查新号",NULL,QueryJzzl);
                break;
        
        case 123:  //电费代收费
	case 124:  //众望
	case 125:  //蓬溪
	        BindMenu(pmn,"电费代收",sJym,DlywPay);
		BindMenu(pmn,"本次交费撤消",sJym,PayRecall);
	  	BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
		if(nCxms==125)	
		{
		  BindMenu(pmn,"查询用户信息",sJym,QueryPower_Info);
		  BindMenu(pmn,"查询滞纳金",sJym,QueryZnj_Info);
		}
		else
		{
		  BindMenu(pmn,"划帐对照表",sJym,QueryPowerbyJh);
		  BindMenu(pmn,"向供电局签到",sJym,PowerSignIn);
	  	  if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0) 
	   	  { 
	            BindMenu(pmn,"向供电局签退",sJym,PowerSignOut);
	            BindMenu(pmn,"与供电局对帐",sJym,PowerCheckAccount);
	   	  } //DLZZ
		}//end ohter
	        break;
		
	case 222:
	case 223:
		BindMenu(pmn,"地税代收",sJym,DlywPay);
		BindMenu(pmn,"本次交费撤消",sJym,PayRecall);
		BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
		BindMenu(pmn,"查询纳税人信息",sJym,QueryTax_Info);
		BindMenu(pmn,"定额发票销售",sJym,Terra_Defp);
		if(nSysCzyqx==2||nSysCzyqx==6)
		{
                   BindMenu(pmn,"汇总专用缴款书",sJym,TerraPrintBill);
                   BindMenu(pmn,"缴款书文件生成",sJym,TerraJks);
		}
		break;
        case 333://太平洋保险
		BindMenu(pmn,"太平洋保险代收",sJym,DlywPay);
                BindMenu(pmn,"本次交费冲正",sJym,PayRecall);
		BindMenu(pmn,"补打收费单据",sJym,ReprintBill);
                BindMenu(pmn,"查询保险人信息",sJym,QueryTax_Id_Info);
                break;

        default:	return;         
         
    }//end switch
}

void DaiShouYw()
{
     MENU * pmn;
     
     pmn=CreateJymMenu(1);
     clrscr();     
     if(pmn)
     {
     	gotoxy(16,10);
     	outtext("请输入代收业务代码:");
     	PopMenu(pmn,36,10,12);
     }	
     DropMenu(pmn);
}

void DaiBanYw()
{
     MENU * pmn;
     
     pmn=CreateJymMenu(3);
     clrscr();     
     if(pmn)
     {
     	gotoxy(16,10);
     	outtext("请输入代办业务代码:");
     	PopMenu(pmn,36,10,12);
     }	
     DropMenu(pmn);
}


/*--------------------------------------------------------------
函数：CreateJymMenu
功能：建立业务选择菜单
返回：菜单指针
说明：根据不同的程序模式增加相应的子菜单项
*--------------------------------------------------------------*/
MENU * CreateJymMenu(long nlx)//nlx==1 表代收业务 2 表代售业务 3 代办业务
{
    
    MENU * pMenu,* pmn;
    long nId,nSyzt;
    char sTemp[81];
    char sSjydz[21]=".";
    long nRetVal=1;
    
    pMenu=CreateMenu("业务",0);
    
    
    if(pMenu==NULL) return NULL;

    nId=OpenCursor("select cjym,cjymc,ncxms,csjydz from dl$jym where nbl=%ld",nlx);

    if(nId<0)
    {
             	DropMenu(pMenu);
             	HiError(nId,"Error!");
             	return NULL;
    }	                
    
    for(;;)
    {
       	 if(FetchCursor(nId,"%s%s%ld%s",sJym,sJymc,&nCxms,sSjydz)<=0) break;
         
	 alltrim(sSjydz);
      
         nSyzt=-1;
         RunSelect("select nsyzt from dl$jymkzfb where cjym=%s and cjh=%s into %ld",
                   sJym,sSysJsdh,&nSyzt);
         switch(nSyzt)
         {
         	case 0:          
         	     if(nlx==3) ShowHelp("tpgz.hlp",NULL);
         	     sprintf(sTemp,"[√]%s",sJymc);
                     pmn=CreateMenu(sTemp,0);
                     AddPayMenuItem(pmn);
    	             BindSubMenu(pMenu,pmn,sJym);
     	             break;
            	case 1:
         	    sprintf(sTemp,"[Ｘ]%s",sJymc);
                    BindMenu(pMenu,sTemp,sJym,prohibit);
                    break;
                default:
         	    sprintf(sTemp,"[？]%s",sJymc);
                    BindMenu(pMenu,sTemp,sJym,nosetting);
                    break;
                        
         }
    }//end for
    CloseCursor(nId);
    return pMenu;
}

void prohibit(long nType,char * sItem,char * sJym)
{
    char sMsg[300];
      
    RunSelect("select cxx from dl$jymkzfb where cjym=%s and cjh=%s into %s",
               sJym,sSysJsdh,sMsg);
    if(!strcmp(sMsg,"."))
       strcpy(sMsg,"该业务被代理中心禁止");
    
    MessageBox(sMsg,1);
}


void nosetting(long nType,char * sItem,char * sJym)
{
    MessageBox("    代理中心还未开通该业务，暂不能使用",1);
}


void QueryAirAcc(long number,char * item,char *sJym)
{
	MENU * pmenu;
        
    pmenu=CreateMenu("综合查询",1);
    if(pmenu==NULL) return;
    
	BindMenu(pmenu,"锁票查询",sJym,QueryAirSp);
	BindMenu(pmenu,"退票查询",sJym,QueryAirTp);
	BindMenu(pmenu,"送达查询",sJym,QueryAirSd);
	BindMenu(pmenu,"送票方式查询",sJym,QueryAirSpfs);
	BindMenu(pmenu,"订单查询",sJym,QueryAirDdxx);

	AddMenu(pmenu,"  0.退  出",NULL);   	
    	ShowMenu(pmenu);
	DropMenu(pmenu);
}

void MenuYwbb(long number,char * item,char *sJym)
{
	MENU * pmenu;
        
    	pmenu=CreateMenu("业务报表",1);
    	if(pmenu==NULL) return;
    
	BindMenu(pmenu,"送达报表",sJym,BbAirSd);
	BindMenu(pmenu,"现金收支报表",sJym,BbAirXj);
	AddMenu(pmenu,"  0.退  出",NULL);   	
    	ShowMenu(pmenu);
	DropMenu(pmenu);
}

void QueryAccountData(long nType,char * sItem,char * sJym)
{
	clrscr();
	QueryLocal("交易码          对帐不成功日期",
      	           "%s%ld","select distinct cjym,nlkrq from dl$zwmx where ncsbz=1 and cjym=%s order by nlkrq",sJym);
}
