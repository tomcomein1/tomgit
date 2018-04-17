// 2001.7.2 李宝东 为防止客户端程序在传输文件的同时操作数据库超时的情况，
// 客户端与服务端的netmain.h中的MAX_WAITING_TIME和MIN_WAITING_TIME设置成不同值。

// 请同时更改bkfx.h和netmain.h里的操作系统定义
#ifndef NETMAIN_H
#define NETMAIN_H


// DOS下编译
//#define DOS

// SCO UNIX下编译
#define UNIX

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

#include <stdarg.h>
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
static long time_now(void); 
void UDPping();
void SetWaitingTime(int nTime);
void SocketLoseControl();

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
