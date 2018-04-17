#ifndef FILE_SQLD_H
#define FILE_SQLD_H

#include"des.h"
/*Sql Server 服务定义*/
#define   TASK_CLOSE_MISSION           0
#define   TASK_CONNECT_DATABASE        1
#define   TASK_CLOSE_DATABASE          2
#define   TASK_COMMIT_WORK             3
#define   TASK_ROLLBACK_WORK           4
#define   TASK_RUN_SELECT              5
#define   TASK_RUN_SQL                 6
#define   TASK_OPEN_CURSOR             7
#define   TASK_FETCH_CURSOR            8
#define   TASK_CLOSE_CURSOR            9
#define   TASK_RUN_PROCEDURE          10
#define   TASK_GET_SQL_ERRNO          11
#define   TASK_GET_SQL_ERRINFO        12


#define   MIN_WAITING_TIME            20
#define   MAX_TRANS_TIME              60


static void sqlserver(int ssock);
static void cleanup(int sig);


#endif
