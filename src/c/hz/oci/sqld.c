// 用法：
//     dlywd [端口号] 启动服务进程，缺省端口号 dlyw
//     dlywd stop [端口号] 停止服务进程 缺省端口号 dlyw
// 编程：李宝东
// 日期：2000.2.22


#include "sqld.h"
#include "oracle.h"

int main(int argc, char *argv[])
{
  char service[21] = "sqlsrv";  /* service name or port number  */
  struct sockaddr_in fsin;  /* the address of a client  */
  int alen;     /* length of client's address */
  int msock;      /* master server socket   */
  int ssock;      /* slave server socket    */
  long  nTaskNo;
  long nRetVal;
  time_t now;
  

  //确定DLYWD的工作目录

  
  if(argc > 1)  strcpy(service, argv[1]);

  //将进程转移到后台以免阻塞
  switch (fork())
  {
    case 0:		/* child */
      break;
    case -1:
      errexit("fork: %s\n", strerror(errno));
    default:	/* parent */
      exit(0);
  }

  (void) signal(SIGCLD, cleanup);
  signal(SIGHUP,SIG_IGN);
      
  msock = passiveTCP(service, 5);

  printf("\nSqld start... ");
  printf("\nSQL服务器(for Oracle 7.x/8.x)启动完毕...\n");
  fflush(stdout);

  for(;;)
  {
    signal(SIGCLD,cleanup);
    alen = sizeof(fsin);
    ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
    if (ssock < 0)  continue;
   
    switch (fork())
    {
      case 0:		/* child */
         break;
      case -1:          /* error in fork */
         errexit("fork: %s\n", strerror(errno));
      default:         	/* parent */
         close(ssock);
         continue;
    }
    
    close(msock);

    fprintf(stderr,"\nconnecting from %s\n",inet_ntoa(fsin.sin_addr));
    
    /*连接认证*/
    {
       char encrypt[9],keyword[9],identify[9];
       int length;
       
       time(&now);
       sprintf(keyword,"%08ld",now%100000000);
       length=8;
       DesEncString(keyword,8,keyword,8,identify,&length);
       SendRecordToSocket(ssock,keyword,8);
       SetWaitingTime(MIN_WAITING_TIME);
       if(GetOneRecordFromSocket(ssock,keyword,8)!=8) break;
    
       for(length=0;length<8;length++) 
          if(keyword[length]!=identify[length]) break;
   
       if(length<8) break;
    }    
    
    if(SendLongToSocket(ssock,0L)) break;
    sqlserver(ssock);
    break;
    
  }//end 后台狩护循环
  fprintf(stderr,"\nconnect closed...");
  close(ssock);
  exit(0);            
}

static void sqlserver(int ssock)
{
  
  long nTaskNo;
   
  for(;;)
  {
      SetWaitingTime(MAX_WAITING_TIME);
      if(GetLongFromSocket(ssock,&nTaskNo)) break;
      fprintf(stderr,"\ntaskno=%ld",nTaskNo);
      
      switch(nTaskNo)
      {
        case TASK_CLOSE_MISSION:   
             return;

        case TASK_CONNECT_DATABASE:
             dsConnectDatabase(ssock); 
             break;

        case TASK_CLOSE_DATABASE:
             dsCloseDatabase(ssock);
             return;

        case TASK_COMMIT_WORK:     
             dsCommitWork(ssock);
             break;

        case TASK_ROLLBACK_WORK: 
             dsRollbackWork(ssock);  
             break;

        case TASK_RUN_SELECT:      
             dsRunSelect(ssock);
             break;

        case TASK_RUN_SQL:      
             dsRunSql(ssock);   
             break;

        case TASK_OPEN_CURSOR:
             dsOpenCursor(ssock);
             break;

        case TASK_FETCH_CURSOR:  
             dsFetchCursor(ssock);  
             break;

        case TASK_CLOSE_CURSOR:   
             dsCloseCursor(ssock); 
             break;

        case TASK_RUN_PROCEDURE:  
              
             break;

        case TASK_GET_SQL_ERRNO:  
             dsGetSqlErrNo(ssock);
             break;

        case TASK_GET_SQL_ERRINFO:
             dsGetSqlErrInfo(ssock);
             break;
      }  
  }//end for
}


/*==========================================================
函数：cleanup()
目的：去除僵死进程，并回收进程表空间
===========================================================*/
static void cleanup(int sig)
{
    int status;
    
    while(waitpid(-1,&status,WNOHANG)>0);
}


