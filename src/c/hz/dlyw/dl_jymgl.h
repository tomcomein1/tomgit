#ifndef DL_JYMGL_H
#define DL_JYMGL_H
#define MODE_CUSTOMER   0x00000001
#define MODE_IDENTIFIED 0x00000002

MENU * CreateJymPopMenu(long nlx);
void AddDsJym();
void AddSpJym();
void OpenManage();
void DeleteJym();
#endif
