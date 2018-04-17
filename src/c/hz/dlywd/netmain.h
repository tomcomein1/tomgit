// 请同时更改dlpublic.h和netmain.h里的操作系统定义
#ifndef NETMAIN_H
#define NETMAIN_H


// DOS下编译
//#define DOS

// SCO UNIX下编译
//#define UNIX

#define CONNECT_IDENTIFICATION

// make文件中已定义 HP UNIX下编译
//#define HPUNIX

// make文件中已定义 IBM UNIX下编译
//#define IBMUNIX

#if defined(HPUNIX) || defined(IBMUNIX)
#define UNIX
// 长型个位字节在最后
#define LITTLEEND
#endif

#if defined(DOS) && defined(UNIX)
#error Both DOS and UNIX have been defined.
#endif

#if !defined(DOS) && !defined(UNIX)
#error Neither DOS nor UNIX has been defined.
#endif

#ifdef UNIX

#include <errno.h>
#include <signal.h>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <netdb.h>

#include <sys/resource.h>
#include <sys/wait.h>

#endif

#ifdef WINDOWS
#include <winsock.h>
#endif

#ifdef DOS

#include <sys\types.h>
#include <sys\socket.h>
#include <4bsddefs.h>
#include <netinet\in.h>
#include <netinet\tcp.h>
#include <netdb.h>
#include <arpa\inet.h>

#endif

#define TASK_NULL 0
#define TASK_CSXTWJ 1
#define TASK_CSWJ 2
#define TASK_FSWJ 3
#define TASK_CSXTDS 4
#define TASK_CSDD 5
#define TASK_BHCSXTWJ 6
#define TASK_BHCSWJ 7
#define TASK_BHCSXTDS 8
#define TASK_SCWJ 9
#define TASK_CWJM 10

#define TASK_CONNECTDATABASE 11
#define TASK_CLOSEDATABASE 12
#define TASK_GETSQLERRNO 13
#define TASK_GETSQLERRINFO 14
#define TASK_COMMITWORK 15
#define TASK_ROLLBACKWORK 16
#define TASK_SETAUTOCOMMITON 17
#define TASK_SETAUTOCOMMITOFF 18
#define TASK_RUNSQL 21
#define TASK_OPENCURSOR 22
#define TASK_FETCHCURSOR 23
#define TASK_CLOSECURSOR 24
#define TASK_RUNSELECT 25
#define TASK_RUNPROC 26
#define TASK_FETCHCURSORARRAY 27
#define TASK_SQLPLUS 31

#define TASK_BHCSDDWJ 51
#define TASK_BHFSWJ 52
#define TASK_BHFSBDWJ 53
#define TASK_BHCSBKTX 54
// 上载文件
#define TASK_BHSZWJ 55
// 取给定目录下一个文件名
#define TASK_GETONEFILENAME 61
// 移动文件到另一个目录
#define TASK_MOVEFILE 62
#define TASK_COMMAND 101

#define ERR_TCPIP 32767
#define ERR_SELECT 32766

#define BUFFERSIZE 1024

#ifdef WINDOWS
#define WSVERS          MAKEWORD(2, 0)
#define WINEPOCH        2208988800      /* Windows epoch, in UCT secs   */
#endif

#ifdef UNIX
#define UNIXEPOCH       2208988800      /* UNIX epoch, in UCT secs      */
#endif

typedef struct {
  int sent;
  int recvd;
  double max;
  double min;
  int total;}PINGSTAT;

#ifdef UNIX
extern int      errno;
#endif

int errexit(const char *format, ...);

#ifdef UNIX
int connectsock(const char *host, const char *service, const char *transport);
int connectTCP(const char *host, const char *service );
int connectUDP(const char *host, const char *service );
time_t UDPtime(const char *host);
time_t TCPtime(const char *host);
PINGSTAT ping(const char * host);
void show_ping_stat(PINGSTAT pingstat); 
int passiveUDP(const char *service);
int passiveTCP(const char *service, int qlen);
int passivesock(const char *service, const char *transport, int qlen);
#endif

#ifdef WINDOWS
SOCKET connectsock(const char *host, const char *service, const char *transport );
SOCKET connectTCP(const char *host, const char *service );
SOCKET connectUDP(const char *host, const char *service );
SOCKET passiveUDP(const char *service);
SOCKET passiveTCP(const char *service, int qlen);
SOCKET passivesock(const char *service, const char *transport, int qlen);
#endif

#ifdef DOS
int connectsock(const char *host, const char *service, const char *transport);
int connectTCP(const char *host, const char *service );
int connectUDP(const char *host, const char *service );
#endif


int ConnectRemote(char* sRemoteHost, char* sService);
int DisconnectRemote(int nSocket);
int GetStringFromSocket(const int socket,char *sValue);
int GetLongFromSocket(const int socket,long* pnValue);
int GetDoubleFromSocket(const int socket,double* pdValue);
int SendStringToSocket(const int socket,char * sValue);
int SendRecordToSocket(const int socket,char * sValue,int nLen);
int GetRecordFromSocket(const int socket, char* sValue, const char cEndChar);
int SendLongToSocket(const int socket,long nValue);
int SendDoubleToSocket(const int socket,double dValue);
int SendToSocket(const int socket, char* sFormat, ...);
int GetFromSocket(const int socket, char* sFormat, ...);

long ExecTask(int nSocket, long nTaskNo, char* sRemoteServer);
long RunTask(char* sDestination, long nTaskNo, char* sInputArg, char* sOutputArg, ...);
long RunCall(char* sDestination, long nTaskNo, char* sInBuffer, char* sOutBuffer);


#ifdef LITTLEEND
long ReverseLong(long nVal);
long ReverseLong1(long nVal);
long ReverseLong2(long nVal);
double ReverseDouble(double dVal);
#endif

#define ERR_NO_SELECT 32766
#define ERR_NO_TCPIP 32767
#define ERR_NO_FETCH 32765
#define ERR_NO_PROCESS 32764
#define ERR_NO_ROUTE 32763
#define ERR_NO_ENCRYPT 32762
#define ERR_NO_CONNECT 32761
#define ERR_NO_SQL 32760
#define MAX_WAITING_TIME 3600*24
#define MIN_WAITING_TIME 20
#define MAX_ROUTE_NUM 10

#endif

