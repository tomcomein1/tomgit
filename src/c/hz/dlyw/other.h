#ifndef OTHER_H
#define OTHER_H
long GenerateLsh(long nLb,char *cjh,char *cjym,char *clsh);
void SystemBackup();
void HintError(int nType,char *sInfo, ...);
long SelectJym(char * sValue,long nlx);
void ChangeUserPassword();
void show_statue();
void ChangeUserPassword();
int DlPasswordEnc(char* sPassWord,char * sNewPassWord);
int DlPasswordDec(char* sPassWord,char* sNewPassWord);
long FindRoute(char * sDestination,char * sGateway);
#endif

