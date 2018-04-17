#ifndef IFX_FACE_H
#define IFX_FACE_H

EXEC SQL INCLUDE sqlca; 
EXEC SQL INCLUDE sqlda; 
EXEC SQL INCLUDE sqlhdr; 
EXEC SQL INCLUDE decimal; 
EXEC SQL INCLUDE locator; 
EXEC SQL INCLUDE varchar; 
EXEC SQL INCLUDE datetime; 
EXEC SQL INCLUDE sqlstype; 
EXEC SQL INCLUDE sqltypes; 

EXEC SQL DEFINE SQL_USRNAME_MAX_LEN 64;   /* User name max len */ 
EXEC SQL DEFINE SQL_PASSWD_MAX_LEN 64;    /* Password max len */ 
EXEC SQL DEFINE SQL_SVRNAME_MAX_LEN 64;   /* Database name max len */ 
EXEC SQL DEFINE SQL_CNNAME_MAX_LEN 64;    /* Connect name max len */ 
EXEC SQL DEFINE SQL_STMT_MAX_LEN 1024;    /* SQL statement max len */ 

EXEC SQL BEGIN DECLARE SECTION; 
typedef struct 
{ 
    int id; 
    char name[32]; 
    char gender; 
    int age; 
    char brf[256]; 
}DBStudent_t; 

char svrname[SQL_SVRNAME_MAX_LEN]; /* Server name */ 
char usrname[SQL_USRNAME_MAX_LEN]; /* User name */ 
char passwd[SQL_PASSWD_MAX_LEN];   /* Passwd */ 
char cnname[SQL_CNNAME_MAX_LEN];   /* Connect name */

EXEC SQL END DECLARE SECTION; 

#endif


