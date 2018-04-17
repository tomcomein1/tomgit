#ifndef ZHW_EXEC_H
#define ZHW_EXEC_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

//int zhw_exec(char * sUserName,char * sPassWord,char * sExec);
int zhw_exec(char *,char *,char *,char *,char *);
#endif

