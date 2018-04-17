/*              综合网服务接口（service.c)
 *
 * Orginal Programmer: Chenbo
 *
 * Date: 2001/12/17
 */
#include <stdio.h>
#include <unistd.h>
#include "netmain.h"
#include "service.h"
#include "zhw_dec.h"

extern int errno;
extern char sSysPath[50];


//static long terminated;


int zhwmain()
{
    int msock;  //main socket
    int ssock;  //slave socket
    struct sockaddr_in address;
    int alen;

    int i;

    msock = passiveTCP("zhwsrv",5);
    signal(SIGHUP,SIG_IGN);
    signal(SIGCLD,SIG_IGN);

    printf("\n综合网服务接口进程(zhwsrv)启动完毕...\n");
    for(;;)
    {
    	alen=sizeof(address);
    	alarm(0);
    	ssock=accept(msock,(struct sockaddr *)&address,&alen);
    	if(fork())     //parent
    	{
    	    close(ssock);
    	    continue;
    	}
    	close(msock);
    	(void)exec_prog(ssock);

        printf("@@@@"); // 2002.7.10  Leofall  向客户端发送结束信息

        exit(0);
    }
}


static int exec_prog(int ssock)
{

    FILE * fexe;
    unsigned char ch;
    char path[81],username[21],password[21],filename[31];
    char sCzymc[21],sJsdh[21];
    char sCzymcEnv[21],sJsdhEnv[21];

    //if(GetFromSocket(ssock,"%s%s%s",filename,username,password)) return -1;
    if(GetFromSocket(ssock,"%s%s%s%s%s",filename,username,password,sCzymc,sJsdh)) return -1;
    if(filename[0]!='/')
        sprintf(path,"%s%s",sSysPath,filename);
    else
        strcpy(path,filename);

    if(zhw_enc(username, password) == 1)
    {
      // 2002.6.7  Leofall  增加HZCZYMC HZJSDH两个环境变量
      sprintf(sCzymcEnv,"HZCZYMC=%s",sCzymc);
      if(putenv(sCzymcEnv))
      {
        return -1;
      }
      sprintf(sJsdhEnv,"HZJSDH=%s",sJsdh);
      if(putenv(sJsdhEnv))
      {
        return -1;
      } // end if

       dup2(ssock,STDIN_FILENO);
       dup2(ssock,STDOUT_FILENO);
       dup2(ssock,STDERR_FILENO);

       close(ssock);
       system(path);

    }
    return 1;
}
