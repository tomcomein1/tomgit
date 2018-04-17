#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "bkfx.h"
#include "netmain.h"
#include "task.h"

extern char sSysErrInfo[81];
#define SERVICE "dlyw"

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
  
  nOutSocket = ConnectRemote(sDestination, SERVICE);

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
