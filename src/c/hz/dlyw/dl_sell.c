/***********************************************************************
**filename	�ļ���:dl_sell.c
**target	Ŀ��:����ϵͳ�л�������ۼ��ͻ��˻�
**create time	��������: 2001.3.19
**original programmer	����Ա:�²�[chenbo]
**edit  history	�޸���ʷ:
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
extern char sSysJm[JM_JM_LEN+1];/*����  cs2*/
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
     	if(MessageBox("    �����з������˻���¼û����ʽ��⣬���Ƚ�������ʽ����������ۣ�\
������������ҡ�Ҫ�鿴�������˻���ϸ��?",0)==KEY_YES)
        {
        	QueryLocal( "��Ʒ����               ��Ʒ��ʶ        ���    ����  �ӻ�����Ա        ����   ",
        	            "%s%s%ld[3:����;2:�˻�;]%ld%s%ld",
        	            "select cjymc,chwbz,nczlb,nsl,cjhczydh,nfhrq from dl$hwjcb \
        	             where  ((nczlb=3 and njlzt=0) or (nczlb=2 and njlzt=0)) \
                             and cfhczydh=%s",sSysSyz);
        }                     
     	return;
     }
    
     ///////////////////////////////////////////////////////////////////     
     pmsp=CreateGoodsMenu();
     if(!(pd=CreateDialog(60,21,FRAME))) return;
     AddDialog(pd,"static",STATIC_TEXT,"                 ��  ��  Ʒ  ��  ��  ��\n");
     AddDialog(pd,"set1",SEPARATOR,"__\n");
     
     AddDialog(pd,"cspmc",INPUT,"��Ʒ����:%-46.46s\n\n",sJymc);           //3
     AddDialog(pd,"nspsl",INPUT,"��Ʒ����:%8d",&nCount);                  //4
     AddDialog(pd,"cspbs",INPUT,"         ��Ʒ��ʶ:%-20.20s\n",sHwbz);    //5
     
     AddDialog(pd,"sep2",SEPARATOR,"__\n");

     AddDialog(pd,"cyhmc",INPUT,"�û�����:%-26.26s\n\n",sUserName);        //7
     AddDialog(pd,"czjhm",INPUT,"֤������:%-30.30s\n\n",sLicence);         //8
     AddDialog(pd,"cyhdz",INPUT,"�û���ַ����ϵ��ʽ:%-24.24s\n",sAddress); //9

     AddDialog(pd,"sep3",SEPARATOR,"__");
     AddDialog(pd,"dspdj",OUTPUT,"��Ʒ����:%10.2f",&dPrice);
     AddDialog(pd,"dyszk",OUTPUT,"        Ӧ���ܿ�:%10.2f",&dMoney);
     //////////////////////////////////////////////////////////////////
     clrscr();
     ShowDialog(pd,10,2);
     for(;;)
     {
     	nwhere=1;   //start
        dMoney=0.0;
        nCount=0;
        nsum=0;
        detail=0;  //¼���û���Ϣ
        identify=0;//¼���Ʒ��ʶ
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
                        SetDialogInfo(pd,"��������Ʒ���ƣ��򰴻س���ѡ����Ʒ..");
                        break;

                   case 2:
                        sprintf(sInfo,"��������Ʒ������Ŀǰ�����Ϊ:%ld",nsum);
                        SetDialogInfo(pd,sInfo);
                        break;

                   case 3:
                        SetDialogInfo(pd,"�������Ʒ��ʶ���򰴻س���ѡ���Ʒ��ʶ..");
                        break;

                   case 4:
                   case 5:
                   case 6:
                        SetDialogInfo(pd,"������ͻ��ĸ��������Ϣ...");
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

                        SetDialogInfo(pd,"���ڲ�ѯ����Ʒ�Ŀ�棬���Ժ�...");
                        RunSelect("select nkcl from dl$hwkcb where \
                                   cjym=%s and csyz=%s into %ld",sJym,sSysSyz,&nsum);
                        if(nsum<=0)
                        {
                              SetDialogInfo(pd,"����Ʒ���޿�棬��������...");
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
                        SetDialogInfo(pd,"���ڼ����Ʒ��ʶ�����Ժ�...");

                        if(sHwbz[0]!='.')  
                           RunSelect("select count(*) from dl$hwjcb \
                                         where cjym=%s and cjhczydh=%s and chwbz=%s \
                                         and ((nczlb=1 and njlzt=1) or \
                                         (nczlb=3 and njlzt=2)) into %ld",
                                         sJym,sSysSyz,sHwbz,&nTest);
                        if(nTest<=0||sHwbz[0]=='.')
                        {                 
                             SetDialogInfo(pd,"��Ʒ��ʶ�����ڣ������γ���Ʒ��ʶ�˵������Ժ�...");
                             if(!(pmident=CreateIdentMenu(sJym))) continue;
                             SetDialogInfo(pd,"��Ӳ˵���ѡ��һ����Ʒ��ʶ������Ӧ���ּ�...");
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
        
        SetDialogInfo(pd,"��ѡ��ɷѷ�ʽ��1.�ֽ� 2.�̿� 3.ת��֧Ʊ ESC����...");
        
        for(;;)
        {
        	nKey=keyb();
        	if(nKey==KEY_ESC||nKey=='1'||nKey=='2'||nKey=='3') break;
        }
        if(nKey==KEY_ESC) continue;
        //������ˮ��
        GenerateLsh(1,sSysJsdh,sJym,sLsh);
        
        //��ʼ�ɿ�
//      ��ˮ��|�û���־|�û�����|�ɿʽ|ʵ���ܿ��|����Ա����|ʱ����|�ɿ��·���|���|�·�|���|�·�|......
        
        alltrim(sHwbz);
        alltrim(sUserName);
        alltrim(sAddress);
        alltrim(sLicence);
        
        sprintf(sInfo,"%s@%s@%s",sLicence,sUserName,sAddress);
        sInfo[50]='\0';
        
        sprintf(sbuf,"%s|%s|%s|%ld|%.2lf|%s|%ld|%ld|",sLsh,sHwbz,sInfo,
                                    nKey-'0',dMoney,sSysCzydh,nSysSfm,nCount);

        //����Դ�ɿ�
        SetDialogInfo(pd,"����������Դ�ɷ�,���Ժ�...");
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

        //SetDialogInfo(pd,"�����޸Ŀ����,���Ժ�...");
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
                SetDialogInfo(pd,"��׼���ô�ӡ������'1'����ʼ��ӡ����,ESC����...");
                for(;;)
                {
                	nKey=keyb();
                	if(nKey==KEY_ESC||nKey=='1') break;
                }
                if(nKey=='1')  nPrintDssj(sLsh);
                continue;  //successfully

        }

        //�޸Ŀ�治�ɹ��Ĵ���,������Դ�������νɿ�
        SetDialogInfo(pd,"���ڳ������νɷѣ���ȴ�...");
        GenerateLsh(1,sSysJsdh,sJym,sInfo);//���ɳ�����ˮ�� 
    
        // ������ˮ��|��������ˮ��|ʵ�ɿ��|����Ա����|ʱ����|
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
     AddDialog(pd,"static",STATIC_TEXT,"                 ��  ��  ��  ��  ��  ��\n");
     AddDialog(pd,"sep1",SEPARATOR,"__\n");
     
     AddDialog(pd,"cspmc",INPUT,"��Ʒ����:%-46.46s\n\n",sJymc);           //3
     AddDialog(pd,"nspsl",INPUT,"��Ʒ����:%8d",&nCount);                  //4
     AddDialog(pd,"cspbs",INPUT,"         ��Ʒ��ʶ:%-20.20s\n",sHwbz);    //5
     
     AddDialog(pd,"sep2",SEPARATOR,"__\n");

     AddDialog(pd,"dspdj",OUTPUT,"��Ʒ����:%10.2f",&dPrice);
     AddDialog(pd,"dytzk",OUTPUT,"        Ӧ���ܿ�:%10.2f",&dMoney);
     //////////////////////////////////////////////////////////////////
     clrscr();
     
     for(;;)
     {
     	nwhere=1;//star
        dMoney=0.0;
        nCount=0;
        identify=0;//¼���Ʒ��ʶ
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
                        SetDialogInfo(pd,"��������Ʒ���ƣ��򰴻س���ѡ����Ʒ..");
                        break;

                   case 2:
                        SetDialogInfo(pd,"�������˻�����...");
                        break;

                   case 3:
                        SetDialogInfo(pd,"�������Ʒ��ʶ...");
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
                        SetDialogInfo(pd,"���ڼ����Ʒ��ʶ�����Ժ�...");

                        RunSelect("select count(*) from dl$hwjcb \
                                        where cjym=%s and chwbz=%s and nczlb=7 into %ld",
                                        sJym,sHwbz,&nTest);
                        if(nTest<=0)
                        {                 
                             SetDialogInfo(pd,"�����ڸ���Ʒ�����ۼ�¼...");
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
        
        SetDialogInfo(pd,"��'1'��ȷ���˻���ESC������...");
        
        for(;;)
        {
        	nKey=keyb();
        	if(nKey==KEY_ESC||nKey=='1') break;
        }
        if(nKey==KEY_ESC) continue;
        //������ˮ��
        GenerateLsh(1,sSysJsdh,sJym,sLsh);
        
        //��ʼ�ɿ�
//      ��ˮ��|�û���־|�û�����|�ɿʽ|ʵ���ܿ��|����Ա����|ʱ����|�ɿ��·���|���|�·�|���|�·�|......
        
        alltrim(sHwbz);
        
        sprintf(sbuf,"%s|%s|RECALL|%ld|%.2lf|%s|%ld|%ld|",sLsh,sHwbz,
                                   nKey-'0',-dMoney,sSysCzydh,nSysSfm,-nCount);

        //����Դ�ɿ�
        SetDialogInfo(pd,"����������Դ�ɿ�,���Ժ�...");
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
        
        //SetDialogInfo(pd,"�����޸Ŀ����,���Ժ�...");
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
        else   //�ɹ�
        {
                DropRecord(&rec);
        	SetDialogInfo(pd,"�ɿ�޸Ŀ��ɹ�...");
        	sleep(1);
        	continue;
        }

        //�޸Ŀ�治�ɹ��Ĵ���,������Դ�������νɿ�
        SetDialogInfo(pd,"���ڳ������νɷѣ���ȴ�...");
        GenerateLsh(1,sSysJsdh,sJym,sInfo);//���ɳ�����ˮ�� 
    
        // ������ˮ��|��������ˮ��|ʵ�ɿ��|����Ա����|ʱ����|
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
     	//�ɹ���Ĵ���
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
     AddDialog(pd,"static",STATIC_TEXT,"                 ��  ��  Ʒ  ��  ��  ��\n");
     AddDialog(pd,"sep1",SEPARATOR,"__\n");
     
     AddDialog(pd,"cspmc",INPUT,"��Ʒ����:%-46.46s\n\n",sJymc);           //3
     AddDialog(pd,"nspsl",INPUT,"��Ʒ����:%8d",&nCount);                  //4
     AddDialog(pd,"cspbs",INPUT,"         ��Ʒ��ʶ:%-20.20s\n",sHwbz);    //5
     
     AddDialog(pd,"sep2",SEPARATOR,"__\n");

     AddDialog(pd,"dspdj",OUTPUT,"��Ʒ����:%10.2f",&dPrice);
     AddDialog(pd,"dbszj",OUTPUT,"        �����ܿ�:%10.2f",&dMoney);
     //////////////////////////////////////////////////////////////////
     clrscr();
     
     for(;;)
     {
     	nwhere=1;//star
        dMoney=0.0;
        nCount=0;
        identify=0;//¼���Ʒ��ʶ
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
                        SetDialogInfo(pd,"��������Ʒ���ƣ��򰴻س���ѡ����Ʒ..");
                        break;

                   case 2:
                        SetDialogInfo(pd,"�����뱨������...");
                        break;

                   case 3:
                        SetDialogInfo(pd,"�������Ʒ��ʶ...");
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
                        	sprintf(sbuf,"����Ʒ�Ŀ����Ϊ:%ld",nKcl);
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
                        SetDialogInfo(pd,"���ڼ����Ʒ��ʶ�����Ժ�...");

                        if(sHwbz[0]!='.')  
                           RunSelect("select count(*) from dl$hwjcb \
                                         where cjym=%s and cjhczydh=%s and chwbz=%s \
                                         and ((nczlb=1 and njlzt=1) or \
                                         (nczlb=3 and njlzt=2)) into %ld",
                                         sJym,sSysSyz,sHwbz,&nTest);
                        if(nTest<=0||sHwbz[0]=='.')
                        {                 
                             SetDialogInfo(pd,"��Ʒ��ʶ�����ڣ������γ���Ʒ��ʶ�˵������Ժ�...");
                             if(!(pmident=CreateIdentMenu(sJym))) continue;
                             SetDialogInfo(pd,"��Ӳ˵���ѡ��һ����Ʒ��ʶ������Ӧ���ּ�...");
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
        
        SetDialogInfo(pd,"��'1'��ȷ�ϱ����ESC������...");
        
        for(;;)
        {
        	nKey=keyb();
        	if(nKey==KEY_ESC||nKey=='1') break;
        }
        if(nKey==KEY_ESC) continue;
        
        GenerateLsh(1,sSysJsdh,sJym,sLsh);//���ɳ�����ˮ�� 

        SetDialogInfo(pd,"�����޸Ŀ����,���Ժ�...");
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
        else   //�ɹ�
        {
                DropRecord(&rec);
        	SetDialogInfo(pd,"�����޸Ŀ��ɹ�...");
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
   long nSl;//����
   long year;
   long month;
   long day;
   double dYjke;//Ӧ�ɿ��
   double dXj;//�ֽ�
   double dCz;//����
   double dXyk;//���ÿ�
   double dSjk;//ʵ�ɿ�
   double dDj;//����
   char cCzydh[13];//����Ա����
   char cYhmc[31];//�û�����
   char cJym[JYM_JYM_LEN+1];//������
   char cSpmc[31];//������Ʒ����
   char cSpdw[11];//��Ʒ��λ
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
   
   pt=CreateTable(5,"|Ʒ�������                  |��λ  |����  |����    |���    |��ע          |");
   sprintf(buf,"�ֽ�:%.2lf\n֧Ʊ:%.2lf\n�̿�:%.2lf\n�ϼ�:%.2lf",dXj,dCz,dXyk,dYjke);
   FillTable(pt,2,"> > > > > %s ",buf);
   MergeTable(pt,CreateTable(1,
                    "|����д|                                            |                        |"));
   FillTable(pt,2,"%s%s%d%.2f%.2f",cSpmc,cSpdw,nSl,dDj,dYjke);
   to_Chinese(dYjke,buf);
   FillTable(pt,6,"> %s %.2f",buf,dYjke);
   year=nSysLkrq/10000;
   month=(nSysLkrq%10000)/100;
   day=(nSysLkrq%10000)%100;
   setprinton();
   output("                           �� �� ʡ �� �� �� �� �� ��\n\n");
   output("                                                            %4ld��%2ld��%2ld������\n",year,month,day);
   output("   ������λ:%-20.20s      NO.%s\n",cYhmc,sLsh);
   //ShowTable(pt,1,2);                 
   setprintoff();
   OutputTable(pt);//����д�����ô˺������ӡ�����
   setprinton();

   output("   ������λ:%-30.30s         ����Ա:%s\n",sSysJm,cCzydh);
   eject();
   setprintoff();
   DropTable(pt);
}




