#ifndef EXCHFILE_H
#define EXCHFILE_H
long SendLocalFile(char *,char * ,char *,int);
long GetRemoteFile(char *,char * ,char *,int);
long ExecCommand(char * );
int  SearchFileTree(char *,char *,char *,int);
long ReplaceFile(char * sdest,char * sfilename);
long ConnectBkfxSrv(char *);
long DisconnectBkfxSrv();
void ExchangeFile(char *);
long ChangeVersion(char * sfile,char * sKey,char * sVersion);
#endif

