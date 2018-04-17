//´úÊÛÃñº½»úÆ±Ä£¿é

#include"tools.h"
#include"query.h"
#include"dl_air.h"
#include"dlpublic.h"
#include"tasktool.h"
#include"newtask.h"
#include"dlprint.h"

//È«¾Ö¶¨Òå¾Ö
extern char sSysJsdh[10];
extern char sSysCzydh[7];
extern char sSysJsmc[21];
extern long nSysLkrq;
extern long nSysSfm;
extern long nSysCzyqx;
extern char sSysIp[20];
extern char sTcpipErrInfo[81];

static char sFromCode[4],     //ÆðÊ¼³ÇÊÐ´úÂë
            sDestCode[4],     //Ä¿µÄ³ÇÊÐ´úÂë
            sFromCity[21],    //ÆðÊ¼³ÇÊÐÃû³Æ
            sDestCity[21],    //ÖÕµã³ÇÊÐÃû³Æ
            sGuestName[30],   //¿Í»§Ãû³Æ
            sGuestId[20],     //¿Í»§Éí·ÝÖ¤ºÅÂë
            sAddress[50],     //¿Í»§µØÖ·
            sGuestPhone[30],   //ÁªÏµµç»°
            sHpfs[30],
            sHpcs[30],
            sHpdz[50],
	    sMemo[50],         //±¸×¢ÐÅÏ¢
            sAirberth[20][30], //¸÷º½Î»µÄ¿Õ×ùÐÅÏ¢
            sAirLsh[41];      //·¢Æð¶©Æ±ºóµÄÁ÷Ë®ºÅ

static long nAirDate,nAirTime,nHprq,nHpsj;//Æð·ÉÈÕÆÚºÍÊ±¼ä
static DATATABLE * pdt_air=NULL;   //º½°àÁÐ±í
static DATATABLE * pdt_berth=NULL; //²ÕÎ»¼°¼Û¸ñÁÐ±í
static DIALOG * pd_in=NULL;        //Êý¾ÝÊäÈë¶Ô»°¿ò
static DIALOG * pd_air_guest=NULL; //¿Í»§ÐÅÏ¢µÄÊäÈë
static MENU * pm_city=NULL;        //¸÷³ÇÊÐÁÐ±í
static MENU * pm_get_mode=NULL;    //È¡Æ±·½Ê½
static double dPrice,dFinalFee,dMinPrice;
static void printAirAccount(DATATABLE *pdt);
static char sSysDlDz[21]="dldj";
static long SaveAirDdProduct(DATATABLE * pdt,char * sJym);

//¶©Æ±ÒµÎñÈë¿Ú
void AirTicket(long number,char * item,char * sJym)
{
     long nRetVal;
     long nState;
     
     clrscr();
     
      //³õÊ¼»¯±í¸ñ
     if(InitAirTicket()) 
     {
     	 EndAirTicket();
     	 return;
     }

     for(;;)
     {
         PreAirTicket();
         ShowAirTicketFace();

         //ÊäÈëº½ÏßÌõ¼þ
         if(InputAirInfo(sJym))
         {
             EndAirTicket();
             return;
         }
         //¼ì²éÀúÊ·Êý¾Ý
         //nState=CheckQueryState(sJym);
	nState=0;
         
         switch(nState)
         {
           case 1:
              SelectAirLineOne(sJym);             //Ö»ÊÇ²éÑ¯ÀúÊ·Êý¾Ý
              break;
           case 0:
              if(QueryAirLine(sJym)) continue;    //²éÑ¯¶©Æ±
              SelectAirLine(sJym);  
              break;
           default:
              nState=1;
              continue;
         }
     }
}
//´òµÇ»úÒ×:
void AirPrintReceipt(char * sLsh)
{	
     PRINTFORM * ppf;
     long nprint;
     char sJym[16],sJsdh[11];
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     clearline(23);
     if(!(ppf=CreatePrintForm("airdjy"))) return ;
     if(LoadPrintData(ppf,"from DL$DBMX_%t where clsh=%s",
                       sJym,sLsh)<0)
     {
        	DropPrintForm(ppf);
        	return ;
     }
     for(nprint=1;nprint<=ppf->nCount;nprint++)
     {
            gotoxy(14,22);
            outtext("°´ÈÎÒâ¼ü¿ªÊ¼´òÓ¡µÚ%ldÕÅ·¢Æ±£¬¹²%ldÕÅ£¬ESC¼ü·ÅÆú...",nprint,ppf->nCount);
            if(keyb()==KEY_ESC)
            {
            	DropPrintForm(ppf);
            	return ;
            }
            OutputPrintForm(ppf,NULL,nprint);
     }
     DropPrintForm(ppf);

}


static void PreAirTicket()
{

     memset(sFromCity,'\0',sizeof(sFromCity));
     memset(sDestCity,'\0',sizeof(sDestCity));
     memset(sAirberth,'\0',sizeof(sAirberth));
     memset(sGuestName,'\0',sizeof(sGuestName));
     memset(sGuestId,'\0',sizeof(sGuestId));
     memset(sAddress,'\0',sizeof(sAddress));
     memset(sGuestPhone,'\0',sizeof(sGuestPhone));
     memset(sHpfs,'\0',sizeof(sHpfs));
     memset(sHpcs,'\0',sizeof(sHpcs));
     memset(sHpdz,'\0',sizeof(sHpdz));
     memset(sMemo,'\0',sizeof(sMemo));
     nAirDate=nSysLkrq;
     nAirTime=0;
     while(pdt_air->nCount) DeleteDataNode(pdt_air,1);

}

static void EndAirTicket()
{
     if(pdt_air)   pdt_air->pTable->nVisual=0;
     if(pdt_berth) pdt_berth->pTable->nVisual=0;
     DropDataTable(pdt_air);
     DropDataTable(pdt_berth);
     DropDialog(pd_in);
     DropDialog(pd_air_guest);
     DropMenu(pm_city);
     DropMenu(pm_get_mode);
}


static void ShowAirTicketFace()
{
     clrscr();
     outtext("                             ËÄ´¨ÓÊÕþÃñº½»úÆ±´ú°ì");
     ShowDialog(pd_in,6,2);
     ShowDataTable(pdt_air,2,5);
}

static long CheckQueryState(char * sJym)
{
    long nRetVal,nCount;
    char route[7];
    char inbuf[100];
    long nStime,nEtime;
    char sAirLine[10]="0",sAirType[20],sBerth[38],sRoute[20];
    RECORD rec;
    
     ShowDatatableInfo(pdt_air,"ÕýÔÚ¼ì²é²éÑ¯Êý¾Ý£¬ÇëµÈ´ý...",0);
    
     sprintf(route,"%s%s",sFromCode,sDestCode);
     alltrim(route);
     sprintf(inbuf,"%ld|%ld|%s|",nAirDate,nAirTime,route);
    
     nRetVal=AutoRunTask(sSysDlDz,TASK_AIR_CHECK,"%s","%r",inbuf,&rec);
     if(nRetVal)
    	return -1;

     nCount=0;
     for(;;)
     {
        if(ExportRecord(&rec,"%s%s%ld%ld%s%s",
                     sAirLine,sRoute,&nStime,&nEtime,sAirType,sBerth)<=0)
           break;
        
        if(sAirLine[0]=='1' && sRoute[0]=='1')
        { 
           DropRecord(&rec);
           return 0;
        }

        ImportData(pdt_air,pdt_air->nCount+1,
                   "opt,airline,route,st_time,en_time,airtype,berth",
                   "+",sAirLine,sRoute,nStime,nEtime,sAirType,sBerth); 
     }
     DropRecord(&rec);   
     return 1;  
}

static long QueryAirLine(char * sJym)
{

    long nRetVal,nCount;
    char sBuffer[4096];
    char * offset;
    char sAirLine[10],sAirType[20],sBerth[38],sRoute[20];
    char route[8]=".";
    long nStartTime=0,nEndTime=0,nJt=0;
   
    memset(sBuffer,'\0',sizeof(sBuffer));
    ShowDatatableInfo(pdt_air,"ÕýÔÚ²éÑ¯º½°àÐÅÏ¢£¬ÇëµÈ´ý...",0);

    sprintf(route,"%s%s",sFromCode,sDestCode);

    nRetVal=AutoRunTask(sSysDlDz,TASK_QUERY_AIRLINE,"%s%s%s%ld%ld%s","%s",
            sSysJsdh,sFromCode,sDestCode,nAirDate,nAirTime,sSysCzydh,sBuffer);

    if(nRetVal)
    {
    	ShowDatatableInfo(pdt_air,sTcpipErrInfo,1);
    	return -1;
    }

    if(strncmp(sBuffer,"000000",6))
    {
    	ShowDatatableInfo(pdt_air,sBuffer,1);
    	return -2;
    }
    offset=sBuffer+7;

    nCount=0;
    offset=FetchStringToArg(offset,"%d|",&nCount);

    if(nCount<=0)
    {
    	ShowDatatableInfo(pdt_air,"Ã»ÓÐÏàÓ¦µÄº½°àÐÅÏ¢...",1);
    	return -3;
    }

    while(nCount--)
    {
        offset=FetchStringToArg(offset,"%s|%s|%d|%d|%s|%s|%d|",
              sAirLine,sRoute,&nStartTime,&nEndTime,sAirType,sBerth,&nJt);
      
        //if(strcmp(route,sRoute))  continue;
        //fillterberth(sBerth,sBerth);
        //if(FindDataTable(pdt_berth,1,"airline",sAirLine)>0) continue;
        ImportData(pdt_air,pdt_air->nCount+1,
                   "opt,airline,route,st_time,en_time,airtype,berth,jt",
                   "+",sAirLine,sRoute,nStartTime,nEndTime,sAirType,sBerth,nJt);
       
    }
    return 0;
}

static long SelectAirLine(char * sJym)
{

    long nKey,nCurrent,nWhere,nLoop;
    char sOpt[3];

    nCurrent=1;
    for(;;)
    {
        nWhere=1;
        ShowDatatableInfo(pdt_air,"ÇëÑ¡Ôñº½°àºÅ,1:²éÑ¯Æ±¼Û 2.¶©Æ± 3:²éÑ¯¾­Í£µã 4:À©Õ¹²ÕÎ» ",0);
    	
    	nKey=FetchData(pdt_air,nCurrent,nWhere,"opt");
    	if(nKey==KEY_ESC) break;
    	if(nKey!=KEY_RETURN)
    	{
    	     DefaultTableKey(pdt_air,nKey,&nCurrent,&nWhere);
    	     continue;
    	}
    	ExportData(pdt_air,nCurrent,"opt",sOpt);
    	ImportData(pdt_air,nCurrent,"opt","+");
    	if(strchr("IiAa-",sOpt[0])) continue;
    	switch(sOpt[0])
    	{
           case '1':
      	      savewin();
      	      OnLineHelp(sJym,nCurrent);
      	      popwin();
      	      break;      
           
            case '2':
              ImportData(pdt_air,nCurrent,"opt","*");
    	      savewin();
    	      QueryTicketPrice(sJym,nCurrent);
    	      popwin();
    	      break;
    	       	   
    	    case '3':
      	      savewin();
    	      QueryAirRoute(sJym,nCurrent);
    	      popwin();
    	      break;
    	      
      	    case '4':
      	      savewin();
      	      QueryAirBerth(sJym,nCurrent);
      	      popwin();
      	      break;
      	            	      
    	    default: 
    	      DefaultTableChoice(pdt_air,sOpt[0],&nCurrent,&nWhere,0);
    	      RefreshDialog(pd_in,0);
    	      continue;
      	}//end swi
      	
      	ImportData(pdt_air,nCurrent,"opt","+");
      	RefreshDialog(pd_in,0);
      	continue;
      	
    }
}

static long QueryTicketPrice(char * sJym,long nSelect)
{
     
    long nRetVal,nCount,nCurrent,nKey,nWhere,nJt;
    char sResult[4096];
    char sAirLine[11];
    char sairline[11];
    char * offset;
    char sBerth[38],sOpt[3];
    char cBerth[3];
    double dPrice,dMinPrice,dRebate;
    RECORD rec;
     
    ShowDataTable(pdt_berth,18,6);
    ShowDatatableInfo(pdt_air,"Õý²éÑ¯ËùÑ¡º½°àµÄÆ±¼Û,ÇëÉÔºò...",0);
         
    ExportData(pdt_air,nSelect,"airline,berth,jt",sAirLine,sBerth,&nJt);
    alltrim(sAirLine);
    strcpy(sairline,sAirLine);
    sAirLine[2]='\0';
    memset(sResult,'\0',sizeof(sResult));
     
    nRetVal=AutoRunTask(sSysDlDz,TASK_QUERY_AIR_PRICE,"%s%s%s%ld","%r",
             sAirLine,sFromCode,sDestCode,nAirDate,&rec);
     
    if(nRetVal)
    {
    	ShowDatatableInfo(pdt_air,sTcpipErrInfo,1);
    	return -1;
    }    
    
    for(;;)
    {
        if(ExportRecord(&rec,"%s|%lf|%lf|%lf|",
               cBerth,&dPrice,&dMinPrice,&dRebate)<=0)
               break;

        nRetVal=FindDataTable(pdt_berth,1,"berth",cBerth);
        if(nRetVal<=0) nRetVal=pdt_berth->nCount+1;
  	
  	    if(strstr(sBerth,cBerth))
              ImportData(pdt_berth,nRetVal,
                   "opt,berth,price,minprice,rebate",
                   "+",cBerth,dPrice,dMinPrice,dRebate);
    }
    DropRecord(&rec);  
    if(!strcmp(cBerth,"1") )
    {
       	ShowDatatableInfo(pdt_air,"      ÄãÎÞÈ¨¶©¹º´Ëº½¿Õ¹«Ë¾»úÆ±£¡",1);
    	return -1;     
    }
    nCurrent=1;
    
    for(;;)
    {
        nWhere=1;
        ShowDatatableInfo(pdt_air,"ÇëÑ¡Ôñ²ÕÎ»£¬°´'1'¼üÁ¢¼´³öÆ±·½Ê½,'9',ËøÆ±·½Ê½...",0);
   		nKey=FetchData(pdt_berth,nCurrent,nWhere,"opt");
    	if(nKey==KEY_ESC) break;
    	if(nKey!=KEY_RETURN)
    	{
    	     DefaultTableKey(pdt_berth,nKey,&nCurrent,&nWhere);
    	     continue;
    	}
    	ExportData(pdt_berth,nCurrent,"opt",sOpt);
    	ImportData(pdt_berth,nCurrent,"opt","+");
    	if(strchr("IiAa-",sOpt[0])) continue;
    	if(sOpt[0]!='1' && sOpt[0]!='9')
    	{
    	     DefaultTableChoice(pdt_berth,sOpt[0],&nCurrent,&nWhere,0);
             RefreshDialog(pd_in,0);
    	     continue;
    	}
    	savewin();
    	switch(sOpt[0])
    	{
           case '1':
             if(!nJt)
                 PurchaseTicket(sJym,nSelect,nCurrent,1,sairline);
             else ShowDatatableInfo(pdt_air,"º½°à¾­Í£,ÓëÊ¡Æ±ÎñÖÐÐÄÁªÏµ(028-185)",1);
    	     break;
    	   case '9':
             if(!nJt)
    	        PurchaseTicket(sJym,nSelect,nCurrent,2,sairline);
             else ShowDatatableInfo(pdt_air,"º½°à¾­Í£,ÓëÊ¡Æ±ÎñÖÐÐÄÁªÏµ(028-185)",1);
    	     break;
    	}
    	popwin();
        RefreshDialog(pd_in,0);
    }	
    while(pdt_berth->nCount)
    	DeleteDataNode(pdt_berth,pdt_berth->nCount);

    return 0;

}
static void QueryAirRoute(char * sJym,long nSelect)
{
    char sAirLine[8];
    long nRetVal;
    char sResult[1024]=".";

    ExportData(pdt_air,nSelect,"airline",sAirLine);
    
    nRetVal = AutoRunTask(sSysDlDz,TASK_QUERY_AIR_ROUTE,"%s%ld","%s",
                          sAirLine,nAirDate,sResult);
    
    if(nRetVal) MessageBox(sTcpipErrInfo,1);
    else MessageBox(sResult,1);                      
}

static void QueryAirBerth(char * sJym,long nSelect)
{
    char sAirLine[8],sRoute[7];
    char sairline[12];
    long nRetVal,nJt=0;
    char sResult[1024]=".";
    char sTemp[128]=".";
    
    ExportData(pdt_air,nSelect,"airline,route,jt",sAirLine,sRoute,&nJt);
    if(nJt==0)
    {
       sprintf(sairline,"%s|%s|",sAirLine,sFromCode);
       nRetVal=AutoRunTask(sSysDlDz,TASK_QUERY_AIR_BERTH,"%s%ld","%s",
                         sairline,nAirDate,sResult);
    }
    else ShowDatatableInfo(pdt_air,"º½°à¾­Í£,ÓëÊ¡Æ±ÎñÖÐÐÄÁªÏµ(028-86259911)",1);
    
    if(nRetVal) MessageBox(sTcpipErrInfo,1);
    else
    {
       alltrim(sResult);
       sprintf(sTemp,"%s %s:%s",sAirLine,sRoute,sResult);
       //printf("%s",sTemp);
       MessageBox(sTemp,1);  
    }
    ImportData(pdt_air,nSelect,"airline,route,berth",sAirLine,sRoute,sResult); 
    if(!FlushDataLine(pdt_air,nSelect)) 
       ShowDatatableInfo(pdt_air,"³ö´íÁË£¬´Ë´ÎÀ©Õ¹Î´³É¹¦£¡",1);

}

static void OnLineHelp(char * sJym,long nSelect)
{
    QUERYTABLE *pqt;
    long nRetVal;
    char sTitle[200];
    char sBuffer[1024];
    char sAirLine[8],sRoute[7];
    char cBerth[3];
    char sBerth[46];
    double dRebate,dSellbate,dSxf,dPubbate,dBerbate,dSr;
    RECORD rec;

    ExportData(pdt_air,nSelect,"airline,route,berth",sAirLine,sRoute,sBerth);                             
    alltrim(sBerth);
    sprintf(sTitle,"\t\t%s---%s",sAirLine,sRoute);
    pqt=CreateQueryTable(sTitle,
                         "²ÕÎ»  ¹«²¼¼Û      ²ÕÎ»¼Û      ½áËã¼Û      ÕÛ¿Û      ÊÖÐø·Ñ    ÊÕÈë        ",
                         "%-1s  %-10.2lf     %-10.2lf     %-10.2lf  %-6.1lf     %-10.2lf %-12.2lf",ST_VISUAL);
    if(!pqt) return;
    
    sAirLine[2]='\0';
    sprintf(sBuffer,"%s|%s|",sAirLine,sRoute);
    nRetVal=AutoRunTask(sSysDlDz,TASK_AIR_ONLINE,"%s","%r",sBuffer,&rec);
    if(nRetVal) return;
    else
    {
       clrscr();
       for(;;)
       {
	     if(ExportRecord(&rec,"%s%lf%lf%lf%lf%lf%lf",
	                     cBerth,&dPubbate,&dBerbate,&dRebate,
			     &dSellbate,&dSxf,&dSr)<=0) break;

	     if(strstr(sBerth,cBerth))
	       ImportQueryTable(pqt,1,
			cBerth,dPubbate,dBerbate,dRebate,dSellbate,dSxf,dSr);
       }
       ImportQueryTable(pqt,0);
    }//end else 
    DropRecord(&rec);                      
    DropQueryTable(pqt);
    
}//end fun

static long PurchaseTicket(char * sJym,long nAirLine,long nBerth,long nFlag,char * sLine)
{
    char sBuffer[1024];
    char sResult[1024];
    char sAirInfo[1024];
    char sBerth[3],sTemp[10];
    char sAirLine[8];
    char sRoute[8],sAirId[6]=".";
    long st_time,en_time;
    long nIdLen,nTs=0,nSj=0;
    long mx,my,nIndex;
    long nWhere,nKey,nRetVal;
    char sAirType[10]=".";
    char * sMessage[11]={"ÇëÊäÈëÂÃ¿ÍµÄÐÕÃû£¬±ØÐëÓëÉí·ÝÖ¤Ãû×ÖÏàÍ¬...",
			"ÇëÊäÈëÂÃ¿ÍµÄÉú·ÝÖ¤ºÅÂë...",
                        "ÇëÊäÈëÂÃ¿ÍµÄÁªÏµµØÖ·...",
                        "ÇëÊäÈëÂÃ¿ÍµÄÁªÏµµç»°...",
			"ÇëÊäÈë¶©Æ±·½Ê½,»Ø³µ¿É²éÑ¯Ñ¡Ôñ...",
                        "ÇëÊäÈë»»Æ±³ÇÊÐ...",
                        "ÇëÊäÈë»»Æ±µØÖ·...",
                        "ÇëÊäÈë»»Æ±ÈÕÆÚ...",
                        "ÇëÊäÈë»»Æ±Ê±¼ä...",
                        "ÇëÊäÈë±¸×¢ÐÅÏ¢...",
                        "ÇëÊäÈëÊÕÈ¡µÄÊµ¼Ê¿î¶î..."
                       }; 
    char * offset;
    double dRebate;
    
    //dPrice¹«²¼¼Û,dMinPriceº½Î»¼Û\ÕÛ¿Û¼ÛdRebateÕÛ¿ÛdFinalFeeÊµÊÕ¿î
    ExportData(pdt_berth,nBerth,
    		"berth,price,minprice,rebate",sBerth,&dPrice,&dMinPrice,&dRebate);
    sBerth[1]='\0';
    dFinalFee=dMinPrice;
    ShowDialog(pd_air_guest,16,4);

    strcpy(sGuestName,".");
    strcpy(sGuestId,".");
    strcpy(sAddress,".");
    strcpy(sGuestPhone,".");
    strcpy(sHpfs,".");
    strcpy(sHpcs,".");
    strcpy(sHpdz,".");
    strcpy(sMemo,".");
    
    nWhere=1;
    for(;;)
    {    
        RefreshDialog(pd_air_guest,0);
        SetDialogInfo(pd_air_guest,sMessage[nWhere-1]);
        
        nKey=ActiveDialog(pd_air_guest,nWhere,"name,id,address,phone,hpfs,hpcs,hpdz,hprq,hpsj,memo,fee");
        
        switch(nKey)
        {
           case KEY_ESC:
                if(nWhere>1)
                {
             	   nWhere=1;
             	   continue;
                }
                return 0;
           
           case KEY_LEFT:
           case KEY_UP:
                if(nWhere>1) nWhere--;
                continue;
           
           case KEY_RETURN:
                break;
           
           default:continue;
        }              
        
        switch(nWhere)
        {
           case 1:
                alltrim(sGuestName);
                if(strlen(sGuestName)>=4) nWhere++;
                continue;
           case 2:
        	alltrim(sGuestId);
                //nIdLen=strlen(sGuestId);
                //if(nIdLen!=15 && nIdLen!=18) continue;
                offset=sGuestId;
                while(offset[0])
                {
                     if(!strchr("0123456789x",offset[0])) break;
                     offset++;
                }
                if(offset[0]) continue;
                nWhere++;
	        continue;
           case 3:
		alltrim(sAddress);
                nWhere++;
                continue;
           case 4:
                alltrim(sGuestPhone);
                if(strlen(sGuestPhone)<7) continue;
                offset=sGuestPhone;
                while(offset[0])
                {
                     if(!strchr("0123456789-",offset[0])) break;
                     offset++;
                }
                if(offset[0]) continue;
                nWhere++;
                continue;     
           case 5: 
                nIndex=IsInMenu(pm_get_mode,sHpfs,0);
                if(nIndex<=0)
                {
                    GetDialogPosition(pd_air_guest,"hpfs",&my,&mx);
                    mx+=4;
                    savewin();
                    nIndex=PopMenu(pm_get_mode,mx,my,16);
                    popwin();
                    if(nIndex<=0)  continue;
                }

                strcpy(sHpfs,GetMenu(pm_get_mode,nIndex));
                alltrim(sHpfs);
                offset=strstr(sHpfs,"->");
                if(!offset) continue;
                offset+=2;
                strcpy(sHpfs,offset);
		
		alltrim(sHpfs);
                nWhere++;
                continue;
           case 6:
		strcpy(sHpcs,"³É¶¼");
           	alltrim(sHpcs);
           	if(strcmp(sHpcs,"³É¶¼")) continue;
           	nWhere++;
		continue;
           case 7:
    		alltrim(sHpdz);
           	nWhere++;
            	continue;
           case 8:
           	if(nHprq>nAirDate || nHprq<nSysLkrq) continue;
                nWhere++;
                continue;
           case 9:
		//RunSelect("select ntqsj from ticket.dl$air_qpfs where cqpcs=%s and cqpmc=%s into %ld",sHpcs,sHpfs,&nSj);
                nWhere++;
                continue;
           case 10:
        	alltrim(sMemo);
           	nWhere++;
           	continue;
           case 11:
		if(dFinalFee>0.0) break;
           	continue;
        }
        
        RefreshDialog(pd_air_guest,0);
        memset(sResult,'\0',sizeof(sResult));
        memset(sBuffer,'\0',sizeof(sBuffer));
        GenerateLsh(1,sSysJsdh,sJym,sAirLsh);        
        ExportData(pdt_air,nAirLine,"airline,route,st_time,en_time,airtype",
                sAirLine,sRoute,&st_time,&en_time,sAirType);

        ShowDatatableInfo(pdt_air,"ÇëÈ·ÈÏÐÅÏ¢(Y/N),Y¼ü¼ÌÐø,N¼ü·µ»Ø",0);
	    if(GetYn()==KEY_NO) continue;

	if(strlen(sGuestId)<15)  strcpy(sGuestId,".");
        sprintf(sBuffer,"%s|%s|%s|%s|%s|%s|%s|%ld|%04ld|%04ld|%s|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%s|%s|%s|%s|%ld|%ld|%s|%s|%ld|%ld|",
                sAirLsh,sAirLine,sRoute,sFromCity,sDestCity,sAirType,sBerth,nAirDate,st_time,en_time,sGuestName,sAddress,
                sGuestPhone,dPrice,dMinPrice,dFinalFee,dRebate,sSysJsmc,sSysCzydh,sGuestId,sHpfs,nFlag,nAirLine,sHpcs,sHpdz,nHprq,nHpsj);
        
        //[1] ·¢Æð¶©Æ±ÇëÇó
        nRetVal=AutoRunTask(sSysDlDz,TASK_AIR_PURCHASE,"%s","%s",sBuffer,sResult);
        if(nRetVal)
	{
           ShowDatatableInfo(pdt_air,sTcpipErrInfo,1);
	   return -1;
	}
        
        if(!strncmp(sResult,"000000",6))
	{
		//ShowDatatableInfo(pdt_air,sResult,1);
	        if(strstr(sResult,"³É¹¦") ) strcpy(sAirInfo,sResult+6);
	        else sprintf(sAirInfo,"%s%s","»úÆ±¼ÇÂ¼±àÂë",sResult+6);
	        
	        SetDialogInfo(pd_air_guest,sAirInfo);
		while(1) if(keyb()=='Y' || keyb()=='y') break;
		
		if(RunSql("update dl$dbmx_%t set cvalue17=%s,cvalue20=%s \
			where clsh=%s",sJym,sLine,sMemo,sAirLsh)>0) CommitWork();
			
		if(strstr(sResult,"³É¹¦") )  {nWhere=1; continue;}
		//´òµÇ»úÒ×£¡
		SetDialogInfo(pd_air_guest,sResult);
		ShowDatatableInfo(pdt_air,"          °´ÈÎÒâ¼ü´òµÇ»úÒ×,ESCÈ¡Ïû...",0);
		if(keyb()!=KEY_ESC)  AirPrintReceipt(sAirLsh);
        }
        else
        {
            SetDialogInfo(pd_air_guest,sResult);
            ShowDatatableInfo(pdt_air,"      Óë³É¶¼185ÁªÏµ(028)185",1);
 	}
 
	strcpy(sGuestName,".");
	strcpy(sGuestId,".");
	//strcpy(sAddress,".");
	//strcpy(sGuestPhone,".");
	strcpy(sMemo,".");
	nWhere=1;
    }//end for

}

static long InputAirInfo(char * sJym)
{
    long mx,my,nWhere,nKey,nIndex,move;
    char * sMessage[4]={
    	   "ÇëÊäÈëÆðµã³ÇÊÐ´úÂë£¬°´»Ø³µ²éÑ¯...",
    	   "ÇëÊäÈëÖÕµã³ÇÊÐ´úÂë£¬°´»Ø³µ¼ü²éÑ¯...",
    	   "ÇëÊäÈëº½°àÈÕÆÚ(YYYYMMDD)...",
    	   "ÇëÊäÈëº½°àµÄÆðÊ¼Ê±¼ä,ÈôÎª0000±íÊ¾²»ÏÞÖÆÊ±¼ä..."};
    char sCity[51],sSearch[20];
    char * offset;

    nWhere=1;
    for(;;)
    {
 		RefreshDialog(pd_in,0);
 		ShowDatatableInfo(pdt_air,sMessage[nWhere-1],0);

    	nKey=ActiveDialog(pd_in,nWhere,"from,dest,date,time");

    	switch(nKey)
    	{
    	   case KEY_RETURN:
    	        break;

    	   case KEY_ESC:
    	        if(nWhere>1)
    	        {
    	            nWhere=1;
    	            continue;
    	        }
    	        return -1;

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
                alltrim(sFromCity);
                upper(sFromCity);
                nIndex=IsInMenu(pm_city,sFromCity,0);
                if(nIndex<=0)
                {
                    GetDialogPosition(pd_in,"from",&my,&mx);
                    mx+=4;
                    savewin();
                    nIndex=PopMenu(pm_city,mx,my,16);
                    popwin();
                    if(nIndex<=0)  continue;
                }

                strcpy(sCity,GetMenu(pm_city,nIndex));
                alltrim(sCity);
                offset=strstr(sCity,"->");
                if(!offset) continue;
                offset+=2;

                move=0;
                while(offset[0]!='[')
                {
                   sFromCity[move]=offset[0];
                   move++;
                   offset++;
                }
                offset++;
                sFromCity[move]='\0';
                strncpy(sFromCode,offset,3);
                nWhere++;
                continue;

           case 2:
                alltrim(sDestCity);
                upper(sDestCity);
                nIndex=IsInMenu(pm_city,sDestCity,0);
                if(nIndex<=0)
                {
                    GetDialogPosition(pd_in,"Dest",&my,&mx);
                    mx+=7;
                    savewin();
                    nIndex=PopMenu(pm_city,mx,my,16);
                    popwin();
                    if(nIndex<=0)  continue;
                }

                strcpy(sCity,GetMenu(pm_city,nIndex));
                alltrim(sCity);
                offset=strstr(sCity,"->");
                if(!offset) continue;
                offset+=2;

                move=0;
                while(offset[0]!='[')
                {
                   sDestCity[move]=offset[0];
                   move++;
                   offset++;
                }
                offset++;
                sDestCity[move]='\0';
                strncpy(sDestCode,offset,3);
                if(strcmp(sDestCode,sFromCode)==0) continue;
                nWhere++;
                continue;
           case 3:
                if(nAirDate>=nSysLkrq) nWhere=4;
                continue;
           case 4:
                if(nAirTime>=0 && nAirTime<2359) return 0;
                continue;
        }
    }//end for
}

static long InitAirTicket()
{
     RECORD rec,rec1;
     char sCity[41],sSearch[20];
     char sQpmc[20],sQpdh[3];
     long nLoop;

     //½¨Á¢º½ÏßÊý¾Ý±í¸ñ
     pdt_air=CreateDataTable(7,
             "|  |º½°àºÅ|º½¶Î  |Æð·É|½µÂä|»úÐÍ|²ÕÎ»ÐÅÏ¢                            |",
             "%-1.1s %-6.6s %-6.6s %04d %04d %-4.4s %-36.36s%d");
     if(pdt_air==NULL) return -1;
     SetTableStyle(pdt_air,ST_MESSAGELINE|ST_ALLTRIM|ST_LINENUM,1);
     AssignTableFields(pdt_air,"opt,airline,route,st_time,en_time,airtype,berth,jt");

     //½¨Á¢Æ±¼Û¼°²ÕÎ»Êý¾Ý±í¸ñ
     pdt_berth=CreateDataTable(6,
             "|  |²ÕÎ»|ÕýÊ½Æ±¼Û|ÕÛ¿ÛÆ±¼Û|ÕÛ¿ÛÂÊ|",
             "%-1.1s %-2.2s %8.2f %8.2f %6.2f");
     if(pdt_berth==NULL) return -2;
     SetTableStyle(pdt_berth,ST_ALLTRIM,1);
     AssignTableFields(pdt_berth,"opt,berth,price,minprice,rebate");

     //½¨Á¢Êý¾ÝÊäÈë¶Ô»°¿ò
     pd_in=CreateDialog(74,4,NORMAL);
     if(pd_in==NULL) return -3;
     AddDialog(pd_in,"from",INPUT,"Æðµã:%-10.10s",sFromCity);
     AddDialog(pd_in,"dest",INPUT,"   ÖÕµã:%-10.10s",sDestCity);
     AddDialog(pd_in,"date",INPUT,"   ÈÕÆÚ:%8d",&nAirDate);
     AddDialog(pd_in,"time",INPUT,"   Ê±¼ä:%04d",&nAirTime);



     pd_air_guest=CreateDialog(50,19,FRAME);
     if(pd_air_guest==NULL) return -5;

     AddDialog(pd_air_guest,"space",SPACELINE,1);
     AddDialog(pd_air_guest,"text",STATIC_TEXT,"                ÂÃ ¿Í ÐÅ Ï¢ Êä Èë\n");
     AddDialog(pd_air_guest,"sep",SEPARATOR,"~~");
     AddDialog(pd_air_guest,"name",INPUT,"ÂÃ¿ÍÐÕÃû:%-20.20s\n",sGuestName);
     AddDialog(pd_air_guest,"id",INPUT,"Éí·ÝÖ¤ºÅ:%-18.18s\n",sGuestId);
     AddDialog(pd_air_guest,"address",INPUT,"µØÖ·:%-40.40s\n",sAddress);
     AddDialog(pd_air_guest,"phone",INPUT,"ÂÃ¿Íµç»°:%-20.20s\n\n",sGuestPhone);
     AddDialog(pd_air_guest,"hpfs",INPUT,"»»Æ±·½Ê½:%-20.20s\n",sHpfs);
     AddDialog(pd_air_guest,"hpcs",INPUT,"»»Æ±³ÇÊÐ:%-20.20s\n",sHpcs);
     AddDialog(pd_air_guest,"hpdz",INPUT,"»»Æ±µØÖ·:%-38.38s\n",sHpdz);
     AddDialog(pd_air_guest,"hprq",INPUT,"»»Æ±ÈÕÆÚ:%8d",&nHprq);
     AddDialog(pd_air_guest,"hpsj",INPUT,"    »»Æ±Ê±¼ä:%6d\n\n",&nHpsj);
     AddDialog(pd_air_guest,"memo",INPUT,"±¸×¢:%-40.40s",sMemo);
     AddDialog(pd_air_guest,"sep",SEPARATOR,"~~");
     AddDialog(pd_air_guest,"price",OUTPUT, "Õý¼Û:%8.2f",&dPrice);
     AddDialog(pd_air_guest,"minprice",OUTPUT, "  ´òÕÛ¼Û:%8.2f",&dMinPrice);
     AddDialog(pd_air_guest,"fee",  INPUT,"  ÊµÊÕ:%8.2f",&dFinalFee);

     //½¨Á¢³ÇÊÐ¼ìË÷°ïÖú²Ëµ¥

     pm_city=CreateMenu("MENU",0);
     pm_get_mode=CreateMenu("MENU",0);
     if(AutoRunTask(sSysDlDz,TASK_GET_ALL_CITY," ","%r""%r",&rec,&rec1))
     {
     	return -6;
     }
     for(;;)
     {
        if(ExportRecord(&rec,"%s%s",sCity,sSearch)<=0) break;
        if(sCity[0]=='.') break;
        AddMenu(pm_city,sCity,sSearch);
     }
     DropRecord(&rec);

     for(;;)
     {
        if(ExportRecord(&rec1,"%s%s",sQpmc,sQpdh)<=0) break;
        if(sQpmc[0]=='.') break;
        AddMenu(pm_get_mode,sQpmc,sQpdh);
     }
     DropRecord(&rec1);

     GetSysLkrq();
     return 0;
}

/////////////////////////////////////////////////////////////////////////////
//³öÆ±
void OutAirTicket(long number,char * item,char * sJym)
{
	char sResult[1024];
	long nRetVal;
	long nFlag=1;
	long nKey;
	char *offset;
	char inbuf[1024];
	char sYhbz[6];
   
	clrscr();
	gotoxy(20,3);outtext("È¡ÏûËøÆ±»òÈË¹¤³öÆ±");
	gotoxy(20,4);outtext("==================");
	while(1)
	{
		sYhbz[0]='\0';
		nKey=GetValue(20,6,"ÇëÊäÈë»úÆ±¼ÇÂ¼±àÂë[5Î»]:%-5.5s",sYhbz);
   	    
		if(nKey==KEY_ESC)  return;
		alltrim(sYhbz);
		if(strlen(sYhbz)<5) continue;
   	   	        
		if(GetValue(20,7,"ÇëÊäÈëÑ¡Ôñ(1.È¡Ïû 2.³öÆ±):%1ld",&nFlag)==KEY_ESC)
			return;
   	   
		upper(sYhbz);
		if(nFlag==1 || nFlag==2)
 			break;
   	   
	}//end while

	gotoxy(20,8);printf("È·ÈÏ²Ù×÷(y/n):");
	nKey=GetYn();
	if(nKey==KEY_NO)
		return;
		
	sprintf(inbuf,"%s|%s|",sJym,sYhbz);
	nRetVal=AutoRunTask(sSysDlDz,TASK_OUT_AIR_TICKET,"%s%ld","%s",inbuf,nFlag,sResult);
	
	outtext("\n\n%s",sResult);
	keyb();
	if(nFlag==2 && strstr(sResult,"000000") ) 
	AirPrintReceipt(sResult+7);
}

static long SelectAirLineOne(char * sJym)
{
    long nKey,nCurrent,nWhere,nLoop;
    long i;
    char sOpt[3];

    nCurrent=1;
    for(;;)
    {
        nWhere=1;
        ShowDatatableInfo(pdt_air,"ÇëÑ¡Ôñº½°àºÅ,1:²éÑ¯Æ±¼Û 2.È·¶¨¶©Æ±  <ÀúÊ·Êý¾Ý>",0);
    	
    	nKey=FetchData(pdt_air,nCurrent,nWhere,"opt");
    	if(nKey==KEY_ESC) break;
    	if(nKey!=KEY_RETURN)
    	{
    	     DefaultTableKey(pdt_air,nKey,&nCurrent,&nWhere);
    	     continue;
    	}
    	ExportData(pdt_air,nCurrent,"opt",sOpt);
    	ImportData(pdt_air,nCurrent,"opt","+");
    	if(strchr("IiAa-",sOpt[0])) continue;
    	switch(sOpt[0])
    	{
           case '1':
      	      savewin();
      	      OnLineHelp(sJym,nCurrent);
      	      popwin();
      	      break;      
           
            case '2':
              //for(i=1;i<=pdt_air->nCount;i++)
  	          // DeleteDataNode(pdt_air,i);
              pdt_air->nCount=0;
              if(QueryAirLine(sJym)) return 1;
              SelectAirLine(sJym);
              return 1;

    	    default: 
    	      DefaultTableChoice(pdt_air,sOpt[0],&nCurrent,&nWhere,0);
    	      RefreshDialog(pd_in,0);
    	      continue;
      	}//end swi
      	
      	ImportData(pdt_air,nCurrent,"opt","+");
      	RefreshDialog(pd_in,0);
      	continue;
      	
    }
}

void CheckAirTicket(long number,char * item,char *sJym)
{
    DATATABLE * pdt;
    long i,nKey,nCurrent,nwhere,nBz,nModify=1;
    long nId,nLkrq,nRetVal,nCount;
    long nSt_time;
    char inbuf[100]=".";
    char sAirId[6]=".",sName[22]=".",sFromCity[21]=".",sDestCity[21]=".";
    char sAirLine[21]=".",sBerth[4]=".";
    char sLsh[41]=".";
    char sDy[2]=".";

    char sOpt[2];
    RECORD rec;
	
    clrscr();

	pdt=CreateDataTable(8,
			"|  |Æ±±àÂë|ÓÃ»§Ãû³Æ  |º½¶Î          |Æð·ÉÊ±¼ä|²ÕÎ»|¶¨Æ±ÈÕÆÚ|´òÓ¡|",
			"%-1.1s%-5.5s%-10.10s %-14.14s        %-4.4ld %-4.4s %-8.8ld %1s %s");
	if(!pdt)
	{
		MessageBox("½¨Á¢Êý¾Ý±í¸ñ³ö´í...",1);
		return;
	}
    
	AssignTableFields(pdt,"opt,cairid,cname,cairline,nst_time,cberth,nlkrq,cdy,clsh");
	SetTableStyle(pdt,ST_ALLTRIM|ST_MESSAGELINE|ST_UPDOWN|ST_LINENUM,1);
	clearline(3);
	gotoxy(30,1);
	outtext("»úÆ±´òÓ¡È·ÈÏ¹ÜÀí");
	ShowDataTable(pdt,5,3);
	ShowDatatableInfo(pdt,"ÕýÔÚÌáÈ¡Êý¾Ý£¬ÇëµÈ´ý...",0);
    
	sprintf(inbuf,"%s|",sJym);
	nRetVal=AutoRunTask(sSysDlDz,TASK_LOAD_CHECK_TICKET,"%s","%ld%r",inbuf,&nBz,&rec);
	if(nRetVal)	return;
	if(nBz==-1)
	{
		ShowDatatableInfo(pdt,"ÌáÈ¡Î´È·ÈÏÊý¾Ý³ö´í£¡",1);
		return;
	}
	nCount=0;
	
	strcpy(sDy,".");
	for(i=0;;i++)
	{
		if(ExportRecord(&rec,"%s%s%s%s%s%s%ld%ld",
			sLsh,sName,sAirId,sFromCity,sDestCity,sBerth,&nSt_time,&nLkrq)<=0)	break;
 		
 		sprintf(sAirLine,"%s-%s",sFromCity,sDestCity);
		if(strlen(sAirLine)>13) sAirLine[13]='\0';
 		
		if(strlen(sAirId)<5)  strcpy(sAirId,"*");
		ImportData(pdt,pdt->nCount+1,"opt,cairid,cname,cairline,nst_time,cberth,nlkrq,cdy,clsh",
				"+",sAirId,sName,sAirLine,nSt_time,sBerth,nLkrq,sDy,sLsh);
	}
	DropRecord(&rec);
	
	if(i==0)
	{
		ShowDatatableInfo(pdt,"Ã»ÓÐÐèÒª´òÓ¡È·ÈÏµÄ»úÆ±£¡ÈÎÒ»½¡·µ»Ø...",1);
		DropDataTable(pdt);
		return;
	}

	nCurrent=1;//start with the first line
	nwhere=1;
    
	for(;;)
	{
//		if(AutoExtendTable(pdt,&nCurrent)>0)
//			ImportData(pdt,nCurrent,"opt,cairid,cdy","+",".",".");
		
		switch(nwhere)
		{
			case 1:
				ShowDatatableInfo(pdt,"ÇëÊäÈë¹¦ÄÜ¿ØÖÆ,°´M¼ü°ïÖú...",0);
				break;
			case 2:
				ShowDatatableInfo(pdt,"Çë¼ì²é»úÆ±¼ÇÂ¼±àÂë...",0);
				break;
			case 3:
				ShowDatatableInfo(pdt,"»úÆ±´òÓ¡³É¹¦Çë°´<y>...",0);
				break;	

			default: break;
		}
		
		nKey=FetchData(pdt,nCurrent,nwhere,"opt,cairid,cdy");

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
				nKey=MessageBox("ÄúµÄºË¶Ô½á¹ûÃ»ÓÐ±£´æ£¬Òª±£´æÂð£¿",0);
				if(nKey==KEY_ESC)
				{
                   	ShowDataTable(pdt,5,3);
			       	continue;
				}
				if(nKey==KEY_YES)
				{
					nKey=MessageBox("Êý¾ÝÐèÒª×ÐÏ¸ºË¶Ô,ÄúÒÑ¾­·´¸´ºË¶ÔÁËÂð£¿",0);
					if(nKey==KEY_ESC)
					{
                   			ShowDataTable(pdt,5,3);
			       		continue;
					}
					if(nKey==KEY_YES)
					{
						ShowDataTable(pdt,5,3);
						if(SaveAirTicketData(pdt,sJym)==0)
						continue;
					}
				}
		    	DropDataTable(pdt);
		    	return;
            default:
				DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
				continue;
		}
	
		ExportData(pdt,nCurrent,"opt,cairid,cdy",sOpt,sAirId,sDy);
		ImportData(pdt,nCurrent,"opt","+");
		
		switch(nwhere)
		{
			case 1: //²Ù×÷·½·¨
				switch(sOpt[0])
				{
					case 's':   //save as temporary record
					case 'S':
						nKey=MessageBox("Êý¾ÝÐèÒª×ÐÏ¸ºË¶Ô,ÄúÒÑ¾­·´¸´ºË¶ÔÁËÂð£¿",0);
						if(nKey==KEY_ESC)
						{
                   			ShowDataTable(pdt,5,3);
			       			continue;
						}
						if(nKey==KEY_YES)
						{
							ShowDataTable(pdt,5,3);
							if(SaveAirTicketData(pdt,sJym)==0)
							continue;
						}
						DropDataTable(pdt);
						return;
					default:
						DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nwhere,&nModify);
						continue;
				}break;
			
			case 2:
				alltrim(sAirId);
				if(strlen(sAirId)!=5)
					continue;
				nwhere++;
				break;
			case 3:
				alltrim(sDy);
				if(strcmp(sDy,"y")!=0&&strcmp(sDy,"Y")!=0&&strcmp(sDy,".")!=0)
					continue;
				nwhere++;
				break;		
		}//end switch(nwhere);
		
		ImportData(pdt,nCurrent,"cairid,cdy",sAirId,sDy);

		if(nwhere>3)
		{
			if(nCurrent<pdt->nCount)
				nCurrent++;
			nwhere=2;
		}
	}//end for(;;)
}

static long SaveAirTicketData(DATATABLE * pdt,char *sJym)
{
	long nRetVal,nId,i;
	char inbuf[4096],inbuf1[4096],sResult[200];
	char sAirId[6]=".",sLsh[41]=".",sDy[2]=".";

	ShowDatatableInfo(pdt,"ÕýÔÚÈë¿â£¬ÇëÉÔºò...",0);
    
	memset(inbuf,'\0',strlen(inbuf));
	memset(inbuf1,'\0',strlen(inbuf));
	i=0;
	for(nId=1;nId<=pdt->nCount;nId++)
    {
    	ExportData(pdt,nId,"cairid,cdy,clsh",sAirId,sDy,sLsh);
    	if(strlen(sLsh)<40) continue;

		alltrim(sDy);
    	alltrim(sAirId);
    	
    	if(strcmp(sDy,"y")!=0&&strcmp(sDy,"Y")!=0)
    		continue;
    	i++;

    	sprintf(inbuf,"%s%s|%s|",inbuf1,sAirId,sLsh);
        strcpy(inbuf1,inbuf);      
    }
    sprintf(inbuf,"%s|%ld|%s",sJym,i,inbuf1);
	if(i==0) return 0;
    nRetVal=AutoRunTask(sSysDlDz,TASK_SAVE_CHECK_RESULT,"%s","%s",inbuf,sResult);
    ShowDatatableInfo(pdt,sResult,1);
    if(nRetVal) return 0;
    else return 1;
}


//»úÆ±»úÆ±ÊÕ¿îÔ¤²é
void AirTicketPay(long number,char * item,char *sJym)
{
	DATATABLE * pdt;
	RECORD rec;
	long i,nRetVal,nJsbz,nId;
	double dSjk,dZk=0;
	char sName[20],sPhone[20],sId[6],sLsh[41];
	char sFromCity[21]=".",sDestCity[21]=".",sAirLine[21]=".";
    long nBz,nSlkrq=nSysLkrq,nElkrq;
	char sTitle[100],sOpt[2];
	long nKey,nCurrent,nwhere,nModifyFlag;
	char inbuf[4096],outbuf[1024]=".";
   
	clrscr();
	GetSysLkrq();
	
	nSlkrq=nSysLkrq;
	if(GetValue(2,3,"ÇëÊäÈëÆðÊ¼ÈÕÆÚ:%-8ld",&nSlkrq)==KEY_ESC) return;
	nElkrq=nSysLkrq;
	if(GetValue(2,5,"ÇëÊäÈë½ØÖ¹ÈÕÆÚ:%-8ld",&nElkrq)==KEY_ESC) return;
      
	clrscr();
	sprintf(sTitle,"[%ld--%ld] %s»úÆ±ÊÕ¿î´¦Àí[%s]",nSlkrq,nElkrq,sSysJsmc,sSysCzydh);
	outtext("\n            %s",sTitle);

    pdt=CreateDataTable(8,
    "|  |½ð¶î    |º½¶Î          |ÂÃ¿ÍÐÕÃû  |ÁªÏµµç»°        |Æ±ºÅ  |ÊÕ¿î|",
    "%-1.1s%-8.1lf%-14.14s       %-8.8s   %-16.16s           %-5.5s %1ld");
    if(!pdt)
    {
		MessageBox("½¨Á¢Êý¾Ý±í¸ñ³ö´í...",1);
		return;
	}
    AssignTableFields(pdt,"opt,dsjk,chd,cname,cphone,cid,njsbz");
    SetTableStyle(pdt,ST_ALLTRIM|ST_MESSAGELINE|ST_UPDOWN,1);
    
	ShowDatatableInfo(pdt,"ÕýÔÚÌáÈ¡Êý¾Ý£¬ÇëµÈ´ý...",0);

	sprintf(inbuf,"%s|%d|%d|%s|",sJym,nSlkrq,nElkrq,sSysCzydh);
	nRetVal=AutoRunTask(sSysDlDz,TASK_LOAD_PAY_TICKET,"%s","%ld%r",inbuf,&nBz,&rec);
	if(nRetVal)	return;
	if(nBz==-1)
	{
		ShowDatatableInfo(pdt,"ÌáÈ¡Êý¾Ý³ö´í£¡",1);
		return;
	}
	
	for(i=0;;i++)
	{
		if(ExportRecord(&rec,"%lf%ld%s%s%s%s%s",
			&dSjk,&nJsbz,sName,sId,sFromCity,sDestCity,sPhone)<=0)	break;

 		sprintf(sAirLine,"%s-%s",sFromCity,sDestCity);
		if(strlen(sAirLine)>13) sAirLine[13]='\0';
 		
		ImportData(pdt,pdt->nCount+1,"opt,dsjk,chd,cname,cphone,cid,njsbz",
  	               "+",dSjk,sAirLine,sName,sPhone,sId,nJsbz);
	}
	DropRecord(&rec);
   
	if(i==0)
	{
		ShowDatatableInfo(pdt,"¸ÃÈÕÆÚ¶ÎÄÚÃ»ÓÐ¶©Æ±£¡ÈÎÒ»½¡·µ»Ø...",1);
		DropDataTable(pdt);
		return;
	}
	

	//begin input
	nCurrent=1;//start with the first line
	nwhere=1;
	nModifyFlag=0;//the document is modified if the flag is nozero
	ShowDataTable(pdt,3,3);
    
	for(;;)
	{
		dZk=0;
		for(i=1;i<=pdt->nCount;i++)
		{
			ExportData(pdt,i,"dsjk,njsbz",&dSjk,&nJsbz);
			if(nJsbz==1) dZk+=dSjk;
		}
		
		switch(nwhere)
		{
			case 1:
				sprintf(sTitle,"ÊÕ¿î×Ü½ð¶î:(%.2lf)",dZk);
				ShowDatatableInfo(pdt,sTitle,0);
				break;
            
			default:
				ShowDatatableInfo(pdt,"0.Î´ÊÕ¿î    1.ÒÑÊÕ¿î",0);
				break;
		}
		//Fetch data into container
		nKey=FetchData(pdt,nCurrent,nwhere,"opt,njsbz");

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
					nKey=MessageBox("ÄúµÄÐÞ¸ÄÃ»ÓÐ±£´æ£¬Òª±£´æÂð¿",0);
					if(nKey==KEY_ESC)
					{
						ShowDataTable(pdt,3,3);
						continue;
					}
					if(nKey==KEY_YES)
					{
						nModifyFlag=SaveAirTiketPay(pdt,sJym);
					}
				}
				DropDataTable(pdt);
				return;
			default:
				DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
				continue;
		}

		ExportData(pdt,nCurrent,"opt,njsbz,dsjk",sOpt,&nJsbz,&dSjk);
		ImportData(pdt,nCurrent,"opt","+");
		switch(nwhere)
		{
			case 1: //²Ù×÷·½·¨
				switch(sOpt[0])
				{
					case 's':   //save as temporary record
					case 'S':
						if(nModifyFlag)
						{
							nModifyFlag=SaveAirTiketPay(pdt,sJym);
						}
						continue;
					case 'p':
					case 'P':
						if(keyb()==KEY_RETURN)  printAirAccount(pdt);
						continue;
					default:
						DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nwhere,&nModifyFlag);
						continue;
				}
				break;
			case 2:
				nModifyFlag=1;
				if(nJsbz!=0 && nJsbz!=1) continue;
				nwhere=1;
				break;
		}//end switch(nwhere);
		ImportData(pdt,nCurrent,"njsbz",nJsbz);

		if(nCurrent<pdt->nCount)
			nCurrent++;
		
   }//end for(;;)

}

static long SaveAirTiketPay(DATATABLE * pdt,char * sJym)
{
	long nRetVal,nId,i;
	long njsbz=0;
	char inbuf[4096],inbuf1[4096],sResult[200];
	char sAirId[6]=".";

	ShowDatatableInfo(pdt,"ÕýÔÚÈë¿â£¬ÇëÉÔºò...",0);
    
	memset(inbuf,'\0',strlen(inbuf));
	memset(inbuf1,'\0',strlen(inbuf));
	sprintf(inbuf1,"%s|%ld|",sJym,pdt->nCount);
	
	for(nId=1;nId<=pdt->nCount;nId++)
    {
    	ExportData(pdt,nId,"cid,njsbz",sAirId,&njsbz);
    	
    	sprintf(inbuf,"%s%s|%ld|",inbuf1,sAirId,njsbz);
        strcpy(inbuf1,inbuf);      
    }
    
    nRetVal=AutoRunTask(sSysDlDz,TASK_SAVE_TICKET_PAY_RESULT,"%s","%s",inbuf,sResult);
    ShowDatatableInfo(pdt,sResult,1);
    if(nRetVal)
    	return -1;
    else 
    	return 0;
}

static void printAirAccount(DATATABLE *pdt)
{
  long nId,nRetVal;
  double dSjk,dZk=0;
  long nJsbz=0;
  char sName[20],sPhone[20],sId[6],sLsh[41];
   
    setprinton();
    output("|Êµ½É¿î      |ÐÕÃû        |µç»°                |±êÖ¾|\n");
    for(nId=1;nId<=pdt->nCount;nId++)
    {
		ExportData(pdt,nId,"dsjk,cname,cphone,cid,njsbz",
  	               &dSjk,sName,sPhone,sId,&nJsbz);
    	if(sLsh[0]=='.') continue;
		output("[%12.2lf][%12.12s][%20.20s][%-ld]\n",dSjk,sName,sPhone,sId,nJsbz);
    }
    setprintoff();
    return;
}


//»úÆ±ËÍ´ï
void AirTicketDeliver(long number,char * item,char *sJym)
{
	DATATABLE * pdt;
	RECORD rec;
	long i,nRetVal,nJsbz,nId;
	double dSjk,dZk=0;
	char sName[20],sPhone[20],sId[6],sLsh[41];
	char sFromCity[21]=".",sDestCity[21]=".",sAirLine[21]=".",sSdpl[7]=" ";
    long nBz,nSlkrq=nSysLkrq,nElkrq,nSdpl;
	char sTitle[100],sOpt[2];
	long nKey,nCurrent,nwhere,nModifyFlag;
	char inbuf[4096],outbuf[1024]=".";
   
	clrscr();
	GetSysLkrq();
	
	nSlkrq=nSysLkrq;
	if(GetValue(2,3,"ÇëÊäÈëÆðÊ¼ÈÕÆÚ:%-8ld",&nSlkrq)==KEY_ESC) return;
	nElkrq=nSysLkrq;
	if(GetValue(2,5,"ÇëÊäÈë½ØÖ¹ÈÕÆÚ:%-8ld",&nElkrq)==KEY_ESC) return;
      
	clrscr();
	sprintf(sTitle,"[%ld--%ld] %s»úÆ±ËÍ´ï´¦Àí",nSlkrq,nElkrq,sSysJsmc);
	outtext("\n            %s",sTitle);

    pdt=CreateDataTable(8,
    "|  |ÂÃ¿ÍÐÕÃû  |º½¶Î          |ÁªÏµµç»°        |Æ±ºÅ  |ËÍ´ïÆ±Àà|",
    "%-1.1s%-8.8s¡¡%-14.14s      %-16.16s        %-5.5s  %-6s");
    if(!pdt)
    {
		MessageBox("½¨Á¢Êý¾Ý±í¸ñ³ö´í...",1);
		return;
	}
    AssignTableFields(pdt,"opt,cname,chd,cphone,cid,csdpl");
    SetTableStyle(pdt,ST_ALLTRIM|ST_MESSAGELINE|ST_UPDOWN,1);
    
	ShowDatatableInfo(pdt,"ÕýÔÚÌáÈ¡Êý¾Ý£¬ÇëµÈ´ý...",0);

	sprintf(inbuf,"%s|%d|%d|%s|",sJym,nSlkrq,nElkrq,sSysJsdh);
	nRetVal=AutoRunTask(sSysDlDz,TASK_LOAD_DELIVER_TICKET,"%s","%ld%r",inbuf,&nBz,&rec);
	if(nRetVal)	return;
	if(nBz==-1)
	{
		ShowDatatableInfo(pdt,"ÌáÈ¡Êý¾Ý³ö´í£¡",1);
		return;
	}
	
	for(i=0;;i++)
	{
		if(ExportRecord(&rec,"%ld%s%s%s%s%s",
			&nSdpl,sName,sId,sFromCity,sDestCity,sPhone)<=0)	break;

 		if(strlen(sName)>10) sName[10]='\0';
 		
 		sprintf(sAirLine,"%s-%s",sFromCity,sDestCity);
		if(strlen(sAirLine)>13) sAirLine[13]='\0';
 		
		switch(nSdpl)
		{
			case 1:
				strcpy(sSdpl,"µÇ»úÒ×");
				break;
			case 2:
				strcpy(sSdpl,"»úÆ±");
				break;
			default:
				strcpy(sSdpl,".");
				break;
		}
		
		ImportData(pdt,pdt->nCount+1,"opt,cname,chd,cphone,cid,csdpl",
  	               "+",sName,sAirLine,sPhone,sId,sSdpl);
	}
	DropRecord(&rec);
   
	if(i==0)
	{
		ShowDatatableInfo(pdt,"¸ÃÈÕÆÚ¶ÎÄÚÃ»ÓÐ¶©Æ±£¡ÈÎÒ»½¡·µ»Ø...",1);
		DropDataTable(pdt);
		return;
	}
	

	//begin input
	nCurrent=1;//start with the first line
	nwhere=1;
	nModifyFlag=0;//the document is modified if the flag is nozero
	ShowDataTable(pdt,5,3);
    
	for(;;)
	{
		switch(nwhere)
		{
			case 1:
				sprintf(sTitle,"M.¿ØÖÆ°ïÖú...");
				ShowDatatableInfo(pdt,sTitle,0);
				break;
            
			default:
				ShowDatatableInfo(pdt,"1.µÇ»úÒ×    2.»úÆ±",0);
				break;
		}
		//Fetch data into container
		nKey=FetchData(pdt,nCurrent,nwhere,"opt,csdpl");

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
					nKey=MessageBox("ÄúµÄÐÞ¸ÄÃ»ÓÐ±£´æ£¬Òª±£´æÂð¿",0);
					if(nKey==KEY_ESC)
					{
						ShowDataTable(pdt,5,3);
						continue;
					}
					if(nKey==KEY_YES)
					{
						nModifyFlag=SaveAirTiketDeliver(pdt,sJym);
					}
				}
				DropDataTable(pdt);
				return;
			default:
				DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
				continue;
		}

		ExportData(pdt,nCurrent,"opt,csdpl",sOpt,sSdpl);
		ImportData(pdt,nCurrent,"opt","+");
		switch(nwhere)
		{
			case 1: //²Ù×÷·½·¨
				switch(sOpt[0])
				{
					case 's':   //save as temporary record
					case 'S':
						if(nModifyFlag)
						{
							nModifyFlag=SaveAirTiketDeliver(pdt,sJym);
						}
						continue;
					default:
						DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nwhere,&nModifyFlag);
						continue;
				}
				break;
			case 2:
				nModifyFlag=1;
				alltrim(sSdpl);
				if(strcmp(sSdpl,"µÇ»úÒ×")!=0&&strcmp(sSdpl,"»úÆ±")!=0&&strcmp(sSdpl,".")!=0
						&&strcmp(sSdpl,"1")!=0&&strcmp(sSdpl,"2")!=0)
					continue;
				if(strcmp(sSdpl,"1")==0) 
					strcpy(sSdpl,"µÇ»úÒ×");
				if(strcmp(sSdpl,"2")==0) 
					strcpy(sSdpl,"»úÆ±"); 
				nwhere=1;
				break;
		}//end switch(nwhere);
		ImportData(pdt,nCurrent,"csdpl",sSdpl);

		if(nCurrent<pdt->nCount)
			nCurrent++;
		
   }//end for(;;)
}

static long SaveAirTiketDeliver(DATATABLE * pdt,char * sJym)
{
	long nRetVal,nId,i;
	long nSdpl=0;
	char inbuf[4096],inbuf1[4096],sResult[200];
	char sAirId[6]=".",sSdpl[7]=".";

	ShowDatatableInfo(pdt,"ÕýÔÚÈë¿â£¬ÇëÉÔºò...",0);
    
	memset(inbuf,'\0',strlen(inbuf));
	memset(inbuf1,'\0',strlen(inbuf));
	sprintf(inbuf1,"%s|%ld|",sJym,pdt->nCount);
	
	for(nId=1;nId<=pdt->nCount;nId++)
    {
    	ExportData(pdt,nId,"cid,csdpl",sAirId,sSdpl);
    	
    	nSdpl=0;
    	if(strcmp(sSdpl,"µÇ»úÒ×")==0)
    		nSdpl=1;
	    else if(strcmp(sSdpl,"»úÆ±")==0)
    		nSdpl=2;
    		
   	   	sprintf(inbuf,"%s%s|%ld|",inbuf1,sAirId,nSdpl);
        strcpy(inbuf1,inbuf);
    }
    
    nRetVal=AutoRunTask(sSysDlDz,TASK_SAVE_TICKET_DELIVER_RESULT,"%s","%s",inbuf,sResult);
    ShowDatatableInfo(pdt,sResult,1);
    if(nRetVal) return -1;
    else return 0;

}
   
//ÍËÆ±ÉêÇë
void AirReturnTicketBeg(long number,char * item,char *sJym)
{
	DIALOG * pd_tp;
	RECORD rec;
	
	long i,nRetVal,nId;
	long nBz,nKey,nWhered;
	long nDate=0,nDate1=0,nDate2=0,nTime=0,nCsbz=0,nJsbz=0;	
	char sName[20]=".",sPhone[20]=".",sAirId[6]=".",sManId[41]=".";
	char sFromCity[21]=".",sDestCity[21]=".",sAirLine[31]=".";
	char sInfo[46],inbuf[1024];
	
	double dPrice=0,dRePrice=0,dRebat=0,dReBate=0;
	
	clrscr();
	GetSysLkrq();
	
	//½¨Á¢±ä¶¯ÀàÐÍ¶Ô»°¿ò
	pd_tp=CreateDialog(50,19,FRAME);
	if(pd_tp==NULL)
	{
		clearline(24);gotoxy(30,24);outtext("½¨Á¢ÍËÆ±¶Ô»°¿òÊ§°Ü!");keyb();
		return;
	}
	
	AddDialog(pd_tp,NULL,STATIC_TEXT,"                ÍËÆ±ÐÅÏ¢Â¼Èë\n");
	AddDialog(pd_tp,NULL,SPACELINE,1);
	AddDialog(pd_tp,"airid",INPUT,"  »úÆ±±àÂë:%5s\n",sAirId);
	AddDialog(pd_tp,NULL,SPACELINE,1);
	AddDialog(pd_tp,"airline",OUTPUT,"  º½Ïß:%-16.16s",sAirLine);
	AddDialog(pd_tp,"price",OUTPUT,"  Æ±¼Û:%.2f\n",&dPrice);
	AddDialog(pd_tp,NULL,SPACELINE,1);
	AddDialog(pd_tp,"date",OUTPUT,"  Æð·ÉÈÕÆÚ:%8d",&nDate);
	AddDialog(pd_tp,"time",OUTPUT,"      Æð·ÉÊ±¼ä:%4d\n",&nTime);
	AddDialog(pd_tp,NULL,SPACELINE,1);
	AddDialog(pd_tp,"name",OUTPUT,"  ÂÃ¿ÍÐÕÃû:%-20.20s\n",sName);
	AddDialog(pd_tp,NULL,SPACELINE,1);
	AddDialog(pd_tp,"manid",OUTPUT,"  Éí·ÝÖ¤ºÅÂë:%-20.20s\n",sManId);
	AddDialog(pd_tp,NULL,SPACELINE,1);
	AddDialog(pd_tp,"phone",OUTPUT,"  µç»°ºÅÂë:%-20.20s\n",sPhone);
	AddDialog(pd_tp,NULL,SPACELINE,1);
	AddDialog(pd_tp,"reprice",INPUT,"  ÍËÆ±¼Û:%-8.2f",&dRePrice);
	AddDialog(pd_tp,"rebate",INPUT,"   ÍËÆ±ÂÊ:%-6.2f",&dReBate);
	
	
	ShowDialog(pd_tp,14,3);
	
	nBz=0;nWhered=1;
	for(;;)
	{
		RefreshDialog(pd_tp,0);
		switch(nWhered)
		{
			case 1:
				SetDialogInfo(pd_tp,"       ESC.ÍË³ö");
				break;
			default:
				GetSysLkrq();
 				sprintf(sInfo," ÏÖÔÚÈÕÆÚ:%ld Ê±¼ä:%ld  (F1.¹æÔò ESC.ÍË³ö)",nSysLkrq,nSysSfm/100);
 				SetDialogInfo(pd_tp,sInfo);
 				break;
		}
		
		nKey=ActiveDialog(pd_tp,nWhered,"airid,reprice,rebate");
		switch(nKey)
		{
			case KEY_PAGEUP:
			case KEY_LEFT:
			case KEY_UP:
				if(nWhered>1) nWhered--;
				continue;
			case KEY_F1:
				savewin();
				ShowHelp("tpgz.hlp",NULL);
				popwin();
				continue;
			case KEY_ESC:
				return;
		}//end switch(nKey)
			
		switch(nWhered)
		{
			case 1:
				alltrim(sAirId);
				upper(sAirId);
				if(strlen(sAirId)<5)
					continue;
				
				SetDialogInfo(pd_tp,"      ÕýÔÚÌáÈ¡Êý¾Ý£¬ÇëµÈ´ý...");
			
				sprintf(inbuf,"%s|%s|%s|%s|",sJym,sAirId,sSysJsdh,sSysCzydh);
				nRetVal=AutoRunTask(sSysDlDz,TASK_LOAD_RETURN_TICKET_BEG,
												"%s","%ld%r",inbuf,&nBz,&rec);
				if(nRetVal)	return;
				if(nBz==-1)
				{
					SetDialogInfo(pd_tp,"ÌáÈ¡Êý¾Ý³ö´í...");keyb();
					return;
				}
				if(nBz==0)
				{
					SetDialogInfo(pd_tp,"¹ó¾ÖÃ»ÓÐ¸Ã±àÂëµÄ»úÆ±ÐÅÏ¢!");keyb();
					continue;
				}
	
				if(ExportRecord(&rec,"%s%s%lf%ld%ld%s%s%s%ld%ld%ld%ld%lf",
						sFromCity,sDestCity,&dPrice,&nDate,&nTime,
						sName,sManId,sPhone,&nCsbz,&nJsbz,&nDate1,&nDate2,&dRebat)<=0)
				{
					SetDialogInfo(pd_tp,"ÌáÈ¡Êý¾Ý³ö´í...");keyb();
					return;
				}
 				sprintf(sAirLine,"%s-%s",sFromCity,sDestCity);
 				
 				if(nCsbz!=2)
 				{
 					SetDialogInfo(pd_tp,"    ¸Ã»úÆ±Î´±»È·¶¨´òÓ¡³É¹¦!ÇëÓëÖÐÐÄÁªÏµ...");keyb();
					continue;
				}
 				if(nJsbz!=1)
 				{
 					SetDialogInfo(pd_tp,"    Çë×¢Òâ,¸Ã»úÆ±Î´±»ÊÕ¿î...");
 					keyb();
				}
 				if(dRebat<5.0)
 				{
 					SetDialogInfo(pd_tp,"   ²»µÃÇ©×ªÔ­³öÆ±µØÍËÆ±...");
 					keyb();
 				}
 				if(nDate1!=0)
 				{
 					if(nDate2==0)
 					{
 						SetDialogInfo(pd_tp,"    ¸Ã»úÆ±ÒÑ±»ÍËÆ±ÉêÇë,ÊÇ·ñÈ¡Ïû?(y/n)");
 						nKey=GetYn();
						if(nKey==KEY_NO)
							continue;
						
						GetSysLkrq();
						sprintf(inbuf,"%s|%s|%s|",
							sJym,sAirId,sSysCzydh);
						nRetVal=AutoRunTask(sSysDlDz,TASK_SAVE_CANCEL_RETURN_TICKET_BEG,
												"%s","%ld%s",inbuf,&nBz,sInfo);
						if(nRetVal)	return;
						
						SetDialogInfo(pd_tp,sInfo);
 						keyb();
 						continue;
					}
					else
					{
						SetDialogInfo(pd_tp,"    ¸Ã»úÆ±ÒÑ×öÁËÍËÆ±´¦Àí!");
 						keyb();
 						continue;
 					}
				}
 				nWhered++;
 				break;
 			case 2:
 				if(dRePrice>dPrice||dRePrice<0.01)
 					continue;
 				SetDialogInfo(pd_tp,"    ÍËÆ±¼Û¸ñÊÇ·ñÒÑ×ÐÏ¸ºË¶Ô?(y/n)");
 				nKey=GetYn();
				if(nKey==KEY_NO)
				{
					nWhered=1;
					continue;
				}

				GetSysLkrq();
				sprintf(inbuf,"%s|%s|%d|%d|%f|%f|%s|",
							sJym,sAirId,nSysLkrq,nSysSfm/100,dPrice,dRePrice,sSysCzydh);
				nRetVal=AutoRunTask(sSysDlDz,TASK_SAVE_RETURN_TICKET_BEG,
												"%s","%ld%s",inbuf,&nBz,sInfo);
				if(nRetVal)
				{
					nWhered++;
					continue;
				}
				SetDialogInfo(pd_tp,sInfo);keyb();
				break;
			case 3:
			if(dReBate>=0) 
			{
				alltrim(sAirId);
				if(RunSql("update dl$dbmx_%t set dvalue6=%lf where cvalue6=%s ",
						sJym,dReBate,sAirId)>0) CommitWork();

				break;
			}
			continue;
 		}
	}//end for(;;)
}

//»úÆ±ÍËÆ±È·Õý
void AirDoReturnTicketBeg(long number,char * item,char *sJym)
{
	DATATABLE * pdt;
	RECORD rec;
	long i,nRetVal,nJsbz,nId,nBz;
	long nKey,nCurrent,nwhere;
	long nDate=0,nDate1=0,nDate2=0;
	long nTime=0,nTime1=0,nTime2=0;
	
	char sName[20]=".",sAirId[5]=".";
	char sFromCity[21]=".",sDestCity[21]=".",sAirLine[21]=".",sDo[5]=".";
    char sTitle[100],sOpt[2];
	char inbuf[4096],sResult[1024]=".";
   
	double dPrice=0,dRePrice=0;
	
	clrscr();
	GetSysLkrq();

	outtext("\n                    ¡¡ÍËÆ±ÉêÇë´¦Àí");

    pdt=CreateDataTable(8,
    "|  |´¦Àí|Æ±ºÅ  |ÂÃ¿ÍÐÕÃû  |º½¶Î          |Æð·ÉÈÕÆÚ|Æð·ÉÊ±¼ä|¹ºÆ±¼Û|ÉêÇëÈÕÆÚ|Ê±¼ä|ÍËÆ±¼Û|",
    "%-1.1s%-4s%-5.5s%-8.8s¡¡  %-14.14s      	 %-8.8d    %-4d  ¡¡%-6.1f %-8.8d   %-4d  %-6.1f %d%d");
    if(!pdt)
    {
		MessageBox("½¨Á¢Êý¾Ý±í¸ñ³ö´í...",1);
		return;
	}
    AssignTableFields(pdt,"opt,cdo,cid,cname,chd,ndate,ntime,dprice,ndate1,ntime1,dreprice,ndate2,ntime2");
    SetTableStyle(pdt,ST_ALLTRIM|ST_MESSAGELINE|ST_UPDOWN,1);
    
	ShowDatatableInfo(pdt,"ÕýÔÚÌáÈ¡Êý¾Ý£¬ÇëµÈ´ý...",0);

	sprintf(inbuf,"%s|",sJym);
	nRetVal=AutoRunTask(sSysDlDz,TASK_LOAD_DO_RETURN_TICKET_BEG,"%s","%ld%r",inbuf,&nBz,&rec);
	if(nRetVal)	return;
	if(nBz==-1)
	{
		ShowDatatableInfo(pdt,"ÌáÈ¡Êý¾Ý³ö´í£¡",1);
		return;
	}
	
	for(i=0;;i++)
	{
		if(ExportRecord(&rec,"%s%s%s%s%ld%ld%lf%ld%ld%lf",
				sAirId,sName,sFromCity,sDestCity,
				&nDate,&nTime,&dPrice,
				&nDate1,&nTime1,&dRePrice)<=0)	break;

 		if(strlen(sName)>10) sName[10]='\0';
 		
 		sprintf(sAirLine,"%s-%s",sFromCity,sDestCity);
		if(strlen(sAirLine)>13) sAirLine[13]='\0';
 		
		ImportData(pdt,pdt->nCount+1,
				"opt,cdo,cid,cname,chd,ndate,ntime,dprice,ndate1,ntime1,dreprice,ndate2,ntime2",
  	            "+",".",sAirId,sName,sAirLine,
  	            nDate,nTime,dPrice,
				nDate1,nTime1,dRePrice,0,0);
	}
	DropRecord(&rec);
   
	if(i==0)
	{
		ShowDatatableInfo(pdt,"Ã»ÓÐÍËÆ±ÉêÇë£¡ÈÎÒ»½¡·µ»Ø...",1);
		DropDataTable(pdt);
		return;
	}
	

	//begin input
	nCurrent=1;//start with the first line
	nwhere=1;
	ShowDataTable(pdt,3,3);
    
	for(;;)
	{
		switch(nwhere)
		{
			case 1:
				nDate2=0;nTime2=0;
				ExportData(pdt,nCurrent,"ndate2,ntime2",&nDate2,&nTime2);
					if(nDate2==0)
				{
					sprintf(sTitle,"M.¿ØÖÆ°ïÖú...");
					ShowDatatableInfo(pdt,sTitle,0);
				}
				else
				{
					sprintf(sTitle,"ÍËÆ±´¦ÀíÈÕÆÚ:%ld Ê±¼ä:%ld",nDate2,nTime2);
					ShowDatatableInfo(pdt,sTitle,0);
				}
				break;
            
			default:
				ShowDatatableInfo(pdt,"n.·ÅÆú    y.ÍËÆ±",0);
				break;
		}
		//Fetch data into container
		nKey=FetchData(pdt,nCurrent,nwhere,"opt,cdo");

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
				DropDataTable(pdt);
				return;
			default:
				DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
				continue;
		}

		ExportData(pdt,nCurrent,"opt,cdo,cid,ndate2,ntime2",sOpt,sDo,sAirId,&nDate2,&nTime2);
		ImportData(pdt,nCurrent,"opt","+");
		switch(nwhere)
		{
			case 1: //²Ù×÷·½·¨
				switch(sOpt[0])
				{
					default:
						DefaultTableChoice(pdt,sOpt[0],&nCurrent,&nwhere,0);
						continue;
				}
				break;
			case 2:
				alltrim(sDo);
				if(strcmp(sDo,".")==0)
				{
					nwhere=1;
					break;
				}
				if(strcmp(sDo,"y")==0||strcmp(sDo,"Y")==0)
				{
					GetSysLkrq();
					nDate2=nSysLkrq;
					nTime2=nSysSfm/100;
					strcpy(sDo,"ÒÑÍË");
					
					sprintf(inbuf,"%s|%s|%d|%d|%s|",sJym,sAirId,nDate2,nTime2,sSysCzydh);
        			nRetVal=AutoRunTask(sSysDlDz,TASK_SAVE_DO_RETURN_TICKET_RESULT,
        									"%s","%ld%s",inbuf,&nBz,sResult);
					if(nRetVal) return;
					if(nBz<0)
					{
						nDate2=0;
						nTime2=0;
					}
					ShowDatatableInfo(pdt,sResult,1);
					break;
				}
				if(strcmp(sDo,"n")==0||strcmp(sDo,"N")==0)
				{
					strcpy(sDo,".");
					nDate2=0;nTime2=0;
					ExportData(pdt,nCurrent,"ndate2,ntime2",&nDate2,&nTime2);
					if(nDate2>0)
					{
						ShowDatatableInfo(pdt,"È·¶¨Òª³·ÏúÍËÆ±ÉêÇëµÄ´¦Àí(y/n)",0);
						nKey=GetYn();
						if(nKey==KEY_NO)
							continue;
						
						sprintf(inbuf,"%s|%s|",sJym,sAirId);
        				nRetVal=AutoRunTask(sSysDlDz,TASK_SAVE_REDO_RETURN_TICKET_RESULT,
        									"%s","%ld%s",inbuf,&nBz,sResult);
						if(nRetVal) return;
						if(nBz>0)
						{
							nDate2=0;
							nTime2=0;
						}
						strcpy(sDo,".");
						ShowDatatableInfo(pdt,sResult,1);
						break;
					}
					else
						break;
				}
				continue;
		}//end switch(nwhere);
		ImportData(pdt,nCurrent,"cdo,ndate2,ntime2",sDo,nDate2,nTime2);
		
		if(nCurrent<pdt->nCount)
			nCurrent++;
		
   }//end for(;;)
}

void CheckPurTicket(long nId,char * item,char * sJym)
{
	long i,nKey,ncount=0,nBz=0,nRetVal=0;
	long nSp=0,nDy=0,nHk=0,nTp=0,nDd=0;
	char inbuf[1024]=".",sState[2]=".";
	
	clrscr();
	drawbox(12,2,68,22);
	
	//¼à¿Ø×´Ì¬ÖÃÎ»
	if(RunSql("update dl$jymcsfb set ccs='n' where cjym=%s and nbh=1",sJym)>0)
		CommitWork();
	
	while(1)
	{
		RunSelect("select ccs from dl$jymcsfb "
						"where cjym=%s and nbh=1 into %s",
						sJym,sState);
		if(strcmp(sState,"y")==0||strcmp(sState,"Y")==0)
			return;

		gotoxy(34,4);
		outtext("È«Ê¡¶©Æ±Çé¿ö¼à¿Ø");

		sprintf(inbuf,"%s|",sJym);
		nRetVal=AutoRunTask(sSysDlDz,TASK_LOAD_WATCH_TICKET,
						"%s","%ld%ld%ld%ld%ld%ld",inbuf,&nBz,&nSp,&nDy,&nHk,&nTp,&nDd);
		if(nRetVal)	return;
		if(nBz<0)
		{
			gotoxy(20,12),printf("ÌáÈ¡¼à¿ØÊý¾Ý³ö´í!");
			keyb();
			clrscr();
			return;
		}

		gotoxy(26,8);outtext("Ëø  Æ±  Ìõ  Êý:%ld      ",nSp);
		gotoxy(26,10);outtext("Î´¼ì²é»úÆ±´òÓ¡ÌõÊý:%ld      ",nDy);
		gotoxy(26,12);outtext("Î´ÊÕ¿î»úÆ±ÌõÊý:%ld      ",nHk);
		gotoxy(26,14);outtext("Î´´¦ÀíÍËÆ±ÌõÊý:%ld      ",nTp);
		gotoxy(26,16);outtext("»úÆ±¶©µ¥ÌõÊý[×¢Òâ]:%ld    ",nDd);

		for(i=0;i<30;i++)
		{
			gotoxy(38,20);
			outtext("%ld  ",30-i);
			sleep(1);
		}
	}
}
//²éÑ¯ËøÆ±²éÑ¯ TASK_PUBLIC_QUERY
void QueryAirSp(long number,char * item,char *sJym)
{
	QUERYTABLE * pqt;
	char sBuffer[1024];
	char sWhere[1000],sTitle[100];
	char sRet[8],sQueryCzydh[7]=".";
	long nCount,iLoop,nQsrq=nSysLkrq,nJzrq=nSysLkrq;
	long nValue,nId,nRetVal,nStime,nSdate;
    double dSfmx,dZk,dSjk;
	char sYhbz[8],sJh[11],sCzydh[12],sJm[21];
    char sDate[12],sName[20],sHd[21];
	RECORD   record;
	
	memset(sBuffer,'\0',1024);
	
   clrscr();
   if(GetValue(2,2,"ÇëÊäÈë²éÑ¯Ê±¶Î£º%8d",&nQsrq)==KEY_ESC) return;
   if(GetValue(26,2," ÖÁ %8d",&nJzrq)==KEY_ESC) return;
   clearline(2);

    sprintf(sTitle,"                 ËøÆ±Çé¿ö²éÑ¯ (²éÑ¯Ê±¶Î:%ldÖÁ%ld)",nQsrq,nJzrq);
	pqt=CreateQueryTable(sTitle,
                         "Æ±±êÊ¶  ¾ÖÃû                  ²Ù×÷Ô±      Æ±¿î        Æð·ÉÈÕÆÚ  Æð·ÉÊ±¼ä  ÂÃ¿ÍÐÕÃû      º½¶Î              ",
                         "%-6.6s %-20.20s %-10.10s %-12.2lf %8ld %8ld %-12.12s %-16.16s",ST_LASTLINE|ST_VISUAL);
    if(!pqt) return;
    clrscr();

	sprintf(sWhere," nlkrq between %ld and %ld and cjym='%s'",nQsrq,nJzrq,sJym);
	if(nSysCzyqx==3) strcpy(sQueryCzydh,sSysCzydh);
    
	if(sQueryCzydh[0]!='.')
      sprintf(sWhere+strlen(sWhere)," and cczydh='%s'",sQueryCzydh);
    if(strstr(sSysJsdh,"272201") && nSysCzyqx!=3) ;//·Ç³É¶¼¾ÖµÄÒµÎñ²Ù×÷Ô±
	else  sprintf(sWhere+strlen(sWhere)," and cjh='%s' ",sSysJsdh);
	
    sprintf(sBuffer,"%s|%ld|%s|",sJym,1002,sWhere);
    
    if(AutoRunTask(sSysDlDz,TASK_PUBLIC_QUERY,"%s","%r",sBuffer,&record)<0)
    {      
         HintError(TASK_ERR,"²éÑ¯ÓÃ»§ÐÅÏ¢ÍøÂç³ö´í!"); 
    	 return;
    }
    
    ExportRecord(&record,"%s",sRet);
   	alltrim(sRet);
   	if(strcmp(sRet,"000000")==0)
   	{
    	ExportRecord(&record,"%ld%lf",&nCount,&dZk);
     	ImportLastLine(pqt,".",".",".",dZk,0,0,".",".");
     	for(iLoop=0;iLoop<nCount;iLoop++)
     	{
          	ExportRecord(&record,"%lf%s(5)%ld(2)",
          	          		&dSfmx,sJm,sCzydh,sName,sHd,sYhbz,&nSdate,&nStime);
          	
          	/*if(!strcmp(sYhbz,"0")) strcpy(sDate,"ËøÆ±Î´³ö");
          	if(!strcmp(sYhbz,"*")) strcpy(sDate,"ËøÆ±È¡Ïû");
          	else strcpy(sDate,"Õý³£³öÆ±");*/

          	ImportQueryTable(pqt,1,sYhbz,sJm,sCzydh,dSfmx,nSdate,nStime,sName,sHd);
     	}
     	DropRecord(&record);
     	ImportQueryTable(pqt,0);
   	}
   	else
   	{
      ExportRecord(&record,"%s",sTcpipErrInfo);
      DropRecord(&record);
      gotoxy(1,23);
      outtext("    %-70.70s",sTcpipErrInfo);
      keyb();
      return;
   	}
   	DropQueryTable(pqt);
}

void QueryAirSd(long number,char * item,char *sJym)
{
	QUERYTABLE * pqt;
	char sBuffer[1024];
	char sWhere[1000],sTitle[100];
	char sRet[8],sQueryCzydh[7]=".";
	long nCount,iLoop,nQsrq=nSysLkrq,nJzrq=nSysLkrq;
	long nValue,nId,nRetVal,nValue9,nValue8;
    double dSfmx,dZk,dSjk;
	char sYhbz[8],sJh[11],sCzydh[12],sJm[21],sName[18],sHd[21];
    char sDate[14];
	RECORD   record;
	
	memset(sBuffer,'\0',1024);
	
   clrscr();
   if(GetValue(2,2,"ÇëÊäÈë²éÑ¯Ê±¶Î£º%8d",&nQsrq)==KEY_ESC) return;
   if(GetValue(26,2," ÖÁ %8d",&nJzrq)==KEY_ESC) return;
   clearline(2);

    sprintf(sTitle,"                 ËÍ´ïÇé¿ö²éÑ¯ (²éÑ¯Ê±¶Î:%ldÖÁ%ld)",nQsrq,nJzrq);
	pqt=CreateQueryTable(sTitle,
                         "¾ÖÃû                  ±êÆ±Ê¶  ²Ù×÷Ô±      Æ±¿î        ËÍ´ïÊÕ¿î×´Ì¬  ÐÕÃû        º½¶Î              ",
                         "%-20.20s %-6.6s %-10.10s %-12.2lf %-12.12s %-10.10s %-16.16s",ST_LASTLINE|ST_VISUAL);
    if(!pqt) return;
    clrscr();

	sprintf(sWhere," nlkrq between %ld and %ld and cjym='%s'",nQsrq,nJzrq,sJym);
	if(nSysCzyqx==3) strcpy(sQueryCzydh,sSysCzydh);
    
	if(sQueryCzydh[0]!='.')
      sprintf(sWhere+strlen(sWhere)," and cczydh='%s'",sQueryCzydh);
    if(strstr(sSysJsdh,"272201") && nSysCzyqx!=3) ;//·Ç³É¶¼¾ÖµÄÒµÎñ²Ù×÷Ô±
	else sprintf(sWhere+strlen(sWhere)," and cjh='%s' ",sSysJsdh);
	
	sprintf(sBuffer,"%s|%ld|%s|",sJym,1004,sWhere);
    
    if(AutoRunTask(sSysDlDz,TASK_PUBLIC_QUERY,"%s","%r",sBuffer,&record)<0)
    {      
         HintError(TASK_ERR,"²éÑ¯ÓÃ»§ÐÅÏ¢ÍøÂç³ö´í!"); 
    	 return;
    }
    
    ExportRecord(&record,"%s",sRet);
   	alltrim(sRet);
   	if(strcmp(sRet,"000000")==0)
   	{
    	ExportRecord(&record,"%ld%lf",&nCount,&dZk);
     	ImportLastLine(pqt,".",".",".",dZk,".",".",".");
     	for(iLoop=0;iLoop<nCount;iLoop++)
     	{
          	ExportRecord(&record,"%lf%ld(2)%s(5)",
          	          		&dSfmx,&nValue9,&nValue8,sYhbz,sJm,sCzydh,sName,sHd);

			if(nValue8==1) strcpy(sDate,"µÇ»úÒ×");
			else if(nValue8==2) strcpy(sDate,"»úÆ±");
			else strcpy(sDate,"Î´ËÍ");
			if(nValue9) strcat(sDate,",ÊÕ¿î");
          	
          	ImportQueryTable(pqt,1,sJm,sYhbz,sCzydh,dSfmx,sDate,sName,sHd);
     	}
     	DropRecord(&record);
     	ImportQueryTable(pqt,0);
   	}
   	else
   	{
      ExportRecord(&record,"%s",sTcpipErrInfo);
      DropRecord(&record);
      gotoxy(1,23);
      outtext("    %-70.70s",sTcpipErrInfo);
      keyb();
      return;
   	}
   	DropQueryTable(pqt);
}

void QueryAirTp(long number,char * item,char *sJym)
{
	QUERYTABLE * pqt;
	char sBuffer[1024];
	char sWhere[1000],sTitle[100];
	char sRet[8],sQueryCzydh[7]=".";
	long nCount,iLoop,nQsrq=nSysLkrq,nJzrq=nSysLkrq;
	long nValue,nId,nRetVal,nValue5,nValue6,nValue11,nValue12;
    double dSfmx,dZk,dSjk,dReBate=0;
	char sYhbz[8],sJh[11],sCzydh[7],sJm[21],sName[13],sHd[21];
    char sDate[12]=".";
	RECORD   record;
	
	memset(sBuffer,'\0',1024);
	
   clrscr();
   if(GetValue(2,2,"ÇëÊäÈë²éÑ¯Ê±¶Î£º%8d",&nQsrq)==KEY_ESC) return;
   if(GetValue(26,2," ÖÁ %8d",&nJzrq)==KEY_ESC) return;
   clearline(2);

    sprintf(sTitle,"                 ÍËÆ±Çé¿ö²éÑ¯ (²éÑ¯Ê±¶Î:%ldÖÁ%ld)",nQsrq,nJzrq);
	pqt=CreateQueryTable(sTitle,
                         "¾ÖÃû                  Æ±±êÊ¶  ÍËÆ±²Ù×÷Ô±  Æ±¿î        ÍËÆ±ÂÊ  ÐÕÃû        º½¶Î            ÍËÆ±×´Ì¬  ÉêÇëÈÕÆÚ  ÉêÇëÊ±¼ä  ´¦ÀíÈÕÆÚ  ´¦ÀíÊ±¼ä",
                         "%-20.20s %-6.6s %-10.10s %-12.2lf %6.2lf %-10.10s %-14.14s %-10.10s  %8ld %6ld %8ld %6ld",ST_LASTLINE|ST_VISUAL);
    if(!pqt) return;
    clrscr();

	sprintf(sWhere," zw.nlkrq between %ld and %ld and zw.cjym='%s'",nQsrq,nJzrq,sJym);
	if(nSysCzyqx==3) strcpy(sQueryCzydh,sSysCzydh);
    
	if(sQueryCzydh[0]!='.')
      sprintf(sWhere+strlen(sWhere)," and zw.cczydh='%s'",sQueryCzydh);
    if(strstr(sSysJsdh,"272201") && nSysCzyqx!=3) ;//·Ç³É¶¼¾ÖµÄÒµÎñ²Ù×÷Ô±
	else  sprintf(sWhere+strlen(sWhere)," and zw.cjh='%s' ",sSysJsdh);
		
    sprintf(sBuffer,"%s|%ld|%s|",sJym,1006,sWhere);
    
    if(AutoRunTask(sSysDlDz,TASK_PUBLIC_QUERY,"%s","%r",sBuffer,&record)<0)
    {      
         HintError(TASK_ERR,"²éÑ¯ÓÃ»§ÐÅÏ¢ÍøÂç³ö´í!"); 
    	 return;
    }
    
    ExportRecord(&record,"%s",sRet);
   	alltrim(sRet);
   	if(strcmp(sRet,"000000")==0)
   	{
    	ExportRecord(&record,"%ld%lf",&nCount,&dZk);
     	ImportLastLine(pqt,".",".",".",dZk,0,".",".",".",0,0,0,0);
     	for(iLoop=0;iLoop<nCount;iLoop++)
     	{
          	ExportRecord(&record,"%lf%ld(4)%s(5)%lf",
          	  &dSfmx,&nValue5,&nValue6,&nValue11,&nValue12,sYhbz,sJm,sCzydh,sName,sHd,&dReBate);

			if(nValue11) strcpy(sDate,"ÒÑÍËÆ±");
			else	strcpy(sDate,"Çë´¦Àí");
			
			ImportQueryTable(pqt,1,sJm,sYhbz,sCzydh,dSfmx,dReBate,sName,sHd,sDate,nValue5,nValue6,nValue11,nValue12);
     	}
     	DropRecord(&record);
     	ImportQueryTable(pqt,0);
   	}
   	else
   	{
      ExportRecord(&record,"%s",sTcpipErrInfo);
      DropRecord(&record);
      gotoxy(1,23);
      outtext("    %-70.70s",sTcpipErrInfo);
      keyb();
      return;
   	}
   	DropQueryTable(pqt);
}

void BbAirSd(long number,char * item,char *sJym)
{
	QUERYTABLE * pqt;
	char sBuffer[1024];
	char sWhere[1000],sTitle[100];
	char sRet[8],sQueryCzydh[7]=".";
	long nCount,iLoop,nQsrq=nSysLkrq,nJzrq=nSysLkrq;
	long nValue,nId,nRetVal,nValue5,nValue6,nValue11,nValue12;
    double dSfmx,dZk,dSjk;
	char sYhbz[8],sJh[11],sCzydh[7],sJm[21],sName[13],sHd[21];
    char sDate[14];
	
	memset(sBuffer,'\0',1024);
	
   clrscr();
   if(GetValue(2,2,"ÇëÊäÈë²éÑ¯Ê±¶Î£º%8d",&nQsrq)==KEY_ESC) return;
   if(GetValue(26,2," ÖÁ %8d",&nJzrq)==KEY_ESC) return;
   clearline(2);

    sprintf(sTitle,"             Æ±¾ÝËÍ´ïÇé¿ö±¨±í (²éÑ¯Ê±¶Î:%ldÖÁ%ld)",nQsrq,nJzrq);
	pqt=CreateQueryTable(sTitle,
                         "ÂÃ¿ÍÐÕÃû    ±êÆ±Ê¶  Æ±¿î      ËÍ´ïÇé¿ö        ",
                         "%-10.10s %-6.6s %-12.2lf %-12.12s",ST_LASTLINE|ST_VISUAL);
    if(!pqt) return;
    clrscr();

	sprintf(sWhere," nlkrq between %ld and %ld and cjym='%s'",nQsrq,nJzrq,sJym);
	if(nSysCzyqx==3) strcpy(sQueryCzydh,sSysCzydh);
    
	if(sQueryCzydh[0]!='.')
      sprintf(sWhere+strlen(sWhere)," and cczydh='%s'",sQueryCzydh);
    if(strstr(sSysJsdh,"272201") && nSysCzyqx!=3) ;//·Ç³É¶¼¾ÖµÄÒµÎñ²Ù×÷Ô±
	else sprintf(sWhere+strlen(sWhere)," and cjh='%s' ",sSysJsdh);
	
	RunSelect("select sum(nsl),sum(dsjk) from dl$zwmx where %t and cyhbz!='0' and cyhbz!='*' and ncscs!=0 into %ld%lf",sWhere,&nCount,&dZk);
    
	nId=OpenCursor("select cyhmc,cyhbz,dsjk,ncscs from dl$zwmx where %t and cyhbz!='0' and cyhbz!='*' and ncscs!=0",sWhere);
	if(nId<0)
	{
     	GetSqlErrInfo(sWhere);
     	return;
	}
	
	ImportLastLine(pqt,".","ºÏ ¼Æ",dZk,".");
	while(1)
	{
     	nRetVal=FetchCursor(nId,"%s%s%lf%ld",sName,sYhbz,&dSfmx,&nValue);

		if(nValue==1) strcpy(sDate,"»úÆ±ËÍ´ï");
		else if(nValue==2) strcpy(sDate,"µÇ»úÒ×ËÍ´ï");
		
		if(ImportQueryTable(pqt,nRetVal,sName,sYhbz,dSfmx,sDate)<=0)
			break;
	}//end while
    CloseCursor(nId);
   	DropQueryTable(pqt);
}

void BbAirXj(long number,char * item,char *sJym)
{
	QUERYTABLE * pqt;
	char sBuffer[1024];
	char sWhere[1000],sTitle[100];
	char sRet[8],sQueryCzydh[7]=".";
	long nCount,iLoop,nQsrq=nSysLkrq,nJzrq=nSysLkrq;
	long nValue,nId,nRetVal,nValue5,nValue6,nValue11,nValue12;
    	double dSfmx,dZk,dSjk;
	char sYhbz[8],sJh[11],sCzydh[7],sJm[21],sName[21],sHd[21],sLsh[41],sCzymc[21];
    	char sDate[14],sHd1[21],sHd2[21];
	
	memset(sBuffer,'\0',1024);
	
   clrscr();
   if(GetValue(2,2,"ÇëÊäÈë²éÑ¯Ê±¶Î£º%8d",&nQsrq)==KEY_ESC) return;
   if(GetValue(26,2," ÖÁ %8d",&nJzrq)==KEY_ESC) return;
   clearline(2);

    sprintf(sTitle,"          »úÆ±ÏÖ½ðÊÕÖ§±¨±í (²éÑ¯Ê±¶Î:%ldÖÁ%ld)",nQsrq,nJzrq);
    pqt=CreateQueryTable(sTitle,
                         "ÂÃ¿ÍÐÕÃû    Æ±±êÊ¶  Æ±¿î      ²Ù×÷Ô±        º½¶Î             ",
                         "%-10.10s %-6.6s %-12.2lf %-12.12s %-16.16s",ST_LASTLINE|ST_VISUAL);
    if(!pqt) return;
    clrscr();

	sprintf(sWhere," nlkrq between %ld and %ld and cjym='%s'",nQsrq,nJzrq,sJym);
	if(nSysCzyqx==3) strcpy(sQueryCzydh,sSysCzydh);
    
	if(sQueryCzydh[0]!='.')
	{
      sprintf(sWhere+strlen(sWhere)," and cczydh='%s'",sQueryCzydh);
      RunSelect("select cczymc from dl$czyb where cczydh=%s into %s",sCzydh,sCzymc);
    }
    if(strstr(sSysJsdh,"272201") && nSysCzyqx!=3) ;//·Ç³É¶¼¾ÖµÄÒµÎñ²Ù×÷Ô±
	else sprintf(sWhere+strlen(sWhere)," and cjh='%s' ",sSysJsdh);
	
	RunSelect("select sum(nsl),sum(dsjk) from dl$zwmx where %t and cyhbz!='0' and cyhbz!='*' and njsbz!=0 into %ld%lf",sWhere,&nCount,&dZk);
    
	nId=OpenCursor("select cyhmc,cyhbz,dsjk,cczydh,clsh from dl$zwmx where %t and cyhbz!='0' and cyhbz!='*' and njsbz!=0",sWhere);
	if(nId<0)
	{
     	GetSqlErrInfo(sWhere);
     	return;
	}
	
	ImportLastLine(pqt,".","ºÏ ¼Æ",dZk,".",".");
	while(1)
	{
     	nRetVal=FetchCursor(nId,"%s%s%lf%s%s",sName,sYhbz,&dSfmx,sCzydh,sLsh);
		
		RunSelect("select cvalue13,cvalue14 from dl$dbmx_%t where clsh=%s into %s%s",sJym,sLsh,sHd1,sHd2);
        //RunSelect("select cjm from dl$jh where cjh=%s into %s",sJh,sJm);
        if(sQueryCzydh[0]=='.')
        	RunSelect("select cczymc from dl$czyb where cczydh=%s into %s",sCzydh,sCzymc);
		
		sprintf(sHd,"%s-%s",sHd1,sHd2);
		if(strlen(sHd)>15)	sHd[15]='\0';
		if(ImportQueryTable(pqt,nRetVal,sName,sYhbz,dSfmx,sCzymc,sHd)<=0)
			break;
	}//end while
    CloseCursor(nId);
    DropQueryTable(pqt);
}

void QueryAirSpfs(long number,char * item,char *sJym)
{
	QUERYTABLE * pqt;
	char sBuffer[1024];
	char sWhere[1000],sTitle[100];
	char sRet[8],sQueryCzydh[7]=".";
	long nCount,iLoop,nQsrq=nSysLkrq,nJzrq=nSysLkrq;
	long nValue,nId,nRetVal,nV4,nV5;
    double dSfmx,dZk,dSjk;
	char cV2[20],cV9[30],cV6[7],cV5[40],cV11[30],cV12[40];
	
	memset(sBuffer,'\0',1024);
	
   clrscr();
   if(GetValue(2,2,"ÇëÊäÈë²éÑ¯Ê±¶Î£º%8d",&nQsrq)==KEY_ESC) return;
   if(GetValue(26,2," ÖÁ %8d",&nJzrq)==KEY_ESC) return;
   clearline(2);

    sprintf(sTitle,"          ËÍÆ±·½Ê½²éÑ¯ (²éÑ¯Ê±¶Î:%ldÖÁ%ld)",nQsrq,nJzrq);
	pqt=CreateQueryTable(sTitle,
                         "ÂÃ¿ÍÐÕÃû    ÂÃ¿Íµç»°          Æ±±êÊ¶  Æ±¿î        ËÍÆ±·½Ê½  ËÍÆ±³ÇÊÐ  ËÍÆ±µØÖ·              ËÍÆ±ÈÕÆÚ  ËÍÆ±Ê±¼ä  ",
                         "%-10.10s %-16.16s %-6.6s %-10.2lf %-12.12s %-8.8s %-20.20s %8ld %6ld",ST_LASTLINE|ST_VISUAL);
    if(!pqt) return;
    clrscr();

	sprintf(sWhere," nlkrq between %ld and %ld ",nQsrq,nJzrq);
	if(nSysCzyqx==3) strcpy(sQueryCzydh,sSysCzydh);
    
	if(strstr(sSysJsdh,"272201") && nSysCzyqx!=3) ;//·Ç³É¶¼¾ÖµÄÒµÎñ²Ù×÷Ô±
	else sprintf(sWhere+strlen(sWhere)," and cvalue3='%s' ",sSysJsdh);
	
	if(sQueryCzydh[0]!='.')
      sprintf(sWhere+strlen(sWhere)," and cczydh='%s'",sQueryCzydh);

	RunSelect("select sum(nsl),sum(dsjk) from dl$zwmx where %t into %ld%lf",sWhere,&nCount,&dZk);
    
	nId=OpenCursor("select cvalue2,cvalue9,dvalue3,cvalue6,cvalue5,cvalue11,cvalue12,nvalue4,nvalue5 \
				from dl$dbmx_%t where %t order by nvalue4,cvalue5",sJym,sWhere);
	if(nId<0)
	{
     	GetSqlErrInfo(sWhere);
     	return;
	}
	
	ImportLastLine(pqt,".",".",".",dZk,".",".",".",0,0);
	while(1)
	{
     	nRetVal=FetchCursor(nId,"%s%s%lf%s%s%s%s%ld%ld",cV2,cV9,&dSjk,cV6,cV5,cV11,cV12,&nV4,&nV5);
		
		if(ImportQueryTable(pqt,nRetVal,cV2,cV9,cV6,dSjk,cV5,cV11,cV12,nV4,nV5)<=0)
			break;
	}//end while
    CloseCursor(nId);
    DropQueryTable(pqt);

}

//ÐÂÔö²éÑ¯////////////////////
void QueryAirDdxx(long number,char * item,char *sJym)
{
	QUERYTABLE * pqt;
	long nQsrq=nSysLkrq,nJzrq;
	long nId,nRetVal,nlkrq;
	char sTitle[100];
	char cvalue2[20],cvalue9[20],cvalue7[10],cvalue8[6],cvalue10[30],cczydh[8];
	char cvalue4[20],cvalue5[10],cvalue1[40],cvalue17[10],cvalue20[30];
	double dvalue1,dvalue4;
	long nvalue1,nvalue2;
	
	clrscr();
   	if(GetValue(2,2,"ÇëÊäÈë²éÑ¯Ê±¶Î£º%8d",&nQsrq)==KEY_ESC) return;
   	nJzrq=nSysLkrq;
   	if(GetValue(26,2," ÖÁ %8d",&nJzrq)==KEY_ESC) return;
   	sprintf(sTitle,"          ¶¨µ¥ÓÃ»§²éÑ¯ (²éÑ¯Ê±¶Î:%ldÖÁ%ld)",nQsrq,nJzrq);
	
	pqt=CreateQueryTable(sTitle,
    	"ÂÃ¿ÍÐÕÃû      ÂÃ¿Íµç»°          ½ð¶î        ÈÕÆÚ      º½Ïß        ÕÛ¿Û  ²ÕÎ»  ¾ÖÃû                ²Ù×÷Ô±    Éí·ÝÖ¤              »»Æ±·½Ê½  µØÖ·                            Æð·ÉÈÕÆÚ  Æð·ÉÊ±¼ä  º½¶Î    ±¸×¢                  ",
    	"%-14.14s %-15.15s %.2lf %8ld %10s %.3lf 2%-2.2s %-20.20s %-10.10s %-18.18s %-8.8s %-30.30s%-8ld%-6ld%-6.6s%-20.20s",ST_VISUAL|ST_KEYWORD);

    	if(!pqt) return;
	clrscr();
	nId=OpenCursor("select cvalue2,cvalue9,dvalue1,nlkrq,cvalue7,dvalue4,cvalue8,cvalue10,cczydh,cvalue4,cvalue5,cvalue1,nvalue1,nvalue2,cvalue17,cvalue20 \
		from dl$dbmx_%t where nlkrq>=%ld and nlkrq<=%ld and cvalue6='XXXX' ",
		sJym,nQsrq,nJzrq);
	if(nId<0) 
	{
		CloseCursor(nId);
		return;
	}
	while(1)
	{
		nRetVal=FetchCursor(nId,"%s%s%lf%ld%s%lf%s%s%s%s%s%s%ld%ld%s%s",
		cvalue2,cvalue9,&dvalue1,&nlkrq,cvalue7,&dvalue4,cvalue8,cvalue10,cczydh,cvalue4,cvalue5,cvalue1,&nvalue1,&nvalue2,cvalue17,cvalue20);

		if(ImportQueryTable(pqt,nRetVal,
			cvalue2,cvalue9,dvalue1,nlkrq,cvalue7,dvalue4,
			cvalue8,cvalue10,cczydh,cvalue4,cvalue5,cvalue1,
			nvalue1,nvalue2,cvalue17,cvalue20)<=0)
		break;
	}
	CloseCursor(nId);
	DropQueryTable(pqt);
	return;
}

void AirDdProduct(long nId,char * item,char * sJym)
{
    DATATABLE * pdt;//Â¼Èë½çÃæÊý¾Ý±í¸ñ¶ÔÏó
    long nCurrent,nwhere,nModifyFlag,nKey;
    char sOpt[2];
    char sXm[16],sDh[16],sCzydh[10],sLsh[41];
    long nCl=0,nId1,nRetVal;

	clrscr();
	pdt=CreateDataTable(8,
          "|  |ÐÕÃû          |µç»°            |²Ù×÷Ô±    |´¦Àí|",
	  "%-1.1s%-12.12s%-14.14s%-8.8s%-1d%s");
	
	if(!pdt)
    	{
		MessageBox("½¨Á¢Êý¾Ý±í¸ñ³ö´í...",1);
		return;
    	}
	
	SetTableStyle(pdt,ALL_STYLE^ST_NOTCONFIRM^ST_DOUBLELINE,1);
    	AssignTableFields(pdt,"copt,cxm,cdh,cczydh,ncl,clsh");

    	gotoxy(1,1);
    	outtext("                           »ú Æ± ¶© µ¥ ¹Ü Àí");

    	ShowDataTable(pdt,6,3);
    	ShowDatatableInfo(pdt,"ÕýÔÚÌáÈ¡¶©µ¥ÐÅÏ¢£¬ÇëÉÔºò...  ",0);
    	
    	{//load message
    	   nId1=OpenCursor("select cvalue2,cvalue9,cczydh,nvalue15,clsh from \
    	   		dl$dbmx_%t where cvalue6='XXXX' and nvalue15=2 ",sJym);
    	   if(nId1<0) {DropDataTable(pdt); return; }
    	   while(1)
    	   {
    	   	nRetVal=FetchCursor(nId1,"%s%s%s%ld%s",sXm,sDh,sCzydh,&nCl,sLsh);
    	   	if(nRetVal==0) break;
    	   	if(nRetVal<0){DropDataTable(pdt); return; }
 		   	   	
    	   	ImportData(pdt,pdt->nCount+1,"copt,cxm,cdh,cczydh,ncl,clsh",
    	 		"+",sXm,sDh,sCzydh,nCl,sLsh);
    	   }
    	   CommitWork();
    	   CloseCursor(nId1);
    	}//load over
    	
    //begin input
    nCurrent=1;//start with the first line
    nwhere=1;
    nModifyFlag=0;//the document is modified if the flag is nozero
    for(;;)
    {
	//auto append a new record
        if(AutoExtendTable(pdt,&nCurrent)>0)
           ImportData(pdt,nCurrent,"copt,ncl","+",nCl);

        switch(nwhere)
        {
            case 1:
            ShowDatatableInfo(pdt,"ÇëÊäÈë¹¦ÄÜ¿ØÖÆ£¬°´M¼üÏÔÊ¾¹¦ÄÜ¼ü°ïÖú...",0);
            break;
            case 2:
            ShowDatatableInfo(pdt,"ÇëÊäÈë´¦Àí±êÖ¾,[1]Îª´¦Àí[2]Ã»ÓÐ´¦Àí...",0);
            break;
        }

	//Fetch data into container
        nKey=FetchData(pdt,nCurrent,nwhere,"copt,ncl");

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
			nKey=MessageBox("ÄúµÄÐÞ¸ÄÃ»ÓÐ±£´æ£¬Òª±£´æÂð£¿",0);
			if(nKey==KEY_ESC)
			{
                                ShowDataTable(pdt,6,3);
			        continue;
			}
			if(nKey==KEY_YES) SaveAirDdProduct(pdt,sJym);
		    }
		    DropDataTable(pdt);
	            return;

           default:
                    DefaultTableKey(pdt,nKey,&nCurrent,&nwhere);
                    continue;
	}

        //Export data from container which you inputed
	ExportData(pdt,nCurrent,"copt,cxm,cdh,cczydh,ncl,clsh",
    	 		sOpt,sXm,sDh,sCzydh,&nCl,sLsh);
 	ImportData(pdt,nCurrent,"copt","+");
	
	switch(nwhere)
	{
	   case 1: //²Ù×÷·½·¨
		    switch(sOpt[0])
		    {
                             case 's':   //save as temporary record
			     case 'S':
				 if(nModifyFlag)
				 {
				     nModifyFlag=SaveAirDdProduct(pdt,sJym);
				     keyb();
				     ShowDataTable(pdt,6,3);
				 }
				 continue;
			     default:
			         DefaultTableChoice(pdt,sOpt[0],
			                      &nCurrent,&nwhere,&nModifyFlag);
			         continue;
		    }
		    break;
		   
	   case 2:
	   	 nModifyFlag=1;
        	 if(nCl!=1 && nCl!=2) continue;
                 nwhere=1;
	   	 break;

        }//end switch(nwhere);
	ImportData(pdt,nCurrent,"copt,ncl,clsh","+",nCl,sLsh);
	if(nCurrent<pdt->nCount) nCurrent++;
   }//end for(;;)

}

static long SaveAirDdProduct(DATATABLE * pdt,char * sJym)
{
    long nId,nRetVal;
    char sLsh[41]=".";
    long nCl=2;
    
    ShowDatatableInfo(pdt,"ÕýÔÚÈë¿â£¬ÇëÉÔºò...",0);
    
    nRetVal=1;
    for(nId=1;nId<=pdt->nCount;nId++)
    {
    	
    	ExportData(pdt,nId,"ncl,clsh",&nCl,sLsh);
        if(sLsh[0]=='.') continue;
        printf("\nTEST:sLsh=%s",sLsh);
        nRetVal=RunSql("update dl$dbmx_%t set nvalue15=%ld where clsh=%s",sJym,nCl,sLsh);
        if(nRetVal<0) break; 
    }

    if(nRetVal<0)
    {
    	RollbackWork();
        HiError(nRetVal,"Êý¾Ý¿â²Ù×÷Ê§°Ü");
    	return -1;
    }
    else
    {
    	CommitWork();
        ShowDatatableInfo(pdt,"±£´æÍê±Ï...",0);
        return 0;
    }
}
