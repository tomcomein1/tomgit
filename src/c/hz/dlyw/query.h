/*=============================================================
                     标准化查询工具模块头文件
                         (query.h)

original programmer:chenbo [陈博]
created date:2001.08.15
=============================================================*/

#ifndef QUERY_H
#define QUERY_H
#include<stdio.h>
#include<stdarg.h>
#define STRINGSIZE              500               //最大查询字符串长度
#define ITEMSIZE                100               //每次打开游标的最大字段数
#define MAX_SQL_SIZE           4000        

#define KEY_PROCESS   99999998
#define KEY_PRINT     99999999
#define ST_VISUAL     0x00000001
#define ST_PRINT      0x00000002
#define ST_KEYWORD    0x00000004
#define ST_LASTLINE   0x00000008


typedef union  datanode{
                        long   * pLong;
                        double * pDouble;
                        char   * pChar;
                       }DATANODE;

typedef struct queryunit{
                         char sFormat[2000];
                         DATANODE data[ITEMSIZE];
                        }QUERYUNIT;

typedef struct {
                 char * sTitle;        //表头
                 char * sFormat;       //显示格式字符串
                 char sHeader[256];    //标题串
                 char sLastLine[640];  //最后一行汇总
                 char sFileName[31];   //临时文件名
                 char sIndex[41];      //查询关键字
                 long nStyle;          //风格
                 long nStatus;         //状态字
                 long nLength;         //标题长度
                 long nCurrent;        //屏幕顶行状态
                 long nStartCol;       //屏幕列偏移
                 long nStartRow;       //查询开始行
                 long nStartLine;      //屏幕起始行
                 long nPage;           //每页显示的记录数
                 long nCount;          //文件中记录数
                 long nReverse;        //反显行
                 void (*pAction)(char * msgline);  //任务函数指针
                 FILE * fp;            //临时文件指针
                 long nCurrentGroup;     //当前列
                 long nGroupSize;        //列大小
                 long nFieldSize;      //每行的字段数
                 char sStorage[640];   //临时存储
               }QUERYTABLE;

QUERYTABLE * CreateQueryTable(char * sHeader,char * sTitle,char * sFormat,long nStyle);
void SetQueryAction(QUERYTABLE * pqt,void (*pAction)(char * msgline));
void SetQueryGroupSize(QUERYTABLE * pqt,long nGroupSize);
long ShowQueryTable(QUERYTABLE * pqt);
long ImportLastLine(QUERYTABLE * pqt,...);
long ImportQueryTable(QUERYTABLE * pqt,long nflag,...);
static long FindQueryTable(QUERYTABLE * pqt);
long PrintQueryTable(QUERYTABLE * pqt,long nmode);
static long FormStringByFormat(QUERYTABLE * pqt,char * sOutput,va_list pvar);
long DropQueryTable(QUERYTABLE * pqt);


long QueryLocal(char * sTitle,char * sFormat,char * sSqlExpression,...);
void ReleaseQueryUnit(QUERYUNIT * query);
long FetchStruction(long nCursorId,QUERYUNIT * que);
long StringMatch(char * sObject,char * sKeyWord,char * sResult);

#endif
