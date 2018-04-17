#if !defined(_DBTOOLS_H)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

EXEC SQL include sqlca;
EXEC SQL include sqlda;
EXEC SQL include sqltypes;
EXEC SQL include decimal;

/** 定义调试 开关 **/
#define _DEBUG 0


/**部分宏定义**/
#define SHOWSQLERR do{ \
        WriteLog(ERR_LVL, "SQLCODE=%d, ISAM Error=%6hd\n",\ 
                          sqlca.sqlcode, sqlca.sqlerrd[1]);\
        } while(0)

#define SQL_STMT "select b.colno, b.colname, b.coltype, b.collength \n"\
        "from systables a, syscolumns b \n"\
        "where a.tabname = '%s' \n"\
         " and a.tabid = b.tabid \n" \
         " order by b.colno "

/****************/
#define IS_LEEP_YEAR(y) ( ((y%4 == 0) && (y%100 != 0)) || (y%400 == 0) )
#define NELEMS(array) (sizeof(array) / sizeof(array[0])) 

#define FLDSIZE 255
#define MAX_DB_LEN   50
#define MAX_TAB_LEN  128
#define MAX_FILE_LEN 256
#define MAX_SQL_LEN  2048

EXEC SQL BEGIN DECLARE SECTION;

struct ifx_table_cols{
    int  colno;
    char colname[128+1];
    int  coltype;
    int  collength;
};

typedef struct ifx_sys_table IFX_SYS_TABLE;

EXEC SQL END DECLARE SECTION;

/*******fuction define************/

extern int connect_database(char *db_name);
extern int close_database();
extern int runsql(char *_sql);
/*******fuction define************/

#define _DBTOOLS_H
#endif

