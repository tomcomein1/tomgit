/*-------------------------------------------------------------

                    与远程主机交换文件
                    
Original programmer：陈博
Created Date： 2000.11.30                    



*-------------------------------------------------------------*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include"dlpublic.h"
#include"netmain.h"
#include"filechar.h"
#include"exchfile.h"
#include"tasktool.h"
#include"newtask.h"
#ifdef UNIX
#include<dirent.h>
#else
#include<direct.h>
#endif


extern int   nSysSocket;
/*-----------------------------------------------------------------
函数：void ExchangeFile;
功能：与远程主机交换文件
参数：sConfiguration:交换配置文件，其格式如下：
      
      若是发送文件： 
      sendfile=host;localdir;remotedir;flag;
      
         其中：host:远程主机IP地址或别名；
               localdir:需发送的本地文件名或目录；
               remotedir:远方目录；
               flag=1表增覆盖原有文件，flag=0表示不覆盖
      若是接收远方文件：
      getfile=host;localdir;remotedir;flag;
         其中各参数的意义与发送文件配置一致。
          
*-----------------------------------------------------------------*/

void ExchangeFile(char * sConfiguration)
{
    char sMsg[200];
    char sLocalDir[100];
    char sRemoteDir[100];
    char sRemoteHost[31];
    char sBuffer[400];
    char * sOffset;
    long nFlag;
    FILE * fpr;
    
    
    if(access(sConfiguration,0))
    {
    	sprintf(sMsg,"\n交换配置文件%s不存在！",sConfiguration);
    	return;
    }
    
    if(!(fpr=fopen(sConfiguration,"rt")))
    {
    	sprintf(sMsg,"\n无法打开交换配置文件%s！",sConfiguration);
    	return;
    }
    
    while(getline(sBuffer,fpr)>0)
    {
    	sOffset=strstr(sBuffer,"sendfile=");
    	if(sOffset)
    	{
    		if(FetchStringToArg(sOffset+9,"%s;%s;%s;%1d",
    		               sRemoteHost,sLocalDir,sRemoteDir,&nFlag))
    		     SendLocalFile(sRemoteHost,sLocalDir,sRemoteDir,nFlag);
    		continue;
    	}
    	     
    	sOffset=strstr(sBuffer,"getfile=");
    	if(sOffset)
  	    if(FetchStringToArg(sOffset+8,"%s;%s;%s;%1d",
  	                   sRemoteHost,sLocalDir,sRemoteDir,&nFlag))
    		     GetRemoteFile(sRemoteHost,sLocalDir,sRemoteDir,nFlag);
    } 		
    fclose(fpr);
}
// 取文件得错误原因
void  ReasonReceiveFileFromRemote(long nId,char* sReason)
{
     switch(nId)
     {
     	case 0 :
     	     strcpy(sReason,"取文件成功!");
     	     break;
     	case 1 :
     	     strcpy(sReason,"网络出错!");
     	     break;
     	case -1 :
     	     strcpy(sReason,"远方主机无文件或文件长度为0! ");
     	     break;
     	case -2 :
     	     strcpy(sReason,"取至本机时存在断点文件! ");
     	     break;
     	case -3 :
     	     strcpy(sReason,"取至本机时不能写临时文件! ");
     	     break;
     	case -4 :
     	     strcpy(sReason,"移动文件出错! ");
     	     break;
     	case -5 :
     	     strcpy(sReason,"连接不能建立!");
     	     break;
         default :
     	     strcpy(sReason,"连接不能建立!");
     	     break;
     }//end switch
     return;
}

/*-----------------------------------------------------------------
函数：long GetRemoteFile;
功能：从远程主机取得文件；
参数：sDest:远程主机名
      sLocalDir:本地目录+文件名
      sRemoteDir,远地目录+文件名
      nFlag:传输方式,0:不覆盖  1:覆盖
返回：0:成功
     <0:失败[原因请参看log]
*-----------------------------------------------------------------*/
long GetRemoteFile(char * sDest,char * sLocalDir,char * sRemoteDir,int nFlag)
{
    char sBuffer[100];
    char sShell[100];
    char sReason[101];
    long nRetVal;
    char * sPureDir;
    RECORD rec;
    #ifdef UNIX 
    if(strchr(sLocalDir,'/'))
    {
        strcpy(sBuffer,sLocalDir);
        sPureDir=sBuffer;
        
        while(strchr(sPureDir,'/')) sPureDir=strchr(sPureDir,'/')+1;
        sBuffer[sPureDir-sBuffer-1]='\0';


        if(access(sBuffer,0))
            mkdir(sBuffer,0777);
  
    }
    #endif
        
    if(nFlag==0&&!access(sLocalDir,0))
    {
        return 0;
    }

    if(AutoRunTask(sDest,TASK_GET_FILE_FROM_REMOTE,
                "%s","%ld%r",sRemoteDir,&nRetVal,&rec)) return -1;
    fclose(rec.fp);
    rename(rec.filename,sLocalDir);
    if(access(sLocalDir,0)) return -1;
    return 0;
}


/*-----------------------------------------------------------------
函数：long SendLocalFile
功能：发送本地目录到远方或文件到远方
参数：sDest:远程主机名
      sLocalDir:本地目录
      sRemoteDir,远地目录
      nFlag:传输方式,0:不覆盖  1:覆盖
返回：0:成功
     <0:失败[原因请参看log]
*-----------------------------------------------------------------*/
long SendLocalFile(char * sDest,char * sLocalDir,char * sRemoteDir,int nFlag)
{
	
    char sMsg[200];
    struct stat fstat;
    char sRemoteFile[100];
    char sLocalFile[100];
    char * sFind;
    long nRetVal,nWhy;
    RECORD rec;
        
    if(access(sLocalDir,0))
    {
    	sprintf(sMsg,"目录或文件“%s”不存在\n",sLocalDir);
    	return -1;
    }	

    stat(sLocalDir,&fstat);
    if(S_ISREG(fstat.st_mode))  //如果是纯文件
    {
    	strcpy(sLocalFile,sLocalDir);
        sprintf(sRemoteFile,sRemoteDir);
        
        printf("\n正在发送文件 %s 到 %s，请稍候...\n",sLocalDir,sDest);
        
        rec.filename=sLocalFile;
        rec.fp=(FILE *)1;
        rec.nsize=fstat.st_size;
        rec.sdata=NULL;
        rec.scurrent=NULL;
        
        nRetVal=AutoRunTask(sDest,TASK_SEND_FILE_TO_REMOTE,
                            "%s%r","%ld",sRemoteFile,rec,&nWhy);
        
        if(nRetVal||nWhy)
        {
        	sprintf(sMsg,"传输文件 %s 失败，\n失败代码：%ld\n",
                             sLocalFile,nRetVal);
                return -3;
        }
            	                 	
      	sprintf(sMsg,"传输文件“%s”成功\n",sLocalFile);
    
        HintError(TAKE_POINT,sMsg);
        return 0;
    }

    if(S_ISDIR(fstat.st_mode))
       SearchFileTree(sLocalDir,sRemoteDir,sDest,nFlag);
    
    return 0;
    
}

//与指按远程主机的bkfxd服务建立连接
long ConnectBkfxSrv(char * sDest)
{
    char sMsg[100];
    

    DisconnectBkfxSrv();
    
    printf("\nConnecting to %s bkfx service...",sDest);
    nSysSocket=connectTCP(sDest,"bkfx");

    
    if(nSysSocket<0)
    {
    	sprintf(sMsg,"无法与远程主机“%s”建立连接\n",sDest);
    	return 0;
    }
    return 1;
}


long DisconnectBkfxSrv()
{
    
    
    if(nSysSocket<=0) return -1;
    
    SendLongToSocket(nSysSocket,0);
    close(nSysSocket);
    nSysSocket=0;
    return 1;


}

/*---------------------------------------------------------
函数：long SearchFileTree;
功能：深度优先搜索本机目录，将搜索到的文件发送到远程主机
参数：bak: 备分目录
      root:本地目录
      dest,远地目录
      flag:传输方式,0:不覆盖  1:覆盖
返回：0:成功
     <0:失败[原因请参看log]
*---------------------------------------------------------*/

int SearchFileTree(char * root,char * remote,char * dest,int flag)
{
   
    DIR * pdir;
    struct dirent * pdirent;
    struct stat fstat;
    struct stat statFile;
    char sCurFile[200];   //当前的路径及文件名
    char sOldFile[200];
    char sRemoteDir[200];
    char sShell[500];
    long nFlag,nWhy;
    RECORD rec;
    
    
    if(!(pdir=opendir(root))) return -2;  //can't open the directory
    while(pdirent=readdir(pdir))
    {
    	//如果是'.','..'则不予理采
    	if(!strcmp(pdirent->d_name,".")||!strcmp(pdirent->d_name,"..")) continue;
    	
    	sprintf(sCurFile,"%s/%s",root,pdirent->d_name);
        sprintf(sRemoteDir,"%s/%s",remote,pdirent->d_name);
        
        stat(sCurFile, &statFile);

        if(S_ISDIR(statFile.st_mode))
            SearchFileTree(sCurFile,remote,dest,flag);
            
    
        if(S_ISREG(statFile.st_mode))
        {
            //如果是纯文件，则发送
            printf("\n正在发送文件 %s 到 %s，请稍候...\n",sCurFile,dest);

            rec.filename=sCurFile;
            rec.fp=(FILE *)1;
            rec.nsize=statFile.st_size;
            rec.sdata=NULL;
            rec.scurrent=NULL;
        
            nFlag=AutoRunTask(dest,TASK_SEND_FILE_TO_REMOTE,
                            "%s%r","%ld",sRemoteDir,rec,&nWhy);
            
            if(nFlag)
            {
        	sprintf(sShell,"传输文件“%s”失败，失败代码：%ld\n",
                             sCurFile,nFlag);
                
            }

        }
  
    }//end while

    closedir(pdir);
    return 0;
}

//在远程主机上执行一条SHELL命令
long ExecCommand(char * command)
{
    long nRetVal;
    
    if(nSysSocket<=0) return -4;
    
    if(SendLongToSocket(nSysSocket,TASK_COMMAND)) return -1;
    if(SendStringToSocket(nSysSocket,command)) return -2;
    
    if(GetLongFromSocket(nSysSocket,&nRetVal)) return -3;
    
    return nRetVal;
	
}

/*-----------------------------------------------------------------
函数：long SearchAndReceiveFile;
功能：搜索远程主机目录中的文件，并将这些文件接收到本机，然后将远方主机 
      的文件移动到备份目录中；
参数：sDest:远程主机名
      sLocalDir:本地目录
      sRemoteDir,远地目录
      nFlag:传输方式,0:不覆盖  1:覆盖
返回：0:成功
     <0:失败[原因请参看log]
*-----------------------------------------------------------------*/
long SearchAndReceiveFile(char * sDest,char * sLocalDir,char * sRemoteDir,int nFlag)
{
      
    struct stat fstat;
    char sRemoteBak[100];
    char sRemoteFile[81];
    char sRealRemoteFile[100];
    char sRealLocalFile[100];
    char sShell[200];
    long nRetVal;
    int nError=0;

    if(access(sLocalDir,0))
        mkdir(sLocalDir,0777);
    
    if(ConnectBkfxSrv(sDest)==0) return -1;
    
    sprintf(sRemoteBak,"%sold",sRemoteDir);         
    sprintf(sShell,"cd %s>null",sRemoteBak);
    if(ExecCommand(sShell))
    {
    	
         sprintf(sShell,"mkdir -p %s>null",sRemoteBak);
         ExecCommand(sShell);
    }
    
    for(;;)
    {
    	
    	nRetVal=GetOneFileName(sRemoteDir,sRemoteFile);
    	switch(nRetVal)
    	{
    	    case 1:
                 sprintf(sShell,"网络出错！");
    	         nError=-1;
    	         break;
    	    case -2:
                 sprintf(sShell,"连接没有建立！");
    	         nError=-2;
                 break;
        }
        
        if(nRetVal==-1) break;
                 
    	
    	sprintf(sRealRemoteFile,"%s/%s",sRemoteDir,sRemoteFile);
    	sprintf(sRealLocalFile,"%s/%s",sLocalDir,sRemoteFile);
            	
    	nRetVal=ReceiveFileFromRemote(sRealRemoteFile,sRealLocalFile,nFlag);
    	if(nRetVal)
    	{
             ReasonReceiveFileFromRemote(nRetVal,sShell);
             nError=nRetVal;
             break;
        }     
         
        sprintf(sRemoteBak,"%sold/%s",sRemoteDir,sRemoteFile);
        MoveRemoteFile(sRealRemoteFile,sRemoteBak);
   }//end for;
   DisconnectBkfxSrv();
   return nError;
}

long ReplaceFile(char * sdest,char * sfilename)
{
    long nflag;
    char sremote[51],sShell[100]; 
    char * spurefile;

    strcpy(sremote,sfilename);
    spurefile=sremote;
    lower(sremote);
    while(strchr(spurefile,'\\')) spurefile++;
    nflag=GetRemoteFile(sdest,"replace.tmp",sfilename,1);
    if(nflag) return nflag;
    
    sprintf(sShell,"o%s",sfilename);
    rename(sfilename,sShell);
    rename("replace.tmp",sfilename);
    chmod(sfilename,0777);
    return 0;
}

