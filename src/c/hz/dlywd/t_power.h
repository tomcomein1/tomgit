#ifndef T_POWER_H
#define T_POWER_H
long dsTaskPowerSignIn(char * sJym);
long dsTaskPowerSignOut(char * sJym);
long dsTaskPowerQuery(char * inbuf,RECORD * Client);
long dsTaskPowerPayConfirm(char * inbuf,char * outbuf);
static int dsPowerSendAndRecv(int nsock,char * sinbuf,long nopt,char * sSendAdd,char * soutbuf);
static void GetPowerErrorInfo(char * sError,char * sInfo);
#endif
