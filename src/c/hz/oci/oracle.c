/*        
 *                            oracle.c
 *
 *                  Oracle Connection Interface (Server)
 *
 *                       (for Oracle 7.x 8.x)
 *                           
 *   (c)Copyright by Chenbo 2001. 
 *
 *   Original Programmer : chenbo[陈博]
 *
 *   Modify Record:
 *
 *   DATE   NAME         REASON      
 */


#include"oracle.h"


//global variable declare

static Lda_Def lda;
static Cda_Def cda;
static Cda_Def cursor[MAX_CURSOR_NUM];
static char    sCursorFlag[MAX_CURSOR_NUM+1];
static long    nColNum[MAX_CURSOR_NUM];
static ub4     hda[HDA_SIZE/(sizeof(ub4))];
static sword   sql_function;
static sword   numwidth = 14;
static struct  describe desc[MAX_SELECT_LIST_SIZE];
static struct  define   def[MAX_SELECT_LIST_SIZE];
static long    nSqlErrNo;
static char    sDsUserName[MAX_USERNAME_LEN+1];
extern char    sTcpipErrInfo[81];


/* 
 * 函数：ConnectDatabase
 * 功能：连接数据库
 * 参数：char * usermame:用户名
 *       char * password:口令
 * 返回：>=0:成功，否由失败
 * 说明：
 */

long ConnectDatabase(char *username,char *password)
{
  long retcode;

  nSqlErrNo=0L;

  DBG("ConnectDatabase %s",username);
  strcpy(sDsUserName,username);
  
  memset(sCursorFlag,'0',MAX_CURSOR_NUM);
  retcode = olog(&lda, (ub1 *)hda, (text *)username, -1, (text *)password, -1, 
                 (text *)0, -1, (ub4) OCI_LM_DEF);

  if(retcode==0)
     retcode = oopen(&cda, &lda, (text *) 0, -1, -1, (text *) 0, -1);

  if(retcode!=0)
  {
      nSqlErrNo=cda.rc;
      ShowErrMsg();
      return -nSqlErrNo;
  }
  return 0;
}

/* 
 * 函数：CloseDatabase
 * 功能：关闭数据库连接
 * 参数：
 * 返回：
 * 说明：
 */
long CloseDatabase()
{
  long nCode,cur;


  DBG("%s: CloseDatabase",sDsUserName);

  nCode=oclose(&cda);                                         /* close cursor */

  if(nCode==0)
  {
    for(cur=0;cur<MAX_CURSOR_NUM;cur++)
      if(sCursorFlag[cur]=='1') oclose(&cursor[cur]);

    nCode=ologof(&lda);                               /* log off the database */
  }
  
  if(nCode!=0)
  {
    nSqlErrNo=cda.rc;
    ShowErrMsg();
    return -nSqlErrNo;
  }
  return 0;
}

/* 
 * 函数：
 * 功能：得到最近一次的错误代码
 * 参数：
 * 返回：错误代码
 * 说明：
 */
long GetSqlErrNo()
{
  DBG("%s: GetSqlErrNo: %ld\n",sDsUserName,-nSqlErrNo);
  return -nSqlErrNo;
}


/* 
 * 函数：
 * 功能：得到最近一次的错误信息
 * 参数：char * sErrMsg:错误回带字符串
 * 返回：
 * 说明：
 */
long GetSqlErrInfo(char *sErrMsg)
{
  long nCharCount;

  if(nSqlErrNo==ERR_NO_TCPIP)
  {
    DBG("%s: GetSqlErrInfo: %s",sDsUserName,sTcpipErrInfo);
    strcpy(sErrMsg,sTcpipErrInfo);
    return strlen(sErrMsg);
  }

  if(nSqlErrNo==ERR_NO_SELECT)
  {
    DBG("%s: GetSqlErrInfo: not SELECT statement",sDsUserName);
    strcpy(sErrMsg,"非SELECT语句");
    return strlen(sErrMsg);
  }

  nCharCount=oerhms(&lda,nSqlErrNo,(text *)sErrMsg,(sword)MAX_ERR_MSG_LEN);
  DBG("%s: GetSqlErrInfo: %s",sDsUserName,sErrMsg);
  return nCharCount;
}

/* 
 * 函数：CommitWork
 * 功能：数据库提交
 * 参数：无
 * 返回：0:成功，否则失败
 * 说明：
 */
long CommitWork()
{
  DBG("%s: CommitWork",sDsUserName);
  if(ocom(&lda)==0)  return 0;
  nSqlErrNo=lda.rc;
  ShowErrMsg();
  return -nSqlErrNo;
}


/* 
 * 函数：RollbackWork
 * 功能：数据库回滚
 * 参数：无
 * 返回：0:成功，否则失败
 * 说明：
 */
long RollbackWork()
{
  DBG("%s: RollbackWork",sDsUserName);
  if(orol(&lda)==0) return 0;
  nSqlErrNo=lda.rc;
  ShowErrMsg();
  return -nSqlErrNo;
}


/* 
 * 函数：RunSql
 * 功能：执行一条SQL数据操作语句
 * 参数：char *format:SQL数据操作语句
 *       ...:本条SQL语句所使用的参数
 * 返回：>0:成功  <0:失败   =0:没找到数据
 * 说明：
 */
long RunSql(char * statement,...)
{
  char sql[MAX_SQL_SIZE];
  va_list pvar;
  long nRetVal;

  memset(sql,0,sizeof(sql));
  va_start(pvar,statement);
  nRetVal = FormSqlWithArgs(statement,pvar,sql);
  va_end(pvar);
  
  if(nRetVal<0) return nRetVal;

  return RunSqlStmt(sql);
  
}


/* 
 * 函数：OpenCursor
 * 功能：打开一个游标数据区
 * 参数：char * statement:SELECT语句
 * 返回：>0:成功，游标ID   <0:失败   =0:没找到数据
 * 说明：
 */
long OpenCursor(char *statement,...)
{
  
  char sql[MAX_SQL_SIZE];
  va_list pvar;
  long nRetVal;
  
  memset(sql,0,sizeof(sql));
  va_start(pvar,statement);
  nRetVal = FormSqlWithArgs(statement,pvar,sql);
  va_end(pvar);
  
  if(nRetVal<0) return nRetVal;
  
  return OpenCursorStmt(sql);

}

/* 
 * 函数：FetchCursor
 * 功能：从已打开的游标中取得数据
 * 参数：long nCursorId:游标ID
 *       char * format: 宿主变量数据格式
 * 返回：>0:成功  <0:失败  =0:数据已取完
 * 说明：
 */
long FetchCursor(long nCursorId,char * format,...)
{
  long nRetVal;
  long col,field;
  va_list pvar;
  long args;
  char * offset;
  
  
  if (ofetch(&cursor[nCursorId-1]))
  {
    if (cursor[nCursorId-1].rc == NO_DATA_FOUND) return 0;
    else
    {
      nSqlErrNo=cursor[nCursorId-1].rc;
      ShowErrMsg();
      return -nSqlErrNo;
    }
  }

  offset = format;
  va_start(pvar,format);
  field = nColNum[nCursorId-1];

  for (col = 0; col < field; col++)
  {
      offset=strchr(offset,'%');
      if(!offset)
      {
      	 DBG("打开字段的个数与取游标的数据个数不一致");
      	 va_end(pvar);
      	 return -2;
      }
      offset++;
      args=va_arg(pvar,long);
      
      if(offset[0]=='l' && offset[1]=='f')
      {
         (*(double*)args)=atof((char*)def[col].buf);
      }
      else if(offset[0]=='l' && offset[1]=='d')
      {
         (*(long*)args)=atol((char*)def[col].buf);
      }   
      else if(offset[0]=='s')
      {
         strcpy((char*)args,(char*)def[col].buf);
      }   
      else
      {  
      	 DBG("无法鉴别的数据类型格式");
      	 va_end(pvar);
      	 return -3;
      }	 
  }
  va_end(pvar);
  return 1;
}

/* 
 * 函数：CloseCursor
 * 功能：关闭一个已找开的游标
 * 参数：long nCursorId:游标ID
 * 返回：1：成功，否则失败
 * 说明：
 */
long CloseCursor(long nCursorId)
{
  
  DBG("%s: CloseCursor: %ld",sDsUserName,nCursorId);

  if(nCursorId<=0||nCursorId>MAX_CURSOR_NUM)
  {
    nSqlErrNo=1001;
    ShowErrMsg();
    return -nSqlErrNo;
  }
  /* oclose() disconnects a cursor from the data areas 
     in the Oracle Server with which it is associated. */
  if (oclose(&cursor[nCursorId-1]))
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    ShowErrMsg();
    return -nSqlErrNo;
  }
  sCursorFlag[nCursorId-1]='0';
  return 1;
}

/* 
 * 函数：RunSelect
 * 功能：以OCI方式执行一条SELECT语句
 * 参数：char * statement:SQL语句
 *       ...:statement中需要使用的参数
 * 返回：>0:成功 =0:没有数据  <0:失败
 * 说明：
 */
long RunSelect(char *statement,...)
{
  long nRetVal;
  long col,field,args,loop;
  char sql[MAX_SQL_SIZE];
  char execute[MAX_SQL_SIZE];
  char * offset,* into;
  va_list pvar;
  
  memset(sql,0,sizeof(sql));
  strcpy(execute,statement);
  into=strstr(execute,"into");
  if(!into) into=strstr(execute,"INTO");
  
  if(into)
  {
      into--;
      into[0]='\0';
      into++;
  }    
  va_start(pvar,statement);
  loop = FormSqlWithArgs(execute,pvar,sql);
  
  if(loop<0)
  {
      DBG("SQL语名格式错误");
      return -1;
  }    
  
  offset=strchr(execute,'%');
  while(offset)
  {
      if(strncmp(offset,"%s",2 )==0) (void)va_arg(pvar,char *);
      else if(strncmp(offset,"%lf",3)==0) (void)va_arg(pvar,double);
      else if(strncmp(offset,"%ld",3)==0) (void)va_arg(pvar,long);

      offset++;
      offset=strchr(offset,'%');
  }           
  
  DBG("%s: RunSelect: %s",sDsUserName,sql);

  /* Parse the statement; do not defer the parse,
       so that errors come back right away. */
  if (oparse(&cda, (text *) sql, (sb4) -1,
             (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
    nSqlErrNo=cda.rc;
    ShowErrMsg();
    return -nSqlErrNo;
  }

  /* Save the SQL function code right after parse. */
  sql_function = cda.ft;

  if(sql_function!=FT_SELECT)
  {
    nSqlErrNo=ERR_NO_SELECT;
    ShowErrMsg();
    return -nSqlErrNo;
  }

  /* describe and define all select-list items. */
  if((field = describe_define(&cda)) <0)
  {
    nSqlErrNo=cda.rc;
    ShowErrMsg();
    return -nSqlErrNo;
  }

  /* Execute the statement. */

  if (oexfet(&cda, (ub4) 1, FALSE, FALSE))
  {
      if (cda.rc == NO_DATA_FOUND)
      {
        nSqlErrNo=cda.rc;
        ShowErrMsg();
        va_end(pvar);
        return 0;
      }
      else
      {
        nSqlErrNo=cda.rc;
        ShowErrMsg();
        va_end(pvar);
        return -nSqlErrNo;
      }
  }
  
  if(!into) return 1;

  offset=into;
  
  for (col = 0; col < field; col++)
  {
      offset=strchr(offset,'%');
      if(!offset)
      {
      	 DBG("取得字段的个数与实际数据个数不一致");
      	 va_end(pvar);
      	 return -2;
      }
      offset++;
      args=va_arg(pvar,long);

      if(offset[0]=='l' && offset[1]=='f')
      {
         (*(double*)args)=atof((char*)def[col].buf);
      }
      else if(offset[0]=='l' && offset[1]=='d')
      {
         (*(long*)args)=atol((char*)def[col].buf);
      }   
      else if(offset[0]=='s')
      {
         strcpy((char*)args,(char*)def[col].buf);
      }   
      else
      {  
      	 DBG("无法鉴别的数据类型格式");
      	 va_end(pvar);
      	 return -3;
      }	 
  }
  va_end(pvar);
  return 1;

}




/////////////////////////////////////////////////////////////////
///////////////////   以下是内部功能函数 ////////////////////////
/////////////////////////////////////////////////////////////////

int dsConnectDatabase(int socket)
{
  long nRetVal;
  char username[MAX_USERNAME_LEN+1];
  char password[MAX_PASSWORD_LEN+1];
 
  if(GetStringFromSocket(socket,username))
  {
    close(socket);
    return 1;
  }

  if(GetStringFromSocket(socket,password))
  {
    close(socket);
    return 1;
  }

  nRetVal=ConnectDatabase(username,password);

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

int dsCloseDatabase(int socket)
{
  long nRetVal;
  
  nRetVal=CloseDatabase();
  SendLongToSocket(socket,nRetVal);
  close(socket);
  return 0;
}

int dsGetSqlErrNo(int socket)
{
  DBG("%s: GetSqlErrNo: %ld",sDsUserName,-nSqlErrNo);

  if(SendLongToSocket(socket,-nSqlErrNo))
  {
    close(socket);
    return 1;
  }
  return 0;
}



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



int dsRunSql(int socket)
{
  long nRetVal;
  char statement[MAX_SQL_SIZE];
  
  if(GetStringFromSocket(socket,statement))
  {
    close(socket);
    return 1;
  }
  nRetVal=RunSqlStmt(statement);
  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }
  return 0;
}

static long RunSqlStmt(char *statement)
{

  DBG("%s: RunSql: %s",sDsUserName, statement);

  /* oparse() parses a SQL statement or a PL/SQL block and associates it 
     with a cursor. The parse can optionally be deferred. */
  /* Parse the statement; do not defer the parse,
     so that errors come back right away. */
  if (oparse(&cda, (text *) statement, (sb4) -1,
       (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
    nSqlErrNo=cda.rc;
    ShowErrMsg();
    return -nSqlErrNo;
  }

  /* Save the SQL function code right after parse. */
  sql_function = cda.ft;

  /* Execute the statement. */
  if (oexec(&cda))
  {
    nSqlErrNo=cda.rc;
    ShowErrMsg();
    return -nSqlErrNo;
  }

  if(sql_function==FT_UPDATE||sql_function==FT_DELETE||sql_function==FT_INSERT)
  {
    if(cda.rpc==-999) return 0;
    return cda.rpc;
  }
  
  return 1; 
} 


int dsOpenCursor(int socket)
{
  long nRetVal;
  char statement[MAX_SQL_SIZE];
  
  if(GetStringFromSocket(socket,statement))
  {
    close(socket);
    return 1;
  }

  nRetVal=OpenCursorStmt(statement);

  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }

  return 0;
}


static long OpenCursorStmt(char * statement)
{
  long nCursorId;

  nCursorId=1;
  while(sCursorFlag[nCursorId-1]=='1'&&nCursorId<=MAX_CURSOR_NUM)
    nCursorId++; 

  DBG("%s: OpenCursor: %ld: %s",sDsUserName,nCursorId,statement);

  if(nCursorId==MAX_CURSOR_NUM+1)
  {
    nSqlErrNo=1000;
    ShowErrMsg();
    return -nSqlErrNo;
  }

  sCursorFlag[nCursorId-1]='1'; 

  if (oopen(&cursor[nCursorId-1], &lda, (text *) 0, -1, -1, (text *) 0, -1))
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    ShowErrMsg();
    return -nSqlErrNo;
  }

  /* Parse the statement; do not defer the parse,
       so that errors come back right away. */
  if (oparse(&cursor[nCursorId-1], (text *) statement, (sb4) -1,
             (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
     nSqlErrNo=cursor[nCursorId-1].rc;
     CloseCursor(nCursorId);
     ShowErrMsg();
     return -nSqlErrNo;
  }

  /* Save the SQL function code right after parse. */
  sql_function = cursor[nCursorId-1].ft;

  if(sql_function!=FT_SELECT)
  {
    CloseCursor(nCursorId);
    nSqlErrNo=ERR_NO_SELECT;
    DBG("%s: OpenCursor: not SELECT statement",sDsUserName);
    return -nSqlErrNo;
  }

  /* describe and define all select-list items. */
  if((nColNum[nCursorId-1] = describe_define(&cursor[nCursorId-1])) <0)
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    CloseCursor(nCursorId);
    ShowErrMsg();
    return -nSqlErrNo;
  }

  /* Execute the statement. */
  if (oexec(&cursor[nCursorId-1]))
  {
    nSqlErrNo=cursor[nCursorId-1].rc;
    ShowErrMsg();
    return -nSqlErrNo;
  }
  return nCursorId;
}





int dsFetchCursor(int socket)
{
  long nRetVal;
  long col,field,len,size,count;
  long nCursorId;
  char result[MAX_SQL_SIZE];
  char * offset;
  
  
  if(GetLongFromSocket(socket,&nCursorId))
  {
    close(socket);
    return 1;
  }

  if (ofetch(&cursor[nCursorId-1]))
  {
    if (cursor[nCursorId-1].rc == NO_DATA_FOUND)
      nRetVal=0;
    else
    {
      nSqlErrNo=cursor[nCursorId-1].rc;
      ShowErrMsg();
      nRetVal=-nSqlErrNo;
    }
  }
  else  nRetVal=1;

  if(SendLongToSocket(socket,nRetVal))
  {
    close(socket);
    return 1;
  }

  if(nRetVal==1)
  {
    field = nColNum[nCursorId-1];

    count=0;
    offset=result;
    memset(result,0,sizeof(result));

    for (col = 0; col < field; col++)
    {
        len=(long)strlen((char*)def[col].buf);
        memcpy(offset,(char*)def[col].buf,len+1);
        offset+=(len+1);
        count+=(len+1);
    }

    SendToSocket(socket,"%ld%ld",field,count);

    if(SendRecordToSocket(socket,result,count))
    {
        close(socket);
        return 1;
    }
  }
  return 0;
}


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


int dsRunSelect(int socket)
{
  long nRetVal;
  long col,len,count,size;
  long field;
  char statement[MAX_SQL_SIZE];
  char result[MAX_SQL_SIZE];
  char * offset;
  
  
  if(GetStringFromSocket(socket,statement))
  {
    close(socket);
    return 1;
  }

  DBG("%s: RunSelect: %s",sDsUserName,statement);

  
  /* Parse the statement; do not defer the parse,
       so that errors come back right away. */
  if (oparse(&cda, (text *) statement, (sb4) -1,
             (sword) PARSE_NO_DEFER, (ub4) PARSE_V7_LNG))
  {
    nSqlErrNo=cda.rc;
    nRetVal=-nSqlErrNo;
    ShowErrMsg();
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
    ShowErrMsg();
    if(SendLongToSocket(socket,nRetVal))
    {
      close(socket);
      return 1;
    }
    return 0;
  }

  /* describe and define all select-list items. */
  if((field = describe_define(&cda)) <0)
  {
    nSqlErrNo=cda.rc;
    nRetVal=-nSqlErrNo;
    ShowErrMsg();
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
        ShowErrMsg();
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
        ShowErrMsg();
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

  if(GetLongFromSocket(socket,&count))
  {
    close(socket);
    return 1;
  }

  if(count<=0) return 0;

  offset=result;
  memset(result,0,sizeof(result));
  count=0;
  
  for (col = 0; col < field; col++)
  {
    len=(long)strlen((char*)def[col].buf);
    memcpy(offset,(char*)def[col].buf,len+1);
    offset+=(len+1);
    count+=(len+1);
  }

  SendToSocket(socket,"%ld%ld",field,count);

  if(SendRecordToSocket(socket,result,count))
  {
    close(socket);
    return 1;
  }
  return 0;
}


/* 
 * 函数：FormSqlWithArgs
 * 功能：根据源SQL语句和参数列表形成最终的SQL语句
 * 参数: source:源SQL语句
 *       va_list:变长参数指针
 *       dest:目标串指针;
 * 返回：1:成功，否则失败
 * 说明：
 */
long FormSqlWithArgs(char * source,va_list pvar,char * dest)
{

    char * sSearch;
    char * sForm;
    long nError;
    long loop;

    nError=0;
    sSearch=source;
    sForm=dest;
    loop=0;
    
    while(sSearch[0]!='\0')
    {
        switch(sSearch[0])
        {
           case '%':
                sSearch++;
                switch(sSearch[0])
                {
                    case 's':
                         sprintf(sForm,"'%s'",va_arg(pvar,char *));
                         while(sForm[0]!='\0') sForm++;
                         sSearch++;
                         loop++;
                         break;
                    case 't':
                         sprintf(sForm,"%s",va_arg(pvar,char *));
                         while(sForm[0]!='\0') sForm++;
                         sSearch++;
                         loop++;
                         break;
                    case 'l':
                         sSearch++;
                         switch(sSearch[0])
                         {
                             case 'f':
                                  sprintf(sForm,"%-.2lf",va_arg(pvar,double));
                                  while(sForm[0]!='\0') sForm++;
                                  sSearch++;
                                  loop++;
                                  break;

                             case 'd':
                                  sprintf(sForm,"%ld",va_arg(pvar,long));
                                  while(sForm[0]!='\0') sForm++;
                                  sSearch++;
                                  loop++;
                                  break;

                             default:nError=1;
                         }
                         break;

                    default:
                         sForm[0]='%';
                         sForm++;
                         sForm[0]='\0';
                         break;
                 }//end switch
                 break;

           default:          //非‘%’
                sForm[0]=sSearch[0];
                sForm++;
                sSearch++;
                break;
        }//end switch
        if(nError)  return -1;

    }//end while
    
    return loop;
}


static sword describe_define(Cda_Def *cda)
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


static void DBG(char * format,...)
{
   #ifdef  DEBUG
   va_list pvar;
   char    buf[MAX_SQL_SIZE];
   
   va_start(pvar,format);
   vsprintf(buf,format,pvar);
   va_end(pvar);
   fprintf(stderr,"\n%s\n",buf);
   #endif
	
}
	
static void ShowErrMsg()
{
  #ifdef DEBUG
  char sErrMsg[MAX_ERR_MSG_LEN];

  if(nSqlErrNo==ERR_NO_TCPIP)
    strcpy(sErrMsg,sTcpipErrInfo);
  if(nSqlErrNo==ERR_NO_SELECT)
    strcpy(sErrMsg,"not SELECT statement");

  oerhms(&lda,nSqlErrNo,(text *)sErrMsg,(sword)MAX_ERR_MSG_LEN);
  fprintf(stderr,"\n%s",sErrMsg);
  #endif
  
}
