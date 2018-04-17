#include<stdio.h>
#include<sys/types.h>
#include"dlpublic.h"


#define SCREEN_WIDTH    80
#define SCREEN_LINE     24
#define STRINGSIZE      500                         //����ѯ�ַ�������
#define ITEMSIZE        100
#define MAX_SQL_SIZE   4000                       //SQL�����󳤶�
#define CURSOR_PACKAGE_SIZE    6144               //�α����ݰ���󳤶�
#define CURSOR_SEND_SIZE       3072               //�ְ���������


#define TASK_OPEN_REMOTECURSOR 9995
#define TASK_FETCH_FILECURSOR  9996               //ȡ�ļ��α�����   
#define TASK_OPEN_FILECURSOR   9997               //��Զ���ļ��α�[����ʽ����]
#define TASK_RUN_REMOTESQL     9998
#define TASK_QUERY_REMOTE      9999                  //��ѯԶ������
#define TASK_FETCHCURSOR 23
#define MAX_ROUTE_NUMBER  5

typedef union  datanode{
                        long   * pLong;
                        double * pDouble;
                        char   * pChar;
                       }DATANODE;

typedef struct queryunit{
                         char sFormat[2000];
                         DATANODE data[ITEMSIZE];
                        }QUERYUNIT;
                          

int FetchStruction(long nCursorId,QUERYUNIT * que);
int GetStructionFromServer(int nSocket,QUERYUNIT * pQuery);
int SendStructionToClient(int nSocket,QUERYUNIT * pQuery);
void ReleaseQueryUnit(QUERYUNIT * query);


long dsTaskFetchFileCursor(int nInSocket, pid_t child);
long dsTaskOpenFileCursor(int nInSocket, long nTaskNo);
long dsTaskOpenRemoteCursor(int nInSocket, long nTaskNo);
extern char * FetchString(char * sString,char * sFormat,...);

long FindRoute(char * sDestination,char * sGateway);
void SetSocketNoDelay(int nsock);


