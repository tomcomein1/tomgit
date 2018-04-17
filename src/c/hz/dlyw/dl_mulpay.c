#include "tools.h"
#include "dl_pay.h"

extern long nCxms,nSysLkrq,nSysSfm;
extern char sJym[16],sRemoteHost[20],sJymc[30];
extern char sSysJh[10],sSysJsdh[11];
extern YWMX rYwmx[YWMX_ELEMENT_LEN+1];
extern long nBl3[YWMX_ELEMENT_LEN+1],nBl4[YWMX_ELEMENT_LEN+1];
extern char sPacket[257],sLsh[21];
extern char sSysCzydh[7],sSysCzymc[20],sTcpipErrInfo[81];

//extern long QulSend(long nTaskNo);
//extern void PayPrint(long nType);
static long N[30],I=0,P[30];
static DATATABLE * pdata=NULL;
static YWMX show,allmx[YWMX_ELEMENT_LEN+1];
static DIALOG * pdmxs=NULL;
static long MulInitDlywPay();
static long MulAddPayDetailDialog();
static void MulEndDlywPay();
static void MulStdpay_Input();
static long MulValidateCheck(char *sYhbz1);

void  MulDlywPay(long ntype,char *cjymc,char *cjym)
{
	clrscr();
	strcpy(sJym,cjym);
	if(MulInitDlywPay()<0) return;
	memset(&nBl3,0,sizeof(nBl3));
	memset(&nBl4,0,sizeof(nBl4));

	MulStdpay_Input();
		
	MulEndDlywPay();	
}

static void MulStdpay_Input()
{
    long nCurrent,nwhere,nModifyFlag,nKey;
    long nYsnd,nYsyf,nTaskNo=0,iLoop,nJkyfs=0,nSelect=0;
    char sOpt[2],sYhbz[23],sDjh[14],sYhmc[81],sLs[41],sYh[10];
    char sDate[120]="\0",sbuf[256],sTemp[240];
    double dYjk=0,dSjk=0,dZk;
    
    nCurrent=1;//start with the first line
    nwhere=1;
    nModifyFlag=0;//the document is modified if the flag is nozero
    for(;;)
    {
	//AssignTableFields(pdata,"copt,cyhbz,nysnd,nysyf,dyjk,dsjk,cdjh,cyhmc,clsh");
	if(AutoExtendTable(pdata,&nCurrent)>0)
           ImportData(pdata,nCurrent,"copt,cdjh","+",".");

        switch(nwhere)
        {
            case 1:
            ShowDatatableInfo(pdata,"�����빦�ܿ��ƣ���M����ʾ���ܼ�����,S���ύ����",0);
            break;
            case 2:
            ShowDatatableInfo(pdata,
            "�������ѯ�û���־���������Ѳ�ѯ�ĺ����ظ���",0);
            break;
            case 3:
            ShowDatatableInfo(pdata,"������ʵ�ɿ���,��������ΪԤ��",0);
            break;
            
	    default:
	    	ShowDatatableInfo(pdata,"~~~~~~~~~",0);
	    break;
        }

	//Fetch data into container
        nKey=FetchData(pdata,nCurrent,nwhere,"copt,cyhbz,dsjk");

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
			nKey=MessageBox("�����Ҫ�˳��ۺϽɷ�ҵ����",0);
			if(nKey==KEY_ESC)
			{
                                ShowDataTable(pdata,2,4);
			        continue;
			}
			if(nKey==KEY_YES) return;
		    }
		    
	            return;

           default:
                    DefaultTableKey(pdata,nKey,&nCurrent,&nwhere);
                    continue;
	}

        //Export data from container which you inputed
	ExportData(pdata,nCurrent,
	"copt,cyhbz,nYsnd,nysyf,dyjk,dsjk,cdjh,cyhmc,clsh",
	sOpt,sYhbz,&nYsnd,&nYsyf,&dYjk,&dSjk,sDjh,sYhmc,sLs);

 	ImportData(pdata,nCurrent,"copt","+");
	if(nSelect)
	{
		switch(sOpt[0])
		{
			case 'u':
			case 'U':
			case 'd':
			case 'D':
			case 'p':
			case 'P':
			case 'm':
			case 'M':
				break;
			default:
			 ShowDatatableInfo(pdata,"����'u','d'�鿴,����ʧ�ܺ��룬�Ա��ؽ�����'p'����Ʊ.",1);
			 continue;
		}//end sw
	}//end ctrl
	
	switch(nwhere)
	{
	   case 1: //��������
		    switch(sOpt[0])
		    {
                             case 's':   //save as temporary record
			     case 'S':
	  			dZk=0;
	  			memset(sTemp,'\0',strlen(sTemp));
	  			if(pdata->nCount==1) continue;   //������޸Ķ�
	  			for(iLoop=1;iLoop<=pdata->nCount;iLoop++)
	  			{
				  memset(sbuf,'\0',sizeof(sbuf));
				  ExportData(pdata,iLoop,"cyhbz,nYsnd,nysyf,dyjk,dsjk,cdjh,cyhmc,clsh",
					sYhbz,&nYsnd,&nYsyf,&dYjk,&dSjk,sDjh,sYhmc,sLs);

				  alltrim(sYhbz); if(sYhbz[0]=='.') continue;  //������Ϣ
				  
				  sprintf(sPacket,"%-40.40s|%s|%s|1|%.2lf|%s|%ld|%ld|%s|%s|",
					sLs,sYhbz,sYhmc,dSjk,sSysCzydh,nSysSfm,nJkyfs,sDate,".");
					if(AutoRunTask(sRemoteHost,TASK_PAY_CONFIRM,"%s","%s",sPacket,sbuf)<0)
					{      
						sprintf(sbuf,"%s�ɷѳ���:%s",sYhbz,sTcpipErrInfo);
						ShowDatatableInfo(pdata,sbuf,1);
						strcat(sTemp,sYhbz);
						strcat(sTemp,"|");
						ImportData(pdata,iLoop,"cdjh","ʧ��");
						continue;
					}
					if(strncmp(sbuf,"000000",6))
					{
						sprintf(sbuf,"%s�ɷѳ���:%s",sYhbz,sTcpipErrInfo);
						ShowDatatableInfo(pdata,sbuf,1);
						strcat(sTemp,sYhbz);
						strcat(sTemp,"|");
						ImportData(pdata,iLoop,"cdjh","ʧ��");
						continue;
					}//ERRO MESSAGE
					ImportData(pdata,iLoop,"cdjh","�ɹ�");
					dZk+=dSjk;
					if(sTemp[0])
					  if(RunSql("update dl$ywmx_%t set cbz=%s where clsh=%s ",sJym,sTemp,sLs)>0) CommitWork();
				}//end for 
				
				sprintf(sbuf,"�ɷѳɹ�:�ܽ��:%.2lf",dZk);
				ShowDatatableInfo(pdata,sbuf,1);
				RefreshDataTable(pdata);
				nSelect=1;
				continue;
				
				case 'p':
				case 'P':
				//for print fp
				for(iLoop=1;iLoop<=pdata->nCount;iLoop++)
	  			{
	  				ExportData(pdata,iLoop,"cyhbz,clsh",sYhbz,sLsh);
	  				alltrim(sYhbz); if(sYhbz[0]=='.' || strstr(sTemp,sYhbz)) continue;  //������Ϣ
	  				PayPrint(1);
				}//~~ 
				
				while(pdata->nCount>0)
					DeleteDataNode(pdata,pdata->nCount);
				RefreshDataTable(pdata);  //ɾ���ɷѼ�¼
				clrscr();  ShowDataTable(pdata,2,4);
				nCurrent=nwhere=1;
				I=nSelect=0;
				continue;
				
				case 'r':
				case 'R':
				if(nCurrent==pdata->nCount) continue;
			   	savewin();
				//printf("(%ld)|%s|%ld|%ld|%lf|\n",iLoop,rYwmx[0].cYhbz,rYwmx[0].nYsnd,rYwmx[0].nYsyf,rYwmx[0].dYjzk);
			   	ShowDialog(pdmxs,30,4);
				for(iLoop=0;iLoop<N[nCurrent];iLoop++)
			   	{
			   	  show=allmx[P[nCurrent-1]+iLoop];
			   	  RefreshDialog(pdmxs,0);
			   	  if(keyb()==KEY_ESC) break;
			   	} //SHOW MX RECORD
			   	clrscr();
			   	popwin();
			   	continue;
			        
			        default:
			         DefaultTableChoice(pdata,sOpt[0],
			                      &nCurrent,&nwhere,&nModifyFlag);
			         continue;
		    }
		    break;

	   case 2:
	            nModifyFlag=1;
	            alltrim(sYhbz);
		    if(!MulValidateCheck(sYhbz)) continue;

	            nKey=FindDataTable(pdata,1,"cyhbz",sYhbz);
	            if(nKey==nCurrent) 
	               nKey=FindDataTable(pdata,nCurrent+1,"cyhbz",sYhbz);

	            if(nKey>0)
	            {
                        ShowDatatableInfo(pdata,"���û���־�Ѿ����ڣ�����������",1);
                        continue;
                    }
		    
	    	    if(GenerateLsh(1,sSysJsdh,sJym,sLs)==0)  continue;
		    		CommitWork();
		    
		    	dYjk=0;
		    	memset(sPacket,'\0',sizeof(sPacket));    
    		    	if(nCxms==821)
    		    	{
    			  sprintf(sPacket,
            			"%-40.40s|%5.5s|%-22.22s|%-6.6s|",
            			sLs,"10000",sYhbz,sSysCzydh);
		    	}
    		    	else
    		    	{
        		  sprintf(sPacket,
            			"%-40.40s|%5.5s|%-20.20s%-80.80s|%-6.6s|",
            			sLs,"10000",sYhbz,"...",sSysCzydh);
    		    	}
		    
	    	    	//printf("send_pack:%s",sPacket); keyb();
		    	if(QulSend(nTaskNo)<0)  continue;
		    
                    	for(iLoop=0;iLoop<100;iLoop++)  //������
                    	{	
                    	//printf("(%ld)|%s|%ld|%ld|%lf|\n",iLoop,rYwmx[iLoop].cYhbz,rYwmx[iLoop].nYsnd,rYwmx[iLoop].nYsyf,rYwmx[iLoop].dYjzk);
			  dYjk+=rYwmx[iLoop].dYjzk;
                    	  if(strlen(rYwmx[iLoop].cYhbz)<4) break;
                    	  allmx[I++]=rYwmx[iLoop];
                    	}
			P[nCurrent]=I;
			N[nCurrent]=iLoop;
                    	{nYsnd=rYwmx[0].nYsnd;
                    	nYsyf=rYwmx[0].nYsyf;
                    	strcpy(sYhmc,rYwmx[0].cYhmc);
			strncpy(sDjh,rYwmx[0].cYhmc,10);
			sDjh[10]='\0';
		    	dSjk=dYjk; }//���±���
                    	
                    if(nCxms==130) {nwhere=3; continue;}
                    else {nwhere=2;}

                    break;
	   case 3:
                    nwhere=2;
                    break;
           default: nwhere=2; continue;
        }//end switch(nwhere);
	
	ImportData(pdata,nCurrent,"copt,cyhbz,nYsnd,nysyf,dyjk,dsjk,cdjh,cyhmc,clsh",
			sOpt,sYhbz,nYsnd,nYsyf,dYjk,dSjk,sDjh,sYhmc,sLs);
	nCurrent++;

   }//end for(;;)

}

static long MulValidateCheck(char *sYhbz1)
{
	long nLoop;
	char sTemp[16]="\0";
    	char sXjrq[16]=".";
    	long nYear,nDay,nMonth,nLkrq=0;

	switch(nCxms)
	{
		case 110:
		case 111:
			for(nLoop=0;nLoop<=10;nLoop++)
				if(sYhbz1[nLoop]<'0' || sYhbz1[nLoop]>'9' ) 
					return 0;
			if(strlen(sYhbz1)==11 
					&& !strncmp(sYhbz1,"13",2)
					&& strchr("0123",sYhbz1[2]))
				return 1;
			break;
		case 130:
			for(nLoop=0;nLoop<=10;nLoop++)
			if(sYhbz1[nLoop]<'0' || sYhbz1[nLoop]>'9' ) 
				return 0;
			if(strlen(sYhbz1)==11 
				&& !strncmp(sYhbz1,"13",2)
				&& strchr("56789",sYhbz1[2])) 
				return 1;
		case 230:
		case 220:
		case 210:
			if(strchr(sYhbz1,'-') && strlen(sYhbz1)>8) return 1;
			break;
		case 610:
			if(strlen(sYhbz1)>1) return 1;
			break;
		case 140:
		case 141:
			for(nLoop=0;nLoop<=6;nLoop++)
				if(sYhbz1[nLoop]<'0' || sYhbz1[nLoop]>'9' )
					return 0;
			return 1;

		case 821:
			if(strlen(sYhbz1)==22) 
				return 1;
			break;
		
		case 123:
		case 124:
			if(strlen(sYhbz1)>=11) return 1;
			break;
		case 521:
		case 522:
		case 223:
			//if(strstr(sSysJsdh,"271502"))
			//	if(strncmp(sYhbz1,"20",2)) break;
			if(strlen(sYhbz1)<5) return 0;
			RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=1 and cjym=%s into %ld",sJym,&nLkrq);
             		if(nSysLkrq>=nLkrq) 
             		{
             			ShowDatatableInfo(pdata,"��[��]˰ҵ�����Ѿ�ֹͣ����,���������",0);
             			keyb();
             			break;
             		}//����			
			return 1;
		case 222: 
			//if(strlen(sYhbz1)>14) return 1;
			if(strlen(sYhbz1)>4) return 1;
			break;
		case 333:
			if(strlen(sYhbz1)<14) break;
			if(nCxms==333 && strstr(sSysJh,"2715"))
             		{
             			alltrim(sYhbz1); upper(sYhbz1);
             			if(RunSelect("select cbl2 from dl$ywmx_%t where nxzbz=0 and cyhbz=%s into %s",sJym,sYhbz1,sXjrq)>0)
             			{
             			FetchStringToArg(sXjrq,"%2d%1s%2d%1s%4d%9s",&nMonth,sTemp,&nDay,sTemp,&nYear,sTemp);
             			nLkrq=nYear*10000+nMonth*100+nDay;
                		//printf("TEST:RQ=%ld,nY=%ld,nM=%ld,nD=%ld\n",nLkrq,nYear,nMonth,nDay);
                		if(nSysLkrq>GetRandomDate(nLkrq,60,1))
                		{
                  	  	  MessageBox("��ı��պ����ѹ��ɷ��ڣ��뵽���չ�˾���ѣ�",1);
             	  	  	  return 0;
             			}
				}//      
             		}//new for ms
             	        return 1;
		
	}//end switch
	return 0;       
}

static long MulInitDlywPay()
{
    RunSelect("select ncxms,csjydz,cJymc from dl$jym where cjym=%s into %ld%s%s",
               sJym,&nCxms,sRemoteHost,sJymc);
    if(!(pdmxs=CreateDialog(22,20,FRAME)))
    {
    	DropDialog(pdmxs);
    	return -1;
    }
    //��ѯ     sprintf(sPacket,
    //         "%-40.40s|%5.5s|%-20.20s%-20.20s%-20.20s%-20.20s%-20.20s|%-6.6s|",
    //         sLsh,sWt,sYhbz1,sYhbz2,sYhbz3,sYhbz4,sYhbz5,sSysCzydh);
    //�ɷ�
    //sprintf(sPacket,"%-40.40s|%s|%s|%ld|%.2lf|%s|%ld|%ld|%s|%s|",
    //		sLsh,rYwmx[0].cYhbz,rYwmx[0].cYhmc,
    //		nKey,dSjk,sSysCzydh,nSysSfm,nJkyfs,sDate,sSjh);
    
    
    pdata=CreateDataTable(8,"|  |�û���ʶ              |���|�·�| Ӧ�ɿ�� | ʵ�ɽ�� |�û�����  |",
                         "%-1.1s%-22.22s %4d %02d %10.2f %10.2f %-10.10s %80s %40s"); //����yhbz,yhmc,lsh
    if(pdata==NULL)
    {
    	MessageBox("�������ݱ�����...",1);
    	DropDialog(pdmxs);
    	return -2;
    }
    AssignTableFields(pdata,"copt,cyhbz,nysnd,nysyf,dyjk,dsjk,cdjh,cyhmc,clsh");
    SetTableStyle(pdata,ST_MESSAGELINE|ST_ALLTRIM,1);
    
    gotoxy((80-strlen(sJymc))/2,2);
    outtext(sJymc);
    
    ShowDataTable(pdata,2,4);
    MulAddPayDetailDialog();
    return 1;
}

static void MulEndDlywPay()
{
    DropDataTable(pdata);
    DropDialog(pdmxs);
}

static long MulAddPayDetailDialog()
{
    //��д��ͬ����ϸ��ʾ
    switch(nCxms)
    {
       case 130:
             AddDialog(pdmxs,NULL,OUTPUT,"�� ��״ ̬:%6s\n",show.cBl1);
             AddDialog(pdmxs,NULL,OUTPUT,"����Ӧ�տ�:%9.2f\n",&show.dSfmx[24]);
             AddDialog(pdmxs,NULL,OUTPUT,"���������:%9.2f\n",&show.dSfmx[0]);
             AddDialog(pdmxs,NULL,OUTPUT,"�ϴν���:%9.2f\n",&show.dBl2);
             AddDialog(pdmxs,NULL,OUTPUT,"Ƶ  ռ  ��:%9.2f\n",&show.dSfmx[1]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&show.dSfmx[2]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&show.dSfmx[3]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  ;  ��:%9.2f\n",&show.dSfmx[4]);
             AddDialog(pdmxs,NULL,OUTPUT,"��;���ӷ�:%9.2f\n",&show.dSfmx[5]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  Ϣ  ��:%9.2f\n",&show.dSfmx[6]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&show.dSfmx[9]);
             AddDialog(pdmxs,NULL,OUTPUT,"ũ  ��  ��:%9.2f\n",&show.dSfmx[10]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&show.dSfmx[13]);
             AddDialog(pdmxs,NULL,OUTPUT,"Ԥ����:%9.2f\n",&show.dSfmx[14]);
             AddDialog(pdmxs,NULL,OUTPUT,"�츮������:%9.2f\n",&show.dSfmx[15]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&show.dSfmx[16]);
             AddDialog(pdmxs,NULL,OUTPUT,"���������:%9.2f\n",&show.dSfmx[17]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&show.dSfmx[12]);

             break;                                              
                                                                 
       case 140:                                                 
             AddDialog(pdmxs,NULL,OUTPUT,"�¡��⡡��:%9.2f\n",&show.dSfmx[0]);
             AddDialog(pdmxs,NULL,OUTPUT,"�С�������:%9.2f\n",&show.dSfmx[1]);
             if(strstr(sSysJh,"2721")) {                         
             AddDialog(pdmxs,NULL,OUTPUT,"С��ͨ����:%9.2f\n",&show.dSfmx[2]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&show.dSfmx[3]);
             }                                                   
             else {                                              
             AddDialog(pdmxs,NULL,OUTPUT,"����ͨ����:%9.2f\n",&show.dSfmx[2]);
             AddDialog(pdmxs,NULL,OUTPUT,"��;ͨ����:%9.2f\n",&show.dSfmx[3]);
             }                                                   
             AddDialog(pdmxs,NULL,OUTPUT,"����ͨ����:%9.2f\n",&show.dSfmx[4]);
             AddDialog(pdmxs,NULL,OUTPUT,"800 ҵ���:%9.2f\n",&show.dSfmx[6]);
             AddDialog(pdmxs,NULL,OUTPUT,"����绰��:%9.2f\n",&show.dSfmx[7]);
             AddDialog(pdmxs,NULL,OUTPUT,"�̿���ҵ��:%9.2f\n",&show.dSfmx[8]);
             AddDialog(pdmxs,NULL,OUTPUT,"ISDN/ADSL :%9.2f\n",&show.dSfmx[9]);
             AddDialog(pdmxs,NULL,OUTPUT,"��·�����:%9.2f\n",&show.dSfmx[11]);
             AddDialog(pdmxs,NULL,OUTPUT,"����ͨ�ŷ�:%9.2f\n",&show.dSfmx[12]);
             AddDialog(pdmxs,NULL,OUTPUT,"ע�������:%9.2f\n",&show.dSfmx[13]);
             AddDialog(pdmxs,NULL,OUTPUT,"���������:%9.2f\n",&show.dSfmx[14]);
             AddDialog(pdmxs,NULL,OUTPUT,"һ���Է���:%9.2f\n",&show.dSfmx[16]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  ������:%9.2f\n",&show.dSfmx[17]);
             AddDialog(pdmxs,NULL,OUTPUT,"�š��ݡ���:%9.2f\n",&show.dSfmx[18]);
             AddDialog(pdmxs,NULL,OUTPUT,"��  �ɡ���:%9.2f\n",&show.dSfmx[19]);
             AddDialog(pdmxs,NULL,OUTPUT,"�����Ѻϼ�:%9.2f\n",&show.dSfmx[24]);
             break;
        case 222:
	     AddDialog(pdmxs,NULL,OUTPUT,"ע������:%-10s\n",&show.cBl1);
             AddDialog(pdmxs,NULL,OUTPUT,"˰������:%-9s\n",&show.cBl2);
	     AddDialog(pdmxs,NULL,OUTPUT,"˰Ʊ��ע:%-9s\n",&show.cBl4);
	     AddDialog(pdmxs,NULL,OUTPUT,"��˰���:%9.2lf\n",&show.dSfmx[1]);
             AddDialog(pdmxs,NULL,OUTPUT,"˰    ��:%9.2lf\n",&show.dSfmx[2]);
             AddDialog(pdmxs,NULL,OUTPUT,"ʵ��˰��:%9.2lf\n",&show.dSfmx[3]);
	     break;
     
	default: break;
     }
}
