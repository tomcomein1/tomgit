/*-------------------------------------------------------------*

                 ����ҵ��ϵͳƱ�ݴ�ӡ��ʽ����


Original programmer : �²�
Created date : 2001.01.02

*-------------------------------------------------------------*/
#include <stdarg.h>
#include <stdio.h>
#include "dlpublic.h"
#include "tools.h"
#include "dlprint.h"
#include "query.h"

#ifndef HOUTAI
/*------------------------------------------------------------*
  ������BillFormatManager
  ���ܣ���ӡ���ݹ������
*------------------------------------------------------------*/
void BillFormatManager()
{

   long nId,nChoose;
   MENU * pmn;
   TABLE * pt;
   long nWhere;
   char sName[81],sTable[81];

   if(!(pmn=CreateMenu("MENU",0))) return;

   nId=OpenCursor("select distinct cname,ctable from dl$print");
   if(nId<0)
   {
   	HiError(nId,"�α����!");
        DropMenu(pmn);
   	return;
   }
   for(;;)
   {
   	if(FetchCursor(nId,"%s%s",sName,sTable)<=0) break;
   	AddMenu(pmn,sName,sTable);
   }
   CloseCursor(nId);

   if(!(pt=CreateTable(2,
        "|��ӡģʽ����        |                                            |")))
   {
   	DropMenu(pmn);
   	return;
   }
   MergeTable(pt,CreateTable(1,
        "|������ģʽ���ƺ����ݻ�������ENTERȷ��/������ESC����...            |"));

   FillTable(pt,2,"%s","���ݱ���������סϢ");

   clrscr();

   if(ShowTable(pt,6,6)<0) return;

   memset(sName,'\0',sizeof(sName));
   strcpy(sTable,"DUAL");

   for(;;)
   {

        nWhere=2;
   	nChoose=FetchTable(pt,1,nWhere,">%20s",sName);
        if(nChoose==KEY_ESC)
        {
        	DropMenu(pmn);
                DropTable(pt);
        	return;
        }

        alltrim(sName);

        RunSelect("select ctable from dl$print where cname=%s into %s",sName,sTable);
        alltrim(sTable);
        if(nChoose==KEY_RETURN)
        {
        	if(strlen(sName)<2)
        	{
                        savewin();
        		nChoose=PopMenu(pmn,26,6,16);
        		popwin();
        	        if(nChoose<1) continue;
              	        strcpy(sName,GetMenu(pmn,nChoose));
                        strcpy(sTable,GetAddition(pmn,nChoose));
                        FillTable(pt,2,"> %-50.50s",sTable);
                        continue;
                }
        }
        else continue;

        alltrim(sTable);
        alltrim(sName);
   	for(;;)
   	{

   		nChoose=FetchTable(pt,2,nWhere,">%40s",sTable);

        	if(nChoose==KEY_ESC)
        	{
        		DropMenu(pmn);
	                DropTable(pt);
        		return;
        	}
                if(nChoose==KEY_UP) break;
        	alltrim(sTable);
                if(nChoose==KEY_RETURN)
                	if(strlen(sTable)>2) break;

        }
        if(nChoose==KEY_UP) continue;
        break;

   }//end for
   DropMenu(pmn);
   DropTable(pt);


   StartWorkSpace(sName,sTable);

}


static void StartWorkSpace(char * sName,char * sTable)
{

    DATATABLE * pdt;
    TABLE * pt;
    MENU * phm;
    char sValue[142],sOpt[3],sKind[6];
    char * trans;
    long nId,nRow=0,nCol=0,nLength=0,nPrecision=0,nFlag=0;
    long nCurrent,nKey;
    long nWhere,nModifyFlag=0;



    pdt=CreateDataTable(7,"|  | �� | �� |              ��          ��            |����|����|С��|",
                       "%-1.1s%4d%4d%-40.40s%1d%4d%2d");

    if(pdt==NULL) return;

    SetTableStyle(pdt,ST_DOUBLELINE|ST_ALLTRIM|ST_LINENUM,1);
    AssignTableFields(pdt,"copt,nrow,ncol,cvalue,nflag,nlength,nprecision");

    if(LoadBillFormat(pdt,sName))
    {
    	 DropDataTable(pdt);
    	 return;
    }
    clrscr();
    gotoxy(1,1);
    outtext("                     �� �� ҵ �� ϵ ͳ �� ӡ Ʊ �� �� ��");
    ShowDataTable(pdt,1,3);

    nCurrent=1;
    nWhere=1;

    for(;;)
    {
        memset(sValue,0,sizeof(sValue));

        if(AutoExtendTable(pdt,&nCurrent)>0)
            ImportData(pdt,nCurrent,"copt,nflag,nlength","+",4,1);

        switch(nWhere)
        {
             case 1:
	          ShowDatatableInfo(pdt,"��������ƹ��ܣ�C:���Ϊ,X:ģ����ʾ,M:���ư���,Y:ȫ���ƶ�...",0);
                  break;
             case 2:
                  ShowDatatableInfo(pdt,"�������ֵ�Ĵ�ӡ��λ��...",0);
                  break;
             case 3:
                  ShowDatatableInfo(pdt,"�������ֵ�Ĵ�ӡ��λ��...",0);
                  break;
             case 4:
                  break;
             case 5:
                  ShowDatatableInfo(pdt,"�����ӡֵ���ͣ�1���ַ��� 2������ 3����д��� 4������/����C���� "
                  "5������ \n  6������(YYYY/MM/DD)    7������(YYYY��MM��DD��)      8�������",0);
                  break;
             case 6:
                  ShowDatatableInfo(pdt,"�������ֵ�Ĵ�ӡ���ȣ��ܳ��ȣ�...",0);
                  break;
             case 7:
                  ShowDatatableInfo(pdt,"�������ֵ��С��λ���ȣ�ӦС���ܳ���-2��...",0);
                  break;
        }


        if(nWhere!=4)  nKey=FetchData(pdt,nCurrent,nWhere,
                                      "copt,nrow,ncol,cvalue,nflag,nlength,nprecision");
        else           nKey=KEY_RETURN;

        switch(nKey)
        {
            case KEY_RETURN:
                 break;
            case KEY_ESC:
                 if(nWhere==1)
                 {
                 	if(nModifyFlag)
                 	{
                 	     nKey=MessageBox("���������޸���δ��⣬Ҫ�����",0);
                 	     ShowDataTable(pdt,1,3);
                 	     if(nKey==KEY_ESC) break;

                 	     if(nKey==KEY_YES)
                 	     {
                 	        ShowDatatableInfo(pdt,"�������...",0);
                                SaveBillFormat(pdt,sName,sTable);
                             }
               		     DropDataTable(pdt);
                 	     return;
                 	}
                 	else
                 	{
                 	     DropDataTable(pdt);
                 	     return;
                 	}

                 }
                 else nWhere=1;
                 continue;
           default:
                 DefaultTableKey(pdt,nKey,&nCurrent,&nWhere);
                 continue;
        }//end switch


        ExportData(pdt,nCurrent,"copt,nrow,ncol,cvalue,nflag,nlength,nprecision",
                       sOpt,&nRow,&nCol,sValue,&nFlag,&nLength,&nPrecision);
        ImportData(pdt,nCurrent,"copt","+");

        switch(nWhere)
        {
           case 1:
                switch(sOpt[0])
                {

                        case 's':
                        case 'S':
                             ShowDatatableInfo(pdt,"�������...",0);
                             if(SaveBillFormat(pdt,sName,sTable))
                                     nModifyFlag=0;
                             continue;

                        case 'c':
                        case 'C':
                             savewin();
                             if(SaveBillFormat(pdt,NULL,sTable))
                                          nModifyFlag=0;
                             popwin();
                             continue;

                        case 'X':
                        case 'x':
                             savewin();
                             ShowBillFormat(pdt);
                             popwin();
                             break;

                        case 'Y':
                        case 'y':
                             nModifyFlag = ChangeFormPosition(pdt);
                             continue;


                        default:
                             DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nWhere,&nModifyFlag);
                             continue;
                     }//end switch
                     break;
           case 2:
                if(nRow<0) continue;
                nWhere=3;
                break;

           case 3:
           case 4:
                if(nCol<0) continue;
                nModifyFlag=1;
                ShowDatatableInfo(pdt,"������Ҫ��ӡ��ֵ���ֶΡ����ʽ������������"
                "(�����ܰ����س����ȿ����ַ�)��\n�����룺$lf��$ld��$s����C���Ա�����"
                "��ע�⣺ÿһ����ֻ�ܰ���һ�����ã�",0);
                savewin();
                nKey=InputString(21,(nCurrent-pdt->nCurrent+1)*2+pdt->pTable->npy,
                                    "  ",INPUT_STRING_SIZE-1,sValue);

                alltrim(sValue);
                popwin();
                if(nKey==KEY_LEFT)
                {
                	nWhere=3;
                    	break;
                }
                if(nKey==KEY_UP)
                {
                      if(nCurrent>1)
                      {
                      	nCurrent--;
                              nWhere=4;
                      	break;
                      }
                }
                if(nKey==KEY_DOWN)
                {
                      if(nCurrent<pdt->nCount)
                      {
                      	nCurrent++;
                      	nWhere=4;
                      	break;
                      }
                }
                nWhere=5;
                break;

           case 5:
                if(nFlag<1||nFlag>8) continue;
                nWhere=6;
                nLength=0;
                if(nFlag==4) nLength=strlen(sValue);
                if(nFlag==6) nLength=10;
                if(nFlag==7) nLength=13;
                break;

           case 6:
                if(nLength<=0) continue;
                if(nFlag==2)
                {
                     nWhere=7;
                     nPrecision=2;
                     break;
                }

           case 7:
                if(nPrecision>0&&nPrecision>nLength-3) continue;
                nCurrent++;
                nWhere=2;
                continue;

        }//end switch(nWhere)
        ImportData(pdt,nCurrent,"cvalue,nflag,nlength,nprecision",
                                sValue,nFlag,nLength,nPrecision);
    }//end for
}

static long LoadBillFormat(DATATABLE * pdt,char * sName)
{

    char * trans;
    char sValue[142];
    long nId,nRow=0,nCol=0,nLength=0,nPrecision=0,nFlag=0,nCurrent,nRetVal;

    if((nId=OpenCursor("select nrow,ncol,cvalue,nflag,nlength,nprecision from dl$print \
                        where  cname=%s order by nrow,ncol",sName))<=0)
    {
    	DropDataTable(pdt);
    	HiError(nId,"Error!");
    	return -1;
    }
    nCurrent=0;

    for(;;)
    {
        nCurrent++;
        memset(sValue,0,sizeof(sValue));
        nRetVal = FetchCursor(nId,"%ld%ld%s%ld%ld%ld",
                       &nRow,&nCol,sValue,&nFlag,&nLength,&nPrecision);
        if(nRetVal==0) break;
        if(nRetVal<0)
        {
    	     DropDataTable(pdt);
    	     HiError(nRetVal,"Error!");
    	     return -1;
        }

        trans=sValue;
        while(trans[0])
        {
        	if(trans[0]=='\"') trans[0]='\'';
        	trans++;
        }

        ImportData(pdt,nCurrent,"copt,nrow,ncol,cvalue,nflag,nlength,nprecision",
                   "+",nRow,nCol,sValue,nFlag,nLength,nPrecision);

    }
    CloseCursor(nId);
    return 0;
}

static long SaveBillFormat(DATATABLE *pdt,char *sName,char *sTable)
{
    char sValue[142],sOpt[3],sKind[6],sMsName[21];
    long nLoop,nRow=0,nCol=0,nLength=0;
    long nRetVal,nPrecision=0,nFlag=0;
    long nWhere=2;
    char * trans;
    TABLE * pt;
	long nCxms=0;

    nLoop=0;

    if(sName==NULL)
    {
    	pt=CreateTable(1,"|������ģʽ����|                      |");
    	if(pt==NULL) return 0;

    	ShowTable(pt,20,20);
    	memset(sMsName,0,sizeof(sMsName));
    	for(;;)
    	{
    	     nFlag=FetchTable(pt,1,nWhere,"> %20s",sMsName);
             if(nFlag==KEY_ESC||nFlag==KEY_RETURN) break;
        }

    	DropTable(pt);
    	if(nFlag==KEY_ESC)  return 0;
    }
    else strcpy(sMsName,sName);
    alltrim(sMsName);

	//if(GetValue(20,20,"���������ģʽ:%5ld",&nCxms)!=KEY_RETURN) return 0;
	
    RunSql("delete dl$print where cname=%s",sMsName);
    nRetVal=1;
    for(nLoop=1;nLoop<=pdt->nCount;nLoop++)
    {
    	ExportData(pdt,nLoop,"copt,nrow,ncol,cvalue,nflag,nlength,nprecision",
                   sOpt,&nRow,&nCol,sValue,&nFlag,&nLength,&nPrecision);

        trans=sValue;
        while(trans[0])
        {
        	if(trans[0]=='\'') trans[0]='\"';
        	trans++;
        }

        if(nCol>0&&nRow>0)
              nRetVal=RunSql("insert into dl$print(cname,ctable,ncol,nrow,cvalue,\
                        nflag,nlength,nPrecision) values(%s,%s,%ld,%ld,%s,\
                        %ld,%ld,%ld)",sMsName,sTable,nCol,nRow,sValue,
                        nFlag,nLength,nPrecision);

        if(nRetVal<0) break;
    }

    if(nRetVal>0)
    {
    	CommitWork();
    	return 1;
    }
    else
    {
    	RollbackWork();
    	return 0;
    }

}

static void ShowBillFormat(DATATABLE * pdt)
{

    long nStX=0,nStY=0,nRow,nCol,nprint;
    long nKey,nLoop,nLength,nFlag;
    char sValue[142];
    TEXTBOX show,reset;
    long nstepx=0,nstepy=0;

    reset.x=1;
    reset.y=1;
    reset.nLen=1;
    strcpy(reset.sValue," ");
    clrscr();

    gotoxy(4,2);
    outtext("+");
    for(nprint=5;nprint<=74;nprint+=5)
    {
    	gotoxy(nprint,2);
    	outtext("----+");

    }
    outtext("->");
    for(nprint=3;nprint<23;nprint++)
    {
    	gotoxy(4,nprint);
    	outtext("$");
    }
    outtext("\n   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+->\
                       ��ʹ���������ҹ���������ESC���˳�ģ����ʾ...");


    for(;;)
    {

        for(nFlag=0;nFlag<=1;nFlag++)
        {

            if(nFlag==0)
            {
            	for(nLoop=3;nLoop<=22;nLoop++)
            	{
        		gotoxy(5,nLoop);
        		printf("                                                                             ");
                }
                continue;
            }
            nStX+=nstepx;
            nStY+=nstepy;
            for(nLoop=1;nLoop<=pdt->nCount;nLoop++)
            {
        	ExportData(pdt,nLoop,"nrow,ncol,cvalue,nlength",&nRow,&nCol,sValue,&nLength);
        	alltrim(sValue);

        	if((nRow-nStY)<1||(nRow-nStY)>20||(nCol-nStX+nLength)<1||
        	                   (nCol-nStX)>(SCREEN_WIDTH-5)) continue;

        	show.x=nCol-nStX+4;
        	show.y=nRow-nStY+2;
        	show.nLen=nLength;
                if(show.x+nLength>=SCREEN_WIDTH)
                  	show.nLen=SCREEN_WIDTH-show.x-1;
        	memset(show.sValue,'\0',sizeof(show.sValue));
        	strncpy(show.sValue,sValue,show.nLen);

                if(show.x<5 && (show.x+nLength)>4)
                {
                     nprint=-show.x+5;
                     show.x=5;
                     show.nLen-=nprint;
                     memset(show.sValue,' ',nprint);
                     alltrim(show.sValue);

                }


        	ReverseXY(&show);
        	ShowXY(&reset);
            }

        }//end for


	for(nprint=5;nprint<=75;nprint+=5)
    	{
    		gotoxy(nprint-1,1);
    		printf("%-ld  ",nprint+nStX-5);

    	}
    	for(nprint=3;nprint<23;nprint++)
    	{
    		gotoxy(1,nprint);
    		printf("%3ld",nprint+nStY-2);
    	}



        nstepx=0;
        nstepy=0;
        for(;;)
        {
    	    switch(keyb())
    	    {
    	    	case KEY_ESC:
    	    	case 'e':
    	    	case 'E':
    	    	     return;
    	    	case KEY_UP:
    	    	     if(nStY>=1)
    	    	     {
    	    	     	nstepy=-1;
    	    	     	break;
    	    	     }
    	    	     continue;

    	    	case KEY_DOWN:
    	    	     nstepy=1;
    	    	     break;

    	    	case KEY_LEFT:
    	    	     if(nStX>=2)
    	    	     {
    	    	     	nstepx=-2;
    	    	     	break;
    	    	     }
    	    	     continue;
    	    	case KEY_RIGHT:
    	    	     nstepx=2;
    	    	     break;

    	     }
             break;
         }//end for
     }//end for

}


static long ChangeFormPosition(DATATABLE * pdt)
{

     long mx,my,nId,nRow,nCol,nKey;
     char sValue[21];


     mx=pdt->pTable->npx+2;
     my=pdt->pTable->npy+pdt->pTable->nLine*2+1;
     memset(sValue,'\0',sizeof(sValue));
     ShowDatatableInfo(pdt,"",0);

     nKey=GetValue(mx,my,
     "�������ƶ���λ������(���Ҹ���,���¸���):%-10.10s",
     sValue);
     if(nKey!=KEY_RETURN) return 0;
     alltrim(sValue);

     if(FetchStringToArg(sValue,"%d,%d",&mx,&my)==NULL)
     {
     	ShowDatatableInfo(pdt,
     	"��ʽ����:��ȷ��ʽ�磺\"-1,2\"��ʾ������һ��λ�ã�������2����",
     	1);
     	return 0;
     }

     for(nId=1;nId<=pdt->nCount;nId++)
     {
     	ExportData(pdt,nId,"ncol,nrow",&nCol,&nRow);
     	if((nCol+mx)<0||(nRow+my)<0)
     	{
      	    ShowDatatableInfo(pdt,
     	    "������Ч��ֵ������ӡ�����귶Χ...",
     	     1);
     	    return 0;
     	}
     }

     for(nId=1;nId<=pdt->nCount;nId++)
     {
     	ExportData(pdt,nId,"ncol,nrow",&nCol,&nRow);
        nCol+=mx;
        nRow+=my;
        ImportData(pdt,nId,"ncol,nrow",nCol,nRow);
     }
     ShowDatatableInfo(pdt,"�޸ĳɹ���",1);
     return 1;
}

#endif

/*------------------------------------------------------------*
  ������CreatePrintForm
  ���ܣ����ݴ�ӡģʽ���ƽ�DL$PRINT����Ӧ�ĸ�ʽ����װ�ص���ӡ����ĸ�ʽ����;
  ������
        sPrintName:��ӡģʽ���ƣ�

  ���أ�NULL:ʧ��
        ���򣬷���һ��ָ���ѽ����õĴ�ӡ���ݱ�ָ�루PRINTFORM *��
  ˵����
        sPrintName�����ڱ��ش�ӡƱ�ݹ������Ѿ����ڣ�
  ������

        #include"dlprint.h"

        PRINTFORM * ppf;
        long nLoop;


        ppf=CreatePrintForm("printtest");
        if(ppf==NULL) return;      //����

        LoadPrintData(ppf,"where cyhbz=%s",sPhone);
        if(ppf->nCount<=0) return; //ppf->nCount��ֵ��ʾ�ж�������Ҫ��ӡ��

        for(nLoop=1;nLoop<=ppf->nCount;nLoop++)
        {

            //�ڴ�ӡÿһ��ʱ�������ܸı����ò�����ֵ���Դﵽ��ӡʱ����C���Ա���
            //��Ŀ�ģ�
            //ע������ʱ�������е�˳��Ӧ���ӡ�����е�����˳��һ�£������ϵ��£�
            //�����ҵ�˳�����С�

            OutputPrintForm(ppf,NULL,nLoop,������д���ò���);
        }
        DropPrintForm(ppf);//����ͷŴ�ӡ��

 *------------------------------------------------------------*/


PRINTFORM * CreatePrintForm(char * sPrintName)
{

    PRINTFORM * ppf;
    char sValue[142],sTable[51];
    long nCurrent,nRow=0,nCol=0,nLength=0,nPrecision=0,nFlag=0;
    long nId;

    if(!(ppf=(PRINTFORM *)malloc(sizeof(PRINTFORM)))) return NULL;
    ppf->pd=NULL;
    memset(ppf,0,sizeof(PRINTFORM));

    if(RunSelect("select distinct ctable from dl$print where cname=%s into %s",
       sPrintName,ppf->sTable)<=0)
    {
        MessageBox("�ô�ӡģʽ��δ���ã��������ã�",1);
        DropPrintForm(ppf);
        CommitWork();
        return NULL;
    }

    alltrim(ppf->sTable);

    ppf->pf=CreateContainer("%4d%4d%s%1d%4d%2d");

    if((nId=OpenCursor("select nrow,ncol,cvalue,nflag,nlength,nPrecision from dl$print \
                        where  cname=%s order by nrow,ncol",sPrintName))<=0)
    {
    	DropPrintForm(ppf);
        GetSqlErrInfo(sValue);
        MessageBox(sValue,1);
        CommitWork();
    	return NULL;
    }
    nCurrent=0;

    for(;;)
    {
        nCurrent++;
        memset(sValue,0,sizeof(sValue));
        if(FetchCursor(nId,"%ld%ld%s%ld%ld%ld",
                       &nRow,&nCol,sValue,&nFlag,&nLength,&nPrecision)<=0) break;
        ImportData(ppf->pf,nCurrent,"%d%d%s%d%d%d",
                       nRow,nCol,sValue,nFlag,nLength,nPrecision);

    }
    CloseCursor(nId);
    CommitWork();
    if(ppf->pf->nCount<=0)
    {
    	DropPrintForm(ppf);
    	return NULL;
    }
    return ppf;  //�����ѽ����õĸ�ʽ����

}

/*------------------------------------------------------------*
  ������LoadPrintData
  ���ܣ�������װ�صĸ�ʽ���ݼ��ֶ����ݲ����WHERE�Ӿ��γ�SQL��ѯ���,
        ������ѯ�õ�������װ�ص���ӡ����Ĵ�ӡ���ݱ���;
  ������
        sWhere:SQL���������Ӿ䣬����where�Ӿ䡱
        ...:where�Ӿ�����Ҫ��������������%ld,%s,%lf,%t

  ���أ� <0:ʧ��
        ���򣬷��ط��������ļ�¼��(�ɴ�ӡ���������)
 *------------------------------------------------------------*/

long LoadPrintData(PRINTFORM * ppf,char * sWhere,...)
{
    va_list pVar;
    QUERYUNIT prdata;
    char sImp[200],sSql[4000];
    char * sFetch,* sOffset,* trans;
    long nValue=0,nRetVal,nLoop,nFlag,nId;
    double dValue=0.0;
    char sValue[INPUT_STRING_SIZE]="\0";
    long nCurrent,nError;

    memset(sSql,'\0',sizeof(sSql));
    memset(&prdata,0,sizeof(QUERYUNIT));
    sOffset=sSql;
    sFetch=prdata.sFormat;

    strcpy(sOffset,"select ");
    while(sOffset[0]) sOffset++;

    //////////////////  ���ݴ�ӡ��ʽ�����γ�SQL��� ///////////////////////////
    nLoop=0;
    for(nCurrent=1;nCurrent<=ppf->pf->nCount;nCurrent++)
    {
    	ExportData(ppf->pf,nCurrent,"> > %s %d > >",sValue,&nFlag);
        trans=sValue;
        while(trans[0])
        {
        	if(trans[0]=='\"') trans[0]='\'';
        	trans++;
        }

    	if(nFlag!=4)
    	{
       	     sprintf(sOffset,"%s,",sValue);
             while(sOffset[0]) sOffset++;

             sFetch[0]='%';
             sFetch++;
             switch(nFlag)
             {
             	case 1:
             	     sFetch[0]='s';
             	     sFetch++;
             	     prdata.data[nLoop].pChar=(char *)malloc(INPUT_STRING_SIZE);
             	     break;
             	case 2:
             	case 3:
             	case 8:
             	     sFetch[0]='l';
             	     sFetch++;
             	     sFetch[0]='f';
             	     sFetch++;
             	     prdata.data[nLoop].pDouble=(double *)malloc(sizeof(double));
             	     break;
             	default:
             	     sFetch[0]='l';
             	     sFetch++;
             	     sFetch[0]='d';
             	     sFetch++;
             	     prdata.data[nLoop].pLong=(long *)malloc(sizeof(long));
             	     break;
             }
             nLoop++;
        } // end if
    }//end for

    sOffset--;
    sOffset[0]=' ';
    sOffset++;

    va_start(pVar,sWhere);
    FormSqlWithArgs(sWhere,pVar,sOffset);
    va_end(pVar);

    if(!ppf->pd) ppf->pd=CreateContainer(prdata.sFormat);
    if(ppf->pd==NULL)
    {
    	ReleaseQueryUnit(&prdata);
    	return -2;
    }
    while(ppf->pd->nCount) DeleteDataNode(ppf->pd,ppf->pd->nCount);

    nId=OpenCursor(sSql);
    if(nId<0)
    {
    	ReleaseQueryUnit(&prdata);
    	HiError(nId,"ȡ��ӡ���ݳ���");
    	return -3;
    }

    nCurrent=0;
    for(;;)
    {
      nCurrent++;
      sFetch=prdata.sFormat;
      nLoop=0;

      if(FetchStruction(nId,&prdata)<=0)
      {
      	 CloseCursor(nId);
      	 ReleaseQueryUnit(&prdata);
      	 return nCurrent-1;
      }

      while(sFetch=strchr(sFetch,'%'))
      {
         memset(sImp,0,sizeof(sImp));
         memset(sImp,'>',nLoop);
         sImp[nLoop]='%';

         sFetch++;

         switch(sFetch[0])
         {
             case 's':
                  sImp[nLoop+1]='s';
                  ImportData(ppf->pd,nCurrent,sImp,prdata.data[nLoop].pChar);
                  break;
             case 'l':
                  sFetch++;
                  switch(sFetch[0])
                  {
                        case 'd':
                             sImp[nLoop+1]='d';
                             ImportData(ppf->pd,nCurrent,sImp,
                                        *(prdata.data[nLoop].pLong));
                             break;
                        case 'f':
                             sImp[nLoop+1]='f';
                             ImportData(ppf->pd,nCurrent,sImp,
                                        *(prdata.data[nLoop].pDouble));
                             break;
                  }
                  break;
         }
         nLoop++;
      }//end while
      ppf->nCount++;
    }//end for
}

/*------------------------------------------------------------*
  ������OutputPrintForm��
  ���ܣ�����ӡ���������ӡ�����ַ�����
  ������
        ppf:��CreatePrintForm,����LoadPrintDataװ�����ݺ�Ĵ�ӡ����ָ�룻
        outbuf:�����ֵ��ΪNULL���򽫴�ӡ�������뵽�û��壻
               ����ֱ�������ӡ����
        nIndex:��ӡ�ŵ���ţ�
        ...:��ӡ����Ҫ���õ���C���Ա�����
  ���أ�nIndex�ɹ���0��ʧ��
  ˵����
        nIndex��ֵӦ��1��ppf->nCount֮�䡣ֻ���ڴ�ӡ�����д���
        $lf,$s,$ld�����òŻ��б䳤����ֵ��

  ��������μ�CreatePrintForm
 *------------------------------------------------------------*/
long OutputPrintForm(PRINTFORM * ppf,char * outbuf,long nIndex,...)
{
    DATALINK * pdl;
    char sResult[8000];
    char sOutput[4000];
    char sFormat[100];
    char sValue[142];
    char * sFetch;
    long nLine,nLoop,nprint,nValue,address;
    long nRow=0,nCol=0,nLength=0,nPrecision=0,nFlag=0;
    va_list pVar;

    if(ppf==NULL||ppf->nCount<=0) return 0;
    pdl=ppf->pd->pdata;
    if(pdl==NULL) return 0;
    while(--nIndex) pdl=pdl->pNext;
    if(pdl==NULL) return 0;

    memset(sOutput,'\0',sizeof(sOutput));
    memset(sResult,'\0',sizeof(sResult));
    sOutput[0]=' ';
    nprint=0;
    va_start(pVar,nIndex);
    nLoop=1;
    nLine=1;
    for(;;)
    {

        memset(sFormat,'\0',sizeof(sFormat));
    	ExportData(ppf->pf,nLoop,"%d%d%s%d%d%d",
                             &nRow,&nCol,sValue,&nFlag,&nLength,&nPrecision);
        if(nLoop>ppf->pf->nCount) nRow++;

        if(nLine<nRow)
        {
                sFetch=sOutput+3999;
                while(!sFetch[0]) sFetch--;
                while(sFetch>=sOutput)
                {
                	if(sFetch[0]=='\0') sFetch[0]=' ';
                	sFetch--;
                }
                strcat(sResult,sOutput);
       	        memset(sOutput,'\0',sizeof(sOutput));
                memset(sOutput,'\n',nRow-nLine);
                strcat(sResult,sOutput);
                memset(sOutput,'\0',sizeof(sOutput));
                nLine=nRow;

        }
        if(nLoop>ppf->pf->nCount) break;

        address=ppf->pd->pArg[nprint]+pdl->address;
        switch(nFlag)
        {

            case 1:
                sprintf(sFormat,"%%-%ld.%lds",nLength,nLength);
                sprintf(sOutput+nCol,sFormat,address);
                nprint++;
                break;
            case 2:
                sprintf(sFormat,"%%%ld.%ldlf",nLength,nPrecision);
                sprintf(sOutput+nCol,sFormat,*((double *) address));
                nprint++;
                break;
            case 3:
                to_Chinese(*((double *) address),sOutput+nCol);
                nprint++;
                break;
            case 4:
                sFetch=strchr(sValue,'$');
                if(sFetch)
                {
                     sFetch[0]='%';
                     if(strstr(sFetch,"lf"))
                  	sprintf(sOutput+nCol,sValue,va_arg(pVar,double));
                     else if(strstr(sFetch,"ld"))
                  	sprintf(sOutput+nCol,sValue,va_arg(pVar,long));
                     else if(strstr(sFetch,"s"))
                  	sprintf(sOutput+nCol,sValue,va_arg(pVar,char *));
                }
                else    strcpy(sOutput+nCol,sValue);

                break;

            case 5:
                sprintf(sFormat,"%%%ldld",nLength);
                sprintf(sOutput+nCol,sFormat,*((long *)address));
                nprint++;
                break;

            case 6:
                nValue=*((long *)address);
                sprintf(sOutput+nCol,"%4ld/%02ld/%02ld",
                             nValue/10000,(nValue%10000)/100,(nValue%10000)%100);
                nprint++;
                break;
            case 7:
                nValue=*((long *)address);
                sprintf(sOutput+nCol,"%4ld��%02ld��%02ld��",
                             nValue/10000,(nValue%10000)/100,(nValue%10000)%100);
                nprint++;
                break;
            case 8:
                sprintf(sFormat,"��%%.%ldlf",nPrecision);
                sprintf(sOutput+nCol,sFormat,*((double *) address));
                nprint++;
                break;
         }//end switch
         nLoop++;
     }//end for
     va_end(pVar);
     if(outbuf==NULL)
     {
     	 setprinton();
     	 output(sResult);
     	 setprintoff();
     }    
     else strcpy(outbuf,sResult);
     return nIndex;
}

/*------------------------------------------------------------*
  ������DropPrintForm��
  ���ܣ��ͷ�һ����ӡ��Դ
  ������ppf:��CreatePrintForm�����Ĵ�ӡ��
 *------------------------------------------------------------*/

void DropPrintForm(PRINTFORM * ppf)
{
     if(ppf==NULL) return;
     DropDataTable(ppf->pf);
     DropDataTable(ppf->pd);
     free(ppf);
}

/*------------------------------------------------------------*
  ������to_Chinese��
  ���ܣ���һ��˫����������ת���ɴ�д���
  ������
        dMoney:��ת�����ݣ�
        sValue:�����Ż�������
  ˵������ת�����ֵΪ9����
 *------------------------------------------------------------*/
void to_Chinese(double dMoney,char * sValue)
{

     long integer,fraction;
     char sFraction[10];
     char sTemp[60];
     char sInteger[10];
     char * sOffset;
     char sCaps[11][3]={"��","Ҽ","��","��","��","��","½","��","��","��","ʰ"};
     char sUnit[13][3]={"Բ","ʰ","��","Ǫ","��","ʰ","��","Ǫ","��","ʰ","��","Ǫ","��"};



     memset(sFraction,0,sizeof(sFraction));
     memset(sTemp,0,sizeof(sTemp));
     memset(sInteger,0,sizeof(sInteger));
     integer=(long)dMoney;
     sprintf(sFraction,"%4.1lf",(dMoney-(double)integer)*100.0);
     sOffset=strchr(sFraction,'.');
     sOffset[0]='\0';
     fraction=atol(sFraction);

     sOffset=sFraction;


     if(fraction==0&&integer==0)
     {
     	strcpy(sValue,"��Բ��");
     	return;
     }

     //С������
     if(fraction==0) strcpy(sOffset,"��");
     else
     {

        strcpy(sOffset,sCaps[fraction/10]);
        if((fraction/10)>0)
        {
           strcpy(sOffset+2,"��");
           sOffset+=2;
        }
        sOffset+=2;

        if(fraction%10)
        {
     	   strcpy(sOffset,sCaps[fraction%10]);
     	   strcpy(sOffset+2,"��");
     	   sOffset+=4;
        }
        else
        {
           strcpy(sOffset,"��");
           sOffset+=2;
        }
     }


     //��������

     sOffset=sTemp;
     sprintf(sInteger,"%-ld",integer);

     fraction=strlen(sInteger);

     for(integer=0;integer<fraction;integer++)
     {

     	if(sInteger[integer]!='0')
     	{
     		strcpy(sOffset,sCaps[sInteger[integer]-'0']);
     		sOffset+=2;
     		strcpy(sOffset,sUnit[fraction-integer-1]);
     		sOffset+=2;
     	}
     	else
     	{
           if((fraction-integer)==5)
           {
     		strcpy(sOffset,"��");
     	        sOffset+=2;
     	   }
           else if((fraction-integer)==9)
           {
     		strcpy(sOffset,"��");
     	        sOffset+=2;
     	   }

     	   else if(sInteger[integer+1]!='0' && sInteger[integer+1]!='\0')
     	   {
     		strcpy(sOffset,"��");
     	        sOffset+=2;


       	   }
       	   else if(sInteger[integer+1]=='\0')
       	   {
       	   	if(fraction!=1&&sInteger[0]!='0')
       	   	{
           		strcpy(sOffset,"Բ");
         	        sOffset+=2;
         	}
           }
       	}

     }//end for

     sprintf(sValue,"%s%s",sTemp,sFraction);
     return;


}
void ExportPrintTable()
{
     char sFileName[51];
     FILE * fp;
     char sValue[142],sOpt[3],sKind[6],sSql[1024],sName[31],sTable[51];
     long nId,nRow=0,nCol=0,nLength=0,nPrecision=0,nFlag=0;
     char sOldName[51]=".";

     strcpy(sFileName,"dydj.sql");
     clrscr();

     if(GetValue(20,10,"�����뵼��SQL�ļ���:%20s",
                       sFileName)!=KEY_RETURN) return;
     alltrim(sFileName);
     if(!(fp=fopen(sFileName,"wt"))) return;
     if((nId=OpenCursor("select cname,ctable,nrow,ncol,cvalue,nflag,nlength,\
                         nPrecision from dl$print order by cname,nrow,ncol"))<=0)
     {
        GetSqlErrInfo(sValue);
        MessageBox(sValue,1);
    	return;
     }

     clrscr();
     outtext("���ڵ�����ӡ���ݸ�ʽ���ļ�%s�� . . . ",sFileName);
     for(;;)
     {
        memset(sValue,0,sizeof(sValue));
        if(FetchCursor(nId,"%s%s%ld%ld%s%ld%ld%ld",sName,sTable,
           &nRow,&nCol,sValue,&nFlag,&nLength,&nPrecision)<=0) break;

        alltrim(sName);
        if(strcmp(sName,sOldName))
        {
             sprintf(sSql,"\n\ndelete dl$print where cname='%s';\n",sName);
             fwrite(sSql,strlen(sSql),1,fp);
             strcpy(sOldName,sName);
        }
        sprintf(sSql,"insert into dl$print(cname,ctable,ncol,nrow,\
        cvalue,nflag,nlength,nprecision) values('%s','%s',%ld,%ld,\
        '%s',%ld,%ld,%ld);\n",sName,sTable,nCol,nRow,sValue,nFlag,
        nLength,nPrecision);

        fwrite(sSql,strlen(sSql),1,fp);
        outtext(".");
        fflush(stdout);

     }

     strcpy(sSql,"commit;\n");
     fwrite(sSql,strlen(sSql),1,fp);
     fclose(fp);

     outtext("\n������ϣ���Ҫ�ָ���ʽ�뽫�ļ�%s��sqlplus��ִ�м���...",sFileName);
     keyb();
}
