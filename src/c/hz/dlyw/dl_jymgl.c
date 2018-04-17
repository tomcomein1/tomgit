

#include"dlpublic.h"
#include"tools.h"
#include"query.h"
#include"tasktool.h"
#include"newtask.h"
#include"dl_jymgl.h"

extern char sTcpipErrInfo[81];
extern char sSysJsdh[10];

void CxExistJym()
{
   clrscr();
   
   QueryLocal(
   "  ������            ��������                  ����ģʽ    ���  ����Դ��ַ     ",
   "%s %s %ld %ld[1:����;2:����;3:����;] %s",
   "select cjym,cjymc,ncxms,nbl,csjydz from dl$jym"
   );

}

void AddDsJym()
{
    
    DIALOG * pd;
    long nRetVal,nWhere;
    long nGsbh=0,nYwbh=0,nCxms=0,nTxfs=0;
    char sGsmc[21]=".",sYwmc[21]=".",sJym[16]="01",sJymc[51]="����";
    char sJh[7]=".",sJm[21]=".",sSjydz[21]="dldj";
    char sbuf[128];
    char * sMessage[7]={
    	   "�����뱻ί�й�˾�ı�ţ����س�����ѯ...",
    	   "�����뱻����ҵ��ı�ţ����س�����ѯ...",
    	   "ͨ�ŷ�ʽ: 0:���ؼ��� 1:����ʵʱ 2:��ؼ��� 3:���ʵʱ...",
    	   "������ӵ�иý��׵ľֺŵ�ǰ��λ...",
   	   "�����뽻��ģʽ������һ������...",
    	   "�������ҵ�����շ����ص�IP��ַ�����...",
    	   "���޸��Զ�������ҵ�����ƣ����س�����ʾȷ��..."};
    if(!(pd=CreateDialog(60,21,FRAME))) return;


    AddDialog(pd,"sep1",SPACELINE,1);    //1
    AddDialog(pd,"static",STATIC_TEXT,"              ��  �� �� �� �� �� �� ��  ��\n");//2
    AddDialog(pd,"sep2",SEPARATOR,"~~\n");
    AddDialog(pd,"ngsbh",INPUT,"  ��˾���:%-3d",&nGsbh);    //4;1
    AddDialog(pd,"cgsmc",OUTPUT,"           ��˾����:%-21.21s\n\n",sGsmc);    //5
    AddDialog(pd,"nywbh",INPUT,"  ҵ����:%-4d",&nYwbh);    //6;2
    AddDialog(pd,"cywmc",OUTPUT,"          ҵ������:%-21.21s\n\n",sYwmc);    //7
    AddDialog(pd,"ntxfs",INPUT,"  ͨ�ŷ�ʽ:%-1d",&nTxfs);    //8;2
    AddDialog(pd,"cjh",INPUT,"             ��    ��:%-6.6s\n\n",sJh);    //9;2
    AddDialog(pd,"njyms",INPUT,"  ����ģʽ:%-5d",&nCxms);    //10
    AddDialog(pd,"csjydz",INPUT,"         ����Դ��ַ:%-20.20s\n",sSjydz);    //11
    AddDialog(pd,"sep3",SEPARATOR,"~~\n");//12
    AddDialog(pd,"cjym",OUTPUT,"  ������:%-15.15s\n\n",sJym);//13
    AddDialog(pd,"cjymc",INPUT,"  ��������:%-40.40s\n\n",sJymc);//14

    clrscr();
    ShowDialog(pd,10,2);
    RefreshDialog(pd,0);
        
    nWhere=1;
    for(;;)
    {
 	RefreshDialog(pd,0);
 	SetDialogInfo(pd,sMessage[nWhere-1]);

    	nRetVal=ActiveDialog(pd,nWhere,"ngsbh,nywbh,ntxfs,cjh,njyms,csjydz,cjymc");

    	switch(nRetVal)
    	{
    	   case KEY_RETURN:
    	        break;

    	   case KEY_ESC:
    	        DropDialog(pd);
    	        return;

    	   case KEY_LEFT:
    	   case KEY_UP:
    	        if(nWhere>1) nWhere--;
    	        continue;

    	   default:
    	        continue;
    	}        
    
        switch(nWhere)
        {
        
           case 1:
                nRetVal=RunSelect("select cgsmc from dl$gs where ngsbh=%ld into %s",
                     nGsbh,sGsmc);
                if(nRetVal<=0)
                {
             	   savewin();
             	   QueryLocal("  ��˾���            ��˾����           ",
             	              "%ld%s",
             	              "select ngsbh,cgsmc from dl$gs order by ngsbh");
             	   popwin();
                   continue;
                }   
                nWhere++;
                sprintf(sJym,"01%03ld",nGsbh);
                sprintf(sJymc,"����%s",sGsmc);
                continue;
             	           
           case 2:
                nRetVal=RunSelect("select cywmc from dl$ywzl where nywbh=%ld into %s",
                     nYwbh,sYwmc);
                if(nRetVal<=0)
                {
             	   savewin();
             	   QueryLocal("  ҵ����            ҵ������           ",
             	              "%ld%s",
             	              "select nywbh,cywmc from dl$ywzl order by nywbh");
             	   popwin();
                   continue;
                }   
                nWhere++;
                sprintf(sJym,"01%03ld%03ld",nGsbh,nYwbh);
                sprintf(sJymc,"����%s%s",sGsmc,sYwmc);
                continue;
             	                  
           case 3:
                if(nTxfs>=0 && nTxfs<=3)
                {
                   sprintf(sJym,"01%03ld%03ld%1ld",nGsbh,nYwbh,nTxfs);
                   nWhere++;
                }
                continue;
         

           case 4:
                alltrim(sJh);
                nRetVal=RunSelect("select cjm from dl$jh where substr(cjh,1,6)=%s \
                                   and nlb=3 into %s",sJh,sJm);
                if(nRetVal<=0)
                {
             	   savewin();
             	   QueryLocal("  �ؾֺ�            �ؾ���           ",
             	              "%s%s",
             	              "select substr(cjh,1,6),cjm from dl$jh where \
             	              nlb=3 order by cjh");
             	   popwin();
                   continue;
                }   
                nWhere++;
                sprintf(sJym,"01%03ld%03ld%1ld%6s",nGsbh,nYwbh,nTxfs,sJh);
                sprintf(sJymc,"����%s%s%s",sJm,sGsmc,sYwmc);
                continue;
           case 5:
           case 6:
                alltrim(sSjydz);
                nRetVal=1;
                if(nRetVal) nWhere++;

                continue;
           case 7:
                alltrim(sJymc);
                SetDialogInfo(pd,"���Ҫ�����˽�����(y/n)");
                nRetVal=keyb();
                if(nRetVal=='y' || nRetVal=='Y')
                {
                    nRetVal=0;
                    sprintf(sbuf,"���ڽ���������:%s,���Ժ�...",sJym);
                    SetDialogInfo(pd,sbuf);

                    AutoRunTask("dldj",TASK_CREATE_DS_JYM,"%s%s%s%ld","%ld%s",
                                sJym,sJymc,sSjydz,nCxms,&nRetVal,sbuf);

                    if(!nRetVal)
                    	strcpy(sbuf,sTcpipErrInfo);
                    SetDialogInfo(pd,sbuf);
                    keyb();
                    
                    nGsbh=0;nYwbh=0;nCxms=0;nTxfs=0;
                    strcpy(sGsmc,".");
                    strcpy(sYwmc,".");
                    strcpy(sJym,"01");
                    strcpy(sJymc,"����");
                    strcpy(sJh,".");
                    strcpy(sJm,".");
                                
                }
                nWhere=1;
                continue;
        }
    }//end for
}

void AddSpJym()
{
    
    DIALOG * pd;
    long nRetVal,nWhere;
    long nGsbh=0,nYwbh=0,nCustomer=0,nIdentified=0,nMode;
    char sGsmc[21]=".",sYwmc[21]=".",sJym[16]="02",sJymc[51]="����";
    char sJh[7]=".",sJm[21]=".";
    char sbuf[128];
    char * sMessage[7]={
    	   "�����뱻ί�й�˾�ı�ţ����س�����ѯ...",
    	   "�����뱻����ҵ��ı�ţ����س�����ѯ...",
    	   "������ӵ�иý��׵ľֺŵ�ǰ��λ...",
    	   "����Ʒ������ʱ�Ƿ���Ҫ¼��ͻ���Ϣ   0������Ҫ 1����Ҫ",
    	   "����Ʒ�Ƿ���Ҫһ��Ψһ�ı�ʶ�������� 0������Ҫ 1����Ҫ",
    	   "���޸��Զ�������ҵ�����ƣ����س�����ʾȷ��..."};
    
    if(!(pd=CreateDialog(60,21,FRAME))) return;
    AddDialog(pd,"sep1",SPACELINE,1);   //1
    AddDialog(pd,"static",STATIC_TEXT,"              ��  �� �� �� �� �� �� ��  ��\n");//2
    AddDialog(pd,"sep2",SEPARATOR,"~~\n");//3
    AddDialog(pd,"ngsbh",INPUT,"  ��˾���:%-3d",&nGsbh);//4,1
    AddDialog(pd,"cgsmc",OUTPUT,"           ��˾����:%-21.21s\n\n",sGsmc); //5
    AddDialog(pd,"nywbh",INPUT,"  ҵ����:%-4d",&nYwbh);//6,2
    AddDialog(pd,"cywmc",OUTPUT,"          ҵ������:%-21.21s\n\n",sYwmc);//7
    AddDialog(pd,"cjh",INPUT,"  ��    ��:%-6.6s",sJh);//8,3    
    AddDialog(pd,"cjm",OUTPUT,"        ��    ��:%-21.21s\n\n",sJm);//9,3    
    AddDialog(pd,"nkhxxsx",INPUT,"  �ͻ���Ϣ����: %-1d",&nCustomer);//10,4
    AddDialog(pd,"nspbhsx",INPUT,"        ��Ʒ�������: %-1d\n",&nIdentified);//11,5
    AddDialog(pd,"sep3",SEPARATOR,"~~\n");//12
    AddDialog(pd,"cjym",OUTPUT,"  ������:%-15.15s\n\n",sJym);//13
    AddDialog(pd,"cjymc",INPUT,"  ��������:%-40.40s\n\n",sJymc);//14,6
    clrscr();
    ShowDialog(pd,10,2);
    RefreshDialog(pd,0);
        
    nWhere=1;
    for(;;)
    {
 	RefreshDialog(pd,0);
 	SetDialogInfo(pd,sMessage[nWhere-1]);
    	nRetVal=ActiveDialog(pd,nWhere,"ngsbh,nywbh,cjh,nkhxxsx,nspbhsx,cjymc");

    	switch(nRetVal)
    	{
    	   case KEY_RETURN:
    	        break;

    	   case KEY_ESC:
    	        DropDialog(pd);
    	        return;

    	   case KEY_LEFT:
    	   case KEY_UP:
    	        if(nWhere>1) nWhere--;
    	        continue;

    	   default:
    	        continue;
    	}        
    
    
        switch(nWhere)
        {
        
           case 1:
                nRetVal=RunSelect("select cgsmc from dl$gs where ngsbh=%ld into %s",
                     nGsbh,sGsmc);
                if(nRetVal<=0)
                {
             	   savewin();
             	   QueryLocal("  ��˾���            ��˾����           ",
             	              "%ld%s",
             	              "select ngsbh,cgsmc from dl$gs order by ngsbh");
             	   popwin();
                   continue;
                }   
                nWhere++;
                sprintf(sJym,"02%03ld",nGsbh);
                sprintf(sJymc,"����%s",sGsmc);
                continue;
             	           
           case 2:
                nRetVal=RunSelect("select cywmc from dl$ywzl where nywbh=%ld into %s",
                     nYwbh,sYwmc);
                if(nRetVal<=0)
                {
             	   savewin();
             	   QueryLocal("  ҵ����            ҵ������           ",
             	              "%ld%s",
             	              "select nywbh,cywmc from dl$ywzl order by nywbh");
             	   popwin();
                   continue;
                }   
                nWhere++;
                sprintf(sJym,"02%03ld%04ld",nGsbh,nYwbh);
                sprintf(sJymc,"����%s%s",sGsmc,sYwmc);
                continue;
             	                  
           case 3:
                alltrim(sJh);
                nRetVal=RunSelect("select cjm from dl$jh where substr(cjh,1,6)=%s \
                                   and nlb=3 into %s",sJh,sJm);
                if(nRetVal<=0)
                {
             	   savewin();
             	   QueryLocal("  �ؾֺ�            �ؾ���           ",
             	              "%s%s",
             	              "select substr(cjh,1,6),cjm from dl$jh where \
             	              nlb=3 order by cjh");
             	   popwin();
                   continue;
                }   
                nWhere++;
                sprintf(sJym,"02%03ld%04ld%6s",nGsbh,nYwbh,sJh);
                sprintf(sJymc,"����%s%s%s",sJm,sGsmc,sYwmc);
                continue;
           case 4:
                nWhere+=(nCustomer==0 || nCustomer==1);
                continue;
           case 5:
                nWhere+=(nIdentified==0 || nIdentified==1);
                continue;
           case 6:
                alltrim(sJymc);
                SetDialogInfo(pd,"���Ҫ�����˽�����(y/n)");
                nRetVal=keyb();
                if(nRetVal=='y' || nRetVal=='Y')
                {
                    nRetVal=0;
                    nMode=0;
                    if(nCustomer) nMode|=MODE_CUSTOMER;
                    if(nIdentified) nMode|=MODE_IDENTIFIED;
                    sprintf(sbuf,"���ڽ���������:%s,���Ժ�...",sJym);
                    SetDialogInfo(pd,sbuf);

                    AutoRunTask("dldj",TASK_CREATE_SP_JYM,"%s%s%ld","%ld%s",
                                sJym,sJymc,nMode,&nRetVal,sbuf);

                    if(!nRetVal)
                    	strcpy(sbuf,sTcpipErrInfo);
                    SetDialogInfo(pd,sbuf);
                    keyb();
                    
                    nGsbh=0;nYwbh=0;
                    strcpy(sGsmc,".");
                    strcpy(sYwmc,".");
                    strcpy(sJym,"02");
                    strcpy(sJymc,"����");
                    strcpy(sJh,".");
                    strcpy(sJm,".");
                }
                nWhere=1;
                continue;
        }
    }//end for
}

void OpenManage()
{
    DIALOG * pd;
    MENU * pmn;
    long nRetVal,nWhy=0,nWhere,nRow,nCol,nId;
    char sJym[16]=".",sJymc[51]=".",sJh[10]=".";
    char sbuf[128],sWhy1[41]=".",sWhy2[41]=" ";
    char * sMessage[5]={"������Ҫ���ŵĽ����룬���س���ѡ��...",
                        "������ֺŻ�ֺŷ�Χ�����س�����ѯ...",
                        "�����뿪�ŷ�ʽ��0������  1����ֹ...",
                        "�����뿪�Ż��ֹ��ԭ���Ա����Ա֪��...",
                        "�����뿪�Ż��ֹ��ԭ���Ա����Ա֪��..."};
    
    if(!(pd=CreateDialog(60,18,FRAME))) return;
    AddDialog(pd,"sep1",SPACELINE,1);  
    AddDialog(pd,"static",STATIC_TEXT,"              ��  �� �� �� �� �� �� �� ��\n");
    AddDialog(pd,"sep2",SEPARATOR,"~~\n");
    AddDialog(pd,"cjym",INPUT,"  �� �� ��:%-15s\n\n",sJym);
    AddDialog(pd,"cjymc",OUTPUT,"  ��������:%-40.40s\n\n",sJymc); 
    AddDialog(pd,"cjh",INPUT,"  ��    ��:%-9.9s\n\n",sJh);
    AddDialog(pd,"nkffs",INPUT,"  ���ŷ�ʽ:%-1d\n\n",&nWhy);
    AddDialog(pd,"cyysm1",INPUT,"  ԭ��˵��:%-40.40s\n",sWhy1);
    AddDialog(pd,"cyysm2",INPUT,"           %-40.40s\n\n",sWhy2);
    clrscr();
    ShowDialog(pd,10,4);
    RefreshDialog(pd,0);
    pmn=CreateJymPopMenu(0);
        
    nWhere=1;
    for(;;)
    {
 	RefreshDialog(pd,0);
 	SetDialogInfo(pd,sMessage[nWhere-1]);
    	nRetVal=ActiveDialog(pd,nWhere,"cjym,cjh,nkffs,cyysm1,cyysm2");

    	switch(nRetVal)
    	{
    	   case KEY_RETURN:
    	        break;

    	   case KEY_ESC:
    	        DropDialog(pd);
    	        DropMenu(pmn);
    	        return;

    	   case KEY_LEFT:
    	   case KEY_UP:
    	        if(nWhere>1) nWhere--;
    	        continue;

    	   default:
    	        continue;
    	}        
        switch(nWhere)
        {
           case 1:
                nRetVal=RunSelect("select cjymc from dl$jym where cjym=%s into %s",
                        sJym,sJymc);
                if(nRetVal==0)
                {
                    GetDialogPosition(pd,"cjym",&nRow,&nCol);
                    savewin();
                    nId=PopMenu(pmn,nCol+12,nRow,18);
                    popwin();
                    if(nId<=0) continue;
                    strcpy(sJym,GetAddition(pmn,nId));
                    continue;
                }
                nWhere++;
                continue;
           case 2:
                alltrim(sJh);
                if(strlen(sJh)<4)
                {
                     savewin();
                     QueryLocal("  �ֺ�              ����            ",
                                "%s%s",
                                "select cjh,cjm from dl$jh where nlb>=3 and \
                                substr(cjh,1,4)=substr(%s,1,4) order by cjh,nlb",
                                sSysJsdh);
                     popwin();
                     continue;
                }     
                nWhere++;
                continue;
           case 3:
                if(nWhy==1)
                {
                   nWhere=4;
                   continue;
                }   
                if(nWhy==0) nWhere=4;
           case 4:
                if(nWhy==1)
                {
                   nWhere=5;
                   continue;
                }   
           case 5:
                nWhere=0;
                SetDialogInfo(pd,"���Ҫ���Ŵ˽�����(y/n)");
                nRetVal=keyb();
                if(nRetVal=='y' || nRetVal=='Y')
                {
                    nRetVal=0;


                    sprintf(sbuf,"���ڲ���,���Ժ�...");
                    SetDialogInfo(pd,sbuf);
                    
                    sprintf(sbuf,"%s%s",sWhy1,sWhy2);
                    AutoRunTask("dldj",TASK_OPEN_JYM,"%s%s%s%ld","%ld%s",
                                sJym,sJh,sbuf,nWhy,&nRetVal,sbuf);

                    if(!nRetVal)
                    	strcpy(sbuf,sTcpipErrInfo);
                    SetDialogInfo(pd,sbuf);
                    keyb();

                    strcpy(sJym,".");
                    strcpy(sJymc,".");
                    strcpy(sJh,".");
                }
                nWhere=1;
                continue;     
        }//end case
    }//end for
}

void DeleteJym()
{
    DIALOG * pd;
    MENU * pmn;
    long nRetVal,nCol,nRow,nId;
    char sJym[16]=".",sJymc[51]=".";
    char sbuf[128];
    
    if(!(pd=CreateDialog(60,12,FRAME))) return;
    AddDialog(pd,"sep1",SPACELINE,1);  
    AddDialog(pd,"static",STATIC_TEXT,"                 ��  ɾ �� �� �� ��  ��\n");
    AddDialog(pd,"sep2",SEPARATOR,"~~\n");
    AddDialog(pd,"cjym",INPUT,"  �� �� ��:%-15s\n\n",sJym);
    AddDialog(pd,"cjymc",OUTPUT,"  ��������:%-40.40s\n\n",sJymc); 
    
    clrscr();
    ShowDialog(pd,10,6);
    pmn=CreateJymPopMenu(0);
        
    for(;;)
    {
 	RefreshDialog(pd,0);
 	SetDialogInfo(pd,"������Ҫɾ���Ľ����룬���س���ѡ��...");
    	nRetVal=ActiveDialog(pd,1,"cjym");

    	switch(nRetVal)
    	{
    	   case KEY_RETURN:
    	        break;

    	   case KEY_ESC:
    	        DropDialog(pd);
    	        DropMenu(pmn);
    	        return;
    	   default:
    	        continue;
    	}        
        nRetVal=RunSelect("select cjymc from dl$jym where cjym=%s into %s",
                          sJym,sJymc);
        if(nRetVal==0)
        {
           GetDialogPosition(pd,"cjym",&nRow,&nCol);
           savewin();
           nId=PopMenu(pmn,nCol+12,nRow,18);
           popwin();
           if(nId<=0) continue;
           strcpy(sJym,GetAddition(pmn,nId));
           continue;
        }
        RefreshDialog(pd,0);
        SetDialogInfo(pd,"���Ҫɾ���˽�����(y/n)");
        nRetVal=keyb();
        if(nRetVal=='y' || nRetVal=='Y')
        {
             nRetVal=0;
             SetDialogInfo(pd,"����ɾ��������,���Ժ�...");
             
             AutoRunTask("dldj",TASK_DELETE_JYM,"%s","%ld%s",
                         sJym,&nRetVal,sbuf);

             if(!nRetVal)
             	strcpy(sbuf,sTcpipErrInfo);
             SetDialogInfo(pd,sbuf);
             keyb();

             strcpy(sJym,".");
             strcpy(sJymc,".");
        }
        continue;     
    }//end for;
}
/*--------------------------------------------------------------
������CreateJymPopMenu
���ܣ�����ҵ��ѡ��˵�
������nlx��1�������ҵ�� 2�������ҵ�� 3������ҵ��
���أ��˵�ָ��
*--------------------------------------------------------------*/
MENU * CreateJymPopMenu(long nlx)
{
    
    MENU * pmn;
    char sJym[16],sJymc[51],sTemp[60];
    long nId,nSyzt;
    
    
    pmn=CreateMenu("MENU",0);
    
    if(pmn==NULL) return NULL;
    if(nlx)
       nId=OpenCursor("select cjym,cjymc from dl$jym where nbl=%ld",nlx);
    else
       nId=OpenCursor("select cjym,cjymc from dl$jym");
           
    if(nId<0)
    {
         DropMenu(pmn);
         HiError(nId,"Error!");
         return NULL;
    }	                
    
    for(;;)
    {
       	 if(FetchCursor(nId,"%s%s",sJym,sJymc)<=0) break;
         nSyzt=-1;
         RunSelect("select nsyzt from dl$jymkzfb where cjym=%s and cjh=%s into %ld",
                   sJym,sSysJsdh,&nSyzt);
         switch(nSyzt)
         {
         	case 0:          
         	     sprintf(sTemp,"[��]%s",sJymc);
     	             break;
            	case 1:
         	    sprintf(sTemp,"[��]%s",sJymc);
                    break;
                default:
         	    sprintf(sTemp,"[��]%s",sJymc);
                    break;
                        
         }
         AddMenu(pmn,sTemp,sJym);
    }//end for
    CloseCursor(nId);
    return pmn;
}
void AddDbJym()
{
    
    DIALOG * pd;
    long nRetVal,nWhere;
    long nGsbh=0,nYwbh=0,nCxms=0;
    char sGsmc[21]=".",sYwmc[21]=".",sJym[16]="03",sJymc[51]="����";
    char sJh[7]=".",sJm[21]=".";
    char sbuf[128];
    char sSjydz[21]="dldj";
    char * sMessage[6]={
    	   "�����뱻ί�й�˾�ı�ţ����س�����ѯ...",
    	   "�����뱻����ҵ��ı�ţ����س�����ѯ...",
    	   "������ӵ�иý��׵ľֺŵ�ǰ��λ...",
    	   "�����뽻��ģʽ��һ������1000������)...",
           "������ʵ���׵�ַ:����(dldj),���(dlsj)...",
    	   "���޸��Զ�������ҵ�����ƣ����س�����ʾȷ��..."};
    
    if(!(pd=CreateDialog(60,21,FRAME))) return;
    AddDialog(pd,"sep1",SPACELINE,1);   //1
    AddDialog(pd,"static",STATIC_TEXT,"              ��  �� �� �� �� �� �� ��  ��\n");//2
    AddDialog(pd,"sep2",SEPARATOR,"~~\n");//3
    AddDialog(pd,"ngsbh",INPUT,"  ��˾���:%-3d",&nGsbh);//4,1
    AddDialog(pd,"cgsmc",OUTPUT,"           ��˾����:%-21.21s\n\n",sGsmc); //5
    AddDialog(pd,"nywbh",INPUT,"  ҵ����:%-4d",&nYwbh);//6,2
    AddDialog(pd,"cywmc",OUTPUT,"          ҵ������:%-21.21s\n\n",sYwmc);//7
    AddDialog(pd,"cjh",INPUT,"  ��    ��:%-6.6s",sJh);//8,3    
    AddDialog(pd,"cjm",OUTPUT,"        ��    ��:%-21.21s\n\n",sJm);//9,3    
    AddDialog(pd,"ncxms",INPUT,"  ����ģʽ: %-6d",&nCxms);//10,4
    AddDialog(pd,"csjydz",INPUT,"       ����Դ��ַ:%-20.20s\n",sSjydz);    //11
    AddDialog(pd,"sep3",SEPARATOR,"~~\n");//12
    AddDialog(pd,"cjym",OUTPUT,"  ������:%-15.15s\n\n",sJym);//13
    AddDialog(pd,"cjymc",INPUT,"  ��������:%-40.40s\n\n",sJymc);//14,6
    clrscr();
    ShowDialog(pd,10,2);
    RefreshDialog(pd,0);
        
    nWhere=1;
    for(;;)
    {
 	RefreshDialog(pd,0);
 	SetDialogInfo(pd,sMessage[nWhere-1]);
    	nRetVal=ActiveDialog(pd,nWhere,"ngsbh,nywbh,cjh,ncxms,csjydz,cjymc");

    	switch(nRetVal)
    	{
    	   case KEY_RETURN:
    	        break;

    	   case KEY_ESC:
    	        DropDialog(pd);
    	        return;

    	   case KEY_LEFT:
    	   case KEY_UP:
    	        if(nWhere>1) nWhere--;
    	        continue;

    	   default:
    	        continue;
    	}        
    
    
        switch(nWhere)
        {
        
           case 1:
                nRetVal=RunSelect("select cgsmc from dl$gs where ngsbh=%ld into %s",
                     nGsbh,sGsmc);
                if(nRetVal<=0)
                {
             	   savewin();
             	   QueryLocal("  ��˾���            ��˾����           ",
             	              "%ld%s",
             	              "select ngsbh,cgsmc from dl$gs order by ngsbh");
             	   popwin();
                   continue;
                }   
                nWhere++;
                sprintf(sJym,"03%03ld",nGsbh);
                sprintf(sJymc,"����%s",sGsmc);
                continue;
             	           
           case 2:
                nRetVal=RunSelect("select cywmc from dl$ywzl where nywbh=%ld into %s",
                     nYwbh,sYwmc);
                if(nRetVal<=0)
                {
             	   savewin();
             	   QueryLocal("  ҵ����            ҵ������           ",
             	              "%ld%s",
             	              "select nywbh,cywmc from dl$ywzl order by nywbh");
             	   popwin();
                   continue;
                }   
                nWhere++;
                sprintf(sJym,"03%03ld%04ld",nGsbh,nYwbh);
                sprintf(sJymc,"����%s%s",sGsmc,sYwmc);
                continue;
             	                  
           case 3:
                alltrim(sJh);
                nRetVal=RunSelect("select cjm from dl$jh where substr(cjh,1,6)=%s \
                                   and nlb=3 into %s",sJh,sJm);
                if(nRetVal<=0)
                {
             	   savewin();
             	   QueryLocal("  �ؾֺ�            �ؾ���           ",
             	              "%s%s",
             	              "select substr(cjh,1,6),cjm from dl$jh where \
             	              nlb=3 order by cjh");
             	   popwin();
                   continue;
                }   
                nWhere++;
                sprintf(sJym,"03%03ld%04ld%6s",nGsbh,nYwbh,sJh);
                sprintf(sJymc,"����%s%s%s",sJm,sGsmc,sYwmc);
                continue;
           case 4:
                if(nCxms>1000) nWhere++;
                continue;
           case 5:
                alltrim(sSjydz);
                nRetVal=1;
                /*if(strcmp(sSjydz,"dldj"))
                {
			if(AutoRunTask(sSjydz,TASK_GET_YD_JYM,"%s","%ld%s",sJym,&nRetVal,sbuf))
                	{
                	   strcpy(sbuf,sTcpipErrInfo);
                	   SetDialogInfo(pd,sbuf);
                	   keyb();
                	   continue;
                	}//end default
                	if(nRetVal)
                	{
                	  FetchStringToArg(sbuf,"%s|%s|",sJym,sJymc);
                	  RefreshDialog(pd,0);
                	}
                	SetDialogInfo(pd,sbuf);
                        keyb();
                }*/
                if(nRetVal) nWhere++;
                continue;
           case 6:
                alltrim(sJymc);
                SetDialogInfo(pd,"���Ҫ�����˽�����(y/n)");
                nRetVal=keyb();
                if(nRetVal=='y' || nRetVal=='Y')
                {
                    nRetVal=0;
                    sprintf(sbuf,"���ڽ���������:%s,���Ժ�...",sJym);
                    SetDialogInfo(pd,sbuf);
                        
                    AutoRunTask("dldj",TASK_CREATE_DB_JYM,"%s%s%s%ld","%ld%s",
                                sJym,sJymc,sSjydz,nCxms,&nRetVal,sbuf);

                    if(!nRetVal)
                    	strcpy(sbuf,sTcpipErrInfo);
                    SetDialogInfo(pd,sbuf);
                    keyb();
                    
                    nGsbh=0;nYwbh=0;
                    strcpy(sGsmc,".");
                    strcpy(sYwmc,".");
                    strcpy(sJym,"03");
                    strcpy(sJymc,"����");
                    strcpy(sJh,".");
                    strcpy(sJm,".");
                    strcpy(sSjydz,"dldj");
                }
                nWhere=1;
                continue;
        }
    }//end for
}
