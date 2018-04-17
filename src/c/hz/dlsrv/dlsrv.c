#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include "netmain.h"
#include "dlsrv.h"

long nSysLkrq,nSysSfm;
char sSysPath[50];


int main(int argc, char *argv[])
{
  char buf[21];
  char * offset;
  pid_t pid=-1;
  
  memset(sSysPath,'\0',sizeof(sSysPath));
  strcpy(sSysPath,argv[0]);
  offset = sSysPath+strlen(sSysPath);
  while(offset>sSysPath && offset[0]!='/')
  {
  	offset[0]='\0';
  	offset--;
  }
  if(offset==sSysPath) strcpy(sSysPath,"./");

  if(argc==2)
  {
       if(!strcmp(argv[1],"mobile"))
            if((pid=fork())==0) mobilemain();
       if(!strcmp(argv[1],"bp"))
            if((pid=fork())==0) bpmain();  
       if(!strcmp(argv[1],"tele"))
            if((pid=fork())==0) telemain();  
       if(!strcmp(argv[1],"dltx"))
            if((pid=fork())==0) dltxmain();  
       if(!strcmp(argv[1],"zhw"))
            if((pid=fork())==0) zhwmain();  
       if(!strcmp(argv[1],"air"))
            if((pid=fork())==0) airmain();  
       if(!strcmp(argv[1],"policy"))
            if((pid=fork())==0) b2imain();
       if(!strcmp(argv[1],"power"))
            if((pid=fork())==0) powermain();
       if(!strcmp(argv[1],"tax"))
            if((pid=fork())==0) taxmain();
       if(!strcmp(argv[1],"yzsd"))
	    if((pid=fork())==0) yzsdmain();
       if(!strcmp(argv[1],"yatele"))
            if((pid=fork())==0) yatelemain();
       if(!strcmp(argv[1],"dl185")) 
            if((pid=fork())==0) dl185main();

  }
  if(pid<0) 
       printf("\nusage: dlsrv [mobile|bp|tele|dltx|zhw|air|policy|power|tax|yatele] \n"
              "       --------------------------------\n"
              "       bp     - ��Ѱ����˾ͨ�ŷ������(2)\n"
              "       zhw    - ���ۺ����Ľӿڷ������\n"
              "       air    - ���й���ͨ�ŷ������(8)\n"              
	      "       tax    - ���˰��ͨ�ŷ������(10)\n"
              "       tele   - ���й�����ͨ�ŷ������(4)\n"
              "       dltx   - ��ί�з��ļ�ͨ�Ž���\n"
              "       power  - �빩���ͨ�ŷ������(6)\n"
	      "       mobile - ���ƶ���˾ͨ�ŷ������(3)\n"
	      "       yatele - ���Ű�����ͨ�ŷ������(4)\n"
              "       policy - ���й����ٱ���ͨ�ŷ������(9)\n"
              "       dl185  - ��185ͨѶ�������\n");
              

  if(pid>0) printf("\nServer:\"%s\" started with process id:%d\n",argv[1],pid);
  exit(0);  
}


