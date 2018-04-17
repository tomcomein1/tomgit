/*------------------------------------------------------------------
 
 frame.c :����ϵͳ�����е���ʽ�˵��ܿ�ģ�� 
 ˵�������й��ܲ˵�ģ�鶼Ӧ���ڱ��ļ��У�����������Ӧ����ʹ��Ӣ��
       �˵���ʹ�õ��ĺ�������frame.h��˵��
       ϵͳȨ�ޣ�
	0  �����û�
  	1  ϵͳά��
  	2  �����鳤
  	3  ҵ��Ա
  	4  ����
  	5  ��ѯͳ��
  	6  ֧�ֳ�

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
//����ϵͳ��һ���˵�
long dlywxt()  
{
  MENU * pMenu;
  char sTitle[51];
  char cCzyqx;

  strcpy(sTitle,"�� �� ҵ �� �� �� ϵ ͳ ");
  sprintf(sTitle,"%s(%s",sTitle,sSysJm);
  if(nSysPcType==1)
	strcat(sTitle,"ͨѶ��)");
  else
	strcat(sTitle,")"); 
  cCzyqx=nSysCzyqx;
  if(!(pMenu=CreateMenu(sTitle,1))) return -1;
  
  cCzyqx=nSysCzyqx+'0';

  if(nSysPcType==1)
  {
 	#ifdef UNIX
 	if(strchr("1", cCzyqx)) BindMenu(pMenu,"�ļ����",NULL,FileExchange);
    if(strchr("1",cCzyqx)) BindMenu(pMenu,"�ļ�����",NULL,FileDownload);
 	#endif
  }
  else
  {
  	if(strchr("016",cCzyqx)) BindMenu(pMenu,"ϵ ͳ �� ��",NULL,SystemManage);
  	if(strchr("236",cCzyqx))
  	{
  		BindMenu(pMenu,"ҵ �� �� ��",NULL,Transaction);
  	        BindMenu(pMenu,"�� �� �� ��",NULL,GoodsManage);
  	}

 	if(strchr("2346", cCzyqx)) BindMenu(pMenu,"�� ѯ ͳ ��",NULL,QueryAndStatistic);
 	if(strchr("2346", cCzyqx)) BindMenu(pMenu,"�� �� �� ��",NULL,AccountManage);
  }
  
  BindMenu(pMenu,"ϵ ͳ �� ��",NULL,SystemHelp);
  //BindMenu(pMenu,"TEST",NULL,ToolsTest);
  
  if(nSysCzylb==0) BindMenu(pMenu,"����",NULL,Others);
  AddMenu(pMenu,"  0.��  ��",NULL);  

  while(ShowMenu(pMenu)!=0);
  DropMenu(pMenu);
  return 0;
}

void  AccountManage() //����
{
  MENU * pMenu;
  
  long cCzyqx=0; 

  if(!(pMenu=CreateMenu("������",1))) return;
  cCzyqx=nSysCzyqx+'0';
  BindMenu(pMenu,"��  ��",NULL,ChargeConfirm);
  BindMenu(pMenu,"�ɿ��ӡ",NULL,ReprintCzyJkd);
  AddMenu(pMenu,"  0.�� ��",NULL);

  ShowMenu(pMenu);
  DropMenu(pMenu);    
}

void  SystemHelp()  //ϵͳ����
{
  ShowHelp("dlyw.hlp",NULL);	
}


void SystemManage()  //ϵͳ����
{
  MENU * pMenu;
  
  char cCzyqx;
  cCzyqx=nSysCzyqx;  
  if(!(pMenu=CreateMenu("ϵ  ͳ  ��  ��",1))) return;
  
  cCzyqx=nSysCzyqx+'0';
  
  if(cCzyqx=='0')
  {
          BindMenu(pMenu,"ϵͳ����",NULL,SystemSetup);
  	  BindMenu(pMenu,"·�ɹ���",NULL,RouteManage);
  	  BindMenu(pMenu,"ϵͳά��",NULL,SystemMaintenance);

  }        
  if(cCzyqx=='1')
  {
          BindMenu(pMenu,"ҵ������",NULL,JymSetup);
	  BindMenu(pMenu,"���������",NULL,JymManage);
	  BindMenu(pMenu,"ҵ��չ����",NULL,OpenManage);
	  BindMenu(pMenu,"ί�й�˾����",NULL,CompanyManage);
	  BindMenu(pMenu,"ҵ���������",NULL,YwzlManage);
      	  BindMenu(pMenu,"��ӡ���ݸ�ʽ����",NULL,BillFormatManager);
      	  BindMenu(pMenu,"��ӡ���ݸ�ʽ����",NULL,ExportPrintTable);
	  if(strstr(sSysJh,"2715"))
	  	BindMenu(pMenu,"��˰���ο�Ŀ����",NULL,Terra_Jc);
	  BindMenu(pMenu,"��˰���������й���",NULL,Tax_Khhgl);
	  BindMenu(pMenu,"�ֹ����ʴ���",NULL,AccessAccount);
		          
  }

  if(strchr("016",  cCzyqx))
  {
	  BindMenu(pMenu,"����Ա����",NULL,UserManage);
          if(cCzyqx!='6') BindMenu(pMenu,"�ֺŹ���",NULL,JuHaoManage);

  }
  AddMenu(pMenu,"  0.�� ��",NULL);
  ShowMenu(pMenu);
  DropMenu(pMenu);
}


void Transaction()
{

  MENU * pMenu;
  char cCzyqx;
  
  cCzyqx=nSysCzyqx;  
  if(!(pMenu=CreateMenu("�� �� ҵ ��",1))) return;
  
  if(cCzyqx>=0&&cCzyqx<=9)  cCzyqx=nSysCzyqx+'0';

  BindMenu(pMenu,"����ҵ��",NULL,DaiShouYw);
  BindMenu(pMenu,"����ҵ��",NULL,SellGoods);
  BindMenu(pMenu,"����ҵ��",NULL,DaiBanYw);
  AddMenu(pMenu,"  0.��  ��",NULL);

  ShowMenu(pMenu);
  DropMenu(pMenu);

}

void JymManage() //���������
{
    
  MENU * pMenu;
  if(!(pMenu=CreateMenu("���׹���",1))) return;

  BindMenu(pMenu,"���Ӵ��ս���",NULL,AddDsJym);
  BindMenu(pMenu,"������Ʒ����",NULL,AddSpJym);
  BindMenu(pMenu,"���Ӵ��콻��",NULL,AddDbJym);
  //BindMenu(pMenu,"������ؽ���",NULL,AddYdJym);
  BindMenu(pMenu,"��ѯ���н���",NULL,CxExistJym);
  BindMenu(pMenu,"ɾ������",NULL,DeleteJym);
  AddMenu(pMenu,"  0.�� ��",NULL);
  ShowMenu(pMenu);
  DropMenu(pMenu);    
}


void JuHaoManage() //�ֺŹ���
{
    
  MENU * pMenu;
  if(!(pMenu=CreateMenu("�ֺŹ���",1))) return;

  if(nSysCzyqx==0) BindMenu(pMenu,"�ؾ־ֺŹ���",NULL,XianJuJuHao);
  if(nSysCzyqx==0) BindMenu(pMenu,"�ؾ־ֺŶ�������",NULL,XianJuHaoDuiZhao);

  if(nSysCzyqx==1) BindMenu(pMenu,"֧�־ֺŹ���",NULL,ZhiJuJuHao);
  if(nSysCzyqx==1) BindMenu(pMenu,"֧�־ֺŶ�������",NULL,ZhiJuHaoDuiZhao);
  AddMenu(pMenu,"  0.�� ��",NULL);
  ShowMenu(pMenu);
  DropMenu(pMenu);    
}

void GoodsManage()//���۹���
{
   MENU * pMenu;
   char cCzyqx;
  
   cCzyqx=nSysCzyqx;  
   if(!(pMenu=CreateMenu("�� �� �� ��",1))) return;
  
   if(cCzyqx>=0&&cCzyqx<=9)  cCzyqx=nSysCzyqx+'0';
   if(strchr("26",cCzyqx))
   {
       BindMenu(pMenu,"��    ��",NULL,PurchaseGoods);
       BindMenu(pMenu,"��ί�з��˻�",NULL,ReturnGoods);
       BindMenu(pMenu,"��    ��",NULL,WastingGoods);
   }

   BindMenu(pMenu,"��    ��",NULL,AcceptGoods);
   BindMenu(pMenu,"��    ��",NULL,DistributeGoods);
   BindMenu(pMenu,"�ͻ��˻�",NULL,RecallGoods);
   AddMenu(pMenu,"  0.��  ��",NULL);  

   ShowMenu(pMenu);
   DropMenu(pMenu);
}

void Others()//��������
{
   MENU * pMenu;
   clrscr();

   if(!(pMenu=CreateMenu("��  ��",1))) return;
    
 
   BindMenu(pMenu,"���Ĳ���Ա����",NULL,ChangeUserPassword);
   if(nSysCzyqx==2) BindMenu(pMenu,"�绰����ҵ��",NULL,tb_menu);

   AddMenu(pMenu,"  0.��  ��",NULL);  
 

   ShowMenu(pMenu);
   DropMenu(pMenu);
    

}

void SystemMaintenance()
{
   MENU * pMenu;
   clrscr();

   if(!(pMenu=CreateMenu("ϵ ͳ �� ��",1))) return;

   BindMenu(pMenu,"���ݿⱸ��",NULL,DatabaseBackup);
   BindMenu(pMenu,"�ָ����ݿⱸ��",NULL,DatabaseRestore);
   BindMenu(pMenu,"�����ʷ����",NULL,CleanupHistory);
   AddMenu(pMenu,"  0.��  ��",NULL);  

   ShowMenu(pMenu);
   DropMenu(pMenu);

}
