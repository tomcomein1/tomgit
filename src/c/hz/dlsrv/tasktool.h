/************************************************************************
make date:  2001.2.20
filename :  tasktool.h
target:     任务工具头文件
*************************************************************************/
#ifndef TASKTOOL_H
#define TASKTOOL_H
#include<stdio.h>

#define LONG_COUNT        30          //请求/返回结构中长整型数据的最大个数
#define DOUBLE_COUNT      30          //请求/返回结构中双精度数据的最大个数
#define STRING_COUNT      20          //请求/返回结构中字符串及数据流的最大个数
#define INCREMENT_SIZE    4096        //RECORD缓冲区长度[形成文件触发长度]
typedef struct taskstruct{
                            long   nTaskNo;                //任务号
                            char   sDestination[21];       //目地地地址
                            long   nRouteNum;              //路由数目
                            long   nRetVal;                //返回值
                            long   nValue[LONG_COUNT];     //输入输出长整型
                            double dValue[DOUBLE_COUNT];   //输入输出浮点型
                            long   nLength[STRING_COUNT];  //输入输出内存区长度
                            char * sValue[STRING_COUNT];   //输入输出内存区指针
                          }TASKSTRUCT;
                         
typedef struct memorystruct{
                             long   nlen;                  //数据流长度
                             char   * pointer;             //数据流头指针
                           }MEMORY;

typedef struct record{
                       long nsize;                         //记录的总长度
                       FILE * fp;
                       char * scurrent;                    //当前记录指针位置
                       char * sdata;                       //记录数据的头指针
                       char * filename;
                     }RECORD;
                     
#ifdef HOUTAI
long ApplyToClient(char * soutstr,...);                    //响应客端
long dsTaskAutoRunTask(int nInSocket,long nTaskNo);        //服务端任务主框架
#endif


long AutoRunTask(char * sdest,long ntask,char * sinstr,char * soutstr,...);
MEMORY MakeMemory(long nlen,char * pointer);               //发送内存区域形成函数
long SendTaskStructToSocket(int nsock, TASKSTRUCT * task); 
long GetTaskStructFromSocket(int nsock,TASKSTRUCT * task);
void DestroyTaskStruct(TASKSTRUCT * task);                

RECORD CreateRecord();                                     //建立流式记录结构
long ImportRecord(RECORD * rec,char * arglist,...);        //向记录结构中增加记录
long RewindRecord(RECORD * rec);                           //记录指针复位
void DropRecord(RECORD * rec);                             //释放记录
static long GetArgType(char * list);                              
static long GetArgCount(char * list);
long ExportRecord(RECORD * rec,char * arglist,...);        //从记录中取中数据
long GetRecordSize(RECORD * rec);
#endif


