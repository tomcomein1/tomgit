/**************************************
*****
*****Filename �ļ���: ftssw.c
*****Target   Ŀ��:Զ���ļ�����״̬��ѯ
*****����Ա: ����־
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
//****ǰ       ̨********
int SearchFailedFile(char * sRemoteHost)
{
    long iId,nRetVal,nT;
    int nStatus,nNum,nB;
    char sJinggao[70];
    char sYczjm[21],sYwjm[51],sMdwjm[51],sArq[9],sAsj[7];
    //**Զ��������,Դ�ļ���,Ŀ���ļ���,����,ʱ��;
    DATATABLE * pdt;
    DIALOG * pd;
    int nSocket;
        
    nSocket = connectTCP(sRemoteHost, sSysDsService);
    if(nSocket < 0)
    {
  	//sprintf(sTcpipErrInfo,"�޷���Զ������\"%s\"��\"%s\"����������",
  	//        sRemoteHost,sSysDsService);
  	return -1;
    }
    if(SendToSocket(nSocket,"%ld",(long)TASK_SEARCH_FAILED_FILE))
    {
       //  sprintf(sTcpipErrInfo,"���������ʧ��");
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
              AddDialog(pd,STATIC_TEXT,"       ���Ͳ��ɹ����ļ���\n");
              nB=0;
         }
         else
         {
              AddDialog(pd,STATIC_TEXT,"       ��ʧ���ļ���\n");
              nB=1;
         }
         AddDialog(pd,OUTPUT,"       Զ��������:%-20.20s\n",sYczjm);             
         AddDialog(pd,OUTPUT,"       Դ�ļ���:%-50.50s\n",sYwjm);
         if(nB==0)
         {
       	    AddDialog(pd,OUTPUT,"       �Ǽ�����:%-8s",sArq);
            AddDialog(pd,OUTPUT,"       �Ǽ�ʱ��:%-7s\n",sAsj); 
         }
         else
        {
             AddDialog(pd,OUTPUT,"       Ŀ���ļ���:%-50.50s\n",sMdwjm);
             AddDialog(pd,OUTPUT,"       ���ͳɹ�����:%-8s",sArq);
             AddDialog(pd,OUTPUT,"       ���ͳɹ�ʱ��:%-7s\n",sAsj);  
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
       sprintf(sJinggao,"����:��%d���ļ�����ʧ�ܻ�ʧ!����ά���ֻ��־��˾��ϵ!!",nNum);
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
    //**Զ��������,Դ�ļ���,Ŀ���ļ���,�Ǽ�����,�Ǽ�ʱ��,���ͳɹ�����,���ͳɹ�ʱ��;
    
    
    if(ConnectDatabase("FXZ",sSysPassword)) 
     {
  	SendToSocket(nsock,"%ld%s",-1,"�޷��������ݿ⣬�����ǿ����д�!");
     	return 0;
     }
    //*******�ļ����Ͳ��ɹ�
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
    
    //*******�ļ���ʧ
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

