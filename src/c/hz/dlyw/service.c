/*              综合网服务接口（service.c)
 *
 * Orginal Programmer: Chenbo 
 *
 * Date: 2001/12/17
 */
#include <stdio.h>
#include <unistd.h>
#include "dlpublic.h" 
#include "netmain.h"
#include "service.h"

static long terminated;

int zhwmain()
{
	
    int msock;  //main socket
    int ssock;  //slave socket
    struct sockaddr_in address; 
    int alen;
    
    msock = passiveTCP("zhwsrv",5);
    signal(SIGHUP,SIG_IGN);
    
    printf("\n综合网服务接口进程(zhwsrv)启动完毕...\n");
    for(;;)
    {
    	alen=sizeof(address);
    	alarm(0);
        signal(SIGCLD,zhwcleanup);
    	ssock=accept(msock,(struct sockaddr *)&address,&alen);
    	if(fork())     //parent
    	{
    	    close(ssock);  
    	    continue;
    	}
    	close(msock);
    	(void)exec_prog(ssock);
        close(ssock);
    	exit(0);
    }	
}

static int exec_prog(int ssock)
{
    
    FILE * fexe;
    unsigned char ch;
    char path[81],username[21],password[21];
    char userenv[31],passenv[31];
    int nfds,retcode;
    fd_set rfds;
    struct timeval waittime;
    
    if(GetFromSocket(ssock,"%s%s%s",path,username,password)) return -1;
    alltrim(path);
    alltrim(username);
    alltrim(password);
    
    sprintf(userenv, "HZUSER=%s", username);
    if(putenv(userenv)) return -1;
    
    sprintf(passenv, "HZPASS=%s", password);
    if(putenv(passenv)) return -1;
    
    signal(SIGCLD,zhwcleanup);

    if(!(fexe=popen(path,"rw"))) return -1;
    terminated=0;
    nfds=getdtablesize();
    while(terminated==0)
    {
          FD_ZERO(&rfds);
          FD_SET(ssock,&rfds);
          FD_SET(fexe->__file,&rfds);
          waittime.tv_sec=0;
          waittime.tv_usec=100;
          fflush(fexe);
 
          select(nfds,&rfds,(fd_set *)0,(fd_set *)0,&waittime);

          if(FD_ISSET(fexe->__file,&rfds))
          {
               for(;;)
               {
                   retcode=read(fexe->__file,&ch,1);
                   if(retcode==0) break;
                   if(retcode<0)  return -2;
                   write(ssock,&ch,1);
               }
          }      

          if(FD_ISSET(0,&rfds))
          {
               while(read(ssock,&ch,1)>0) 
                  if(write(fexe->__file,&ch,1)<0) return -2;
          }        
          
    }
    return 1;
}

static void zhwcleanup(int signo)
{
    int status;
    waitpid(-1,&status,WNOHANG);
    terminated=1;
}
