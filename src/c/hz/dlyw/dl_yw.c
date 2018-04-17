/***********************************************************************
** @Copyright........... 2000.7
**filename       �ļ���: d02.c
**target           Ŀ��: �ƶ��ɷѲ˵�
**create time  ��������: 2000.7
**original programmer          ԭʼ�����Ա: �²�
                              
**edit history �޸���ʷ :       �ı���ˣ���֮��
**date����    editer�޸���Ա     reasonԭ�� 
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

char  sJym[16];               //������
long  nCxms;                  //����ģʽ
char  sJymc[51];              //��������
char  sRemoteHost[21];        //Զ��������

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
������AddPayMenuItem
���ܣ����ݲ�ͬ�ĳ���ģʽӳ����Ӧ�Ĺ��ܣ�һ����˵��ǰ����ֱ�Ϊ��
      ���շ�(DlywPay)�����ѳ���(PayRecall)�����򵥾�(ReprintBill)��
      ����Ĺ������Ǳ�ҵ������⹦�ܡ�
*==============================================================*/

void AddPayMenuItem(MENU * pmn)
{
    switch(nCxms)
    {
        case 130:  //���ƶ����շ�
	if(strcmp(sSysCzydh,"DL185") )
	{
            	BindMenu(pmn,"�ƶ��ֻ��Ѵ���",sJym,DlywPay);
	   	BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
        	BindMenu(pmn,"������ȫ������Ʊ",sJym,MobileCompletePrint);
        	BindMenu(pmn,"���ƶ�ǩ��",sJym,MobileSignIn);
	}
   	BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
  	if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0) 
   	{ 
        		BindMenu(pmn,"���ƶ�ǩ��",sJym,MobileSignOut);
        		BindMenu(pmn,"���ƶ�����",sJym,MobileCheckAccount);
			BindMenu(pmn,"��ѯ�������",sJym,QueryAccountData);
   	}
                break;
                        
        case 110: //��ͨ����
                BindMenu(pmn,"��ͨ�ֻ��Ѵ���",sJym,DlywPay);
                BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
                BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
                BindMenu(pmn,"����Ԥ���Ʊ��",sJym,UnicomPrint);
                break;

        case 140: //���Ŵ���
	case 141: //YA
                BindMenu(pmn,"�̶��绰���շ�",sJym,DlywPay);
		BindMenu(pmn,"[���]�绰�Ѵ���",sJym,MulDlywPay);
                BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
                BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
                if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0)
                   BindMenu(pmn,"����Ŷ���",sJym,TelecomCheckAccount);
                break;


        case 111:  //��ͨԤ��
                BindMenu(pmn,"��ͨ�ֻ���Ԥ��",sJym,DlywPay);
 	        BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
                BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
                break;
        
        case 210: //Ѱ��
        case 220:
        case 230:
                BindMenu(pmn,"Ѱ������Ѵ���",sJym,DlywPay);
                BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
                BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
  		if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0) 
                    BindMenu(pmn,"��Ѱ������",sJym,CheckAccountWithBP);
                break;
         
        case 610: // ��Ȼ������
                BindMenu(pmn,"��Ȼ���Ѵ���",sJym,DlywPay);
                BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
                BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
                BindMenu(pmn,"��ѯ���⹦��",sJym,PostGasToFile);
                break;

        case 1001://��
                BindMenu(pmn,"��Ʊ�붩Ʊ",sJym,AirTicket);
                //BindMenu(pmn,"��ѯ��Ʊ",sJym,QueryAirTicket);
                BindMenu(pmn,"����Ʊ����",sJym,ReprintBill);
                BindMenu(pmn,"��Ʊ�ؿ�",sJym,AirTicketPay);
                BindMenu(pmn,"��Ʊ�ʹ�",sJym,AirTicketDeliver);
                BindMenu(pmn,"��Ʊ����",sJym,AirReturnTicketBeg);
		BindMenu(pmn,"ȡ����Ʊ���Ʊ",sJym,OutAirTicket);
		if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,4)==0)
                {
                	BindMenu(pmn,"��Ʊ��ӡȷ��",sJym,CheckAirTicket);
			BindMenu(pmn,"��Ʊ���봦��",sJym,AirDoReturnTicketBeg);
			BindMenu(pmn,"��ض�Ʊ���",sJym,CheckPurTicket);
			BindMenu(pmn,"��Ʊ��������",sJym,AirDdProduct);			                	
                }//ʡ��
		BindMenu(pmn,"�ۺϲ�ѯ",sJym,QueryAirAcc);
		BindMenu(pmn,"ҵ�񱨱�",sJym,MenuYwbb);
                break;
       
        case 821://���ٱ���
		if(strcmp(sSysCzydh,"DL185") )
		{
                BindMenu(pmn,"���ٱ��մ���",sJym,DlywPay);
                BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
		}
                BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
                if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0)
                   BindMenu(pmn,"���ٱ��ն���",sJym,B2iCheckAccount);
                break;
       
        case 521:
        case 522:
                BindMenu(pmn,"��˰����",sJym,DlywPay);
                BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
                BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
		BindMenu(pmn,"��ѯ��˰����Ϣ",sJym,QueryTax_Info);
                if(nSysCzyqx==2||nSysCzyqx==6)
                   BindMenu(pmn,"����ר�ýɿ���",sJym,TaxPrintBill);
                if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0)
                {   
                   if(nCxms==522) BindMenu(pmn,"���˰�ֶ���",sJym,TaxCheckAccount);
                   else BindMenu(pmn,"�������ļ�",sJym,HzDayTaxFile);
                   //BindMenu(pmn,"��˰�˻��������",NULL,InsertJzzl);
                }
		BindMenu(pmn,"��˰�˾ɺ�����º�",NULL,QueryJzzl);
                break;
        
        case 123:  //��Ѵ��շ�
	case 124:  //����
	case 125:  //��Ϫ
	        BindMenu(pmn,"��Ѵ���",sJym,DlywPay);
		BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
	  	BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
		if(nCxms==125)	
		{
		  BindMenu(pmn,"��ѯ�û���Ϣ",sJym,QueryPower_Info);
		  BindMenu(pmn,"��ѯ���ɽ�",sJym,QueryZnj_Info);
		}
		else
		{
		  BindMenu(pmn,"���ʶ��ձ�",sJym,QueryPowerbyJh);
		  BindMenu(pmn,"�򹩵��ǩ��",sJym,PowerSignIn);
	  	  if(nSysCzyqx==2 && strncmp(sJym+9,sSysJsdh,6)==0) 
	   	  { 
	            BindMenu(pmn,"�򹩵��ǩ��",sJym,PowerSignOut);
	            BindMenu(pmn,"�빩��ֶ���",sJym,PowerCheckAccount);
	   	  } //DLZZ
		}//end ohter
	        break;
		
	case 222:
	case 223:
		BindMenu(pmn,"��˰����",sJym,DlywPay);
		BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
		BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
		BindMenu(pmn,"��ѯ��˰����Ϣ",sJym,QueryTax_Info);
		BindMenu(pmn,"���Ʊ����",sJym,Terra_Defp);
		if(nSysCzyqx==2||nSysCzyqx==6)
		{
                   BindMenu(pmn,"����ר�ýɿ���",sJym,TerraPrintBill);
                   BindMenu(pmn,"�ɿ����ļ�����",sJym,TerraJks);
		}
		break;
        case 333://̫ƽ����
		BindMenu(pmn,"̫ƽ���մ���",sJym,DlywPay);
                BindMenu(pmn,"���ν��ѳ���",sJym,PayRecall);
		BindMenu(pmn,"�����շѵ���",sJym,ReprintBill);
                BindMenu(pmn,"��ѯ��������Ϣ",sJym,QueryTax_Id_Info);
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
     	outtext("���������ҵ�����:");
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
     	outtext("���������ҵ�����:");
     	PopMenu(pmn,36,10,12);
     }	
     DropMenu(pmn);
}


/*--------------------------------------------------------------
������CreateJymMenu
���ܣ�����ҵ��ѡ��˵�
���أ��˵�ָ��
˵�������ݲ�ͬ�ĳ���ģʽ������Ӧ���Ӳ˵���
*--------------------------------------------------------------*/
MENU * CreateJymMenu(long nlx)//nlx==1 �����ҵ�� 2 �����ҵ�� 3 ����ҵ��
{
    
    MENU * pMenu,* pmn;
    long nId,nSyzt;
    char sTemp[81];
    char sSjydz[21]=".";
    long nRetVal=1;
    
    pMenu=CreateMenu("ҵ��",0);
    
    
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
         	     sprintf(sTemp,"[��]%s",sJymc);
                     pmn=CreateMenu(sTemp,0);
                     AddPayMenuItem(pmn);
    	             BindSubMenu(pMenu,pmn,sJym);
     	             break;
            	case 1:
         	    sprintf(sTemp,"[��]%s",sJymc);
                    BindMenu(pMenu,sTemp,sJym,prohibit);
                    break;
                default:
         	    sprintf(sTemp,"[��]%s",sJymc);
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
       strcpy(sMsg,"��ҵ�񱻴������Ľ�ֹ");
    
    MessageBox(sMsg,1);
}


void nosetting(long nType,char * sItem,char * sJym)
{
    MessageBox("    �������Ļ�δ��ͨ��ҵ���ݲ���ʹ��",1);
}


void QueryAirAcc(long number,char * item,char *sJym)
{
	MENU * pmenu;
        
    pmenu=CreateMenu("�ۺϲ�ѯ",1);
    if(pmenu==NULL) return;
    
	BindMenu(pmenu,"��Ʊ��ѯ",sJym,QueryAirSp);
	BindMenu(pmenu,"��Ʊ��ѯ",sJym,QueryAirTp);
	BindMenu(pmenu,"�ʹ��ѯ",sJym,QueryAirSd);
	BindMenu(pmenu,"��Ʊ��ʽ��ѯ",sJym,QueryAirSpfs);
	BindMenu(pmenu,"������ѯ",sJym,QueryAirDdxx);

	AddMenu(pmenu,"  0.��  ��",NULL);   	
    	ShowMenu(pmenu);
	DropMenu(pmenu);
}

void MenuYwbb(long number,char * item,char *sJym)
{
	MENU * pmenu;
        
    	pmenu=CreateMenu("ҵ�񱨱�",1);
    	if(pmenu==NULL) return;
    
	BindMenu(pmenu,"�ʹﱨ��",sJym,BbAirSd);
	BindMenu(pmenu,"�ֽ���֧����",sJym,BbAirXj);
	AddMenu(pmenu,"  0.��  ��",NULL);   	
    	ShowMenu(pmenu);
	DropMenu(pmenu);
}

void QueryAccountData(long nType,char * sItem,char * sJym)
{
	clrscr();
	QueryLocal("������          ���ʲ��ɹ�����",
      	           "%s%ld","select distinct cjym,nlkrq from dl$zwmx where ncsbz=1 and cjym=%s order by nlkrq",sJym);
}
