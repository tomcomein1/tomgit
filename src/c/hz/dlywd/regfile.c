/*=============================================================================
  filename : regfile.c  
  target   : 为保证报刊系统中文件传输的正确性而采用的注册式文件传输
  orginal programmer: 陈博
==============================================================================*/

#include<stdio.h>
#include<sys/stat.h>
#include<dirent.h>
#include"bkfx.h"
#include"regfile.h"
extern long nSysLkrq,nSysSfm;


#ifndef DATASERVER   //客户端
#include"tools.h"
#include"bkfxtask.h"

extern int nSysSocket;
extern char sSysDsService[41];
extern char sSysJh[JM_JH_LEN+1];
extern char sTcpipErrInfo[81];
/*---------------------------------------------------------
函数：SearchAndRegisteFileTree;
功能：深度优先搜索本机目录，将搜索到的文件注删到bk$wjcsdj表中
参数：
      root:本地目录
      remote:远地目录
      dest:远地主机
      nMode:MODE_NONE_DIR：在无地和本地都不考虑子目录
            MODE_LOCAL_DIR:插索本地子目录有效，否则不考虑子目录
            MODE_REMOTE_DIR：建立相应的远方子目录
            可能过‘|’操作复选；
            
返回：0:成功，否则失败
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
    	//如果是'.','..'则不予理采
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
函数：long RegisteFile;
目的：将待传输的文件登记到文件传输登记表中(bk$wjcsdj),并拒绝重复
      登记(CLOCALFILE为主键)。
参数：
      long nOpt: 0:发送文件，1:接收文件
      char * sRemoteHost:远程主机地址或别名
      char * sLocalFile:本地文件
      char * sRemoteFile:远方文件

返回：>0:登记成功
      =0:重复登记
      <0:登记失败
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
     	HiError(nRetVal,"文件登记出错！");
        RollbackWork();
     	return -3;
     }
     CommitWork();
     return 1;
}


//省发行局将省中心文件接收后并将它移去
long MoveRemoteAfterReceive(char * sRemoteHost,char * sRemoteFile,char * sBackupDir)
{
     long nRetVal=-1;
     
     RunTask(sRemoteHost,TASK_MOVE_FILE_AFTERRECEIVE,"%s%s","%ld",
             sRemoteFile,sBackupDir,&nRetVal);
    
     return nRetVal;                      
}


//省局搜索省中心目录并将所有登记的文件在本地也登记
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
  	sprintf(sTcpipErrInfo,"无法与远程主机\"%s\"的\"%s\"服务建立连接",
  	        sRemoteHost,sSysDsService);
  	return -1;
  }

  if(SendToSocket(nSocket,"%ld%ld%s",(long)TASK_SEARCH_ALL_FILE,nMode,sRemoteDir))
  {
      sprintf(sTcpipErrInfo,"发送任务号失败");
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
函数：long DealwithRegisteredFile;
目的：将文件传输登记表中所登记的但未传输成功的文件传输给远方或接
      收到本地。
参数：long nOpt:0:处理发送文件，1:处理接收文件
      char * sBackupDir:传输成功后将文件移至该目录[发送时使用];
返回：>0:成功
      =0:没有文件可传输
      <0:某些文件或全部文件没传输成功。
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
     "|  |本地文件名                              |目的地          |状态      |",
     "%-2.2s%-40.40s%-16.16s%-10.10s%s");
     if(!pdt) return -1;
     MergeTable(pdt->pTable,CreateTable(1,
     "|                                                                          |"));
     
     clrscr();
     if(nOpt==0) outtext("                            发 送 文 件");
     else outtext("                            接 收 文 件");
     ShowDataTable(pdt,1,3);
     ShowDatatableInfo(pdt,"正在提取未传输的文件...",0);
     
     nId=OpenCursor("select cremotehost,clocalfile,cremotefile from bk$wjcsdj \
                     where ncsfx=%ld and nstatus=0 order by cremotehost",nOpt);
     if(nId<=0)
     {
     	HiError(nId,"提取文件出错!");
     	DropDataTable(pdt);
     	return -2;
     }
     for(;;)
     {
     	if(FetchCursor(nId,"%s%s%s",sRemoteHost,sLocalFile,sRemoteFile)<=0) break;
        ImportData(pdt,pdt->nCount+1,"%s%s%s%s%s",
                       " ",sLocalFile,sRemoteHost,"未传输",sRemoteFile);
     }
     CloseCursor(nId);
     
     if(BrowseRegistedTable(pdt)==KEY_ESC)
     {
     	DropDataTable(pdt);
     	return -2;
     }

     for(nId=1;nId<=pdt->nCount;nId++)
     {
     	ImportData(pdt,nId,"%s","★");
     	if(nId>pdt->nCurrent+pdt->nPage) ScrollTable(pdt,pdt->nPage);
     	ExportData(pdt,nId,"> %s %s > %s",sLocalFile,sRemoteHost,sRemoteFile);
     	alltrim(sLocalFile);
     	alltrim(sRemoteHost);
     	alltrim(sRemoteFile);
        if(nId==1) strcpy(sTempHost,sRemoteHost);
     	if(strcmp(sTempHost,sRemoteHost)||nSysSocket<=0)
     	{
             sprintf(sbuf,"正在连接远程主机\"%s\"的\"%s\"服务...",
                     sRemoteHost,sSysDsService);       
             ShowDatatableInfo(pdt,sbuf,0);
     	     DisconnectServer();     	
     	     nSysSocket=0;
     	     ConnectServer(sRemoteHost);
     	     if(nSysSocket<=0)
     	     {
                 sprintf(sbuf,"严重异常：无法连接主机\"%s\"...",sRemoteHost);
                 ShowDatatableInfo(pdt,sbuf,1);
                 ImportData(pdt,nId," %s > > %s","Ｘ","连接失败");
                 nError=-1;
                 continue;
             }
        }
        strcpy(sTempHost,sRemoteHost);
        
        if(nOpt==0)
        {  
             ShowDatatableInfo(pdt,"正在进行远地注册...",0);
             nRetVal=RegisteRemote(sRemoteHost,sLocalFile,sRemoteFile);
             if(nRetVal<0)
             {   
                sprintf(sbuf,"严重异常：无法在主机\"%s\"上进行文件注册...",sRemoteHost);
                ShowDatatableInfo(pdt,sbuf,1);
             	ImportData(pdt,nId," %s > > %s","Ｘ","注册失败");
             	nError=-1;
             	continue;
             }
             if(nRetVal==0) //远方文件已传成功，只本地未更改
             {
             	ImportData(pdt,nId," %s > > %s","√","成功");
             	MoveRegisteredFile(sLocalFile,sBackupDir);
             	continue;
             }
             nRetVal=SendFileToRemote(sLocalFile,sRemoteFile,1);
        }
        else nRetVal=ReceiveFileFromRemote(sRemoteFile,sLocalFile,0);

        if(nRetVal)
        {   
             ImportData(pdt,nId,"%s > > %s","Ｘ","传输失败");
             switch(nRetVal)
             {
             	case 1:
             	     strcpy(sbuf,"严重错误:网络出错...");
             	     break;
             	case -1:
             	     sprintf(sbuf, "严重错误:远方文件'%s'文件长度为0...",sRemoteFile);
             	     break;
             	case -2:
             	     strcpy(sbuf,"严重错误:文件不存在...");
             	     break;
                case -3:
                     strcpy(sbuf,"严重错误:不能写临时文件...");
                     break;
                case -4:
                     strcpy(sbuf,"严重错误:移动文件出错...");
                     break;
                case -5:
                     strcpy(sbuf,"严重错误:连接没有建立...");
                     break;
                default:strcpy(sbuf,"未知的错误...");      
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
             ImportData(pdt,nId,"%s > > %s","Ｘ","确认失败");
             nError=-1;
             continue;
        }
        CommitWork();
        ImportData(pdt,nId,"%s > > %s","√","成功");
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
     
     ShowDatatableInfo(pdt,"按上下光标键翻页,Enter开始下一步,ESC/0返回..",0);
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
函数：long MoveRegisteredFile
目的：将传送成功的文件移到备份目录下并将文件传输登记表中该文件的
      状态置1(nstatus=1);
参数：char * sLocalFile:本地文件名（应与登记表中的完全一致）
      char * sBackupDir:传输成功后将文件移至该目录;
返回：>0:成功
      <0:失败
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
     	outtext("\n无法将文件%s移动到%s,请手工移动它.",sLocalFile,sDirectory);
     	keyb();
     	popwin();
        return -1;
     }	
     return 1;
}



/*=============================================================
函数：long RegisteRemote
目的：1.完成远方文件登记，若远方未登记则登记并返回1；
      2.若远方已登记但标状态为未成功，则返回1；
      3.若远方文件已登记为成功接收则返回0；
参数：char * sRemoteHost:远程主机名
      char * sLocalFile:本地文件名（应与登记表中的完全一致）
      char * sBackupDir:传输成功后将文件移至该目录;
返回：>0:登记成功，可以开始传文件
      =0:文件已经传输成功，不必再传送文件
      <0:登记失败 
*=============================================================*/
static long RegisteRemote(char * sRemoteHost,char * sLocalFile,char * sRemoteFile)
{
   long nRetVal;
   char sLocalIp[21]=".";
   
   nRetVal=-1;//初始状态为-1
   RunSelect("select ccs from bk$bkfxcsfb where cjh=%s and nbh=97 into %s",
               sSysJh,sLocalIp);     
   
   RunTask(sRemoteHost,TASK_REGISTE_FILE,"%s%s%s","%ld",
           sLocalFile,sRemoteFile,sLocalIp,&nRetVal);
   
   return nRetVal;        
   

}

/*=============================================================
函数：long RegisteRemoteSuccessed
目的：将远方文件登记为已成功；
      1.若远方没有原始登记则返回-1；
      2.若远方登记为未传输成功状态，先检查本地文件和远方文件是
        否一致，若一致则将远地标识为已成功并返回1，若不一致则返回-1；
      2.若远方登记为已传输成功，则返回0；
参数：char * sRemoteHost:远程主机名
      char * sLocalFile:本地文件名（应与登记表中的完全一致）
      char * sBackupDir:传输成功后将文件移至该目录;
返回：>0:登记成功
      =0:已经成功
      <0:登记失败 
*=============================================================*/
static long RegisteRemoteSuccessed(char * sRemoteHost,char * sLocalFile,char * sRemoteFile)
{
   long nRetVal,nFileSize;
   struct stat filestat;
   
   nRetVal=-1;  //初始状态为-1
   
   if(access(sLocalFile,0)) return -1;
   stat(sLocalFile,&filestat);
   nFileSize=(long)filestat.st_size;
   
   RunTask(sRemoteHost,TASK_REGISTE_SUCCESSED,"%s%s%ld","%ld",
           sLocalFile,sRemoteFile,nFileSize,&nRetVal);
   
   return nRetVal;        
}


#else  //服务端

extern char sSysPassword[41];  //FZX口令
/*=============================================================
函数：long dsTaskRegisteRemote[TASK_REGISTE_REMOTE]
目的：在远方登记上传文件请求
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
     	SendToSocket(nSocket,"%ld%s",-1,"无法连接数据库，可能是口令有错!");
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
     //已经登记但未传送
     if(nStatus==0)
     {
        SendToSocket(nSocket,"%ld%ld",0,1);
        return 0;
     }
     //已经登记并传送成功
     SendToSocket(nSocket,"%ld%ld",0,0);
     return 0;
}

/*=============================================================
函数：long dsTaskRegisteRemoteSuccessed[TASK_REGISTE_REMOTE_SUCCESSED]
目的：修改文件成功传输标志
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
     	SendToSocket(nSocket,"%ld%s",-1,"无法连接数据库，可能是口令有错!");
     	return 0;
     }
     
     nRetVal=RunSelect("select nstatus from bk$wjcsdj where cremotefile=%s \
                       and ncsfx=1 into %ld",sLocalFile,&nStatus);
     if(nRetVal<=0)
     {
     	//没有原始登记
     	CloseDatabase();
     	SendToSocket(nSocket,"%ld%ld",0,-1);
     	return 0;
     }
     
     if(nStatus==0)
     {
        //有原始正常登记
        if(access(sRemoteFile,0))
        {
             //有登记但没有相应文件
       	     CloseDatabase();
     	     SendToSocket(nSocket,"%ld%ld",0,-2);
     	     return 0;
     	}
        stat(sRemoteFile,&filestat);
        if(nFileSize!=(long)filestat.st_size)
        {
       	     //文件字节数不符
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
函数：long dsTaskSearchAllFileName[TASK_SEARCH_ALL_FILE]
目的：搜索数据库中或指定目录中的文件
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
  	SendToSocket(nSocket,"%ld%s",-1,"无法连接数据库，可能是口令有错!");
     	return 0;
     }
     if(!(pdir=opendir(sDirectory)))
     {
        CloseDatabase();
     	SendToSocket(nSocket,"%ld%s",(long)-3,"无法打开指定目录");
     	return 0;
     }
     
     while((pdirent=readdir(pdir))&&!nRetVal)
     {
    	//如果是'.','..'则不予理采
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
           //登记表中没有
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
  	SendToSocket(nSocket,"%ld%s",-1,"无法连接数据库，可能是口令有错!");
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
