// 2001.4.17 李宝东修改接收和发送函数以提高性能，以后读socket必须使用ReadSocket

#define TIME            37
#define ECHO            7

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "tcpip.h"

/* 通讯数据存入文件开关*/
/*
#define TEST
*/

#ifdef TEST
#include <sys/types.h>
#include <sys/stat.h>
char sLogName[] = "tcpip.log";
#endif

#ifdef DOS

#ifdef __TURBOC__
extern unsigned _stklen = 8096;
#endif

#endif

#ifndef INADDR_NONE
#define INADDR_NONE     0xffffffff
#endif  /* INADDR_NONE */

#define UNIXEPOCH       2208988800      /* UNIX epoch, in UCT secs      */
#define UDPTIMEMSG      "what time is it?\n"

/*
# define        MAX(x,y)        ((x) < (y) ? (y) : (x))
# define        MIN(x,y)        ((x) > (y) ? (y) : (x))
*/


#define TYPE_INT 1
#define TYPE_LONG 2
#define TYPE_SHORT 3
#define TYPE_FLOAT 4
#define TYPE_DOUBLE 5
#define TYPE_LONGDOUBLE 6
#define TYPE_CHAR 11
#define TYPE_STRING 12
#define TYPE_TABLE 13
#define TYPE_NULL -1

char sTcpipErrInfo[81];
int nLostSocket;
int nWaitingTime = MAX_WAITING_TIME;

char sSysErrInfo[81] ="";

#if(defined(DOS)||defined(UNIX))

/*------------------------------------------------------------------------
 * connectTCP - connect to a specified TCP service on a specified host
 *------------------------------------------------------------------------
 */
int connectTCP(const char *host, const char *service )
/*
 * Arguments:
 *      host    - name of host to which connection is desired
 *      service - service associated with the desired port
 */
{
  return connectsock( host, service, "tcp");
}

#endif

#ifdef WINDOWS
SOCKET connectTCP(const char *host, const char *service )
{
  return connectsock( host, service, "tcp");
}
#endif

#if(defined(DOS)||defined(UNIX))

/*------------------------------------------------------------------------
 * connectUDP - connect to a specified UDP service on a specified host
 *------------------------------------------------------------------------
 */
int connectUDP(const char *host, const char *service )
/*
 * Arguments:
 *      host    - name of host to which connection is desired
 *      service - service associated with the desired port
 */
{
  return connectsock(host, service, "udp");
}

#endif

#ifdef WINDOWS
SOCKET connectUDP(const char *host, const char *service )
{
  return connectsock(host, service, "udp");
}
#endif

#if(defined(DOS)||defined(UNIX))

/*------------------------------------------------------------------------
 * bindconnectTCP - bind a local specified TCP service and connect to a specified TCP service on a specified host
 *------------------------------------------------------------------------
 */
int bindconnectTCP(const char* localservice, const char *host, const char *service )
/*
 * Arguments:
 *      localservice - service associated with the local desired port
 *      host    - name of host to which connection is desired
 *      service - service associated with the desired port
 */
{
  return bindconnectsock(localservice, host, service, "tcp");
}

#endif

#ifdef WINDOWS
SOCKET bindconnectTCP(const char* localservice, const char *host, const char *service )
{
  return bindconnectsock(localservice, host, service, "tcp");
}
#endif

/*------------------------------------------------------------------------
 * connectsock - allocate & connect a socket using TCP or UDP
 *------------------------------------------------------------------------
 */

#ifdef UNIX
int connectsock(const char *host, const char *service, const char *transport )
/*
 * Arguments:
 *      host      - name of host to which connection is desired
 *      service   - service associated with the desired port
 *      transport - name of transport protocol to use ("tcp" or "udp")
 */
{
  struct hostent  *phe;   /* pointer to host information entry    */
  struct servent  *pse;   /* pointer to service information entry */
  struct protoent *ppe;   /* pointer to protocol information entry*/
  struct sockaddr_in sin; /* an Internet endpoint address         */
  int     s, type;        /* socket descriptor and socket type    */

  struct linger       stTimeOut;              /* Linger time(out) structure */ 

/*
#ifdef  __WATCOMC__
  stdout->_bufsize = 1;
  stderr->_bufsize = 1;
#endif
*/
  
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  #ifdef IBMUNIX
  sin.sin_len = sizeof(sin);
  #endif

  /* Map service name to port number */
  if ( pse = getservbyname(service, transport) )
    sin.sin_port = pse->s_port;
  else if ( (sin.sin_port = htons((u_short)atoi(service))) == 0 )
  {
    sprintf(sTcpipErrInfo,"不能得到\"%s\"端口号\n", service);
    return -1;
  }

  /* Map host name to IP address, allowing for dotted decimal */
  if ( phe = gethostbyname(host) )
    memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
  else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
  {
    sprintf(sTcpipErrInfo,"不能得到\"%s\"IP地址\n", host);
    return -1;
  }

  /* Map transport protocol name to protocol number */
  if ( (ppe = getprotobyname(transport)) == 0)
  {
    sprintf(sTcpipErrInfo,"can't get \"%s\" protocol entry\n", transport);
    return -1;
  }

  /* Use protocol to choose a socket type */
  if (strcmp(transport, "udp") == 0)
    type = SOCK_DGRAM;
  else
    type = SOCK_STREAM;

  /* Allocate a socket */
  s = socket(PF_INET, type, ppe->p_proto);
  if (s < 0)
  {
    sprintf(sTcpipErrInfo,"can't create socket: %s\n", strerror(errno));
    return -1;
  }

  {
    int a=1;
    if(setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *) &a, sizeof(a)))
    {
      sprintf(sTcpipErrInfo,"setsockopt TCP_NODELAY: %s\n", strerror(errno));
      return -1;
    }
  }

  /* Connect the socket */
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    sprintf(sTcpipErrInfo,"不能连接到%s.%s: %s\n",host,service,strerror(errno));
    return -1;
  }

  /* Can't Count on a Graceful Close since no Application Protocol */
  /*  to tell the server when to end ...so timeout 0 to send reset! */
  stTimeOut.l_onoff = 1;   /* Linger On */
  stTimeOut.l_linger = 0;  /* 0 Seconds */
  if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &stTimeOut, sizeof(stTimeOut)))
  {
//    sprintf(sTcpipErrInfo,"setsockopt SO_LINGER: %s\n", strerror(errno));
//    return -1;
  }

  return s;
}
#endif

#ifdef DOS
int connectsock(const char *host, const char *service, const char *transport )
/*
 * Arguments:
 *      host      - name of host to which connection is desired
 *      service   - service associated with the desired port
 *      transport - name of transport protocol to use ("tcp" or "udp")
 */
{
  struct hostent  *phe;   /* pointer to host information entry    */
  struct servent  *pse;   /* pointer to service information entry */
  struct protoent *ppe;   /* pointer to protocol information entry*/
  struct sockaddr_in sin; /* an Internet endpoint address         */
  struct linger       stTimeOut;              /* Linger time(out) structure */ 

  int     s, type;        /* socket descriptor and socket type    */

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;

  /* Map service name to port number */
  if ( pse = getservbyname(service, transport) )
    sin.sin_port = pse->s_port;
  else if ( (sin.sin_port = htons((u_short)atoi(service))) == 0 )
  {
    sprintf(sTcpipErrInfo,"不能得到\"%s\"端口号\n", service);
    return -1;
  }

  /* Map host name to IP address, allowing for dotted decimal */
  if ( phe = gethostbyname(host) )
    memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
  else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
  {
    sprintf(sTcpipErrInfo,"不能得到\"%s\"IP地址\n", host);
    return -1;
  }

  /* Map transport protocol name to protocol number */
  if ( (ppe = getprotobyname(transport)) == 0)
  {
    sprintf(sTcpipErrInfo,"can't get \"%s\" protocol entry\n", transport);
    return -1;
  }

  /* Use protocol to choose a socket type */
  if (strcmp(transport, "udp") == 0)
    type = SOCK_DGRAM;
  else
    type = SOCK_STREAM;

  /* Allocate a socket */
  s = socket(PF_INET, type, ppe->p_proto);
  if (s < 0)
  {
    sprintf(sTcpipErrInfo,"can't create socket: %s\n", strerror(errno));
    return -1;
  }

  {
    int a=1;
    if(setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *) &a, sizeof(a)))
    {
      sprintf(sTcpipErrInfo,"setsockopt TCP_NODELAY: %s\n", strerror(errno));
      return -1;
    }
  }

  /* Connect the socket */
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    sprintf(sTcpipErrInfo,"不能连接到%s.%s: %s\n",host,service,strerror(errno));
    return -1;
  }

  /* Can't Count on a Graceful Close since no Application Protocol */
  /*  to tell the server when to end ...so timeout 0 to send reset! */
  stTimeOut.l_onoff = 1;   /* Linger On */
  stTimeOut.l_linger = 0;  /* 0 Seconds */
  if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &stTimeOut, sizeof(stTimeOut)))
  {
    sprintf(sTcpipErrInfo,"setsockopt SO_LINGER: %s\n", strerror(errno));
    return -1;
  }

  return s;
}
#endif

#ifdef WINDOWS
SOCKET connectsock(const char *host, const char *service, const char *transport )
{
  struct hostent  *phe;   /* pointer to host information entry    */
  struct servent  *pse;   /* pointer to service information entry */
  struct protoent *ppe;   /* pointer to protocol information entry*/
  struct sockaddr_in sin; /* an Internet endpoint address         */
  int     s, type;        /* socket descriptor and socket type    */


  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;

  /* Map service name to port number */
  if ( pse = getservbyname(service, transport) )
    sin.sin_port = pse->s_port;
  else if ( (sin.sin_port = htons((u_short)atoi(service))) == 0 )
    errexit("can't get \"%s\" service entry\n", service);

  /* Map host name to IP address, allowing for dotted decimal */
  if ( phe = gethostbyname(host) )
    memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
  else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
    errexit("can't get \"%s\" host entry\n", host);

  /* Map protocol name to protocol number */
  if ( (ppe = getprotobyname(transport)) == 0)
    errexit("can't get \"%s\" protocol entry\n", transport);
  /* Use protocol to choose a socket type */
  if (strcmp(transport, "udp") == 0)
    type = SOCK_DGRAM;
  else
    type = SOCK_STREAM;

  /* Allocate a socket */
  s = socket(PF_INET, type, ppe->p_proto);
  if (s == INVALID_SOCKET)
    errexit("can't create socket: %d\n", GetLastError());

  /* Connect the socket */
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
    errexit("can't connect to %s.%s: %d\n", host, service, GetLastError());

  return s;
}
#endif

/*------------------------------------------------------------------------
 * bindconnectsock - allocate & bind & connect a socket using TCP or UDP
 *------------------------------------------------------------------------
 */

#ifdef UNIX
int bindconnectsock(const char* localservice, const char *host, const char *service, const char *transport)
/*
 * Arguments:
 *      localservice   - service associated with the local desired port
 *      host      - name of host to which connection is desired
 *      service   - service associated with the desired port
 *      transport - name of transport protocol to use ("tcp" or "udp")
 */
{
  struct hostent  *phe;   /* pointer to host information entry    */
  struct servent  *pse;   /* pointer to service information entry */
  struct protoent *ppe;   /* pointer to protocol information entry*/
  struct sockaddr_in sin; /* an Internet endpoint address         */
  struct sockaddr_in lsin; /* an local Internet endpoint address         */
  int     s, type;        /* socket descriptor and socket type    */
  struct linger stTimeOut;
/*
#ifdef  __WATCOMC__
  stdout->_bufsize = 1;
  stderr->_bufsize = 1;
#endif
*/

  memset(&lsin, 0, sizeof(lsin));
  lsin.sin_family = AF_INET;
  #ifdef IBMUNIX
  lsin.sin_len = sizeof(lsin);
  #endif

  lsin.sin_addr.s_addr = INADDR_ANY;

  /* Map service name to port number */
  if ( pse = getservbyname(localservice, transport) )
    lsin.sin_port = htons(ntohs((u_short)pse->s_port));
  else if ( (lsin.sin_port = htons((u_short)atoi(localservice))) == 0 )
  {
    sprintf(sTcpipErrInfo,"不能得到\"%s\"端口号\n", localservice);
    return -1;
  }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  #ifdef IBMUNIX
  sin.sin_len = sizeof(sin);
  #endif

  /* Map service name to port number */
  if ( pse = getservbyname(service, transport) )
    sin.sin_port = pse->s_port;
  else if ( (sin.sin_port = htons((u_short)atoi(service))) == 0 )
  {
    sprintf(sTcpipErrInfo,"不能得到\"%s\"端口号\n", service);
    return -1;
  }

  /* Map host name to IP address, allowing for dotted decimal */
  if ( phe = gethostbyname(host) )
    memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
  else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
  {
    sprintf(sTcpipErrInfo,"不能得到\"%s\"IP地址\n", host);
    return -1;
  }

  /* Map transport protocol name to protocol number */
  if ( (ppe = getprotobyname(transport)) == 0)
  {
    sprintf(sTcpipErrInfo,"can't get \"%s\" protocol entry\n", transport);
    return -1;
  }

  /* Use protocol to choose a socket type */
  if (strcmp(transport, "udp") == 0)
    type = SOCK_DGRAM;
  else
    type = SOCK_STREAM;

  /* Allocate a socket */
  s = socket(PF_INET, type, ppe->p_proto);
  if (s < 0)
  {
    sprintf(sTcpipErrInfo,"can't create socket: %s\n", strerror(errno));
    return -1;
  }

  {
    int a=1;
    if(setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *) &a, sizeof(a)))
    {
      sprintf(sTcpipErrInfo,"setsockopt TCP_NODELAY: %s\n", strerror(errno));
      return -1;
    }
  }


  /* Bind the socket */
  if (bind(s, (struct sockaddr *)&lsin, sizeof(lsin)) < 0)
  {
    sprintf(sTcpipErrInfo,"can't bind to %s port: %s\n", localservice, strerror(errno));
    return -1;
  }

  /* Connect the socket */
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    sprintf(sTcpipErrInfo,"不能连接到%s.%s: %s\n",host,service,strerror(errno));
    return -1;
  }

  /* Can't Count on a Graceful Close since no Application Protocol */
  /*  to tell the server when to end ...so timeout 0 to send reset! */
  stTimeOut.l_onoff = 1;   /* Linger On */
  stTimeOut.l_linger = 0;  /* 0 Seconds */
  if (setsockopt(s,SOL_SOCKET,SO_LINGER,(char *) &stTimeOut, sizeof(stTimeOut)))
    sprintf(sTcpipErrInfo,"setsockopt SO_LINGER: %s\n", strerror(errno));

  return s;
}
#endif

#ifdef DOS
int bindconnectsock(const char* localservice, const char *host, const char *service, const char *transport)
/*
 * Arguments:
 *      localservice   - service associated with the local desired port
 *      host      - name of host to which connection is desired
 *      service   - service associated with the desired port
 *      transport - name of transport protocol to use ("tcp" or "udp")
 */
{
  struct hostent  *phe;   /* pointer to host information entry    */
  struct servent  *pse;   /* pointer to service information entry */
  struct protoent *ppe;   /* pointer to protocol information entry*/
  struct sockaddr_in sin; /* an Internet endpoint address         */
  struct sockaddr_in lsin; /* an local Internet endpoint address         */
  struct linger       stTimeOut;              /* Linger time(out) structure */ 

  int     s, type;        /* socket descriptor and socket type    */

  memset(&lsin, 0, sizeof(lsin));
  lsin.sin_family = AF_INET;
  lsin.sin_addr.s_addr = INADDR_ANY;

  /* Map service name to port number */
  if ( pse = getservbyname(localservice, transport) )
    lsin.sin_port = htons(ntohs((u_short)pse->s_port));
  else if ( (lsin.sin_port = htons((u_short)atoi(localservice))) == 0 )
  {
    sprintf(sTcpipErrInfo,"不能得到\"%s\"端口号\n", localservice);
    return -1;
  }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;

  /* Map service name to port number */
  if ( pse = getservbyname(service, transport) )
    sin.sin_port = pse->s_port;
  else if ( (sin.sin_port = htons((u_short)atoi(service))) == 0 )
  {
    sprintf(sTcpipErrInfo,"不能得到\"%s\"端口号\n", service);
    return -1;
  }

  /* Map host name to IP address, allowing for dotted decimal */
  if ( phe = gethostbyname(host) )
    memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
  else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
  {
    sprintf(sTcpipErrInfo,"不能得到\"%s\"IP地址\n", host);
    return -1;
  }

  /* Map transport protocol name to protocol number */
  if ( (ppe = getprotobyname(transport)) == 0)
  {
    sprintf(sTcpipErrInfo,"can't get \"%s\" protocol entry\n", transport);
    return -1;
  }

  /* Use protocol to choose a socket type */
  if (strcmp(transport, "udp") == 0)
    type = SOCK_DGRAM;
  else
    type = SOCK_STREAM;

  /* Allocate a socket */
  s = socket(PF_INET, type, ppe->p_proto);
  if (s < 0)
  {
    sprintf(sTcpipErrInfo,"can't create socket: %s\n", strerror(errno));
    return -1;
  }

  {
    int a=1;
    if(setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *) &a, sizeof(a)))
    {
      sprintf(sTcpipErrInfo,"setsockopt TCP_NODELAY: %s\n", strerror(errno));
      return -1;
    }
  }


  /* Bind the socket */
  if (bind(s, (struct sockaddr *)&lsin, sizeof(lsin)) < 0)
  {
    sprintf(sTcpipErrInfo,"can't bind to %s port: %s\n", localservice, strerror(errno));
    return -1;
  }

  /* Connect the socket */
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    sprintf(sTcpipErrInfo,"不能连接到%s.%s: %s\n",host,service,strerror(errno));
    return -1;
  }

  /* Can't Count on a Graceful Close since no Application Protocol */
  /*  to tell the server when to end ...so timeout 0 to send reset! */
  stTimeOut.l_onoff = 1;   /* Linger On */
  stTimeOut.l_linger = 0;  /* 0 Seconds */
  if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &stTimeOut, sizeof(stTimeOut)))
  {
    sprintf(sTcpipErrInfo,"setsockopt SO_LINGER: %s\n", strerror(errno));
    return -1;
  }

  return s;
}
#endif

#ifdef WINDOWS
SOCKET bindconnectsock(const char* localservice, const char *host, const char *service, const char *transport)
{
  struct hostent  *phe;   /* pointer to host information entry    */
  struct servent  *pse;   /* pointer to service information entry */
  struct protoent *ppe;   /* pointer to protocol information entry*/
  struct sockaddr_in sin; /* an Internet endpoint address         */
  struct sockaddr_in lsin; /* an local Internet endpoint address         */
  int     s, type;        /* socket descriptor and socket type    */

  memset(&lsin, 0, sizeof(lsin));
  lsin.sin_family = AF_INET;
  lsin.sin_addr.s_addr = INADDR_ANY;

  /* Map service name to port number */
  if ( pse = getservbyname(localservice, transport) )
    lsin.sin_port = htons(ntohs((u_short)pse->s_port));
  else if ( (lsin.sin_port = htons((u_short)atoi(localservice))) == 0 )
    errexit("can't get \"%s\" service entry\n", localservice);

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;

  /* Map service name to port number */
  if ( pse = getservbyname(service, transport) )
    sin.sin_port = pse->s_port;
  else if ( (sin.sin_port = htons((u_short)atoi(service))) == 0 )
    errexit("can't get \"%s\" service entry\n", service);

  /* Map host name to IP address, allowing for dotted decimal */
  if ( phe = gethostbyname(host) )
    memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
  else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
    errexit("can't get \"%s\" host entry\n", host);

  /* Map protocol name to protocol number */
  if ( (ppe = getprotobyname(transport)) == 0)
    errexit("can't get \"%s\" protocol entry\n", transport);
  /* Use protocol to choose a socket type */
  if (strcmp(transport, "udp") == 0)
    type = SOCK_DGRAM;
  else
    type = SOCK_STREAM;

  /* Allocate a socket */
  s = socket(PF_INET, type, ppe->p_proto);
  if (s == INVALID_SOCKET)
    errexit("can't create socket: %d\n", GetLastError());

  /* Bind the socket */
  if (bind(s, (struct sockaddr *)&lsin, sizeof(lsin)) == SOCKET_ERROR)
    errexit("can't bind to %s port: %d\n", localservice, GetLastError());

  /* Connect the socket */
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
    errexit("can't connect to %s.%s: %d\n", host, service, GetLastError());

  return s;
}
#endif

/*------------------------------------------------------------------------
 * errexit - print an error message and exit
 *------------------------------------------------------------------------
 */
/*VARARGS1*/
int errexit(const char *format, ...)
{
  va_list args;

  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  #ifdef WINDOWS
  WSACleanup();
  #endif
  exit(1);
}

// 测试给定主机是否可以ping通
// 返回值 1 成功 0 失败
#ifdef UNIX
int pingtest(const char *host)
{
  FILE* fp;
  char buf[201];
  
  #ifdef HPUNIX
  sprintf(buf, "ping %s -n 100", host);
  #else
  sprintf(buf, "ping -c 100 %s", host);
  #endif
  fp = popen(buf, "r");
  if(fp != NULL)
  {
    while(fgets(buf, 201, fp) != NULL)
    {
       if(strstr(buf, " from "))
       {
         pclose(fp);
         return 1;
       }
       else if(strstr(buf, "ret=-1"))
       {
         sprintf(sTcpipErrInfo,"不能得到远方主机服务(%s)\n",host);
         pclose(fp);
         return 0;
       }
    }
    pclose(fp);
    sprintf(sTcpipErrInfo,"时间超出,不能得到远方主机服务(%s)\n",host);
  }
  else
  {
    sprintf(sTcpipErrInfo,"不能执行与远方主机的连通测试(%s)\n",host);
  }
  return 0;
}
#endif


#ifdef USELESS

#ifdef UNIX
time_t UDPtime(const char *host)
{
  time_t  now;                    /* 32-bit integer to hold time  */ 
  int     s, n;                   /* socket descriptor, read count*/

  s = connectUDP(host, "time");

  (void) write(s, UDPTIMEMSG, strlen(UDPTIMEMSG));

  /* Read the time */

  n = read(s, (char *)&now, sizeof(now));
  if (n < 0)
    return 0;
  now = ntohl((u_long)now);       /* put in host byte order       */
  now -= UNIXEPOCH;               /* convert UCT to UNIX epoch    */
  return now;
}
#endif

#ifdef WINDOWS
time_t UDPtime(const char *host)
{
  time_t  now;                    /* 32-bit integer to hold time  */
  SOCKET  s;                      /* socket descriptor            */
  int     n;                      /* recv count                   */
  WSADATA wsadata;

  if (WSAStartup(WSVERS, &wsadata))
    errexit("WSAStartup failed\n");

  s = connectUDP(host, "time");

  (void) send(s, MSG, strlen(MSG), 0);

  /* Read the time */

  n = recv(s, (char *)&now, sizeof(now), 0);
  if (n == SOCKET_ERROR)
    errexit("recv failed: recv() error %d\n", GetLastError());
  WSACleanup();
  now = ntohl((u_long)now);       /* put in host byte order       */
  now -= WINEPOCH;                /* convert UCT to Windows epoch */
  return now;
}
#endif

int pingsock;
int pingtime;

#ifdef UNIX
void TCPgettime();
time_t TCPtime(const char *host)
{
  time_t  now;                    /* 32-bit integer to hold time  */ 
  int     n,inchars;           /* socket descriptor, read count*/
  int nSuccess;

  pingtime = 0;
          
  sigset(SIGALRM, TCPgettime);
  alarm(10);

  pingtime++;
  
  for(;;)
  {
    pingsock = connectTCP(host, "time");
    if(pingsock<0)
    {
      if(pingtime>12)
      {
        alarm(0);
        sprintf(sTcpipErrInfo,"时间超出,不能得到远方主机服务\n");
        return 0;
      }
      else
        continue;
    }
    nSuccess = 1;
    /* Read the time */
    for (inchars = 0; inchars < sizeof(now); inchars+=n ) 
    {
      n = read(pingsock, (char*)&now+inchars, sizeof(now) - inchars);
      if (n <= 0)
      {
      	nSuccess = 0;
      	break;
      }
    }
    if(nSuccess == 1)
    {
    	alarm(0);

       /* Read the time */
        now = ntohl((u_long)now);       /* put in host byte order       */
        now -= UNIXEPOCH;               /* convert UCT to UNIX epoch    */
    	return now;
    }
  }
}

void TCPgettime() 
{
  shutdown(pingsock,2);
  pingtime++;
  alarm(10);
}
#endif

#ifdef USELESS
time_t TCPtime(const char *host)
{
  time_t  now;                    /* 32-bit integer to hold time  */ 
  int     s, n,inchars;           /* socket descriptor, read count*/
          
  s = connectTCP(host, "time");

  /* Read the time */
  for (inchars = 0; inchars < sizeof(now); inchars+=n ) 
  {
    n = read(s, (char*)&now+inchars, sizeof(now) - inchars);
    if (n <= 0)
      return 0;
  }

  now = ntohl((u_long)now);       /* put in host byte order       */
  now -= UNIXEPOCH;               /* convert UCT to UNIX epoch    */
  return now;
}
#endif

/* ping: send packets to the echo port and wait for them to come back.
 * Set an interval timer to send another packet every 1 seconds.  
 *
 */

#ifdef UNIX

PINGSTAT pingstat;
int pingsock;
int pingtime;
PINGSTAT ping(const char * host)
{
  char buf[255];
  int  seq, timeout=100, i, namelen,time,oldtime;

  pingstat.sent=0;
  pingstat.recvd=0;
  pingstat.min=0.0;
  pingstat.max=0.0;
  pingstat.total=0;
  pingtime=0;

  pingsock = connectUDP(host, "echo");

  oldtime=time_now(); 
  sigset(SIGALRM, UDPping);
  alarm(1);
  UDPping();
  for ( ;; )
  {
    register size, delta;

    size = read(pingsock, buf, sizeof(buf));
    if (size == -1  &&  errno == EINTR)
      continue;
    pingstat.recvd++;
    sscanf(buf, "Sequence: %d, %d", &seq, &time);
    delta = time_now() - time;
    pingstat.max = MAX(delta, pingstat.max);
    pingstat.min = pingstat.min ? MIN(delta, pingstat.min) : delta;
    pingstat.total += delta;
    if(pingtime>=20)
      break;
  }
  return pingstat;
}

#endif

#ifdef UNIX
void UDPping() 
{
  char buf[200];

  sprintf(buf, "Sequence: %03d, %d", pingstat.sent++, time_now());
  if (write(pingsock, buf, strlen(buf) + 1) < 0)
  {
    perror("send failed");
    exit(6);
  }
  pingtime++;
  alarm(1);
}

#endif

#ifdef UNIX
void show_ping_stat(PINGSTAT pingstat) 
{
  outtext("ping: %d sent, %d received, %d%% lost, ", 
  pingstat.sent, pingstat.recvd, 100-(100*pingstat.recvd/pingstat.sent)); 
  if (pingstat.recvd)
    outtext("min/ave/max: %.2lf/%.2lf/%.2lf seconds.\n", pingstat.min/1000.0,
      pingstat.total/pingstat.recvd/1000.0, pingstat.max/1000.0);
  else
    outtext(" (dead maybe?)\n");
}

#endif

#ifdef UNIX
static long time_now() 
{
  struct timeval now;

  gettimeofday(&now, 0);
  return now.tv_sec * 1000 + now.tv_usec / 1000;
}

#endif


#endif // USELESS

#ifdef UNIX

int passivesock(const char *service, const char *transport, int qlen);

/*------------------------------------------------------------------------
 * passiveUDP - create a passive socket for use in a UDP server
 *------------------------------------------------------------------------
 */
int passiveUDP(const char *service)
/*
 * Arguments:
 *      service - service associated with the desired port
 */
{
  return passivesock(service, "udp", 0);
}

#endif

#ifdef WINDOWS
SOCKET passiveUDP(const char *service)
{
  return passivesock(service, "udp", 0);
}
#endif

/*------------------------------------------------------------------------
 * passiveTCP - create a passive socket for use in a TCP server
 *------------------------------------------------------------------------
 */

#ifdef UNIX

int passiveTCP(const char *service, int qlen)
/*
 * Arguments:
 *      service - service associated with the desired port
 *      qlen    - maximum server request queue length
 */
{
  return passivesock(service, "tcp", qlen);
}

#endif

#ifdef WINDOWS
SOCKET passiveTCP(const char *service, int qlen)
{
  return passivesock(service, "tcp", qlen);
}
#endif

#ifdef UNIX

u_short portbase = 0;           /* port base, for non-root servers      */

/*------------------------------------------------------------------------
 * passivesock - allocate & bind a server socket using TCP or UDP
 *------------------------------------------------------------------------
 */
int passivesock(const char *service, const char *transport, int qlen)
/*
 * Arguments:
 *      service   - service associated with the desired port
 *      transport - transport protocol to use ("tcp" or "udp")
 *      qlen      - maximum server request queue length
 */
{
  struct servent  *pse;   /* pointer to service information entry */
  struct protoent *ppe;   /* pointer to protocol information entry*/
  struct sockaddr_in sin; /* an Internet endpoint address         */
  int     s, type;        /* socket descriptor and socket type    */
  struct linger       stTimeOut;              /* Linger time(out) structure */ 


  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  #ifdef IBMUNIX
  sin.sin_len = sizeof(sin);
  #endif
  sin.sin_addr.s_addr = INADDR_ANY;

  /* Map service name to port number */
  if ( pse = getservbyname(service, transport) )
    sin.sin_port = htons(ntohs((u_short)pse->s_port) + portbase);
  else if ( (sin.sin_port = htons((u_short)atoi(service))) == 0 )
    errexit("can't get \"%s\" service entry\n", service);

  /* Map protocol name to protocol number */
  if ( (ppe = getprotobyname(transport)) == 0)
    errexit("can't get \"%s\" protocol entry\n", transport);

  /* Use protocol to choose a socket type */
  if (strcmp(transport, "udp") == 0)
    type = SOCK_DGRAM;
  else
    type = SOCK_STREAM;

  /* Allocate a socket */
  s = socket(PF_INET, type, ppe->p_proto);
  if (s < 0)
    errexit("can't create socket: %s\n", strerror(errno));

  {
    int a=1;
    if(setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *) &a, sizeof(a)))
    {
      sprintf(sTcpipErrInfo,"setsockopt TCP_NODELAY: %s\n", strerror(errno));
      return -1;
    }
  }

  stTimeOut.l_onoff = 1;   /* Linger On */
  stTimeOut.l_linger = 0;  /* 0 Seconds */
  setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &stTimeOut, sizeof(stTimeOut));

  /* Bind the socket */
  if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    errexit("can't bind to %s port: %s\n", service, strerror(errno));
  if (type == SOCK_STREAM && listen(s, qlen) < 0)
    errexit("can't listen on %s port: %s\n", service, strerror(errno));
  return s;
}

#endif

#ifdef WINDOWS
SOCKET passivesock(const char *service, const char *transport, int qlen)
{
  struct servent  *pse;   /* pointer to service information entry */
  struct protoent *ppe;   /* pointer to protocol information entry*/
  struct sockaddr_in sin; /* an Internet endpoint address         */
  SOCKET          s;      /* socket descriptor                    */
  int             type;   /* socket type (SOCK_STREAM, SOCK_DGRAM)*/

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;

  /* Map service name to port number */
  if ( pse = getservbyname(service, transport) )
    sin.sin_port = htons(ntohs((u_short)pse->s_port) + portbase);
  else if ( (sin.sin_port = htons((u_short)atoi(service))) == 0 )
    errexit("can't get \"%s\" service entry\n", service);

  /* Map protocol name to protocol number */
  if ( (ppe = getprotobyname(transport)) == 0)
    errexit("can't get \"%s\" protocol entry\n", transport);

  /* Use protocol to choose a socket type */
  if (strcmp(transport, "udp") == 0)
    type = SOCK_DGRAM;
  else
    type = SOCK_STREAM;

  /* Allocate a socket */
  s = socket(PF_INET, type, ppe->p_proto);
  if (s == INVALID_SOCKET)
    errexit("can't create socket: %d\n", GetLastError());

  /* Bind the socket */
  if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
    errexit("can't bind to %s port: %d\n", service, GetLastError());
  if (type == SOCK_STREAM && listen(s, qlen) == SOCKET_ERROR)
    errexit("can't listen on %s port: %d\n", service, GetLastError());
  return s;
}
#endif

int GetStringFromSocket(const int socket,char *sValue)
{
  int     n;
  long nLen;

  sValue[0] = '\0';        
  if(GetLongFromSocket(socket,&nLen))
    return 1;
      
  n = ReadSocket(socket, sValue, nLen);
  if (n < nLen)
    return 1;

  sValue[nLen]='\0';

  #ifdef TEST
  {
    char sText[201];
    snprintf(sText, sizeof(sText), "GetStringFromSocket: [%s]\n", sValue);
    WriteLog(sLogName, sText);
  }
  #endif

  return 0;
}

int GetLongFromSocket(const int socket,long* pnValue)
{
  int     n;

  *pnValue = 0L;        

  n = ReadSocket(socket, (char*)pnValue, 4);
  if (n < 4)
      return 1;
  
/* put in host byte order       */
/*
  *pnValue = ntohl((u_long)*pnValue); 
*/

  #ifdef LITTLEEND
  *pnValue = ReverseLong2(*pnValue);
  #endif

  #ifdef TEST
  {
    char sText[51];
    snprintf(sText, sizeof(sText), "GetLongFromSocket: [%ld]\n", *pnValue);
    WriteLog(sLogName, sText);
  }
  #endif

  return 0;
}

int GetDoubleFromSocket(const int socket,double* pdValue)
{
  int     n;

  *pdValue = 0.0;        

  n = ReadSocket(socket, (char*)pdValue, sizeof(double));
  if (n < sizeof(double))
    return 1;

  #ifdef LITTLEEND
  *pdValue = ReverseDouble(*pdValue);
  #endif

  #ifdef TEST
  {
    char sText[51];
    snprintf(sText, sizeof(sText), "GetDoubleFromSocket: [%lf]\n", *pdValue);
    WriteLog(sLogName, sText);
  }
  #endif

  return 0;
}

int SendStringToSocket(const int socket,char * sValue)
{
  int outchars,n;
  char sOut[4008];
  char* s;
  char* s1;
  int nOutLen;
  int i;
  long nLen;

  s = sOut;
  nOutLen = 0;

  nLen=strlen(sValue);
  if(nLen > 4000)
  {
    if(SendLongToSocket(socket,(long)nLen))
      return 1;

    #ifdef TEST
    {
      char sText[201];
      snprintf(sText, sizeof(sText), "SendStringToSocket: [%s]\n", sValue);
      WriteLog(sLogName, sText);
    }
    #endif

    nLostSocket = socket;

    #ifdef UNIX
    sigset(SIGALRM, SocketLoseControl);
    alarm(nWaitingTime);
    #endif

    for (outchars = 0; outchars < nLen; outchars+=n ) 
    {
      n=write(socket, sValue+outchars, nLen-outchars);
      if(n<0)
        return 1;
    }

    #ifdef UNIX
    alarm(0);
    #endif
  }
  else
  {
    #ifdef LITTLEEND
    nLen = ReverseLong1(nLen);
    #endif
    s1=(char*)&nLen;
    for(i=0;i<4;i++)
    {
      s[i]=s1[i];
    }
    s+=4;
    nOutLen+=4;

    strcpy(s,sValue);
    nLen=strlen(sValue);
    nOutLen+=nLen;

    nLostSocket = socket;

    #ifdef UNIX
    sigset(SIGALRM, SocketLoseControl);
    alarm(nWaitingTime);
    #endif

    for (outchars = 0; outchars < nOutLen; outchars+=n ) 
    {
      n=write(socket, sOut+outchars, nOutLen-outchars);
      if(n<0)
        return 1;
    }

    #ifdef UNIX
    alarm(0);
    #endif
  }

  return 0;
}

int SendRecordToSocket(const int socket,char * sValue,int nLen)
{
  int outchars,n;

  nLostSocket = socket;

  #ifdef UNIX
  sigset(SIGALRM, SocketLoseControl);
  alarm(nWaitingTime);
  #endif

  for (outchars = 0; outchars < nLen; outchars+=n ) 
  {
    n=write(socket, sValue+outchars, nLen-outchars);
    if(n<0)
      return 1;
  }

  #ifdef UNIX
  alarm(0);
  #endif

  return 0;
}

// cEndChar 结束符 寻呼为'#'
int GetRecordFromSocket(const int socket, char* sValue, const char cEndChar)
{
  int     n,inchars;              /* read count*/
        
  /* Read enough chars */
  for (inchars = 0; ; inchars+=n ) 
  {
    n = ReadSocket(socket, sValue+inchars, 1);
    if (n <= 0)
      return 1;
    if(*(sValue + inchars + n - 1) == cEndChar)
      break;
  }

  return 0;
}

// 取定长记录
// nLen 最大接收长度
// 返回值 >=0 实际接收长度 -1出错 
int GetOneRecordFromSocket(const int socket, char* sValue, const int nLen)
{
  int     n;

  n = ReadSocket(socket, sValue, nLen);
  if (n < 0)
    return -1;

  return n;
}

int SendLongToSocket(const int socket,long nValue)
{
  int outchars,nLen,n;

  #ifdef TEST
  {
    char sText[51];
    snprintf(sText, sizeof(sText), "SendLongToSocket: [%ld]\n", nValue);
    WriteLog(sLogName, sText);
  }
  #endif

/*
  nValue=htonl((u_long)nValue);
*/
 
  nLen=4; 

  nLostSocket = socket;

  #ifdef UNIX
  sigset(SIGALRM, SocketLoseControl);
  alarm(nWaitingTime);
  #endif

  #ifdef LITTLEEND
  nValue = ReverseLong1(nValue);
  #endif

  for (outchars = 0; outchars < nLen; outchars+=n ) 
  {
    n=write(socket, (char*)&nValue+outchars, nLen-outchars);
    if(n<0)
      return 1;
  }

  #ifdef UNIX
  alarm(0);
  #endif

  return 0;
}

int SendDoubleToSocket(const int socket,double dValue)
{
  int outchars,nLen,n;

  #ifdef TEST
  {
    char sText[51];
    snprintf(sText, sizeof(sText), "SendDoubleToSocket: [%lf]\n", dValue);
    WriteLog(sLogName, sText);
  }
  #endif

  nLen=sizeof(dValue); 

  nLostSocket = socket;

  #ifdef UNIX
  sigset(SIGALRM, SocketLoseControl);
  alarm(nWaitingTime);
  #endif

  #ifdef LITTLEEND
  dValue = ReverseDouble(dValue);
  #endif

  for (outchars = 0; outchars < nLen; outchars+=n ) 
  {
    n=write(socket, (char*)&dValue+outchars, nLen-outchars);
    if(n<0)
      return 1;
  }

  #ifdef UNIX
  alarm(0);
  #endif

  return 0;
}

void SetWaitingTime(int nTime)
{
  nWaitingTime = nTime;
}

// 返回值:
// 0 正常
// 1 网络连接错误
// -1 语法错误
// -2 socket非法

int SendToSocket(const int socket, char* sFormat, ...)
{ 
  long nType;/*变长列表中参数的类型*/
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  va_list pVarArgList;/*变长参数列表指针*/

  char sOut[4008];
  char* s;
  int nOutLen;
  long nVal;
  double dVal;
  char* sVal;
  char* s1;
  int i;

  if(socket<=0)
    return -2;

  sTop=sFormat;
  sBottom=sFormat;

  nType=TYPE_NULL;
  
  s = sOut;
  nOutLen = 0;

  va_start(pVarArgList,sFormat);

  while(sTop&&sTop[0])
  {
    nType=0;
    sBottom=(char*)strchr(sTop,'%');
    if(sBottom==NULL)
    {
      break;
    }
    else
      sBottom++;

    while(nType==0)
    {
      switch(sBottom[0])
      {
        case 'l':
          sBottom++;
          switch(sBottom[0])
          {
            case 'd':
              nType=TYPE_LONG;
              break;
            case 'f':
              nType=TYPE_DOUBLE;
              break;  
            default:
              fprintf(stdout,"\n\a语法错误\n");
              return -1;
              break;
          }
          break;
        case 's':
          nType=TYPE_STRING; 
          break;
        case '\0':
          nType=TYPE_NULL;
          break;
      }
      if (nType)
        break;
      if(sBottom!=NULL&&sBottom[0]!='\0')
        sBottom++;
    }

    if(nType!=TYPE_NULL)
    {
      sTop=sBottom+1;
    }
    else
      break;

    switch(nType)
    { 
      case TYPE_LONG:
        if(nOutLen >= 4000)
        {
          if(SendRecordToSocket(socket,sOut,nOutLen))
          {
            close(socket);
            return 1;
          }
          s = sOut;
          nOutLen = 0;
        }
        nVal = va_arg(pVarArgList,long);
        #ifdef LITTLEEND
        nVal = ReverseLong1(nVal);
        #endif
        s1=(char*)&nVal;
        for(i=0;i<4;i++)
        {
          s[i]=s1[i];
        }
        s+=4;
        nOutLen+=4;

        /*
        if(SendLongToSocket(socket, va_arg(pVarArgList,long)))
        {
          return 1;
        }
        */
        break;
      case TYPE_DOUBLE:
        if(nOutLen >= 4000)
        {
          if(SendRecordToSocket(socket,sOut,nOutLen))
          {
            close(socket);
            return 1;
          }
          s = sOut;
          nOutLen = 0;
        }
        dVal = va_arg(pVarArgList,double);
        #ifdef LITTLEEND
        dVal = ReverseDouble(dVal);
        #endif
        s1=(char*)&dVal;
        for(i=0;i<8;i++)
        {
          s[i]=s1[i];
        }
        s+=8;
        nOutLen+=8;

        /*
        if(SendDoubleToSocket(socket,va_arg(pVarArgList,double)))
        {
          return 1;
        }
        */
        break;
      case TYPE_STRING:
        sVal = va_arg(pVarArgList,char*);
        nVal = strlen(sVal);

        if(nOutLen + nVal >= 4000)
        {
          if(SendRecordToSocket(socket,sOut,nOutLen))
          {
            close(socket);
            return 1;
          }
          s = sOut;
          nOutLen = 0;
          if(SendStringToSocket(socket,va_arg(pVarArgList,char*)))
          {
            return 1;
          }
          break;
        }

        #ifdef LITTLEEND
        nVal = ReverseLong1(nVal);
        #endif
        s1=(char*)&nVal;
        for(i=0;i<4;i++)
        {
          s[i]=s1[i];
        }
        s+=4;
        nOutLen+=4;

        strcpy(s,sVal);
        nVal = strlen(sVal);
        s+=nVal;
        nOutLen+=nVal;


        /*
        if(SendStringToSocket(socket,va_arg(pVarArgList,char*)))
        {
          return 1;
        }
        */
        break;
    }
  }
  if(SendRecordToSocket(socket,sOut,nOutLen))
  {
    close(socket);
    return 1;
  }

  return 0;
}

// 返回值:
// 0 正常
// 1 网络连接错误
// -1 语法错误
// -2 socket非法

int GetFromSocket(const int socket, char* sFormat, ...)
{ 
  long nType;/*变长列表中参数的类型*/
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  va_list pVarArgList;/*变长参数列表指针*/
  double *pdValue;
  long *pnValue;
  char *sValue;

  if(socket<=0)
    return -2;

  sTop=sFormat;
  sBottom=sFormat;

  nType=TYPE_NULL;

  va_start(pVarArgList,sFormat);

  while(sTop&&sTop[0])
  {
    nType=0;
    sBottom=(char*)strchr(sTop,'%');
    if(sBottom==NULL)
    {
      break;
    }
    else
      sBottom++;

    while(nType==0)
    {
      switch(sBottom[0])
      {
        case 'l':
          sBottom++;
          switch(sBottom[0])
          {
            case 'd':
              nType=TYPE_LONG;
              break;
            case 'f':
              nType=TYPE_DOUBLE;
              break;  
            default:
              fprintf(stdout,"\n\a语法错误\n");
              return -1;
              break;
          }
          break;
        case 's':
          nType=TYPE_STRING; 
          break;
        case '\0':
          nType=TYPE_NULL;
          break;
      }
      if (nType)
        break;
      if(sBottom!=NULL&&sBottom[0]!='\0')
        sBottom++;
    }

    if(nType!=TYPE_NULL)
    {
      sTop=sBottom+1;
    }
    else
      break;

    switch(nType)
    { 
      case TYPE_LONG:
        pnValue=va_arg(pVarArgList,long *);
        if(GetLongFromSocket(socket, pnValue))
        {
          return 1;
        }
        break;
      case TYPE_DOUBLE:
        pdValue=va_arg(pVarArgList,double *);
        if(GetDoubleFromSocket(socket,pdValue))
        {
          return 1;
        }
        break;
      case TYPE_STRING:
        sValue=va_arg(pVarArgList,char *);
        if(GetStringFromSocket(socket,sValue))
        {
          return 1;
        }
        break;
    }
  }
  
  return 0;
}

#ifdef TEST
int WriteLog(char* sLogName, char* sText)
{
  FILE* fpLog;
  struct stat statFile;

  if(!stat(sLogName, &statFile))
  {
    if(statFile.st_size > 1048576)
    {
      remove(sLogName);
    }
  }

  if((fpLog=fopen(sLogName,"a+"))!=NULL)
  {
    fprintf(fpLog, sText);
    fclose(fpLog);
    return 1;
  }
  return 0;
}
#endif

#ifdef LITTLEEND
/*
  long64->long32->socket
*/
long ReverseLong1(long nVal)
{
  long nRetVal;
  if(nVal >= 0)
  {
    nRetVal = nVal;
  }
  else
  {
    nRetVal = 4294967296L + nVal;
  }
  // 以上用于64位编译，如果32位编译，则直接用nRetVal = nVal;

  
  return ReverseLong(nRetVal);
}

/*
  socket->long32->long64
*/
long ReverseLong2(long nVal)
{
  long nRetVal;
  
  nRetVal = ReverseLong(nVal);
  if(nRetVal > 2147483647L)
  {
    nRetVal -= 4294967296L;
  }
  // 以上用于64位编译，如果32位编译，则不需要转换;
  
  return nRetVal;
}

long ReverseLong(long nVal)
{
  char* s1;
  char* s2;
  long nRetVal = 0L;
  int i;

  s1 = (char*)&nVal + sizeof(long) - 1;
  s2 = (char*)&nRetVal;
  
  for(i = 0; i < sizeof(long); i++)
  {
    *(s2 + i) = *(s1 - i);
  }
  
  return nRetVal;
}

double ReverseDouble(double dVal)
{
  char* s1;
  char* s2;
  double dRetVal = 0.0;
  int i;

  s1 = (char*)&dVal + sizeof(double) - 1;
  s2 = (char*)&dRetVal;
  
  for(i = 0; i < sizeof(double); i++)
  {
    *(s2 + i) = *(s1 - i);
  }
  
  return dRetVal;
}
#endif

int ReadSocket(int socket, void* buf, int nLen)
{
  int i,n;
  char* s;

  s = (char*)buf;
  
  nLostSocket = socket;
  #ifdef UNIX
  sigset(SIGALRM, SocketLoseControl);
  alarm(nWaitingTime);
  #endif

  /* Read enough chars */
  for (i = 0; i < nLen; i+=n)
  {
    n = read(socket, s + i, nLen - i);
    if(n == 0)
      return i;
    if (n < 0)
      return n;
  }
  #ifdef UNIX
  alarm(0);
  #endif

  return nLen;
}

void SocketLoseControl()
{
  close(nLostSocket);
}
