#include <fcntl.h>
#include <errno.h>
#include <termio.h>
#include "zhw_exec.h"

#define ZHWEXE

#ifdef ZHWEXE

static char sTcpipErrInfo[81];
static long nWaitingTime=60;
static int  nLostSocket;
static char sSysDsHost[41];

static int  connectTCP(const char *host, const char *service );
static int  connectsock(const char *host, const char *service, const char *transport );

static int  ReadSocket(int socket, void* buf, int nLen);
static void SocketLoseControl(int sig);
static int  SendLongToSocket(const int socket,long nValue);
static int  SendStringToSocket(const int socket,char * sValue);

static long ReadIni(void);
static void alltrim(char * s);
static void lower(char s[]);

#endif

extern int errno;

/*　　2002.6.7  Leofall  增加参数　*/
//int zhw_exec(char * sUserName,char * sPassWord,char * sExec)
int zhw_exec(char * sExec,char * sUserName,char * sPassWord,char * sCzymc,char * sJsdh)
{
    int sock,statue;
    pid_t pid;
    char buf[80];
    int nfds,retcode,nbyte;
    fd_set rfds;

    struct timeval waittime;

/*  2002.7.2  Leofall  修改综合网接口，可配置远程的dlsrv服务  */
#ifdef ZHWEXE

    (void)ReadIni();
    sock = connectTCP(sSysDsHost,"zhwsrv");
    if(sock<0)
    {
    	printf("\n无法连接应用服务器(%s)的综合网服务(zhwsrv),请检查dlsrv zhw服务进程是否已启动...",sSysDsHost);
    	perror("connect:");
    	fflush(stdout);
    	return -1;
    }

#else

    sock = connectTCP("localhost","zhwsrv");
    if(sock<0)
    {
    	printf("\n无法连接本机(localhost)的综合网服务(zhwsrv),请检查dlsrv zhw服务进程是否已启动...");
    	perror("connect:");
    	fflush(stdout);
    	return -1;
    }

#endif

    if(SendStringToSocket(sock,sExec))      return -1;
    if(SendStringToSocket(sock,sUserName))  return -1;
    if(SendStringToSocket(sock,sPassWord))  return -1;

    /*  2002.6.7  Leofall  增加综合网接口的传递参数  */
    if(SendStringToSocket(sock,sCzymc))  return -1;
    if(SendStringToSocket(sock,sJsdh))  return -1;

    nfds=getdtablesize();

    for(;;)
    {
          FD_ZERO(&rfds);
          FD_SET(0,&rfds);
          FD_SET(sock,&rfds);
          alarm(0);

          retcode=select(nfds,&rfds,(fd_set *)0,(fd_set *)0,NULL);

          if (retcode<0) break;

          if(FD_ISSET(0,&rfds))
          {
               fflush(stdin);

               memset(buf,0,sizeof(buf));
               nbyte=read(0,buf,80);

               if(nbyte>0)
               {

                  retcode=write(sock,buf,nbyte);
                  if(retcode<0 && errno!=EAGAIN) break;

               }
          }

          if(FD_ISSET(sock,&rfds))
          {
               nbyte=read(sock,buf,80);

               if(nbyte>0)
               {
                  // 2002.7.10  Leofall  判断结束标志
                  if (strstr(buf,"@@@@"))
                  {
                    break;
                  } // end if

               	  write(1,buf,nbyte);
                  fflush(stdout);
               }
          }

    }//end for(;;)
    close(sock);

    //exit(0);
    return 0;
}


#ifdef ZHWEXE

/*------------------------------------------------------------------------
 * connectTCP - connect to a specified TCP service on a specified host
 *------------------------------------------------------------------------
 */
static int connectTCP(const char *host, const char *service )
/*
 * Arguments:
 *      host    - name of host to which connection is desired
 *      service - service associated with the desired port
 */
{
  return connectsock( host, service, "tcp");
}

static int connectsock(const char *host, const char *service, const char *transport )
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

//  struct linger       stTimeOut;              /* Linger time(out) structure */

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

  return s;
}

static int ReadSocket(int socket, void* buf, int nLen)
{
  int i,n;
  char* s;

  s = (char*)buf;

  nLostSocket = socket;
  signal(SIGALRM, SocketLoseControl);
  alarm(nWaitingTime);

  /* Read enough chars */
  for (i = 0; i < nLen; i+=n)
  {
    n = read(socket, s + i, nLen - i);
    if(n == 0)
      return i;
    if (n < 0)
      return n;
  }
  alarm(0);
  return nLen;
}

static void SocketLoseControl(int sig)
{
  close(nLostSocket);
}

static int SendStringToSocket(const int socket,char * sValue)
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

    nLostSocket = socket;

    signal(SIGALRM, SocketLoseControl);
    alarm(nWaitingTime);

    for (outchars = 0; outchars < nLen; outchars+=n )
    {
      n=write(socket, sValue+outchars, nLen-outchars);
      if(n<0)
        return 1;
    }

    alarm(0);
  }
  else
  {
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

    signal(SIGALRM, SocketLoseControl);
    alarm(nWaitingTime);

    for (outchars = 0; outchars < nOutLen; outchars+=n )
    {
      n=write(socket, sOut+outchars, nOutLen-outchars);
      if(n<0)
        return 1;
    }
    alarm(0);
  }

  return 0;
}


static int SendLongToSocket(const int socket,long nValue)
{
  int outchars,nLen,n;

  nValue=htonl(nValue);
  nLen=sizeof(long);

  nLostSocket = socket;

  signal(SIGALRM, SocketLoseControl);
  alarm(nWaitingTime);

  for (outchars = 0; outchars < nLen; outchars+=n )
  {
    n=write(socket, (char*)&nValue+outchars, nLen-outchars);
    if(n<0)
      return 1;
  }
  alarm(0);
  return 0;
}


/*************************************
**
**函数: ReadIni
**目的: 从与主程序同名的ini文件中读取初始化设置
**参数: 无
**返回值: 0 有ini文件，1无ini文件
**
*************************************/

static long ReadIni(void)
{
  FILE *fp;
  char *s;
  char sLine[81];
  char sIniFile[]="zjbkfx.ini";
  char sDsHostFlag[]="dlsrvhost";

  strcpy(sSysDsHost,"localhost");

  if((fp=fopen(sIniFile,"r"))==NULL)
    return 1;

  while(fgets(sLine,81,fp)!=NULL)
  {
    alltrim(sLine);
    lower(sLine);

    if(strncmp(sLine,sDsHostFlag,strlen(sDsHostFlag))==0)
    {
      if((s=strchr(sLine,'='))==NULL)
      {
/*
        gotoxy(10,13);
        outtext("%s文件中%s行缺少'='号",sIniFile,sDsHostFlag);
        keyb();
*/
        continue;
      }
      strncpy(sSysDsHost,s+1,20);
      sSysDsHost[20]='\0';
      alltrim(sSysDsHost);
      if(sSysDsHost[strlen(sSysDsHost)-1]=='\n')
        sSysDsHost[strlen(sSysDsHost)-1]='\0';
      continue;
    }
  }
  fclose(fp);
  return 0;
} // ReadIni

/*************************************
**
**函数: alltrim 
**目的: 删除字符串左右空格 
**参数: 字符串指针 
**返回值: 无
**
*************************************/

static void alltrim(char *s)
{
  long i;
  char* sTop;
  sTop=s;
  while(*sTop==' '||*sTop==9)
  {
    sTop++;
  } 
  if(*sTop=='\0')
  {
    s[0]='\0';
    return;
  }
  if(sTop!=s)
  {
    strcpy(s,sTop);
  }
  i=strlen(s)-1;
  while(s[i]==' '||s[i]==9)
    i--;
  s[i+1]='\0';
}

/*************************************
**
**函数: lower
**目的: 把字符串中大写字母变成小写
**参数: 需转换的字符串
**返回值: 无
**
*************************************/

static void lower(char s[])
{
  long i=0;
  char ch;
  while((ch=s[i])!='\0')
  {
    if(ch>='A'&&ch<='Z')
      s[i]+='a'-'A';
    i++;
  }
}

#endif
