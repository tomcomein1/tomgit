#ifndef FILE_DB_H
#define FILE_DB_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include"des.h"
#include"tcpip.h"

/*Sql Server 服务定义*/
#define   TASK_CLOSE_MISSION           0
#define   TASK_CONNECT_DATABASE        1
#define   TASK_CLOSE_DATABASE          2
#define   TASK_COMMIT_WORK             3
#define   TASK_ROLLBACK_WORK           4
#define   TASK_RUN_SELECT              5
#define   TASK_RUN_SQL                 6
#define   TASK_OPEN_CURSOR             7
#define   TASK_FETCH_CURSOR            8
#define   TASK_CLOSE_CURSOR            9
#define   TASK_RUN_PROCEDURE          10
#define   TASK_GET_SQL_ERRNO          11
#define   TASK_GET_SQL_ERRINFO        12
#define   MAX_SQL_SIZE              8192


int  ConnectSqlServer(char * machine,char * service);
long ConnectDatabase(char * username,char * password);
long CloseDatabase();
long GetSqlErrNo();
long GetSqlErrInfo(char * err_msg);
long CommitWork();
long RollbackWork();
long RunSql(char * statement,...);
long OpenCursor(char * statement,...);
long FetchCursor(long cursor_id,char * format,...);
long CloseCursor(long cursor_id);
long RunSelect(char * statement,...);
long RunProc(char * procedure_name,char * in_format,char * out_format,...);
long FormSqlWithArgs(char * source,va_list pvar,char * dest);


#endif
