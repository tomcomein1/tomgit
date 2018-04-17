#ifndef EXCHANGE_H
#define EXCHANGE_H
extern long Dlsrv_ConnectDatabase();
long SendFileByFtp(char * host,char * user,char * password,char * remote,char * local);
long GetFileByFtp(char * host,char * user,char * password,char * remote,char * local);
void dsUnicomFileTrans();
void dsFormAndSendYckFile(char * sJym);
void dsFormAndSendDsfFile(char * sJym);
long GetTempFileName(char * sfilename);
void GetSysLkrq();
void CreateDir(char * path);
void dsMobileAccount();
void dsB2iAccount();
void dsTelAccount();
void dsTpyFileTrans();
long dsPostToTpyFile(char * cJym,char * sFileName);
long TpyFileImport(char * sJym,char * sDirect,char * sFileName);
#endif
