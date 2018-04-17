/*        
 *                             db.c
 *
 *                   Database Access Interface (Client)
 *
 *   (c)Copyright by Chenbo 2001. 
 *
 *   Original Programmer : chenbo[陈博]
 *
 *   Modify Record:
 *
 *   DATE   NAME         REASON      
 */

 
#include"db.h"


static int  mission=-1;
char        sSysDsHost[40]="sqlhost";
char        sSysDsService[40]="sqlsrv";

/* 
 * 函数：ConnectSqlServer
 * 功能：与sqld服务建立连接
 * 参数：char * machine:服务器地址或别名
 *       char * service:服务器端口
 * 返回：>=0:成功，否则失败
 * 说明：
 */

int ConnectSqlServer(char * machine,char * service)
{
     
   int  socket,length;
   char keyword[9],identify[9];
   long nRetVal;
   
   length=8;
   socket = connectTCP(machine,service);
   if(socket<0) return socket;
   
   memset(keyword,0,sizeof(keyword));
   memset(identify,0,sizeof(identify));
   
   if(GetOneRecordFromSocket(socket,keyword,8)!=8)
   {
      close(socket);
      return -1;
   }   
   
   
   
   DesEncString(keyword,8,keyword,8,identify,&length);
   
   SendRecordToSocket(socket,identify,8);
   
   if(GetLongFromSocket(socket,&nRetVal)||nRetVal!=0)	
   {
      close(socket);
      return -2;
   }   
   
   return socket;
}


/* 
 * 函数：ConnectDatabase
 * 功能：连接数据库
 * 参数：char * usermame:用户名
 *       char * password:口令
 * 返回：=0:成功，否则失败
 * 说明：
 */

long ConnectDatabase(char *username,char *password)
{   
    
    long nRetVal=-1;
    
    mission = ConnectSqlServer(sSysDsHost,sSysDsService);
    if(mission<0) return mission;
    
    
    if(SendToSocket(mission,"%ld%s%s",(long)TASK_CONNECT_DATABASE,
                    username,password))
    {
    	close(mission);
    	mission=-1;
    	return -1;
    }
    
    if(GetLongFromSocket(mission,&nRetVal)||nRetVal!=0)
    {
    	close(mission);
    	mission=-1;
    	return -2;
    }
    return 0;
}

/* 
 * 函数：CloseDatabase
 * 功能：关闭数据库连接
 * 参数：
 * 返回：=0:成功
 * 说明：
 */
long CloseDatabase()
{
    long nRetVal=-1;
    
    if(SendLongToSocket(mission,(long)TASK_CLOSE_DATABASE))
    {
    	close(mission);
    	mission=-1;
    	return -1;
    }

    if(GetLongFromSocket(mission,&nRetVal)||nRetVal!=0)
    {
    	close(mission);
    	mission=-1;
    	return -2;
    }
    close(mission);
    mission=-1;
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
    long nRetVal=-1;
    
    if(SendLongToSocket(mission,(long)TASK_GET_SQL_ERRNO))
    {
    	close(mission);
    	mission=-1;
    	return -1;
    }

    if(GetLongFromSocket(mission,&nRetVal)||nRetVal!=0)
    {
    	close(mission);
    	mission=-1;
    	return -2;
    }
    return nRetVal;
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
	
    long nRetVal=-1;
    if(mission<0)
    {
    	strcpy(sErrMsg,"连接没有建立或被中断...");
    	return 0;
    }
    
    if(SendLongToSocket(mission,(long)TASK_GET_SQL_ERRINFO))
    {
    	close(mission);
    	mission=-1;
    	strcpy(sErrMsg,"连接没有建立或被中断...");
    	return -1;
    }

    if(GetStringFromSocket(mission,sErrMsg))
    {
    	close(mission);
    	mission=-1;
    	strcpy(sErrMsg,"连接没有建立或被中断...");
    	return -2;
    }
    return 0;
}

/* 
 * 函数：CommitWork
 * 功能：数据库提交
 * 参数：无
 * 返回：1:成功，否则失败
 * 说明：
 */
long CommitWork()
{
    long nRetVal=-1;
    
    if(SendLongToSocket(mission,(long)TASK_COMMIT_WORK))
    {
    	close(mission);
    	mission=-1;
    	return -1;
    }

    if(GetLongFromSocket(mission,&nRetVal)||nRetVal!=0)
    {
    	close(mission);
    	mission=-1;
    	return -2;
    }
    return nRetVal;

}


/* 
 * 函数：RollbackWork
 * 功能：数据库回滚
 * 参数：无
 * 返回：1:成功，否则失败
 * 说明：
 */
long RollbackWork()
{
    long nRetVal=-1;
    
    if(SendLongToSocket(mission,(long)TASK_ROLLBACK_WORK))
    {
    	close(mission);
    	mission=-1;
    	return -1;
    }

    if(GetLongFromSocket(mission,&nRetVal)||nRetVal!=0)
    {
    	close(mission);
    	mission=-1;
    	return -2;
    }

    return nRetVal;
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
    
    if(SendLongToSocket(mission,(long)TASK_RUN_SQL))
    {
    	close(mission);
    	mission=-1;
    	return -1;
    }

    if(SendStringToSocket(mission,sql))
    {
    	close(mission);
    	mission=-1;
    	return -2;
    }

    if(GetLongFromSocket(mission,&nRetVal))
    {
    	close(mission);
    	mission=-1;
    	return -3;
    }
    return nRetVal;
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

  
    if(SendLongToSocket(mission,(long)TASK_OPEN_CURSOR))
    {
    	close(mission);
    	mission=-1;
    	return -1;
    }

    if(SendStringToSocket(mission,sql))
    {
    	close(mission);
    	mission=-1;
    	return -2;
    }

    if(GetLongFromSocket(mission,&nRetVal))
    {
    	close(mission);
    	mission=-1;
    	return -3;
    }
    return nRetVal;
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
    long nRetVal=-1;
    long field,length,col,args;
    char data[MAX_SQL_SIZE];
    char * offset,* argtype;
    char value[2048];
    va_list pvar;
    
    
    if(SendToSocket(mission,"%ld%ld",(long)TASK_FETCH_CURSOR,nCursorId))
    {
    	close(mission);
    	mission=-1;
    	return -1;
    }

    if(GetLongFromSocket(mission,&nRetVal))
    {
    	close(mission);
    	mission=-1;
    	return -2;
    }
    if(nRetVal!=1) return nRetVal;
    
    if(GetFromSocket(mission,"%ld%ld",&field,&length))
    {
    	close(mission);
    	mission=-1;
    	return -4;
    }
    
    memset(data,0,sizeof(data));
    if(GetOneRecordFromSocket(mission,data,(int)length)!=(int)length)
    {
    	close(mission);
    	mission=-1;
    	return -5;
    }
    
    offset=data;
    argtype=format;
    va_start(pvar,format);
    
    for(col=0;col<field;col++)
    {
    	args=va_arg(pvar,long);
    	while(argtype[0]!='%' && argtype[0]) argtype++;
        strcpy(value,offset);

        while(offset[0]) offset++;
        offset++;
        
    	if(!argtype[0])
    	{
    	     va_end(pvar);
    	     return -6;
    	}
    	     
    	argtype++;

    	if(argtype[0]=='l' && argtype[1]=='d')
    	     (*(long*)args)= atol(value);
    	else if(argtype[0]=='l' && argtype[1]=='f')
    	     (*(double*)args)= atof(value);
    	else if(argtype[0]=='s')
    	     strcpy((char *)args,value);
        else 
        {
             va_end(pvar);
             return -7;
        }
    }//end for
    va_end(pvar);

    return nRetVal;
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
    long nRetVal=-1;
    
    if(SendToSocket(mission,"%ld%ld",(long)TASK_CLOSE_CURSOR,nCursorId))
    {
    	close(mission);
    	mission=-1;
    	return -1;
    }

    if(GetLongFromSocket(mission,&nRetVal))
    {
    	close(mission);
    	mission=-1;
    	return -2;
    }
    return nRetVal;
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
    long nRetVal=-1;
    long field,length,col,args,loop;
    char data[MAX_SQL_SIZE];
    char sql[MAX_SQL_SIZE];
    char execute[MAX_SQL_SIZE];
    char * offset,* argtype,*into;
    char value[2048];
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
    nRetVal = FormSqlWithArgs(execute,pvar,sql);
    
    if(nRetVal<0)
    {
    	va_end(pvar);
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
  
    if(SendLongToSocket(mission,(long)TASK_RUN_SELECT))
    {
    	close(mission);
    	va_end(pvar);
    	mission=-2;
    	return -1;
    }

    if(SendStringToSocket(mission,sql))
    {
    	close(mission);
    	va_end(pvar);
    	mission=-1;
    	return -3;
    }

    if(GetLongFromSocket(mission,&nRetVal))
    {
    	close(mission);
    	va_end(pvar);
    	mission=-1;
    	return -4;
    }
    if(nRetVal!=1) return nRetVal;
    
    if(!into)
    {
    	if(SendLongToSocket(mission,0L))
    	{
      	    close(mission);
    	    va_end(pvar);
    	    mission=-1;
    	}    
    	return nRetVal;
    }

    if(SendLongToSocket(mission,1L))
    {
    	close(mission);
    	va_end(pvar);
    	mission=-1;
    	return -4;
    }
    
    if(GetFromSocket(mission,"%ld%ld",&field,&length))
    {
    	close(mission);
    	va_end(pvar);
    	mission=-1;
    	return -5;
    }
    
    memset(data,0,sizeof(data));
    if(GetOneRecordFromSocket(mission,data,(int)length)!=(int)length)
    {
    	close(mission);
    	va_end(pvar);
    	mission=-1;
    	return -6;
    }

    offset=data;
    argtype=into;
    
    for(col=0;col<field;col++)
    {
    	args=va_arg(pvar,long);
    	while(argtype[0]!='%' && argtype[0]) argtype++;

        strcpy(value,offset);

        while(offset[0]) offset++;
        offset++;
        
    	if(!argtype[0])
    	{
    	     va_end(pvar);
    	     return -7;
    	}
    	     
    	argtype++;

    	if(argtype[0]=='l' && argtype[1]=='d')
    	     (*(long*)args)= atol(value);
    	else if(argtype[0]=='l' && argtype[1]=='f')
    	     (*(double*)args)= atof(value);
    	else if(argtype[0]=='s')
    	     strcpy((char *)args,value);
        else 
        {
             va_end(pvar);
             return -8;
        }
    }//end for
    va_end(pvar);
    return nRetVal;
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


