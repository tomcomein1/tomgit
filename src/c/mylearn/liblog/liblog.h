#if !defined (_LIBLOG_H)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#define WINDOWS

#define MAX_STRING_LEN 0x2000
#define DT_STR_LEN     30
#define DIR_STR_LEN    256

#define WRITE_LOG	0

char _LOGfile[256];

/*取指定格式日期时间*/
unsigned long GetDateTime(char *date_time, const char *fmt);
/*系统调用创建日志目录*/
void  CreateDir(const char *path);

int GetLogFile(char *logfile, const char *file) ;

/*输出日志信息*/
void WriteLog(const char *logfile, const char *file, int line, char *format, ...);
#define _ARGL_ __FILE__,__LINE__
#define DBG WriteLog
/** #define DBG(logfile, fmt, argl...) WriteLog(logfile, __FILE__, __LINE__, fmt, ##argl) **/

typedef struct err_struct
{
    char SysNo[20];
    long ErrNo;
    char ErrMsg[256];
} ERR_STRUCT;

typedef struct list
{
    ERR_STRUCT ERR;
    struct list *next;
} LIST;

LIST *init_err_list ();
void free_err_list (LIST *head);
int insert_err_list_tail(LIST *head, ERR_STRUCT *err_data);
int insert_err_list_head(LIST *head, ERR_STRUCT *err_data);
void prt_err_list(LIST *head);
char *get_sub_string(char *source, char *dest, int c, int n);
int get_err_msg(char *filename, char *_sysno, int _errno, char *_errmsg);

typedef struct list *_LIST;

#define _LIBLOG_H
#endif

