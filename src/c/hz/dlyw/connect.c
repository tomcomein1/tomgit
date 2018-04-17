#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dlpublic.h"
#include "netmain.h"
#ifdef UNIX
#include <sys/stat.h>
#include <dirent.h>
#endif
#ifdef DOS
#include <sys\stat.h>
#include <direct.h>
#endif

extern char sSysDsService[41];

int nSysSocket = 0;


/*------------------------------------------------------------------------
 * ConnectServer - TCP client for Database(BKFX) service
 *------------------------------------------------------------------------
 *
 * ������
 *     sRemoteHost �����ӵ�Զ������������
 * ����ֵ��> 0 ������Զ����������socket
 *         < 0 ����
 *            -1 �������ӳ���
 *            -2 �ѽ�����û�жϿ�
 */

int ConnectServer(char* sRemoteHost)
{
  int nSocket; // ���ص�socketֵ
  int nRetVal;
  double dVersion;
  long nKey;
  
  if(nSysSocket>0)
  {
    return -2;
  }
  nSysSocket = 0;

  #ifdef UNIX
  if(pingtest(sRemoteHost)==0)
  {
    return -1;
  }
  #endif 
  nSocket = connectTCP(sRemoteHost, sSysDsService);
  if(nSocket < 0)
    return -1;
  
  nSysSocket = nSocket;

  return nSocket;
}

/*
���ܣ��Ͽ�����
����ֵ��
     0 �����Ͽ�
    -1 ���ӳ���
*/

int DisconnectServer()
{
  if(SendLongToSocket(nSysSocket, TASK_NULL))
  {
    shutdown(nSysSocket,2);
    return -1;
  }
  close(nSysSocket);

  nSysSocket=0;
  SetWaitingTime(MAX_WAITING_TIME);
  
  return 0;
}

/*
���ܣ�
  ��ȡԶ��·���µ�һ���ļ���
������
  sPathName �������·����
  sFileName ��ŷ��ص�һ���ļ���
����ֵ��
    0 �ɹ�
    1 �������
   -1 Ŀ¼�����ļ�
   -2 ����û�н���
*/
int GetOneFileName(char* sPathName, char* sFileName)
{
  int socket;
  long nTaskNo;
  long bFileExist;

  sFileName[0] = '\0';
  if(nSysSocket<=0)
  {
    return -2;
  }
  socket=nSysSocket;

  nTaskNo = TASK_GETONEFILENAME;
  if(SendLongToSocket(socket,nTaskNo))
    return 1;

  if(SendStringToSocket(socket, sPathName))
    return 1;
  if(GetLongFromSocket(socket,&bFileExist))
    return 1;

  if(bFileExist == 0)
    return -1;

  if(GetStringFromSocket(socket,sFileName))
    return 1;
    
  return 0;
}

/*
���ܣ�
  �ƶ�Զ���ļ�
������
  sSource ԭ·��+�ļ���
  sDestination ��·��+�ļ���
����ֵ��
    0 �ɹ�
    1 �������
   -1 �ƶ�ʧ��
   -2 ����û�н���
*/
int MoveRemoteFile(char* sSource, char* sDestination)
{
  int socket;
  long nTaskNo;

  if(nSysSocket<=0)
  {
    return -2;
  }
  socket=nSysSocket;

  nTaskNo = TASK_MOVEFILE;
  if(SendLongToSocket(socket,nTaskNo))
    return 1;
  if(SendStringToSocket(socket, sSource))
    return 1;
  if(SendStringToSocket(socket, sDestination))
    return 1;
  if(GetLongFromSocket(socket,&nTaskNo))
    return 1;
  if(nTaskNo == 0)
    return -1;
  return 0;
}

/*
���ܣ�
  ����Զ���ļ�
������
  sRemoteFileName Զ��·��+�ļ���
  sLocalFileName ����·��+�ļ���
  nFlag ��־
    0 ���ܱ������޶ϵ��ļ���ͷ�����ļ�, �˷�ʽ�ȫ
    1 ���жϵ��ļ�������
    2 �ϵ�����
����ֵ
    0 ���ճɹ�
    1 �������
   -1 Զ���������ļ�sRemoteFileName���ļ�����Ϊ0
   -2 ���ڶϵ��ļ�
   -3 ����д��ʱ�ļ�
   -4 �ƶ��ļ�����
   -5 ����û�н���
*/
#ifdef UNIX
int ReceiveFileFromRemote(char* sRemoteFileName, char* sLocalFileName, int nFlag)
{
  int socket;
  long nTaskNo=TASK_BHCSWJ;
  FILE *fp;
  int nReadLen;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nRecvSize=0;
  long x,y;
  long nOffset;
  char sPureFileName[51];
  char sRealFileName[51];
  char sBakFileName[51];
  long nLeftSize;
  long nTemp;
  
//  clrscr();
  if(nSysSocket<=0)
  {
    return -5;
  }
  socket=nSysSocket;

  if(SendLongToSocket(socket,nTaskNo))
    return 1;

  if(SendStringToSocket(socket,sRemoteFileName))
    return 1;

  if(GetLongFromSocket(socket,&nTaskNo))
    return 1;

  if(nTaskNo==0)
    return -1;
  
  strcpy(sPureFileName, (strrchr(sLocalFileName, '/'))? strrchr(sLocalFileName, '/')+1:sLocalFileName);
  sprintf(sRealFileName, "tempfile/%s.___", sPureFileName);
  sprintf(sBakFileName, "tempfile/%s.bak", sPureFileName);

  if(access("tempfile",0))
    mkdir("tempfile",S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);

  if(nFlag)
  {
    if(!access(sRealFileName, 0))
    {
      if(nFlag == 1)
        return -2;
      umask(0002);
      if((fp=fopen(sRealFileName,"a+b"))==NULL)
      {
        SendLongToSocket(socket,0L);
        return -3;
      }
      fseek(fp, 0, SEEK_END);
      nRecvSize = ftell(fp);
    }
  }
  else
  {
    umask(0002);
    if((fp=fopen(sRealFileName,"w+b"))==NULL)
    {
      SendLongToSocket(socket,0L);
      return -3;
    }
    nRecvSize = 0;
  }

  if(SendLongToSocket(socket,1L))
  {
    fclose(fp);
    return 1;
  }

  if(GetLongFromSocket(socket,&nFileSize))
  {
    fclose(fp);
    return 1;
  }
//  gotoxy(20,10);
  outtext("\n    ���ڽ����ļ�%s...",sLocalFileName);
  getxy(&x,&y);

  for(;;)
  {
    if(SendLongToSocket(socket,-1L))
    {
      fclose(fp);
      return 1;
    }
    if(SendLongToSocket(socket,nRecvSize))
    {
      fclose(fp);
      return 1;
    }
    if(SendLongToSocket(socket,-2L - nRecvSize))
    {
      fclose(fp);
      return 1;
    }
  
    if(GetLongFromSocket(socket,&nTemp))
    {
      fclose(fp);
      return 1;
    }
    
    if(nTemp != -3L)
    {
      fclose(fp);
      return 1;
    }

    if(GetLongFromSocket(socket,&nTemp))
    {
      fclose(fp);
      return 1;
    }
    
    if(nTemp != -4L)
    {
      fclose(fp);
      return 1;
    }

    nLeftSize = nFileSize - nRecvSize;
    if(nLeftSize > BUFFERSIZE)
      nLeftSize = BUFFERSIZE;
      
    nOffset = 0;
    while(nLeftSize != 0)
    {
      if((nReadLen=read(socket, sBuffer + nOffset, nLeftSize))<0)
      {
        fclose(fp);
        return 1;
      }
      nOffset += nReadLen;
      nLeftSize -= nReadLen;
    }
    if(GetLongFromSocket(socket,&nTemp))
    {
      fclose(fp);
      return 1;
    }
    
    if(nTemp != -5L)
    {
      fclose(fp);
      return 1;
    }

    if(GetLongFromSocket(socket,&nTemp))
    {
      fclose(fp);
      return 1;
    }
    
    if(nTemp != -6L)
    {
      fclose(fp);
      return 1;
    }
    
    if(nOffset&&fwrite(sBuffer, 1,nOffset,fp)!=nOffset)
    {
      fclose(fp);
      return 1;
    }

    nRecvSize+=nOffset;

//    gotoxy(x,y);
    outtext("%ld%%",(long)(nRecvSize/(nFileSize*0.01)));

    if(nFileSize==nRecvSize)
    {
      fclose(fp);
      if(!access(sLocalFileName, 0))
      {
        if(rename(sLocalFileName, sBakFileName))
          return -4;
      }
      if(rename(sRealFileName, sLocalFileName))
        return -4;
      break;
    } 
  } 
  return 0;
}
#endif



#ifdef DOS
int ReceiveFileFromRemote(char* sRemoteFileName, char* sLocalFileName, int nFlag)
{
  int socket;
  long nTaskNo=TASK_BHCSWJ;
  FILE *fp;
  int nReadLen;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nRecvSize=0;
  long x,y;
  long nOffset;
  char sPureFileName[51];
  char sRealFileName[51];
  char sBakFileName[51];
  long nLeftSize;
  long nTemp;
  char* s;
  
//  clrscr();
  if(nSysSocket<=0)
  {
    return -5;
  }
  socket=nSysSocket;

  if(SendLongToSocket(socket,nTaskNo))
    return 1;

  if(SendStringToSocket(socket,sRemoteFileName))
    return 1;

  if(GetLongFromSocket(socket,&nTaskNo))
    return 1;

  if(nTaskNo==0)
    return -1;
  
  strcpy(sPureFileName, (strrchr(sLocalFileName, '\\'))? strrchr(sLocalFileName, '\\')+1:sLocalFileName);
  s = strchr(sPureFileName, '.');
  if(s)
    *s = '\0';
  sprintf(sRealFileName, "tempfile\\%s.___", sPureFileName);
  sprintf(sBakFileName, "tempfile\\%s.bak", sPureFileName);

  if(access("tempfile",0))
    mkdir("tempfile");

  if(nFlag)
  {
    if(!access(sRealFileName, 0))
    {
      if(nFlag == 1)
        return -2;
      if((fp=fopen(sRealFileName,"a+b"))==NULL)
      {
        SendLongToSocket(socket,0);
        return -3;
      }
      fseek(fp, 0, SEEK_END);
      nRecvSize = ftell(fp);
    }
  }
  else
  {
    if((fp=fopen(sRealFileName,"w+b"))==NULL)
    {
      SendLongToSocket(socket,0);
      return -3;
    }
    nRecvSize = 0;
  }

  if(SendLongToSocket(socket,1L))
  {
    fclose(fp);
    return 1;
  }

  if(GetLongFromSocket(socket,&nFileSize))
  {
    fclose(fp);
    return 1;
  }

  for(;;)
  {
    if(SendLongToSocket(socket,-1L))
    {
      fclose(fp);
      return 1;
    }
    if(SendLongToSocket(socket,nRecvSize))
    {
      fclose(fp);
      return 1;
    }
    if(SendLongToSocket(socket,-2L - nRecvSize))
    {
      fclose(fp);
      return 1;
    }
  
    if(GetLongFromSocket(socket,&nTemp))
    {
      fclose(fp);
      return 1;
    }
    
    if(nTemp != -3L)
    {
      fclose(fp);
      return 1;
    }

    if(GetLongFromSocket(socket,&nTemp))
    {
      fclose(fp);
      return 1;
    }
    
    if(nTemp != -4L)
    {
      fclose(fp);
      return 1;
    }

    nLeftSize = nFileSize - nRecvSize;
    if(nLeftSize > BUFFERSIZE)
      nLeftSize = BUFFERSIZE;
      
    nOffset = 0;
    while(nLeftSize != 0)
    {
      if((nReadLen=read(socket, sBuffer + nOffset, nLeftSize))<0)
      {
        fclose(fp);
        return 1;
      }
      nOffset += nReadLen;
      nLeftSize -= nReadLen;
    }
    if(GetLongFromSocket(socket,&nTemp))
    {
      fclose(fp);
      return 1;
    }
    
    if(nTemp != -5L)
    {
      fclose(fp);
      return 1;
    }

    if(GetLongFromSocket(socket,&nTemp))
    {
      fclose(fp);
      return 1;
    }
    
    if(nTemp != -6L)
    {
      fclose(fp);
      return 1;
    }
    
    if(nOffset&&fwrite(sBuffer, 1,nOffset,fp)!=nOffset)
    {
      fclose(fp);
      return 1;
    }

    nRecvSize+=nOffset;

    outtext(".");

    if(nFileSize==nRecvSize)
    {
      fclose(fp);
      if(!access(sLocalFileName, 0))
      {
        if(rename(sLocalFileName, sBakFileName))
          return -4;
      }
      if(rename(sRealFileName, sLocalFileName))
        return -4;
      break;
    } 
  } 
  return 0;
}
#endif



/*
���ܣ������ļ���Զ��
������
  sLocalFileName ����·��+�ļ���
  sRemoteFileName Զ��·��+�ļ���
  nflag
    0 Զ���ļ�����Ѵ��ڣ����أ��˷�ʽ�ȫ
    1 Զ���ļ�����Ѵ��ڣ��򸲸�
����ֵ��
    0 ���ͳɹ�
    1 �������
   -1 Զ������sRemoteFileName�ļ��Ѵ���
   -2 ����sLocalFileName�ļ����ɶ�
   -3 �����ļ�����Ϊ0
   -4 Զ����ʱ�ļ����ܱ��ƶ�
   -5 ����û�н���
*/
int SendFileToRemote(char* sLocalFileName, char* sRemoteFileName, int nFlag)
{
  int socket;
  long nTaskNo=TASK_BHSZWJ;
  FILE *fp;
  int nReadLen,nWriteLen,n;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nSendSize;
  long x,y;
  long nStartPosition, nVerifyPosition;

//  clrscr();
  if(nSysSocket<=0)
    return -5;
  socket=nSysSocket;

  if(SendLongToSocket(socket,nTaskNo))
    return 1;
    
  if(SendLongToSocket(socket,nFlag))
    return 1;

  if(SendStringToSocket(socket,sRemoteFileName))
    return 1;

  if(GetLongFromSocket(socket,&nTaskNo))
    return 1;

  if(nTaskNo==0)
  {
    return -1;
  }

  if((fp=fopen(sLocalFileName,"rb"))==NULL)
  {
    SendLongToSocket(socket,0L);
    return -2;
  }

  fseek(fp,0,SEEK_END);
  nFileSize=ftell(fp);
  if(nFileSize==0)
  {
    SendLongToSocket(socket,0L);
    fclose(fp);
    return -3;
  }
  fseek(fp,0,SEEK_SET);

  if(SendLongToSocket(socket,nFileSize))
  {
    fclose(fp);
    return 1;
  }

  nSendSize=0;


  for(;;)
  {
    if(GetLongFromSocket(socket,&nStartPosition))
    {
      fclose(fp);
      close(socket);
      return 1;
    }

    if(nStartPosition != -1L)
    {
      fclose(fp);
      close(socket);
      return 1;
    }

    if(GetLongFromSocket(socket,&nStartPosition))
    {
      fclose(fp);
      close(socket);
      return 1;
    }

    if(GetLongFromSocket(socket,&nVerifyPosition))
    {
      fclose(fp);
      close(socket);
      return 1;
    }
    
    if(nStartPosition + nVerifyPosition != -2L)
    {
      fclose(fp);
      close(socket);
      return 1;
    }
    fseek(fp,nStartPosition,SEEK_SET);
    nStartPosition = -3L;
    if(SendLongToSocket(socket,nStartPosition))
    {
      fclose(fp);
      close(socket);
      return 1;
    }
    nStartPosition = -4L;
    if(SendLongToSocket(socket,nStartPosition))
    {
      fclose(fp);
      close(socket);
      return 1;
    }

    nReadLen=fread(sBuffer, 1, BUFFERSIZE, fp);

    for (nWriteLen = 0; nWriteLen < nReadLen; nWriteLen+=n ) 
    {
      n=write(socket, sBuffer+nWriteLen, nReadLen-nWriteLen);
      if(n<0)
      {
        fclose(fp);
        return 1;
      }
    }

    nStartPosition = -5L;
    if(SendLongToSocket(socket,nStartPosition))
    {
      fclose(fp);
      close(socket);
      return 1;
    }
    nStartPosition = -6L;
    if(SendLongToSocket(socket,nStartPosition))
    {
      fclose(fp);
      close(socket);
      return 1;
    }

    nSendSize+=nReadLen;

    outtext(".");

    if(nSendSize==nFileSize)
    {
      fclose(fp);

      if(GetLongFromSocket(socket,&nTaskNo))
        return 1;
      if(nTaskNo == 0)
      	return -4;
      break;
    }
    if(nReadLen<BUFFERSIZE)
    {
      fclose(fp);
      return 1;
    }
  } 
  return 0;
}




