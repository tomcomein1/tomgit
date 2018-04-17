/*------------------------------------------------------------------
 
 frame.c :代理系统中所有弹出式菜单总控模块 
 说明：所有功能菜单模块都应定在本文件中，函数的命名应尽量使用英文
       菜单中使用到的函数请在frame.h中说明
       系统权限：
	0  超级用户
  	1  系统维护
  	2  代理组长
  	3  业务员
  	4  帐务
  	5  查询统计
  	6  支局长

*-----------------------------------------------------------------*/
#include"dlpublic.h"
#include"tools.h"
#include"frame.h"

void ToolsTest();

extern char sSysJh[10];
extern char sSysJm[21];
extern long nSysCzyqx;
extern long nSysPcType;
extern long nSysCzylb;
//代理系统第一级菜单
long dlywxt()  
{
  MENU * pMenu;
  char sTitle[51];
  char cCzyqx;

  strcpy(sTitle,"代 理 业 务 网 络 系 统 ");
  sprintf(sTitle,"%s(%s",sTitle,sSysJm);
  if(nSysPcType==1)
	strcat(sTitle,"通讯机)");
  else
	strcat(sTitle,")"); 
  cCzyqx=nSysCzyqx;
  if(!(pMenu=CreateMenu(sTitle,1))) return -1;
  
  cCzyqx=nSysCzyqx+'0';

  if(nSysPcType==1)
  {
 	#ifdef UNIX
 	if(strchr("1", cCzyqx)) BindMenu(pMenu,"文件入库",NULL,FileExchange);
    if(strchr("1",cCzyqx)) BindMenu(pMenu,"文件生成",NULL,FileDownload);
 	#endif
  }
  else
  {
  	if(strchr("016",cCzyqx)) BindMenu(pMenu,"系 统 管 理",NULL,SystemManage);
  	if(strchr("236",cCzyqx))
  	{
  		BindMenu(pMenu,"业 务 办 理",NULL,Transaction);
  	        BindMenu(pMenu,"代 售 管 理",NULL,GoodsManage);
  	}

 	if(strchr("2346", cCzyqx)) BindMenu(pMenu,"查 询 统 计",NULL,QueryAndStatistic);
 	if(strchr("2346", cCzyqx)) BindMenu(pMenu,"帐 务 处 理",NULL,AccountManage);
  }
  
  BindMenu(pMenu,"系 统 帮 助",NULL,SystemHelp);
  //BindMenu(pMenu,"TEST",NULL,ToolsTest);
  
  if(nSysCzylb==0) BindMenu(pMenu,"其它",NULL,Others);
  AddMenu(pMenu,"  0.退  出",NULL);  

  while(ShowMenu(pMenu)!=0);
  DropMenu(pMenu);
  return 0;
}

void  AccountManage() //帐务
{
  MENU * pMenu;
  
  long cCzyqx=0; 

  if(!(pMenu=CreateMenu("帐务处理",1))) return;
  cCzyqx=nSysCzyqx+'0';
  BindMenu(pMenu,"缴  款",NULL,ChargeConfirm);
  BindMenu(pMenu,"缴款单打印",NULL,ReprintCzyJkd);
  AddMenu(pMenu,"  0.退 出",NULL);

  ShowMenu(pMenu);
  DropMenu(pMenu);    
}

void  SystemHelp()  //系统帮助
{
  ShowHelp("dlyw.hlp",NULL);	
}


void SystemManage()  //系统管理
{
  MENU * pMenu;
  
  char cCzyqx;
  cCzyqx=nSysCzyqx;  
  if(!(pMenu=CreateMenu("系  统  管  理",1))) return;
  
  cCzyqx=nSysCzyqx+'0';
  
  if(cCzyqx=='0')
  {
          BindMenu(pMenu,"系统设置",NULL,SystemSetup);
  	  BindMenu(pMenu,"路由管理",NULL,RouteManage);
  	  BindMenu(pMenu,"系统维护",NULL,SystemMaintenance);

  }        
  if(cCzyqx=='1')
  {
          BindMenu(pMenu,"业务设置",NULL,JymSetup);
	  BindMenu(pMenu,"交易码管理",NULL,JymManage);
	  BindMenu(pMenu,"业务开展管理",NULL,OpenManage);
	  BindMenu(pMenu,"委托公司管理",NULL,CompanyManage);
	  BindMenu(pMenu,"业务种类管理",NULL,YwzlManage);
      	  BindMenu(pMenu,"打印单据格式管理",NULL,BillFormatManager);
      	  BindMenu(pMenu,"打印单据格式导出",NULL,ExportPrintTable);
	  if(strstr(sSysJh,"2715"))
	  	BindMenu(pMenu,"地税级次科目管理",NULL,Terra_Jc);
	  BindMenu(pMenu,"国税局所开户行管理",NULL,Tax_Khhgl);
	  BindMenu(pMenu,"手工调帐处理",NULL,AccessAccount);
		          
  }

  if(strchr("016",  cCzyqx))
  {
	  BindMenu(pMenu,"操作员管理",NULL,UserManage);
          if(cCzyqx!='6') BindMenu(pMenu,"局号管理",NULL,JuHaoManage);

  }
  AddMenu(pMenu,"  0.退 出",NULL);
  ShowMenu(pMenu);
  DropMenu(pMenu);
}


void Transaction()
{

  MENU * pMenu;
  char cCzyqx;
  
  cCzyqx=nSysCzyqx;  
  if(!(pMenu=CreateMenu("代 理 业 务",1))) return;
  
  if(cCzyqx>=0&&cCzyqx<=9)  cCzyqx=nSysCzyqx+'0';

  BindMenu(pMenu,"代收业务",NULL,DaiShouYw);
  BindMenu(pMenu,"代售业务",NULL,SellGoods);
  BindMenu(pMenu,"代办业务",NULL,DaiBanYw);
  AddMenu(pMenu,"  0.退  出",NULL);

  ShowMenu(pMenu);
  DropMenu(pMenu);

}

void JymManage() //交易码管理
{
    
  MENU * pMenu;
  if(!(pMenu=CreateMenu("交易管理",1))) return;

  BindMenu(pMenu,"增加代收交易",NULL,AddDsJym);
  BindMenu(pMenu,"增加商品交易",NULL,AddSpJym);
  BindMenu(pMenu,"增加代办交易",NULL,AddDbJym);
  //BindMenu(pMenu,"增加异地交易",NULL,AddYdJym);
  BindMenu(pMenu,"查询现有交易",NULL,CxExistJym);
  BindMenu(pMenu,"删除交易",NULL,DeleteJym);
  AddMenu(pMenu,"  0.退 出",NULL);
  ShowMenu(pMenu);
  DropMenu(pMenu);    
}


void JuHaoManage() //局号管理
{
    
  MENU * pMenu;
  if(!(pMenu=CreateMenu("局号管理",1))) return;

  if(nSysCzyqx==0) BindMenu(pMenu,"县局局号管理",NULL,XianJuJuHao);
  if(nSysCzyqx==0) BindMenu(pMenu,"县局局号对照设置",NULL,XianJuHaoDuiZhao);

  if(nSysCzyqx==1) BindMenu(pMenu,"支局局号管理",NULL,ZhiJuJuHao);
  if(nSysCzyqx==1) BindMenu(pMenu,"支局局号对照设置",NULL,ZhiJuHaoDuiZhao);
  AddMenu(pMenu,"  0.退 出",NULL);
  ShowMenu(pMenu);
  DropMenu(pMenu);    
}

void GoodsManage()//代售管理
{
   MENU * pMenu;
   char cCzyqx;
  
   cCzyqx=nSysCzyqx;  
   if(!(pMenu=CreateMenu("代 售 管 理",1))) return;
  
   if(cCzyqx>=0&&cCzyqx<=9)  cCzyqx=nSysCzyqx+'0';
   if(strchr("26",cCzyqx))
   {
       BindMenu(pMenu,"进    货",NULL,PurchaseGoods);
       BindMenu(pMenu,"向委托方退货",NULL,ReturnGoods);
       BindMenu(pMenu,"报    损",NULL,WastingGoods);
   }

   BindMenu(pMenu,"接    货",NULL,AcceptGoods);
   BindMenu(pMenu,"发    货",NULL,DistributeGoods);
   BindMenu(pMenu,"客户退货",NULL,RecallGoods);
   AddMenu(pMenu,"  0.退  出",NULL);  

   ShowMenu(pMenu);
   DropMenu(pMenu);
}

void Others()//其它处理
{
   MENU * pMenu;
   clrscr();

   if(!(pMenu=CreateMenu("其  它",1))) return;
    
 
   BindMenu(pMenu,"更改操作员口令",NULL,ChangeUserPassword);
   if(nSysCzyqx==2) BindMenu(pMenu,"电话银行业务",NULL,tb_menu);

   AddMenu(pMenu,"  0.退  出",NULL);  
 

   ShowMenu(pMenu);
   DropMenu(pMenu);
    

}

void SystemMaintenance()
{
   MENU * pMenu;
   clrscr();

   if(!(pMenu=CreateMenu("系 统 备 份",1))) return;

   BindMenu(pMenu,"数据库备份",NULL,DatabaseBackup);
   BindMenu(pMenu,"恢复数据库备份",NULL,DatabaseRestore);
   BindMenu(pMenu,"清空历史数据",NULL,CleanupHistory);
   AddMenu(pMenu,"  0.退  出",NULL);  

   ShowMenu(pMenu);
   DropMenu(pMenu);

}
