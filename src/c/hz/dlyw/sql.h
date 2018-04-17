#define MAX_USERNAME_LEN          100
#define MAX_PASSWORD_LEN          30

/* Constants used in this program. */
#ifdef UNIX
#define MAX_BINDS               100
#else
#define MAX_BINDS               80
#endif

#ifdef UNIX
#define MAX_ITEM_BUFFER_SIZE    2001
#else
#define MAX_ITEM_BUFFER_SIZE    501
#endif

#define MAX_SELECT_LIST_SIZE    100
#define MAX_ERR_MSG_LEN         512
#define MAX_CURSOR_NUM          10
#define MAX_SQL_IDENTIFIER      31

#define PARSE_NO_DEFER           0
#define PARSE_DEFER              1
#define PARSE_V7_LNG             2

long ConnectDatabase(char *sUserName,char *sPassWord);
long CloseDatabase(void);
long GetSqlErrNo(void);
long GetSqlErrInfo(char *sErrMsg);
long CommitWork(void);
long RollbackWork(void);
long SetAutoCommitOn(void);
long SetAutoCommitOff(void);
long RunSql(char *sFormat,...);
long OpenCursor(char *sFormat,...);
long FetchCursor(long nCursorId,char *sFormat,...);
long FetchCursorArray(long nCursorId, long* nLenArray, long nRecNum, char *sFormat,...);
long CloseCursor(long nCursorId);
long RunSelect(char *sFormat,...);
long RunProc(char *sProcedureName,char *sInputArg,char *sOutputArg,...);

#ifdef DATASERVER
int dsConnectDatabase(int socket);
int dsCloseDatabase(int socket);
int dsGetSqlErrNo(int socket);
int dsGetSqlErrInfo(int socket);
int dsCommitWork(int socket);
int dsRollbackWork(int socket);
int dsRunSql(int socket);
long RunSqlStmt(char *sOut);
int dsOpenCursor(int socket);
long OpenCursorStmt(char *sOut);
int dsFetchCursor(int socket);
int dsCloseCursor(int socket);
int dsSetAutoCommitOn(int socket);
int dsSetAutoCommitOff(int socket);
int dsRunSelect(int socket);
int dsRunProc(int socket);
#endif

#ifdef TEST
void ShowTestErrMsg(void);
#endif



