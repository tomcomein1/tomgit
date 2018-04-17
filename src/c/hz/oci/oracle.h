/*        
 *                            oracle.h
 *
 *                  Oracle Connection Interface Server
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

#ifndef FILE_ORACLE_H
#define FILE_ORACLE_H

#include"tcpip.h"
 
//此开关用于打开调试语句
#define DEBUG

#define TYPE_LONG             1
#define TYPE_DOUBLE           2
#define TYPE_STRING           3
#define TYPE_DATE             4

#define MAX_USERNAME_LEN      100      
#define MAX_PASSWORD_LEN      30      
#define MAX_BINDS             100     
#define MAX_ITEM_BUFFER_SIZE  2001     
#define MAX_SELECT_LIST_SIZE  100
#define MAX_ERR_MSG_LEN       512
#define MAX_SQL_IDENTIFIER    31
#define MAX_CURSOR_NUM        10
#define MAX_CURSOR_BUFFER     10000
#define MAX_SQL_SIZE          8192

#define PARSE_NO_DEFER        0
#define PARSE_DEFER           1
#define PARSE_V7_LNG          2

//系统头文件包含
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

//ORACLE头文件包含
#include <oratypes.h>
#include <ocidfn.h>
#ifdef __STDC__
#include <ociapr.h>
#else
#include <ocikpr.h>
#endif
#include <ocidem.h>

//ORACLE全局数据结构定义

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



//OCI函数接口定义
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

//Server函数定义
int  dsConnectDatabase(int socket);
int  dsCloseDatabase(int socket);
int  dsGetSqlErrNo(int socket);
int  dsGetSqlErrInfo(int socket);
int  dsCommitWork(int socket);
int  dsRollbackWork(int socket);
int  dsRunSql(int socket);
int  dsOpenCursor(int socket);
int  dsFetchCursor(int socket);
int  dsCloseCursor(int socket);
int  dsRunSelect(int socket);
int  dsRunProc(int socket);

static long RunSqlStmt(char * statement);
static long OpenCursorStmt(char * statement);
static sword describe_define(Cda_Def *cda);
long FormSqlWithArgs(char * source,va_list pvar,char * dest);

static void DBG(char * format,...);
static void ShowErrMsg();
#endif
