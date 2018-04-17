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
      outtext("�������ӳ���!\n");
      break;
/*
    case -ERR_NO_ENCRYPT:
      outtext("��Կ��֤����!\n");
      break;
    case -ERR_NO_TCPIP:
      outtext("�������!\n");
      break;
    case -ERR_NO_PROCESS:
      outtext("����������������!\n");
      break;
    case -ERR_NO_CONNECT:
      outtext("�����������������ݿ�!\n");
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

  long nType;/*�䳤�б��в���������*/
  char* sTop;/*�����ʽͷָ��*/
  char* sBottom;/*�����ʽβָ��*/

  double *pdValue;
  long *pnValue;
  char *sValue;
  va_list pVarArgList;/*�䳤�����б�ָ��*/

  va_start(pVarArgList,sOutputArg);
  
  nOutSocket = ConnectRemote(sSysDlywHost, sSysDlywService);

  if(nOutSocket > 0)
  {
    if(SendToSocket(nOutSocket, "%ld %s %ld", nTaskNo, sDestination, 1))
    {
      shutdown(nOutSocket,2);
      return 1;
    }

    // ���¼��뷢���������:nOutSocket
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
                  fprintf(stdout,"\n\a�﷨����\n");
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
    // ���ϼ��뷢���������

    if(GetLongFromSocket(nOutSocket, &nRetVal))
    {
      shutdown(nOutSocket,2);
      return 1;
    }

    if(nRetVal == 0)
    {
      // ���¼�������������:nOutSocket
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
                    fprintf(stdout,"\n\a�﷨����\n");
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
      // ���ϼ�������������
      DisconnectRemote(nOutSocket);
    }
    else
    {
      // Զ��ִ���������
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
  sBuffer = malloc(nBufferSize); // �˾����
  clrscr();
  if(sBuffer == NULL)
  {
    gotoxy(20,20);    
    outtext("�ڴ治��, ���˳�һЩ����Ҫ�ĳ���!");
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
  free(sBuffer); // �˾����
  keys();
}

/*
���ܣ�
  ��ɶԷ���������ĵ���
������
  sDestination Ŀ���ַ
  nTaskNo �����
  sInBuffer ���뻺��
  sOutBuffer �������
����ֵ��
  0 ����
  1 ���ӳ���
  < 0 ������Ϣ����sSysErrInfo
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

    // ���¼��뷢�����뻺��:nOutSocket
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
    // ���ϼ��뷢���������

    if(GetLongFromSocket(nOutSocket, &nRetVal))
    {
      shutdown(nOutSocket,2);
      return 1;
    }

    if(nRetVal == 0)
    {
      // ���¼�������������:nOutSocket
      if(GetStringFromSocket(nOutSocket,sOutBuffer))
      {
        shutdown(nOutSocket,2);
        return 1;
      }
      // ���ϼ�������������
      DisconnectRemote(nOutSocket);
    }
    else
    {
      // Զ��ִ���������
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


