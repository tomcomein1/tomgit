#ifndef T_TELCOM_H
#define T_TELCOM_H
long dsTaskTelecomPayQuery(char * inbuf,RECORD * Client);
long dsTaskTelcomPayConfirm(char *inbuf,char *outbuf);
long dsTelecomRecallConfirm(char * inbuf,long nxh,char * outbuf);
long dsTelecomCheckAccount(char * inbuf,char * outbuf);
long dsTaskSfTelecomPayQuery(char * inbuf,RECORD * Client);
static long dsTelecomSendAndRecv(char * inbuf,char * outbuf);
static void TelecomEncrypt(char *buf,long buflen);
static void TelecomDecrypt(char *buf,long buflen);
#endif
