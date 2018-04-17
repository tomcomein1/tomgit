#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "dlpublic.h"
#include "netmain.h"
#include "task.h"

extern char sSysDlywHost[41];
extern char sSysDlywService[41];
extern char sSysErrInfo[81];

long testcall();
long testtask();

void ShowErrInfo(int x, int y, long nErrNo)
{
  gotoxy(x, y);
  switch(nErrNo)
  {
    case 1:
      outtext("网络连接出错!\n");
      break;
/*
    case -ERR_NO_ENCRYPT:
      outtext("密钥验证出错!\n");
      break;
    case -ERR_NO_TCPIP:
      outtext("传输出错!\n");
      break;
    case -ERR_NO_PROCESS:
      outtext("服务器进程数超出!\n");
      break;
    case -ERR_NO_CONNECT:
      outtext("服务器不能连接数据库!\n");
      break;
*/
    case -1:
    default:
      outtext("%s\n", sSysErrInfo);
      break;
  }
}

long testtask()
{
  int nRetVal;
  double dVal;
  char sVal[21];
  long nVal;
  clrscr();
  if(nRetVal)
  {
    ShowErrInfo(10, 20, nRetVal);
  }
  else
  {
    printf("\n%lf\n%s\n%ld", dVal, sVal, nVal);
  }
  keys();
}

long RunTask(char* sDestination, long nTaskNo, char* sInputArg, char* sOutputArg, ...)
{
  int nOutSocket;
  long nRetVal;

  long nType;/*变长列表中参数的类型*/
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/

  double *pdValue;
  long *pnValue;
  char *sValue;
  va_list pVarArgList;/*变长参数列表指针*/

  va_start(pVarArgList,sOutputArg);
  
  nOutSocket = ConnectRemote(sSysDlywHost, sSysDlywService);

  if(nOutSocket > 0)
  {
    if(SendToSocket(nOutSocket, "%ld %s %ld", nTaskNo, sDestination, 1))
    {
      shutdown(nOutSocket,2);
      return 1;
    }

    // 以下加入发送输入参数:nOutSocket
    if(sInputArg && sInputArg[0])
    {
      sTop=sInputArg;
      sBottom=sInputArg;

      nType=TYPE_NULL;
      while(sTop&&sTop[0])
      {
        nType=0;
        sBottom=(char*)strchr(sTop,'%');
        if(sBottom==NULL)
        {
          break;
        }
        else
          sBottom++;

        while(nType==0)
        {
          switch(sBottom[0])
          {
            case 'l':
              sBottom++;
              switch(sBottom[0])
              {
                case 'd':
                  nType=TYPE_LONG;
                  break;
                case 'f':
                  nType=TYPE_DOUBLE;
                  break;  
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  keyb();
                  return -1;
                  break;
              }
              break;
            case 's':
              nType=TYPE_STRING; 
              break;
            case '\0':
              nType=TYPE_NULL;
              break;
          }
          if (nType)
            break;
          if(sBottom!=NULL&&sBottom[0]!='\0')
            sBottom++;
        }

        if(nType!=TYPE_NULL)
        {
          sTop=sBottom+1;
        }
        else
          break;

        switch(nType)
        { 
          case TYPE_LONG:
            if(SendLongToSocket(nOutSocket, va_arg(pVarArgList,long)))
            {
              shutdown(nOutSocket,2);
              return 1;
            }
            break;
          case TYPE_DOUBLE:
            if(SendDoubleToSocket(nOutSocket,va_arg(pVarArgList,double)))
            {
              shutdown(nOutSocket,2);
              return 1;
            }
            break;
          case TYPE_STRING:
            if(SendStringToSocket(nOutSocket,va_arg(pVarArgList,char*)))
            {
              shutdown(nOutSocket,2);
              return 1;
            }
            break;
        }
      } 
    }  
    // 以上加入发送输入参数

    if(GetLongFromSocket(nOutSocket, &nRetVal))
    {
      shutdown(nOutSocket,2);
      return 1;
    }

    if(nRetVal == 0)
    {
      // 以下加入接收输出参数:nOutSocket
      if(sOutputArg && sOutputArg[0])
      {
        sTop=sOutputArg;
        sBottom=sOutputArg;

        nType=TYPE_NULL;

        while(sTop&&sTop[0])
        {
          nType=0;
          sBottom=(char*)strchr(sTop,'%');
          if(sBottom==NULL)
          {
            break;
          }
          else
            sBottom++;

          while(nType==0)
          {
            switch(sBottom[0])
            {
              case 'l':
                sBottom++;
                switch(sBottom[0])
                {
                  case 'd':
                    nType=TYPE_LONG;
                    break;
                  case 'f':
                    nType=TYPE_DOUBLE;
                    break;  
                  default:
                    fprintf(stdout,"\n\a语法错误\n");
                    keyb();
                    return -1;
                  break;
                }
                break;
              case 's':
                nType=TYPE_STRING; 
                break;
              case '\0':
                nType=TYPE_NULL;
                break;
            }
            if (nType)
              break;
            if(sBottom!=NULL&&sBottom[0]!='\0')
              sBottom++;
          }

          if(nType!=TYPE_NULL)
          {
            sTop=sBottom+1;
          }
          else
            break;

          switch(nType)
          { 
            case TYPE_LONG:
              pnValue=va_arg(pVarArgList,long *);
              if(GetLongFromSocket(nOutSocket, pnValue))
              {
                shutdown(nOutSocket,2);
                return 1;
              }
              break;
            case TYPE_DOUBLE:
              pdValue=va_arg(pVarArgList,double *);
              if(GetDoubleFromSocket(nOutSocket,pdValue))
              {
                shutdown(nOutSocket,2);
                return 1;
              }
              break;
            case TYPE_STRING:
              sValue=va_arg(pVarArgList,char *);
              if(GetStringFromSocket(nOutSocket,sValue))
              {
                shutdown(nOutSocket,2);
                return 1;
              }
              break;
          }
        }
      }
      // 以上加入接收输出参数
      DisconnectRemote(nOutSocket);
    }
    else
    {
      // 远方执行任务出错
      if(GetStringFromSocket(nOutSocket, sSysErrInfo))
      {
        shutdown(nOutSocket,2);
        return 1;
      }
    }
  }
  else
  {
    return nOutSocket;
  }
  
  return nRetVal;
}

long testcall()
{
  int nRetVal;
  long nBufferSize = 20 * 1024;
  char* sBuffer;
  int i;
  sBuffer = malloc(nBufferSize); // 此句必需
  clrscr();
  if(sBuffer == NULL)
  {
    gotoxy(20,20);    
    outtext("内存不足, 请退出一些不必要的程序!");
    return -1;
  }
  if(nRetVal)
  {
    ShowErrInfo(10, 20, nRetVal);
  }
  else
  {
    printf("\n%s\n", sBuffer);
  }
  free(sBuffer); // 此句必需
  keys();
}

/*
功能：
  完成对服务器任务的调用
参数：
  sDestination 目标地址
  nTaskNo 任务号
  sInBuffer 输入缓冲
  sOutBuffer 输出缓冲
返回值：
  0 正常
  1 连接出错
  < 0 出错信息存于sSysErrInfo
*/
long RunCall(char* sDestination, long nTaskNo, char* sInBuffer, char* sOutBuffer)
{
  int nOutSocket;
  long nRetVal;

  nOutSocket = ConnectRemote(sSysDlywHost, sSysDlywService);

  if(nOutSocket > 0)
  {
    if(SendToSocket(nOutSocket, "%ld %s %ld", nTaskNo, sDestination, 1))
    {
      shutdown(nOutSocket,2);
      return 1;
    }

    // 以下加入发送输入缓冲:nOutSocket
    if(sInBuffer)
    {
      if(SendStringToSocket(nOutSocket, sInBuffer))
      {
        shutdown(nOutSocket,2);
        return 1;
      }
    }
    else
    {
      if(SendStringToSocket(nOutSocket, ""))
      {
        shutdown(nOutSocket,2);
        return 1;
      }
    }
    // 以上加入发送输入参数

    if(GetLongFromSocket(nOutSocket, &nRetVal))
    {
      shutdown(nOutSocket,2);
      return 1;
    }

    if(nRetVal == 0)
    {
      // 以下加入接收输出参数:nOutSocket
      if(GetStringFromSocket(nOutSocket,sOutBuffer))
      {
        shutdown(nOutSocket,2);
        return 1;
      }
      // 以上加入接收输出参数
      DisconnectRemote(nOutSocket);
    }
    else
    {
      // 远方执行任务出错
      if(GetStringFromSocket(nOutSocket, sSysErrInfo))
      {
        shutdown(nOutSocket,2);
        return 1;
      }
    }
  }
  else
  {
    return nOutSocket;
  }
  
  return nRetVal;
}


