/*************************************************************************
** @Copyright........... 2000.7
**filename       �ļ���: dl_pay.c
**target           Ŀ��: ���ս���
**create time  ��������: 2001.4
**original programmer          ԭʼ�����Ա: ��֥��
**edit history �޸���ʷ:
**date����    editer�޸���Ա     reasonԭ�� 
**************************************************************************/
#include "dl_pay.h"
extern char sTcpipErrInfo[81];  //��������
extern char sSysIp[21];         //����IP��ַ
extern char sSysJh[10];         //�������ľֺ�
extern char sSysJm[20];
extern char sSysJsdh[10];       //��֧�ִ���
extern char sSysCzydh[7];       //ϵͳ����Ա����
extern char sSysCzymc[9];       //ϵͳ����Ա
extern char sJym[16];           //������
extern char sJymc[51];          //��������
extern char sRemoteHost[16];    //����Դ��ַ
extern long nSysLkrq;           //ϵͳ¼������
extern long nSysSfm;            //ϵͳʱ����
extern long nCxms;              //����ģʽ
static long nWt=1;
//��������  1 -- ����ҵ��(����Ԥ��)  2 -- ��������ҵ�񵥾�  3 -- ��������  4 -- ��¼���շ�

long nSelect=0;                //�Ƿ�����ѡ�񽻷�
long nModify=0;                //�Ƿ������޸Ľɿ��
long nModyhmc=0;               //�Ƿ����������û�����
long nDsyc=0;                  //�շ����ʹ���(0)����Ԥ��(1)
long nJls=0;                   //�ɷѼ�¼�� 0 ���ѯ��������ȫ�� ���������ɷѼ�¼��
long nJkyfs=0;
long nBl3[YWMX_ELEMENT_LEN+1]; //ҵ��ṹ���ֶβ���
long nBl4[YWMX_ELEMENT_LEN+1]; //ҵ��ṹ���ֶβ���
double dYjzk=0.0;              //�û��ܼ�Ӧ���ܿ� 
double dSjk=0.0;               //�û�ʵ�ʽɿ�
static double dSumMoney=0.0;	//����ͳ��һ���û��ɼ����û���

char sYhbz1[23];               //�û���־1���û����룩
char sYhbz2[21];               //�û���־2����ͬ���룩
char sYhbz3[21];               //�û���־3��Ԥ����־��
char sYhbz4[21];               //�û���־4��Ԥ����־��
char sYhbz5[21];               //�û���־5��Ԥ����־��
char sYhmc[81];                //�û�����
char sPacket[257];                 //�ϴ��ַ���
char sLsh[41];                 //�ɷ���ˮ��

YWMX rYwmx[YWMX_ELEMENT_LEN+1];//����ҵ����ϸ��¼�����Դ�ŽɷѼ�¼��
static YWMX rmx;                      //��ʾҵ����ϸ�ṹ��
static DIALOG * pd=NULL;              //����ҵ���շѽ���Ի���ָ��
static DIALOG * pdmx=NULL;            //����ҵ���շ���ϸ�Ի���ָ
static DATATABLE * pdt=NULL;

void  DlywPay(long ntype,char *cjymc,char *cjym)
{
	int  nkey=0;
	long nTaskNo=0,nflag=0;

	clrscr();
	strcpy(sJym,cjym);   
	//���³�ʼ��
	InitDlywPay();
	
	//��������ɷ�����   
	for(;;)
	{
		clearline2(22,23);
		Clear_Space(1,4,46,6);
		while(pdt->nCount>0) 
			DeleteDataNode(pdt,pdt->nCount);
		nSelect=0;
		nModify=0;
		nModyhmc=0;
		nDsyc=0;
		memset(sLsh,0,sizeof(sLsh));
		memset(sYhbz1,'\0',sizeof(sYhbz1));
		memset(sYhbz2,'\0',sizeof(sYhbz2));
		memset(sYhbz3,'\0',sizeof(sYhbz3));
		memset(sYhbz4,'\0',sizeof(sYhbz4));
		memset(sYhbz5,'\0',sizeof(sYhbz5));
 
		dYjzk=0.0;
		dSjk=0.0;
		memset(&rmx,0,sizeof(YWMX));
		memset(&nBl3,0,sizeof(nBl3));
		memset(&nBl4,0,sizeof(nBl4));
		if(nCxms!=111) RefreshDialog(pdmx,0);

		if(Stdpay_Input()<0)
			break;
		
		//���²�ѯ����   
		if(QulSend(nTaskNo)<0)
			continue;
     
		//��ʾ�ɷ���ϸ
		if(Stdpay_Show()<0)
			continue;

		
		//��������ҵ��ɷѱ���--sPacket�������--nTaskNo ��������÷��ʹ������
		
		//�ɷѷ��ͽ���
		if(PaySend(nTaskNo)<0)
			continue;
    	
    	//����������ӡָʾ�����µ��ô�ӡ����
		PayPrint(1);
		
		//����������̣����Ǳ���
		AdditionProcess(); 
	}//end for(;;)
	
	EndDlywPay();
	return;
}

long Stdpay_Input()
{
    long nKey=0,nQzrq=0;
    char sWt[6]="00000";
    char sTemp[16]="\0",zgswbm[3]=".";
    char sXjrq[20]=".";
    long nYear,nDay,nMonth,nLkrq=0;
    
    CommitWork();
    clearline(23);
    FillTable(pdt->pTable,7,"> %s > %s","           ","          "); 

    for(;;)
    {
        switch(nCxms)
        {
        case 110:
             nSelect=0;
             nModify=0;
             nModyhmc=0;
             nDsyc=0;
             nKey=GetValue(1,4," ������绰����:%-11.11s",sYhbz1);
             nWt=1;
             break;
        case 111:
             nSelect=0;
             nModify=1;
             nModyhmc=1;
             nDsyc=1;
             nKey=GetValue(1,4," ������绰����:%-11.11s",sYhbz1);
             nWt=1;
             break;
        case 130:
             nSelect=0;
             nModify=1;
             nModyhmc=0;
             nDsyc=0;
             //nKey=GetValue(1,4," ������绰����:%-11.11s",sYhbz1);
             //nWt=1;
             nKey=GetMultiValue(1,4,&nWt,
             	" ������绰����:%-11.11s; �������ͬ����:%-11.11s;",
             	sYhbz1,sYhbz2);
             break;

        case 140:
	case 141:
             nSelect=0;
             nModify=0;
             nModyhmc=0;
             nDsyc=0;
             nKey=GetMultiValue(1,4,&nWt,
             " ������绰����:%-11.11s; �������ͬ����:%-20.20s; ������97��ͬ��:%-20.20s;",
             sYhbz1,sYhbz2,sYhbz3);
             if(strstr(sSysJh,"2704"))
             {
             	strcpy(sTemp,sYhbz2+2);
             	strcpy(sYhbz2,sTemp);
             	//printf("TEST:HDH=%s",sYhbz2);
             }
             break;

        case 230:
        case 220:
        case 210:
             nSelect=0;
             nModify=0;
             nModyhmc=0;
             nDsyc=0;
             nKey=GetValue(1,4," �����봫������:%-20.20s",sYhbz1);
             nWt=1;
             break;
        case 610:
             nSelect=1;
             nModify=0;
             nModyhmc=0;
             nDsyc=0;
             nKey=GetValue(1,4," �������������:%-20.20s",sYhbz1);
             upper(sYhbz1);
             nWt=1;
             break;
        case 821:
        case 333:
             nSelect=0;
             nModify=0;
             nModyhmc=0;
             nDsyc=0;
             nKey=GetValue(1,4," �����뱣������:%-22.22s",sYhbz1);
             if(nCxms==333 && strstr(sSysJh,"2715"))
             {
             	alltrim(sYhbz1); upper(sYhbz1);
             	if(RunSelect("select cbl2 from dl$ywmx_%t where nxzbz=0 and cyhbz=%s into %s",sJym,sYhbz1,sXjrq)>0)
             	{
             		FetchStringToArg(sXjrq,"%2d%1s%2d%1s%4d%9s",&nMonth,sTemp,&nDay,sTemp,&nYear,sTemp);
             		nLkrq=nYear*10000+nMonth*100+nDay;
                	//printf("TEST:RQ=%ld,nY=%ld,nM=%ld,nD=%ld\n",nLkrq,nYear,nMonth,nDay);
                	//if(nSysLkrq>(nLkrq+200))
                	if(nSysLkrq>GetRandomDate(nLkrq,60,1) )
                	{
                  	  MessageBox("��ı��պ����ѹ��ɷ��ڣ��뵽���չ�˾���ѣ�",1);
             	  	  return -1;
             		}
		}//      
             }//new for ms
             
             nWt=1;
             break;
        case 123:
        case 124:
        case 125:
             nSelect=1;
             nModify=0;
             nModyhmc=0;
             nDsyc=0;
             nKey=GetValue(1,4," �������û�����:����š�%-11.11s",sTemp);
	     if(nCxms==125) { strcpy(sYhbz1,sTemp); nWt=1; break; }
	     
	     if(strstr(sSysJh,"2708") && nCxms==123)
	       strcpy(sYhbz1,sTemp);
	     else
	       sprintf(sYhbz1,"6205%s",sTemp);
             nWt=1;
             break;
	case 521:        
	case 522:
	case 222:
	case 223:
	     RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=1 and cjym=%s into %ld",sJym,&nQzrq);
             if(nSysLkrq>=nQzrq) 
             {
             	outtext("\n\n  ��[��]˰ҵ�����Ѿ�ֹͣ���գ� \n  ���������...��������");
             	keyb();
             	return -1;
             }//����
             nSelect=0;
             nModify=0;
             nModyhmc=0;
             nDsyc=0;
             nKey=GetValue(1,4," ��������˰�˱���:%-20.20s",sYhbz1);
	     nWt=1;
             break;
        default:
             bell();
             continue;
       }//end switch
       
       if(nKey==KEY_ESC) return -1;
       if(nKey!=KEY_RETURN) continue;
       alltrim(sYhbz1);
       alltrim(sYhbz2);
       alltrim(sYhbz3);
       alltrim(sYhbz4);
       alltrim(sYhbz5);
       upper(sYhbz1);
       upper(sYhbz2);
       upper(sYhbz3);
       upper(sYhbz4);
       upper(sYhbz5);
	   if(nWt==1) if(strlen(sYhbz1)<2) dSumMoney=0.0;
	   if(nWt==2) if(strlen(sYhbz2)<2) dSumMoney=0.0;
	   if(nWt==3) if(strlen(sYhbz3)<2) dSumMoney=0.0;

       if(dSumMoney==0) clearline(1);
       if(ValidateCheck(nWt)>0) break;
    }//end for(;;)


    sWt[nWt-1]='1';
    strcpy(rYwmx[0].cYhbz,sYhbz1);
    strcpy(rYwmx[0].cYhbz1,sYhbz2);

    if(nWt!=1) strcpy(sYhbz1,"....................");
    if(nWt!=2) strcpy(sYhbz2,"....................");
    if(nWt!=3) strcpy(sYhbz3,"....................");
    if(nWt!=4) strcpy(sYhbz4,"....................");
    if(nWt!=5) strcpy(sYhbz5,"....................");

    if(GenerateLsh(1,sSysJsdh,sJym,sLsh)==0)  return -1;
    CommitWork();
    
    if(nCxms==821)
    {
    	sprintf(sPacket,
            "%-40.40s|%5.5s|%-22.22s|%-6.6s|",
            sLsh,sWt,sYhbz1,sSysCzydh);
    }
    else
    {
        sprintf(sPacket,
            "%-40.40s|%5.5s|%-20.20s%-20.20s%-20.20s%-20.20s%-20.20s|%-6.6s|",
            sLsh,sWt,sYhbz1,sYhbz2,sYhbz3,sYhbz4,sYhbz5,sSysCzydh);
    }
    return 0;
}

/*========================================================
������ValidateCheck
���ܣ���������û���ʶ�ĺϷ���
������ 
      nWt:λͼ���

���أ�0������ ����ɹ�      
*========================================================*/
long ValidateCheck(long nWt)
{
	long nLoop;

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
			switch(nWt)
			{
				case 1:

				for(nLoop=0;nLoop<=10;nLoop++)
				if(sYhbz1[nLoop]<'0' || sYhbz1[nLoop]>'9' ) 
							return 0;
				if(strlen(sYhbz1)==11 
					&& !strncmp(sYhbz1,"13",2)
					&& strchr("56789",sYhbz1[2])) 
				return 1;
				
				case 2:
					for(nLoop=0;nLoop<=10;nLoop++)
					if(sYhbz2[nLoop]<'0' || sYhbz2[nLoop]>'9' ) 
							return 0;
					if(strlen(sYhbz2)==11 
						&& !strncmp(sYhbz2,"13",2)
						&& strchr("56789",sYhbz2[2])) 
					return 1;
					default: break;
			}//end switch
			break;
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
			switch(nWt)
			{
				case 1:
					for(nLoop=0;nLoop<=6;nLoop++)
						if(sYhbz1[nLoop]<'0' || sYhbz1[nLoop]>'9' )
							return 0;
					return 1;

				case 2:
					/*nLoop=0;
					while(sYhbz2[nLoop])
					{
						if(sYhbz2[nLoop]<'0'||sYhbz2[nLoop]>'9') 
							sYhbz2[nLoop]=' ';
						nLoop++;
					}*/
					alltrim(sYhbz2);
					if(strlen(sYhbz2)<5) 
						return 0;
					return 1;

				case 3:
					/*nLoop=0;
					while(sYhbz3[nLoop])
					{
						if(sYhbz3[nLoop]<'0'||sYhbz3[nLoop]>'9') 
							sYhbz3[nLoop]=' ';
						nLoop++;
					}*/
					alltrim(sYhbz3);
					if(strlen(sYhbz3)<5) 
						return 0;
					return 1;
                   
				default:
					return 0;   
				}  
		case 821:
			if(strlen(sYhbz1)==22) 
				return 1;
			break;
		case 333:
			if(strlen(sYhbz1)==15) 
				return 1;
			break;
		case 123:
		case 124:
			if(strlen(sYhbz1)>=11) return 1;
			break;
		case 125:
			if(strlen(sYhbz1)>=8) return 1;
			break;
		case 521:
		case 522:
		case 223:
			//for(nLoop=0;nLoop<=6;nLoop++)
			//   if((sYhbz1[nLoop]<'0' || sYhbz1[nLoop]>'9')) 
			
			if(strstr(sSysJsdh,"271502"))
				if(strncmp(sYhbz1,"20",2)) break;
			
			if(strlen(sYhbz1)>=5) return 1;
			break;
		case 222: 
			//if(strlen(sYhbz1)>14) return 1;
			if(strlen(sYhbz1)>4) return 1;
			break;
			
	}//end switch
	return 0;       
}


/******************************************************************************
                            ��ѯ����ģ��
��ѯ������ṹ����ˮ��|��������λͼ|������������|
              char(40)  char(5)      char(<=100)
              ��ˮ�ţ�[9λ�ֺ�+15λ������+8λ����+8λ���]
���أ�>0�ɹ�
              [����RECORD]
              �ɹ���000000|�û�����|��ͬ��|Ԥ����|��ϸ��¼��|{���|�·�|25���շ���ϸ...|����Ӧ��|}....
              ʧ�ܣ�XXXXXX|������Ϣ���ֽ���|
******************************************************************************/
long QulSend(long nTaskNo)//��Ѱ���ͽ���
{
	int  nRet=0;
	long nRev=0;
	long nKey=1,iLoop=0;
      
	char TempPacket[256];//ת��������ֶ�
	char sRet[7];//���ش���
	char sError[101];//���ش�����
	RECORD   record;
   
	if(nDsyc==1) return 0;

	//clearline2(22,23);
	gotoxy(4,23);
	outtext("���ڲ�ѯ��Ϣ,���Ժ�......                       ");
 
	//��������ҵ���ѯ����--sPacket�������--nTaskNo ��������÷��ʹ������
	//�������벢������صĲ�Ѱ���
	//�γ���ʾ��ҵ��ṹ�ŵ�rYwmx�ṹ��

	if(AutoRunTask(sRemoteHost,TASK_PAY_QUERY,"%s","%r",sPacket,&record)<0)
	{      
		HintError(TASK_ERR,"��ѯ�û���Ϣ�������!"); 
		return -1;
	}
	
	memset(sPacket,'\0',sizeof(sPacket));
	ExportRecord(&record,"%s",sRet);
	alltrim(sRet);
	if(strcmp(sRet,"000000")==0)
	{
		ExportRecord(&record,"%s%s%lf%ld",rYwmx[0].cYhmc,rYwmx[0].cYhbz1,&rYwmx[0].dBl2,&nJls);
		for(iLoop=1;iLoop<nJls;iLoop++)
		{
			strcpy(rYwmx[iLoop].cYhmc,rYwmx[0].cYhmc);
			rYwmx[iLoop].dBl2=rYwmx[0].dBl2;
		}
		for(iLoop=0;iLoop<nJls;iLoop++)
		{
			ExportRecord(&record,"%s%ld%ld%lf[25]%lf%ld(4)%s(4)",
				rYwmx[iLoop].cYhbz,&rYwmx[iLoop].nYsnd,&rYwmx[iLoop].nYsyf,
				rYwmx[iLoop].dSfmx,&rYwmx[iLoop].dYjzk,&rYwmx[iLoop].nBl1,
				&rYwmx[iLoop].nBl2,&nBl3[iLoop],&nBl4[iLoop],rYwmx[iLoop].cBl1,
				rYwmx[iLoop].cBl2,rYwmx[iLoop].cBl3,rYwmx[iLoop].cBl4);
		}
		DropRecord(&record);
	}
	else
	{
		ExportRecord(&record,"%s",sTcpipErrInfo);
		DropRecord(&record);
		gotoxy(1,23);
		outtext("    %-70.70s",sTcpipErrInfo);
		keyb();
		return -1;
	}
	
	return 0;
}
/*============================================================*
�������ɷѲ�ѯ�ͷ��˳���PaySend
���ܣ������û�Ҫ���ָ�����������������ݳ���ģʽ��ncxms������ѯ
      �����ض��򣬲�����ѯ�����֯��ͳһ�ĸ�ʽ���ظ��ͻ���
������inbuf:��������ַ��������ĸ�ʽ���£��ڸ��ֶ��á�|��������
      
��ˮ��|�û���־|�û�����|�ɿʽ|ʵ���ܿ��|����Ա����|ʱ����|�ɿ��·���|���|�·�|���|�·�|......
char(40) char     char    LONG(2) FLOAT(10.2) char(4)   LONG(8)  LONG(2)

ע�����ɿ��·���Ϊ0�����ʾ�����·ݶ��ɣ�����Ϊ����ֻ���������Ⱥ��·ݵķ���

*============================================================*/
long PaySend(long nTaskNo)//�ɷѷ��ͽ���
{
	char sbuf[256];
  
	//���������ɷ�ҵ����������ýɷѷ��ͺ���
	//���»�Ľɷѷ��ؽ�� 
	sbuf[0]='\0';
	if(AutoRunTask(sRemoteHost,TASK_PAY_CONFIRM,"%s","%s",sPacket,sbuf)<0)
	{      
		outtext("�ɷѳ���:%s",sTcpipErrInfo); 
		keyb();
		return -1;
	}
	if(strncmp(sbuf,"000000",6))
	{
	  gotoxy(1,23);
	  outtext(" �ɷѳ���:%s",sbuf); 
 	  keyb();
	  return -1;
	}//
	switch(nCxms)
	{
		case 230:
		case 220:
		case 210:
			FillTable(pdt->pTable,7,"> %8d > %10.2f",nJkyfs,dSjk); 
			break;
		default:
			break;
	}   
	//�ɷѳɹ�.�ӽ��
	dSumMoney+=dSjk;
	gotoxy(60,1);
	outtext("�ϼ�:%.2lf",dSumMoney);
	
	clearline2(22,23);
	gotoxy(1,23);
	outtext(" �ɷѳɹ�,���������ʼ��ӡ,ESC������...");
	if(keyb()==KEY_ESC) 
		return -1;
	
	return 1;
}

/*--------------------------------------------------------------
������AddPayDetailDialog
���ܣ�������ϸ��ʾ����
˵�������ݲ�ͬ�ĳ���ģʽ��ҵ����ϸ�е�ĳ����������Ӧ����ϸ˵��
      ��ϵ����������rmxΪһ��YWMX�ṹ��ȫ�ֱ����� ע��ÿ����ϸ
      ������һ��������ָ������ϵ�Ҹ�ָ��ָ��ı�����ȫ�ֵġ�
      [��Ҫ����Ա���ݲ�ͬ�ĳ���ģʽ������Ӧ����ϸ��ʾ����]
*--------------------------------------------------------------*/

long AddPayDetailDialog()
{
    //��д��ͬ����ϸ��ʾ
    switch(nCxms)
    {
       case 110:
        
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[0]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[2]);
             AddDialog(pdmx,NULL,OUTPUT,"���ڳ�;��:%9.2f\n",&rmx.dSfmx[3]);
             AddDialog(pdmx,NULL,OUTPUT,"��;���ӷ�:%9.2f\n",&rmx.dSfmx[4]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[5]);
             AddDialog(pdmx,NULL,OUTPUT,"���ʳ�;��:%9.2f\n",&rmx.dSfmx[6]);
             AddDialog(pdmx,NULL,OUTPUT," IP ������:%9.2f\n",&rmx.dSfmx[13]);
             AddDialog(pdmx,NULL,OUTPUT,"Ƶ��ռ�÷�:%9.2f\n",&rmx.dSfmx[7]);
             AddDialog(pdmx,NULL,OUTPUT,"�⿨��ŷ�:%9.2f\n",&rmx.dSfmx[1]);
             AddDialog(pdmx,NULL,OUTPUT,"�� ��Ƿ ��:%9.2f\n",&rmx.dSfmx[9]);
             AddDialog(pdmx,NULL,OUTPUT,"�� ���� ��:%9.2f\n",&rmx.dSfmx[8]);
             AddDialog(pdmx,NULL,OUTPUT,"����������:%9.2f\n",&rmx.dSfmx[24]);
             AddDialog(pdmx,NULL,OUTPUT,"Ԥ���ѳ��:%9.2f\n",&rmx.dSfmx[12]);
             AddDialog(pdmx,NULL,OUTPUT,"Ԥ�������:%9.2f\n",&rmx.dBl1);
             AddDialog(pdmx,NULL,OUTPUT,"165 ʹ�÷�:%9.2f\n",&rmx.dSfmx[14]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[10]);
             AddDialog(pdmx,NULL,OUTPUT,"�ײ����Ż�:%9.2f\n",&rmx.dSfmx[11]);
             AddDialog(pdmx,NULL,OUTPUT,"�������Ż�:%9.2f\n",&rmx.dBl2);
             break;
       
       case 111:
             break; //��ͨԤ������ϸ
       
       case 130:
             AddDialog(pdmx,NULL,OUTPUT,"�� ��״ ̬:%6s\n",rmx.cBl1);
             AddDialog(pdmx,NULL,OUTPUT,"����Ӧ�տ�:%9.2f\n",&rmx.dSfmx[24]);
             AddDialog(pdmx,NULL,OUTPUT,"���������:%9.2f\n",&rmx.dSfmx[0]);
             AddDialog(pdmx,NULL,OUTPUT,"�ϴν���:%9.2f\n",&rmx.dBl2);
             AddDialog(pdmx,NULL,OUTPUT,"Ƶ  ռ  ��:%9.2f\n",&rmx.dSfmx[1]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[2]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[3]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ;  ��:%9.2f\n",&rmx.dSfmx[4]);
             AddDialog(pdmx,NULL,OUTPUT,"��;���ӷ�:%9.2f\n",&rmx.dSfmx[5]);
             AddDialog(pdmx,NULL,OUTPUT,"��  Ϣ  ��:%9.2f\n",&rmx.dSfmx[6]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[9]);
             AddDialog(pdmx,NULL,OUTPUT,"ũ  ��  ��:%9.2f\n",&rmx.dSfmx[10]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[13]);
             AddDialog(pdmx,NULL,OUTPUT,"Ԥ����:%9.2f\n",&rmx.dSfmx[14]);
             AddDialog(pdmx,NULL,OUTPUT,"�츮������:%9.2f\n",&rmx.dSfmx[15]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[16]);
             AddDialog(pdmx,NULL,OUTPUT,"���������:%9.2f\n",&rmx.dSfmx[17]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[12]);
       
             break;
       case 230:
       case 220:
       case 210:
             AddDialog(pdmx,NULL,OUTPUT,"�� �� ��:%9.2f\n",&rmx.dSfmx[0]);
             AddDialog(pdmx,NULL,OUTPUT,"��    ��:%9s\n",&rmx.cBl1);
             AddDialog(pdmx,NULL,OUTPUT,"��������:%9s\n",&rmx.cBl2);
             AddDialog(pdmx,NULL,OUTPUT,"��Ч����:%10s\n",&rmx.cBl3);
             break;

       case 140:
             AddDialog(pdmx,NULL,OUTPUT,"�¡��⡡��:%9.2f\n",&rmx.dSfmx[0]);
             AddDialog(pdmx,NULL,OUTPUT,"�С�������:%9.2f\n",&rmx.dSfmx[1]);
             if(strstr(sSysJh,"2721")) {
             AddDialog(pdmx,NULL,OUTPUT,"С��ͨ����:%9.2f\n",&rmx.dSfmx[2]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ��  ��:%9.2f\n",&rmx.dSfmx[3]);
             }
             else {
             AddDialog(pdmx,NULL,OUTPUT,"����ͨ����:%9.2f\n",&rmx.dSfmx[2]);
             AddDialog(pdmx,NULL,OUTPUT,"��;ͨ����:%9.2f\n",&rmx.dSfmx[3]);
             }
             AddDialog(pdmx,NULL,OUTPUT,"����ͨ����:%9.2f\n",&rmx.dSfmx[4]);
             AddDialog(pdmx,NULL,OUTPUT,"800 ҵ���:%9.2f\n",&rmx.dSfmx[6]);
             AddDialog(pdmx,NULL,OUTPUT,"����绰��:%9.2f\n",&rmx.dSfmx[7]);
             AddDialog(pdmx,NULL,OUTPUT,"�̿���ҵ��:%9.2f\n",&rmx.dSfmx[8]);
             AddDialog(pdmx,NULL,OUTPUT,"ISDN/ADSL :%9.2f\n",&rmx.dSfmx[9]);
             AddDialog(pdmx,NULL,OUTPUT,"��·�����:%9.2f\n",&rmx.dSfmx[11]);
             AddDialog(pdmx,NULL,OUTPUT,"����ͨ�ŷ�:%9.2f\n",&rmx.dSfmx[12]);
             AddDialog(pdmx,NULL,OUTPUT,"ע�������:%9.2f\n",&rmx.dSfmx[13]);
             AddDialog(pdmx,NULL,OUTPUT,"���������:%9.2f\n",&rmx.dSfmx[14]);
             AddDialog(pdmx,NULL,OUTPUT,"һ���Է���:%9.2f\n",&rmx.dSfmx[16]);
             AddDialog(pdmx,NULL,OUTPUT,"��  ������:%9.2f\n",&rmx.dSfmx[17]);
             AddDialog(pdmx,NULL,OUTPUT,"�š��ݡ���:%9.2f\n",&rmx.dSfmx[18]);
             AddDialog(pdmx,NULL,OUTPUT,"��  �ɡ���:%9.2f\n",&rmx.dSfmx[19]);
             AddDialog(pdmx,NULL,OUTPUT,"�����Ѻϼ�:%9.2f\n",&rmx.dSfmx[24]);
             break;

       case 141:
             AddDialog(pdmx,NULL,OUTPUT,"�շѺϼ�:%9.2f\n",&rmx.dSfmx[3]);
             AddDialog(pdmx,NULL,OUTPUT,"Ƿ�Ѻϼ�:%9.2f\n",&rmx.dSfmx[1]);
	     AddDialog(pdmx,NULL,OUTPUT,"�� �� ��:%9.2f\n",&rmx.dSfmx[4]);
	     AddDialog(pdmx,NULL,OUTPUT,"�� Ϣ ��:%9s\n",&rmx.cBl1);
    	     AddDialog(pdmx,NULL,OUTPUT,"Ƿ���·�:%10s\n",&rmx.cBl2);

	    break;

       case 610: 
             AddDialog(pdmx,NULL,OUTPUT,"�������ʣ�%9s\n",&rmx.cBl1);
             AddDialog(pdmx,NULL,OUTPUT,"����������%9.2f\n",&rmx.dSfmx[0]);
             AddDialog(pdmx,NULL,OUTPUT,"���õ��ۣ�%9.2f\n",&rmx.dSfmx[1]);
             AddDialog(pdmx,NULL,OUTPUT,"���ý�%9.2f\n",&rmx.dSfmx[2]);
             AddDialog(pdmx,NULL,OUTPUT,"����������%9.2f\n",&rmx.dSfmx[3]);
             AddDialog(pdmx,NULL,OUTPUT,"���õ��ۣ�%9.2f\n",&rmx.dSfmx[4]);
             AddDialog(pdmx,NULL,OUTPUT,"���ý�%9.2f\n",&rmx.dSfmx[5]);
             AddDialog(pdmx,NULL,OUTPUT,"�����ɽ�%9.2f\n",&rmx.dSfmx[6]);
             AddDialog(pdmx,NULL,OUTPUT,"���Żݿ%9.2f\n",&rmx.dSfmx[24]);
             AddDialog(pdmx,NULL,OUTPUT,"��������: %9.2f\n",&rmx.dDlsxf);
             break;
       case 821:
 	     AddDialog(pdmx,NULL,OUTPUT,"�ɿ�״̬:%9ld\n",&rmx.nBl1);
             AddDialog(pdmx,NULL,OUTPUT,"�ɿ����:%9ld\n",&rmx.nBl2);
             AddDialog(pdmx,NULL,OUTPUT,"ҵ��Ա:%9s\n",&rmx.cBl1);
             AddDialog(pdmx,NULL,OUTPUT,"���ѷ�ʽ:%9s\n",&rmx.cBl2);
             AddDialog(pdmx,NULL,OUTPUT,"�������:%9s\n",&rmx.cBl3); 
             AddDialog(pdmx,NULL,OUTPUT,"��ֹ����:%9s\n",&rmx.cBl4);
             AddDialog(pdmx,NULL,OUTPUT,"�����ܿ�:%9.2lf\n",&rmx.dSfmx[0]); 
             break;
        case 521:
        case 522:
        case 223:
             AddDialog(pdmx,NULL,OUTPUT,"ע������:%-10s\n",&rmx.cBl1);
             AddDialog(pdmx,NULL,OUTPUT,"��˰˰��:%-9s\n",&rmx.cBl2);
             AddDialog(pdmx,NULL,OUTPUT,"��˰˰Ŀ:%-12s\n",&rmx.cBl3);
             AddDialog(pdmx,NULL,OUTPUT,"��˰���:%9.2lf\n",&rmx.dSfmx[1]);
             AddDialog(pdmx,NULL,OUTPUT,"˰    ��:%9.2lf\n",&rmx.dSfmx[2]);
             AddDialog(pdmx,NULL,OUTPUT,"ʵ��˰��:%9.2lf\n",&rmx.dSfmx[3]);
             break;     
        case 123:
        case 124:
 	     AddDialog(pdmx,NULL,OUTPUT,"�� �� ��:%9s\n",&rmx.cBl2);
             AddDialog(pdmx,NULL,OUTPUT,"��ѽ��:%9.2lf\n",&rmx.dSfmx[1]);
             AddDialog(pdmx,NULL,OUTPUT,"Υ Լ ��:%9.2lf\n",&rmx.dSfmx[2]);
             AddDialog(pdmx,NULL,OUTPUT,"��    ��:%9s\n",&rmx.cBl1);
             break;  //�����ϸ
        case 125:
             AddDialog(pdmx,NULL,OUTPUT,"�շѽ��:%9.2lf\n",&rmx.dSfmx[0]);
             AddDialog(pdmx,NULL,OUTPUT,"��    ��:%9.2lf\n",&rmx.dSfmx[1]);
             AddDialog(pdmx,NULL,OUTPUT,"ֹ    ��:%9.2lf\n",&rmx.dSfmx[2]);
             AddDialog(pdmx,NULL,OUTPUT,"��������:%9.2lf\n",&rmx.dSfmx[3]);
             AddDialog(pdmx,NULL,OUTPUT,"�� �� ��:%9.2lf\n",&rmx.dSfmx[4]);
             break;     
	case 222:
	     AddDialog(pdmx,NULL,OUTPUT,"ע������:%-10s\n",&rmx.cBl1);
             AddDialog(pdmx,NULL,OUTPUT,"˰������:%-9s\n",&rmx.cBl2);
	     AddDialog(pdmx,NULL,OUTPUT,"˰Ʊ��ע:%-9s\n",&rmx.cBl4);
	     AddDialog(pdmx,NULL,OUTPUT,"��˰���:%9.2lf\n",&rmx.dSfmx[1]);
             AddDialog(pdmx,NULL,OUTPUT,"˰    ��:%9.2lf\n",&rmx.dSfmx[2]);
             AddDialog(pdmx,NULL,OUTPUT,"ʵ��˰��:%9.2lf\n",&rmx.dSfmx[3]);
	     break;
	case 333:
	     AddDialog(pdmx,NULL,OUTPUT,"���ֱ���:%9s\n",&rmx.cBl1);
             AddDialog(pdmx,NULL,OUTPUT,"��    ��:%9ld\n",&rmx.nBl1);
             AddDialog(pdmx,NULL,OUTPUT,"ҵ �� Ա:%9s\n",&rmx.cBl2);
             AddDialog(pdmx,NULL,OUTPUT,"���ѷ�ʽ:%9s\n",&rmx.cBl3);
             AddDialog(pdmx,NULL,OUTPUT,"����״̬:%9s\n",&rmx.cBl4);
             AddDialog(pdmx,NULL,OUTPUT,"�����ܿ�:%9.2lf\n",&rmx.dSfmx[0]);
	     break;
     }
}


/*--------------------------------------------------------------
������Stdpay_Show
���ܣ���ʾ�û�Ӧ�ɷ���Ϣ��������Ľ��׶�
���أ�KEY_ESC:��ʾ�����ɷѣ�KEY_RETURN:��ʾȷ�Ͻɷ�
˵����[��ģ����Ҫ����Ա�޸�]
*--------------------------------------------------------------*/
long Stdpay_Show()
{
	long i=0,nwhere=1;
	long nRet=0;
	long ncurrent=1,nKey=1,nYear=0,nMonth=0,iLoop=0;//�ɿ��·���
	char sKey[3],sOpt[3],sDate[121];
	char sTemp[10],sRet[7];
	double dCal,dJsje=0;
	long nCs=0,nBh=0;
	char sSjh[13]=".",sSjh1[13]=".";
	char sFp[13],sFp1[13];
	//������
   
    //������������õ�����
	sTemp[0]='\0';
	sRet[0]='\0';
	memset(sDate,'\0',sizeof(sDate));
	dSjk=dYjzk;
	alltrim(rYwmx[0].cYhbz);
	alltrim(rYwmx[0].cYhmc);
	clearline(23);
	if(nModyhmc==0)
	{
		gotoxy(2,5);
		outtext("�û����ƣ�%s",rYwmx[0].cYhmc);
	}

	if(nModyhmc==1)
	{
		rYwmx[0].cYhmc[0]='\0';
		RunSelect("select cyhmc from dl$zwmx where cjym=%s and cyhbz=%s into %s",
					sJym,rYwmx[0].cYhbz,rYwmx[0].cYhmc);
 		while(1)
 		{
 		  if(GetValue(2,6,"�û����ƣ�%9s",rYwmx[0].cYhmc)!=KEY_RETURN) return -1;
		  alltrim(rYwmx[0].cYhmc);
		  if(rYwmx[0].cYhmc[0]!='\0') break;
		  gotoxy(6,22); outtext("�û����Ʋ���Ϊ��");
		}
		clearline(22);
	}
    
	switch(nCxms)
	{
		case 111:
			ncurrent=0; 
			nJls=1;
			rYwmx[0].dYjzk=0.00;
			strcpy(rYwmx[0].cYhbz,sYhbz1);
			strcpy(sOpt,"+");
			ImportData(pdt,ncurrent+1,"%s%s%d%d%f",sOpt,
						rYwmx[0].cYhbz,nSysLkrq/10000,(nSysLkrq%10000)/100,rYwmx[0].dYjzk);


		case 230:         
		case 210:
			dSjk=0.00;
			rmx=rYwmx[0];
			sDate[0]='\0';
			RefreshDialog(pdmx,0);
			nJkyfs=6;
			nwhere=2;           
			for(;;) 
			{
				if(FetchTable(pdt->pTable,7,nwhere,"> %6d > >",
								&nJkyfs)==KEY_ESC) return -1;
				if(nJkyfs>0&&nJkyfs<99) break;
			}
			//dSjk=nJkyfs*rYwmx[0].dSfmx[0];
			/*if(nJkyfs>=6)
			dSjk-=(5*nJkyfs);*/
			if(!strcmp(rYwmx[0].cBl1,"127")) nBh=1;
			else if(strcmp(rYwmx[0].cBl1,"96167")) nBh=2;
			else nBh=3;
			RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=%ld into %ld",nBh,&nCs);
			if(nCs!=0)
			{
				if(nJkyfs==nCs) 
					nJkyfs=12;
				else 
					if(nJkyfs>nCs) 
						nJkyfs=(nJkyfs/nCs)*12+(nJkyfs%nCs);
			}//ZY_POST
			   
			dSjk=nJkyfs*rYwmx[0].dSfmx[0];
				
			FillTable(pdt->pTable,7,"> %8d > %10.2f",nJkyfs,dSjk); 
			break;

			//case 821:
			case 333:
			gotoxy(2,6);outtext("��������:%s",rYwmx[0].cYhbz1);
		default:
			dYjzk=0.0;
			strcpy(sOpt,"+");
			for(ncurrent=0;ncurrent<nJls;ncurrent++)
			{
				ImportData(pdt,ncurrent+1,"%s%s%d%d%f",sOpt,
						rYwmx[ncurrent].cYhbz,rYwmx[ncurrent].nYsnd,
						rYwmx[ncurrent].nYsyf,rYwmx[ncurrent].dYjzk);
				dYjzk+=rYwmx[ncurrent].dYjzk;
				if(nSelect==1) 
					strcpy(sOpt,"-");
			}
			dSjk=dYjzk;
       		
			FillTable(pdt->pTable,7,"> %10.2f > %10.2f",dYjzk,dSjk); 
    
			clearline(23);
			gotoxy(10,23);
    
			if(nJls>1)
			{
				if(nSelect==0)
					outtext("�����¹����ɲ鿴������ϸ,����'1'ȷ�Ͻɷ�...        ");
				if(nSelect==1)
					outtext("����'+'��ʾ�ɸ��µķ���,'-'��ʾ����,��'1'ȷ�Ͻɷ�...");
			}
			else 
				if(nJls==1)
				{
					if(nSelect==0)
						outtext("�밴��'1'ȷ�Ͻɷ�...");
					else 
						if(nSelect==1)
							outtext("��'1'��ʾȷ�ϱ��½ɷ�...                 ");
				}
				else 
					if(nJls==0)
					{
						outtext("���û�������Ƿ���·�!                     ");
						keyb();
						return -1;
					}
			ncurrent=1;
			for(;;)
			{
				nwhere=1;
				rmx=rYwmx[ncurrent-1];
				RefreshDialog(pdmx,0);
				if(nModyhmc==0)
				{
					gotoxy(2,5);
					outtext("�û����ƣ�%s",rmx.cYhmc);
				}
				dYjzk=0.0;
				for(nKey=0;nKey<nJls;nKey++)
				{
					ExportData(pdt,nKey+1,"%s > > > %f",sOpt,&dCal);
					if(sOpt[0]=='+') 
						dYjzk+=dCal;
				}
				dSjk=dYjzk;
				FillTable(pdt->pTable,7,"> %10.2f > %10.2f",dYjzk,dSjk);
				nKey=FetchData(pdt,ncurrent,nwhere,"%1s > > > >");
				ExportData(pdt,ncurrent,"%s",sKey);
				if(sKey[0]=='-')
				{
					if(nSelect!=1) 
						ImportData(pdt,ncurrent,"%s","+");
				}
				else
				{
					ImportData(pdt,ncurrent,"%s","+");
					if(sKey[0]=='1')  
						break;
				}
				if(nKey==KEY_UP)  
					if(ncurrent>1) ncurrent--;
				if(nKey==KEY_DOWN) 
					if(ncurrent<pdt->nCount) ncurrent++;
				if(nKey==KEY_ESC) 
					return -1;
                
			}//end for()	
			nJls=pdt->nCount;
			sDate[0]='\0';
			nJkyfs=0;
			if(nSelect==1)
			for(ncurrent=1;ncurrent<=pdt->nCount;ncurrent++)
			{
				ExportData(pdt,ncurrent,"%s > %d %d",sOpt,&nYear,&nMonth);
				if(sOpt[0]=='+')
				{
					nJkyfs++;
					sprintf(sTemp,"%ld|%ld|",nYear,nMonth);
					strcat(sDate,sTemp);
					memset(sTemp,0,sizeof(sTemp));
					ExportData(pdt,ncurrent,"> > > > %f",&dSjk);//���� for power
					FillTable(pdt->pTable,7,"> %10.2f > %10.2f",dSjk,dSjk);//���� for power
					break; //����ѡ���Խ������νɷ�
				}
			}
			////////////////����С��ģ˰//////////////////////////
			if((nCxms==521) ||(nCxms==522))   //&&(dSjk==0))
			{
				for(nKey=0;nKey<nJls;nKey++)
				{
					if(strcmp(rYwmx[nKey].cBl4,"3")) continue;
					gotoxy(1,22);outtext("%s",rYwmx[nKey].cBl2);
					if(GetValue(12,22,"�������˰���:%-12.2lf",&dJsje)==KEY_ESC) 
						return -1;
					dSjk=dJsje*rYwmx[nKey].dSfmx[2]/100;
					clearline(22);
					if(RunSql("update dl$ywmx_%t set dsfmx2=%lf,dsfmx5=%lf where clsh=%s and cbl5=%s",sJym,dJsje,dSjk,sLsh,rYwmx[nKey].cBl2)>0)
						CommitWork();
					if(nKey>0) dYjzk+=dSjk;
					else dYjzk=dSjk;
				}
				dSjk=dYjzk;
				FillTable(pdt->pTable,7,"> %10.2f > %10.2f",dSjk,dSjk); 
			}//���� for ls
			break;
	}//end switch
    
	clearline(23);
	gotoxy(6,23);
	if(nModify==1)
	{
		outtext("�����Ǹ��û�Ӧ�ɿ��,�޸Ĵ˽��,������ΪԤ���...");
		nwhere=4;
		for(;;) 
		{ 
			dSjk=dYjzk;
			if(FetchTable(pdt->pTable,7,nwhere,"> > > %10.2f",&dSjk)==KEY_ESC) 
				return -1;
			if(dSjk>=dYjzk && dSjk!=0.0) break;
		}
	}
	
	clearline2(22,23);
	gotoxy(1,22);
	if(strstr(sSysJh,"2715") )
	outtext("��ѡ�񸶿ʽ:x.�ֽ�ɷ�  l.�̿��ɷ�  z.ת��֧Ʊ  f.����:");
	else 
	outtext("��ѡ�񸶿ʽ:1.�ֽ�ɷ�  2.�̿��ɷ�  3.ת��֧Ʊ  4.����:");
	nKey=0;
	if(strstr(sSysJh,"2715") )
	{
		while(1)
		{
			nKey=keyb();
			if(nKey=='x' || nKey=='l' || nKey=='z' || nKey=='f') break;
		}
		if(nKey=='f') return -1;
		else if(nKey=='x') nKey=1;
		else if(nKey=='z') nKey=2;
		else nKey=3;
	}
	else
	{
		while(nKey<1 || nKey>4) 
			nKey=keyb()-'0'; 
		if(nKey==4) return -1;
	}
   
	//���ݺŵ����뼰��Ҫ��������
	switch(nCxms)
	{
		case 123:
		case 124:
		case 222:
		case 223:
		case 333:
			clearline(22);
			for(i=1;i<3;i++)
			{
				if(GetValue(1,22,"�������վ�ӡˢ��:%-12s",sSjh)==KEY_ESC)
					return -1;
				if(GetValue(1,23,"���������ӡˢ��:%-12s",sSjh1)==KEY_ESC)
					return -1;
				if(!strcmp(sSjh,sSjh1)) break;
				alltrim(sSjh);
				if(strlen(sSjh)!=7) continue;
			}//end for
			if(strcmp(sSjh,sSjh1) && i>=3)
				return -1;
			else clearline2(22,23);
			break;
		
		case 521:
		case 522:
			clearline(22);
			gotoxy(1,22);
			if(dSjk!=0) {outtext("����˰����Ҫ��%ld����˰֤������һ������....",pdt->nCount);keyb();}
			clearline(22);
			
			memset(sPacket,'\0',sizeof(sPacket));
			sprintf(sPacket,"%-40.40s|%s|%s|%ld|%.2lf|%s|%ld|%ld|",
							sLsh,rYwmx[0].cYhbz,rYwmx[0].cYhmc,
							nKey,dSjk,sSysCzydh,nSysSfm,pdt->nCount);
			if(dSjk==0 && (strstr(sSysJh,"2704") || strstr(sSysJh,"2706")) )
			{
				strcat(sPacket,"9|");
				gotoxy(1,23);
				outtext("���ڽɷ�,���Ժ�......");
				return 1;
			}//�˱�
			for(i=1;i<=pdt->nCount;i++)
			{
				for(;;)
				{
					sFp[0]='\0';
					sFp1[0]='\0';
					if(GetValue(1,22,"��������˰֤Ʊ��:%-8.8s",sFp)==KEY_ESC) return -1;
					for(nKey=0;nKey<=7;nKey++)
						if(sFp[nKey]<'0' || sFp[nKey]>'9' ) 
							break;
					if(nKey<7) continue;
					
					nKey=GetValue(1,23,"��������˰֤Ʊ��:%-8.8s",sFp1);
					if(nKey==KEY_ESC) return -1;
					if(strcmp(sFp,sFp1)) continue;
					clearline2(22,23);
					break;
				}
				strcat(sPacket,sFp);
				strcat(sPacket,"|");
				
				if(pdt->nCount==1) break;
				else
				{
					gotoxy(1,23);
					outtext("��%ld����˰֤����ɹ�������һ������....",i);keyb();
					clearline(23);
				}//end else
			}
			gotoxy(1,23);
			outtext("���ڽɷ�,���Ժ�......");
			return 1;
		default: break;
	}//end switch

	gotoxy(1,23); outtext("���ڽɷ�,���Ժ�......"); GetSysLkrq(); 
	alltrim(sSjh);
	
	if(!strcmp(rYwmx[0].cYhbz,"...........") )
			strcpy(rYwmx[0].cYhbz,sYhbz2);
	
	sprintf(sPacket,"%-40.40s|%s|%s|%ld|%.2lf|%s|%ld|%ld|%s|%s|",
					sLsh,rYwmx[0].cYhbz,rYwmx[0].cYhmc,
					nKey,dSjk,sSysCzydh,nSysSfm,nJkyfs,sDate,sSjh);
	rYwmx[0].cYhmc[0]='\0';
	return 1;
}

/*--------------------------------------------------------------
������InitDlywPay
���ܣ������շѵĳ�ʼ��
���أ�����0�ɹ�
˵�������ݲ�ͬ�ĳ���ģʽ������Ӧ��������
      �������ѡ���·ݽ��ѣ��뽫nSelect��1
      ����������Ա�޸��շѽ��뽫nModify��1
*--------------------------------------------------------------*/
long InitDlywPay()
{
    char sTemp[57];
    
    RunSelect("select ncxms,csjydz,cJymc from dl$jym where cjym=%s into %ld%s%s",
               sJym,&nCxms,sRemoteHost,sJymc);
    if(!(pd=CreateDialog(56,23,NORMAL))) return -1;
    if(!(pdmx=CreateDialog(22,19,FRAME)))
    {
    	DropDialog(pd);
    	return -1;
    }
    pdt=CreateDataTable(5,"|  |�û���ʶ              |���|�·�| Ӧ�ɿ�� |",
                         "%-1.1s%-22.22s%4d%02d%10.2f");
    if(pdt==NULL)
    {
    	DropDialog(pd);
    	DropDialog(pdmx);
    	return -2;
    }
    switch(nCxms)
    {
       case 230:
       case 220:
       case 210:
             if(MergeTable(pdt->pTable,CreateTable(1,
                          "|�����·���|            |�û�ʵ�ɿ�|            |"))<0)
             {
               DropDialog(pd); 
    	       DropDataTable(pdt);
    	       DropDialog(pdmx);
               return -2;
             }
             break;
       default:
             if(MergeTable(pdt->pTable,CreateTable(1,
                          "|�ۼ�Ӧ�ɿ�|            |�û�ʵ�ɿ�|            |"))<0)
             {
    	       DropDialog(pd); 
    	       DropDataTable(pdt);
    	       DropDialog(pdmx);
               return -2;
             }
             break; 
    }
    memset(sTemp,' ',56);
    strcpy(sTemp+(56-strlen(sJymc))/2,sJymc);
    AddDialog(pd,NULL,STATIC_TEXT,sTemp);
    AddDialog(pd,NULL,SPACELINE,1);
    AddDialog(pd,NULL,STATIC_TEXT,"              ---------------------------- ");

    //���ݲ�ͬ�ĳ���ģʽ���Ӳ�ͬ������/����������3�У�
    //���������Ӧ�ı���������ȫ�ֵ�
    AddPayDetailDialog();

    ShowDialog(pd,1,1);
    ShowDialog(pdmx,55,2);
    ShowDataTable(pdt,1,7);
    return 1;
}

void EndDlywPay()
{
    DropDataTable(pdt);
    DropDialog(pd);
    if(nDsyc==0)
    DropDialog(pdmx);
}

void PayPrint(long nType)
{
  char Yn[2]=".";
  clearline(23);
  gotoxy(10,23);
  outtext("����ȡ��ӡ����,���Ժ�......");
  switch(nCxms)
  {
      case 110:
          PrintLtdsfp(sLsh);
          break;
      case 111:
          PrintLtdssj(sLsh);
          break;
      case 130:
          if(RunSelect("SELECT * from dl$ywmx_%t where \
                        clsh=%s and dyjzk>0.10",sJym,sLsh)>0)
                 PrintYdfp(sLsh);
          else   PrintYdsj(sLsh);
          break;

      case 210:
      case 220:
      case 230: 
           PrintXhfp(sLsh);
           break;
      case 140:
           if(strstr(sSysJh,"2707")||strstr(sSysJh,"2714")||strstr(sSysJh,"2717")||strstr(sSysJh,"2704"))
           	  PrintGddhfp(sLsh);
           else if(strstr(sSysJh,"2721"))
           	  PrintPzhGddhfp(sJym,sLsh);
	   else PrintMyGddhfp(sJym,sLsh);  //new struct
           break;
      case 141:
           PrintYaGddhfp(sLsh);
	   break;
      case 610:
          PrintTrqfp(sLsh);
          break;
      case 821:
          PrintBxfp(sLsh);
          break;
      case 123:
          PrintDffp(sLsh);
          break;
      case 124:
       	  PrintzwDffpL(sLsh);
       	  break;
      case 125:
      	 if(GetValue(10,22,"ѡ'Y'��ӡ��Ʊ��������վ�?[Y/N]%1s",Yn)==KEY_ESC) break;
	 alltrim(Yn); upper(Yn);
         if(!strcmp(Yn,"Y"))
           PrintPxDffp(sLsh);
         else PrintPxDfsj(sLsh);
      	 break;
      case 521:
      case 522:
          if(strstr(sSysJh,"2719") || strstr(sSysJh,"2716") )
	  {
	  	if(GetValue(10,22,"�Ƿ��ӡͨ�ýɿ���?[Y/N]%1s",Yn)==KEY_ESC) break;
	  	alltrim(Yn);
	  	upper(Yn);
	  	if(!strcmp(Yn,"Y")) PrintTaxJkfp(sLsh);
	  	else PrintTaxfp(sLsh);
	  	break;
	  }
      	  //if(dSjk!=0)
      	  PrintTaxfp(sLsh);
      	  break;
      case 222:
	  if(strstr(sSysJh,"2715"))
	       PrintMsTerrafp(sLsh,nType);
	  else PrintTerrafp(sLsh);
	  break;
      case 333:
	  PrintTpyfp(sLsh,nType);
      	  break;
      case 223:
	  if(strstr(sSysJsdh,"271502"))
	  	PrintRsTerrafp(sLsh,nType);
	  else if(strstr(sSysJsdh,"271505"))
	  	PrintDlTerrafp(sLsh,nType);
	  else if(strstr(sSysJh,"2715"))
	  	PrintHyTerrafp(sLsh,nType);
	  else PrintDyTerrafp(sLsh,nType);
	  break;
      default:
          break;
  }
}

void PrintYdfp(char * sLsh)  //�ƶ���Ʊ��ӡ
{
     long nRetVal,nCount,nprint;
     char sResult[TOTAL_PACKAGE_SIZE],sPage[2048];
     char * offset;
     
     nRetVal = AutoRunTask("dldj",TASK_REQUEST_PRINT,"%s","%s",sLsh,sResult);
     
     if(nRetVal)
     {
         gotoxy(14,22);
         outtext(sTcpipErrInfo);
         keyb();
         return;
     }

     if(strncmp(sResult,"000000",6))
     {
         PrintYdsj(sLsh);
         return;
     }
     
     offset = FetchStringToArg(sResult+7,"%d|",&nCount);
     if(nCount==1 && strlen(sResult)<80) 
     {
     	gotoxy(20,21);
     	outtext("���û�û�и��µ�Ԥ������Ʊ...");
     	keyb();
     	clearline(21);
      	return;
     }
     for(nprint=1;nprint<=nCount;nprint++)
     {
         if(nprint==1)  //�վ�ֻ��һ��
         if((nBl4[nprint-1]==9) || (dSjk>dYjzk))  PrintYdsj(sLsh);  //�󸺷��û�
        
         gotoxy(14,22);
         outtext("���������ʼ��ӡ��Ʊ����%ld�ţ���%ld��,ESC������...",nCount,nprint);
         if(keyb()==KEY_ESC) return;
         //clrscr();
         memset(sPage,'\0',sizeof(sPage));
         offset = FetchStringToArg(offset,"%s|",sPage);

         setprinton();
         output(sPage);
	 eject();
         setprintoff(); 
     }
}

void PrintYdsj(char * sLsh)  //�ƶ��վݴ�ӡ
{
     PRINTFORM * ppf;

     clearline(23);
     gotoxy(10,23);
     outtext("��ӡ�ƶ��վ�,ESC����,���⿪ʹ��ӡ.....");
     if(keyb()==KEY_ESC) return;
     Clear_Space(1,23,80,23);
     if(!(ppf=CreatePrintForm("yddssj"))) return ;
     if(LoadPrintData(ppf,"from dl$zwmx where clsh=%s",sLsh)<0)
     {
        	DropPrintForm(ppf);
        	return ;
     }
     OutputPrintForm(ppf,NULL,1);
     DropPrintForm(ppf);
}

void PrintTrqfp(char * sLsh)  //��Ȼ����Ʊ��ӡ
{
     PRINTFORM * ppf;

     Clear_Space(1,23,80,23);
     if(!(ppf=CreatePrintForm("trqdsfp"))) return ;
     if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",
                       sJym,sLsh)<0)
     {
        	DropPrintForm(ppf);
        	return ;
     }
     OutputPrintForm(ppf,NULL,1);
     DropPrintForm(ppf);
}


void PrintLtddfp(char * sLsh) 
{
     PRINTFORM * ppf;

     Clear_Space(1,23,80,23);
     if(!(ppf=CreatePrintForm("110ltddfp"))) return ;
     if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s and nywlb=2",
                       sJym,sLsh)<0)
     {
        	DropPrintForm(ppf);
        	return ;
     }
     OutputPrintForm(ppf,NULL,1);
     DropPrintForm(ppf);
}

void PrintLtdsfp(char * sLsh)  //��ͨ��Ʊ��ӡ
{
     PRINTFORM * ppf;

     Clear_Space(1,23,80,23);
     if(!(ppf=CreatePrintForm("110ltdsfp"))) return ;
     if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s and nywlb=0",
                       sJym,sLsh)<0)
     {
        	DropPrintForm(ppf);
        	return ;
     }
     OutputPrintForm(ppf,NULL,1);
     DropPrintForm(ppf);
}

void PrintLtdssj(char * sLsh)   //��ͨ�վݴ�ӡ
{
     PRINTFORM * ppf;

     Clear_Space(1,23,80,23);
     if(!(ppf=CreatePrintForm("111dsltsj"))) return ;
     if(LoadPrintData(ppf,"from dl$zwmx where clsh=%s",sLsh)<0)
     {
        	DropPrintForm(ppf);
        	return ;
     }
     OutputPrintForm(ppf,NULL,1);
     DropPrintForm(ppf);
}

void PrintXhfp(char * sLsh) //Ѱ����Ʊ��ӡ
{
     PRINTFORM * ppf;

     clearline(23);
     if(!(ppf=CreatePrintForm("xhdsfp"))) return ;
     if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",
                       sJym,sLsh)<0)
     {
        	DropPrintForm(ppf);
        	return ;
     }
     OutputPrintForm(ppf,NULL,1);
     DropPrintForm(ppf);
}

void PrintBxfp(char * sLsh) //���շ�Ʊ��ӡ
{
     PRINTFORM * ppf;
     char sBdh[30]=".";
     char temp[5],temp1[7],temp2[4],temp3[9],temp4[2],sQq[12],sZq[12];
     char sDate[30]=".",sBz[4]=".",sJt[152]="\0";
     char sTemp[10]=".",sBuffer[800]="\0",sYhmc[40],sYhmc1[100],space[12]="   ";
     long nLkrq,nId,nYsnd;
     double dsjk;

     clearline(23);
     RunSelect("select cyhbz,cyhbz2,nlkrq,cbl7,nysnd,cbl4,cbl5,dsjk from dl$ywmx_%t where clsh=%s \
     		and substr(cyhbz1,11,1)!='F' into %s%s%ld%s,%ld,%s%s%lf",
                sJym,sLsh,sBdh,sYhmc,&nLkrq,sBz,&nYsnd,sQq,sZq,&dsjk);
     FetchStringToArg(sBdh,"%4s%6s%3s%8s%1s",temp,temp1,temp2,temp3,temp4);
     sprintf(sBdh,"%s-%s-%s-%s-%s",temp,temp1,temp2,temp3,temp4);
     
     sBuffer[0]='\0';
     sprintf(sBuffer,"��������%20s%ld%s%s��%s%10s%.2lf\n",space,nYsnd,space,sQq,sZq,space,dsjk);//����
     
     alltrim(sBz);
     if(!strcmp(sBz,"JT"))
	 sprintf(sJt,"��ҵ��Ϊ�й����ٱ��գ����ţ���˾��ԭ�й����ٱ��չ�˾��ҵ��,�й����ٱ��չɷ����� \n��˾��Ϊ������˴�Ϊ�����ɴ����������ķ����������й����ٱ��գ����ţ���˾�е���");
     else strcpy(sJt,"\n\n");

     nId=OpenCursor("select cyhbz1,cbl4,cbl5,dsjk,nysnd from dl$ywmx_%t where clsh=%s and substr(cyhbz1,11,1)='F' order by dsjk",sJym,sLsh);
     if(nId<0) return;
     while(1)
     {
     	if(FetchCursor(nId,"%s%s%s%lf%ld",sDate,sQq,sZq,&dsjk,&nYsnd)<=0) break;
     	FetchStringToArg(sDate,"%4s%6s%3s%8s%1s",temp,temp1,temp2,temp3,temp4);	
		sprintf(sDate,"%s-%s-%s-%s-%s  ",temp,temp1,temp2,temp3,temp4);
		memset(sYhmc1,'\0',strlen(sYhmc1));
		sprintf(sYhmc1,"%s%s%ld%s%s��%s%10s%.2lf", 
			sDate,space,nYsnd,space,sQq,sZq,space,dsjk);
		strcat(sBuffer,sYhmc1);
		strcat(sBuffer,"\n");
     }

     if(!(ppf=CreatePrintForm("bxdsffp"))) return ;
     if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",
                       sJym,sLsh)<0)
     {
        	DropPrintForm(ppf);
        	return ;
     }
          
     OutputPrintForm(ppf,NULL,1,sJt,(nLkrq%10000)%100,sBdh,sYhmc,sBuffer);
     DropPrintForm(ppf);
}

void PrintGddhfp(char * sLsh) //�̶��绰��Ʊ��ӡ
{
     PRINTFORM * ppf;
     long nprint;
     long nXh;
     
     clearline(23);
     if(!(ppf=CreatePrintForm("gddhdsfp"))) return ;
     if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",
                       sJym,sLsh)<0)
     {
        	DropPrintForm(ppf);
        	return ;
     }
     for(nprint=1;nprint<=ppf->nCount;nprint++)
     {
            gotoxy(14,22);
            outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,ppf->nCount);
            if(keyb()==KEY_ESC)
            {
            	DropPrintForm(ppf);
            	return ;
            }
			
			RunSelect("select nxh from dl$ywmx_%t where clsh=%s and njscz2=%ld into %ld",
					sJym,sLsh,nprint,&nXh);
			
			if(RunSelect("select * from dl$dxfp where nxh=%ld and nfpzs=%ld",nXh,nprint)>0 && ppf->nCount>1)
			   PrintGa(nXh,nprint);
			else
			OutputPrintForm(ppf,NULL,nprint);
     }
     DropPrintForm(ppf);
}

void PrintTaxfp(char * sLsh)  //��˰��Ʊ��ӡ
{
	PRINTFORM * ppf;
	long nprint;
	long nYsnd,nYsyf,nLd;
	long nKey,nXjrq;
	double dSjk;
     
	char sRqfw[10];
	
	Clear_Space(1,23,80,23);
     
	RunSelect("select nysnd,nysyf,dsjk from dl$ywmx_%t "
				"where clsh=%s into %ld,%ld,%lf",
				sJym,sLsh,&nYsnd,&nYsyf,&dSjk);
     
     		
	if(!(ppf=CreatePrintForm("zswsz"))) return ;
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",
							sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	for(nprint=1;nprint<=ppf->nCount;nprint++)
	{
		if(!strcmp(rYwmx[nprint-1].cBl4,"3") && dSjk==0) continue;
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,ppf->nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
          
		sprintf(sRqfw,"1-%ld",LastDay(nYsnd,nYsyf)%100);
     
		OutputPrintForm(ppf,NULL,nprint,
						(nSysLkrq/10000),(nSysLkrq%10000)/100,(nSysLkrq%10000)%100,sRqfw,sSysCzymc);
	
	}
	DropPrintForm(ppf);
}

void PrintTaxJkfp(char * sLsh)  //��˰��Ʊ��ӡ
{
	PRINTFORM * ppf;
        long nprint;
        long nYsnd,nYsyf,nLd;
        long nKey,nXjrq;
        double dSjk;
        char sJkdw[31],sJkgk[31],sZh[31],sKhh[31],sJc[31];

        char sRqfw[10];

        Clear_Space(1,23,80,23);

        RunSelect("select nysnd,nysyf,dsjk from dl$ywmx_%t "
                                "where clsh=%s into %ld,%ld,%lf",
                                sJym,sLsh,&nYsnd,&nYsyf,&dSjk);
        RunSelect("select ckhh,cskgk,czh,cjkdw from dl$khhgl where cjsdh=%s into %s,%s,%s,%s ",
        		sSysJsdh,sKhh,sJkgk,sZh,sJkdw);
        RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=3 into %s ",sJym,sJc);

	while(1)
	{
		sZh[0]='\0';
		if(GetValue(14,23,"�����������ʺ�:%-20s",sZh)==KEY_ESC) return;
		alltrim(sZh);
		if(strlen(sZh)<3){
		  gotoxy(14,23);
		  outtext("�ʺŲ���Ϊ��");
		  keyb();
		  clearline(23);
		  continue;
		}
		break;
	} //end while
        if(!(ppf=CreatePrintForm("sstyjks"))) return ;
	
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	for(nprint=1;nprint<=ppf->nCount;nprint++)
	{
		clearline(22);
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,ppf->nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
          
		OutputPrintForm(ppf,NULL,nprint,
			(nSysLkrq/10000),(nSysLkrq%10000)/100,(nSysLkrq%10000)%100,sJc,sZh,sSysCzymc);
	
		RunSql("update dl$zwmx set dyjzk=0.0 where cjym=%s and clsh=%s ",sJym,sLsh);
		if(RunSql("update dl$ywmx_%t set cbl17=%s,cbz='ת��',nxzbz=4 where clsh=%s ",sJym,sZh,sLsh)>0)
			CommitWork();
	}
	DropPrintForm(ppf);
}

void PrintDffp(char * sLsh)  //��ѷ�Ʊ��ӡ
{
     long nRetVal,nCount,nprint,i;
     char sResult[TOTAL_PACKAGE_SIZE],sPage[2048];
     char * offset;
     char jfrq[9],jh[8],yhqh[8],hbh1[6]=".",pbbh[18],yhmc[31],hhhh[15],yhdz[31],gddy[60],hbh[6];
     char ydlb[12],zds_new[12],zds_old[12],jlbl[12],sydl[12],sh[12],dljj[12],hjdl[12],djjj[12],dfje[12];
     char qt_title[12],qt_data[12],jjlb[12],jjdl[12],jjdj[12],jjje[12],jjlb1[12],jjdl1[12],jjdj1[12],jjje1[12];
     char zdlll[12],cbry[12],shry[12],sfrq[12],sum_chinese[50],sum_real[12],ctemp[3];
     char sTemp[12];

     nRetVal = AutoRunTask("dldj",TASK_REQUEST_PRINT,"%s","%s",sLsh,sResult);
     if(nRetVal)
     {
         gotoxy(14,22);
         outtext(sTcpipErrInfo);
	 keyb();
         return;
     }

     if(strncmp(sResult,"000000",6))
     {
         outtext("�޴�ӡ����!");
         return;
     }

     offset = FetchStringToArg(sResult+7,"%d|",&nCount);

     if(nCount<1)
        return;

     for(nprint=1;nprint<=nCount;nprint++)
     {
         gotoxy(14,22);
         outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,nCount);
         if(keyb()==KEY_ESC) return;
	
	 if(offset==NULL) break;
	 setprinton();
         output("\n\n\n");
         //offset=FetchStringToArg(offset,"%s\n",sPage);
         
         offset=FetchStringToArg(offset,"%s|%s|%s|%s|%s|%s|%s|",sTemp,sTemp,jfrq,jh,yhqh,hbh1,pbbh);
         output("                %8s                    %-8.8s\n",jfrq,yhqh);
         offset=FetchStringToArg(offset,"%s|%s|%s|%s|%s|",yhmc,hhhh,yhdz,gddy,hbh);
         output("                       %-30.30s%-14.14s  %-30.30s%s    %-5.5s\n\n\n",
                                yhmc,hhhh,yhdz,gddy,hbh);
         for(i=0;i<7;i++)
         {
                offset=FetchStringToArg(offset,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
                                	        ydlb,zds_new,zds_old,jlbl,sydl,sh,dljj,hjdl,djjj,dfje);
                output("             %-10.10s %-10.10s%-10.10s%-10.10s%-10.10s %-8.8s%-10.10s%-10.10s%-8.8s  %-12.12s\n",
                                ydlb,zds_new,zds_old,jlbl,sydl,sh,dljj,hjdl,djjj,dfje);
         }
 	 output("\n\n\n\n");
         for(i=0;i<7;i++)
         {
                offset=FetchStringToArg(offset,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
                                qt_title,qt_data,jjlb,jjdl,jjdj,jjje,jjlb1,jjdl1,jjdj1,jjje1);
                output("             %-10.10s %-10.10s%-10.10s%-10.10s%-8.8s  %-10.10s%-10.10s%-10.10s%-8.8s%-12.12s\n",
                                qt_title,qt_data,jjlb,jjdl,jjdj,jjje,jjlb1,jjdl1,jjdj1,jjje1);
         }
         output("\n");
         offset=FetchStringToArg(offset,"%s|%s|%s|%s|%s|",sum_chinese,sum_real,zdlll,cbry,shry);
         output("              %-70.70s\n\n",zdlll);
         output("                        %-70.70s        %-12.12s\n",sum_chinese,sum_real);
         output("                                       %-10.10s             %-10.10s    %14.14s          %8ld\n",cbry,shry,sSysCzymc,nSysLkrq);

         eject();
         setprintoff();
         //offset++;
     }//END FOR
	
}

//�ɷѳɹ���ĸ��Ӳ���
void AdditionProcess()
{
     long nRetVal;
     
     switch(nCxms)
     {
       case 220:
       case 210:
       case 230:
          AutoRunTask("dldj",TASK_BPUSER_TEST_CALL,"%s%s","%ld",sJym,sYhbz1,&nRetVal);
          break;
     }
}

static void PrintGa(long nXh,long nprint)
{

    long nId,nRetVal;
    char sFpmc[45][22];
    double dFpje[45];
    long nCol,nCount=0,nRq1,nRq2;
    char sBuffer[1024],space[30],sTemp[360],sRow[30];
    char cYhbz[21],cYhbz1[21],cYhmc[41],sZk[31]=".";
    double dZk=0;
     
    for(nCount=0;nCount<45;nCount++) dFpje[nCount]=0;
    RunSelect("select nxh,dSfmx21,dsfmx22,cyhbz,cyhbz1,cyhmc,dsjk from dl$ywmx_%t "
    		          "where clsh=%s and njscz2=%ld into %ld%lf%lf%s%s%s%lf",
               sJym,sLsh,nprint,&nXh,&dFpje[37],&dFpje[38],cYhbz,cYhbz1,cYhmc,&dZk);
	nCount=0;
	memset(sBuffer,'\0',sizeof(sBuffer));
	memset(space,'\0',30);

	strcpy(sFpmc[37],"�ϴ���Ǯ");
	strcpy(sFpmc[38],"������Ǯ");
	strcpy(space,"    "); //�м�ո���
	     
	sprintf(sBuffer,"\n\n\n\n%66.66s%10ld\n%70.70s",space,nSysLkrq,space,nRq1,nRq2);	
	sprintf(sTemp,"%-20.20s\n%s%-50.50s%20.20s\n\n",
		cYhbz1,space,cYhmc,cYhbz);
	strcat(sBuffer,sTemp);
	     
	RunSelect("select sum(dfpje) from dl$dxfp where nxh=%ld and nfpzs=%ld into %lf",nXh,nprint,&dZk);
	
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
	strcpy(sRow,"  ");    //��߽�ո���
	for(nCol=0;nCol<9;nCol++)
	{
	    alltrim(sFpmc[nCol]);
	    if(dFpje[nCol]==0 && nCol==nCount) break;
	
		if(dFpje[nCol]!=0)
	         sprintf(sTemp,"\n%-14.14s%-20.20s%-10.2lf",
	                sRow,sFpmc[nCol],dFpje[nCol]);
        else continue;

        strcat(sBuffer,sTemp);
     }
	 
     for(;nCol<9;nCol++) strcat(sBuffer,"\n");
     sprintf(sTemp,"\n%-60.60s%-20.20s%-10.2lf",sRow,sFpmc[37],dFpje[37]);
     strcat(sBuffer,sTemp);
     sprintf(sTemp,"\n%-60.60s%-20.20s%-10.2lf",sRow,sFpmc[38],dFpje[38]);
     strcat(sBuffer,sTemp);
     memset(sTemp,'\0',strlen(sTemp));

     to_Chinese(dZk,sZk);
     sprintf(sTemp,"\n%40.40s%30.30s��%-12.2lf",sZk,sRow,dZk);
     strcat(sBuffer,sTemp);
	 memset(sTemp,'\0',strlen(sTemp));
     sprintf(sTemp,"\n\n%-14.14s%-8.8s%-24.24s%20.20s������\n",
                  space,sSysCzymc,space,sSysJm);
     strcat(sBuffer,sTemp);
       
     setprinton();
     output(sBuffer);
	 eject();
     setprintoff();

	 return;
}

void PrintYaGddhfp(char * sLsh)  //��ѷ�Ʊ��ӡ
{
     long nRetVal,nCount,nprint,i,nLkrq,nYear,year,month,nXh;
     char sResult[TOTAL_PACKAGE_SIZE],sPage[2048];
     char * offset,* p,buf[2048];
     char sTemp[1024],temp[100];
     char sYhbz[13],sHdh[13],sYhmc[41],sZk[40]=".";
     char sHm[30],space[6]=" ";
     double dJe=0,dZje,dZnj;
     
     RunSelect("select nxh from dl$zwmx where clsh=%s into %ld",sLsh,&nXh);
     nRetVal = AutoRunTask("dldj",TASK_REQUEST_PRINT,"%s","%s",sLsh,sResult);
     if(nRetVal)
     {
         gotoxy(14,22);
         outtext(sTcpipErrInfo);
         keyb();
         return;
     }

     if(strncmp(sResult,"000000",6))
     {
         outtext("�޴�ӡ����!");
         return;
     }

     offset = FetchStringToArg(sResult+7,"%d|",&nCount);
     if(nCount<1)
        return;

     for(nprint=1;nprint<=nCount;nprint++)
     {
		memset(sPage,0,strlen(sPage));
		memset(buf,'\0',strlen(buf));
	    	gotoxy(14,22);
	   	outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,nCount);
	    	if(keyb()==KEY_ESC) return;
	    	offset = FetchStringToArg(offset,"%s\n",sPage);
	
		p=FetchStringToArg(sPage,"%s|%s|%s|",sYhbz,sHdh,sYhmc);
		memset(sTemp,0,1024);
		for(i=1;i<15;i++)
		{
			memset(temp,'\0',100);
			p=FetchStringToArg(p,"%s|",temp);
			alltrim(temp);
			if(strlen(temp)<2) break;
			FetchStringToArg(temp,"%s,%f",sHm,&dJe);
			sprintf(temp,"\n%-6.6s%-20.20s%.2lf",space,sHm,dJe);
			strcat(sTemp,temp);
		}
		 
		while(1)
		{
			p=FetchStringToArg(p,"%s|",temp);
			alltrim(temp);
			if(temp[0]=='\0') continue;	
			break;
		}
		FetchStringToArg(p,"%d|%f|%f|%f|",&nLkrq,&dJe,&dZje,&dZnj);
	
		year=nYear=nSysLkrq/10000;
		month=((nLkrq%10000)%100);
		
		if(month==12)  {year--; month--; }
		else if(month==1) {year--; month=12; }
		else month--;

		sprintf(buf,"\n\n\n\n%-10.10s%4ld-%02ld-%02ld  %02ld:%02ld:%02ld  ����:%4ld-%02ld-21��%4ld-%02ld-20",
			space,nYear,(nSysLkrq%10000)/100,
			(nSysLkrq%10000)%100,nSysSfm/10000,
			(nSysSfm%10000)/100,(nSysSfm%10000)%100,
			year,month,(nLkrq/100),(nLkrq%10000)%100);
		 sprintf(temp,"\n%-14.14s%-46.46s%20.20s",space,sYhmc,sYhbz);
		 strcat(buf,temp);
		 memset(temp,'\0',100);
		 sprintf(temp,"\n%-56.56s%20.20s\n",space,sHdh);
		 strcat(buf,temp);
		 memset(temp,'\0',100);
	     	 to_Chinese(dZje,sZk);
		 sprintf(temp,"\n\n%-15.15s%-50.50s%.2lf\n\n\n",space,sZk,dZje);
		 strcat(buf,temp);
		 strcat(buf,sTemp);
		 memset(temp,'\0',100);
		 for(;i<14;i++) strcat(buf,"\n");
		 sprintf(temp,"%-5s ��ˮ:%ld     %-30.-30s%20.20s����\n",
			space,nXh,sSysCzydh,sSysJm);
		 strcat(buf,temp);
		 
	   	 setprinton();
		 output(buf);
		 eject();
		 setprintoff();
     }

}

//��˰Ʊ
void PrintTerrafp(char * sLsh)
{
	PRINTFORM * ppf;
	long nprint,nCount,ncount;
	long nId,nRetVal;
	char sm[21],pm[28],sBuf[5][1024],sTemp[360];
	double kssl,jsje,sl,yjje,sjje;
	
	Clear_Space(1,23,80,23);
     
	if(!(ppf=CreatePrintForm("dsdswsz"))) return ;
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",
							sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	
	RunSelect("select count(distinct cbl3) from dl$ywmx_%t where clsh=%s into %ld",
			sJym,sLsh,&nCount);
	nCount=(nCount/5)+1;

	nId=OpenCursor("select cbl3,cbl10,sum(dsfmx1),sum(dsfmx2),\
			sum(dsfmx3),sum(dsfmx4),sum(dsfmx5) from \
			dl$ywmx_%t where clsh=%s group by cbl3,cbl10",sJym,sLsh);
	if(nId<=0) return;
	
	ncount=0;
	sBuf[0][0]='\0';
	while(1)
	{
		nRetVal=FetchCursor(nId,"%s%s%lf%lf%lf%lf%lf",sm,pm,&kssl,&jsje,&sl,&yjje,&sjje);
		if(nRetVal<=0) break;
		ncount++;
		sTemp[0]='\0';
		if(ncount%5==1)
		sprintf(sTemp,"   %-14s  %-20s  %.2lf  %.2lf  %8.2lf  %8.2lf  %12.2lf\n", sm,pm,kssl,jsje,sl,yjje,sjje);
		else
		sprintf(sTemp,"    %-14s  %-20s  %.2lf  %.2lf  %8.2lf  %8.2lf  %12.2lf\n", sm,pm,kssl,jsje,sl,yjje,sjje);

		nprint=(ncount-1)/5;
		strcat(sBuf[nprint],sTemp);
	}
	CloseCursor(nId);

	//printf("\nTEST:sBuf=%s\n",sBuf);
	for(nprint=1;nprint<=nCount;nprint++)
	{
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
		OutputPrintForm(ppf,NULL,nprint,
				(nSysLkrq%10000)/100,(nSysLkrq%10000)%100,sBuf[nprint-1],sSysCzymc);
	}
	DropPrintForm(ppf);
}

void PrintTpyfp(char * sLsh,long nType)  //̫ƽ���շ�Ʊ
{
	PRINTFORM * ppf;
	long nprint,nCount,ncount;
	long nId,nRetVal,nBl1;
	char sTemp[360]=".",sBuffer[1024]=".",sQq[12],sZq[12],sYhbz[40],sFph[14]=".";
	double dJe=0;
	
	Clear_Space(1,23,80,23);
     
	if(!(ppf=CreatePrintForm("tpybxfp"))) return ;
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",
							sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	
	RunSelect("select count(*) from dl$ywmx_%t where clsh=%s group by cbl1 into %ld",sJym,sLsh,&nCount);

	/*nId=OpenCursor("select cyhbz2,nbl1,substr(cbl3,1,10),substr(cbl4,1,10),dsfmx1 \
			from dl$ywmx_%t where clsh=%s and cbl7='2'",sJym,sLsh);
	if(nId<0) return;
	strcpy(sBuffer,"��������                      ����  �ɷ���ʼ����  �ɷ���ֹ����  �ɷѽ��\n");
	while(1)
	{
		nRetVal=FetchCursor(nId,"%s%ld%s%s%lf",sYhbz,&nBl1,sQq,sZq,&dJe);
		if(nRetVal<=0) break;
		sTemp[0]='\0';
		sprintf(sTemp,"               %-30s  %ld  %-16s  %-12s  %.2lf",sYhbz,nBl1,sQq,sZq,dJe);
		strcat(sBuffer,sTemp);
	}			
	CloseCursor(nId);*/
	
	for(nprint=1;nprint<=nCount;nprint++)
	{
		RunSelect("select cyhbz2,nbl1,substr(cbl3,1,10),substr(cbl4,1,10),dsfmx1 \
			from dl$ywmx_%t where clsh=%s and cbl7='2' and nyfkfs=%ld into %s%ld%s%s%lf",
			sJym,sLsh,nprint,sYhbz,&nBl1,sQq,sZq,&dJe);
		sBuffer[0]='\0';
		sprintf(sBuffer,"               %-30s  %ld  %-16s  %-12s  %.2lf",sYhbz,nBl1,sQq,sZq,dJe);
		
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
		if(nType==2)
		{
			if(GetValue(14,23,"�������µ��ݺ�:%-12s",sFph)==KEY_ESC) continue;
			alltrim(sFph);
			if(RunSql("update dl$ywmx_%t set cbl8=%s where clsh=%s",sJym,sFph,sLsh)>0) CommitWork();
			clearline(23);
		} //����¼�վݺ�
		OutputPrintForm(ppf,NULL,nprint,sBuffer,sSysCzymc);
	}
	DropPrintForm(ppf);
}

void PrintzwDffpL(char * sLsh)  //��ѷ�Ʊ��ӡ
{
     long nRetVal,nCount,nprint,i,j,no;
     char sResult[TOTAL_PACKAGE_SIZE],stemp[160]="\0";
     char sBuffer[4096],sTemp[160]="\0";
     char * offset;
     char jfrq[9],jh[8],yhqh[8],hbh1[6],pbbh[18],yhmc[41],hhhh[9],yhdz[41],gddy[11],hbh[6];
     char qtl_title[10],qtl_data[12];
     char ydlb1[11],zds_new1[13],zds_old1[13],jlbl1[13],sydl1[13],sh1[13],dljj1[13],hjdl1[13],djjj1[13],dfje1[13];
     char jjlb1[11],jjdl1[13],jjdj1[13],jjje1[13];
     char zdlll[13],sum_real[13],sum_chinese[51],sfqsr[3];
     char space[20]="         ";
     
     nRetVal = AutoRunTask("dldj",TASK_REQUEST_PRINT,"%s","%s",sLsh,sResult);
     if(nRetVal)
     {
         gotoxy(14,22);
         outtext(sTcpipErrInfo);
         keyb();
         return;
     }

     if(strncmp(sResult,"000000",6))
     {
         outtext("�޴�ӡ����!");
         return;
     }

     offset = FetchStringToArg(sResult+7,"%d|",&nCount);

     if(nCount<1)
        return;

     for(nprint=1;nprint<=nCount;nprint++)
     {
         gotoxy(14,22);
   	 outtext("���������ʼ��ӡ��%ld��(С)��Ʊ����%ld�ţ�ESC������...",nprint,nCount);
         if(keyb()==KEY_ESC) return;

		 memset(sBuffer,0,strlen(sBuffer));
         
  	 	 offset=FetchStringToArg(offset,"%8s%8s%9s%1d%4s%40s%40s%10s%5s",
  	 	 		jfrq,hhhh,pbbh,&no,yhqh,yhmc,yhdz,gddy,hbh);
		 sprintf(sBuffer,"\n\n%s%-20s%10s\n\n%11s%-30s%-8s%-32s%ld�µ��\n\n\n",space,jfrq,yhqh,space,yhmc,hhhh,yhdz,(atol(jfrq)%10000)/100);
		
//		 for(i=1;i<=7;i++)
//		 {
//		 	offset=FetchStringToArg(offset,"%10s%12s",qtl_title,qtl_data);
//		 }		 		
		 offset=FetchStringToArg(offset,"%154s",stemp);

		 for(i=1;i<=7;i++)
		 {
		 	offset=FetchStringToArg(offset,"%10s%12s%12s%12s%12s%12s%12s%12s%12s%12s",
		 		ydlb1,zds_new1,zds_old1,jlbl1,sydl1,sh1,dljj1,hjdl1,djjj1,dfje1);
			
			if(i>4) continue;
		 	sTemp[0]='\0';
		 	sprintf(sTemp,"%s%10s%10s%8s%8s%8s%8s%8s%10s%10s\n",
		 		space,ydlb1,zds_new1,zds_old1,jlbl1,sh1,dljj1,hjdl1,djjj1,dfje1);
		 	strcat(sBuffer,sTemp);
		 }

		 for(i=1;i<=3;i++) strcat(sBuffer,"\n");

		 for(i=1;i<=7;i++)
		 {
		 	offset=FetchStringToArg(offset,"%10s%12s%12s%12s",jjlb1,jjdl1,jjdj1,jjje1);
		 	sTemp[0]='\0';
		 	sprintf(sTemp,"%s%-12s%-8s%8s%8s",space,jjlb1,jjdl1,jjdj1,jjje1);
		 	offset=FetchStringToArg(offset,"%10s%12s%12s%12s",jjlb1,jjdl1,jjdj1,jjje1);
		 	if(i>4 && i<7) continue;
		 	stemp[0]='\0';
		 	sprintf(stemp,"%s%s%-12s%8s%8s%8s\n",sTemp,space,jjlb1,jjdl1,jjdj1,jjje1);
		 	strcat(sBuffer,stemp);
		 }

		 offset=FetchStringToArg(offset,"%12s%12s%50s%2s",zdlll,sum_real,sum_chinese,sfqsr);
		 sTemp[0]='\0';
		 sprintf(sTemp,"\n%16s�����%-40s%26s",space,sum_chinese,sum_real);
		 strcat(sBuffer,sTemp);
		 sTemp[0]='\0';
         sprintf(sTemp,"\n%40s%s%30s%ld",space,sSysCzymc,space,nSysLkrq);
         strcat(sBuffer,sTemp);

        setprinton();
	output(sBuffer);
      	eject();
        setprintoff();
     }//end for	 		

}//this is little format

////////////////////////////BIG FP FORMAT////////////////////
void PrintzwDffpB(char * sLsh)
{
     long nRetVal,nCount,nprint,i,j,no;
     char sResult[TOTAL_PACKAGE_SIZE],stemp[160]="\0";
     char sBuffer[4096],sTemp[160]="\0";
     char * offset;
     char jfrq[9],jh[8],yhqh[8],hbh1[6],pbbh[18],yhmc[41],hhhh[9],yhdz[41],gddy[11],hbh[6];
     char qtl_title[10],qtl_data[12];
     char ydlb1[11],zds_new1[13],zds_old1[13],jlbl1[13],sydl1[13],sh1[13],dljj1[13],hjdl1[13],djjj1[13],dfje1[13];
     char jjlb1[11],jjdl1[13],jjdj1[13],jjje1[13];
     char zdlll[13],sum_real[13],sum_chinese[51],sfqsr[3];
     char space[20]="         ";
     
     nRetVal = AutoRunTask("dldj",TASK_REQUEST_PRINT,"%s","%s",sLsh,sResult);
     if(nRetVal)
     {
         gotoxy(14,22);
         outtext(sTcpipErrInfo);
         keyb();
         return;
     }

     if(strncmp(sResult,"000000",6))
     {
         outtext("�޴�ӡ����!");
         return;
     }

     offset = FetchStringToArg(sResult+7,"%d|",&nCount);

     if(nCount<1)
        return;

     for(nprint=1;nprint<=nCount;nprint++)
     {
         gotoxy(14,22);
   		 outtext("���������ʼ��ӡ��%ld��(��)��Ʊ����%ld�ţ�ESC������...",nprint,nCount);
         if(keyb()==KEY_ESC) return;

		 memset(sBuffer,0,strlen(sBuffer));
         
  	 	 offset=FetchStringToArg(offset,"%8s%8s%9s%1d%4s%40s%40s%10s%5s",
  	 	 		jfrq,hhhh,pbbh,&no,yhqh,yhmc,yhdz,gddy,hbh);
		 sprintf(sBuffer,"\n\n%s%-20s%10s\n\n%11s%-30s%-8s%-32s%s%s\n\n\n",space,jfrq,yhqh,space,yhmc,hhhh,yhdz,gddy,hbh);
		
//		 for(i=1;i<=7;i++)
//		 {
//		 	offset=FetchStringToArg(offset,"%10s%12s",qtl_title,qtl_data);
//		 }		 		
		 offset=FetchStringToArg(offset,"%154s",stemp);

 		 for(i=1;i<=7;i++)
		 {
		 	offset=FetchStringToArg(offset,"%10s%12s%12s%12s%12s%12s%12s%12s%12s%12s",
		 		ydlb1,zds_new1,zds_old1,jlbl1,sydl1,sh1,dljj1,hjdl1,djjj1,dfje1);

		 	sTemp[0]='\0';
		 	sprintf(sTemp,"%s%10s%10s%10s%10s%10s%10s%10s%10s%10s\n",
		 		space,ydlb1,zds_new1,zds_old1,jlbl1,sh1,dljj1,hjdl1,djjj1,dfje1);
		 	strcat(sBuffer,sTemp);
		 }

		 for(i=1;i<=1;i++) strcat(sBuffer,"\n");

		 for(i=1;i<=14;i++)
		 {
		 	offset=FetchStringToArg(offset,"%10s%12s%12s%12s",jjlb1,jjdl1,jjdj1,jjje1);
		 	sTemp[0]='\0';
		 	sprintf(sTemp,"%s%-12s%-12s%12s%12s\n",space,jjlb1,jjdl1,jjdj1,jjje1);
		 	strcat(sBuffer,stemp);
		 }

		 offset=FetchStringToArg(offset,"%12s%12s%50s%2s",zdlll,sum_real,sum_chinese,sfqsr);
		 sTemp[0]='\0';
		 sprintf(sTemp,"\n%16s�����%-40s%26s",space,sum_chinese,sum_real);
		 strcat(sBuffer,sTemp);
		 sTemp[0]='\0';
         sprintf(sTemp,"\n%40s%s%30s%ld",space,sSysCzymc,space,nSysLkrq);
         strcat(sBuffer,sTemp);

        setprinton();
	output(sBuffer);
	eject();
        setprintoff();
     }//end for

}//this is big format

void PrintHyTerrafp(char * sLsh,long nType)  //MSHY��˰��Ʊ��ӡ
{
	PRINTFORM * ppf;
	long nprint;
	long nYsnd,nYsyf,nLd;
	long nKey,nXjrq;
	double dSjk,dZk=0;
	char cYhbz[15]=".",sTemp[10]=".",sSjk[16]=".",cBz[20]=".",sFph[12]=".",sZk[20]=".",zk[14]=".",sSh[10]=".";
     
	Clear_Space(1,23,80,23);
     
	RunSelect("select dsfmx5,cyhbz from dl$ywmx_%t "
				"where clsh=%s into %lf,%s",
				sJym,sLsh,&dSjk,cYhbz);

	strcpy(sSh,cYhbz+2);
        nprint=atol(sSh)+40000;
	sprintf(sSh,"%ld",nprint);
	if(strncmp(cYhbz,"601",3))
	{
	  strcpy(sTemp,"1.00%");
	  sprintf(sSjk,"%.2lf",dSjk*0.01);
	  dZk=dSjk*0.25;
	}     
	else
	{  
	  strcpy(sTemp,"5.00%");
	  sprintf(sSjk,"%.2lf",dSjk*0.05);
	  dZk=dSjk*0.29;
	}

	if(!(ppf=CreatePrintForm("hydswsz"))) return ;
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	for(nprint=1;nprint<=ppf->nCount;nprint++)
	{
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,ppf->nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
         	if(nType==2)
		{
			while(1)
			{
				sFph[0]='\0';
				if(GetValue(14,23,"�������µ��ݺ�:%-12s",sFph)==KEY_ESC) continue;
				alltrim(sFph);
				if(strlen(sFph)<3){
				 gotoxy(14,23);
				 outtext("���ݺŲ���Ϊ��");
				 keyb();
				 clearline(23);
				 continue;
				}
				break;
			}
			if(RunSql("update dl$ywmx_%t set cbl8=%s where clsh=%s",sJym,sFph,sLsh)>0) CommitWork();
			clearline(23);
		} //����¼�վݺ�
		
		to_Chinese(dZk,sZk);
		sprintf(zk,"%.2lf",dZk);
         	OutputPrintForm(ppf,NULL,nprint,
			(nSysLkrq/10000),(nSysLkrq%10000)/100,(nSysLkrq%10000)%100,sTemp,sSjk,sZk,zk,sSysCzymc,sSh);
	
	}
	DropPrintForm(ppf);
}

void PrintMsTerrafp(char * sLsh,long nType) //üɽ��˰��˰֤
{
	PRINTFORM * ppf;
	long nprint,nCount,ncount;
	long nId,nRetVal,nBl=0,i;
	char sz[30],pm[30],sBuf[5][2048],sTemp[720],space[80]="  ";
	double jsje,sl,sks,dSjk[5];
	char sZk[60]=".",sFph[13]=".",sSjk[20]=".";
	
	Clear_Space(1,23,80,23);
     
	if(!(ppf=CreatePrintForm("msterrawsz"))) return ;
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",
							sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	
	RunSelect("select count(cbl3)  from dl$ywmx_%t where clsh=%s into %ld",
			sJym,sLsh,&nCount);
	nCount=(nCount/5)+1;

	nId=OpenCursor("select  cbl3,cbl10,dsfmx2,dsfmx3,dsfmx5 from dl$ywmx_%t "
       			"where  clsh=%s order by nbl1 ",sJym,sLsh);
	if(nId<=0) return;
	
	ncount=0;
	for(i=0;i<5;i++) 
	{
		dSjk[i]=0;
		sBuf[i][0]='\0';
	}//end for
	while(1)
	{
		nRetVal=FetchCursor(nId,"%s%s%lf%lf%lf",sz,pm,&jsje,&sl,&sks);
		if(nRetVal<=0) break;
		ncount++;
		sTemp[0]='\0';
		if(ncount%5==1)
		sprintf(sTemp,"%-14s  %-20s   %12.2lf  %8.3lf��  %6s  %12.2lf\n",sz,pm,jsje,sl,space,sks);
		else
		sprintf(sTemp,"        %-14s  %-20s   %12.2lf  %8.3lf��  %6s  %12.2lf\n",sz,pm,jsje,sl,space,sks);

		nprint=(ncount-1)/5;
		strcat(sBuf[nprint],sTemp);
		dSjk[nprint]+=sks;
	}
	CloseCursor(nId);

	for(nprint=1;nprint<=nCount;nprint++)
	{
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
		if(nprint!=1 || nType==2)
		{
			while(1)
			{
				sFph[0]='\0';
				if(GetValue(14,23,"�������µ��ݺ�:%-12s",sFph)==KEY_ESC) continue;
				alltrim(sFph);
				if(strlen(sFph)==7) break;
				gotoxy(14,23);
				outtext("���ݺű���Ϊ7λ[��ȷ]");
				keyb();
				clearline(23);
			}
			if(nprint==1 && nType==2) nBl=0;
			else nBl=5;
			if(RunSql("update dl$ywmx_%t set cbl8=%s  where clsh=%s  and nbl1>%ld",sJym,sFph,sLsh,nBl)>0) CommitWork();
			clearline(23);
		} //����¼�վݺ�
		sZk[0]='\0';
		to_Chinese(dSjk[nprint-1],sZk);
		sprintf(sSjk,"%.2lf",dSjk[nprint-1]);
		OutputPrintForm(ppf,NULL,nprint,
			(nSysLkrq%10000)/100,(nSysLkrq%10000)%100,sBuf[nprint-1],sZk,sSjk,sSysCzymc);
	}
	DropPrintForm(ppf);
}

//���ٵ�˰���շ�Ʊ
void PrintRsTerrafp(char * sLsh,long nType)
{
	PRINTFORM * ppf;
	long nprint;
	char sFph[14]=".";
	
	Clear_Space(1,23,80,23);
     
	RunSelect("select cbl8 from dl$ywmx_%t "
				"where clsh=%s into %s",
				sJym,sLsh,sFph);

	
	
	if(!(ppf=CreatePrintForm("rsdswsz"))) return ;
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	for(nprint=1;nprint<=ppf->nCount;nprint++)
	{
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,ppf->nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
         	if(nType==2)
		{
			while(1)
			{
				sFph[0]='\0';
				if(GetValue(14,23,"�������µ��ݺ�:%-12s",sFph)==KEY_ESC) continue;
				alltrim(sFph);
				if(strlen(sFph)<3){
				 gotoxy(14,23);
				 outtext("���ݺŲ���Ϊ��");
				 keyb();
				 clearline(23);
				 continue;
				}
				break;
			}
			if(RunSql("update dl$ywmx_%t set cbl8=%s where clsh=%s",sJym,sFph,sLsh)>0) CommitWork();
			clearline(23);
		} //����¼�վݺ�
		
		OutputPrintForm(ppf,NULL,nprint,
			sFph,(nSysLkrq/10000),(nSysLkrq%10000)/100,(nSysLkrq%10000)%100);
	
	}
	DropPrintForm(ppf);
}

void PrintDyTerrafp(char * sLsh,long nType)
{
	PRINTFORM * ppf;
	long nprint;
	char sFph[14]=".";
	
	Clear_Space(1,23,80,23);
     
	RunSelect("select cbl8 from dl$ywmx_%t "
				"where clsh=%s into %s",
				sJym,sLsh,sFph);

	
	
	if(!(ppf=CreatePrintForm("dydswsz"))) return ;
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	for(nprint=1;nprint<=ppf->nCount;nprint++)
	{
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,ppf->nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
         	if(nType==2)
		{
			while(1)
			{
				sFph[0]='\0';
				if(GetValue(14,23,"�������µ��ݺ�:%-12s",sFph)==KEY_ESC) continue;
				alltrim(sFph);
				if(strlen(sFph)<3){
				 gotoxy(14,23);
				 outtext("���ݺŲ���Ϊ��");
				 keyb();
				 clearline(23);
				 continue;
				}
				break;
			}
			if(RunSql("update dl$ywmx_%t set cbl8=%s where clsh=%s",sJym,sFph,sLsh)>0) CommitWork();
			clearline(23);
		} //����¼�վݺ�
		
		OutputPrintForm(ppf,NULL,nprint,
			sFph,(nSysLkrq/10000),(nSysLkrq%10000)/100,(nSysLkrq%10000)%100);
	
	}
	DropPrintForm(ppf);
}

void PrintDlTerrafp(char * sLsh,long nType)  //üɽ����
{
	PRINTFORM * ppf;
	long nprint;
	char sFph[14]=".";
	
	Clear_Space(1,23,80,23);
     
	RunSelect("select cbl8 from dl$ywmx_%t "
				"where clsh=%s into %s",
				sJym,sLsh,sFph);

	
	
	if(!(ppf=CreatePrintForm("dldswsz"))) return ;
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	for(nprint=1;nprint<=ppf->nCount;nprint++)
	{
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,ppf->nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
         	if(nType==2)
		{
			while(1)
			{
				sFph[0]='\0';
				if(GetValue(14,23,"�������µ��ݺ�:%-12s",sFph)==KEY_ESC) continue;
				alltrim(sFph);
				if(strlen(sFph)<3){
				 gotoxy(14,23);
				 outtext("���ݺŲ���Ϊ��");
				 keyb();
				 clearline(23);
				 continue;
				}
				break;
			}
			if(RunSql("update dl$ywmx_%t set cbl8=%s where clsh=%s",sJym,sFph,sLsh)>0) CommitWork();
			clearline(23);
		} //����¼�վݺ�
		
		OutputPrintForm(ppf,NULL,nprint,
			sFph,(nSysLkrq/10000),(nSysLkrq%10000)/100,(nSysLkrq%10000)%100);
	
	}
	DropPrintForm(ppf);
}

void PrintPxDfsj(char * sLsh)
{
	PRINTFORM * ppf;
	long nprint;
	
	Clear_Space(1,23,80,23);
     
	if(!(ppf=CreatePrintForm("pqdfsj"))) return ;
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	for(nprint=1;nprint<=ppf->nCount;nprint++)
	{
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld���վݣ���%ld�ţ�ESC������...",nprint,ppf->nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
        	OutputPrintForm(ppf,NULL,nprint,sSysCzymc);
	
	}
	DropPrintForm(ppf);
}
//////////////////////////////
void PrintPxDffp(char * sLsh)
{
	PRINTFORM * ppf;
	long nprint;
	
	Clear_Space(1,23,80,23);
     
	if(!(ppf=CreatePrintForm("pxdffp"))) return ;
	if(LoadPrintData(ppf,"from DL$YWMX_%t where clsh=%s",sJym,sLsh)<0)
	{
		DropPrintForm(ppf);
		return ;
	}
	for(nprint=1;nprint<=ppf->nCount;nprint++)
	{
		gotoxy(14,22);
		outtext("���������ʼ��ӡ��%ld�ŷ�Ʊ����%ld�ţ�ESC������...",nprint,ppf->nCount);
		if(keyb()==KEY_ESC)
		{
			DropPrintForm(ppf);
			return ;
		}
        	OutputPrintForm(ppf,NULL,nprint,sSysCzymc);
	
	}
	DropPrintForm(ppf);
}

