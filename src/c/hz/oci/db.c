/*        
 *                             db.c
 *
 *                   Database Access Interface (Client)
 *
 *   (c)Copyright by Chenbo 2001. 
 *
 *   Original Programmer : chenbo[�²�]
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
 * ������ConnectSqlServer
 * ���ܣ���sqld����������
 * ������char * machine:��������ַ�����
 *       char * service:�������˿�
 * ���أ�>=0:�ɹ�������ʧ��
 * ˵����
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
 * ������ConnectDatabase
 * ���ܣ��������ݿ�
 * ������char * usermame:�û���
 *       char * password:����
 * ���أ�=0:�ɹ�������ʧ��
 * ˵����
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
 * ������CloseDatabase
 * ���ܣ��ر����ݿ�����
 * ������
 * ���أ�=0:�ɹ�
 * ˵����
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
 * ������
 * ���ܣ��õ����һ�εĴ������
 * ������
 * ���أ��������
 * ˵����
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
 * ������
 * ���ܣ��õ����һ�εĴ�����Ϣ
 * ������char * sErrMsg:����ش��ַ���
 * ���أ�
 * ˵����
 */
long GetSqlErrInfo(char *sErrMsg)
{
	
    long nRetVal=-1;
    if(mission<0)
    {
    	strcpy(sErrMsg,"����û�н������ж�...");
    	return 0;
    }
    
    if(SendLongToSocket(mission,(long)TASK_GET_SQL_ERRINFO))
    {
    	close(mission);
    	mission=-1;
    	strcpy(sErrMsg,"����û�н������ж�...");
    	return -1;
    }

    if(GetStringFromSocket(mission,sErrMsg))
    {
    	close(mission);
    	mission=-1;
    	strcpy(sErrMsg,"����û�н������ж�...");
    	return -2;
    }
    return 0;
}

/* 
 * ������CommitWork
 * ���ܣ����ݿ��ύ
 * ��������
 * ���أ�1:�ɹ�������ʧ��
 * ˵����
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
 * ������RollbackWork
 * ���ܣ����ݿ�ع�
 * ��������
 * ���أ�1:�ɹ�������ʧ��
 * ˵����
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
 * ������RunSql
 * ���ܣ�ִ��һ��SQL���ݲ������
 * ������char *format:SQL���ݲ������
 *       ...:����SQL�����ʹ�õĲ���
 * ���أ�>0:�ɹ�  <0:ʧ��   =0:û�ҵ�����
 * ˵����
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
 * ������OpenCursor
 * ���ܣ���һ���α�������
 * ������char * statement:SELECT���
 * ���أ�>0:�ɹ����α�ID   <0:ʧ��   =0:û�ҵ�����
 * ˵����
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
 * ������FetchCursor
 * ���ܣ����Ѵ򿪵��α���ȡ������
 * ������long nCursorId:�α�ID
 *       char * format: �����������ݸ�ʽ
 * ���أ�>0:�ɹ�  <0:ʧ��  =0:������ȡ��
 * ˵����
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
 * ������CloseCursor
 * ���ܣ��ر�һ�����ҿ����α�
 * ������long nCursorId:�α�ID
 * ���أ�1���ɹ�������ʧ��
 * ˵����
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
 * ������RunSelect
 * ���ܣ���OCI��ʽִ��һ��SELECT���
 * ������char * statement:SQL���
 *       ...:statement����Ҫʹ�õĲ���
 * ���أ�>0:�ɹ� =0:û������  <0:ʧ��
 * ˵����
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
 * ������FormSqlWithArgs
 * ���ܣ�����ԴSQL���Ͳ����б��γ����յ�SQL���
 * ����: source:ԴSQL���
 *       va_list:�䳤����ָ��
 *       dest:Ŀ�괮ָ��;
 * ���أ�1:�ɹ�������ʧ��
 * ˵����
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

           default:          //�ǡ�%��
                sForm[0]=sSearch[0];
                sForm++;
                sSearch++;
                break;
        }//end switch
        if(nError)  return -1;

    }//end while
    
    return loop;
}


