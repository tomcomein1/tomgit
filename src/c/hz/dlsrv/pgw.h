//*************************************************************************
//*  file name : pgw.h (include file for PGW library version 1.3)         *
//*  edit date : 01/04/2002                                               *
//*************************************************************************

//=========================================================================
// definition
//=========================================================================
#define SUCCESS         0
#define ERR_NO_SESSION -1
#define ERR_NO_DATA    -2
#define ERR_WRITE_PART -3
#define ERR_RET        -4
#define ERR_SOCKET     -5
#define ERR_CLOSED     -6
#define ERR_SESSION_ID -7
#define ERR_LEN_OVER   -8

//=========================================================================
// function : int pgw_ver(char *verbuf, int buflen);
//=========================================================================
// purpose  : to show PGW library version
// input    : *verbuf, pointer of version buffer
//            buflen, size of verbuf in byte (min. length = 30)
// output   : (if ret value = SUCCESS)
//            *verbuf, string buffer contain version and date,
//                     format: "Ver: 1.1 , Date: 01/02/2002"
// ret value: SUCCESS = success
//            ERR_RET = error, length not enough
//
int pgw_ver(char *verbuf, int buflen);

//=========================================================================
// function : int pgw_init(void);
//=========================================================================
// purpose  : to initialize PGW
// input    : none
// output   : none
// ret value: SUCCESS = success
//            ERR_RET = error, PGW control program does not execute.
//
int pgw_init(void);

//=========================================================================
// function : int pgw_open(char *hostname, int *session);
//=========================================================================
// purpose  : to find available session
// input    : *hostname, PGW hostname or IP address
//            *session, pointer of session ID
// output   : (if ret value = SUCCESS)
//            *session, found session ID
// ret value: SUCCESS        = success
//            ERR_NO_SESSION = error, no session available
//            ERR_SOCKET     = error, socket error
//
int pgw_open(char *hostname, int *session);

//=========================================================================
// function : int pgw_read(int session, char *readbuf, int *readlen);
//=========================================================================
// purpose  : to read session data (non-blocking)
// input    : session, the ID got from pgw_open function
//            *readbuf, pointer of readbuf
//            *readlen, size of readbuf in byte
// output   : (if ret value = SUCCESS)
//            *readbuf, contain read data
//            *readlen, length of read data
// ret value: SUCCESS        = success
//            ERR_SESSION_ID = error, session ID error
//            ERR_NO_DATA    = error, no data read
//            ERR_CLOSED     = error, socket closed
//            ERR_SOCKET     = error, socket error
//
int pgw_read(int session, char *readbuf, int *readlen);

//=========================================================================
// function : int pgw_write(int session, char *writebuf, int *writelen);
//=========================================================================
// purpose  : to write session data
// input    : session, the ID got from pgw_open function
//            *writebuf, contain writing data
//            *writelen, length of writing data (max. length = 4069 byte)
// output   : (if ret value = ERR_WRITE_PART)
//            *writelen, written length
// ret value: SUCCESS        = success
//            ERR_LEN_OVER   = error, writing length over range (4069)
//            ERR_SESSION_ID = error, session ID error
//            ERR_WRITE_PART = error, write part of data
//            ERR_SOCKET     = error, socket error
//
int pgw_write(int session, char *writebuf, int *writelen);

//=========================================================================
// function : int pgw_check_status(char *status);
//=========================================================================
// purpose  : to check host line and printer status
// input    : *status, pointer of status
// output   : (if ret value = SUCCESS)
//            *status, host line and printer status
//            bit7   = 0 : host line is down
//                   = 1 : host line is up
//            bit2-0 = 0 : printer is unconfiged
//                   = 1 : printer is not connected
//                   = 3 : printer is ready
//                   = 7 : printer is busy
// ret value: SUCCESS = success
//            ERR_RET = error, can not get host line and printer status
//
int pgw_check_status(char *status);

//=========================================================================
// function : int pgw_close(int session);
//=========================================================================
// purpose  : to close this session
// input    : session, the ID got from pgw_open function
// output   : none
// ret value: SUCCESS        = success
//            ERR_SESSION_ID = error, session ID error
//            ERR_RET        = error, close session error
//
int pgw_close(int session);
