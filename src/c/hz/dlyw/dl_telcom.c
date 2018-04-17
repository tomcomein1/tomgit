#include"dlpublic.h"
#include"tools.h"
#include"tasktool.h"
#include"newtask.h"
#include"dlprint.h"

extern char sTcpipErrInfo[81];
extern long nSysLkrq;
extern char sSysCzymc[10];
extern char sSysJh[11];
extern char sSysJm[20];

void TelecomCheckAccount(long nId,char * item,char * sJym)
{
    
    char sAnswer[1024]=".",sYhbz[21];
    long nLkrq=0,nRetVal,i;
    long nRet,nXh;
    
    GetSysLkrq();
    
    nRetVal=RunSelect("select * from dl$zwmx where ncsbz=1 and cjym=%s",sJym);
    
    if(nRetVal>0)
       RunSelect("select min(nlkrq) from dl$zwmx where ncsbz=1 and cjym=%s into %ld",
                      sJym,&nLkrq);

    if(nRetVal<0) return;
    
    if(nRetVal==0)
    {
    	nRetVal=RunSelect("select * from dl$zwmx where ncsbz=2 and cjym=%s and nlkrq=%ld",
    	             sJym,nSysLkrq);
    	
    	if(nRetVal>0)
    	{
    	   MessageBox("ϵͳ�Ѿ��ɹ����ʣ�",1);
		   return;
		}//endif
    }
                          
    clrscr(); 
    printf("\n            ����ʵ����ڣ�[%ld]",nLkrq);
    if(nLkrq==nSysLkrq||nRetVal==0)
    {
         if(MessageBox("  ���ѣ����ʺ���ֹ����ҵ����ȷ��Ҫ���ж����룿",0)!=KEY_YES) return;
    }
    else
    {
         if(MessageBox("  ��������δ���ʵļ�¼��ѡ��\"ȷ��\"���ȶ������ʣ�������Ӱ�챾��ҵ��",0)!=KEY_YES) return;
    }    	          
    clrscr();
    outtext("\n���ں͵��ŷ����ʣ��������Ҫ�ϳ�ʱ�䣬�����ĵȴ�...\n");
    
	if(AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sAnswer))
    {
       	outtext(sTcpipErrInfo);
       	keyb();
       	return;
    }
    if(!strncmp(sAnswer,"000000",6))  strcpy(sAnswer,"���ʳɹ��������������...");
    outtext(sAnswer);
    keyb();
}

//����������ӡ��ʽ
void PrintMyGddhfp(char * sJym,char * sLsh)
{
    PRINTFORM * ppf;
    long nprint;
    long nId,nRetVal;
    char sFpmc[45][22];
    double dFpje[45],dZk=0;
    long nCol,nCount=0;
    char sBuffer[1024],space[30],sTemp[360],sRow[30],sZk[21]=".";
    long nXh=0;
     
     clearline(23);
     if(!(ppf=CreatePrintForm("mygddhdsfp"))) return ;
     if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",sJym,sLsh)<0)
     {
 	DropPrintForm(ppf);
	 return ;
     }
     for(nprint=1;nprint<=ppf->nCount;nprint++)
     {
        //////////////////////////////////////////     
     	     for(nCount=0;nCount<45;nCount++) dFpje[nCount]=0;
             RunSelect("select nxh,dsfmx21,dsfmx22 from dl$ywmx_%t "
                       "where clsh=%s and njscz2=%ld into %ld%lf%lf",
               sJym,sLsh,nprint,&nXh,&dFpje[37],&dFpje[38]);
	     nCount=0;
	     memset(sBuffer,'\0',sizeof(sBuffer));
	     memset(space,'\0',30);
	     //strcpy(sFpmc[36],"���ɽ�|ΥԼ��");
             strcpy(sFpmc[37],"�������");
             strcpy(sFpmc[38],"�������");
             nId=OpenCursor("select cfpmc,dfpje from dl$dxfp where nxh=%ld and nfpzs=%ld",nXh,nprint);
	     if(nId<0)
	     {
	        outtext("\n���α����(dl$dxfp)");
	        CloseCursor(nId);
	        return;
	     }
	
	     while(1)
	     {
	        nRetVal=FetchCursor(nId,"%s%lf",sFpmc[nCount],&dFpje[nCount]);
	        if(nRetVal<=0) break;
	        nCount++;
	     }
	     CloseCursor(nId);
	     
	     //��֯�ɴ�ӡ��ʽ
	     strcpy(space,"    "); //�м�ո���
	     strcpy(sRow,"        ");    //��߽�ո���
	     for(nCol=0;nCol<=12;nCol++)
	     {
	        alltrim(sFpmc[nCol]);
	        if(dFpje[nCol]==0 && nCol==nCount) break;
	
	        if(dFpje[nCol+23]!=0)
	        {
	          sprintf(sTemp,"\n%s%-20.20s%-10.2lf%s%-20.20s%-10.2lf%s%-20.20s%-10.2lf",
	                        sRow,sFpmc[nCol],dFpje[nCol],space,sFpmc[nCol+11],dFpje[nCol+11],
	                        space,sFpmc[nCol+23],dFpje[nCol+23]);
	        }
	        else if(dFpje[nCol+11]!=0)
	           sprintf(sTemp,"\n%s%-20.20s%-10.2lf%s%-20.20s%-10.2lf",
	               sRow,sFpmc[nCol],dFpje[nCol],space,sFpmc[nCol+11],dFpje[nCol+11]);
	        else if(dFpje[nCol]!=0)
           		sprintf(sTemp,"\n%s%-20.20s%-10.2lf",
	               sRow,sFpmc[nCol],dFpje[nCol]);
	        else continue;

	        strcat(sBuffer,sTemp);     
	     }
	     /*if(dFpje[36]!=0)
	     {
	       sprintf(sTemp,"\n%s%-20.20s%-10.2lf",sRow,sFpmc[36],dFpje[36]);
	       strcat(sBuffer,sTemp);
	       nCol++;
	     }*/
	 
	     for(;nCol<=10;nCol++) strcat(sBuffer,"\n");
	     sprintf(sTemp,"\n%-66.66s%-20.20s%-10.2lf",sRow,sFpmc[37],-dFpje[37]);
	     strcat(sBuffer,sTemp);
	     sprintf(sTemp,"\n%-66.66s%-20.20s%-10.2lf",sRow,sFpmc[38],-dFpje[38]);
	     strcat(sBuffer,sTemp);
	     memset(sTemp,'\0',strlen(sTemp));

	     sprintf(sTemp,"%-28.28s%-8.8s%-24.24s%20.20s������",
	                  space,sSysCzymc,space,sSysJm);
	     
	     //strcat(sBuffer,sTemp);
        //////////////////////////////////////////
        gotoxy(14,22);
        outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,
                   ppf->nCount);
        if(keyb()==KEY_ESC)
        {
                DropPrintForm(ppf);
               return ;
        }
       
        OutputPrintForm(ppf,
                NULL,nprint,(nSysLkrq%10000)/100,(nSysLkrq%10000)%100,sBuffer,sTemp);
     }//endfor
     DropPrintForm(ppf);
     return;
}

void PrintPzhGddhfp(char * sJym,char * sLsh)
{
    PRINTFORM * ppf;
    long nprint;
    long nId,nRetVal;
    char sFpmc[45][22];
    double dFpje[45],dZk=0;
    long nCol,nCount=0;
    char sBuffer[1024],space[30],sTemp[360],sRow[30],sZk[21]=".";
    long nXh=0;

	 clearline(23);
 	if(!(ppf=CreatePrintForm("mygddhdsfp"))) return ;
 	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",sJym,sLsh)<0)
 	{
 		DropPrintForm(ppf);
  		return ;
 	}
	for(nprint=1;nprint<=ppf->nCount;nprint++)
 	{
        //////////////////////////////////////////
             for(nCount=0;nCount<45;nCount++) dFpje[nCount]=0;
             RunSelect("select nxh,dSfmx17,dsfmx21,dsfmx22,dsjk from dl$ywmx_%t "
                       "where clsh=%s and njscz2=%ld into %ld%lf%lf%lf%lf",
               sJym,sLsh,nprint,&nXh,&dFpje[36],&dFpje[37],&dFpje[38],&dZk);
             nCount=0;
             memset(sBuffer,'\0',sizeof(sBuffer));
             memset(space,'\0',30);
             strcpy(sFpmc[36],"���ɽ�");
             strcpy(sFpmc[37],"�������");
             strcpy(sFpmc[38],"�������");
             nId=OpenCursor("select cfpmc,dfpje from dl$dxfp where nxh=%ld and nfpzs=%ld",nXh,nprint);
             if(nId<0)
             {
                outtext("\n���α����(dl$dxfp)");
   		CloseCursor(nId);
                return;
             }

             while(1)
             {
                nRetVal=FetchCursor(nId,"%s%lf",sFpmc[nCount],&dFpje[nCount]);
                if(nRetVal<=0) break;
                nCount++;
             }
             CloseCursor(nId);

             //\327\351\326\257\263\311\264\362\323\241\270\361\312\275
             strcpy(space,"    "); //\326\320\274\344\277\325\270\361\312\375
             strcpy(sRow,"                  ");    //\327\363\261\337\275\347\277\325\270\361\312\375
             for(nCol=0;nCol<=9;nCol++)
             {
                alltrim(sFpmc[nCol]);
                if(dFpje[nCol]==0 && nCol==nCount) break;
				if(dFpje[nCol+23]!=0)
                {
                  sprintf(sTemp,"\n%s%-20.20s%-10.2lf%s%-20.20s%-10.2lf%s%-20.20s%-10.2lf",
                                sRow,sFpmc[nCol],dFpje[nCol],space,sFpmc[nCol+11],dFpje[nCol+11],
                                space,sFpmc[nCol+23],dFpje[nCol+23]);
                }
                else if(dFpje[nCol+11]!=0)
                   sprintf(sTemp,"\n%-20.20s%-10.2lf%s%-20.20s%-10.2lf",
                       sFpmc[nCol],dFpje[nCol],space,sFpmc[nCol+11],dFpje[nCol+11]);
                else if(dFpje[nCol]!=0)
                   sprintf(sTemp,"\n%s%-20.20s%-10.2lf",
                       sRow,sFpmc[nCol],dFpje[nCol]);
                else continue;

                strcat(sBuffer,sTemp);
             }
             if(dFpje[36]!=0)
			{
               sprintf(sTemp,"\n%s%-20.20s%-10.2lf",sRow,sFpmc[36],dFpje[36]);
               strcat(sBuffer,sTemp);
               nCol++;
             }

             for(;nCol<9;nCol++) strcat(sBuffer,"\n");
             sprintf(sTemp,"\n%-60.60s%-20.20s%-10.2lf",sRow,sFpmc[37],-dFpje[37]);
             strcat(sBuffer,sTemp);
             sprintf(sTemp,"\n%-60.60s%-20.20s%-10.2lf",sRow,sFpmc[38],-dFpje[38]);
             strcat(sBuffer,sTemp);
             memset(sTemp,'\0',strlen(sTemp));

             if(strstr(sSysJh,"2721") )
             {
                //RunSelect("select ROUND(sum(dfpje),0) from dl$dxfp where nxh=%ld and nfpzs=%ld into %lf",nXh,nprint,&dZk);
                to_Chinese(dZk,sZk);
				 sprintf(sTemp,"\n%s%-20.20s%40.40s%-12.2lf\n%s%-20.20s%40.40s%s\323\312\325\376\276\326\n\n",
                          sRow,sZk,space,dZk,sRow,sSysCzymc,space,sSysJm);
             }
             else
             sprintf(sTemp,"\n%-28.28s%-8.8s%-24.24s%20.20s������\n\n\n",
                          space,sSysCzymc,space,sSysJm);
             strcat(sBuffer,sTemp);
        //////////////////////////////////////////
        gotoxy(14,22);
        outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,
                   ppf->nCount);
        if(keyb()==KEY_ESC)
        {
                DropPrintForm(ppf);
               return ;
        }
		OutputPrintForm(ppf, NULL,nprint);
                        //(nSysLkrq%10000)/100,(nSysLkrq%10000)%100);

        setprinton();
        output(sBuffer);
     	eject();
        setprintoff();
     }//endfor
     DropPrintForm(ppf);
     return;
}
