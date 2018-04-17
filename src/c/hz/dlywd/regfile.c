/*=============================================================================
  filename : regfile.c  
  target   : Ϊ��֤����ϵͳ���ļ��������ȷ�Զ����õ�ע��ʽ�ļ�����
  orginal programmer: �²�
==============================================================================*/

#include<stdio.h>
#include<sys/stat.h>
#include<dirent.h>
#include"bkfx.h"
#include"regfile.h"
extern long nSysLkrq,nSysSfm;


#ifndef DATASERVER   //�ͻ���
#include"tools.h"
#include"bkfxtask.h"

extern int nSysSocket;
extern char sSysDsService[41];
extern char sSysJh[JM_JH_LEN+1];
extern char sTcpipErrInfo[81];
/*---------------------------------------------------------
������SearchAndRegisteFileTree;
���ܣ����������������Ŀ¼�������������ļ�עɾ��bk$wjcsdj����
������
      root:����Ŀ¼
      remote:Զ��Ŀ¼
      dest:Զ������
      nMode:MODE_NONE_DIR�����޵غͱ��ض���������Ŀ¼
            MODE_LOCAL_DIR:����������Ŀ¼��Ч�����򲻿�����Ŀ¼
            MODE_REMOTE_DIR��������Ӧ��Զ����Ŀ¼
            ���ܹ���|��������ѡ��
            
���أ�0:�ɹ�������ʧ��
*---------------------------------------------------------*/

long SearchAndRegisteFileTree(char * root,char * remote,char * dest,long nMode)
{
    DIR * pdir;
    struct dirent * pdirent;
    struct stat statFile;
    char   sLocalFile[51],sRemoteFile[51];
    
    if(!(pdir=opendir(root))) return -2;  //can't open the directory
    while(pdirent=readdir(pdir))
    {
    	//�����'.','..'�������
    	if(!strcmp(pdirent->d_name,".")||!strcmp(pdirent->d_name,"..")) continue;
    	
    	sprintf(sLocalFile,"%s/%s",root,pdirent->d_name);
        sprintf(sRemoteFile,"%s/%s",remote,pdirent->d_name);
        if(nMode&MODE_REMOTE_DIR)
            sprintf(sRemoteFile,"%s/%s/%s",remote,root,pdirent->d_name);
        
        stat(sLocalFile, &statFile);

        if(S_ISDIR(statFile.st_mode)&&(nMode&MODE_LOCAL_DIR))
            SearchAndRegisteFileTree(sLocalFile,remote,dest,nMode);
    
        if(S_ISREG(statFile.st_mode))
            RegisteFile(0,dest,sLocalFile,sRemoteFile);
  
    }//end while
    closedir(pdir);
    return 0;
}

/*=============================================================
������long RegisteFile;
Ŀ�ģ�����������ļ��Ǽǵ��ļ�����ǼǱ���(bk$wjcsdj),���ܾ��ظ�
      �Ǽ�(CLOCALFILEΪ����)��
������
      long nOpt: 0:�����ļ���1:�����ļ�
      char * sRemoteHost:Զ��������ַ�����
      char * sLocalFile:�����ļ�
      char * sRemoteFile:Զ���ļ�

���أ�>0:�Ǽǳɹ�
      =0:�ظ��Ǽ�
      <0:�Ǽ�ʧ��
*=============================================================*/

long RegisteFile(long nOpt,char * sRemoteHost,char * sLocalFile,char * sRemoteFile)
{

     long nRetVal;
     
     if(!sRemoteHost||!sLocalFile||!sRemoteFile) return -2;
     
     nRetVal=RunSelect("select * from bk$wjcsdj where clocalfile=%s and ncsfx=%ld",
                       sLocalFile,nOpt);
     if(nRetVal<0) return nRetVal;
     if(nRetVal>0) return 0;

     GetSysLkrq();
     nRetVal=RunSql("insert into bk$wjcsdj(cremotehost,clocalfile,cremotefile,\
                    ncsfx,nstatus,nlkrq,nsfm,ncgrq,ncgsfm,nclrq,nclsfm,cbl) \
                    values(%s,%s,%s,%ld,0,%ld,%ld,0,0,0,0,'0')",
                    sRemoteHost,sLocalFile,sRemoteFile,nOpt,nSysLkrq,nSysSfm);

     if(nRetVal<=0)
     {
     	HiError(nRetVal,"�ļ��Ǽǳ���");
        RollbackWork();
     	return -3;
     }
     CommitWork();
     return 1;
}


//ʡ���оֽ�ʡ�����ļ����պ󲢽�����ȥ
long MoveRemoteAfterReceive(char * sRemoteHost,char * sRemoteFile,char * sBackupDir)
{
     long nRetVal=-1;
     
     RunTask(sRemoteHost,TASK_MOVE_FILE_AFTERRECEIVE,"%s%s","%ld",
             sRemoteFile,sBackupDir,&nRetVal);
    
     return nRetVal;                      
}


//ʡ������ʡ����Ŀ¼�������еǼǵ��ļ��ڱ���Ҳ�Ǽ�
long ReceiveAndRegisteFile(char * sRemoteHost,char * sRemoteDir,
                           char * sLocalDir,long nMode)
{
     
  char sLocalFile[61],sRemoteFile[51];
  char * sOffset;
  long nRetVal,nResult;
  int nSocket;
  nSocket = connectTCP(sRemoteHost, sSysDsService);
  
  if(nSocket < 0)
  {
  	sprintf(sTcpipErrInfo,"�޷���Զ������\"%s\"��\"%s\"����������",
  	        sRemoteHost,sSysDsService);
  	return -1;
  }

  if(SendToSocket(nSocket,"%ld%ld%s",(long)TASK_SEARCH_ALL_FILE,nMode,sRemoteDir))
  {
      sprintf(sTcpipErrInfo,"���������ʧ��");
      shutdown(nSocket,2);
      return -2;
  }

  for(;;)
  {
     
     nResult=-1; 
     nRetVal=GetFromSocket(nSocket,"%ld%s",&nResult,sRemoteFile);
     if(nResult<0)
     {
     	strcpy(sTcpipErrInfo,sRemoteFile);
     	clrscr();
     	outtext(sTcpipErrInfo);
     	keyb();
     }
     if(nRetVal||nResult==0) break;
     
     if(nResult<0) continue;
     
     sOffset=sRemoteFile;
     if(strchr(sOffset,'/')) 
     {
        while(sOffset[0]&&sOffset[0]!='/') sOffset++;
        sOffset++;
     }

     sprintf(sLocalFile,"%s/%s",sLocalDir,sOffset);
     nRetVal=RegisteFile(1,sRemoteHost,sLocalFile,sRemoteFile);
     if(nRetVal==0)
     {
         nResult=-1;
         RunSelect("select nstatus from bk$wjcsdj where clocalfile=%s \
                    and ncsfx=1 into %ld",sLocalFile,&nResult);
         if(nResult>=1)
             MoveRemoteAfterReceive(sRemoteHost,sRemoteFile,"olddata");
     }

  }   
  close(nSocket);
  return 0;
}
                            
/*=============================================================
������long DealwithRegisteredFile;
Ŀ�ģ����ļ�����ǼǱ������Ǽǵĵ�δ����ɹ����ļ������Զ�����
      �յ����ء�
������long nOpt:0:�������ļ���1:��������ļ�
      char * sBackupDir:����ɹ����ļ�������Ŀ¼[����ʱʹ��];
���أ�>0:�ɹ�
      =0:û���ļ��ɴ���
      <0:ĳЩ�ļ���ȫ���ļ�û����ɹ���
*=============================================================*/
long DealwithRegisteredFile(long nOpt,char * sBackupDir)
{
     DATATABLE * pdt;
     long nRetVal,nId,nCount,nError=1;
     char sbuf[70],sRemoteHost[21],sLocalFile[51];
     char sRemoteFile[51],sTempHost[21]=".";


     if(nOpt!=0&&nOpt!=1) return 0;
     DisconnectServer();
     nSysSocket=0;
     GetSysLkrq();
     
     pdt=CreateDataTable(8,
     "|  |�����ļ���                              |Ŀ�ĵ�          |״̬      |",
     "%-2.2s%-40.40s%-16.16s%-10.10s%s");
     if(!pdt) return -1;
     MergeTable(pdt->pTable,CreateTable(1,
     "|                                                                          |"));
     
     clrscr();
     if(nOpt==0) outtext("                            �� �� �� ��");
     else outtext("                            �� �� �� ��");
     ShowDataTable(pdt,1,3);
     ShowDatatableInfo(pdt,"������ȡδ������ļ�...",0);
     
     nId=OpenCursor("select cremotehost,clocalfile,cremotefile from bk$wjcsdj \
                     where ncsfx=%ld and nstatus=0 order by cremotehost",nOpt);
     if(nId<=0)
     {
     	HiError(nId,"��ȡ�ļ�����!");
     	DropDataTable(pdt);
     	return -2;
     }
     for(;;)
     {
     	if(FetchCursor(nId,"%s%s%s",sRemoteHost,sLocalFile,sRemoteFile)<=0) break;
        ImportData(pdt,pdt->nCount+1,"%s%s%s%s%s",
                       " ",sLocalFile,sRemoteHost,"δ����",sRemoteFile);
     }
     CloseCursor(nId);
     
     if(BrowseRegistedTable(pdt)==KEY_ESC)
     {
     	DropDataTable(pdt);
     	return -2;
     }

     for(nId=1;nId<=pdt->nCount;nId++)
     {
     	ImportData(pdt,nId,"%s","��");
     	if(nId>pdt->nCurrent+pdt->nPage) ScrollTable(pdt,pdt->nPage);
     	ExportData(pdt,nId,"> %s %s > %s",sLocalFile,sRemoteHost,sRemoteFile);
     	alltrim(sLocalFile);
     	alltrim(sRemoteHost);
     	alltrim(sRemoteFile);
        if(nId==1) strcpy(sTempHost,sRemoteHost);
     	if(strcmp(sTempHost,sRemoteHost)||nSysSocket<=0)
     	{
             sprintf(sbuf,"��������Զ������\"%s\"��\"%s\"����...",
                     sRemoteHost,sSysDsService);       
             ShowDatatableInfo(pdt,sbuf,0);
     	     DisconnectServer();     	
     	     nSysSocket=0;
     	     ConnectServer(sRemoteHost);
     	     if(nSysSocket<=0)
     	     {
                 sprintf(sbuf,"�����쳣���޷���������\"%s\"...",sRemoteHost);
                 ShowDatatableInfo(pdt,sbuf,1);
                 ImportData(pdt,nId," %s > > %s","��","����ʧ��");
                 nError=-1;
                 continue;
             }
        }
        strcpy(sTempHost,sRemoteHost);
        
        if(nOpt==0)
        {  
             ShowDatatableInfo(pdt,"���ڽ���Զ��ע��...",0);
             nRetVal=RegisteRemote(sRemoteHost,sLocalFile,sRemoteFile);
             if(nRetVal<0)
             {   
                sprintf(sbuf,"�����쳣���޷�������\"%s\"�Ͻ����ļ�ע��...",sRemoteHost);
                ShowDatatableInfo(pdt,sbuf,1);
             	ImportData(pdt,nId," %s > > %s","��","ע��ʧ��");
             	nError=-1;
             	continue;
             }
             if(nRetVal==0) //Զ���ļ��Ѵ��ɹ���ֻ����δ����
             {
             	ImportData(pdt,nId," %s > > %s","��","�ɹ�");
             	MoveRegisteredFile(sLocalFile,sBackupDir);
             	continue;
             }
             nRetVal=SendFileToRemote(sLocalFile,sRemoteFile,1);
        }
        else nRetVal=ReceiveFileFromRemote(sRemoteFile,sLocalFile,0);

        if(nRetVal)
        {   
             ImportData(pdt,nId,"%s > > %s","��","����ʧ��");
             switch(nRetVal)
             {
             	case 1:
             	     strcpy(sbuf,"���ش���:�������...");
             	     break;
             	case -1:
             	     sprintf(sbuf, "���ش���:Զ���ļ�'%s'�ļ�����Ϊ0...",sRemoteFile);
             	     break;
             	case -2:
             	     strcpy(sbuf,"���ش���:�ļ�������...");
             	     break;
                case -3:
                     strcpy(sbuf,"���ش���:����д��ʱ�ļ�...");
                     break;
                case -4:
                     strcpy(sbuf,"���ش���:�ƶ��ļ�����...");
                     break;
                case -5:
                     strcpy(sbuf,"���ش���:����û�н���...");
                     break;
                default:strcpy(sbuf,"δ֪�Ĵ���...");      
             }        
             
             ShowDatatableInfo(pdt,sbuf,1);
             nError=-1;
             continue;
        }

        if(nOpt==0)
             nRetVal=RegisteRemoteSuccessed(sRemoteHost,sLocalFile,sRemoteFile);
        else 
             nRetVal=RunSql("update bk$wjcsdj set nstatus=1,ncgrq=%ld,ncgsfm=%ld \
                             where clocalfile=%s and ncsfx=1",
                             nSysLkrq,nSysSfm,sLocalFile);
                          
        if(nRetVal<0)
        {   
             ImportData(pdt,nId,"%s > > %s","��","ȷ��ʧ��");
             nError=-1;
             continue;
        }
        CommitWork();
        ImportData(pdt,nId,"%s > > %s","��","�ɹ�");
        if(nOpt==0) MoveRegisteredFile(sLocalFile,sBackupDir);
        else        MoveRemoteAfterReceive(sRemoteHost,sRemoteFile,"olddata");
   }//end for(;;)     
   DisconnectServer();
   BrowseRegistedTable(pdt);
   DropDataTable(pdt);
   return nError;
}

long  BrowseRegistedTable(DATATABLE * pdt)
{
     long nRetVal;
     
     ShowDatatableInfo(pdt,"�����¹�����ҳ,Enter��ʼ��һ��,ESC/0����..",0);
     if(!pdt->nCount)
     {
     	keyb();
     	return KEY_ESC;
     }

     for(;;)
     {
     	nRetVal=keyb();
     	if(nRetVal==KEY_RETURN)
     	{
             pdt->nCurrent=1;
             RefreshDataTable(pdt);
     	     return KEY_RETURN;
     	}
     	if(nRetVal==KEY_ESC||nRetVal=='0')
     	{
     	     return KEY_ESC;
     	}
     	if(nRetVal==KEY_UP||nRetVal=='1') ScrollTable(pdt,-pdt->nPage);
     	if(nRetVal==KEY_DOWN||nRetVal=='2') ScrollTable(pdt,pdt->nPage);
     }
}


void ShowDatatableInfo(DATATABLE * pdt,char * smsg,long ntype)
{

     if(!pdt||!smsg) return;

     FillTable(pdt->pTable,pdt->pTable->nLine+1,"%-68.68s",smsg);
     if(ntype)
     {
        keyb();
        FillTable(pdt->pTable,pdt->pTable->nLine+1,"%-68.68s"," ");
     }
     fflush(stdout);
}



/*=============================================================
������long MoveRegisteredFile
Ŀ�ģ������ͳɹ����ļ��Ƶ�����Ŀ¼�²����ļ�����ǼǱ��и��ļ���
      ״̬��1(nstatus=1);
������char * sLocalFile:�����ļ�����Ӧ��ǼǱ��е���ȫһ�£�
      char * sBackupDir:����ɹ����ļ�������Ŀ¼;
���أ�>0:�ɹ�
      <0:ʧ��
*=============================================================*/
static long MoveRegisteredFile(char * sLocalFile,char * sBackupDir)
{
    
     char sDirectory[60],sShell[60];
     char * sOffset;
     long nRetVal;
     
     GetSysLkrq();
     nRetVal=RunSql("update bk$wjcsdj set nstatus=1,ncgrq=%ld,ncgsfm=%ld \
                     where clocalfile=%s and ncsfx=0",nSysLkrq,nSysSfm,sLocalFile);
     if(nRetVal<=0) return -1;
     CommitWork();
     
     CommitWork();
     sprintf(sDirectory,"%s/%s",sBackupDir,sLocalFile);
     sOffset=sDirectory;
     while(strchr(sOffset,'/')) sOffset++;
     sOffset[0]='\0';
     
     if(access(sDirectory,0))     
     {
     	sprintf(sShell,"mkdir -p %s",sDirectory);
     	system(sShell);
     }
     
     sprintf(sDirectory,"%s/%s",sBackupDir,sLocalFile);
     if(rename(sLocalFile,sDirectory))
     {
        savewin();
     	clrscr();
     	outtext("\n�޷����ļ�%s�ƶ���%s,���ֹ��ƶ���.",sLocalFile,sDirectory);
     	keyb();
     	popwin();
        return -1;
     }	
     return 1;
}



/*=============================================================
������long RegisteRemote
Ŀ�ģ�1.���Զ���ļ��Ǽǣ���Զ��δ�Ǽ���Ǽǲ�����1��
      2.��Զ���ѵǼǵ���״̬Ϊδ�ɹ����򷵻�1��
      3.��Զ���ļ��ѵǼ�Ϊ�ɹ������򷵻�0��
������char * sRemoteHost:Զ��������
      char * sLocalFile:�����ļ�����Ӧ��ǼǱ��е���ȫһ�£�
      char * sBackupDir:����ɹ����ļ�������Ŀ¼;
���أ�>0:�Ǽǳɹ������Կ�ʼ���ļ�
      =0:�ļ��Ѿ�����ɹ��������ٴ����ļ�
      <0:�Ǽ�ʧ�� 
*=============================================================*/
static long RegisteRemote(char * sRemoteHost,char * sLocalFile,char * sRemoteFile)
{
   long nRetVal;
   char sLocalIp[21]=".";
   
   nRetVal=-1;//��ʼ״̬Ϊ-1
   RunSelect("select ccs from bk$bkfxcsfb where cjh=%s and nbh=97 into %s",
               sSysJh,sLocalIp);     
   
   RunTask(sRemoteHost,TASK_REGISTE_FILE,"%s%s%s","%ld",
           sLocalFile,sRemoteFile,sLocalIp,&nRetVal);
   
   return nRetVal;        
   

}

/*=============================================================
������long RegisteRemoteSuccessed
Ŀ�ģ���Զ���ļ��Ǽ�Ϊ�ѳɹ���
      1.��Զ��û��ԭʼ�Ǽ��򷵻�-1��
      2.��Զ���Ǽ�Ϊδ����ɹ�״̬���ȼ�鱾���ļ���Զ���ļ���
        ��һ�£���һ����Զ�ر�ʶΪ�ѳɹ�������1������һ���򷵻�-1��
      2.��Զ���Ǽ�Ϊ�Ѵ���ɹ����򷵻�0��
������char * sRemoteHost:Զ��������
      char * sLocalFile:�����ļ�����Ӧ��ǼǱ��е���ȫһ�£�
      char * sBackupDir:����ɹ����ļ�������Ŀ¼;
���أ�>0:�Ǽǳɹ�
      =0:�Ѿ��ɹ�
      <0:�Ǽ�ʧ�� 
*=============================================================*/
static long RegisteRemoteSuccessed(char * sRemoteHost,char * sLocalFile,char * sRemoteFile)
{
   long nRetVal,nFileSize;
   struct stat filestat;
   
   nRetVal=-1;  //��ʼ״̬Ϊ-1
   
   if(access(sLocalFile,0)) return -1;
   stat(sLocalFile,&filestat);
   nFileSize=(long)filestat.st_size;
   
   RunTask(sRemoteHost,TASK_REGISTE_SUCCESSED,"%s%s%ld","%ld",
           sLocalFile,sRemoteFile,nFileSize,&nRetVal);
   
   return nRetVal;        
}


#else  //�����

extern char sSysPassword[41];  //FZX����
/*=============================================================
������long dsTaskRegisteRemote[TASK_REGISTE_REMOTE]
Ŀ�ģ���Զ���Ǽ��ϴ��ļ�����
*=============================================================*/
long dsTaskRegisteRemote(int nSocket)
{
     long nRetVal,nStatus=-1;
     char sLocalFile[51],sRemoteFile[51],sRemoteHost[21];
     char sDirectory[61],sShell[81];
     char * sOffset;
     
     nRetVal=GetFromSocket(nSocket,"%s%s%s",sLocalFile,sRemoteFile,sRemoteHost);
     
     if(nRetVal) return -1;
     
     if(strchr(sRemoteFile,'/'))
     {
           strcpy(sDirectory,sRemoteFile);
           sOffset=sDirectory;
           while(strchr(sOffset,'/')) sOffset++;
           sOffset[0]='\0';
           if(access(sDirectory,0))
           {
           	sprintf(sShell,"mkdir -p %s",sDirectory);
           	system(sShell);
           }
     }
        
     if(ConnectDatabase("FXZ",sSysPassword)) 
     {
     	SendToSocket(nSocket,"%ld%s",-1,"�޷��������ݿ⣬�����ǿ����д�!");
     	return 0;
     }
     
     GetSysLkrq();
     nRetVal=RunSelect("select nstatus from bk$wjcsdj where cremotefile=%s \
                        and ncsfx=1 into %ld",
                        sLocalFile,&nStatus);
     
     if(nRetVal==0)
     {
        nRetVal=RunSql("insert into bk$wjcsdj(cremotehost,clocalfile,cremotefile,\
                    ncsfx,nstatus,nlkrq,nsfm,ncgrq,ncgsfm,nclrq,nclsfm,cbl) \
                    values(%s,%s,%s,1,0,%ld,%ld,0,0,0,0,'0')",
                    sRemoteHost,sRemoteFile,sLocalFile,nSysLkrq,nSysSfm);
        CommitWork();
        CloseDatabase();
        if(nRetVal<=0)
        {
           GetSqlErrInfo(sRemoteFile);
           SendToSocket(nSocket,"%ld%s",-2,sRemoteFile);
           return 0;
        }
        SendToSocket(nSocket,"%ld%ld",0,1);
        return 0;
     }
     CloseDatabase();            
     if(nRetVal<0)
     {
        SendToSocket(nSocket,"%ld%ld",0,1);
        return 0;
     }
     //�Ѿ��Ǽǵ�δ����
     if(nStatus==0)
     {
        SendToSocket(nSocket,"%ld%ld",0,1);
        return 0;
     }
     //�Ѿ��Ǽǲ����ͳɹ�
     SendToSocket(nSocket,"%ld%ld",0,0);
     return 0;
}

/*=============================================================
������long dsTaskRegisteRemoteSuccessed[TASK_REGISTE_REMOTE_SUCCESSED]
Ŀ�ģ��޸��ļ��ɹ������־
*=============================================================*/
long dsTaskRegisteRemoteSuccessed(int nSocket)
{
     long nRetVal,nStatus=-1,nFileSize;
     char sLocalFile[51],sRemoteFile[51];
     struct stat filestat;


     GetSysLkrq();
     nRetVal=GetFromSocket(nSocket,"%s%s%ld",sLocalFile,sRemoteFile,&nFileSize);
     if(nRetVal) return -1;
     if(ConnectDatabase("FXZ",sSysPassword)) 
     {
     	SendToSocket(nSocket,"%ld%s",-1,"�޷��������ݿ⣬�����ǿ����д�!");
     	return 0;
     }
     
     nRetVal=RunSelect("select nstatus from bk$wjcsdj where cremotefile=%s \
                       and ncsfx=1 into %ld",sLocalFile,&nStatus);
     if(nRetVal<=0)
     {
     	//û��ԭʼ�Ǽ�
     	CloseDatabase();
     	SendToSocket(nSocket,"%ld%ld",0,-1);
     	return 0;
     }
     
     if(nStatus==0)
     {
        //��ԭʼ�����Ǽ�
        if(access(sRemoteFile,0))
        {
             //�еǼǵ�û����Ӧ�ļ�
       	     CloseDatabase();
     	     SendToSocket(nSocket,"%ld%ld",0,-2);
     	     return 0;
     	}
        stat(sRemoteFile,&filestat);
        if(nFileSize!=(long)filestat.st_size)
        {
       	     //�ļ��ֽ�������
       	     CloseDatabase();
     	     SendToSocket(nSocket,"%ld%ld",0,-3);
     	     return 0;
        }
        nRetVal=RunSql("update bk$wjcsdj set nstatus=1,ncgrq=%ld,ncgsfm=%ld \
                        where cremotefile=%s and ncsfx=1",
                        nSysLkrq,nSysSfm,sLocalFile);
        CommitWork();
        CloseDatabase();
        if(nRetVal<=0)
        {
     	     SendToSocket(nSocket,"%ld%ld",0,-4);
     	     return 0;
        }
        SendToSocket(nSocket,"%ld%ld",0,1);    //successed
        return 0;
     }//endif
     CloseDatabase();
     SendToSocket(nSocket,"%ld%ld",0,0);    //already successed
}

/*=============================================================
������long dsTaskSearchAllFileName[TASK_SEARCH_ALL_FILE]
Ŀ�ģ��������ݿ��л�ָ��Ŀ¼�е��ļ�
*=============================================================*/
long dsTaskSearchAllFileName(int nSocket)
{
     DIR * pdir;
     struct dirent * pdirent;
     long nRetVal,nMode,nId,nFileSize,nStatus;
     char sDirectory[31],sLocalFile[51],sbuf[256];
     struct stat filestat;
     
     nRetVal=GetFromSocket(nSocket,"%ld%s",&nMode,sDirectory);
     if(nRetVal) return -1;

     GetSysLkrq();
     if(ConnectDatabase("FXZ",sSysPassword)) 
     {
  	SendToSocket(nSocket,"%ld%s",-1,"�޷��������ݿ⣬�����ǿ����д�!");
     	return 0;
     }
     if(!(pdir=opendir(sDirectory)))
     {
        CloseDatabase();
     	SendToSocket(nSocket,"%ld%s",(long)-3,"�޷���ָ��Ŀ¼");
     	return 0;
     }
     
     while((pdirent=readdir(pdir))&&!nRetVal)
     {
    	//�����'.','..'�������
    	if(!strcmp(pdirent->d_name,".")||!strcmp(pdirent->d_name,"..")) continue;
    	sprintf(sLocalFile,"%s/%s",sDirectory,pdirent->d_name);
        stat(sLocalFile, &filestat);
    
        if(S_ISREG(filestat.st_mode))
        {
           nStatus=-1;
           nRetVal=RunSelect("select nstatus from bk$wjcsdj where ncsfx=1 and \
          	              clocalfile=%s into %ld",sLocalFile,&nStatus);
           if(nRetVal>0)
           {
        	nRetVal=0;
        	if(nStatus==1)
        	   nRetVal=SendToSocket(nSocket,"%ld%s",(long)1,sLocalFile);
           	continue;
           }	
           //�ǼǱ���û��
           if(nMode&MODE_FROM_DISK)
           {
              nRetVal=RunSql("insert into bk$wjcsdj(cremotehost,clocalfile,cremotefile,\
                    ncsfx,nstatus,nlkrq,nsfm,ncgrq,ncgsfm,nclrq,nclsfm,cbl) \
                    values(%s,%s,'./%t',1,1,%ld,%ld,%ld,%ld,0,0,'0')",
                    "localhost",sLocalFile,sLocalFile,nSysLkrq,
                    nSysSfm,nSysLkrq,nSysSfm);

              if(nRetVal>0)
              {
              	 CommitWork();
          	 nRetVal=SendToSocket(nSocket,"%ld%s",(long)1,sLocalFile);
              	 continue;
              	 
              }
              else
              {
                 nStatus=0;
                 GetSqlErrInfo(sLocalFile);
                 RollbackWork();
        	 nRetVal=SendToSocket(nSocket,"%ld%s",(long)-1,sLocalFile);
        	 continue;
              }	    
           }         
           nRetVal=0;
        }
     }//end while
     CloseDatabase();
     closedir(pdir);
     SendToSocket(nSocket,"%ld%s",(long)0,".");
     return 0;
}

long dsTaskMoveRemoteAfterReceive(int nSocket)
{
     char sDirectory[60],sShell[60],sBackupDir[51],sLocalFile[51];
     char * sOffset;
     long nRetVal;
     
     nRetVal=GetFromSocket(nSocket,"%s%s",sLocalFile,sBackupDir);
     if(nRetVal) return -1;

     GetSysLkrq();
     if(ConnectDatabase("FXZ",sSysPassword)) 
     {
  	SendToSocket(nSocket,"%ld%s",-1,"�޷��������ݿ⣬�����ǿ����д�!");
     	return 0;
     }
     RunSql("update bk$wjcsdj set nstatus=2,nclrq=%ld,nclsfm=%ld where \
             clocalfile=%s",nSysLkrq,nSysSfm,sLocalFile);
     CommitWork();
     CloseDatabase();

     sprintf(sDirectory,"%s/%s",sBackupDir,sLocalFile);
     sOffset=sDirectory;
     while(strchr(sOffset,'/')) sOffset++;
     sOffset[0]='\0';
     
     if(access(sDirectory,0))     
     {
     	sprintf(sShell,"mkdir -p %s",sDirectory);
     	system(sShell);
     }
     
     sprintf(sDirectory,"%s/%s",sBackupDir,sLocalFile);
     rename(sLocalFile,sDirectory);
     SendToSocket(nSocket,"%ld%ld",0,1);
     return 1;
}
#endif
