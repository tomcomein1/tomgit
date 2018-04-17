#ifndef T_AIR_H
#define T_AIR_H
long dsTaskGetAllCity();
long dsTaskQueryAirLine(char * sJh,char * sFromCode,char * sDestCode,long nAirDate,long nAirTime,char * sCzy);
long dsTaskQueryAirPrice(char * sCompany,char * sFromCode,char * sDestCode,long nAirDate);
long dsTaskAirPurchase(char * inbuf);
long dsTaskAirConfirm(char * inbuf);

long dsTaskAirReturnTicket(char * inbuf);
long dsTaskAirSignIn(char * sJym);
//long dsTaskQueryAirBerth(long nSelect);
long dsTaskQueryAirBerth(char * sairline,long ndate);
long dsTaskAirOnLine(char * inbuf);
long dsTaskAirSignOut();
long dsTaskOutAirTicket(char * inbuf,long nFlag);

long dsAirRecall(char * inbuf,long nxh,char * outbuf);
static long dsInsertAirData(char * inbuf);
static int dsAirSendAndRecv(int nsock,char * inbuf,char * outbuf);
static int dsAirCriticalBegin();
static int dsAirCriticalEnd(int nsock);
static void GetAirErrInfo(long nRetCode);
long dsTaskAirCheck(char * inbuf);
long InsertAirQuery(char * sBuffer,long nAirDate,long nAirTime,char * sJh,char * sCzy,char * route);
static long fillterberth(char source[],char sout[]);
long dsTaskQueryAirTicket(char * inbuf);
long dsTaskGetDesAirData(char *inbuf);
long dsTaskCheckDesAirTicket(char *inbuf);
long dsTaskCheckReturnDesAirTicket(char *inbuf);
void QueryAirBerth(char * sairline,long ndate,char * sberth);
void ROUND(double * pVar,int point);
long dsTaskSaveAirPay(char *inbuf);
long dsPnQuery(char *sFromCode,char *sDestCode,char *sResult);

#endif

