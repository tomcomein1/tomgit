/*-------------------------------------------------------------

                    ��Զ�����������ļ�
                    
Original programmer���²�
Created Date�� 2000.11.30                    



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
������void ExchangeFile;
���ܣ���Զ�����������ļ�
������sConfiguration:���������ļ������ʽ���£�
      
      ���Ƿ����ļ��� 
      sendfile=host;localdir;remotedir;flag;
      
         ���У�host:Զ������IP��ַ�������
               localdir:�跢�͵ı����ļ�����Ŀ¼��
               remotedir:Զ��Ŀ¼��
               flag=1��������ԭ���ļ���flag=0��ʾ������
      ���ǽ���Զ���ļ���
      getfile=host;localdir;remotedir;flag;
         ���и������������뷢���ļ�����һ�¡�
          
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
    	sprintf(sMsg,"\n���������ļ�%s�����ڣ�",sConfiguration);
    	return;
    }
    
    if(!(fpr=fopen(sConfiguration,"rt")))
    {
    	sprintf(sMsg,"\n�޷��򿪽��������ļ�%s��",sConfiguration);
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
// ȡ�ļ��ô���ԭ��
void  ReasonReceiveFileFromRemote(long nId,char* sReason)
{
     switch(nId)
     {
     	case 0 :
     	     strcpy(sReason,"ȡ�ļ��ɹ�!");
     	     break;
     	case 1 :
     	     strcpy(sReason,"�������!");
     	     break;
     	case -1 :
     	     strcpy(sReason,"Զ���������ļ����ļ�����Ϊ0! ");
     	     break;
     	case -2 :
     	     strcpy(sReason,"ȡ������ʱ���ڶϵ��ļ�! ");
     	     break;
     	case -3 :
     	     strcpy(sReason,"ȡ������ʱ����д��ʱ�ļ�! ");
     	     break;
     	case -4 :
     	     strcpy(sReason,"�ƶ��ļ�����! ");
     	     break;
     	case -5 :
     	     strcpy(sReason,"���Ӳ��ܽ���!");
     	     break;
         default :
     	     strcpy(sReason,"���Ӳ��ܽ���!");
     	     break;
     }//end switch
     return;
}

/*-----------------------------------------------------------------
������long GetRemoteFile;
���ܣ���Զ������ȡ���ļ���
������sDest:Զ��������
      sLocalDir:����Ŀ¼+�ļ���
      sRemoteDir,Զ��Ŀ¼+�ļ���
      nFlag:���䷽ʽ,0:������  1:����
���أ�0:�ɹ�
     <0:ʧ��[ԭ����ο�log]
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
������long SendLocalFile
���ܣ����ͱ���Ŀ¼��Զ�����ļ���Զ��
������sDest:Զ��������
      sLocalDir:����Ŀ¼
      sRemoteDir,Զ��Ŀ¼
      nFlag:���䷽ʽ,0:������  1:����
���أ�0:�ɹ�
     <0:ʧ��[ԭ����ο�log]
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
    	sprintf(sMsg,"Ŀ¼���ļ���%s��������\n",sLocalDir);
    	return -1;
    }	

    stat(sLocalDir,&fstat);
    if(S_ISREG(fstat.st_mode))  //����Ǵ��ļ�
    {
    	strcpy(sLocalFile,sLocalDir);
        sprintf(sRemoteFile,sRemoteDir);
        
        printf("\n���ڷ����ļ� %s �� %s�����Ժ�...\n",sLocalDir,sDest);
        
        rec.filename=sLocalFile;
        rec.fp=(FILE *)1;
        rec.nsize=fstat.st_size;
        rec.sdata=NULL;
        rec.scurrent=NULL;
        
        nRetVal=AutoRunTask(sDest,TASK_SEND_FILE_TO_REMOTE,
                            "%s%r","%ld",sRemoteFile,rec,&nWhy);
        
        if(nRetVal||nWhy)
        {
        	sprintf(sMsg,"�����ļ� %s ʧ�ܣ�\nʧ�ܴ��룺%ld\n",
                             sLocalFile,nRetVal);
                return -3;
        }
            	                 	
      	sprintf(sMsg,"�����ļ���%s���ɹ�\n",sLocalFile);
    
        HintError(TAKE_POINT,sMsg);
        return 0;
    }

    if(S_ISDIR(fstat.st_mode))
       SearchFileTree(sLocalDir,sRemoteDir,sDest,nFlag);
    
    return 0;
    
}

//��ָ��Զ��������bkfxd����������
long ConnectBkfxSrv(char * sDest)
{
    char sMsg[100];
    

    DisconnectBkfxSrv();
    
    printf("\nConnecting to %s bkfx service...",sDest);
    nSysSocket=connectTCP(sDest,"bkfx");

    
    if(nSysSocket<0)
    {
    	sprintf(sMsg,"�޷���Զ��������%s����������\n",sDest);
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
������long SearchFileTree;
���ܣ����������������Ŀ¼�������������ļ����͵�Զ������
������bak: ����Ŀ¼
      root:����Ŀ¼
      dest,Զ��Ŀ¼
      flag:���䷽ʽ,0:������  1:����
���أ�0:�ɹ�
     <0:ʧ��[ԭ����ο�log]
*---------------------------------------------------------*/

int SearchFileTree(char * root,char * remote,char * dest,int flag)
{
   
    DIR * pdir;
    struct dirent * pdirent;
    struct stat fstat;
    struct stat statFile;
    char sCurFile[200];   //��ǰ��·�����ļ���
    char sOldFile[200];
    char sRemoteDir[200];
    char sShell[500];
    long nFlag,nWhy;
    RECORD rec;
    
    
    if(!(pdir=opendir(root))) return -2;  //can't open the directory
    while(pdirent=readdir(pdir))
    {
    	//�����'.','..'�������
    	if(!strcmp(pdirent->d_name,".")||!strcmp(pdirent->d_name,"..")) continue;
    	
    	sprintf(sCurFile,"%s/%s",root,pdirent->d_name);
        sprintf(sRemoteDir,"%s/%s",remote,pdirent->d_name);
        
        stat(sCurFile, &statFile);

        if(S_ISDIR(statFile.st_mode))
            SearchFileTree(sCurFile,remote,dest,flag);
            
    
        if(S_ISREG(statFile.st_mode))
        {
            //����Ǵ��ļ�������
            printf("\n���ڷ����ļ� %s �� %s�����Ժ�...\n",sCurFile,dest);

            rec.filename=sCurFile;
            rec.fp=(FILE *)1;
            rec.nsize=statFile.st_size;
            rec.sdata=NULL;
            rec.scurrent=NULL;
        
            nFlag=AutoRunTask(dest,TASK_SEND_FILE_TO_REMOTE,
                            "%s%r","%ld",sRemoteDir,rec,&nWhy);
            
            if(nFlag)
            {
        	sprintf(sShell,"�����ļ���%s��ʧ�ܣ�ʧ�ܴ��룺%ld\n",
                             sCurFile,nFlag);
                
            }

        }
  
    }//end while

    closedir(pdir);
    return 0;
}

//��Զ��������ִ��һ��SHELL����
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
������long SearchAndReceiveFile;
���ܣ�����Զ������Ŀ¼�е��ļ���������Щ�ļ����յ�������Ȼ��Զ������ 
      ���ļ��ƶ�������Ŀ¼�У�
������sDest:Զ��������
      sLocalDir:����Ŀ¼
      sRemoteDir,Զ��Ŀ¼
      nFlag:���䷽ʽ,0:������  1:����
���أ�0:�ɹ�
     <0:ʧ��[ԭ����ο�log]
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
                 sprintf(sShell,"�������");
    	         nError=-1;
    	         break;
    	    case -2:
                 sprintf(sShell,"����û�н�����");
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

