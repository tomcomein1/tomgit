/*=======================================================
程序名称：dl_193.c
目的：联通193入网模块
程序员：陈博[chenbo]
日期：10/15/2001
=======================================================*/
#include"dlpublic.h"
#include"tasktool.h"
#include"tools.h"
#include"filechar.h"
#include"newtask.h"


extern char sSysJsdh[10];
extern char sSysCzydh[7];
extern char sTcpipErrInfo[81];

void Unicom193Registe(long number,char * item,char * sJym)
{
    DIALOG * pd;
    char sYhmc[41]=".",sPhone[21]=".",sYhdz[51]=".",sSfzhm[21]=".",sYwlx[6]="193";
    char * sOffset,sLsh[41],sBuf[256];
    long nFkfs=0,nWhere,nRetVal;
    double dYfk=0.0;
    char * sMessage[7]={
    	   "请输入要入网的电话号码...",
    	   "请输入用户名称或单位名称...",
    	   "通输入用户的详细通讯地址...",
    	   "请输入用户或代理人的身份证号码...",
   	   "请输入业务类型：193/17911...",
    	   "请输入付款方式：0：预付费  1：后付费...",
    	   "请修入本次预付费款额..."};


  
    if(!(pd=CreateDialog(60,21,FRAME))) return;

    AddDialog(pd,"sep1",SPACELINE,1);    //1
    AddDialog(pd,"static",STATIC_TEXT,"               〖  联 通 193 电 话 入 网  〗\n");//2
    AddDialog(pd,"sep2",SEPARATOR,"~~\n");
   
    AddDialog(pd,"phone",INPUT,"  电话号码:%-20.20s\n\n",sPhone);
    AddDialog(pd,"yhmc",INPUT,"  用户名称:%-20.20s\n\n",sYhmc);
    AddDialog(pd,"yhdz",INPUT,"  用户地址:%-40.40s\n\n",sYhdz);
    AddDialog(pd,"sfzhm",INPUT,"  身份证号:%-20.20s\n\n",sSfzhm);
    AddDialog(pd,"ywlx",INPUT,"  业务类型:%-5.5s",sYwlx);
    AddDialog(pd,"fkfs",INPUT,"     付费方式:%-1d\n\n",&nFkfs);
    AddDialog(pd,"yfke",INPUT,"  预付款额:%10.2f\n",&dYfk);
    

    clrscr();
    ShowDialog(pd,10,2);
    RefreshDialog(pd,0);
    nWhere=1;
    for(;;)
    {
 	RefreshDialog(pd,0);
 	SetDialogInfo(pd,sMessage[nWhere-1]);

    	nRetVal=ActiveDialog(pd,nWhere,"phone,yhmc,yhdz,sfzhm,ywlx,fkfs,yfke");

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
                alltrim(sPhone);
                if(strlen(sPhone)<7) continue;
                sOffset=sPhone;
                while(sOffset[0])
                {
                    if(sOffset[0]<'0' || sOffset[0]>'9') break;
                    sOffset++;
                }
                if(sOffset[0]) continue;
                break;	
           case 2:
                alltrim(sYhmc);
                if(strlen(sYhmc)<4) continue;
                break;
           case 3:
                alltrim(sYhdz);
                break;
           case 4:
                alltrim(sSfzhm);
                if(strlen(sSfzhm)<15) continue;
                sOffset=sSfzhm;
                while(sOffset[0])
                {
                    if(sOffset[0]<'0' || sOffset[0]>'9') break;
                    sOffset++;
                }
                if(sOffset[0]) continue;
                break;	
           case 5:
                alltrim(sYwlx);
                if(strcmp(sYwlx,"193") && strcmp(sYwlx,"17911")) continue;
                break;
           case 6:
                if(nFkfs!=0 && nFkfs!=1) continue;
                break;
           case 7:
                if(dYfk<0.0) continue; 
                SetDialogInfo(pd,"请输入: '1'-确认入网  '2'-放弃...");
                for(;;)
                {
                   nRetVal=keyb();
                   if(nRetVal=='1' || nRetVal=='2') break;
                }
                if(nRetVal=='1')
                {

                   SetDialogInfo(pd,"正在进行交易，请稍候...");
                   GenerateLsh(1,sSysJsdh,sJym,sLsh);
                   sprintf(sBuf,
                   "%-40.40s|%-6.6s|%s|%s|%s|%s|%s|.|.|.|.|.|"
                   "%ld|0|0|0|0|0|0|0|0|0|%.2lf|0|0|0|0|0|0|0|0|0|",
                   sLsh,sSysCzydh,sPhone,sYhmc,sYhdz,sSfzhm,sYwlx,nFkfs,dYfk);
                   
                   nRetVal=AutoRunTask("dldj",TASK_DAIBANYW,"%s","%s",sBuf,sBuf);
                   if(nRetVal)
                   	SetDialogInfo(pd,sTcpipErrInfo);
                   else
                        SetDialogInfo(pd,sBuf+7);
                   keyb();
                
                }
                strcpy(sYhmc,".");
                strcpy(sPhone,".");
                strcpy(sYhdz,".");
                strcpy(sSfzhm,".");
                nWhere=1;
                continue;
        }
        nWhere++;
    }
}
