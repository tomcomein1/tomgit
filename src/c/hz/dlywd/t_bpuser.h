#ifndef T_BPUSER_H
#define T_BPUSER_H
long dsTaskBpUserQuery(char * inbuf,RECORD * Client);
long dsTaskBpUserPayConfirm(char *inbuf,char *outbuf);
long dsTaskBpUserRecallConfirm(char * inbuf,long nxh,char * outbuf);
long dsTaskBpTestCall(char * sJym,char * syhbz);
long dsTaskBpCheckAccount(char * inbuf,char * outbuf);
static int dsBpSendAndRecv(int nsock,char * sinbuf,char * soutbuf);
#endif

