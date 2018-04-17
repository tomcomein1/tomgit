//��˰���ļ���������ܽɿ���
#include "dl_tax.h"
#include"newtask.h"

extern long nSysLkrq;           //ϵͳ¼������
extern long nSysSfm;            //ϵͳʱ����
extern char sSysJsdh[11];      //ϵͳ��������
extern char sSysJm[20];       //ϵͳ����
extern char sTcpipErrInfo[81];
extern char sRemoteHost[16];    //����Դ��ַ
extern char sSysCzydh[7];
extern char sSysCzymc[20];
extern char sSysJsmc[JM_JM_LEN+1];
static long InsertZwmxTable(char * inbuf,long nmode,long nxh);

//////////////////////��ѯ��˰����Ϣ////////////////////////
void QueryTax_Info(long number,char *item,char *sJym)
{
    QUERYTABLE * pqt;
    long nId,nRetVal,nKey;
    char sYhmc[40],sTemp[3],cBt[100],sWhere[70]=".";
    char jkbm[8]=".",zgswbm[3]=".",jjlx[31]=".",sssj[16]=".",qybm[21]=".",qydz[37]=".",qymc[61]=".",zssz[11]=".",zssm[11]=".";
    double jsje,zssl,sjsk,dzk=0.0;
    
    clrscr();
    while(1)
    {
      clrscr();
      qymc[0]='\0';
      gotoxy(28,3);
      outtext("����������˰�˵����ơ�");
      nKey=GetValue(8,5,"%-60.60s",qymc);
      if(nKey==KEY_ESC) return;
      alltrim(qymc);
      if(strlen(qymc)<2)
      {
      	 QueryLocal("��˰������                                         ��˰�˱���          ",
      	           "%s%s","select distinct cyhmc,cyhbz from dl$ywmx_%t ",sJym);
      	 continue;
      }//help
      if(nKey==KEY_RETURN) break;
    }

    sprintf(cBt,"    %s������  ��%s��",sSysJm,qymc);
    pqt=CreateQueryTable(cBt,"�û�����              ��˰����          ��˰��ע������  ˰��ǼǺ�            ��˰�˵�ַ                          ����˰��    ����˰Ŀ    ��˰���  ˰��    ʵ�ɽ��  ",
                             "%-20.20s              %-16.16s          %-20.20s        %-20.20s              %-36.36s                            %-10.10s    %-10.10s    %10.2lf %5.2lf %10.2lf",ST_VISUAL|ST_LASTLINE);
    if(!pqt) return;
    
    sprintf(sWhere,"%s%",qymc);
    nId=OpenCursor("select distinct cyhmc,cyhbz,cbl2,cbl3,cbl15,cbl5,cbl13,dsfmx2,dsfmx3,dsfmx5 "
                   "from dl$ywmx_%t where (cyhmc=%s or cyhmc like %s)",
                   sJym,qymc,sWhere);
    if(nId<0)
    {
        DropQueryTable(pqt);
	return;
    }
    while(1)
    {
    	nRetVal=FetchCursor(nId,"%s%s%s%s%s%s%s,%lf%lf%lf",
    	        sYhmc,jkbm,jjlx,qybm,qydz,zssz,zssm,&jsje,&zssl,&sjsk);
    	if(nRetVal<0) break;
    	dzk+=sjsk;
        ImportLastLine(pqt,".",".",".",".",".",".","�ܽ��:",0,0,dzk);
    	if(ImportQueryTable(pqt,nRetVal,sYhmc,jkbm,jjlx,qybm,qydz,zssz,zssm,jsje,zssl,sjsk)<=0) break;
    }
    CloseCursor(nId);
    DropQueryTable(pqt);
}

//��ɽ��˾�ɿ����ӡ
void TaxPrintBill(long number,char *item,char *sJym)
{
	PRINTFORM * ppf;
	long i,j,n,nId,nRet,nId2,nRet2;
	long nprint=1;
	long nKey,nCount;
	long nQsrq,nJzrq=nSysLkrq;
	char zssm[12]=".",hzph[9]=".";
	long nSl=0,nLoop;
	char sLsh[41]=".",sDate[20]=".";
	double dSjje[3],dSjzje,dSjzje0,dSjzje1;
	double dSjk0=0,dSjk1=0;
	char sJm[30]=".",sZsrq[30]=".";
	char sZclx[21],sSzmc[31];
	char sPmmc[3][41],sSjje[3][16];
	char sTemp[300],sSjzje[100],sSjzje0[100],sSjzje1[20];
	char sYsjc[30],sSzbm[10],sSzmc1[14];
	char sPmmc1[16],sPmmc2[16],sPmmc3[16],sSjje1[16],sSjje2[16],sSjje3[16];
	char sKhh[51],sJkgk[51],sZh[51],Yn[3]="N",sJkdw[31];
	
	clrscr();
	GetSysLkrq();
	nJzrq=nSysLkrq;
	nQsrq=GetRandomDate(nSysLkrq,5,-1);
	
	if(RunSelect("select ckhh,cskgk,czh,cjkdw from dl$khhgl where cjsdh=%s into %s,%s,%s,%s",
					sSysJsdh,sKhh,sJkgk,sZh,sJkdw)<0)
	{
		outtext("��������δָ��������");
		return;
	}

	for(;;)
	{
		if(GetValue(4,2,"�����������ʼ����:%8ld",&nQsrq)==KEY_ESC) return;
		nKey=GetValue(4,3,"��������ܽ�ֹ����:%8ld",&nJzrq);
		if(nKey==KEY_ESC) return;
		if(nKey==KEY_RETURN) break;
	}

	//�������
/*	strncpy(sTemp,sSysJsdh+4,2);
	sTemp[2]='\0';
	if(RunSelect("select cgsxbm,cgsgz from dl$xjkzb where cyzxbm=%s into %s%s ",sTemp,zgswbm,gsgz)<=0)
    {
    	outtext("\n   ���ؾ�û��˰���Ӧ�ر���.�������ؾֺ�(%s)...",sTemp);
    	keyb();
    	return;
    }
    alltrim(zgswbm);
    alltrim(gsgz);
*/
	
	//�������(ÿ�����㲻�����ն����˰����)
	if(RunSelect("select count(distinct cbl11) from dl$ywmx_%t "
					"where cjh=%s and nxzbz=1 and nlkrq between %ld and %ld "
					"into %ld ",
					sJym,sSysJsdh,nQsrq,nJzrq,&n)<0)
	{
		outtext("\n   ��ѯҵ����ϸ����!");keyb();
		return;
	}
	if(n>1)
	{
		outtext("\n   �г�Ȩ���յ��������˶�!");keyb();
		return;
	}
	
	//�������(������ϸ�ܶ��ҵ����ϸ�ܶ�)
	if(RunSelect("select sum(dyjzk) from dl$zwmx "
					"where cjym=%s and cjh=%s "
					"and nlkrq between %ld and %ld into %lf ",
					sJym,sSysJsdh,nQsrq,nJzrq,&dSjk0)<0)
	{
		outtext("\n   ��ѯ������ϸ����!");keyb();
		return;
	}
	if(RunSelect("select sum(dsjk) from dl$ywmx_%t "
					"where cjh=%s and nxzbz=1 and nlkrq between %ld and %ld into %lf ",
					sJym,sSysJsdh,nQsrq,nJzrq,&dSjk1)<0)
	{
		outtext("\n   ��ѯҵ����ϸ����!");keyb();
		return;
	}
	if(fabs(dSjk0-dSjk1)>0.01)
	{
		outtext("\n   ҵ����ϸ���ݺ�������ϸ���ݲ����������־��˾��ϵ!");keyb();
		return;
	}
	if(dSjk0<0.01)
	{
		outtext("\n   ���ڼ䣬�޻�������!");keyb();
		return;
	}

	sprintf(sZsrq,"%04ld��%02ld��%02ld--%02ld��",
			nJzrq/10000,(nJzrq%10000)/100,(nQsrq%10000)%100,(nJzrq%10000)%100);
	alltrim(sZsrq);
	//˰���޽�����
	sprintf(sDate,"%04ld��%02ld��%02ld��",
			nJzrq/10000,(nJzrq%10000)/100,((nJzrq+1)%10000)%100);  
	alltrim(sDate);

	
	Clear_Space(1,23,80,23);
	if(!(ppf=CreatePrintForm("zsjks"))) return ;
	
     
	//�Ȱ�ע�����͡�˰�ַ�ҳ
	nId2=OpenCursor("select distinct cbl2,cbl5 "
					"from dl$ywmx_%t "
					"where dsjk>=0 and nxzbz=1 and cjh=%s "
					"and nlkrq between %ld and %ld ",
                    sJym,sSysJsdh,nQsrq,nJzrq);
	if(nId2<0)
	{
		outtext("\n���α����nId2�������־��˾��ϵ��");keyb();
		return;
	}

	nprint=1;
	dSjzje0=0;
	for(;;)
	{
		nRet2=FetchCursor(nId2,"%s%s",sZclx,sSzmc);
		if(nRet2<=0) break;
		
		alltrim(sZclx);
		alltrim(sSzmc);
		
		if(strcmp(sSzmc,"��ֵ˰")==0)
		{
			RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=3 into %s",
						sJym,sYsjc);
			RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=4 into %s",
						sJym,sSzbm);
		}
		if(strcmp(sSzmc,"����˰")==0)
		{
			RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=5 into %s",
						sJym,sYsjc);
			RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=6 into %s",
						sJym,sSzbm);
		}			

		//��ƷĿͳ��
		nId=OpenCursor("select cbl13,sum(dsjk) "
					"from dl$ywmx_%t "
					"where dsjk>=0 and nxzbz=1 and cjh=%s "
					"and nlkrq between %ld and %ld "
					"and cbl2=%s and cbl5=%s "
					"group by cbl13",
                    sJym,sSysJsdh,nQsrq,nJzrq,sZclx,sSzmc);
		if(nId<0)
		{
			outtext("\n���α����nId�������־��˾��ϵ��");keyb();
			return;
		}
		
		
		for(i=0;i<3;i++)
		{
			dSjje[i]=0;
			strcpy(sPmmc[i]," ");
		}
		i=0;dSjzje=0;
		
	l1:	for(;;)
		{
			nRet=FetchCursor(nId,"%s%lf",sPmmc[i],&dSjje[i]);
			if(nRet<=0) goto l0;
			
			dSjzje=dSjzje+dSjje[i];

			if(i>1)
			{
	l0:			to_Chinese(dSjzje,sSjzje0);
				sprintf(sSjzje,"�����%s",sSjzje0);
				alltrim(sSjzje);
				
				if(RunSelect("select clsh from dl$ywmx_%t "
							"where cjh=%s and cbl2=%s and cbl5=%s and nxzbz=1 "
							"and nlkrq between %ld and %ld into %s",
							sJym,sSysJsdh,sZclx,sSzmc,nQsrq,nJzrq,sLsh)<1)
				{
					outtext("\n��ѯҵ����ϸ����������־��˾��ϵ��");keyb();
					RollbackWork();
					CloseCursor(nId);
					CloseCursor(nId2);
					DropPrintForm(ppf);
					return;
				}
	        	
				if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",sJym,sLsh)<0)
				{
					DropPrintForm(ppf);
					return ;
				}
				
				gotoxy(14,22);
				outtext("���������ʼ��ӡ��%ld�Ż��ܽɿ��飬ESC������...",nprint++);
				if(keyb()==KEY_ESC)
				{
					CloseCursor(nId);
					CloseCursor(nId2);
					DropPrintForm(ppf);
					return ;
				}//��
				clearline(22);
				
				for(;;)
				{
					nLoop=GetValue(4,8,"���������Ʊ��:%-7.7s",hzph);
					if(nLoop==KEY_ESC) return;
					alltrim(hzph);
					for(nLoop=0;nLoop<=6;nLoop++)
					{
						if(hzph[nLoop]<'0' || hzph[nLoop]>'9' )
						break;
					}
					if(nLoop<6)  continue;
					else break;
				}
				
				sprintf(sSzmc1,"����%s",sSzmc);
				
				for(j=0;j<3;j++)
				{
					if(dSjje[j]<0.01)
					{
						strcpy(sPmmc[j]," ");
						strcpy(sSjje[j]," ");
					}
					else
						sprintf(sSjje[j],"��%.2lf",dSjje[j]);
				}
				sprintf(sSjzje1,"��%.2lf",dSjzje);
				
				RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=7 into %s",sJym,Yn);
				upper(Yn);
				if(!strcmp(Yn,"Y") ) sprintf(sJm,"%s����֧��",sSysJsmc);
				else strcpy(sJm,".");
				alltrim(sJkdw);
				if(strlen(sJkdw)>3) strcpy(sJm,sJkdw);
				
				OutputPrintForm(ppf,NULL,1,
					hzph,nSysLkrq/10000,(nSysLkrq%10000)/100,(nSysLkrq%10000)%100,
					sYsjc,sJm,sJkgk,sKhh,sZsrq,sZh,sDate,sSzbm,sSzmc1,
					sPmmc[0],sSjje[0],sPmmc[1],sSjje[1],sPmmc[2],sSjje[2],
					sSjzje,sSjzje1);
			
       				//setprinton();
     	 			//eject();
         			//setprintoff();
				sprintf(sTemp," and (cbl13='%s' or cbl13='%s' or cbl13='%s') ",
								sPmmc[0],sPmmc[1],sPmmc[2]);
				
				nLoop=RunSql("update dl$ywmx_%t set cbl7=%s "
							"where cjh=%s and cbl2=%s and cbl5=%s and nxzbz=1 %t ",
							sJym,hzph,sSysJsdh,sZclx,sSzmc,sTemp);
				if(nLoop<0)
				{
					outtext("\n�޸Ļ���Ʊ�ų��������־��˾��ϵ��");keyb();
					RollbackWork();
					CloseCursor(nId);
					CloseCursor(nId2);
					DropPrintForm(ppf);
					return;
				}
				hzph[0]='\0';

				
				//���³�ʼ
				for(j=0;j<3;j++)
				{
					dSjje[j]=0;
					strcpy(sPmmc[j]," ");
				}
				dSjzje0=dSjzje0+dSjzje;
				i=0;dSjzje=0;
				
				if(nRet>0) goto l1;
				break;
				
			}
			i++;
		}
		CloseCursor(nId);
	}
	
	dSjzje1=0;
	if(RunSelect("select sum(dsjk) "
					"from dl$ywmx_%t "
					"where dsjk>=0 and nxzbz=1 and cjh=%s "
					"and nlkrq between %ld and %ld "
					"into %lf",
                    sJym,sSysJsdh,nQsrq,nJzrq,&dSjzje1)<1)
    	{
    		RollbackWork();
		outtext("\n������ܽɿ�ܶ���������־��˾��ϵ��");keyb();
		DropPrintForm(ppf);
		return;
	}
	if(fabs(dSjzje0-dSjzje1)>0.01)
	{
		RollbackWork();
		outtext("\n��ӡ���ܽɿ�ܶ���������־��˾��ϵ��");keyb();
		DropPrintForm(ppf);
		return;
	}
	CommitWork();
	CloseCursor(nId2);
	DropPrintForm(ppf);
	return;
}

void TaxCheckAccount(long nId,char * item,char * sJym)
{
    
    char sAnswer[1024]=".";
    long nLkrq=0,nRetVal;
    
    GetSysLkrq();
    
    nSysLkrq=20030411;
    
	nRetVal=RunSelect("select * from dl$zwmx where ncsbz=1 and cjym=%s",sJym);
    if(nRetVal>0)
    	RunSelect("select min(nlkrq) from dl$zwmx where ncsbz=1 and cjym=%s into %ld",sJym,&nLkrq);
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
    outtext("\n���ں͹�˰�����ʣ��������Ҫ�ϳ�ʱ�䣬�����ĵȴ�...\n");
    
    if(AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sAnswer))
    {
        outtext(sTcpipErrInfo);
        keyb();
        return;
    }
     
    if(!strncmp(sAnswer,"000000",6)) strcpy(sAnswer,"���ʳɹ��������������...");
    outtext(sAnswer);
    keyb();
}


void HzDayTaxFile(long nId,char * item,char * sJym)
{
    PostToTaxFile(sJym,0);
    return;
}

//���������й���
void Tax_Khhgl()
{
    DATATABLE * pdt;//¼��������ݱ�����
    long nKey,nCurrent,nwhere,nModifyFlag;
    long nJsdh;
    char sOpt[2];
    char sJsdh[10]=".",sJsdh0[7]=".",sJsmc[21],sKhh[41],sSkgk[31],sZh[31],sJkdw[31]=".";
    
    clrscr();           
     
    pdt=CreateDataTable(8,
    "|  |����|��������            |��������                                |�տ����                      |�˺�                          |�ɿλ                      |",
    "%-1.1s%-3.3d%-20.20s%-40.40s%-30.30s%-30.30s%-30.30s");
    if(!pdt)
    {
		MessageBox("�������ݱ�����...",1);
		return;
    }

    AssignTableFields(pdt,"copt,njsdh,cjsmc,ckhh,cskgk,czh,cjkdw");
    SetTableStyle(pdt,ST_ALLTRIM|ST_MESSAGELINE,1); 


    gotoxy(1,1);	
    outtext("                         ���������й���");
    ShowDataTable(pdt,3,3);
    if(LoadKhhglData(pdt))
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
		ImportData(pdt,nCurrent,"copt","+");
        
        switch(nwhere)
        {
            case 1:
            	ShowDatatableInfo(pdt,"�����빦�ܿ��ƣ���M����ʾ���ܼ�����...",0);
            	break;
            case 2:
           		ShowDatatableInfo(pdt,"�������������(��λ),�����ظ�...",0);
            	break;
            case 3:
            	ShowDatatableInfo(pdt,"�����������Ӧ�Ŀ�������...",0);
            	break;
            case 4:
            	ShowDatatableInfo(pdt,"�����������Ӧ���տ����...",0);
            	break;
            case 5:
            	ShowDatatableInfo(pdt,"�����������Ӧ���˺�...",0);
            	break;	
            case 6:
            	ShowDatatableInfo(pdt,"������ɿ���ɿλ[ȫ��]...",0);
            	break;
            
        }
         

	  //Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"copt,njsdh,ckhh,cskgk,czh,cjkdw");

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
					nKey=MessageBox("�����޸�û�б��棬Ҫ������",0);
					if(nKey==KEY_ESC)
					{
						ShowDataTable(pdt,3,3);
			        	continue;
					}
					if(nKey==KEY_YES) SaveKhhglData(pdt);
		    	}
		    	DropDataTable(pdt);
		    	return;
            default:
            	DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
            	continue;
		}

 		ExportData(pdt,nCurrent,"copt,njsdh,ckhh,cskgk,czh,cjkdw",sOpt,&nJsdh,sKhh,sSkgk,sZh,sJkdw);
		ImportData(pdt,nCurrent,"copt","+");

		switch(nwhere)
		{
	   		case 1: //��������
		    	switch(sOpt[0])
		    	{
                 	case 's':   //save as temporary record
			     	case 'S':
				  		if(nModifyFlag)
				  		{
					 		nModifyFlag=SaveKhhglData(pdt);
					 		return;
				  		}
				  		continue;
			     	default:
			          	DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nwhere,&nModifyFlag);
			          	continue;
		    	}
		    	break;
	   		case 2:
                nModifyFlag=1;
                nKey=FindDataTable(pdt,1,"njsdh",nJsdh);
                
                if(nKey==nCurrent) 
                	nKey=FindDataTable(pdt,nCurrent+1,"njsdh",nJsdh);
                if(nKey>0)
              	{
              	 	ShowDatatableInfo(pdt,"�ô����Ѿ����ڣ�����������",1);
              	 	continue;
              	}
              	
              	strncpy(sJsdh0,sSysJsdh,6);
              	sprintf(sJsdh,"%6s%03d",sJsdh0,nJsdh);
              	nKey=RunSelect("select cjm from dl$jh where cjh=%s and nlb=4 into %s",
              					sJsdh,sJsmc);
              	if(nKey<1)
              	{
              		ShowDatatableInfo(pdt,"�þ������Ų����ڣ�����������",1);
              	 	continue;
              	}
              	ImportData(pdt,nCurrent,"cjsmc",sJsmc);
              	nwhere=3;
				continue;
	    	case 3:
	    		alltrim(sKhh);
	    		if(strcmp(sKhh," ")==0)
	    			continue;
	            nwhere=4;
		    	continue;
           	case 4:
           		alltrim(sSkgk);
	    		if(strcmp(sSkgk," ")==0)
	    			continue;
	    		nwhere=5;
	    		continue;
		case 5:
           		alltrim(sZh);
	    		if(strcmp(sZh," ")==0)
	    			continue;
	    		nwhere=6;
	        	continue;
	    	case 6:
           		alltrim(sJkdw);
	    		if(strcmp(sJkdw," ")==0)
	    			continue;
	    		nwhere=2;
	                nCurrent++;
			continue;
           		    
		}//end switch(nwhere);
	}//end for(;;)
}

static long SaveKhhglData(DATATABLE * pdt)
{
    long nId,nRetVal;
    long nJsdh=0;
    char sJsdh[10]=".",sJsdh0[7]=".",sJsmc[21],sKhh[41],sSkgk[31],sZh[31],sJkdw[31]=".";
    
    ShowDatatableInfo(pdt,"������⣬���Ժ�...",0);
    
    strncpy(sJsdh,sSysJsdh,6);
    sJsdh[6]='\0';
    nRetVal=RunSql("delete from dl$khhgl where substr(cjsdh,1,6)=%s",sJsdh);
    if(nRetVal<0)
    {
        MessageBox("���ݿ����ʧ��",2);
    	RollbackWork();
    	return -1; 
    }

    nRetVal=1;
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	
    	ExportData(pdt,nId,"njsdh,ckhh,cskgk,czh,cjkdw",&nJsdh,sKhh,sSkgk,sZh,sJkdw);
    	
        alltrim(sKhh);
        alltrim(sSkgk);
        
        if(sKhh[0]=='.') continue;
        
		strncpy(sJsdh0,sSysJsdh,6);
		sprintf(sJsdh,"%6s%03ld",sJsdh0,nJsdh);
		
		nRetVal=RunSql("insert into dl$khhgl(cJsdh,ckhh,cskgk,czh,cjkdw) values(%s,%s,%s,%s,%s)",
						sJsdh,sKhh,sSkgk,sZh,sJkdw);
        if(nRetVal<0) break;                        
    }

    if(nRetVal<0)
    {
        HiError(nRetVal,"���ݿ����ʧ��");
    	RollbackWork();
    	return -1;
    }
    else
    {
    	CommitWork();
        ShowDatatableInfo(pdt,"�������...",0);
        return 0;
    }
}

static long LoadKhhglData(DATATABLE * pdt)
{
    long nId,nRetVal,nKey;
    long nJsdh;
    char sJsdh[10],sJsdh0[7]=".",sJsmc[21],sKhh[41],sSkgk[31],sZh[31];
    char sTemp[10],sJkdw[31]=".";

    strncpy(sJsdh,sSysJsdh,6);
    sJsdh[6]='\0';
    nId=OpenCursor("select cJsdh,ckhh,cskgk,czh,cjkdw from dl$khhgl "
    				"where substr(cjsdh,1,6)=%s order by cJsdh",
    				sJsdh);

    if(nId<0)  return -1;
    
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%s%s%s%s%s",sJsdh,sKhh,sSkgk,sZh,sJkdw);
    	if(nRetVal==0) break;
    	if(nRetVal<0)  return -1;
    	
    	nKey=RunSelect("select cjm from dl$jh where cjh=%s and nlb=4 into %s",
              					sJsdh,sJsmc);
        if(nKey<1) continue;
    	FetchStringToArg(sJsdh,"%6s%3d",sTemp,&nJsdh);
    	ImportData(pdt,pdt->nCount+1,"copt,njsdh,cjsmc,ckhh,cskgk,czh,cjkdw",
    	               "+",nJsdh,sJsmc,sKhh,sSkgk,sZh,sJkdw);
    }
    CommitWork();
    CloseCursor(nId);
    return 0;
}

//
void QueryTax_Id_Info(long number,char *item,char *sJym)
{
    QUERYTABLE * pqt;
    long nId,nRetVal,nKey;
    char sYhmc[40],sTemp[3],cBt[100],sWhere[70]=".",sId[20]=".";
    char jkbm[20]=".",qymc[61]=".";
    double dzk=0.0;
    
    clrscr();
    while(1)
    {
      clrscr();
      qymc[0]='\0';
      gotoxy(28,3);
      outtext("�������뱣���˵����֤�š�");
      nKey=GetValue(30,5,"%-18.18s",sId);
      if(nKey==KEY_ESC) return;
      alltrim(sId);
      if(strlen(sId)<2)
      {
      	 QueryLocal("����������          �����˱���              ���֤��            ",
      	           "%s%s%s","select cyhmc,cyhbz,cbl1 from dl$ywmx_%t where nxzbz=0",sJym);
      	 continue;
      }//help
      if(nKey==KEY_RETURN) break;
    }

    sprintf(cBt,"    %s������  ��%s��",sSysJm,sId);
    pqt=CreateQueryTable(cBt,"���֤��            �û�����              ���ձ���          ʵ�ɽ��    ",
                             "%-18.18s            %-20.20s              %-16.16s          %12.2lf",ST_VISUAL);
    if(!pqt) return;
    
    sprintf(sWhere,"%s%",sId);
    nId=OpenCursor("select distinct cbl1,cyhmc,cyhbz,dsfmx1 "
                   "from dl$ywmx_%t where (cbl1=%s or cbl1 like %s) and nxzbz=0",
                   sJym,sId,sWhere);
    if(nId<0)
    {
        DropQueryTable(pqt);
	return;
    }
    while(1)
    {
    	nRetVal=FetchCursor(nId,"%s%s%s,%lf",
    	        	sId,sYhmc,jkbm,&dzk);
    	if(nRetVal<0) break;
    	if(ImportQueryTable(pqt,nRetVal,sId,sYhmc,jkbm,dzk)<=0) break;
    }
    CloseCursor(nId);
    DropQueryTable(pqt);
}

void TerraJks(long number,char *item,char *sJym)
{
	long nId,nRet,nId1,nRet1,nId2,nRet2;
	char ksh[12],jjxz[5],jjxzm[41],szbh[4],sz[32],smbh[5],sm[32],jch[10],ysbh[20],jc[30];
	double dSjk,dZk;
	char sBuffer[4096],sTemp[360],sFileName[80],sRq[30],buf[120];
	long nQsrq,nJzrq,i=0,nKey;
	char sQc[40]=".",sGg[40]=".",sKhh[40]=".",sZh[40]=".",space[6]="  ",sSjk[40],sFph[14],ysmc[30]=".";
	FILE *fp;
	
	clrscr();
	GetSysLkrq();
	nJzrq=nSysLkrq;
	nQsrq=nSysLkrq-10;
	for(;;)
	{
		if(GetValue(4,2,"�����������ʼ����:%8ld",&nQsrq)==KEY_ESC) return;
		nKey=GetValue(4,3,"��������ܽ�ֹ����:%8ld",&nJzrq);
		if(nKey==KEY_ESC) return;
		if(nKey==KEY_RETURN) break;
	}//��������

	nId=OpenCursor("select distinct substr(cbl5,1,2) from dl$ywmx_%t where nxzbz=1 and nlkrq between %ld and %ld ",sJym,nQsrq,nJzrq);
	if(nId<0) {CloseCursor(nId); return;}
	sprintf(sFileName,"sendfile/11/DSJK%8ld.txt",nSysLkrq);
	
	if(!(fp=fopen(sFileName,"ab")))
	{
		fclose(fp);
		return;
	}
		
	while(1)
	{
		nRet=FetchCursor(nId,"%s",ksh);
		if(nRet<=0) break;
		nId1=OpenCursor("select distinct cbl1,cbl2 from dl$ywmx_%t where nxzbz=1 and substr(cbl5,1,2)=%s and nlkrq between %ld and %ld",sJym,ksh,nQsrq,nJzrq);
		if(nId1<0) {CloseCursor(nId1); return;}
		while(1)
		{
			nRet1=FetchCursor(nId1,"%s%s",jjxzm,jjxz);
			if(nRet1<=0) break;
			nId2=OpenCursor("select cbl3,cbl7,min(cbl9),min(cbl11),sum(dsjk),cbl15 from dl$ywmx_%t where nxzbz=1 and \
					substr(cbl5,1,2)=%s and cbl2=%s and nlkrq between %ld and %ld group by cbl3,cbl7,cbl15 ",
					sJym,ksh,jjxz,nQsrq,nJzrq);
			for(;;)
			{
				//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
				i++;
				nRet2=FetchCursor(nId2,"%s%s%s%s%lf%s",
					sz,szbh,smbh,jch,&dSjk,sFph);
				if(nRet2<=0) break;
	  		
	  			RunSelect("select distinct yskm,yskmmc,jc from dl$terra where szmc=%s and jjxz=%s "
    						"into %s%s%s",sz,jjxz,ysbh,ysmc,jc);
	  	  		
	  	  		//�����γ��ļ�.׷�ӷ�ʽ
				{
				alltrim(sFph);
				sprintf(sRq,"%ld %ld:%02ld:%02ld",nSysLkrq,nSysSfm/10000,(nSysSfm%10000)/100,(nSysSfm%10000)%100);
				fprintf(fp,"05|%s|%s|%s|%s|%s|%.2lf|%s|%s|%s|\n",
					sFph,ksh,szbh,smbh,jch,dSjk,jjxz,sRq,ysbh);
				}//���	
			}//end for
			CloseCursor(nId2);
			
		}//end while[2]
		CloseCursor(nId1);
		
	}//end while[1]
	CloseCursor(nId);
	fclose(fp);
	sprintf(buf,"xtod %s > sendfile/11/MDSHZJKS%ld%02ld.txt",
			sFileName,nSysLkrq/10000,(nSysLkrq%10000)/100);
    	system(buf);
        outtext("\n   �γ��ļ�[MDSHZJKSYYYYMM]�ɹ�\n   �����������......");
        keyb();
	return;
}

void TerraPrintBill(long number,char *item,char *sJym)
{
	PRINTFORM * ppf;
	long nId,nRet,nId1,nRet1,nId2,nRet2;
	char ksh[12],jjxz[5],jjxzm[41],szbh[4],sz[32],smbh[5],sm[32],jch[10],ysbh[20],jc[30];
	double dSjk,dZk;
	char sBuffer[4096],sTemp[360],sFileName[80],sRq[30],buf[120];
	long nQsrq,nJzrq,i=0,nKey;
	char sQc[40]=".",sGg[40]=".",sKhh[40]=".",sZh[40]=".",space[6]="  ",sSjk[40],sFph[14],ysmc[30]=".";
	char sjk[16]=".";
	FILE *fp;
	
	clrscr();
	GetSysLkrq();
	nJzrq=nSysLkrq;
	nQsrq=nSysLkrq-10;
	for(;;)
	{
		if(GetValue(4,2,"�����������ʼ����:%8ld",&nQsrq)==KEY_ESC) return;
		if(GetValue(4,3,"��������ܽ�ֹ����:%8ld",&nJzrq)==KEY_ESC) return;
		if(GetValue(4,4,"���������ջ���:%-20s",&sQc)==KEY_ESC) return;
		if(GetValue(4,5,"������ɿ����:%-20s",&sGg)==KEY_ESC) return;
		if(GetValue(4,6,"�����뿪������:%-20s",&sKhh)==KEY_ESC) return;
		nKey=GetValue(4,7,"�������ʺ�:%-20s",&sZh);
		if(nKey==KEY_ESC) return;
		if(nKey==KEY_RETURN) break;
	}//��������
	
	nId=OpenCursor("select distinct substr(cbl5,1,2) from dl$ywmx_%t where nxzbz=1 and nlkrq between %ld and %ld and cjh=%s",sJym,nQsrq,nJzrq,sSysJsdh);
	if(nId<0) {CloseCursor(nId); return;}

	if(!(ppf=CreatePrintForm("dstyjks"))) return ;
	if(LoadPrintData(ppf,"from DUAL")<0)
	{
		DropPrintForm(ppf);
		return ;
	}//end load
	
	while(1)
	{
		nRet=FetchCursor(nId,"%s",ksh);
		if(nRet<=0) break;
		nId1=OpenCursor("select distinct cbl1,cbl2 from dl$ywmx_%t where nxzbz=1 and substr(cbl5,1,2)=%s and nlkrq between %ld and %ld and cjh=%s",sJym,ksh,nQsrq,nJzrq,sSysJsdh);
		if(nId1<0) {CloseCursor(nId1); return;}
		while(1)
		{
			nRet1=FetchCursor(nId1,"%s%s",jjxzm,jjxz);
			if(nRet1<=0) break;
			nId2=OpenCursor("select cbl3,cbl7,min(cbl9),min(cbl10),min(cbl11),sum(dsjk) from dl$ywmx_%t where nxzbz=1 and \
					substr(cbl5,1,2)=%s and cbl2=%s and nlkrq between %ld and %ld and cjh=%s group by cbl3,cbl7 ",
					sJym,ksh,jjxz,nQsrq,nJzrq,sSysJsdh);
			for(;;)
			{
				i++;
				nRet2=FetchCursor(nId2,"%s%s%s%s%s%lf",
					sz,szbh,smbh,sm,jch,&dSjk);
				if(nRet2<=0) break;
		
		RunSelect("select distinct yskm,yskmmc,jc from dl$terra where szmc=%s and jjxz=%s "
    				"into %s%s%s",sz,jjxz,ysbh,ysmc,jc);
    		
    		to_Chinese(dSjk,sSjk);
    		sprintf(sjk,"��%.2lf",dSjk);
				while(1)
				{
					sFph[0]='\0';
					if(GetValue(4,8,"�����뷢Ʊ��:%-10s",&sFph)==KEY_ESC) 
					{
						CloseCursor(nId2);
						CloseCursor(nId1);
						CloseCursor(nId);
						DropPrintForm(ppf);
						return;
					}	
					alltrim(sFph);
					if(strlen(sFph)!=7) continue;
					break;
				}//end while
				
				printf("   ��%ld��\n",i);
				OutputPrintForm(ppf,NULL,1,
					sQc,jjxzm,nSysLkrq/10000,(nSysLkrq%10000)/100,(nSysLkrq%10000)%100,ksh,
					jc,sQc,sGg,sKhh,nJzrq/10000,(nJzrq%10000)/100,(nJzrq%10000)%100,
					sZh,nJzrq/10000,(nJzrq%10000)/100,((nJzrq+3)%10000)%100,
					ysbh,ysmc,sm,sjk,sSjk,sjk,ksh,sFph);
				
				if(RunSql("update dl$ywmx_%t set cbl15=%s where \
					nxzbz=1 and substr(cbl5,1,2)=%s and \
					cbl2=%s and cbl7=%s and nlkrq between %ld and %ld and cjh=%s",
					sJym,sFph,ksh,jjxz,szbh,nQsrq,nJzrq,sSysJsdh)>0) CommitWork();
	
			}//end for
			CloseCursor(nId2);
			
		}//end while[2]
		CloseCursor(nId1);
		
	}//end while[1]
	CloseCursor(nId);
	DropPrintForm(ppf);
        outtext("\n   �˽ɿ����ӡ�ɹ�\n   �����������......");
        keyb();
	return;
}

//��ѯ����Ӧ�Ͻɿ�
void QueryTerra_Info(long number,char *item,char *sJym)
{
    QUERYTABLE * pqt;
    long nId,nRetVal,nKey,nQsrq,nJzrq;
    char sYhmc[40],sTemp[3],cBt[100],sWhere[70]=".";
    char  sJh[11];
    double dcjs,djyf,dgrs,dsjk=0.0;
    double dZcjs,dZjyf,dZgrs,dZk;
    
    clrscr();
    nJzrq=nQsrq=nSysLkrq;

    while(1)
    {
      if(GetValue(4,2,"�����������ʼ����:%8ld",&nQsrq)==KEY_ESC) return;
      nKey=GetValue(4,3,"��������ܽ�ֹ����:%8ld",&nJzrq);
      if(nKey==KEY_ESC) return;
      if(nKey==KEY_RETURN) break;
    }

    sprintf(cBt,"    ����  ��%ld-%ld��",nQsrq,nJzrq);
    
    pqt=CreateQueryTable(cBt,"�ֺ�        �ǽ�˰      �����Ѹ���  ��������˰  �ϼ�          ","%-10.10s %12.2lf %12.2lf %12.2lf %12.2lf",ST_VISUAL|ST_LASTLINE);
    if(!pqt) return;
    RunSelect("select sum(dsfmx5)*5/100,sum(dsfmx5)*3/100,sum(dsfmx2)*1.5/100,(sum(dsfmx5)*8+sum(dsfmx2)*1.5)/100 from dl$ywmx_%t "
    		   " where cjh between '271502000' and '271502999' and nxzbz=1 "
    		   " and nlkrq>=%ld and nlkrq<=%ld group by cjh into %lf%lf%lf%lf",
                   sJym,nQsrq,nJzrq,&dZcjs,&dZjyf,&dZgrs,&dZk);
    
    nId=OpenCursor("select cjh,sum(dsfmx5)*5/100,sum(dsfmx5)*3/100,sum(dsfmx2)*1.5/100,(sum(dsfmx5)*8+sum(dsfmx2)*1.5)/100 from dl$ywmx_%t "
    		   " where cjh between '271502000' and '271502999' and nxzbz=1 and nlkrq>=%ld and nlkrq<=%ld group by cjh",
                   sJym,nQsrq,nJzrq);
    if(nId<0)
    {
        CloseCursor(nId);
	return;
    }

    ImportLastLine(pqt,"�ܽ��:",dZcjs,dZjyf,dZgrs,dZk);
    while(1)
    {
    	nRetVal=FetchCursor(nId,"%s%lf%lf%lf%lf",sJh,&dcjs,&djyf,&dgrs,&dsjk);
    	if(nRetVal<0) break;
    	if(ImportQueryTable(pqt,nRetVal,sJh,dcjs,djyf,dgrs,dsjk)<=0) break;
    }
    CloseCursor(nId);
    DropQueryTable(pqt);
}

//�Ǻǣ���~
void Terra_Jc()
{
    DATATABLE * pdt;//¼��������ݱ�����
    long nKey,nCurrent,nwhere,nModifyFlag;
    long nJsdh;
    char sOpt[2];
    char szmc[22],jjxz[9],yskm[9],yskmmc[34],jc[21];
    
    clrscr();           
     
    pdt=CreateDataTable(8,
    "|  |˰������            |��������|Ԥ���Ŀ����|Ԥ���Ŀ����                    |����                |",
    "%-1.1s%-20.20s%-8.8s%-8.8s%-32.32s%-20.20s");
    if(!pdt)
    {
		MessageBox("�������ݱ�����...",1);
		return;
    }

    AssignTableFields(pdt,"copt,szmc,jjxz,yskm,yskmmc,jc");
    SetTableStyle(pdt,ST_ALLTRIM|ST_MESSAGELINE,1); 


    gotoxy(1,1);	
    outtext("                         ��˰��Ŀ���ι���");
    ShowDataTable(pdt,3,3);
    if(LoadTerraData(pdt))
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
		ImportData(pdt,nCurrent,"copt","+");
        //"|  |˰������            |��������|Ԥ���Ŀ����|Ԥ���Ŀ����                    |����                |",
        switch(nwhere)
        {
            case 1:
            	ShowDatatableInfo(pdt,"�����빦�ܿ��ƣ���M����ʾ���ܼ�����...",0);
            	break;
            case 2:
            	ShowDatatableInfo(pdt,"������Ԥ���Ŀ����...",0);
            	break;
            case 3:
            	ShowDatatableInfo(pdt,"������Ԥ���Ŀ����...",0);
            	break;	
            case 4:
            	ShowDatatableInfo(pdt,"�����뼶��...",0);
            	break;
            
        }
         

	  //Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"copt,yskm,yskmmc,jc");

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
					nKey=MessageBox("�����޸�û�б��棬Ҫ������",0);
					if(nKey==KEY_ESC)
					{
						ShowDataTable(pdt,3,3);
			        	continue;
					}
					if(nKey==KEY_YES) SaveTerraData(pdt);
		    	}
		    	DropDataTable(pdt);
		    	return;
            default:
            	DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
            	continue;
		}

 		ExportData(pdt,nCurrent,"copt,yskm,yskmmc,jc",
 					sOpt,yskm,yskmmc,jc);
		ImportData(pdt,nCurrent,"copt","+");

		switch(nwhere)
		{
	   		case 1: //��������
		    	switch(sOpt[0])
		    	{
                 	case 's':   //save as temporary record
			     	case 'S':
				  		if(nModifyFlag)
				  		{
					 		nModifyFlag=SaveTerraData(pdt);
					 		return;
				  		}
				  		continue;
			     	default:
			          	DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nwhere,&nModifyFlag);
			          	continue;
		    	}
		    	break;
   		case 2:
                	nModifyFlag=1;
			alltrim(yskm);                	
              		nwhere++;
              		continue;
	    	case 3:
	    		alltrim(yskmmc);
	            	nwhere++;
	            	continue;
           	case 4:
           		alltrim(jc);
	    		nwhere=2;
	    		nCurrent++;
	    		continue;
		
		}//end switch(nwhere);

	}//end for(;;)
}

static long SaveTerraData(DATATABLE * pdt)
{
    long nId,nRetVal;
    long nJsdh=0;
    char szmc[22],jjxz[9],yskm[9],yskmmc[34],jc[21];
    
     
    ShowDatatableInfo(pdt,"������⣬���Ժ�...",0);
    
    nRetVal=RunSql("delete from dl$terra ");
    if(nRetVal<0)
    {
        MessageBox("���ݿ����ʧ��",2);
    	RollbackWork();
    	return -1; 
    }

    nRetVal=1;
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	
    	ExportData(pdt,nId,"szmc,jjxz,yskm,yskmmc,jc",szmc,jjxz,yskm,yskmmc,jc);
    	
	alltrim(szmc);
	alltrim(jjxz);
	alltrim(yskm);
	alltrim(yskmmc);
	alltrim(jc);
        if(szmc[0]=='.' || jjxz[0]=='.') continue;
        
	nRetVal=RunSql("insert into dl$terra(szmc,jjxz,yskm,yskmmc,jc) values(%s,%s,%s,%s,%s)",
				szmc,jjxz,yskm,yskmmc,jc);
        if(nRetVal<0) break;                        
    }

    if(nRetVal<0)
    {
        HiError(nRetVal,"���ݿ����ʧ��");
    	RollbackWork();
    	return -1;
    }
    else
    {
    	CommitWork();
        ShowDatatableInfo(pdt,"�������...",0);
        return 0;
    }
}

static long LoadTerraData(DATATABLE * pdt)
{
    long nId,nRetVal,nKey;
    char szmc[22],jjxz[9],yskm[9],yskmmc[34],jc[21];
    char sJym[14];
    
    RunSelect("select cjym from dl$jym where ncxms=222 into %s",sJym);
    
    nId=OpenCursor("select distinct cbl3,cbl2 from dl$ywmx_%t where nysyf=%ld ",
    			sJym,(nSysLkrq%10000)/100);

    if(nId<0)  return -1;
    
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%s%s",szmc,jjxz);
    	if(nRetVal==0) break;
    	if(nRetVal<0)  return -1;
    	
    	alltrim(szmc);alltrim(jjxz);
    	nKey=RunSelect("select distinct yskm,yskmmc,jc from dl$terra where szmc=%s and jjxz=%s "
    		"into %s%s%s",szmc,jjxz,yskm,yskmmc,jc);

	if(nKey<0)
	{
		if(RunSql("create table dl$terra( \
			szmc VARCHAR2(20),\
			jjxz VARCHAR2(10),\
			yskm VARCHAR2(10),\
			yskmmc VARCHAR2(40),\
			jc VARCHAR2(20)) ")>0)	CommitWork();
	}
	else if(nKey==0)
	{
		strcpy(yskm,".");
		strcpy(yskmmc,".");
		strcpy(jc,".");
	}
	
	ImportData(pdt,pdt->nCount+1,"copt,szmc,jjxz,yskm,yskmmc,jc",
    	               "+",szmc,jjxz,yskm,yskmmc,jc);
    }
    
    CloseCursor(nId);
    return 0;
}
//��Ʊ����
void Terra_Defp(long number,char *item,char *sJym)
{
	char sTitle[80],sYhbz[12]=".",sFpbh[21]=".";
	long nKey,nWhere,nBs=1,nSl=1,i,nQdh=0,nZdh=0,nZbs=0;
	double dDj=0,dPmje=0,dMz=5,dJe=0,dSjk=0,dZk,dZje=0,dSl=0,dYys=0,dZsl=0;
	long nRet=0,nHy=4,nYsyf,nXh=0,nYsnd=0;
	char sLsh[41]=".",sYhmc[30]=".",sPacket[127]=".",sBuffer[51]=".";
	double dSum=0;
	char qybh[12],jjxz[5],jjxzm[41],lsybh[20],qydz[60],qymc[80],ksh[12],ssrq[16],szbh[4],sz[32],smbh[5],sm[32],jch[10];
	
	clrscr();
	nYsyf=(nSysLkrq%10000)/100;
	sprintf(sTitle,"[%s]  ��Ʊ������Ϣ  ����:%ld",sSysCzydh,nSysLkrq);
	gotoxy(10,2);
	outtext(sTitle);
	
LINE_1:	nWhere=1;
	dZk=0;
	for(;;)
	{
		switch(nWhere)
	        {
           	  case 1:
           	  	nKey=GetValue(12,4,"��������ҵ����:%-10.10s",sYhbz);
           	  	break;
		  case 2:
		  	nKey=GetValue(12,5,"��ѡ����ҵ:(1.��ʳ,��������2.���3.��������4.����)%2ld",&nHy);
		  	if(nHy<1 || nHy>4) continue;
		  	break;
		  case 3:
           	  	nKey=GetValue(12,6,"�����빺��������(��):%4ld",&nSl);
		  	break;
	  
		  default:break;
		  
		}//end switch 
	
	
	   switch(nKey)
           {
           	case KEY_ESC:
                if(nWhere>1)
                {
             	   nWhere=1;
             	   continue;
                }
                return;
           
           	case KEY_LEFT:
           	case KEY_UP:
                if(nWhere>1) nWhere--;
                continue;
           
           	case KEY_RETURN:
                break;
           
           	default:continue;
           }
	
	  if(nWhere<3)
	  {
		nWhere++;
		continue;
	  }
	  else break;
	
	}//end for

	for(i=0;i<nSl;i++)
	{
	     nWhere=1;
	     strcpy(sFpbh,".");
	     
	     RunSelect("select to_number(ccs) from dl$jymcsfb where cjym=%s and nbh=2 into %lf",sJym,&dDj);
	     while(1)
	     {
		switch(nWhere)
	        {
           	  case 1:
           	  	nKey=GetValue(12,8,"�����뷢Ʊ���:%-20.20s",sFpbh);
           	  	break;
		  case 2:
           	  	nKey=GetValue(12,9,"�����빺����:%3ld",&nBs);
		  	break;
		  case 3:
           	  	nKey=GetValue(12,10,"�����뵥��:%8.2lf",&dDj);
		  	break;
		  case 4:
		  	nKey=GetValue(12,11,"����������:%4ld",&nQdh);
		  	nZdh=(nQdh+nBs*50)-1;//ֹ��ż���
		  	break;
		  case 5:
		  	nKey=GetValue(12,12,"������ֹ���:%4ld",&nZdh);
		  	break;
		  case 6:
		  	nKey=GetValue(12,13,"��������ֵ:%8.2lf",&dMz);
		  	dPmje=nBs*dMz*50;  //Ʊ�������
			dJe=nBs*dDj;       //������
		  	break;
		  default:break;
		  
		}//end switch 
	
	
		switch(nKey)
	        {
	           case KEY_ESC:
	                if(nWhere>1)
	                {
	             	   nWhere=1;
	             	   continue;
	                }
	                goto LINE_1;
	           
	           case KEY_LEFT:
	           case KEY_UP:
	                if(nWhere==1) goto LINE_1;
	                if(nWhere>1) nWhere--;
	                continue;
	           
	           case KEY_RETURN:
	                break;
	           
	           default:continue;
	        }
		
		if(nWhere<6) 
		{
			nWhere++;
			continue;
		}
		else break;
	  
	      }//end while
	      
	      alltrim(sYhbz); 
	      alltrim(sFpbh); 
	      printf("\n\n  ��Ʊ���:%s,Ʊ����:%.2lf\n",sFpbh,dPmje);
	      
	      nRet=RunSql("INSERT INTO dl$terra_fp(cqybm,cczydh,nysyf,nlkrq,\
        		njls,cfpbh,nbs,nqdh,nzdh,ddj,dmz,dje,cbz)  \
        		VALUES (%s,%s,%ld,%ld,%ld,%s,%ld,%ld,%ld,%lf,%lf,%lf,'.')",
        		sYhbz,sSysCzydh,nYsyf,nSysLkrq,
        		i+1,sFpbh,nBs,nQdh,nZdh,dDj,dMz,dPmje);
	       nZbs+=nBs;
	       dZk+=dPmje;
	}//end for
	if(nRet>0) CommitWork();
	RunSelect("select sum(dje) from dl$terra_fp where cqybm=%s and nysyf=%ld into %lf",
			sYhbz,nYsyf,&dZje);
	nRet=RunSelect("select dsfmx2*0.7,cyhmc,cbl1,cbl2,cbl3,cbl4,cbl5,cbl6,cbl7,cbl9,cbl10,cbl11 \
			from dl$ywmx_%t where cyhbz=%s and cbl7='02' into %lf%s%s%s%s%s%s%s%s%s%s%s",
			sJym,sYhbz,&dSjk,sYhmc,jjxzm,jjxz,sz,lsybh,ksh,ssrq,szbh,smbh,sm,jch);
	
	if(nRet<=0) return;
	//printf("  [%ld]\n",nRet);
	//printf("  TEST:%.2lf|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",dSjk,sYhmc,jjxzm,jjxz,sz,lsybh,ksh,ssrq,szbh,smbh,sm,jch);
	if((dZje-dZk)<dSjk)
	{ 
	  dZk=dZje;
	  if(dZk<=dSjk)
	  {
	   dJe=dDj*nZbs;
	   printf("\n  Ӧ�ɿ�[������]:%.2lf\n",dJe);
	   keyb();
	   return;
	  }
	  dZk=dZk-dSjk;
	}//end if
	
	RunSelect("select to_number(ccs) from dl$jymcsfb where cjym=%s and nbh=3 into %lf",sJym,&dSl);
	RunSelect("select to_number(ccs) from dl$jymcsfb where cjym=%s and nbh=%ld into %lf",sJym,nHy+3,&dZsl);
	  
	dYys=dZk*dSl;
	dJe=dDj*nZbs+dYys*dZsl;
	//0.085  0.1;  0.065;	0.24;
	//���������ҵ�����˰��

	printf("\n  ���Ʊ��:%.2lf,Ӧ�ɿ�:%.2lf,������:%.2lf\n",dZk,dJe,dDj*nZbs);
	keyb();
	nYsnd=(nSysLkrq/10000);
	dSum=dJe-dDj*nZbs;
	
	if(GenerateLsh(1,sSysJsdh,sJym,sLsh)==0)  return ;
	GenerateLsh(3,sSysJsdh,sJym,sBuffer);
        nXh=atol(sBuffer);
	
	sprintf(sPacket,"%-40.40s|%s|%s|1|%.2lf|%s|%ld|%ld|%s|%s|",
					sLsh,sYhbz,sYhmc,
					dSum,sSysCzydh,nSysSfm,nYsyf);
	
	if(InsertZwmxTable(sPacket,TASK_PAY_CONFIRM,nXh)>0)
		    CommitWork();
	
	if(RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,"
	        "nywlb,nxh,dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,"
	        "dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,dsfmx18,dsfmx19,dsfmx20,"
	        "dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,njscz2,djscz3,djscz4,nsl,"
	        "dyjzk,dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,"
	        "cbl3,cbl4,cbl5,cbl6,cbl7,cbl8,cbl9,cbl10,cbl11,cbl12,cbl13,cbl14,"
	        "cbl15,cbl16,cbl17,cbl18,"
	        "cjh,cczydh,nlkrq,nsfm,clxbz,nxzbz,ncsbz,cbz,cylsh,clsh) "
	        "VALUES(%s,%ld,%ld,%s,%s,%s,%s,%ld,%ld,0,%lf,%lf,0,%lf,0,0,0,0,0,"
	        "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'.',%ld,0,0,1,%lf,0,%lf,0,0,0,0,%ld,0,"
	        "0,0,%s,%s,%s,%s,%s,%s,%s,'.',%s,%s,%s,'0','0','0','0','0','0','0',"
	        "'0','XTWH',%ld,%ld,'0000000000',1,0,'.',%s,'0')",
	        sJym,sJym,nYsnd,nYsyf,sYhbz,sYhbz,sYhmc,".",0,nXh,dYys,dZsl,dSum,
	        nSysLkrq,dSum,dSum,1,
            	jjxzm,jjxz,sz,lsybh,ksh,ssrq,szbh,smbh,sm,jch,
	        nSysLkrq,nSysSfm,sLsh)>0) CommitWork();
	
	PrintRsTerrafp(sLsh,2); //���ô�Ʊ
}

static long InsertZwmxTable(char * inbuf,long nmode,long nxh)
{
     char sLsh[41],sRecallLsh[41];  //��ˮ��
     char sYhbz[101]; //�û���־
     char sJym[16];  //������
     char sYhmc[51]; //�û�����
     char sCzydh[7]; //����Ա����
     char sJsdh[10]; //�ɷѾ־ֺ�
     char sLxbz[21];
     double dSjkxj=0.0,dSjkcz=0.0,dSjkxyk=0.0; //ʵ�ɿ��ֽ𡢴��ۡ����ÿ�
     long   nFkfs=1,nCxms;   //���ɷ�ʽ1���ֽ�  2���̿�
     long nCount,nCsbz=0,nXzbz=2,nYsnd,nYsyf;
     double dSjk;  //ʵ�ɿӦ�ɿ�
     long nSfm=0;

     FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|%d|",
                       sLsh,sYhbz,sYhmc,&nFkfs,&dSjk,sCzydh,&nSfm,&nCount);
     strcpy(sRecallLsh,sLsh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     
     if(sJym[1]!='2')  nCount=1;
     RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);
     switch(nCxms)
     {
         case 130:
         case 140:
         case 141:
         case 210:
         case 220:
         case 230:
         case 821:
         case 1001:
         case 123:
         case 124:
         case 522:
              nCsbz=1;
              nXzbz=2;
              break;
         default:
              nCsbz=0;
              nXzbz=2;
	      break;
     }
     switch(nmode)
     {
     	
     	case TASK_PAY_CONFIRM:
     	case TASK_POWER_PAY_CONFIRM:
     	case TASK_MOBILE_PAY_CONFIRM:
             strcpy(sLxbz,"00000000000000000000");
             break;
        case TASK_RECALL_CONFIRM:
             dSjk=-dSjk;
             nCount=-nCount;
             strcpy(sLxbz,"00000000000000000010"); 
             strcpy(sRecallLsh,sYhmc);
             strcpy(sYhmc,".");
             break;
        default:
             strcpy(sLxbz,"00000000000000000000");
             break;

     }
     
     switch(nFkfs)  //���ݸ��ʽ�ж����ֽ�ɿ�����ÿ�
     {
     	case 1:
     	     dSjkxj=dSjk;
     	     break;
        case 2:
             dSjkcz=dSjk;
             break;
     }
     
        
     nYsnd=nSysLkrq/10000;
     nYsyf=(nSysLkrq%10000)/100;
     return RunSql("insert into dl$zwmx(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,nywlb,nxh,nsl,"
     				"dyjzk,dxzyjk,dsjk,ddlsxf,dybk,dytk,dsjkxj,dsjkcz,dsjkxyk,"
     				"nsfkfs,cjkdh,cjh,cczydh,nlkrq,nsfm,clxbz,"
     				"nxzbz,ncsbz,ncscs,njsbz,cwbjsdh,"
                    "cnbjsdh,cbl,nbl,cbl1,cbl2,cbl3,cbl4,cbl5,clsh,cylsh) "
                    "VALUES(%s,%ld,%ld,%s,%s,'0',%s,0,%ld,%ld,"
                    "%lf,0,%lf,%lf,0,0,%lf,%lf,%lf,"
                    "%ld,'000000000000000',%s,%s,%ld,%ld,%s,"
                    "%ld,%ld,0,0,'00000000000000000000',"
                    "'00000000000000000000','0',0,'0','0','0','0','0',%s,%s)",
                    sJym,nYsnd,nYsyf,sYhbz,sYhbz,sYhmc,nxh,nCount,
                    dSjk,dSjk,0.0,dSjkxj,dSjkcz,dSjkxyk,
                    nFkfs,sJsdh,sCzydh,nSysLkrq,nSfm,sLxbz,
                    nXzbz,nCsbz,sLsh,sRecallLsh);

}

//for wc
void InsertJzzl()
{
    FILE *fp;
    long nXh=0,nId,nKey;
    char sFileName[60]="tax.txt";
    char sYbz[22],sXbz[8],sDz[80];
    char sData[1024],sTemp[60],sName[50];
    char *offset;

    GetSysLkrq();
    clrscr();
       
    outtext("\n�������...");
    nXh=0;
    if(!(fp=fopen(sFileName,"rt")))
    {
    	outtext("\n���ļ�����!");
    	keyb();
    	return;
    }
   
    for(;;)
    {
    	memset(sData,0,sizeof(sData));
    	if(getline(sData,fp)<=0) break;
    	alltrim(sData);
        offset=sData;
        nXh++;

        offset=FetchStringToArg(sData,"%s|%s|%s|%s|%s|%s",sXbz,sYbz,sTemp,sTemp,sDz,sName);
        
        if(!offset)
        {
            clrscr();
            outtext("\n�ļ���ʽ����[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nId=-1;
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
                
        nId=RunSql("INSERT INTO dl$taxzl(cyyhbz,cxyhbz,cyhmc,cyhdz) VALUES (%s,%s,%s,%s) ",sYbz,sXbz,sName,sDz);
        
        if(nId<0)
        {
            clrscr();
            GetSqlErrInfo(sData);
            outtext("\n������[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        if((nXh%1000)==0) CommitWork();    
        outtext("\n %8ld  %s  %s  ",nXh,sYbz,sXbz);
    }        
    CommitWork();
    fclose(fp);
    return;   
}

void QueryJzzl()
{
	char sYbz[22];

	clrscr();
	sYbz[0]='\0';
	while(1)
	{
	  if(GetValue(2,2,"������ԭ��˰���:%-20.20s",sYbz)==KEY_ESC) return;
	  alltrim(sYbz);
	  if(strlen(sYbz)>=5) break;
	}
	QueryLocal("��˰�˾ɱ���            ��˰���±���  ��˰������                          ",
      	           "%s%s%s","select distinct cyyhbz,cxyhbz,cyhmc from dl$taxzl where cyyhbz=%s ",sYbz);
	
	return;
}
