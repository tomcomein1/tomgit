#ifndef H_SMS_HTTP
#define H_SMS_HTTP

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iconv.h>
#include <ctype.h>

#define SVR_ADDR "192.168.4.17"
#define SVR_PORT  8080
/** my computer test
#define SVR_ADDR "188.178.168.81"
#define SVR_PORT  80
**/

#define USER "snbank"
#define PASS "snbank2018"
/** 
#define USER "test"
#define PASS "test"
**/
#define BASESIZE 4096
#define TIMEOUT_SECOND 2
#define CHUNKED "chunked"

char *para_iconv(char* from_charset, char* to_charset, char *inbuf) ;
char *php_url_encode(char const *s, int len, int *new_length);
int php_url_decode(char *str, int len);
void StringReplace(char *pszInput, char *pszOld, char *pszNew,char *pszOutput,int nOutputlen);
char* get_sub_str(char *source, char *dest, int c, int n);
void HexDump(char *buf,int len,int addr);

#endif
