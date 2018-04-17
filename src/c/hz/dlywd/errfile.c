/**************************************
*****
*****Filename 文件名: ftssw.c
*****Target   目的:远程文件传输状态查询
*****程序员: 刘入志
*****
****************************************/
#include<stdio.h>
#include<stdarg.h>
#include<ctype.h>
#include<string.h> 
#include"bkfx.h"
#include"bkfxtask.h"


extern char sSysPassword[41];
extern long nSysLkrq; 

#ifdef DATACLIENT
#include"tools.h"
extern int nSysSocket;
extern char sSysDsService[41];
extern char sTcpipErrInfo[81];
//****前       台********
int SearchFailedFile(char * sRemoteHost)
{
    long iId,nRetVal,nT;
    int nStatus,nNum,nB;
    char sJinggao[70];
    char sYczjm[21],sYwjm[51],sMdwjm[51],sArq[9],sAsj[7];
    //**远程主机名,源文件名,目的文件名,日期,时间;
    DATATABLE * pdt;
    DIALOG * pd;
    int nSocket;
        
    nSocket = connectTCP(sRemoteHost, sSysDsService);
    if(nSocket < 0)
    {
  	//sprintf(sTcpipErrInfo,"无法与远程主机\"%s\"的\"%s\"服务建立连接",
  	//        sRemoteHost,sSysDsService);
  	return -1;
    }
    if(SendToSocket(nSocket,"%ld",(long)TASK_SEARCH_FAILED_FILE))
    {
       //  sprintf(sTcpipErrInfo,"发送任务号失败");
         shutdown(nSocket,2);
         return -2;
     }

     nNum=0;
     for(;;)
     {
     	nRetVal=GetFromSocket(nSocket,"%ld%s%s%s%s%s",&nT,sYczjm,sYwjm,sMdwjm,sArq,sAsj);
     	if(nRetVal!=0)
     	   return 0;
     	if(nT==0)
     	  break;
         nNum++;      
         if((pd=CreateDialog(70,8,FRAME))==NULL)
                return 0;             
         AddDialog(pd,SPACELINE,1);
         alltrim(sMdwjm);
         if(strcmp(sMdwjm,"")==0)        
         {
              AddDialog(pd,STATIC_TEXT,"       传送不成功的文件：\n");
              nB=0;
         }
         else
         {
              AddDialog(pd,STATIC_TEXT,"       丢失的文件：\n");
              nB=1;
         }
         AddDialog(pd,OUTPUT,"       远程主机名:%-20.20s\n",sYczjm);             
         AddDialog(pd,OUTPUT,"       源文件名:%-50.50s\n",sYwjm);
         if(nB==0)
         {
       	    AddDialog(pd,OUTPUT,"       登记日期:%-8s",sArq);
            AddDialog(pd,OUTPUT,"       登记时间:%-7s\n",sAsj); 
         }
         else
        {
             AddDialog(pd,OUTPUT,"       目的文件名:%-50.50s\n",sMdwjm);
             AddDialog(pd,OUTPUT,"       传送成功日期:%-8s",sArq);
             AddDialog(pd,OUTPUT,"       传送成功时间:%-7s\n",sAsj);  
        }
       AddDialog(pd,SPACELINE,1);
       if(nNum%2!=0)
        {    
             	clrscr();    
               	ShowDialog(pd,3,2);
                RefreshDialog(pd,0);
                DropDialog(pd);
          }
        else
         { 
              	ShowDialog(pd,3,12);
                RefreshDialog(pd,0);
                DropDialog(pd); 
                waitkey();
          }          
   }
  if(nNum>0)
  {
       if(nNum%2!=0)
            waitkey();
       sprintf(sJinggao,"警告:共%d个文件传送失败或丢失!请与维护局或鸿志公司联系!!",nNum);
       MessageBox(sJinggao,1);
  } 
  close(nSocket);
  return 1;
}
#endif

#ifdef DATASERVER
int dsTaskSearchFailedFile(int  nsock)
{  
    long iId,nRetVal;
    int nStatus;
    char sArq[9],sAsj[7];
    char sYczjm[21],sYwjm[51],sMdwjm[51];
    long lDjrq,lDjsj,lCscgrq,lCscgsj;
    //**远程主机名,源文件名,目的文件名,登记日期,登记时间,传送成功日期,传送成功时间;
    
    
    if(ConnectDatabase("FXZ",sSysPassword)) 
     {
  	SendToSocket(nsock,"%ld%s",-1,"无法连接数据库，可能是口令有错!");
     	return 0;
     }
    //*******文件传送不成功
    iId=OpenCursor("select CREMOTEHOST,CLOCALFILE,NLKRQ,NSFM from bk$wjcsdj \
where NSTATUS=0");    
    if(iId<0)
      return 0;
     for(;;)
    {
        nStatus=FetchCursor(iId,"%s%s%ld%ld",sYczjm,sYwjm,&lDjrq,&lDjsj);
        if(nStatus==0)
              break;
        else if (nStatus<0)
              return 0;
        GetSysLkrq();    
        if(nSysLkrq>lDjrq)
         {
             sprintf(sArq,"%ld",lDjrq);
             sprintf(sAsj,"%ld",lDjsj);
             strcpy(sMdwjm,"");
             nRetVal=SendToSocket(nsock,"%ld%s%s%s%s%s",(long)1,sYczjm,sYwjm,sMdwjm,sArq,sAsj);
         }
    }
    CloseCursor(iId);
    
    //*******文件丢失
    iId=OpenCursor("select CREMOTEHOST,CLOCALFILE,CREMOTEFILE,NCGRQ,NCGSFM from bk$wjcsdj where NSTATUS=1");    
    if(iId<0)
      return 0;
    for(;;)
    {
        nStatus=FetchCursor(iId,"%s%s%s%ld%ld",sYczjm,sYwjm,sMdwjm,&lCscgrq,&lCscgsj);
        if(nStatus==0)
              break;
        else if (nStatus<0)
              return 0;
        if(access(sYwjm,0))
        { 
            sprintf(sArq,"%ld",lCscgrq);
            sprintf(sAsj,"%ld",lCscgsj);
            nRetVal=SendToSocket(nsock,"%ld%s%s%s%s%s",(long)1,sYczjm,sYwjm,sMdwjm,sArq,sAsj);       
        }       
    }
    CloseCursor(iId);
    CloseDatabase();
    SendToSocket(nsock,"%ld%s%s%s%s%s",(long)0,"l","l","l","l","l");
              
    return 1;
}
#endif

