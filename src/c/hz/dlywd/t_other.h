#ifndef T_OTHER_H
#define T_OTHER_H
#include<time.h>
void MutualExclusion(char * sJym);
void alltrim(char *s);
void GetSysLkrq();
long GenerateLsh(long nLb,char *cjh,char *cjym,char *clsh);
long dsTaskGetOneFileFromRemote(char * sFile);
long dsTaskSendFileToRemote(char * sFileName,char * sTempFile);
long dsTaskGetValueFromFile(char * sInitFile,char* sTitleFlag,char* sDefault);
long dsTaskPutValueToFile(char * sInitFile,char* sTitleFlag,char* sValue);
long dsTaskSystemBackup();
long dsTaskSystemRestore(char * sBackupFileName);
long dsTaskCleanupHistory(long nDate,long nMode);
long dsTaskGetAllJsdh(char * sJh);
long FindRoute(char * sDestination,char * sGateway);
int DlPasswordEnc(char* sPassWord,char * sNewPassWord);
int DlPasswordDec(char* sPassWord,char* sNewPassWord);
void upper(char * s);
void lower(char * s);
void * IncBufSize(void * buf,long size,long increment);
void TreatementString(char * str);
#endif


