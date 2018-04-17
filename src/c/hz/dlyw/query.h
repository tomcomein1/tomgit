/*=============================================================
                     ��׼����ѯ����ģ��ͷ�ļ�
                         (query.h)

original programmer:chenbo [�²�]
created date:2001.08.15
=============================================================*/

#ifndef QUERY_H
#define QUERY_H
#include<stdio.h>
#include<stdarg.h>
#define STRINGSIZE              500               //����ѯ�ַ�������
#define ITEMSIZE                100               //ÿ�δ��α������ֶ���
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
                 char * sTitle;        //��ͷ
                 char * sFormat;       //��ʾ��ʽ�ַ���
                 char sHeader[256];    //���⴮
                 char sLastLine[640];  //���һ�л���
                 char sFileName[31];   //��ʱ�ļ���
                 char sIndex[41];      //��ѯ�ؼ���
                 long nStyle;          //���
                 long nStatus;         //״̬��
                 long nLength;         //���ⳤ��
                 long nCurrent;        //��Ļ����״̬
                 long nStartCol;       //��Ļ��ƫ��
                 long nStartRow;       //��ѯ��ʼ��
                 long nStartLine;      //��Ļ��ʼ��
                 long nPage;           //ÿҳ��ʾ�ļ�¼��
                 long nCount;          //�ļ��м�¼��
                 long nReverse;        //������
                 void (*pAction)(char * msgline);  //������ָ��
                 FILE * fp;            //��ʱ�ļ�ָ��
                 long nCurrentGroup;     //��ǰ��
                 long nGroupSize;        //�д�С
                 long nFieldSize;      //ÿ�е��ֶ���
                 char sStorage[640];   //��ʱ�洢
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
