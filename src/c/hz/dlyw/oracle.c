
#include"dlpublic.h"
/*此开关用于服务器程序*/
/*
#define DATASERVER
#define OCI
#define TEST
*/

/*此开关用于合为一体程序*/
/*
#define OCI
*/

/*此开关用于客户端程序*/
#define DATACLIENT

#define TYPE_INT 1
#define TYPE_LONG 2
#define TYPE_SHORT 3
#define TYPE_FLOAT 4
#define TYPE_DOUBLE 5
#define TYPE_LONGDOUBLE 6
#define TYPE_CHAR 11
#define TYPE_STRING 12
#define TYPE_TABLE 13
#define TYPE_NULL -1

#if(defined(DATASERVER)||defined(DATACLIENT))

#include "netmain.h"

#endif

extern char sTcpipErrInfo[81];

/* Copyright (c) 1999, 1999 by HongZhi Corporation */
/*
   NAME
     oracle.c - OCI basic function C Program 
*/

/*
   The size of the HDA is defined by the HDA_SIZE constant,
   which is declared in ocidem.h to be 256 bytes for 32-
   bit architectures and 512 bytes for 64-bit architectures.
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "sql.h"

char   sSysSqlStatement[4096];  //lee

#ifdef DATACLIENT
extern char sSysManager[7];//代理业务表所在用户，缺省为DLZ
extern char sSysPassword[41];//代理业务表所在用户的口令，缺省为DDDD
#endif

#ifndef DATACLIENT


/* Include OCI-specific headers. */

#include <oratypes.h>
#include <ocidfn.h>
#ifdef __STDC__
#include <ociapr.h>
#else
#include <ocikpr.h>
#endif
#include <ocidem.h>

/* Define one logon data area and one cursor data area
   Also define a host data area for olog.
   (See ocidfn.h for declarations). */

static Lda_Def lda;
static Cda_Def cda;
static Cda_Def cursor[MAX_CURSOR_NUM];
static char    sCursorFlag[MAX_CURSOR_NUM+1];
static long    nColNum[MAX_CURSOR_NUM];
static ub4     hda[HDA_SIZE/(sizeof(ub4))];

#ifdef TEST
static char sDsUserName[MAX_USERNAME_LEN+1]="";
static char sDsPassWord[MAX_PASSWORD_LEN+1]="";
#endif

/* Globals */

static text sql_statement[8192];
static sword sql_function;
static sword numwidth = 14;

/* Declare an array of bind values. */

static text bind_values[MAX_BINDS][MAX_ITEM_BUFFER_SIZE];
static long nBindVarType[MAX_BINDS];
static char * pBindVar[MAX_BINDS];

/* Declare structures for query information. */
struct describe
{
    sb4             dbsize;
    sb2             dbtype;
    sb1             buf[MAX_ITEM_BUFFER_SIZE];
    sb4             buflen;
    sb4             dsize;
    sb2             precision;
    sb2             scale;
    sb2             nullok;
};

struct define 
{
    ub1             buf[MAX_ITEM_BUFFER_SIZE];
    float           flt_buf;
    sword           int_buf;
    sb2             indp;
    ub2             col_retlen, col_retcode;
};

/* Define arrays of describe and define structs. */
static struct describe desc[MAX_SELECT_LIST_SIZE];
static struct define   def[MAX_SELECT_LIST_SIZE];

/*static long nSqlErrNo;*/
static long nSqlErrNo;
#endif

#ifdef DATACLIENT
static int nOciSocket=0;
#endif

#ifndef DATACLIENT
sword describe_define(Cda_Def *cda);
#endif

long nChnFlag=1;
extern long nSysAutoClose;   //提交后是否自动关库
/*  Declare basic functions. */

#ifdef DATASERVER
int dsConnectDatabase(int socket)
{
  long nRetVal;
  char sUserName[MAX_USERNAME_LEN+1],sPassWord[MAX_PASSWORD_LEN+1];
 
  if(GetStringFromSocket(socket,sUserName))
  {
    close(socket);
    return 1;
  }

  if(GetStringFromSocket(socket,sPassWord))
  {
    close(socket);
    return 1;
  }

  nRetVal=ConnectDatabase(sUserName,sPassWord);

  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }

  if(nRetVal<0)
  {
    close(socket);
    return 1;
  }

  return 0;
}
#endif

#ifdef DATACLIENT
long ConnectDatabase(char *sUserName,char *sPassWord)
{
  long nRetVal;

  extern char sSysDsHost[41];
  extern char sSysDsService[41];

  nOciSocket = connectTCP(sSysDsHost, sSysDsService);
  if(nOciSocket<0)
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(SendLongToSocket(nOciSocket,(long)TASK_CONNECTDATABASE))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(SendStringToSocket(nOciSocket,sUserName))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(SendStringToSocket(nOciSocket,sPassWord))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(nRetVal<0)
    nOciSocket=0;
  return nRetVal;
}

/* 
 * Function: ConnectRemoteDatabase
 *
 * Description: This routine logs on onto the remote database
 *
 * Return: 0 成功 
 */

int ConnectRemoteDatabase(int socket,char *sUserName,char *sPassWord)
{
  long nRetVal;
 
  nRetVal=ConnectDatabase(sUserName,sPassWord);

  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }

  if(nRetVal<0)
  {
    close(socket);
    return 1;
  }

  return 0;
}
#endif

#ifdef OCI

/* 
 * Function: ConnectDatabase
 *
 * Description: This routine logs on onto the database
 *
 * Return: 0 成功 
 */

long ConnectDatabase(char *sUserName,char *sPassWord)
{
  long nCode;

  #ifdef TEST
  strcpy(sDsUserName,sUserName);
  strcpy(sDsPassWord,sPassWord);
  if(sPassWord==NULL||sPassWord[0]=='\0')
    fprintf(stderr,"\nConnectDatabase: %s\n",sUserName);
  else
    fprintf(stderr,"\nConnectDatabase: %s/%s\n",sUserName,sPassWord);
  #endif

  nSqlErrNo=0L;

  /* olog() establishes a connection between an OCI program and 
     an Oracle database. */
  nCode=olog(&lda, (ub1 *)hda, (text *)sUserName, -1, (text *)sPassWord, -1, 
          (text *)0, -1, (ub4) OCI_LM_DEF);
  if(nCode==0)
  {
    nCode=oopen(&cda, &lda, (text *) 0, -1, -1, (text *) 0, -1);
    sprintf(sCursorFlag,"%0*c",MAX_CURSOR_NUM,'0');
    if(nCode!=0)
    {
      nSqlErrNo=cda.rc;
      #ifdef TEST
      ShowTestErrMsg();
      #endif
      return -nSqlErrNo;
    }
  }
  else
  {
    nSqlErrNo=lda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }
  return 0;
}
#endif

#ifdef DATASERVER
int dsCloseDatabase(int socket)
{
  long nRetVal;
  
  nRetVal=CloseDatabase();
  SendLongToSocket(socket,nRetVal);
  close(socket);
  return 0;
}
#endif

#ifdef DATACLIENT
long CloseDatabase()
{
  long nRetVal;

  if(nOciSocket==0)
    return -1012;

  if(SendLongToSocket(nOciSocket,(long)TASK_CLOSEDATABASE))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  close(nOciSocket);

  nOciSocket=0;

  return nRetVal;
}

int CloseRemoteDatabase(int socket)
{
  long nRetVal;
  
  nRetVal=CloseDatabase();
  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }
  close(socket);
  return 0;
}
#endif

#ifdef OCI

/* 
 * Function: CloseDatabase
 *
 * Description: This routine closes out any cursors and logs off the database
 *
 */
long CloseDatabase()
{
  long nCode,i;

  #ifdef TEST
  fprintf(stderr,"%s: CloseDatabase\n",sDsUserName);
  #endif

  /* oclose() disconnects a cursor from the data areas 
     in the Oracle Server with which it is associated. */
  nCode=oclose(&cda);                                         /* close cursor */

  if(nCode==0)
  {
    for(i=0;i<MAX_CURSOR_NUM;i++)
    {
      if(sCursorFlag[i]=='1')
        oclose(&cursor[i]);
    }
    /* ologof() disconnects an LDA from the Oracle program global area
       and frees all Oracle resources owned by the Oracle user process. */
    nCode=ologof(&lda);                               /* log off the database */
    if(nCode!=0)
    {
      nSqlErrNo=lda.rc;
      #ifdef TEST
      ShowTestErrMsg();
      #endif
      return -nSqlErrNo;
    }
  }
  else
  {
    nSqlErrNo=cda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }
 
  return 0;
}
#endif

#ifdef DATASERVER
int dsGetSqlErrNo(int socket)
{
  #ifdef TEST
  fprintf(stderr,"%s: GetSqlErrNo: %ld\n",sDsUserName,-nSqlErrNo);
  #endif

  if(SendLongToSocket(socket,-nSqlErrNo))
  {
    close(socket);
    return 1;
  }
  return 0;
}
#endif

#ifdef DATACLIENT
long GetSqlErrNo()
{
  long nRetVal;

  if(nOciSocket==0) ConnectDatabase(sSysManager,sSysPassword);

  if(nOciSocket==0)
    return -1012;

  if(SendLongToSocket(nOciSocket,(long)TASK_GETSQLERRNO))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  return nRetVal;
}
#endif

#ifdef OCI
long GetSqlErrNo()
{
  #ifdef TEST
  fprintf(stderr,"%s: GetSqlErrNo: %ld\n",sDsUserName,-nSqlErrNo);
  #endif
  return -nSqlErrNo;
}
#endif

#ifdef DATASERVER
int dsGetSqlErrInfo(int socket)
{
  char sErrMsg[MAX_ERR_MSG_LEN+1];

  GetSqlErrInfo(sErrMsg);
  if(SendStringToSocket(socket,sErrMsg))
  {
    close(socket);
    return 1;
  }
  return 0;
}
#endif

#ifdef DATACLIENT
long GetSqlErrInfo(char *sErrMsg)
{
  if(nOciSocket==0) ConnectDatabase(sSysManager,sSysPassword);

  if(nOciSocket==0)
  {
    strcpy(sErrMsg,"ORA-01012: not logged on\n");
    return strlen(sErrMsg);
  }

  if(SendLongToSocket(nOciSocket,(long)TASK_GETSQLERRINFO))
  {
    strcpy(sErrMsg,sTcpipErrInfo);
    return strlen(sErrMsg);
  }
  if(GetStringFromSocket(nOciSocket,sErrMsg))
  {
    strcpy(sErrMsg,sTcpipErrInfo);
    return strlen(sErrMsg);
  }
  return strlen(sErrMsg);
}
#endif

#ifdef OCI
long GetSqlErrInfo(char *sErrMsg)
{
  long nCharCount;

  #ifdef DATASERVER
  if(nSqlErrNo==ERR_NO_TCPIP)
  {
    #ifdef TEST
    fprintf(stderr,"%s: GetSqlErrInfo: %s\n",sDsUserName,sTcpipErrInfo);
    #endif

    strcpy(sErrMsg,sTcpipErrInfo);
    return strlen(sErrMsg);
  }
  if(nSqlErrNo==ERR_NO_SELECT)
  {
    #ifdef TEST
    fprintf(stderr,"%s: GetSqlErrInfo: not SELECT statement\n",sDsUserName);
    #endif
    strcpy(sErrMsg,"非SELECT语句");
    return strlen(sErrMsg);
  }

  #endif

  /* oerhms() returns the text of an Oracle error message,
     given the error code rcode. */
  nCharCount=oerhms(&lda,nSqlErrNo,(text *)sErrMsg,(sword)MAX_ERR_MSG_LEN);

  #ifdef TEST
  fprintf(stderr,"%s: GetSqlErrInfo: %s\n",sDsUserName,sErrMsg);
  #endif

  return nCharCount;
}
#endif

#ifdef DATASERVER
int dsCommitWork(int socket)
{
  long nRetVal;
  nRetVal=CommitWork();
  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }
  return 0;
}
#endif

#ifdef DATACLIENT
long CommitWork()
{
  long nRetVal;

  if(nOciSocket==0)
    return -1012;

  if(SendLongToSocket(nOciSocket,(long)TASK_COMMITWORK))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(nSysAutoClose)  CloseDatabase();
  return nRetVal;
}
#endif

#ifdef OCI
long CommitWork()
{
  #ifdef TEST
  fprintf(stderr,"%s: CommitWork\n",sDsUserName);
  #endif

  /* ocom() commits the current transaction. */
  if(ocom(&lda)==0)
    return 1;
  else
  {
    nSqlErrNo=lda.rc;

    #ifdef TEST
    ShowTestErrMsg();
    #endif

    return 0;
  }
}
#endif

#ifdef DATASERVER
int dsRollbackWork(int socket)
{
  long nRetVal;
  nRetVal=RollbackWork();
  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }
  return 0;
}
#endif

#ifdef DATACLIENT
long RollbackWork()
{
  long nRetVal;


  if(nOciSocket==0)
    return -1012;

  if(SendLongToSocket(nOciSocket,(long)TASK_ROLLBACKWORK))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(nSysAutoClose)  CloseDatabase();
  return nRetVal;
  
}
#endif

#ifdef OCI
long RollbackWork()
{
  #ifdef TEST
  fprintf(stderr,"%s: RollbackWork\n",sDsUserName);
  #endif

  /* orol() rolls back the current transaction. */
  if(orol(&lda)==0)
    return 1;
  else
  {
    nSqlErrNo=lda.rc;

    #ifdef TEST
    ShowTestErrMsg();
    #endif

    return 0;
  }
}
#endif

#ifdef DATASERVER
int dsRunSql(int socket)
{
  long nRetVal;
  char sOut[8192];
  
  if(GetStringFromSocket(socket,sOut))
  {
    close(socket);
    return 1;
  }

  nRetVal=RunSqlStmt(sOut);
  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }
  return 0;
}
#endif

#ifdef DATASERVER
long RunSqlStmt(char *sOut)
{
  #ifdef TEST
  fprintf(stderr,"%s: RunSql: %s\n",sDsUserName, sOut);
  #endif

  /* oparse() parses a SQL statement or a PL/SQL block and associates it 
     with a cursor. The parse can optionally be deferred. */
  /* Parse the statement; do not defer the parse,
     so that errors come back right away. */
  if (oparse(&cda, (text *) sOut, (sb4) -1,
       (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
    nSqlErrNo=cda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  /* Save the SQL function code right after parse. */
  sql_function = cda.ft;

  /* Execute the statement. */
  if (oexec(&cda))
  {
    nSqlErrNo=cda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  if(sql_function==FT_UPDATE||sql_function==FT_DELETE||sql_function==FT_INSERT)
    return cda.rpc;
  else
    return 1; 
} 
#endif

#ifdef DATACLIENT
long RunSql(char *sFormat,...)
{
  long nErrorFlag=0,/*语法错误标志,0为false,1为true*/
       nPrecision,/*精度为*号*/ 
       nPrecisionFlag,/*精度为*号标志,0为False,1为true*/
       nCharCount,/*返回实际输出 的字节数*/
       nType;/*变长列表中参数的类型*/
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char* s;
  char sTempFormat[8192];/*临时格式*/
  char sTemp[8192];/*临时输出*/
  char sOut[8192];/*输出字符串*/
  va_list pVarArgList;/*变长参数列表指针*/
  long nRetVal;

  if(nOciSocket==0) ConnectDatabase(sSysManager,sSysPassword);

  if(nOciSocket==0)
    return -1012;

  if((strstr(sFormat,"procedure")||strstr(sFormat,"PROCEDURE"))&&
     (strstr(sFormat,"create")||strstr(sFormat,"CREATE"))&&
     (strstr(sFormat,"begin")||strstr(sFormat,"BEGIN"))&&
     (strstr(sFormat,"end")||strstr(sFormat,"END")))
    strcpy(sOut,sFormat);
  else
  {
    sTop=sFormat;
    sBottom=sFormat;

    sOut[0]='\0';
    nType=TYPE_NULL;

    va_start(pVarArgList,sFormat);

    while(sTop&&sTop[0])
    {
      nType=0;
      nPrecisionFlag=0;
      sBottom=(char*)strchr(sTop,'%');
      if(sBottom==NULL)
        nType=TYPE_NULL;
      else
        sBottom++;
      while(nType==0)
      {
        switch(sBottom[0])
        {
          case 'd':
          case 'i':
          case 'o':
          case 'u':
          case 'x':
          case 'X':
            nType=TYPE_INT;
            break;
          case 'h':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_SHORT;
                  break;
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stderr,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'f':
          case 'e':
          case 'E':
          case 'g':
          case 'G':
            nType=TYPE_FLOAT;
            break;
          case 'l':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_LONG;
                  break;
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_DOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'L':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_LONGDOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'c':
            nType=TYPE_CHAR;
            break;
          case 's':
            nType=TYPE_STRING; 
            break;
          case 't':
            nType=TYPE_TABLE; 
            break;
          case '*':
            nPrecisionFlag=1;
            nPrecision=va_arg(pVarArgList,long);
            break;
          case '%':
            sBottom++;
            sBottom=(char*)strchr(sBottom,'%');
            break;
          case '\0':
            nType=TYPE_NULL;
            break;
        }
        if (nType||nErrorFlag)
          break;
        if(sBottom!=NULL&&sBottom[0]!='\0')
          sBottom++;
      }
  
      if(nErrorFlag)
      {
        va_end(pVarArgList);
        return 0;
      }

      if(nType!=TYPE_NULL)
      {
        strncpy(sTempFormat,sTop,sBottom-sTop+1);
        sTempFormat[sBottom-sTop+1]='\0';
        sTop=sBottom+1;
        if(nType == TYPE_TABLE)
        {
          s = strstr(sTempFormat, "%t");
          s[1] = 's';
        }
      }
      else
      {
        strcpy(sTempFormat,sTop);
        sTop=&sFormat[strlen(sFormat)];
      }

      switch(nType)
      { 
        case TYPE_INT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,int));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,int));
          break;
        case TYPE_LONG:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long));
          break;
        case TYPE_SHORT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,short));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,short));
          break;
        case TYPE_FLOAT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,float));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,float));
          break;
        case TYPE_DOUBLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,double));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,double));
          break;
        case TYPE_LONGDOUBLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long double));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long double));
          break;
        case TYPE_CHAR:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char));
          break;
        case TYPE_STRING:
          s=sTempFormat+strlen(sTempFormat)-1;
          *(s+3)='\0';
          *(s+2)='\'';
          while(*s!='%')
          {
            *(s+1)=*s;
            s--;
          }
          *(s+1)='%';
          *s='\'';
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
          break;
        case TYPE_TABLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
          break;
        case TYPE_NULL:
          strcpy(sTemp,sTempFormat);
          break;
      }
      strcat(sOut,sTemp);
    }

    va_end(pVarArgList);
  }
  strcpy(sSysSqlStatement,sOut);  //lee1
  if(SendLongToSocket(nOciSocket,(long)TASK_RUNSQL))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(SendStringToSocket(nOciSocket,sOut))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  return nRetVal;
} 
#endif

#ifdef OCI
long RunSql(char *sFormat,...)
{
  long nErrorFlag=0,/*语法错误标志,0为false,1为true*/
       nPrecision,/*精度为*号*/ 
       nPrecisionFlag,/*精度为*号标志,0为False,1为true*/
       nCharCount,/*返回实际输出 的字节数*/
       nType;/*变长列表中参数的类型*/
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char* s;
  char sTempFormat[8192];/*临时格式*/
  char sTemp[8192];/*临时输出*/
  char sOut[8192];/*输出字符串*/
  va_list pVarArgList;/*变长参数列表指针*/

  if((strstr(sFormat,"procedure")||strstr(sFormat,"PROCEDURE"))&&
     (strstr(sFormat,"create")||strstr(sFormat,"CREATE"))&&
     (strstr(sFormat,"begin")||strstr(sFormat,"BEGIN"))&&
     (strstr(sFormat,"end")||strstr(sFormat,"END")))
    strcpy(sOut,sFormat);
  else
  {
    sTop=sFormat;
    sBottom=sFormat;

    sOut[0]='\0';
    nType=TYPE_NULL;

    va_start(pVarArgList,sFormat);
  
    while(sTop&&sTop[0])
    {
      nType=0;
      nPrecisionFlag=0;
      sBottom=(char*)strchr(sTop,'%');
      if(sBottom==NULL)
        nType=TYPE_NULL;
      else
        sBottom++;
      while(nType==0)
      {
        switch(sBottom[0])
        {
          case 'd':
          case 'i':
          case 'o':
          case 'u':
          case 'x':
          case 'X':
            nType=TYPE_INT;
            break;
          case 'h':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_SHORT;
                  break;
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stderr,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'f':
          case 'e':
          case 'E':
          case 'g':
          case 'G':
            nType=TYPE_FLOAT;
            break;
          case 'l':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_LONG;
                  break;
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_DOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom!=NULL&&sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'L':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_LONGDOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'c':
            nType=TYPE_CHAR;
            break;
          case 's':
            nType=TYPE_STRING; 
            break;
          case 't':
            nType=TYPE_TABLE; 
            break;
          case '*':
            nPrecisionFlag=1;
            nPrecision=va_arg(pVarArgList,long);
            break;
          case '%':
            sBottom++;
            sBottom=(char*)strchr(sBottom,'%');
            break;
          case '\0':
            nType=TYPE_NULL;
            break;
        }
        if (nType||nErrorFlag)
          break;
        if(sBottom[0]!='\0')
          sBottom++;
      }

      if(nErrorFlag)
      {
        va_end(pVarArgList);
        return 0;
      }

      if(nType!=TYPE_NULL)
      {
        strncpy(sTempFormat,sTop,sBottom-sTop+1);
        sTempFormat[sBottom-sTop+1]='\0';
        sTop=sBottom+1;
        if(nType == TYPE_TABLE)
        {
          s = strstr(sTempFormat, "%t");
          s[1] = 's';
        }
      }
      else
      {
        strcpy(sTempFormat,sTop);
        sTop=&sFormat[strlen(sFormat)];
      }

      switch(nType)
      { 
        case TYPE_INT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,int));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,int));
          break;
        case TYPE_LONG:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long));
          break;
        case TYPE_SHORT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,short));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,short));
          break;
        case TYPE_FLOAT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,float));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,float));
          break;
        case TYPE_DOUBLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,double));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,double));
          break;
        case TYPE_LONGDOUBLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long double));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long double));
          break;
        case TYPE_CHAR:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char));
          break;
        case TYPE_STRING:
          s=sTempFormat+strlen(sTempFormat)-1;
          *(s+3)='\0';
          *(s+2)='\'';
          while(*s!='%')
          {
            *(s+1)=*s;
            s--;
          }
          *(s+1)='%';
          *s='\'';
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
          break;
        case TYPE_TABLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
          break;
        case TYPE_NULL:
          strcpy(sTemp,sTempFormat);
          break;
      }
      strcat(sOut,sTemp);
    }
 
    va_end(pVarArgList);
  }

  #ifdef TEST
  fprintf(stderr,"%s: RunSql: %s\n",sDsUserName, sOut);
  #endif


  /* oparse() parses a SQL statement or a PL/SQL block and associates it 
     with a cursor. The parse can optionally be deferred. */
  /* Parse the statement; do not defer the parse,
     so that errors come back right away. */
  if (oparse(&cda, (text *) sOut, (sb4) -1,
       (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
    nSqlErrNo=cda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  /* Save the SQL function code right after parse. */
  sql_function = cda.ft;

  /* Execute the statement. */
  if (oexec(&cda))
  {
    nSqlErrNo=cda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  if(sql_function==FT_UPDATE||sql_function==FT_DELETE||sql_function==FT_INSERT)
    return cda.rpc;
  else
    return 1; 
} 
#endif

#ifdef DATASERVER
int dsOpenCursor(int socket)
{
  long nRetVal;
  char sOut[8192];
  
  if(GetStringFromSocket(socket,sOut))
  {
    close(socket);
    return 1;
  }
  nRetVal=OpenCursorStmt(sOut);

  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }

  return 0;
}
#endif

#ifdef DATASERVER
long OpenCursorStmt(char *sOut)
{
  long nCursorId;

  nCursorId=1;
  while(sCursorFlag[nCursorId-1]=='1'&&nCursorId<=MAX_CURSOR_NUM)
    nCursorId++; 

  #ifdef TEST
  fprintf(stderr,"%s: OpenCursor: %ld: %s\n",sDsUserName,nCursorId,sOut);
  #endif

  if(nCursorId==MAX_CURSOR_NUM+1)
  {
    nSqlErrNo=1000;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  sCursorFlag[nCursorId-1]='1'; 

  if (oopen(&cursor[nCursorId-1], &lda, (text *) 0, -1, -1, (text *) 0, -1))
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  /* Parse the statement; do not defer the parse,
       so that errors come back right away. */
  if (oparse(&cursor[nCursorId-1], (text *) sOut, (sb4) -1,
             (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
     nSqlErrNo=cursor[nCursorId-1].rc;
     CloseCursor(nCursorId);
     #ifdef TEST
     ShowTestErrMsg();
     #endif
     return -nSqlErrNo;
  }

  /* Save the SQL function code right after parse. */
  sql_function = cursor[nCursorId-1].ft;

  if(sql_function!=FT_SELECT)
  {
    CloseCursor(nCursorId);
    nSqlErrNo=ERR_NO_SELECT;
    #ifdef TEST
    fprintf(stderr,"%s: OpenCursor: not SELECT statement\n",sDsUserName);
    #endif

    return -nSqlErrNo;
  }

  /* describe and define all select-list items. */
  if((nColNum[nCursorId-1] = describe_define(&cursor[nCursorId-1])) <0)
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    CloseCursor(nCursorId);
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  /* Execute the statement. */
  if (oexec(&cursor[nCursorId-1]))
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  return nCursorId;
}
#endif

#ifdef DATACLIENT
long OpenCursor(char *sFormat,...)
{
  long nErrorFlag=0,/*语法错误标志,0为false,1为true*/
       nPrecision,/*精度为*号*/ 
       nPrecisionFlag,/*精度为*号标志,0为False,1为true*/
       nCharCount,/*返回实际输出 的字节数*/
       nType;/*变长列表中参数的类型*/
  long nCursorId;
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char* s;
  char sTempFormat[8192];/*临时格式*/
  char sTemp[8192];/*临时输出*/
  char sOut[8192];/*输出字符串*/
  va_list pVarArgList;/*变长参数列表指针*/
  long nRetVal;

  if(nOciSocket==0) ConnectDatabase(sSysManager,sSysPassword);

  if(nOciSocket==0)
    return -1012;

  sTop=sFormat;
  sBottom=sFormat;

  sOut[0]='\0';
  nType=TYPE_NULL;

  va_start(pVarArgList,sFormat);

  while(sTop&&sTop[0])
  {
    nType=0;
    nPrecisionFlag=0;
    sBottom=(char*)strchr(sTop,'%');
    if(sBottom==NULL)
      nType=TYPE_NULL;
    else
      sBottom++;
    while(nType==0)
    {
      switch(sBottom[0])
      {
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
          nType=TYPE_INT;
          break;
        case 'h':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'd':
              case 'i':
              case 'o':
              case 'x':
              case 'X':
              case 'u':
                nType=TYPE_SHORT;
                break;
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stderr,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'f':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
          nType=TYPE_FLOAT;
          break;
        case 'l':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'd':
              case 'i':
              case 'o':
              case 'x':
              case 'X':
              case 'u':
                nType=TYPE_LONG;
                break;
              case 'f':
              case 'e':
              case 'E':
              case 'g':
              case 'G':
                nType=TYPE_DOUBLE;
                break;  
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stdout,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'L':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'f':
              case 'e':
              case 'E':
              case 'g':
              case 'G':
                nType=TYPE_LONGDOUBLE;
                break;  
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stdout,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'c':
          nType=TYPE_CHAR;
          break;
        case 's':
          nType=TYPE_STRING; 
          break;
        case 't':
          nType=TYPE_TABLE; 
          break;
        case '*':
          nPrecisionFlag=1;
          nPrecision=va_arg(pVarArgList,long);
          break;
        case '%':
          sBottom++;
          sBottom=(char*)strchr(sBottom,'%');
          break;
        case '\0':
          nType=TYPE_NULL;
          break;
      }
      if (nType||nErrorFlag)
        break;
      if(sBottom!=NULL&&sBottom[0]!='\0')
        sBottom++;
    }

    if(nErrorFlag)
    {
      va_end(pVarArgList);
      return 0;
    }

    if(nType!=TYPE_NULL)
    {
      strncpy(sTempFormat,sTop,sBottom-sTop+1);
      sTempFormat[sBottom-sTop+1]='\0';
      sTop=sBottom+1;
      if(nType == TYPE_TABLE)
      {
        s = strstr(sTempFormat, "%t");
        s[1] = 's';
      }
    }
    else
    {
      strcpy(sTempFormat,sTop);
      sTop=&sFormat[strlen(sFormat)];
    }

    switch(nType)
    { 
      case TYPE_INT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,int));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,int));
        break;
      case TYPE_LONG:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long));
        break;
      case TYPE_SHORT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,short));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,short));
        break;
      case TYPE_FLOAT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,float));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,float));
        break;
      case TYPE_DOUBLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,double));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,double));
        break;
      case TYPE_LONGDOUBLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long double));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long double));
        break;
      case TYPE_CHAR:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char));
        break;
      case TYPE_STRING:
        s=sTempFormat+strlen(sTempFormat)-1;
        *(s+3)='\0';
        *(s+2)='\'';
        while(*s!='%')
        {
          *(s+1)=*s;
          s--;
        }
        *(s+1)='%';
        *s='\'';
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
        break;
      case TYPE_TABLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
        break;
      case TYPE_NULL:
        strcpy(sTemp,sTempFormat);
        break;
    }
    strcat(sOut,sTemp);
  }
  strcpy(sSysSqlStatement,sOut);   //lee2

  va_end(pVarArgList);

  if(SendLongToSocket(nOciSocket,(long)TASK_OPENCURSOR))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(SendStringToSocket(nOciSocket,sOut))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  return nRetVal;

}
#endif

#ifdef OCI
long OpenCursor(char *sFormat,...)
{
  long nErrorFlag=0,/*语法错误标志,0为false,1为true*/
       nPrecision,/*精度为*号*/ 
       nPrecisionFlag,/*精度为*号标志,0为False,1为true*/
       nCharCount,/*返回实际输出 的字节数*/
       nType;/*变长列表中参数的类型*/
  long nCursorId;
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char* s;
  char sTempFormat[8192];/*临时格式*/
  char sTemp[8192];/*临时输出*/
  char sOut[8192];/*输出字符串*/
  va_list pVarArgList;/*变长参数列表指针*/

  nCursorId=1;
  while(sCursorFlag[nCursorId-1]=='1'&&nCursorId<=MAX_CURSOR_NUM)
    nCursorId++; 
  if(nCursorId==MAX_CURSOR_NUM+1)
  {
    nSqlErrNo=1000;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  sCursorFlag[nCursorId-1]='1'; 
  sTop=sFormat;
  sBottom=sFormat;

  sOut[0]='\0';
  nType=TYPE_NULL;

  va_start(pVarArgList,sFormat);

  while(sTop&&sTop[0])
  {
    nType=0;
    nPrecisionFlag=0;
    sBottom=(char*)strchr(sTop,'%');
    if(sBottom==NULL)
      nType=TYPE_NULL;
    else
      sBottom++;
    while(nType==0)
    {
      switch(sBottom[0])
      {
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
          nType=TYPE_INT;
          break;
        case 'h':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'd':
              case 'i':
              case 'o':
              case 'x':
              case 'X':
              case 'u':
                nType=TYPE_SHORT;
                break;
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stderr,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'f':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
          nType=TYPE_FLOAT;
          break;
        case 'l':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'd':
              case 'i':
              case 'o':
              case 'x':
              case 'X':
              case 'u':
                nType=TYPE_LONG;
                break;
              case 'f':
              case 'e':
              case 'E':
              case 'g':
              case 'G':
                nType=TYPE_DOUBLE;
                break;  
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stdout,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'L':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'f':
              case 'e':
              case 'E':
              case 'g':
              case 'G':
                nType=TYPE_LONGDOUBLE;
                break;  
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stdout,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom!=NULL&&sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'c':
          nType=TYPE_CHAR;
          break;
        case 's':
          nType=TYPE_STRING; 
          break;
        case 't':
          nType=TYPE_TABLE; 
          break;
        case '*':
          nPrecisionFlag=1;
          nPrecision=va_arg(pVarArgList,long);
          break;
        case '%':
          sBottom++;
          sBottom=(char*)strchr(sBottom,'%');
          break;
        case '\0':
          nType=TYPE_NULL;
          break;
      }
      if (nType||nErrorFlag)
        break;
      if(sBottom[0]!='\0')
        sBottom++;
    }

    if(nErrorFlag)
    {
      va_end(pVarArgList);
      return 0;
    }

    if(nType!=TYPE_NULL)
    {
      strncpy(sTempFormat,sTop,sBottom-sTop+1);
      sTempFormat[sBottom-sTop+1]='\0';
      sTop=sBottom+1;
      if(nType == TYPE_TABLE)
      {
        s = strstr(sTempFormat, "%t");
        s[1] = 's';
      }
    }
    else
    {
      strcpy(sTempFormat,sTop);
      sTop=&sFormat[strlen(sFormat)];
    }

    switch(nType)
    { 
      case TYPE_INT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,int));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,int));
        break;
      case TYPE_LONG:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long));
        break;
      case TYPE_SHORT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,short));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,short));
        break;
      case TYPE_FLOAT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,float));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,float));
        break;
      case TYPE_DOUBLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,double));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,double));
        break;
      case TYPE_LONGDOUBLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long double));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long double));
        break;
      case TYPE_CHAR:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char));
        break;
      case TYPE_STRING:
        s=sTempFormat+strlen(sTempFormat)-1;
        *(s+3)='\0';
        *(s+2)='\'';
        while(*s!='%')
        {
          *(s+1)=*s;
          s--;
        }
        *(s+1)='%';
        *s='\'';
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
        break;
      case TYPE_TABLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
        break;
      case TYPE_NULL:
        strcpy(sTemp,sTempFormat);
        break;
    }
    strcat(sOut,sTemp);
  }

  va_end(pVarArgList);

  #ifdef TEST
  fprintf(stderr,"%s: OpenCursor: %ld: %s\n",sDsUserName,nCursorId,sOut);
  #endif

  if (oopen(&cursor[nCursorId-1], &lda, (text *) 0, -1, -1, (text *) 0, -1))
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  /* Parse the statement; do not defer the parse,
       so that errors come back right away. */
  if (oparse(&cursor[nCursorId-1], (text *) sOut, (sb4) -1,
             (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
     nSqlErrNo=cursor[nCursorId-1].rc;
     CloseCursor(nCursorId);
     #ifdef TEST
     ShowTestErrMsg();
     #endif
     return -nSqlErrNo;
  }

  /* Save the SQL function code right after parse. */
  sql_function = cursor[nCursorId-1].ft;

  if(sql_function!=FT_SELECT)
  {
    CloseCursor(nCursorId);
    fprintf(stdout,"\n\a非SELECT语句\n");
    keyb();
    return 0;
  }

  /* describe and define all select-list items. */
  if((nColNum[nCursorId-1] = describe_define(&cursor[nCursorId-1])) <0)
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    CloseCursor(nCursorId);
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  /* Execute the statement. */
  if (oexec(&cursor[nCursorId-1]))
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  return nCursorId;
}
#endif

/*  Describe select-list items. */

#ifndef DATACLIENT
sword describe_define(Cda_Def *cda)
{
    sword col, deflen, deftyp;
    static ub1 *defptr;

    /* Describe the select-list items. */
    for (col = 0; col < MAX_SELECT_LIST_SIZE; col++)
    {
        desc[col].buflen = MAX_ITEM_BUFFER_SIZE;
        if (odescr(cda, col + 1, &desc[col].dbsize,
                   &desc[col].dbtype, &desc[col].buf[0],
                   &desc[col].buflen, &desc[col].dsize,
                   &desc[col].precision, &desc[col].scale,
                   &desc[col].nullok))
        {
            /* Break on end of select list. */
            if (cda->rc == VAR_NOT_IN_LIST)
                break;
            else
            {
              nSqlErrNo=cda->rc;
              return -nSqlErrNo;
            }
        }
        /* adjust sizes and types for display */
        switch (desc[col].dbtype)
        {
        case NUMBER_TYPE:
            desc[col].dbsize = numwidth;

            defptr = def[col].buf;
            deflen = desc[col].dbsize + 1;
            deftyp = STRING_TYPE;
            desc[col].dbtype = STRING_TYPE;

            /* Handle NUMBER with scale as float. */
            /*
            if (desc[col].scale != 0)
            {
                defptr = (ub1 *) &def[col].flt_buf;
                deflen = (sword) sizeof(float);
                deftyp = FLOAT_TYPE;
                desc[col].dbtype = FLOAT_TYPE;
            }
            else
            {
                defptr = (ub1 *) &def[col].int_buf;
                deflen = (sword) sizeof(sword);
                deftyp = INT_TYPE;
                desc[col].dbtype = INT_TYPE;
            }
            */
            break;
        default:
            if (desc[col].dbtype == DATE_TYPE)
                desc[col].dbsize = 9;
            if (desc[col].dbtype == ROWID_TYPE)
                desc[col].dbsize = 18;
            defptr = def[col].buf;
            deflen = desc[col].dbsize > MAX_ITEM_BUFFER_SIZE ?
              MAX_ITEM_BUFFER_SIZE : desc[col].dbsize + 1;
            deftyp = STRING_TYPE;
            break;
        }
        if (odefin(cda, col + 1,
                   defptr, deflen, deftyp,
                   -1, &def[col].indp, (text *) 0, -1, -1,
                   &def[col].col_retlen,
                   &def[col].col_retcode))
        {
            nSqlErrNo=cda->rc;
            return -nSqlErrNo;
        }
    }
    return col;
}
#endif

#ifdef DATASERVER
int dsFetchCursor(int socket)
{
  long nRetVal;
  long col;
  long nCursorId;
  
  char sTemp[8192];
  char* s;
  char* s1;
  long n,i;
  int l;

  if(GetLongFromSocket(socket,&nCursorId))
  {
    close(socket);
    return 1;
  }

  #ifdef TEST
/*
  fprintf(stderr,"%s: FetchCursor: %ld\n",sDsUserName,nCursorId);
*/
  #endif

  if (ofetch(&cursor[nCursorId-1]))
  {
    if (cursor[nCursorId-1].rc == NO_DATA_FOUND)
      nRetVal=0;
    else
    {
      nSqlErrNo=cursor[nCursorId-1].rc;
      #ifdef TEST
      ShowTestErrMsg();
      #endif
      nRetVal=-nSqlErrNo;
    }
  }
  else
    nRetVal=1;

  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }

  if(nRetVal==1)
  {
    if(SendLongToSocket(socket,nColNum[nCursorId-1]))
    {
      close(socket);
      return 1;
    }
    
    s=sTemp;
    l=0;
    
    for (col = 0; col < nColNum[nCursorId-1]; col++)
    {
      switch (desc[col].dbtype)
      {
        case FLOAT_TYPE:
          /*
          if(SendLongToSocket(socket,TYPE_DOUBLE))
          {
            close(socket);
            return 1;
          }
          if(SendDoubleToSocket(socket,(double)def[col].flt_buf))
          {
            close(socket);
            return 1;
          }
          */
          break;
        case INT_TYPE:
          /*
          if(SendLongToSocket(socket,TYPE_LONG))
          {
            close(socket);
            return 1;
          }
          if(SendLongToSocket(socket,def[col].int_buf))
          {
            close(socket);
            return 1;
          }
          */
          break;
        default:
          n=(long)TYPE_STRING;
          #ifdef LITTLEEND
          n = ReverseLong1(n);
          #endif
          s1=(char*)&n;
          for(i=0;i<4;i++)
          {
            s[i]=s1[i];
          }
          s+=4;
          l+=4;

          n=(long)strlen((char*)def[col].buf);
          #ifdef LITTLEEND
          n = ReverseLong1(n);
          #endif
          s1=(char*)&n;
          for(i=0;i<4;i++)
          {
            s[i]=s1[i];
          }
          s+=4;
          l+=4;

          strcpy(s,(char*)def[col].buf);
          #ifdef LITTLEEND
          n = ReverseLong2(n);
          #endif
          s+=n;
          l+=n;
          /*
          if(SendLongToSocket(socket,TYPE_STRING))
          {
            close(socket);
            return 1;
          }
          if(SendStringToSocket(socket,(char*)def[col].buf))
          {
            close(socket);
            return 1;
          }
          */
          break;
      }
    }
    if(SendRecordToSocket(socket,sTemp,l))
    {
      close(socket);
      return 1;
    }
  }
  return 0;
}
#endif

#ifdef DATACLIENT
long FetchCursor(long nCursorId,char *sFormat,...)
{
  long nRetVal;
  long col;
  double *pdValue;
  long *pnValue;
  char *sValue;
  long nColNum;
  long nType;

  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char sTemp[8192];
  char sTempFormat[8192];/*临时格式*/

  va_list pVarArgList;/*变长参数列表指针*/
  
  if(nOciSocket==0)
    return -1012;

  if(SendLongToSocket(nOciSocket,(long)TASK_FETCHCURSOR))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(SendLongToSocket(nOciSocket,nCursorId))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(nRetVal==1)
  {
    if(GetLongFromSocket(nOciSocket,&nColNum))
    {
      nOciSocket=0;
      return -ERR_NO_TCPIP;
    }
    va_start(pVarArgList,sFormat);

    sTop=sFormat;
  
    for (col = 0; col < nColNum; col++)
    {
      if(GetLongFromSocket(nOciSocket,&nType))
      {
        nOciSocket=0;
        return -ERR_NO_TCPIP;
      }
      sTop=strchr(sTop,'%');
      if(sTop==NULL)
      {
        fprintf(stderr,"FETCH格式错误\\n");
        keyb();
        return -ERR_NO_FETCH;
      }
      sBottom=strchr(sTop+1,'%');
      if(sBottom)
      {
        strncpy(sTempFormat,sTop+1,sBottom-sTop-1);
        sTempFormat[sBottom-sTop-1]='\0';
        sTop=sBottom;
      }
      else
      {
        strcpy(sTempFormat,sTop+1);
        sTop=NULL;
      }
      alltrim(sTempFormat);
      if(sTempFormat[strlen(sTempFormat)-1]==',')
      {
        sTempFormat[strlen(sTempFormat)-1]='\0'; 
        alltrim(sTempFormat);
      }

      switch (nType)
      {
        case TYPE_DOUBLE:
          pdValue=va_arg(pVarArgList,double *);
          if(GetDoubleFromSocket(nOciSocket,pdValue))
          {
            va_end(pVarArgList);
            nOciSocket=0;
            return -ERR_NO_TCPIP;
          }
          break;
        case TYPE_LONG:
          pnValue=va_arg(pVarArgList,long *);
          if(GetLongFromSocket(nOciSocket,pnValue))
          {
            va_end(pVarArgList);
            nOciSocket=0;
            return -ERR_NO_TCPIP;
          }
          break;
        default:
          if(GetStringFromSocket(nOciSocket,sTemp))
          {
            va_end(pVarArgList);
            nOciSocket=0;
            return -ERR_NO_TCPIP;
          }

          if(!strcmp(sTempFormat,"ld"))
          {
            pnValue=va_arg(pVarArgList,long *);
            *(long*)pnValue=atol(sTemp);
            continue;
          }
          if(!strcmp(sTempFormat,"lf"))
          {
            pdValue=va_arg(pVarArgList,double *);
            *(double*)pdValue=atof(sTemp);
            continue;
          }
          if(!strcmp(sTempFormat,"s"))
          {
            sValue=va_arg(pVarArgList,char *);
            strcpy(sValue,sTemp);
            continue;
          }
          fprintf(stderr,"Format String %s cannot be identified",sTempFormat);
          keyb(); 
          break;
      }
    }
    va_end(pVarArgList);
  }
  return nRetVal;
}

#endif

#ifdef OCI
long FetchCursor(long nCursorId,char *sFormat,...)
{
  long nRetVal;
  long col;
  double *pdValue;
  long *pnValue;
  char *sValue;

  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char sTemp[8192];
  char sTempFormat[8192];/*临时格式*/

  va_list pVarArgList;/*变长参数列表指针*/

  if (ofetch(&cursor[nCursorId-1]))
  {
    if (cursor[nCursorId-1].rc == NO_DATA_FOUND)
      nRetVal=0;
    else
    {
      nSqlErrNo=cursor[nCursorId-1].rc;
      #ifdef TEST
      ShowTestErrMsg();
      #endif
      return -nSqlErrNo;
    }
  }
  else
    nRetVal=1;

  va_start(pVarArgList,sFormat);

  sTop=sFormat;

  for (col = 0; col < nColNum[nCursorId-1]; col++)
  {
    sTop=strchr(sTop,'%');
    if(sTop==NULL)
    {
      fprintf(stderr,"FETCH格式错误\\n");
      keyb();
      return -ERR_NO_FETCH;
    }
    sBottom=strchr(sTop+1,'%');
    if(sBottom)
    {
      strncpy(sTempFormat,sTop+1,sBottom-sTop-1);
      sTempFormat[sBottom-sTop-1]='\0';
      sTop=sBottom;
    }
    else
    {
      strcpy(sTempFormat,sTop+1);
      sTop=NULL;
    }
    alltrim(sTempFormat);
    if(sTempFormat[strlen(sTempFormat)-1]==',')
    {
      sTempFormat[strlen(sTempFormat)-1]='\0'; 
      alltrim(sTempFormat);
    }

    switch (desc[col].dbtype)
    {
      case FLOAT_TYPE:
        pdValue=va_arg(pVarArgList,double *);
        *(double*)pdValue=(double)def[col].flt_buf;
        break;
      case INT_TYPE:
        pnValue=va_arg(pVarArgList,long *);
        *(long*)pnValue=def[col].int_buf;
        break;
      default:
        if(!strcmp(sTempFormat,"ld"))
        {
          pnValue=va_arg(pVarArgList,long *);
          *(long*)pnValue=atol((char*)def[col].buf);
          continue;
        }
        if(!strcmp(sTempFormat,"lf"))
        {
          pdValue=va_arg(pVarArgList,double *);
          *(double*)pdValue=atof((char*)def[col].buf);
          continue;
        }
        if(!strcmp(sTempFormat,"s"))
        {
          sValue=va_arg(pVarArgList,char *);
          strcpy(sValue,(char*)def[col].buf);
          continue;
        }
        fprintf(stderr,"Format String %s cannot be identified",sTempFormat);
        keyb(); 
        break;
    }
  }

  va_end(pVarArgList);
  return nRetVal;
}
#endif

#ifdef DATASERVER
int dsFetchCursorArray(int socket)
{
  long nRetVal;
  long col;
  long nCursorId;
  
  char sTemp[8192];
  char* s;
  char* s1;
  long n,i;
  int l;
  int nLoop;
  long nRecNum;

  if(GetLongFromSocket(socket,&nCursorId))
  {
    close(socket);
    return 1;
  }

  if(GetLongFromSocket(socket,&nRecNum))
  {
    close(socket);
    return 1;
  }

  #ifdef TEST
/*
  fprintf(stderr,"%s: FetchCursor: %ld\n",sDsUserName,nCursorId);
*/
  #endif

  for(nLoop = 1; nLoop <= nRecNum; nLoop++)
  {

    if (ofetch(&cursor[nCursorId-1]))
    {
      if (cursor[nCursorId-1].rc == NO_DATA_FOUND)
        nRetVal=0;
      else
      {
        nSqlErrNo=cursor[nCursorId-1].rc;
        #ifdef TEST
        ShowTestErrMsg();
        #endif
        nRetVal=-nSqlErrNo;
      }
    }
    else
      nRetVal=1;

    if(SendLongToSocket(socket,nRetVal))
    {
      close(socket);
      return 1;
    }

    if(nRetVal != 1)
    {
      return 0; 
    } 
      
    if(SendLongToSocket(socket,nColNum[nCursorId-1]))
    {
      close(socket);
      return 1;
    }
    
    s=sTemp;
    l=0;
    
    for (col = 0; col < nColNum[nCursorId-1]; col++)
    {
      switch (desc[col].dbtype)
      {
        case FLOAT_TYPE:
          /*
          if(SendLongToSocket(socket,TYPE_DOUBLE))
          {
            close(socket);
            return 1;
          }
          if(SendDoubleToSocket(socket,(double)def[col].flt_buf))
          {
            close(socket);
            return 1;
          }
          */
          break;
        case INT_TYPE:
          /*
          if(SendLongToSocket(socket,TYPE_LONG))
          {
            close(socket);
            return 1;
          }
          if(SendLongToSocket(socket,def[col].int_buf))
          {
            close(socket);
            return 1;
          }
          */
          break;
        default:
          n=(long)TYPE_STRING;
          #ifdef LITTLEEND
          n = ReverseLong1(n);
          #endif
          s1=(char*)&n;
          for(i=0;i<4;i++)
          {
            s[i]=s1[i];
          }
          s+=4;
          l+=4;

          n=(long)strlen((char*)def[col].buf);
          #ifdef LITTLEEND
          n = ReverseLong1(n);
          #endif
          s1=(char*)&n;
          for(i=0;i<4;i++)
          {
            s[i]=s1[i];
          }
          s+=4;
          l+=4;

          strcpy(s,(char*)def[col].buf);
          #ifdef LITTLEEND
          n = ReverseLong2(n);
          #endif
          s+=n;
          l+=n;
          /*
          if(SendLongToSocket(socket,TYPE_STRING))
          {
            close(socket);
            return 1;
          }
          if(SendStringToSocket(socket,(char*)def[col].buf))
          {
            close(socket);
            return 1;
          }
          */
          break;
      }
    }
    if(SendRecordToSocket(socket,sTemp,l))
    {
      close(socket);
      return 1;
    }
  }
  return 0;
}
#endif

#ifdef DATACLIENT
long FetchCursorArray(long nCursorId, long* nLenArray, long nRecNum, char *sFormat,...)
{
  long nRetVal;
  long col;
  double *pdValue;
  long *pnValue;
  char *sValue;
  long nColNum;
  long nType;

  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char sTemp[8192];
  char sTempFormat[8192];/*临时格式*/

  va_list pVarArgList;/*变长参数列表指针*/
  
  int nLoop;
  long nRealNum = 0;
  
  if(nOciSocket==0)
    return -1012;

  if(SendLongToSocket(nOciSocket,(long)TASK_FETCHCURSORARRAY))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(SendLongToSocket(nOciSocket,nCursorId))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(SendLongToSocket(nOciSocket,nRecNum))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  for(nLoop = 0; nLoop <= nRecNum - 1; nLoop++)
  {
    if(GetLongFromSocket(nOciSocket,&nRetVal))
    {
      nOciSocket=0;
      return -ERR_NO_TCPIP;
    }

    if(nRetVal == 0)
    {
      break;
    }


    if(nRetVal!=1)
    {
      return nRetVal;
    }
    
    nRealNum++;
    
    if(GetLongFromSocket(nOciSocket,&nColNum))
    {
      nOciSocket=0;
      return -ERR_NO_TCPIP;
    }
    va_start(pVarArgList,sFormat);

    sTop=sFormat;
  
    for (col = 0; col < nColNum; col++)
    {
      if(GetLongFromSocket(nOciSocket,&nType))
      {
        nOciSocket=0;
        return -ERR_NO_TCPIP;
      }
      sTop=strchr(sTop,'%');
      if(sTop==NULL)
      {
        fprintf(stderr,"FETCH格式错误\\n");
        keyb();
        return -ERR_NO_FETCH;
      }
      sBottom=strchr(sTop+1,'%');
      if(sBottom)
      {
        strncpy(sTempFormat,sTop+1,sBottom-sTop-1);
        sTempFormat[sBottom-sTop-1]='\0';
        sTop=sBottom;
      }
      else
      {
        strcpy(sTempFormat,sTop+1);
        sTop=NULL;
      }
      alltrim(sTempFormat);
      if(sTempFormat[strlen(sTempFormat)-1]==',')
      {
        sTempFormat[strlen(sTempFormat)-1]='\0'; 
        alltrim(sTempFormat);
      }

      switch (nType)
      {
        case TYPE_DOUBLE:
          pdValue=va_arg(pVarArgList,double *) + nLoop;
          if(GetDoubleFromSocket(nOciSocket,pdValue))
          {
            va_end(pVarArgList);
            nOciSocket=0;
            return -ERR_NO_TCPIP;
          }
          break;
        case TYPE_LONG:
          pnValue=va_arg(pVarArgList,long *) + nLoop;
          if(GetLongFromSocket(nOciSocket,pnValue))
          {
            va_end(pVarArgList);
            nOciSocket=0;
            return -ERR_NO_TCPIP;
          }
          break;
        default:
          if(GetStringFromSocket(nOciSocket,sTemp))
          {
            va_end(pVarArgList);
            nOciSocket=0;
            return -ERR_NO_TCPIP;
          }

          if(!strcmp(sTempFormat,"ld"))
          {
            pnValue=va_arg(pVarArgList,long *) + nLoop;
            *(long*)pnValue=atol(sTemp);
            continue;
          }
          if(!strcmp(sTempFormat,"lf"))
          {
            pdValue=va_arg(pVarArgList,double *) + nLoop;
            *(double*)pdValue=atof(sTemp);
            continue;
          }
          if(!strcmp(sTempFormat,"s"))
          {
            sValue = va_arg(pVarArgList,char *) + nLoop * nLenArray[col];
            strcpy(sValue,sTemp);
            continue;
          }
          fprintf(stderr,"Format String %s cannot be identified",sTempFormat);
          keyb(); 
          break;
      }
    }
    va_end(pVarArgList);
  }
  return nRealNum;
}

#endif

#ifdef OCI
long FetchCursorArray(long nCursorId, long* nLenArray, long nRecNum, char *sFormat,...)
{
  long nRetVal;
  long col;
  double *pdValue;
  long *pnValue;
  char *sValue;

  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char sTemp[8192];
  char sTempFormat[8192];/*临时格式*/

  va_list pVarArgList;/*变长参数列表指针*/

  int nLoop;
  long nRealNum = 0;

  for(nLoop = 0; nLoop <= nRecNum - 1; nLoop++)
  {
    if (ofetch(&cursor[nCursorId-1]))
    {
      if (cursor[nCursorId-1].rc == NO_DATA_FOUND)
        nRetVal=0;
      else
      {
        nSqlErrNo=cursor[nCursorId-1].rc;
        #ifdef TEST
        ShowTestErrMsg();
        #endif
        return -nSqlErrNo;
      }
    }
    else
      nRetVal=1;


    if(nRetVal == 0)
    {
      break;
    }

    nRealNum++;

    va_start(pVarArgList,sFormat);

    sTop=sFormat;

    for (col = 0; col < nColNum[nCursorId-1]; col++)
    {
      sTop=strchr(sTop,'%');
      if(sTop==NULL)
      {
        fprintf(stderr,"FETCH格式错误\\n");
        keyb();
        return -ERR_NO_FETCH;
      }
      sBottom=strchr(sTop+1,'%');
      if(sBottom)
      {
        strncpy(sTempFormat,sTop+1,sBottom-sTop-1);
        sTempFormat[sBottom-sTop-1]='\0';
        sTop=sBottom;
      }
      else
      {
        strcpy(sTempFormat,sTop+1);
        sTop=NULL;
      }
      alltrim(sTempFormat);
      if(sTempFormat[strlen(sTempFormat)-1]==',')
      {
        sTempFormat[strlen(sTempFormat)-1]='\0'; 
        alltrim(sTempFormat);
      }
  
      switch (desc[col].dbtype)
      {
        case FLOAT_TYPE:
          pdValue=va_arg(pVarArgList,double *) + nLoop;
          *(double*)pdValue=(double)def[col].flt_buf;
          break;
        case INT_TYPE:
          pnValue=va_arg(pVarArgList,long *) + nLoop;
          *(long*)pnValue=def[col].int_buf;
          break;
        default:
          if(!strcmp(sTempFormat,"ld"))
          {
            pnValue=va_arg(pVarArgList,long *) + nLoop;
            *(long*)pnValue=atol((char*)def[col].buf);
            continue;
          }
          if(!strcmp(sTempFormat,"lf"))
          {
            pdValue=va_arg(pVarArgList,double *) + nLoop;
            *(double*)pdValue=atof((char*)def[col].buf);
            continue;
          }
          if(!strcmp(sTempFormat,"s"))
          {
            sValue=va_arg(pVarArgList,char *) + nLoop * nLenArray[col];
            strcpy(sValue,(char*)def[col].buf);
            continue;
          }
          fprintf(stderr,"Format String %s cannot be identified",sTempFormat);
          keyb(); 
          break;
      }
    }

    va_end(pVarArgList);
  }
  return nRealNum;
}
#endif

#ifdef DATASERVER
int dsCloseCursor(int socket)
{
  long nRetVal;
  long nCursorId;
  
  if(GetLongFromSocket(socket,&nCursorId))
  {
    close(socket);
    return 1;
  }
  nRetVal=CloseCursor(nCursorId);
  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }
  return 0;
}
#endif

#ifdef DATACLIENT
long CloseCursor(long nCursorId)
{
  long nRetVal;

  if(nOciSocket==0)
    return -1012;

  if(SendLongToSocket(nOciSocket,(long)TASK_CLOSECURSOR))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(SendLongToSocket(nOciSocket,nCursorId))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  } 
  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  return nRetVal;
}
#endif

#ifdef OCI
long CloseCursor(long nCursorId)
{
  #ifdef TEST
  fprintf(stderr,"%s: CloseCursor: %ld\n",sDsUserName,nCursorId);
  #endif

  if(nCursorId<=0||nCursorId>MAX_CURSOR_NUM)
  {
    nSqlErrNo=1001;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }
  /* oclose() disconnects a cursor from the data areas 
     in the Oracle Server with which it is associated. */
  if (oclose(&cursor[nCursorId-1]))
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }
  sCursorFlag[nCursorId-1]='0';
  return 1;
}
#endif

#ifdef DATASERVER
int dsSetAutoCommitOn(int socket)
{
  long nRetVal;
  
  nRetVal=SetAutoCommitOn();
  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }
  return 0;
}
#endif

#ifdef DATACLIENT
long SetAutoCommitOn()
{
  long nRetVal;

  if(nOciSocket==0)
    return -1012;

  if(SendLongToSocket(nOciSocket,(long)TASK_SETAUTOCOMMITON))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  return nRetVal;
}
#endif

/* 打开自动提交开关,TRUE成功,FALSE失败 */

#ifdef OCI
long SetAutoCommitOn()
{
  #ifdef TEST
  fprintf(stderr,"%s: SetAutoCommitOn\n",sDsUserName);
  #endif

  /* ocon enables autocommit, that is, automatic commit of every SQL data 
     manipulation statement. */
  /* Turn on auto-commit. Default is off, however. */
  if (ocon(&lda))
  {
    nSqlErrNo=lda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return FALSE;
  }
  return TRUE;
}
#endif

#ifdef DATASERVER
int dsSetAutoCommitOff(int socket)
{
  long nRetVal;
  
  nRetVal=SetAutoCommitOff();
  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }
  return 0;
}
#endif

#ifdef DATACLIENT
long SetAutoCommitOff()
{
  long nRetVal;

  if(nOciSocket==0)
    return -1012;

  if(SendLongToSocket(nOciSocket,(long)TASK_SETAUTOCOMMITOFF))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  return nRetVal;
}
#endif

#ifdef OCI
/* 关闭自动提交开关,TRUE成功,FALSE失败 */
long SetAutoCommitOff()
{
  #ifdef TEST
  fprintf(stderr,"%s: SetAutoCommitOff\n",sDsUserName);
  #endif

  /* ocof disables autocommit, that is, automatic commit of every SQL data
     manipulation statement. */
  /* Turn off auto-commit. Default is off, however. */
  if (ocof(&lda))
  {
    nSqlErrNo=lda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return FALSE;
  }
  return TRUE;
}
#endif

#ifdef DATASERVER
int dsRunSelect(int socket)
{
  long nRetVal;
  long col;
  long nColNum;
  char sOut[8192];

  char sTemp[8192];
  char *s;
  char *s1;
  long n,i;
  int l;
  
  if(GetStringFromSocket(socket,sOut))
  {
    close(socket);
    return 1;
  }

  #ifdef TEST
  fprintf(stderr,"%s: RunSelect: %s\n",sDsUserName,sOut);
  #endif

  /* Parse the statement; do not defer the parse,
       so that errors come back right away. */
  if (oparse(&cda, (text *) sOut, (sb4) -1,
             (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
    nSqlErrNo=cda.rc;
    nRetVal=-nSqlErrNo;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    if(SendLongToSocket(socket,nRetVal))
    {
      close(socket);
      return 1;
    }
    return 0;
  }

  /* Save the SQL function code right after parse. */
  sql_function = cda.ft;

  if(sql_function!=FT_SELECT)
  {
    nSqlErrNo=ERR_NO_SELECT;
    nRetVal=-nSqlErrNo;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    if(SendLongToSocket(socket,nRetVal))
    {
      close(socket);
      return 1;
    }
    return 0;
  }

  /* describe and define all select-list items. */
  if((nColNum = describe_define(&cda)) <0)
  {
    nSqlErrNo=cda.rc;
    nRetVal=-nSqlErrNo;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    if(SendLongToSocket(socket,nRetVal))
    {
      close(socket);
      return 1;
    }
    return 0;
  }

  /* Execute the statement. */

  if (oexfet(&cda, (ub4) 1, FALSE, FALSE))
  {
      if (cda.rc == NO_DATA_FOUND)
      {
        nSqlErrNo=cda.rc;
        #ifdef TEST
        ShowTestErrMsg();
        #endif
        if(SendLongToSocket(socket,0L))
        {
          close(socket);
          return 1;
        }
        return 0;
      }
      else
      {
        nSqlErrNo=cda.rc;
        nRetVal=-nSqlErrNo;
        #ifdef TEST
        ShowTestErrMsg();
        #endif
        if(SendLongToSocket(socket,nRetVal))
        {
          close(socket);
          return 1;
        }
        return 0;
      }
  }

  if(SendLongToSocket(socket,1L))
  {
    close(socket);
    return 1;
  }

  if(GetLongFromSocket(socket,&n))
  {
    close(socket);
    return 1;
  }

  if(n==0)
    return 0;

  if(SendLongToSocket(socket,nColNum))
  {
    close(socket);
    return 1;
  }

  s=sTemp;
  l=0;

  for (col = 0; col < nColNum; col++)
  {
    switch (desc[col].dbtype)
    {
      case FLOAT_TYPE:
/*
        if(SendLongToSocket(socket,TYPE_DOUBLE))
        {
          close(socket);
          return 1;
        }

        if(SendDoubleToSocket(socket,(double)def[col].flt_buf))
        {
          close(socket);
          return 1;
        }
*/
        break;
      case INT_TYPE:
/*
        if(SendLongToSocket(socket,TYPE_LONG))
        {
          close(socket);
          return 1;
        }

        if(SendLongToSocket(socket,def[col].int_buf))
        {
          close(socket);
          return 1;
        }
*/
        break;
      default:
        n=(long)TYPE_STRING;
        #ifdef LITTLEEND
        n = ReverseLong1(n);
        #endif
        s1=(char*)&n;
        for(i=0;i<4;i++)
        {
          s[i]=s1[i];
        }
        s+=4;
        l+=4;

        n=(long)strlen((char*)def[col].buf);
        #ifdef LITTLEEND
        n = ReverseLong1(n);
        #endif
        s1=(char*)&n;
        for(i=0;i<4;i++)
        {
          s[i]=s1[i];
        }
        s+=4;
        l+=4;

        strcpy(s,(char*)def[col].buf);
        #ifdef LITTLEEND
        n = ReverseLong2(n);
        #endif
        s+=n;
        l+=n;
/*
        if(SendLongToSocket(socket,TYPE_STRING))
        {
          close(socket);
          return 1;
        }

        if(SendStringToSocket(socket,(char*)def[col].buf))
        {
          close(socket);
          return 1;
        }
*/
        break;
    }
  }
  if(SendRecordToSocket(socket,sTemp,l))
  {
    close(socket);
    return 1;
  }
  return 0;
}
#endif

#ifdef DATACLIENT
long RunSelect(char *sFormat,...)
{
  long nErrorFlag=0,/*语法错误标志,0为false,1为true*/
       nPrecision,/*精度为*号*/ 
       nPrecisionFlag,/*精度为*号标志,0为False,1为true*/
       nCharCount,/*返回实际输出 的字节数*/
       nType;/*变长列表中参数的类型*/
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char* s;
  char* sBreak;
  char sTempFormat[8192];/*临时格式*/
  char sTemp[8192];/*临时输出*/
  char sOut[8192];/*输出字符串*/
  va_list pVarArgList;/*变长参数列表指针*/

  long nColNum;
  long col;
  double *pdValue;
  long *pnValue;
  char *sValue;
  long nRetVal;

  if(nOciSocket==0) ConnectDatabase(sSysManager,sSysPassword);
  if(nOciSocket==0)  return -1012;

  sTop=sFormat;
  sBottom=sFormat;

  s=sFormat;
  sBreak=NULL;
  while(s[0]&&s[5])
  {
    if(!isalnum(s[0])&&s[0]!='_'
     &&toupper(s[1])=='I'&&toupper(s[2])=='N'
     &&toupper(s[3])=='T'&&toupper(s[4])=='O'
     &&!isalnum(s[5])&&s[5]!='_')
    {
      sBreak=s+1;
      break;
    }
    s++;
  }

  sOut[0]='\0';
  nType=TYPE_NULL;

  va_start(pVarArgList,sFormat);

  while(sTop&&sTop[0])
  {
    nType=0;
    nPrecisionFlag=0;
    sBottom=(char*)strchr(sTop,'%');
    if(sBottom==NULL||sBreak&&sBottom>sBreak)
    {
      nType=TYPE_NULL;
      sBottom=sBreak;
    }
    else
      sBottom++;

    while(nType==0)
    {
      switch(sBottom[0])
      {
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
          nType=TYPE_INT;
          break;
        case 'h':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'd':
              case 'i':
              case 'o':
              case 'x':
              case 'X':
              case 'u':
                nType=TYPE_SHORT;
                break;
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stderr,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'f':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
          nType=TYPE_FLOAT;
          break;
        case 'l':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'd':
              case 'i':
              case 'o':
              case 'x':
              case 'X':
              case 'u':
                nType=TYPE_LONG;
                break;
              case 'f':
              case 'e':
              case 'E':
              case 'g':
              case 'G':
                nType=TYPE_DOUBLE;
                break;  
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stdout,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'L':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'f':
              case 'e':
              case 'E':
              case 'g':
              case 'G':
                nType=TYPE_LONGDOUBLE;
                break;  
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stdout,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'c':
          nType=TYPE_CHAR;
          break;
        case 's':
          nType=TYPE_STRING; 
          break;
        case 't':
          nType=TYPE_TABLE; 
          break;
        case '*':
          nPrecisionFlag=1;
          nPrecision=va_arg(pVarArgList,long);
          break;
        case '%':
          sBottom++;
          sBottom=(char*)strchr(sBottom,'%');
          break;
        case '\0':
          nType=TYPE_NULL;
          break;
      }
      if (nType||nErrorFlag)
        break;
      if(sBottom!=NULL&&sBottom[0]!='\0')
        sBottom++;
    }

    if(nErrorFlag)
    {
      va_end(pVarArgList);
      return 0;
    }

    if(nType!=TYPE_NULL)
    {
      strncpy(sTempFormat,sTop,sBottom-sTop+1);
      sTempFormat[sBottom-sTop+1]='\0';
      sTop=sBottom+1;
      if(nType == TYPE_TABLE)
      {
        s = strstr(sTempFormat, "%t");
        s[1] = 's';
      }
    }
    else
    {
      if(sBreak)
      {
        strncpy(sTempFormat,sTop,sBreak-sTop);
        sTempFormat[sBreak-sTop]='\0';
      }
      else
        strcpy(sTempFormat,sTop);
      sTop=&sFormat[strlen(sFormat)];
    }

    switch(nType)
    { 
      case TYPE_INT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,int));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,int));
        break;
      case TYPE_LONG:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long));
        break;
      case TYPE_SHORT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,short));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,short));
        break;
      case TYPE_FLOAT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,float));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,float));
        break;
      case TYPE_DOUBLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,double));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,double));
        break;
      case TYPE_LONGDOUBLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long double));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long double));
        break;
      case TYPE_CHAR:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char));
        break;
      case TYPE_STRING:
        s=sTempFormat+strlen(sTempFormat)-1;
        *(s+3)='\0';
        *(s+2)='\'';
        while(*s!='%')
        {
          *(s+1)=*s;
          s--;
        }
        *(s+1)='%';
        *s='\'';
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
        break;
      case TYPE_TABLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
        break;
      case TYPE_NULL:
        strcpy(sTemp,sTempFormat);
        break;
    }
    strcat(sOut,sTemp);
  }
  strcpy(sSysSqlStatement,sOut); //lee3
  if(SendLongToSocket(nOciSocket,(long)TASK_RUNSELECT))
  {
    va_end(pVarArgList);
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(SendStringToSocket(nOciSocket,sOut))
  {
    va_end(pVarArgList);
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    va_end(pVarArgList);
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(nRetVal==1)
  {
    if(sBreak==NULL)
    {
      if(SendLongToSocket(nOciSocket,0L))
      {
        va_end(pVarArgList);
        nOciSocket=0;
        return -ERR_NO_TCPIP;
      }
    }
    else
    {
      if(SendLongToSocket(nOciSocket,1L))
      {
        va_end(pVarArgList);
        nOciSocket=0;
        return -ERR_NO_TCPIP;
      }

      if(GetLongFromSocket(nOciSocket,&nColNum))
      {
        va_end(pVarArgList);
        nOciSocket=0;
        return -ERR_NO_TCPIP;
      }

      sTop=sBreak;

      for (col = 0; col < nColNum; col++)
      {
        if(GetLongFromSocket(nOciSocket,&nType))
        {
          va_end(pVarArgList);
          nOciSocket=0;
          return -ERR_NO_TCPIP;
        }

        if(sTop==NULL)
        {
          fprintf(stderr,"SELECT格式错误\\n");
          keyb();
          return -ERR_NO_FETCH;
        }

        sTop=strchr(sTop,'%');
        if(sTop==NULL)
        {
          fprintf(stderr,"SELECT格式错误\\n");
          keyb();
          return -ERR_NO_FETCH;
        }
        sBottom=strchr(sTop+1,'%');
        if(sBottom)
        {
          strncpy(sTempFormat,sTop+1,sBottom-sTop-1);
          sTempFormat[sBottom-sTop-1]='\0';
          sTop=sBottom;
        }
        else
        {
          strcpy(sTempFormat,sTop+1);
          sTop=NULL;
        }
        alltrim(sTempFormat);
        if(sTempFormat[strlen(sTempFormat)-1]==',')
        {
          sTempFormat[strlen(sTempFormat)-1]='\0'; 
          alltrim(sTempFormat);
        }

        switch (nType)
        {
          case TYPE_DOUBLE:
            pdValue=va_arg(pVarArgList,double *);
            if(GetDoubleFromSocket(nOciSocket,pdValue))
            {
              va_end(pVarArgList);
              nOciSocket=0;
              return -ERR_NO_TCPIP;
            }
            break;
          case TYPE_LONG:
            pnValue=va_arg(pVarArgList,long *);
            if(GetLongFromSocket(nOciSocket,pnValue))
            {
              va_end(pVarArgList);
              nOciSocket=0;
              return -ERR_NO_TCPIP;
            }
            break;
          default:
            if(GetStringFromSocket(nOciSocket,sTemp))
            {
              va_end(pVarArgList);
              nOciSocket=0;
              return -ERR_NO_TCPIP;
            }
            if(!strcmp(sTempFormat,"ld"))
            {
              pnValue=va_arg(pVarArgList,long *);
              *(long*)pnValue=atol(sTemp);
              continue;
            }
            if(!strcmp(sTempFormat,"lf"))
            {
              pdValue=va_arg(pVarArgList,double *);
              *(double*)pdValue=atof(sTemp);
              continue;
            }
            if(!strcmp(sTempFormat,"s"))
            {
              sValue=va_arg(pVarArgList,char *);
              strcpy(sValue,sTemp);
              continue;
            }
            fprintf(stderr,"Format String %s cannot be identified",sTempFormat);
            keyb(); 
            break;
        }
      }
    }
  }
  va_end(pVarArgList);
  return nRetVal;
}

#endif

#ifdef OCI
long RunSelect(char *sFormat,...)
{
  long nErrorFlag=0,/*语法错误标志,0为false,1为true*/
       nPrecision,/*精度为*号*/ 
       nPrecisionFlag,/*精度为*号标志,0为False,1为true*/
       nCharCount,/*返回实际输出 的字节数*/
       nType;/*变长列表中参数的类型*/
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char* s;
  char* sBreak;
  char sTempFormat[8192];/*临时格式*/
  char sTemp[8192];/*临时输出*/
  char sOut[8192];/*输出字符串*/
  va_list pVarArgList;/*变长参数列表指针*/

  long nColNum;
  long col;
  double *pdValue;
  long *pnValue;
  char *sValue;

  sTop=sFormat;
  sBottom=sFormat;

  s=sFormat;
  sBreak=NULL;
  while(s[0]&&s[5])
  {
    if(!isalnum(s[0])&&s[0]!='_'
     &&toupper(s[1])=='I'&&toupper(s[2])=='N'
     &&toupper(s[3])=='T'&&toupper(s[4])=='O'
     &&!isalnum(s[5])&&s[5]!='_')
    {
      sBreak=s+1;
      break;
    }
    s++;
  }

  sOut[0]='\0';
  nType=TYPE_NULL;

  va_start(pVarArgList,sFormat);

  while(sTop&&sTop[0])
  {
    nType=0;
    nPrecisionFlag=0;
    sBottom=(char*)strchr(sTop,'%');
    if(sBottom==NULL||sBreak&&sBottom>sBreak)
    {
      nType=TYPE_NULL;
      sBottom=sBreak;
    }
    else
      sBottom++;

    while(nType==0)
    {
      switch(sBottom[0])
      {
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
          nType=TYPE_INT;
          break;
        case 'h':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'd':
              case 'i':
              case 'o':
              case 'x':
              case 'X':
              case 'u':
                nType=TYPE_SHORT;
                break;
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stderr,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'f':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
          nType=TYPE_FLOAT;
          break;
        case 'l':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'd':
              case 'i':
              case 'o':
              case 'x':
              case 'X':
              case 'u':
                nType=TYPE_LONG;
                break;
              case 'f':
              case 'e':
              case 'E':
              case 'g':
              case 'G':
                nType=TYPE_DOUBLE;
                break;  
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stdout,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'L':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'f':
              case 'e':
              case 'E':
              case 'g':
              case 'G':
                nType=TYPE_LONGDOUBLE;
                break;  
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stdout,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'c':
          nType=TYPE_CHAR;
          break;
        case 's':
          nType=TYPE_STRING; 
          break;
        case 't':
          nType=TYPE_TABLE; 
          break;
        case '*':
          nPrecisionFlag=1;
          nPrecision=va_arg(pVarArgList,long);
          break;
        case '%':
          sBottom++;
          sBottom=(char*)strchr(sBottom,'%');
          break;
        case '\0':
          nType=TYPE_NULL;
          break;
      }
      if (nType||nErrorFlag)
        break;
      if(sBottom!=NULL&&sBottom[0]!='\0')
        sBottom++;
    }

    if(nErrorFlag)
    {
      va_end(pVarArgList);
      return 0;
    }

    if(nType!=TYPE_NULL)
    {
      strncpy(sTempFormat,sTop,sBottom-sTop+1);
      sTempFormat[sBottom-sTop+1]='\0';
      sTop=sBottom+1;
      if(nType == TYPE_TABLE)
      {
        s = strstr(sTempFormat, "%t");
        s[1] = 's';
      }
    }
    else
    {
      if(sBreak)
      {
        strncpy(sTempFormat,sTop,sBreak-sTop);
        sTempFormat[sBreak-sTop]='\0';
      }
      else
        strcpy(sTempFormat,sTop);
      sTop=&sFormat[strlen(sFormat)];
    }

    switch(nType)
    { 
      case TYPE_INT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,int));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,int));
        break;
      case TYPE_LONG:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long));
        break;
      case TYPE_SHORT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,short));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,short));
        break;
      case TYPE_FLOAT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,float));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,float));
        break;
      case TYPE_DOUBLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,double));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,double));
        break;
      case TYPE_LONGDOUBLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long double));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long double));
        break;
      case TYPE_CHAR:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char));
        break;
      case TYPE_STRING:
        s=sTempFormat+strlen(sTempFormat)-1;
        *(s+3)='\0';
        *(s+2)='\'';
        while(*s!='%')
        {
          *(s+1)=*s;
          s--;
        }
        *(s+1)='%';
        *s='\'';
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
        break;
      case TYPE_TABLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
        break;
      case TYPE_NULL:
        strcpy(sTemp,sTempFormat);
        break;
    }
    strcat(sOut,sTemp);
  }

  #ifdef TEST
  fprintf(stderr,"%s: RunSelect: %s\n",sDsUserName,sOut);
  #endif
  
  /* Parse the statement; do not defer the parse,
       so that errors come back right away. */
  if (oparse(&cda, (text *) sOut, (sb4) -1,
             (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
     nSqlErrNo=cda.rc;
     va_end(pVarArgList);
     #ifdef TEST
     ShowTestErrMsg();
     #endif
     return -nSqlErrNo;
  }

  /* Save the SQL function code right after parse. */
  sql_function = cda.ft;

  if(sql_function!=FT_SELECT)
  {
    fprintf(stdout,"\n\a非SELECT语句\n");
    keyb();
    va_end(pVarArgList);
    return 0;
  }

  /* describe and define all select-list items. */
  if((nColNum = describe_define(&cda)) <0)
  {
    nSqlErrNo=cda.rc;
    va_end(pVarArgList);
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  /* Execute the statement. */

  if (oexfet(&cda, (ub4) 1, FALSE, FALSE))
  {
      if (cda.rc == NO_DATA_FOUND)
      {
        va_end(pVarArgList);
        return 0;
      }
      else
      {
        nSqlErrNo=cda.rc;
        va_end(pVarArgList);
        #ifdef TEST
        ShowTestErrMsg();
        #endif
        return -nSqlErrNo;
      }
  }

  if(sBreak)
  {
    sTop=sBreak;
    for (col = 0; col < nColNum; col++)
    {
      sTop=strchr(sTop,'%');
      if(sTop==NULL)
      {
        fprintf(stderr,"SELECT格式错误\\n");
        keyb();
        return -ERR_NO_FETCH;
      }
      sBottom=strchr(sTop+1,'%');
      if(sBottom)
      {
        strncpy(sTempFormat,sTop+1,sBottom-sTop-1);
        sTempFormat[sBottom-sTop-1]='\0';
        sTop=sBottom;
      }
      else
      {
        strcpy(sTempFormat,sTop+1);
        sTop=NULL;
      }
      alltrim(sTempFormat);
      if(sTempFormat[strlen(sTempFormat)-1]==',')
      {
        sTempFormat[strlen(sTempFormat)-1]='\0'; 
        alltrim(sTempFormat);
      }

      switch (desc[col].dbtype)
      {
        case FLOAT_TYPE:
          pdValue=va_arg(pVarArgList,double *);
          *(double*)pdValue=(double)def[col].flt_buf;
          break;
        case INT_TYPE:
          pnValue=va_arg(pVarArgList,long *);
          *(long*)pnValue=def[col].int_buf;
          break;
        default:
          if(!strcmp(sTempFormat,"ld"))
          {
            pnValue=va_arg(pVarArgList,long *);
            *(long*)pnValue=atol((char*)def[col].buf);
            continue;
          }
          if(!strcmp(sTempFormat,"lf"))
          {
            pdValue=va_arg(pVarArgList,double *);
            *(double*)pdValue=atof((char*)def[col].buf);
            continue;
          }
          if(!strcmp(sTempFormat,"s"))
          {
            sValue=va_arg(pVarArgList,char *);
            strcpy(sValue,(char*)def[col].buf);
            continue;
          }
          fprintf(stderr,"Format String %s cannot be identified",sTempFormat);
          keyb(); 
          break;
      }
    }
  }

  va_end(pVarArgList);

  return 1;
}
#endif

#ifdef DATASERVER
int dsRunProc(int socket)
{
  long nRetVal;
  long nOutputArgNum;
  char sOut[8192];
  char sTemp[8192];
  int i;
  
  if(GetStringFromSocket(socket,sOut))
  {
    close(socket);
    return 1;
  }

  #ifdef TEST
  fprintf(stderr,"%s: RunProc: %s\n",sDsUserName, sOut);
  #endif

  if(GetLongFromSocket(socket,&nOutputArgNum))
  {
    close(socket);
    return 1;
  }

  for(i=1;i<=nOutputArgNum;i++)
  {
    if(GetStringFromSocket(socket,(char *)&bind_values[i-1][0]))
    {
      close(socket);
      return 1;
    }
  }

  /* oparse() parses a SQL statement or a PL/SQL block and associates it 
     with a cursor. The parse can optionally be deferred. */
  /* Parse the statement; do not defer the parse,
     so that errors come back right away. */
  if (oparse(&cda, (text *) sOut, (sb4) -1,
       (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
    nSqlErrNo=cda.rc;
    nRetVal=-nSqlErrNo;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    if(SendLongToSocket(socket,nRetVal))
    {
      close(socket);
      return 1;
    }
    return 0;
  }
  for(i=1;i<=nOutputArgNum;i++)
  {
    sprintf(sTemp,"a%03ld",i);
    if (obndrv(&cda, (text*)sTemp, -1, (ub1 *)&bind_values[i-1][0], -1, 
      VARCHAR2_TYPE, -1, (sb2 *) 0, (text *) 0, -1, -1))
    {
      nSqlErrNo=cda.rc;
      nRetVal=-nSqlErrNo;
      #ifdef TEST
      ShowTestErrMsg();
      #endif
      if(SendLongToSocket(socket,nRetVal))
      {
        close(socket);
        return 1;
      }
      return 0;
    }
  }

  /* Save the SQL function code right after parse. */
  sql_function = cda.ft;

  /* Execute the statement. */
  if (oexec(&cda))
  {
    nSqlErrNo=cda.rc;
    nRetVal=-nSqlErrNo;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    if(SendLongToSocket(socket,nRetVal))
    {
      close(socket);
      return 1;
    }
    return 0;
  }
  if(SendLongToSocket(socket,1L))
  {
    close(socket);
    return 1;
  }

  for(i=1;i<=nOutputArgNum;i++)
  {
    if(SendStringToSocket(socket,(char *)&bind_values[i-1][0]))
    {
      close(socket);
      return 1;
    }
  }

  return 0; 
}
#endif

#ifdef DATACLIENT
long RunProc(char *sProcedureName,char *sInputArg,char *sOutputArg,...)
{
  long nErrorFlag=0,/*语法错误标志,0为false,1为true*/
       nPrecision,/*精度为*号*/ 
       nPrecisionFlag,/*精度为*号标志,0为False,1为true*/
       nCharCount,/*返回实际输出 的字节数*/
       nType;/*变长列表中参数的类型*/
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char* s;
  char sTempFormat[8192];/*临时格式*/
  char sTemp[8192];/*临时输出*/
  char sOut[8192];/*输出字符串*/

  long nInputFlag;/*0表示无输入参数，1表示有输入参数*/
  long nOutputFlag;/*0表示无输出参数，1表示有输出参数*/
  long nOutputArgNum;
  long i;
  long nRetVal;

  /* Declare an array of bind values. */
  char bind_values[MAX_BINDS][MAX_ITEM_BUFFER_SIZE];
  long nBindVarType[MAX_BINDS];
  char * pBindVar[MAX_BINDS];

  va_list pVarArgList;/*变长参数列表指针*/

  if(nOciSocket==0) ConnectDatabase(sSysManager,sSysPassword);

  if(nOciSocket==0)
    return -1012;

  if(sInputArg==NULL || sInputArg[0]=='\0')
    nInputFlag=0;
  else
  {
    strcpy(sTemp,sInputArg);
    alltrim(sTemp);
    if(sTemp[0]=='\0')
      nInputFlag=0;
    else
      nInputFlag=1;
  }
    
  if(sOutputArg==NULL || sOutputArg[0]=='\0')
    nOutputFlag=0;
  else
  {
    strcpy(sTemp,sOutputArg);
    alltrim(sTemp);
    if(sTemp[0]=='\0')
      nOutputFlag=0;
    else
      nOutputFlag=1;
  }

  if(nInputFlag == 0 && nOutputFlag == 0)
  {
    sprintf(sOut,"begin %s; end;",sProcedureName);
  }
  else
  {


  sprintf(sOut,"begin %s(",sProcedureName);
  nType=TYPE_NULL;
  
  va_start(pVarArgList,sOutputArg);

  if(nInputFlag)
  {
    sTop=sInputArg;
    sBottom=sInputArg;

    while(sTop&&sTop[0])
    { 
      nType=0;
      nPrecisionFlag=0;
      sBottom=(char*)strchr(sTop,'%');
      if(sBottom==NULL)
        nType=TYPE_NULL;
      else
        sBottom++;
      while(nType==0)
      {
        switch(sBottom[0])
        {
          case 'd':
          case 'i':
          case 'o':
          case 'u':
          case 'x':
          case 'X':
            nType=TYPE_INT;
            break;
          case 'h':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_SHORT;
                  break;
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stderr,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'f':
          case 'e':
          case 'E':
          case 'g':
          case 'G':
            nType=TYPE_FLOAT;
            break;
          case 'l':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_LONG;
                  break;
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_DOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'L':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_LONGDOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'c':
            nType=TYPE_CHAR;
            break;
          case 's':
            nType=TYPE_STRING; 
            break;
          case 't':
            nType=TYPE_TABLE; 
            break;
          case '*':
            nPrecisionFlag=1;
            nPrecision=va_arg(pVarArgList,long);
            break;
          case '%':
            sBottom++;
            sBottom=(char*)strchr(sBottom,'%');
            break;
          case '\0':
            nType=TYPE_NULL;
            break;
        }
        if (nType||nErrorFlag)
          break;
        if(sBottom!=NULL&&sBottom[0]!='\0')
          sBottom++;
      }

      if(nErrorFlag)
      {
        va_end(pVarArgList);
        return 0;
      }

      if(nType!=TYPE_NULL)
      {
        strncpy(sTempFormat,sTop,sBottom-sTop+1);
        sTempFormat[sBottom-sTop+1]='\0';
        sTop=sBottom+1;
        if(nType == TYPE_TABLE)
        {
          s = strstr(sTempFormat, "%t");
          s[1] = 's';
        }
      }
      else
      {
        strcpy(sTempFormat,sTop);
        sTop=&sInputArg[strlen(sInputArg)];
      }

      switch(nType)
      { 
        case TYPE_INT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,int));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,int));
          break;
        case TYPE_LONG:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long));
          break;
        case TYPE_SHORT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,short));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,short));
          break;
        case TYPE_FLOAT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,float));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,float));
          break;
        case TYPE_DOUBLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,double));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,double));
          break;
        case TYPE_LONGDOUBLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long double));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long double));
          break;
        case TYPE_CHAR:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char));
          break;
        case TYPE_STRING:
          s=sTempFormat+strlen(sTempFormat)-1;
          *(s+3)='\0';
          *(s+2)='\'';
          while(*s!='%')
          {
            *(s+1)=*s;
            s--;
          }
          *(s+1)='%';
          *s='\'';
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
          break;
        case TYPE_TABLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
          break;
        case TYPE_NULL:
          strcpy(sTemp,sTempFormat);
          break;
      }
      strcat(sOut,sTemp);
    }
    if(nOutputFlag)
      strcat(sOut,",");
  }

  nOutputArgNum=0;

  if(nOutputFlag)
  {
    sTop=sOutputArg;
    sBottom=sOutputArg;

    while(sTop&&sTop[0])
    { 
      nType=0;
      nPrecisionFlag=0;
      sBottom=(char*)strchr(sTop,'%');
      if(sBottom==NULL)
        nType=TYPE_NULL;
      else
        sBottom++;
      while(nType==0)
      {
        switch(sBottom[0])
        {
          case 'd':
          case 'i':
          case 'o':
          case 'u':
          case 'x':
          case 'X':
            nType=TYPE_INT;
            break;
          case 'h':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_SHORT;
                  break;
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stderr,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'f':
          case 'e':
          case 'E':
          case 'g':
          case 'G':
            nType=TYPE_FLOAT;
            break;
          case 'l':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_LONG;
                  break;
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_DOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'L':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_LONGDOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'c':
            nType=TYPE_CHAR;
            break;
          case 's':
            nType=TYPE_STRING; 
            break;
          case '*':
            nPrecisionFlag=1;
            nPrecision=va_arg(pVarArgList,long);
            break;
          case '%':
            sBottom++;
            sBottom=(char*)strchr(sBottom,'%');
            break;
          case '\0':
            nType=TYPE_NULL;
            break;
        }
        if (nType||nErrorFlag)
          break;
        if(sBottom!=NULL&&sBottom[0]!='\0')
          sBottom++;
      }

      if(nErrorFlag)
      {
        va_end(pVarArgList);
        return 0;
      }

      if(nType!=TYPE_NULL)
      {
        nOutputArgNum++;
        nBindVarType[nOutputArgNum-1]=nType;
        if(nOutputArgNum==1)
          sprintf(sTemp,":a%03ld",nOutputArgNum);
        else
          sprintf(sTemp,",:a%03ld",nOutputArgNum);
        strcat(sOut,sTemp);
        sTop=sBottom+1;
      }
      else
        sTop=&sOutputArg[strlen(sOutputArg)];

      switch(nType)
      { 
        case TYPE_INT:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,int*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%d",*(int*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*d",nPrecision,*(int*)pBindVar);
          break;
        case TYPE_LONG:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,long*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%ld",*(long*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*ld",nPrecision,*(long*)pBindVar);
          break;
        case TYPE_SHORT:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,short*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%hd",*(short*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*hd",nPrecision,*pBindVar);
          break;
        case TYPE_FLOAT:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,float*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%f",*(float*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*f",nPrecision,*(float*)pBindVar);
          break;
        case TYPE_DOUBLE:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,double*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%lf",*(double*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%lf",nPrecision,*(double*)pBindVar);
          break;
        case TYPE_LONGDOUBLE:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,long double*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%Lf",*(long double*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*Lf",nPrecision,*(long double*)pBindVar);
          break;
        case TYPE_CHAR:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,char*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%c",*pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*c",nPrecision,*pBindVar);
          break;
        case TYPE_STRING:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,char**);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%s",*pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*s",nPrecision,*pBindVar);
          break;
        case TYPE_NULL:
          break;
      }
    }
  }
  strcat(sOut,"); end;");
  va_end(pVarArgList);


  }

  strcpy(sSysSqlStatement,sOut);  //lee1
  if(SendLongToSocket(nOciSocket,(long)TASK_RUNPROC))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(SendStringToSocket(nOciSocket,sOut))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  if(SendLongToSocket(nOciSocket,nOutputArgNum))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }
  for(i=1;i<=nOutputArgNum;i++)
  {
    if(SendStringToSocket(nOciSocket,bind_values[nOutputArgNum-1]))
    {
      nOciSocket=0;
      return -ERR_NO_TCPIP;
    }
  }
  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(nRetVal>0)
  {
    for(i=1;i<=nOutputArgNum;i++)
    {
      if(GetStringFromSocket(nOciSocket,bind_values[i-1]))
      {
        nOciSocket=0;
        return -ERR_NO_TCPIP;
      }

      switch(nBindVarType[i-1])
      { 
        case TYPE_INT:
          *(int*)pBindVar[i-1]=atoi((char*)bind_values[i-1]);
          break;
        case TYPE_LONG:
          *(long*)pBindVar[i-1]=atol((char*)bind_values[i-1]);
          break;
        case TYPE_SHORT:
          break;
        case TYPE_FLOAT:
          break;
        case TYPE_DOUBLE:
          *(double*)pBindVar[i-1]=atof((char*)bind_values[i-1]);
          break;
        case TYPE_LONGDOUBLE:
          break;
        case TYPE_CHAR:
          break;
        case TYPE_STRING:
          strcpy((char*)pBindVar[i-1],(char*)bind_values[i-1]);
          break;
        case TYPE_NULL:
          break;
      }
    }
    return 1;
  }

  return nRetVal;
}
#endif

#ifdef OCI
long RunProc(char *sProcedureName,char *sInputArg,char *sOutputArg,...)
{
  long nErrorFlag=0,/*语法错误标志,0为false,1为true*/
       nPrecision,/*精度为*号*/ 
       nPrecisionFlag,/*精度为*号标志,0为False,1为true*/
       nCharCount,/*返回实际输出 的字节数*/
       nType;/*变长列表中参数的类型*/
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char* s;
  char sTempFormat[8192];/*临时格式*/
  char sTemp[8192];/*临时输出*/
  char sOut[8192];/*输出字符串*/

  long nInputFlag;/*0表示无输入参数，1表示有输入参数*/
  long nOutputFlag;/*0表示无输出参数，1表示有输出参数*/
  long nOutputArgNum;
  long i;

  va_list pVarArgList;/*变长参数列表指针*/

  if(sInputArg==NULL || sInputArg[0]=='\0')
    nInputFlag=0;
  else
  {
    strcpy(sTemp,sInputArg);
    alltrim(sTemp);
    if(sTemp[0]=='\0')
      nInputFlag=0;
    else
      nInputFlag=1;
  }
    
  if(sOutputArg==NULL || sOutputArg[0]=='\0')
    nOutputFlag=0;
  else
  {
    strcpy(sTemp,sOutputArg);
    alltrim(sTemp);
    if(sTemp[0]=='\0')
      nOutputFlag=0;
    else
      nOutputFlag=1;
  }

  if(nInputFlag == 0 && nOutputFlag == 0)
  {
    sprintf(sOut,"begin %s; end;",sProcedureName);
  }
  else
  {


  sprintf(sOut,"begin %s(",sProcedureName);
  nType=TYPE_NULL;
  
  va_start(pVarArgList,sOutputArg);

  if(nInputFlag)
  {
    sTop=sInputArg;
    sBottom=sInputArg;

    while(sTop&&sTop[0])
    { 
      nType=0;
      nPrecisionFlag=0;
      sBottom=(char*)strchr(sTop,'%');
      if(sBottom==NULL)
        nType=TYPE_NULL;
      else
        sBottom++;
      while(nType==0)
      {
        switch(sBottom[0])
        {
          case 'd':
          case 'i':
          case 'o':
          case 'u':
          case 'x':
          case 'X':
            nType=TYPE_INT;
            break;
          case 'h':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_SHORT;
                  break;
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stderr,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'f':
          case 'e':
          case 'E':
          case 'g':
          case 'G':
            nType=TYPE_FLOAT;
            break;
          case 'l':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_LONG;
                  break;
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_DOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'L':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_LONGDOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'c':
            nType=TYPE_CHAR;
            break;
          case 's':
            nType=TYPE_STRING; 
            break;
          case 't':
            nType=TYPE_TABLE; 
            break;
          case '*':
            nPrecisionFlag=1;
            nPrecision=va_arg(pVarArgList,long);
            break;
          case '%':
            sBottom++;
            sBottom=(char*)strchr(sBottom,'%');
            break;
          case '\0':
            nType=TYPE_NULL;
            break;
        }
        if (nType||nErrorFlag)
          break;
        if(sBottom!=NULL&&sBottom[0]!='\0')
          sBottom++;
      }

      if(nErrorFlag)
      {
        va_end(pVarArgList);
        return 0;
      }

      if(nType!=TYPE_NULL)
      {
        strncpy(sTempFormat,sTop,sBottom-sTop+1);
        sTempFormat[sBottom-sTop+1]='\0';
        sTop=sBottom+1;
        if(nType == TYPE_TABLE)
        {
          s = strstr(sTempFormat, "%t");
          s[1] = 's';
        }
      }
      else
      {
        strcpy(sTempFormat,sTop);
        sTop=&sInputArg[strlen(sInputArg)];
      }

      switch(nType)
      { 
        case TYPE_INT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,int));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,int));
          break;
        case TYPE_LONG:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long));
          break;
        case TYPE_SHORT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,short));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,short));
          break;
        case TYPE_FLOAT:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,float));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,float));
          break;
        case TYPE_DOUBLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,double));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,double));
          break;
        case TYPE_LONGDOUBLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long double));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long double));
          break;
        case TYPE_CHAR:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char));
          break;
        case TYPE_STRING:
          s=sTempFormat+strlen(sTempFormat)-1;
          *(s+3)='\0';
          *(s+2)='\'';
          while(*s!='%')
          {
            *(s+1)=*s;
            s--;
          }
          *(s+1)='%';
          *s='\'';
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
          break;
        case TYPE_TABLE:
          if(nPrecisionFlag==0)
            sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
          else
            sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
          break;
        case TYPE_NULL:
          strcpy(sTemp,sTempFormat);
          break;
      }
      strcat(sOut,sTemp);
    }
    if(nOutputFlag)
      strcat(sOut,",");
  }

  nOutputArgNum=0;

  if(nOutputFlag)
  {
    sTop=sOutputArg;
    sBottom=sOutputArg;

    while(sTop&&sTop[0])
    { 
      nType=0;
      nPrecisionFlag=0;
      sBottom=(char*)strchr(sTop,'%');
      if(sBottom==NULL)
        nType=TYPE_NULL;
      else
        sBottom++;
      while(nType==0)
      {
        switch(sBottom[0])
        {
          case 'd':
          case 'i':
          case 'o':
          case 'u':
          case 'x':
          case 'X':
            nType=TYPE_INT;
            break;
          case 'h':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_SHORT;
                  break;
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stderr,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'f':
          case 'e':
          case 'E':
          case 'g':
          case 'G':
            nType=TYPE_FLOAT;
            break;
          case 'l':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'd':
                case 'i':
                case 'o':
                case 'x':
                case 'X':
                case 'u':
                  nType=TYPE_LONG;
                  break;
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_DOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'L':
            sBottom++;
            while(1)
            {
              switch(sBottom[0])
              {
                case 'f':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                  nType=TYPE_LONGDOUBLE;
                  break;  
                case '\0':
                  nType=TYPE_NULL;
                  break;
                default:
                  fprintf(stdout,"\n\a语法错误\n");
                  nErrorFlag=1;
                  keyb();
                  break;
              }
              if (nType)
                break;
              if(sBottom[0]!='\0')
                sBottom++;
            }
            break;
          case 'c':
            nType=TYPE_CHAR;
            break;
          case 's':
            nType=TYPE_STRING; 
            break;
          case '*':
            nPrecisionFlag=1;
            nPrecision=va_arg(pVarArgList,long);
            break;
          case '%':
            sBottom++;
            sBottom=(char*)strchr(sBottom,'%');
            break;
          case '\0':
            nType=TYPE_NULL;
            break;
        }
        if (nType||nErrorFlag)
          break;
        if(sBottom!=NULL&&sBottom[0]!='\0')
          sBottom++;
      }

      if(nErrorFlag)
      {
        va_end(pVarArgList);
        return 0;
      }

      if(nType!=TYPE_NULL)
      {
        nOutputArgNum++;
        nBindVarType[nOutputArgNum-1]=nType;
        if(nOutputArgNum==1)
          sprintf(sTemp,":a%03ld",nOutputArgNum);
        else
          sprintf(sTemp,",:a%03ld",nOutputArgNum);
        strcat(sOut,sTemp);
        sTop=sBottom+1;
      }
      else
        sTop=&sOutputArg[strlen(sOutputArg)];

      switch(nType)
      { 
        case TYPE_INT:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,int*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%d",*(int*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*d",nPrecision,*(int*)pBindVar);
          break;
        case TYPE_LONG:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,long*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%ld",*(long*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*ld",nPrecision,*(long*)pBindVar);
          break;
        case TYPE_SHORT:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,short*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%hd",*(short*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*hd",nPrecision,*pBindVar);
          break;
        case TYPE_FLOAT:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,float*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%f",*(float*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*f",nPrecision,*(float*)pBindVar);
          break;
        case TYPE_DOUBLE:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,double*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%lf",*(double*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%lf",nPrecision,*(double*)pBindVar);
          break;
        case TYPE_LONGDOUBLE:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,long double*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%Lf",*(long double*)pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*Lf",nPrecision,*(long double*)pBindVar);
          break;
        case TYPE_CHAR:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,char*);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%c",*pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*c",nPrecision,*pBindVar);
          break;
        case TYPE_STRING:
          pBindVar[nOutputArgNum-1]=(void*)va_arg(pVarArgList,char**);
          if(nPrecisionFlag==0)
            sprintf((char*)bind_values[nOutputArgNum-1],"%s",*pBindVar);
          else
            sprintf((char*)bind_values[nOutputArgNum-1],"%*s",nPrecision,*pBindVar);
          break;
        case TYPE_NULL:
          break;
      }
    }
  }
  strcat(sOut,"); end;");
  va_end(pVarArgList);


  }

  #ifdef TEST
  fprintf(stderr,"%s: RunProc: %s\n",sDsUserName, sOut);
  #endif

  /* oparse() parses a SQL statement or a PL/SQL block and associates it 
     with a cursor. The parse can optionally be deferred. */
  /* Parse the statement; do not defer the parse,
     so that errors come back right away. */
  if (oparse(&cda, (text *) sOut, (sb4) -1,
       (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
    nSqlErrNo=cda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }
  for(i=1;i<=nOutputArgNum;i++)
  {
    sprintf(sTemp,"a%03ld",i);
    if (obndrv(&cda, (text*)sTemp, -1, (ub1 *)&bind_values[i-1][0], -1, 
      VARCHAR2_TYPE, -1, (sb2 *) 0, (text *) 0, -1, -1))
    {
      nSqlErrNo=cda.rc;
      #ifdef TEST
      ShowTestErrMsg();
      #endif
      return -nSqlErrNo;
    }
  }

  /* Save the SQL function code right after parse. */
  sql_function = cda.ft;

  /* Execute the statement. */
  if (oexec(&cda))
  {
    nSqlErrNo=cda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    return -nSqlErrNo;
  }

  for(i=1;i<=nOutputArgNum;i++)
  {
    switch(nBindVarType[i-1])
    { 
      case TYPE_INT:
        *(int*)pBindVar[i-1]=atoi((char*)bind_values[i-1]);
        break;
      case TYPE_LONG:
        *(long*)pBindVar[i-1]=atol((char*)bind_values[i-1]);
        break;
      case TYPE_SHORT:
        break;
      case TYPE_FLOAT:
        break;
      case TYPE_DOUBLE:
        *(double*)pBindVar[i-1]=atof((char*)bind_values[i-1]);
        break;
      case TYPE_LONGDOUBLE:
        break;
      case TYPE_CHAR:
        break;
      case TYPE_STRING:
        strcpy((char*)pBindVar[i-1],(char*)bind_values[i-1]);
        break;
      case TYPE_NULL:
        break;
    }
  }

  return 1; 
}
#endif

#ifdef TEST
void ShowTestErrMsg()
{
  char sErrMsg[MAX_ERR_MSG_LEN];

  if(nSqlErrNo==ERR_NO_TCPIP)
    strcpy(sErrMsg,sTcpipErrInfo);
  if(nSqlErrNo==ERR_NO_SELECT)
    strcpy(sErrMsg,"not SELECT statement");

  /* oerhms() returns the text of an Oracle error message, 
     given the error code rcode. */
  oerhms(&lda,nSqlErrNo,(text *)sErrMsg,(sword)MAX_ERR_MSG_LEN);

  fprintf(stderr,"%s\n",sErrMsg);
}
#endif

#ifdef DATASERVER
int dsSqlplus(int socket)
{
  long nRetVal;
  char sOut[8192];
  char sTemp[8192];
  long nColNum;
  char *s;
  long col, n;

  if(GetStringFromSocket(socket,sOut))
  {
    close(socket);
    return 1;
  }

  #ifdef TEST
  fprintf(stderr,"%s: Sqlplus: %s\n",sDsUserName, sOut);
  #endif

  /* oparse() parses a SQL statement or a PL/SQL block and associates it 
     with a cursor. The parse can optionally be deferred. */
  /* Parse the statement; do not defer the parse,
     so that errors come back right away. */
  if (oparse(&cda, (text *) sOut, (sb4) -1,
       (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    nSqlErrNo=cda.rc;
    nRetVal=-nSqlErrNo;
    if(SendLongToSocket(socket,nRetVal))
    {
      close(socket);
      return 1;
    }
    return 0;
  }

  /* Save the SQL function code right after parse. */
  sql_function = cda.ft;

  /* If the statement is a query, describe and define
     all select-list items before doing the oexec. */
  if (sql_function == FT_SELECT)
  {
    if((nColNum = describe_define(&cda)) <0)
    {
      nSqlErrNo=cda.rc;
      nRetVal=-nSqlErrNo;
      #ifdef TEST
      ShowTestErrMsg();
      #endif
      if(SendLongToSocket(socket,nRetVal))
      {
        close(socket);
        return 1;
      }
      return 0;
    }
  }

  /* Execute the statement. */
  if (oexec(&cda))
  {
    nSqlErrNo=cda.rc;
    #ifdef TEST
    ShowTestErrMsg();
    #endif
    nRetVal=-nSqlErrNo;
    if(SendLongToSocket(socket,nRetVal))
    {
      close(socket);
      return 1;
    }
    return 0;
  }

  switch(sql_function)
  {
    case FT_UPDATE:
    case FT_DELETE:
    case FT_INSERT:
      if(SendLongToSocket(socket,1L))
      {
        close(socket);
        return 1;
      }
      break;
    case FT_SELECT:
      if(SendLongToSocket(socket,2L))
      {
        close(socket);
        return 1;
      }
      break;
    default:
      if(SendLongToSocket(socket,3L))
      {
        close(socket);
        return 1;
      }
      break;
  } 

  if(sql_function==FT_SELECT)
  {

    if(SendLongToSocket(socket,nColNum))
    {
      close(socket);
      return 1;
    }

    sTemp[0]='\0';
    for (col = 0; col < nColNum; col++)
    {
      n = desc[col].dbsize - desc[col].buflen;
      if (desc[col].dbtype == FLOAT_TYPE ||
            desc[col].dbtype == INT_TYPE)
      {
        sprintf(sTemp,"%s%*c",sTemp, n, ' ');
        sprintf(sTemp,"%s%*.*s",sTemp, desc[col].buflen,
          desc[col].buflen, desc[col].buf);
      }
      else
      {
        sprintf(sTemp,"%s%*.*s", sTemp,desc[col].buflen,
                   desc[col].buflen, desc[col].buf);
        sprintf(sTemp,"%s%*c", sTemp,n, ' ');
      }
      if(col<nColNum-1)
        strcat(sTemp," ");
    }            

    if(SendStringToSocket(socket,sTemp))
    {
      close(socket);
      return 1;
    }

    sTemp[0]='\0';
    s=sTemp;

    for (col = 0; col < nColNum; col++)
    {
      memset(s,'-',desc[col].dbsize);
      s+=desc[col].dbsize;
      s[0]=' ';
      s++;
    }
    s[0]='\0';

    if(SendStringToSocket(socket,sTemp))
    {
      close(socket);
      return 1;
    }

    for (;;)
    {
      /* Fetch a row.  Break on end of fetch,
         disregard null fetch "error". */
      if (ofetch(&cda))
      {
        if (cda.rc == NO_DATA_FOUND)
        {
          if(SendLongToSocket(socket,0L))
          {
            close(socket);
            return 1;
          }
          break;
        }
        if (cda.rc != NULL_VALUE_RETURNED)
        {
          nSqlErrNo=cda.rc;
          #ifdef TEST
          ShowTestErrMsg();
          #endif

          if(SendLongToSocket(socket,0L))
          {
            close(socket);
            return 1;
          }
          break;
        }
      }
      if(SendLongToSocket(socket,1L))
      {
        close(socket);
        return 1;
      }

      for (col = 0; col < nColNum ; col++)
      {
        /* Check col. return code for null.  If
           null, print n spaces, else print value. */
        if (def[col].indp < 0)
        {
          sprintf(sTemp,"%*c", desc[col].dbsize, ' ');
        }
        else
        {
          switch (desc[col].dbtype)
          {
            case FLOAT_TYPE:
              sprintf(sTemp,"%*.*lf", numwidth, 2, (double)def[col].flt_buf);
              break;
            case INT_TYPE:
              sprintf(sTemp,"%*ld", numwidth, def[col].int_buf);
              break;
            default:
              strcpy(sTemp, (char*)def[col].buf);
              n = desc[col].dbsize - strlen((char *) def[col].buf);
              if (n > 0)
                sprintf(sTemp,"%s%*c",sTemp,n, ' ');
              break;
          }
        }
        if(col<nColNum-1)
          strcat(sTemp," ");

        if(SendStringToSocket(socket,sTemp))
        {
          close(socket);
          return 1;
        }
      }
    }  /* end for (;;) */
  }

  if(sql_function==FT_SELECT||sql_function==FT_UPDATE||sql_function==FT_DELETE||sql_function==FT_INSERT)
    nRetVal=cda.rpc;
  else
    nRetVal=1; 

  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }
  
  return 0;
}
#endif

#ifdef DATACLIENT
long sqlplus(char *sOut)
{
  long nErrorFlag=0,/*语法错误标志,0为false,1为true*/
       nType;/*变长列表中参数的类型*/
  char sTempFormat[8192];/*临时格式*/
  char sTemp[8192];/*临时输出*/
  long nRetVal;
  long nSqlType;
  /*1 insert update delete
    2 select
    3 其它*/
  long nColNum;
  long col, n;

  if(nOciSocket==0) ConnectDatabase(sSysManager,sSysPassword);

  if(nOciSocket==0)
    return -1012;

  if(SendLongToSocket(nOciSocket,(long)TASK_SQLPLUS))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(SendStringToSocket(nOciSocket,sOut))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(GetLongFromSocket(nOciSocket,&nSqlType))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  if(nSqlType==2)
  {

    if(GetLongFromSocket(nOciSocket,&nColNum))
    {
      nOciSocket=0;
      return -ERR_NO_TCPIP;
    }

    putchar('\n');

    if(GetStringFromSocket(nOciSocket,sTemp))
    {
      nOciSocket=0;
      return -ERR_NO_TCPIP;
    }
    printf(sTemp);

    putchar('\n');

    if(GetStringFromSocket(nOciSocket,sTemp))
    {
      nOciSocket=0;
      return -ERR_NO_TCPIP;
    }
    printf(sTemp);

    putchar('\n');
  
    for (;;)
    {
      putchar('\n');

      if(GetLongFromSocket(nOciSocket,&n))
      {
        nOciSocket=0;
        return -ERR_NO_TCPIP;
      }

      if(n==0)
        break;

      for (col = 0; col < nColNum ; col++)
      {
        if(GetStringFromSocket(nOciSocket,sTemp))
        {
          nOciSocket=0;
          return -ERR_NO_TCPIP;
        }
        printf(sTemp);
      }
    }  /* end for (;;) */
  }

  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
    nOciSocket=0;
    return -ERR_NO_TCPIP;
  }

  /* Print the rows-processed count. */
  if (nSqlType==1||nSqlType==2)
  {
    if(nChnFlag==1)
      printf("\n%ld行被处理.\n", nRetVal);
    else
      printf("\n%ld row%c processed.\n", nRetVal,nRetVal == 1 ? '\0' : 's');
  }
  else
    printf(nChnFlag==1?"\n语句被处理.\n":"\nStatement processed.\n");
  return nRetVal;
} 
#endif


#ifdef DATACLIENT
int ReturnSocket()
{
    return nOciSocket;
}

#endif





