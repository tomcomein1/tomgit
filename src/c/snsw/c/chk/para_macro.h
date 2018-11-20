#ifndef PARA_MACRO_H
#define PARA_MACRO_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <iconv.h>

#define OK    0
#define FAIL -1

#define FILE_LEN 128
#define COMMAND_LEN 256

#define MAX_STR_LEN  4096
#define MAX_LINE_LEN  4096
#define MAXSLEEP 128
#define BUFLEN 1024
#define MAX_BUFF_LEN 409600
#define ERR_LVL __FILE__, __LINE__

#define PARA_BACKUP_DIR "print/NETBANK/TOPB"

#define GB_CODE "GB18030"
#define UT_CODE "UTF-8"

extern int ParamSet(char *, char *);

extern void print_time( char* tag );
extern char * upper(char *str);
extern char * lower(char *str);
extern char * rtrim(char *str);
extern char *ltrim(char * str);
extern char * rev_order_str(char *_source, char *_dest);
extern char * get_str_usesep(char *_source, int _sep, char *_dest);
extern char* get_sub_str(char *source, char *dest, int c, int n);
extern char *get_mid_str(char *_source, int _s_sep, int _e_sep, char *_dest);
extern int get_line(char * sline, FILE * fp, int endch);
extern char *para_iconv(char* from_charset, char* to_charset, char *inbuf);

char Plogdir[FILE_LEN];
char ErrInfo[COMMAND_LEN];

/* #define DBG_UTF_PKG  */

#endif

