/************************************************************************
make date:  2001.2.20
filename :  tasktool.h
target:     ���񹤾�ͷ�ļ�
*************************************************************************/
#ifndef TASKTOOL_H
#define TASKTOOL_H
#include<stdio.h>

#define LONG_COUNT        30          //����/���ؽṹ�г��������ݵ�������
#define DOUBLE_COUNT      30          //����/���ؽṹ��˫�������ݵ�������
#define STRING_COUNT      20          //����/���ؽṹ���ַ�������������������
#define INCREMENT_SIZE    4096        //RECORD����������[�γ��ļ���������]
typedef struct taskstruct{
                            long   nTaskNo;                //�����
                            char   sDestination[21];       //Ŀ�صص�ַ
                            long   nRouteNum;              //·����Ŀ
                            long   nRetVal;                //����ֵ
                            long   nValue[LONG_COUNT];     //�������������
                            double dValue[DOUBLE_COUNT];   //�������������
                            long   nLength[STRING_COUNT];  //��������ڴ�������
                            char * sValue[STRING_COUNT];   //��������ڴ���ָ��
                          }TASKSTRUCT;
                         
typedef struct memorystruct{
                             long   nlen;                  //����������
                             char   * pointer;             //������ͷָ��
                           }MEMORY;

typedef struct record{
                       long nsize;                         //��¼���ܳ���
                       FILE * fp;
                       char * scurrent;                    //��ǰ��¼ָ��λ��
                       char * sdata;                       //��¼���ݵ�ͷָ��
                       char * filename;
                     }RECORD;
                     
#ifdef HOUTAI
long ApplyToClient(char * soutstr,...);                    //��Ӧ�Ͷ�
long dsTaskAutoRunTask(int nInSocket,long nTaskNo);        //��������������
#endif


long AutoRunTask(char * sdest,long ntask,char * sinstr,char * soutstr,...);
MEMORY MakeMemory(long nlen,char * pointer);               //�����ڴ������γɺ���
long SendTaskStructToSocket(int nsock, TASKSTRUCT * task); 
long GetTaskStructFromSocket(int nsock,TASKSTRUCT * task);
void DestroyTaskStruct(TASKSTRUCT * task);                

RECORD CreateRecord();                                     //������ʽ��¼�ṹ
long ImportRecord(RECORD * rec,char * arglist,...);        //���¼�ṹ�����Ӽ�¼
long RewindRecord(RECORD * rec);                           //��¼ָ�븴λ
void DropRecord(RECORD * rec);                             //�ͷż�¼
static long GetArgType(char * list);                              
static long GetArgCount(char * list);
long ExportRecord(RECORD * rec,char * arglist,...);        //�Ӽ�¼��ȡ������
long GetRecordSize(RECORD * rec);
#endif


