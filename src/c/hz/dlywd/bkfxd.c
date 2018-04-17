// 2001.6.29 李宝东 将sql语句通讯中出现错误的位置写入log/bkfxd.log
// 错误信息格式：
// 第一行为时间，如Fri Jun 29 16:18:20 2001
// 第二行为sql语句类型，如ConnectDatabase
// 第三行为通讯出错原因，如sUserName Get String error

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "bkfxd.h"
#include "netmain.h"
#include "bkfxtask.h"


/* 统计连接数 用于省中心 */
/*
#define COUNT_CONNECT_NUMBER
*/

/* 只提供接收发送文件服务 用于省中心 */
//#define NO_DATABASE_SERVICE

#ifdef COUNT_CONNECT_NUMBER
#include <fcntl.h>
#include <sys/types.h>
#include <sys/locking.h>
#endif

#define	QLEN		   5	/* maximum connection queue length	*/
long nSysLkrq,nSysSfm;

void	reaper(int);
char sIniFile[]="bkfxd.ini";
double dVersion; // main版本号
double dBktxVer; // 报刊通讯版本号
double dDsVersion = 3.03;
int nDebugFlag = 0; // 是否输出文本调试信息 在bkfxd.ini中debug=y 或 n，缺省为n

#ifdef COUNT_CONNECT_NUMBER
long nSumBytes;
#endif

long ReadIni(void);

int g_nMaxWaitingTime = MAX_WAITING_TIME;
int g_nMinWaitingTime = MIN_WAITING_TIME;
char sSysPassword[41];
void OneGetInitValue(char *,char*,char* ,char*);
void GetSysLkrq();

/*------------------------------------------------------------------------
 * main - Concurrent TCP server for BKFX service
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[], char* envp[])
{
  char	service[21] = "bkfx";	/* service name or port number	*/
  struct sockaddr_in fsin;	/* the address of a client	*/
  int	alen;			/* length of client's address	*/
  int	msock;			/* master server socket		*/
  int	ssock;			/* slave server socket		*/
  long  nTaskNo;
  long  nMakeDate;


  
  #ifdef COUNT_CONNECT_NUMBER
  time_t nStartTime;
  time_t nEndTime;
  #endif
  OneGetInitValue("bkfxd.ini","dsservice",service,"bkfx");

  nMakeDate=20010629;
  #ifdef NO_DATABASE_SERVICE
  outtext("BKFXDTX( dDsVersion: %.2lf nMakeDate: %ld  bind port: %s)  start...\n",dDsVersion,nMakeDate,service);
  #else
  outtext("BKFXD( dDsVersion: %.2lf nMakeDate: %ld  bind port: %s)  start...\n",dDsVersion,nMakeDate,service);
  #endif
  outtext("鸿志电子公司通讯解释进程启动(bkfxd),版本号: %.2lf 制作日期: %ld\n",dDsVersion,nMakeDate);

  
  strcpy(sSysPassword,"fxz");
  ReadIni();

  if(argc == 2)
  {
     strcpy(service, argv[1]);
  }

  switch (fork())
  {
    case 0:		/* child */
      break;
    case -1:
      errexit("fork: %s[%d]\n", strerror(errno),errno);
    default:	/* parent */
      exit(0);
  }
  setsid();
  signal(SIGHUP, SIG_IGN);
  signal(SIGCLD, reaper);
  
  #if !defined(IBMUNIX) && !defined(HPUNIX)
  putenv("NLS_LANG=Simplified Chinese");
  #endif

  msock = passiveTCP(service, QLEN);

 
  if(access("newdata",0))
    mkdir("newdata",S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);
  if(access("olddata",0))
    mkdir("olddata",S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);
  if(access("errdata",0))
    mkdir("errdata",S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);
  if(access("log",0))
    mkdir("log",S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);

  while (1) 
  {
    signal(SIGCLD,reaper);
    alen = sizeof(fsin);
    ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
    if (ssock < 0) continue;
  
    switch (fork()) 
    {
      case 0:		/* child */
        (void) close(msock);

        #ifdef COUNT_CONNECT_NUMBER
        nStartTime = time(0);
        CountConnect(1, 0, 0);
        nSumBytes = 0;
        #endif
        
        while(1)
        {
          SetWaitingTime(g_nMaxWaitingTime);
          if(GetLongFromSocket(ssock,&nTaskNo))
          {
            #ifdef COUNT_CONNECT_NUMBER
            CountConnect(3, 0, 0);
            #endif
            errexit("error: %s[]\n", strerror(errno));
          }

          if  (nTaskNo>15000)
          {
             printf("taskno=%ld",nTaskNo);
             switch(nTaskNo)
             {
             	case BK_NAME_INSERT:
                 switch(InsertFileName(ssock,nTaskNo))
                 {
                  case 1:
                    shutdown(ssock,2);
 
                   #ifdef COUNT_CONNECT_NUMBER
                   CountConnect(3, 0, 0);
                   #endif
                   exit(0);
                  case -1:
                  if(SendToSocket(ssock, "%ld %s", -1, "网络出错"))
                  {
                    shutdown(ssock,2);
 
                    #ifdef COUNT_CONNECT_NUMBER
                    CountConnect(3, 0, 0);
                    #endif
                    exit(0);
                  }
                  break;
                }
               break;

               case BK_NAME_CHECK:
                 switch(CheckFileName(ssock,nTaskNo))
                 {
                  case 1:
                    shutdown(ssock,2);
 
                   #ifdef COUNT_CONNECT_NUMBER
                   CountConnect(3, 0, 0);
                   #endif
                   exit(0);
                  case -1:
                  if(SendToSocket(ssock, "%ld %s", -1, "网络出错"))
                  {
                    shutdown(ssock,2);
 
                    #ifdef COUNT_CONNECT_NUMBER
                    CountConnect(3, 0, 0);
                    #endif
                    exit(0);
                  }
                  break;
                }
                break;
               	

                 case BK_UPDATE_CHECK:
                 switch(CheckFileUpdate(ssock,nTaskNo))
                 {
                  case 1:
                    shutdown(ssock,2);
 
                   #ifdef COUNT_CONNECT_NUMBER
                   CountConnect(3, 0, 0);
                   #endif
                   exit(0);
                  case -1:
                  if(SendToSocket(ssock, "%ld %s", -1, "网络出错"))
                  {
                    shutdown(ssock,2);
 
                    #ifdef COUNT_CONNECT_NUMBER
                    CountConnect(3, 0, 0);
                    #endif
                    exit(0);
                  }
                  break;
                }
                break;
 
             	case BK_NAME_UPDATE:
                 switch(UpdateFileName(ssock,nTaskNo))
                 {
                  case 1:
                    shutdown(ssock,2);
 
                   #ifdef COUNT_CONNECT_NUMBER
                   CountConnect(3, 0, 0);
                   #endif
                   exit(0);
                  case -1:
                  if(SendToSocket(ssock, "%ld %s", -1, "网络出错"))
                  {
                    shutdown(ssock,2);
 
                    #ifdef COUNT_CONNECT_NUMBER
                    CountConnect(3, 0, 0);
                    #endif
                    exit(0);
                  }
                  break;
                }
               break;
                         	
               case BK_CWXX_INSERT:
                 switch(InsertFileCwxx(ssock,nTaskNo))
                 {
                  case 1:
                    shutdown(ssock,2);
 
                   #ifdef COUNT_CONNECT_NUMBER
                   CountConnect(3, 0, 0);
                   #endif
                   exit(0);
                  case -1:
                  if(SendToSocket(ssock, "%ld %s", -1, "网络出错"))
                  {
                    shutdown(ssock,2);
 
                    #ifdef COUNT_CONNECT_NUMBER
                    CountConnect(3, 0, 0);
                    #endif
                    exit(0);
                  }
                  break;
                }
               break;               
              }         	
          }	

          switch(nTaskNo)
          { 
            case TASK_CSXTWJ:
              SetWaitingTime(g_nMinWaitingTime);
              if(csxtwj(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_BHCSXTWJ:
              SetWaitingTime(g_nMinWaitingTime);
              if(bhcsxtwj(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_CSXTDS:
              SetWaitingTime(g_nMinWaitingTime);
              if(csxtds(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_BHCSXTDS:
              SetWaitingTime(g_nMinWaitingTime);
              if(bhcsxtds(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_CSWJ:
              SetWaitingTime(g_nMinWaitingTime);
              if(cswj(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_BHCSBKTX:
              SetWaitingTime(g_nMinWaitingTime);
              if(bhcsbktx(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_BHCSWJ:
              SetWaitingTime(g_nMinWaitingTime);
              if(bhcswj(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_BHCSDDWJ:
              SetWaitingTime(g_nMinWaitingTime);
              if(bhcsddwj(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_FSWJ:
              SetWaitingTime(g_nMinWaitingTime);
              if(jswj(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_BHFSWJ:
              SetWaitingTime(g_nMinWaitingTime);
              if(bhjswj(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_BHFSBDWJ:
              SetWaitingTime(g_nMinWaitingTime);
              if(bhjsbdwj(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_SCWJ:
              SetWaitingTime(g_nMinWaitingTime);
              if(scwj(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_CSDD:
              break;
            case TASK_CWJM:
              SetWaitingTime(g_nMinWaitingTime);
              if(cwjm(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_COMMAND:
              SetWaitingTime(g_nMinWaitingTime);
              if(zxml(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;
            case TASK_BHSZWJ:
              SetWaitingTime(g_nMinWaitingTime);
              if(SendFileToRemote(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;

            case TASK_GETONEFILENAME:
              SetWaitingTime(g_nMinWaitingTime);
              if(GetOneFileName(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;

            case TASK_MOVEFILE:
              SetWaitingTime(g_nMinWaitingTime);
              if(MoveFile(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              SetWaitingTime(g_nMaxWaitingTime);
              break;

            #ifndef NO_DATABASE_SERVICE

            case TASK_CONNECTDATABASE:
              if(dsConnectDatabase(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_CLOSEDATABASE:
              dsCloseDatabase(ssock);
              close(ssock);

              #ifdef COUNT_CONNECT_NUMBER
              nEndTime = time(0);
              CountConnect(2, nEndTime - nStartTime, nSumBytes);
              #endif

              exit(0);
              break;
            case TASK_GETSQLERRNO:
              if(dsGetSqlErrNo(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_GETSQLERRINFO:
              if(dsGetSqlErrInfo(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_COMMITWORK:
              if(dsCommitWork(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_ROLLBACKWORK:
              if(dsRollbackWork(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_RUNSQL:
              if(dsRunSql(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_OPENCURSOR:
              if(dsOpenCursor(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_FETCHCURSOR:
              if(dsFetchCursor(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_FETCHCURSORARRAY:
              if(dsFetchCursorArray(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_CLOSECURSOR:
              if(dsCloseCursor(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_RUNSELECT:
              if(dsRunSelect(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            case TASK_RUNPROC:
              if(dsRunProc(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;

            case TASK_REGISTE_FILE:
                 dsTaskRegisteRemote(ssock);
                 close(ssock);
                 exit(0);

            case TASK_REGISTE_SUCCESSED:
                 dsTaskRegisteRemoteSuccessed(ssock);
                 close(ssock);
                 exit(0);
            case TASK_MOVE_FILE_AFTERRECEIVE:
                 dsTaskMoveRemoteAfterReceive(ssock);
                 close(ssock);
                 exit(0);
           
            case TASK_SEARCH_ALL_FILE:
                 dsTaskSearchAllFileName(ssock);
                 close(ssock);
                 exit(0);

            case TASK_SEARCH_FAILED_FILE:
                dsTaskSearchFailedFile(ssock);
                close(ssock);
                exit(0);
            case TASK_SQLPLUS:
              if(dsSqlplus(ssock)==1)
              {
                shutdown(ssock,2);

                #ifdef COUNT_CONNECT_NUMBER
                CountConnect(3, 0, 0);
                #endif

                exit(0);
              }
              break;
            #endif

            case TASK_NULL:
              close(ssock);

              #ifdef COUNT_CONNECT_NUMBER
              nEndTime = time(0);
              CountConnect(2, nEndTime - nStartTime, nSumBytes);
              #endif

              exit(0);
            default:
              shutdown(ssock,2);
 
              #ifdef COUNT_CONNECT_NUMBER
              CountConnect(3, 0, 0);
              #endif

              exit(0);
          }
        }
      default:	/* parent */
      	(void) close(ssock);
      	break;
    }
  }
}

// 旧方式接收main.Z
int csxtwj(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen,nWriteLen,n;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nSendSize;

  ReadIni(); 
  if((fp=fopen("main.Z","rb"))==NULL)
  {
    SendDoubleToSocket(socket,0.0);
    return 0;
  }
  fseek(fp,0,SEEK_END);
  nFileSize=ftell(fp);
  if(nFileSize==0)
  {
    SendDoubleToSocket(socket,0.0);
    fclose(fp);
    return 0;
  }
  fseek(fp,0,SEEK_SET);
  if(SendDoubleToSocket(socket,dVersion))
  {
    fclose(fp);
    close(socket);
    return 1;
  }

  if(GetLongFromSocket(socket,&nTaskNo))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  if(nTaskNo==0)
  {
    fclose(fp);
    return 0;
  }
  if(SendLongToSocket(socket,nFileSize))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  nSendSize=0;
  for(;;)
  {
    nReadLen=fread(sBuffer, 1, BUFFERSIZE, fp);

    for (nWriteLen = 0; nWriteLen < nReadLen; nWriteLen+=n ) 
    {
      n=write(socket, sBuffer+nWriteLen, nReadLen-nWriteLen);
      if(n<0)
      {
        fclose(fp);
        close(socket);
        return 1;
      }
    }
    nSendSize+=nReadLen;
    if(nSendSize==nFileSize)
    {
      fclose(fp);
      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
      break;
    }
    if(nReadLen<BUFFERSIZE)
    {
      fclose(fp);
      close(socket);
      return 1;
    }
  } 
  return 0;
}

// 接收main.Z
int bhcsxtwj(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen,nWriteLen,n;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nSendSize;
  long nStartPosition, nVerifyPosition;

  ReadIni(); 
  if((fp=fopen("main.Z","rb"))==NULL)
  {
    SendDoubleToSocket(socket,0.0);
    return 0;
  }
  fseek(fp,0,SEEK_END);
  nFileSize=ftell(fp);
  if(nFileSize==0)
  {
    SendDoubleToSocket(socket,0.0);
    fclose(fp);
    return 0;
  }
  fseek(fp,0,SEEK_SET);
  if(SendDoubleToSocket(socket,dVersion))
  {
    fclose(fp);
    close(socket);
    return 1;
  }

  SetWaitingTime(g_nMaxWaitingTime);
  if(GetLongFromSocket(socket,&nTaskNo))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  if(nTaskNo==0)
  {
    fclose(fp);
    return 0;
  }
  SetWaitingTime(g_nMinWaitingTime);
  if(SendLongToSocket(socket,nFileSize))
  {
    fclose(fp);
    close(socket);
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
        close(socket);
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
    if(nSendSize==nFileSize)
    {
      fclose(fp);
      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
      break;
    }
    if(nReadLen<BUFFERSIZE)
    {
      fclose(fp);
      close(socket);
      return 1;
    }
  } 
  return 0;
}

// 旧方式接收bkfxd.Z
int csxtds(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen,nWriteLen,n;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nSendSize;

  if((fp=fopen("bkfxd.Z","rb"))==NULL)
  {
    SendDoubleToSocket(socket,0.0);
    return 0;
  }
  fseek(fp,0,SEEK_END);
  nFileSize=ftell(fp);
  if(nFileSize==0)
  {
    SendDoubleToSocket(socket,0.0);
    fclose(fp);
    return 0;
  }
  fseek(fp,0,SEEK_SET);
  if(SendDoubleToSocket(socket,dDsVersion))
  {
    fclose(fp);
    close(socket);
    return 1;
  }

  if(GetLongFromSocket(socket,&nTaskNo))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  if(nTaskNo==0)
  {
    fclose(fp);
    return 0;
  }
  if(SendLongToSocket(socket,nFileSize))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  nSendSize=0;
  for(;;)
  {
    nReadLen=fread(sBuffer, 1, BUFFERSIZE, fp);

    for (nWriteLen = 0; nWriteLen < nReadLen; nWriteLen+=n ) 
    {
      n=write(socket, sBuffer+nWriteLen, nReadLen-nWriteLen);
      if(n<0)
      {
        fclose(fp);
        close(socket);
        return 1;
      }
    }
    nSendSize+=nReadLen;
    if(nSendSize==nFileSize)
    {
      fclose(fp);
      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
      break;
    }
    if(nReadLen<BUFFERSIZE)
    {
      fclose(fp);
      close(socket);
      return 1;
    }
  } 
  return 0;
}

// 接收bkfxd.Z
int bhcsxtds(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen,nWriteLen,n;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nSendSize;
  long nStartPosition, nVerifyPosition;

  ReadIni(); 
  if((fp=fopen("bkfxd.Z","rb"))==NULL)
  {
    SendDoubleToSocket(socket,0.0);
    return 0;
  }
  fseek(fp,0,SEEK_END);
  nFileSize=ftell(fp);
  if(nFileSize==0)
  {
    SendDoubleToSocket(socket,0.0);
    fclose(fp);
    return 0;
  }
  fseek(fp,0,SEEK_SET);
  if(SendDoubleToSocket(socket,dDsVersion))
  {
    fclose(fp);
    close(socket);
    return 1;
  }

  SetWaitingTime(g_nMaxWaitingTime);
  if(GetLongFromSocket(socket,&nTaskNo))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  if(nTaskNo==0)
  {
    fclose(fp);
    return 0;
  }
  SetWaitingTime(g_nMinWaitingTime);
  if(SendLongToSocket(socket,nFileSize))
  {
    fclose(fp);
    close(socket);
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
        close(socket);
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
    if(nSendSize==nFileSize)
    {
      fclose(fp);
      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
      break;
    }
    if(nReadLen<BUFFERSIZE)
    {
      fclose(fp);
      close(socket);
      return 1;
    }
  } 
  return 0;
}

// 接收bktx.Z
int bhcsbktx(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen,nWriteLen,n;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nSendSize;
  long nStartPosition, nVerifyPosition;

  ReadIni(); 
  if((fp=fopen("bktx.Z","rb"))==NULL)
  {
    SendDoubleToSocket(socket,0.0);
    return 0;
  }
  fseek(fp,0,SEEK_END);
  nFileSize=ftell(fp);
  if(nFileSize==0)
  {
    SendDoubleToSocket(socket,0.0);
    fclose(fp);
    return 0;
  }
  fseek(fp,0,SEEK_SET);
  if(SendDoubleToSocket(socket,dBktxVer))
  {
    fclose(fp);
    close(socket);
    return 1;
  }

  SetWaitingTime(g_nMaxWaitingTime);
  if(GetLongFromSocket(socket,&nTaskNo))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  if(nTaskNo==0)
  {
    fclose(fp);
    return 0;
  }
  SetWaitingTime(g_nMinWaitingTime);
  if(SendLongToSocket(socket,nFileSize))
  {
    fclose(fp);
    close(socket);
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
        close(socket);
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
    if(nSendSize==nFileSize)
    {
      fclose(fp);
      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
      break;
    }
    if(nReadLen<BUFFERSIZE)
    {
      fclose(fp);
      close(socket);
      return 1;
    }
  } 
  return 0;
}

// 旧方式接收文件
int cswj(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen,nWriteLen,n;
  char sReceiveFileName[33];
  char sBuffer[BUFFERSIZE];
  long nFileSize,nSendSize;

  if(GetStringFromSocket(socket,sReceiveFileName))
    return 1;

  if((fp=fopen(sReceiveFileName,"rb"))==NULL)
  {
    SendLongToSocket(socket,0);
    return 0;
  }
  fseek(fp,0,SEEK_END);
  nFileSize=ftell(fp);
  if(nFileSize==0)
  {
    SendLongToSocket(socket,0);
    fclose(fp);
    return 0;
  }
  fseek(fp,0,SEEK_SET);
  if(SendLongToSocket(socket,1))
  {
    fclose(fp);
    close(socket);
    return 1;
  }

  if(GetLongFromSocket(socket,&nTaskNo))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  if(nTaskNo==0)
  {
    fclose(fp);
    return 0;
  }
  if(SendLongToSocket(socket,nFileSize))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  nSendSize=0;
  for(;;)
  {
    nReadLen=fread(sBuffer, 1, BUFFERSIZE, fp);

    for (nWriteLen = 0; nWriteLen < nReadLen; nWriteLen+=n ) 
    {
      n=write(socket, sBuffer+nWriteLen, nReadLen-nWriteLen);
      if(n<0)
      {
        fclose(fp);
        close(socket);
        return 1;
      }
    }
    nSendSize+=nReadLen;
    if(nSendSize==nFileSize)
    {
      fclose(fp);
      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
      break;
    }

    if(nReadLen<BUFFERSIZE)
    {
      fclose(fp);
      close(socket);
      return 1;
    }
  } 
  return 0;
}

// 接收文件
int bhcswj(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen,nWriteLen,n;
  char sReceiveFileName[33];
  char sBuffer[BUFFERSIZE];
  long nFileSize,nSendSize;
  long nStartPosition, nVerifyPosition;

  if(GetStringFromSocket(socket,sReceiveFileName))
    return 1;

  if((fp=fopen(sReceiveFileName,"rb"))==NULL)
  {
    SendLongToSocket(socket,0);
    return 0;
  }
  fseek(fp,0,SEEK_END);
  nFileSize=ftell(fp);
  if(nFileSize==0)
  {
    SendLongToSocket(socket,0);
    fclose(fp);
    return 0;
  }
  fseek(fp,0,SEEK_SET);
  if(SendLongToSocket(socket,1))
  {
    fclose(fp);
    close(socket);
    return 1;
  }

  SetWaitingTime(g_nMaxWaitingTime);

  if(GetLongFromSocket(socket,&nTaskNo))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  SetWaitingTime(g_nMinWaitingTime);
  if(nTaskNo==0)
  {
    fclose(fp);
    return 0;
  }
  if(SendLongToSocket(socket,nFileSize))
  {
    fclose(fp);
    close(socket);
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
        close(socket);
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
    if(nSendSize==nFileSize)
    {
      fclose(fp);
      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
      break;
    }
    if(nReadLen<BUFFERSIZE)
    {
      fclose(fp);
      close(socket);
      return 1;
    }
  } 
  return 0;
}

// 接收定单文件, 发行局用
int bhcsddwj(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen,nWriteLen,n;
  char sReceiveFileName[43];
  char sFileName[43];
  char sBuffer[BUFFERSIZE];
  long nFileSize,nSendSize;
  long nStartPosition, nVerifyPosition;

  if(GetStringFromSocket(socket,sReceiveFileName))
    return 1;
  sprintf(sFileName,"newdata/%s",sReceiveFileName);

  if((fp=fopen(sFileName,"rb"))==NULL)
  {
    SendLongToSocket(socket,0L);
    return 0;
  }
  fseek(fp,0,SEEK_END);
  nFileSize=ftell(fp);
  if(nFileSize==0)
  {
    SendLongToSocket(socket,0L);
    fclose(fp);
    return 0;
  }
  fseek(fp,0,SEEK_SET);
  if(SendLongToSocket(socket,1L))
  {
    fclose(fp);
    close(socket);
    return 1;
  }

  SetWaitingTime(g_nMaxWaitingTime);

  if(GetLongFromSocket(socket,&nTaskNo))
  {
    fclose(fp);
    close(socket);
    return 1;
  }
  SetWaitingTime(g_nMinWaitingTime);
  if(nTaskNo==0)
  {
    fclose(fp);
    return 0;
  }
  if(SendLongToSocket(socket,nFileSize))
  {
    fclose(fp);
    close(socket);
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
        close(socket);
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
    if(nSendSize==nFileSize)
    {
      fclose(fp);
      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
      break;
    }
    if(nReadLen<BUFFERSIZE)
    {
      fclose(fp);
      close(socket);
      return 1;
    }
  } 
  return 0;
}

// 旧方式发送文件
int jswj(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nRecvSize=0;
  char sSendFileName[33];
  char sRealFileName[37];
  char sRelationalFileName[35];
  char sFileName[57];
  int nFileLen;

  if(GetStringFromSocket(socket,sSendFileName))
    return 1;

  if(strchr(sSendFileName, '*') || strchr(sSendFileName, '?'))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  nFileLen = strlen(sSendFileName);
  if(nFileLen > 2 && sSendFileName[nFileLen - 2] == '.' && sSendFileName[nFileLen - 1] == 'Z')
  {
    strncpy(sRelationalFileName,sSendFileName, nFileLen - 2);
    sRelationalFileName[nFileLen - 2] = '\0';
  }
  else
  {
    sprintf(sRelationalFileName, "%s.Z", sSendFileName);
  }

  if(!access(sSendFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  if(!access(sRelationalFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sFileName,"newdata/%s",sSendFileName);

  if(!access(sFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sFileName,"newdata/%s",sRelationalFileName);

  if(!access(sFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sFileName,"olddata/%s",sSendFileName);

  if(!access(sFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sFileName,"olddata/%s",sRelationalFileName);

  if(!access(sFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sRealFileName, "%s.___", sSendFileName);

  if((fp=fopen(sRealFileName,"w+b"))==NULL)
  {
    SendLongToSocket(socket,0);
    return 0;
  }
  if(SendLongToSocket(socket,1))
  {
    fclose(fp);
    return 1;
  }

  if(GetLongFromSocket(socket,&nFileSize))
  {
    fclose(fp);
    return 1;
  }

  if(nFileSize==0)
  {
    fclose(fp);
    return 0;
  }

  for(;;)
  {
    if((nReadLen=ReadSocket(socket, sBuffer, BUFFERSIZE))<0)
    {
      fclose(fp);
      return 1;
    }
    
    if(nReadLen&&fwrite(sBuffer, 1,nReadLen,fp)!=nReadLen)
    {
      fclose(fp);
      return 1;
    }

    nRecvSize+=nReadLen;
    if(nFileSize==nRecvSize)
    {
      fclose(fp);
      sprintf(sFileName,"newdata/%s",sSendFileName);
      rename(sRealFileName, sFileName);
      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
      break;
    } 
  } 
  return 0;
}

// 发送文件
int bhjswj(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nRecvSize=0;
  char sSendFileName[33];
  char sRealFileName[37];
  char sRelationalFileName[35];
  char sFileName[57];
  int nFileLen;
  long nTemp;
  long nOffset, nLeftSize;


  if(GetStringFromSocket(socket,sSendFileName))
    return 1;

  if(strchr(sSendFileName, '*') || strchr(sSendFileName, '?'))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  nFileLen = strlen(sSendFileName);
  if(nFileLen > 2 && sSendFileName[nFileLen - 2] == '.' && sSendFileName[nFileLen - 1] == 'Z')
  {
    strncpy(sRelationalFileName,sSendFileName, nFileLen - 2);
    sRelationalFileName[nFileLen - 2] = '\0';
  }
  else
  {
    sprintf(sRelationalFileName, "%s.Z", sSendFileName);
  }

  if(!access(sSendFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  if(!access(sRelationalFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sFileName,"newdata/%s",sSendFileName);

  if(!access(sFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sFileName,"newdata/%s",sRelationalFileName);

  if(!access(sFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sFileName,"olddata/%s",sSendFileName);

  if(!access(sFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sFileName,"olddata/%s",sRelationalFileName);

  if(!access(sFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sRealFileName, "%s.___", sSendFileName);

  if((fp=fopen(sRealFileName,"w+b"))==NULL)
  {
    SendLongToSocket(socket,0);
    return 0;
  }
  if(SendLongToSocket(socket,1))
  {
    fclose(fp);
    return 1;
  }

  if(GetLongFromSocket(socket,&nFileSize))
  {
    fclose(fp);
    return 1;
  }

  if(nFileSize==0)
  {
    fclose(fp);
    return 0;
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
      if((nReadLen=ReadSocket(socket, sBuffer + nOffset, nLeftSize))<0)
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

    if(nFileSize==nRecvSize)
    {
      fclose(fp);
      sprintf(sFileName,"newdata/%s",sSendFileName);
      if(rename(sRealFileName, sFileName) == 0)
        SendLongToSocket(socket, 1);
      else
        SendLongToSocket(socket, 0);

      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
        
      break;
    } 
  } 
  return 0;
}

// 接收变动文件
int bhjsbdwj(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nRecvSize=0;
  char sSendFileName[33];
  char sRealFileName[37];
  char sRelationalFileName[35];
  char sFileName[57];
  int nFileLen;
  long nTemp;
  long nOffset, nLeftSize;


  if(GetStringFromSocket(socket,sSendFileName))
    return 1;

  if(strchr(sSendFileName, '*') || strchr(sSendFileName, '?'))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  nFileLen = strlen(sSendFileName);
  if(nFileLen > 2 && sSendFileName[nFileLen - 2] == '.' && sSendFileName[nFileLen - 1] == 'Z')
  {
    strncpy(sRelationalFileName,sSendFileName, nFileLen - 2);
    sRelationalFileName[nFileLen - 2] = '\0';
  }
  else
  {
    sprintf(sRelationalFileName, "%s.Z", sSendFileName);
  }

  if(!access(sSendFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  if(!access(sRelationalFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  sprintf(sRealFileName, "%s.___", sSendFileName);

  if((fp=fopen(sRealFileName,"w+b"))==NULL)
  {
    SendLongToSocket(socket,0);
    return 0;
  }
  if(SendLongToSocket(socket,1))
  {
    fclose(fp);
    return 1;
  }

  if(GetLongFromSocket(socket,&nFileSize))
  {
    fclose(fp);
    return 1;
  }

  if(nFileSize==0)
  {
    fclose(fp);
    return 0;
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
      if((nReadLen=ReadSocket(socket, sBuffer + nOffset, nLeftSize))<0)
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

    if(nFileSize==nRecvSize)
    {
      fclose(fp);
      if(rename(sRealFileName, sSendFileName) == 0)
        SendLongToSocket(socket, 1);
      else
        SendLongToSocket(socket, 0);

      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
        
      break;
    } 
  } 
  return 0;
}

// 删除文件
int scwj(int socket)
{
  long nTaskNo;
  char sReceiveFileName[33];
  long nFileSize;
  time_t nFileDate;
  struct stat statFile;

  if(GetStringFromSocket(socket,sReceiveFileName))
    return 1;

  if(access(sReceiveFileName, 0))
  {
    if(SendLongToSocket(socket, 0))
    {
      close(socket);
      return 1;
    }
    return 0;
  }

  if(stat(sReceiveFileName, &statFile))
  {
    if(SendLongToSocket(socket, 0))
    {
      close(socket);
      return 1;
    }
    return 0;
  }

  if(SendLongToSocket(socket,1))
  {
    close(socket);
    return 1;
  }

  nFileSize=statFile.st_size;
  nFileDate=statFile.st_mtime;

  if(SendLongToSocket(socket, nFileSize))
  {
    close(socket);
    return 1;
  }

  if(SendLongToSocket(socket, nFileDate))
  {
    close(socket);
    return 1;
  }

  SetWaitingTime(g_nMaxWaitingTime);
  if(GetLongFromSocket(socket,&nTaskNo))
  {
    close(socket);
    return 1;
  }
  if(nTaskNo==0)
  {
    return 0;
  }

  SetWaitingTime(g_nMinWaitingTime);
  if(!remove(sReceiveFileName))
  {
    if(SendLongToSocket(socket, 1))
    {
      close(socket);
      return 1;
    }
  }
  else
  {
    if(SendLongToSocket(socket, 0))
    {
      close(socket);
      return 1;
    }
  }

  return 0;
}

// 传文件名
int cwjm(int socket)
{
  long nTaskNo;
  DIR* dirp;
  struct dirent * dp;

  dirp=opendir("newdata");
  while((dp=readdir(dirp))!=NULL)
  {
    if(!strcmp(dp->d_name,".")||!strcmp(dp->d_name,"..")||strstr(dp->d_name,"File"))
      continue;
    if(SendLongToSocket(socket, 1))
    {
      closedir(dirp);
      close(socket);
      return 1;
    }
    if(SendStringToSocket(socket, dp->d_name))
    {
      closedir(dirp);
      close(socket);
      return 1;
    }
    closedir(dirp);
    return 0;
  }
  closedir(dirp);
  if(SendLongToSocket(socket, 0))
  {
    close(socket);
    return 1;
  }
  return 0;
}

// 执行命令
int zxml(int socket)
{
  long nTaskNo;
  char sCommand[101];
  
  if(GetStringFromSocket(socket,sCommand))
  {
    close(socket);
    return 1;
  }
  if(system(sCommand)==0)
  {  
    if(SendLongToSocket(socket, 1))
    {
      close(socket);
      return 1;
    }
  }
  else
  {
    if(SendLongToSocket(socket, 0))
    {
      close(socket);
      return 1;
    }
  }
  return 0;
}

// 获取给定目录下一个文件名
int GetOneFileName(int socket)
{
  long nTaskNo;
  DIR* dirp;
  struct dirent * dp;
  char sPath[31];

  if(GetStringFromSocket(socket,sPath))
    return 1;
  dirp=opendir(sPath);
  if(dirp)
  {
    while((dp=readdir(dirp))!=NULL)
    {
      if(!strcmp(dp->d_name,".")||!strcmp(dp->d_name,".."))
        continue;
      if(SendLongToSocket(socket, 1))
      {
        closedir(dirp);
        close(socket);
        return 1;
      }
      if(SendStringToSocket(socket, dp->d_name))
      {
        closedir(dirp);
        close(socket);
        return 1;
      }
      closedir(dirp);
      return 0;
    }
    closedir(dirp);
  }
  if(SendLongToSocket(socket, 0))
  {
    close(socket);
    return 1;
  }
  return 0;
}

// 移动文件到另一个目录
int MoveFile(int socket)
{
  long nTaskNo;
  char sSource[51];
  char sDestination[51];
  
  if(GetStringFromSocket(socket,sSource))
    return 1;
  if(GetStringFromSocket(socket,sDestination))
    return 1;
  if(!rename(sSource, sDestination))
  {
     if(SendLongToSocket(socket, 1))
       return 1;
  }
  else
  {
     if(SendLongToSocket(socket, 0))
       return 1;
  } 
 
  return 0;
}

// 上载文件
int SendFileToRemote(int socket)
{
  long nTaskNo;
  FILE *fp;
  int nReadLen;
  char sBuffer[BUFFERSIZE];
  long nFileSize,nRecvSize=0;
  char sSendFileName[51];
  char sPureFileName[51];
  char sRealFileName[51];
  int nFileLen;
  long nTemp;
  long nOffset, nLeftSize;
  long nFlag;

  if(GetLongFromSocket(socket,&nFlag))
    return 1;

  if(GetStringFromSocket(socket,sSendFileName))
    return 1;

  if(strchr(sSendFileName, '*') || strchr(sSendFileName, '?'))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  if(!nFlag && !access(sSendFileName,0))
  {
    SendLongToSocket(socket,0);
    return 0;
  }

  if(access("tempfile",0))
    mkdir("tempfile",S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);

  strcpy(sPureFileName, (strrchr(sSendFileName, '/'))? strrchr(sSendFileName, '/')+1:sSendFileName);
  sprintf(sRealFileName, "tempfile/%s.___", sPureFileName);

  if((fp=fopen(sRealFileName,"w+b"))==NULL)
  {
    SendLongToSocket(socket,0);
    return 0;
  }
  if(SendLongToSocket(socket,1))
  {
    fclose(fp);
    return 1;
  }

  if(GetLongFromSocket(socket,&nFileSize))
  {
    fclose(fp);
    return 1;
  }

  if(nFileSize==0)
  {
    fclose(fp);
    return 0;
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
    
    if(nTemp != -3)
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
      if((nReadLen=ReadSocket(socket, sBuffer + nOffset, nLeftSize))<0)
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

    if(nFileSize==nRecvSize)
    {
      fclose(fp);
      if(rename(sRealFileName, sSendFileName) == 0)
        SendLongToSocket(socket, 1);
      else
        SendLongToSocket(socket, 0);

      #ifdef COUNT_CONNECT_NUMBER
      nSumBytes += nFileSize;
      #endif
        
      break;
    } 
  } 
  return 0;
}

/*------------------------------------------------------------------------
 * reaper - clean up zombie children
 *------------------------------------------------------------------------
 */
/*ARGSUSED*/
void reaper(int sig)
{
    int status,pid;
    while((pid=waitpid(-1,&status,WNOHANG))>0) 
    {
       printf("\nbkfxd child process %d terminated with %d",pid,status);
       fflush(stdout);
    }
}

// 读初始化文件
long ReadIni()
{
  FILE *fp;
  char *s;
  char sVersionFlag[]="version";
  char sMaxWaitingTimeFlag[]="maxtime";
  char sMinWaitingTimeFlag[]="mintime";
  char sBktxVerFlag[]="bktxver";
  char sDebugFlag[]="debug";
  

  char sLine[81];

  dVersion=0.0;
  dBktxVer=0.0;

  if((fp=fopen(sIniFile,"r"))==NULL)
    return 1;

  while(fgets(sLine,81,fp)!=NULL)
  {
    alltrim(sLine);
    lower(sLine);
    if(strncmp(sLine,sVersionFlag,strlen(sVersionFlag))==0)
    {
      if((s=strchr(sLine,'='))==NULL)
      {
        outtext("%s: %s line missing =",sIniFile,sVersionFlag);
        continue;
      }
      dVersion=atof(s+1);
      continue;
    }

    if(strncmp(sLine,sBktxVerFlag,strlen(sBktxVerFlag))==0)
    {
      if((s=strchr(sLine,'='))==NULL)
      {
        outtext("%s: %s line missing =",sIniFile,sBktxVerFlag);
        continue;
      }
      dBktxVer=atof(s+1);
      continue;
    }
 
    if(strncmp(sLine,"password",8)==0)  //FXZ口令
    {
      if((s=strchr(sLine,'='))==NULL)
      {
        outtext("%s: %s line missing =",sIniFile,sBktxVerFlag);
        continue;
      }
      memset(sSysPassword,0,sizeof(sSysPassword));
      strcpy(sSysPassword,sLine+9);
      continue;
    }
    
    if(strncmp(sLine,sMaxWaitingTimeFlag,strlen(sMaxWaitingTimeFlag))==0)
    {
      if((s=strchr(sLine,'='))==NULL)
      {
        outtext("%s: %s line missing =",sIniFile,sMaxWaitingTimeFlag);
        continue;
      }
      g_nMaxWaitingTime = atoi(s+1);
      if(g_nMaxWaitingTime > MAX_WAITING_TIME)
      {
        g_nMaxWaitingTime = MAX_WAITING_TIME;
      }
      continue;
    }

    if(strncmp(sLine,sMinWaitingTimeFlag,strlen(sMinWaitingTimeFlag))==0)
    {
      if((s=strchr(sLine,'='))==NULL)
      {
        outtext("%s: %s line missing =",sIniFile,sMinWaitingTimeFlag);
        continue;
      }
      g_nMinWaitingTime = atoi(s+1);
      if(g_nMinWaitingTime < MIN_WAITING_TIME)
      {
        g_nMinWaitingTime = MIN_WAITING_TIME;
      }
      continue;
    }

    if(strncmp(sLine,sDebugFlag,strlen(sDebugFlag))==0)
    {
      if((s=strchr(sLine,'='))==NULL)
      {
        outtext("%s: %s line missing =",sIniFile,sDebugFlag);
        continue;
      }
      alltrim(s+1);
      if(*(s+1) == 'y' || *(s+1) == 'Y')
        nDebugFlag = 1;
      continue;
    }
  }
  fclose(fp);
  return 0;
}

#ifdef COUNT_CONNECT_NUMBER
// nFlag: 1 更改连接次数
//        2 更改成功退出次数
//        3 更改不成功退出次数
// nTime  连接时间，在nFlag = 2时有意义
// nBytes 字节数，在nFlag = 2时有意义
int CountConnect(int nFlag, long nTime, long nBytes)
{
  int fd;
  char s[201];
  long nStartDate; // 起始日期
  long nConnect;  // 连接次数
  long nSuccess;  // 成功退出次数
  long nFailure;  // 不成功退出次数
  long nTotalTime; // 累计通讯时间
  long nMinTime; // 一次最少通讯时间
  long nMaxTime; // 一次最多通讯时间
  double dTotalBytes; // 累计通讯字节数
  double dMinBytes; // 一次最少通讯字节数
  double dMaxBytes; // 一次最多通讯字节数
  time_t tNow;
  int n; // 有效参数个数

  if((fd=open("log/connect.log",O_RDWR|O_CREAT, 0644))>0)
  {
    locking(fd, LK_RLCK, 0);
    read(fd, s, 200);
    n = sscanf(s, "%ld %ld %ld %ld %ld %ld %ld %lf %lf %lf", 
    	&nStartDate, &nConnect, &nSuccess, &nFailure, &nTotalTime, &nMinTime, &nMaxTime, &dTotalBytes, &dMinBytes, &dMaxBytes);
    if(n < 5)
    {
      tNow = time(0);
      cftime(s, "%Y%m%d", &tNow);
      nStartDate = atol(s);
      nConnect = 0;
      nSuccess = 0;
      nFailure = 0;
      nTotalTime = 0;
    }
    if(n < 10)
    {
    	nMinTime = 0;
    	nMaxTime = 0;
    	dTotalBytes = 0.0;
    	dMinBytes = 0.0;
    	dMaxBytes = 0.0;
    }
    switch(nFlag)
    {
      case 1:
        nConnect++;
        break;
      case 2:
        nSuccess++;
        nTotalTime += nTime;
        dTotalBytes += nBytes;
        if(nMinTime == 0)
          nMinTime = nTime;
        else
        {
          if(nMinTime > nTime)
            nMinTime = nTime;
        }
        if(nMaxTime == 0)
          nMaxTime = nTime;
        else
        {
          if(nMaxTime < nTime)
            nMaxTime = nTime;
        }

        if(nBytes != 0)
        {
          if(dMinBytes == 0.0)
            dMinBytes = nBytes;
          else
          {
            if(dMinBytes > nBytes)
              dMinBytes = nBytes;
          }
          if(dMaxBytes == 0.0)
            dMaxBytes = nBytes;
          else
          {
            if(dMaxBytes < nBytes)
              dMaxBytes = nBytes;
          }
        }
        break;
      case 3:
        nFailure++;
        break;
    }
    sprintf(s, "%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%ld\n%lf\n%lf\n%lf\n", 
      nStartDate, nConnect, nSuccess, nFailure, nTotalTime, nMinTime, nMaxTime, dTotalBytes, dMinBytes, dMaxBytes);
    lseek(fd, 0, SEEK_SET);
    write(fd, s, strlen(s));
    close(fd);
  }
}
#endif
void OneGetInitValue(char * sInitFileName,char* sTitleFlag,char* sValue ,char* sDefault)
{
  FILE *fp;
  char sLine[182],sTempTitleFlag[81];
  char *s;
  long nLenth;
  
  sprintf(sTempTitleFlag,"%s=",sTitleFlag);
  if((fp=fopen(sInitFileName,"r"))==NULL)
  {
  
    if(!strcmp(sDefault,"-9999"))
      return;
    else if(!strcmp(sDefault," "))
      sValue[0]='\0'; 
    else
      strcpy(sValue,sDefault); 
    return ;
  }
  while(fgets(sLine,181,fp)!=NULL)
  {
    alltrim(sLine);
    lower(sLine);
    if(strncmp(sLine,sTempTitleFlag,strlen(sTempTitleFlag))==0)
    {if((s=strchr(sLine,'='))==NULL)
      {
        gotoxy(10,10);
        outtext("%s文件中%s行缺少'='号",sInitFileName,sTitleFlag);
        keyb();
        continue;
      }
      alltrim(sLine);
      nLenth=strlen(sLine);
//outtext("sline-%s  len-%ld\n",sLine,nLenth);
      strncpy(sValue,s+1,nLenth-3);
      alltrim(sValue);
      nLenth=strlen(sValue);
//outtext("svalue-%s  len-%ld\n",sValue,nLenth);
      sValue[nLenth]='\0';
      alltrim(sValue);
      if(sValue[strlen(sValue)-1]=='\n')
         sValue[strlen(sValue)-1]='\0';
//         outtext("\ninit-get-title  %s==svalue-%s\n",sTitleFlag,sValue);

      fclose(fp);
      return ;
    }
  }//end while
  if(!strcmp(sDefault,"-9999"))      ;
  else if(!strcmp(sDefault," "))      sValue[0]='\0'; 
  else      strcpy(sValue,sDefault); 
  fclose(fp); 
  return ;
//heliw
}

void GetSysLkrq()
{
	struct tm now_time;
	time_t now; 

	time(&now);
	now_time=*localtime(&now);
	nSysLkrq=(now_time.tm_year+1900)*10000+(now_time.tm_mon+1)*100+now_time.tm_mday;
	nSysSfm=now_time.tm_hour*10000+now_time.tm_min*100+now_time.tm_sec;
}
