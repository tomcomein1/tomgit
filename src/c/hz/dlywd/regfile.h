#ifndef REGFILE_H
#define REGFILE_H

#define MODE_NONE_DIR    0x00000000   //��������Ŀ¼
#define MODE_LOCAL_DIR   0x00000001   //Ҫ������Ŀ¼
#define MODE_REMOTE_DIR  0x00000002   //���ļ�ʱ�γ�Զ��Ŀ¼

#define MODE_FROM_DISK     0x00000004  //�����Ŀ¼��ѡ���ļ�

#define DATASERVER   //������BKFXD�б�����ſ�����

#ifndef DATASERVER
#include"tools.h"
long SearchAndRegisteFileTree(char * root,char * remote,char * dest,long nMode);
long BrowseRegistedTable(DATATABLE * pdt);
long SfxjRecvRemoteFileAndCommit();
long MoveRemoteAfterReceive(char * sRemoteHost,char * sRemoteFile,char * sBackupDir);
long ReceiveAndRegisteFile(char * sRemoteHost,char * sRemoteDir,
                           char * sLocalDir,long nMode);
long RegisteFile(long nOpt,char * sRemoteHost,char * sLocalFile,
                 char * sRemoteFile,long nRkbz);
long DealwithRegisteredFile(long nOpt,char * sBackupDir);
void QueryWjcsdjb(long nDate);
static long RegisteRemoteSuccessed(char * sRemoteHost,char * sLocalFile,char * sRemoteFile);
static long MoveRegisteredFile(char * sLocalFile,char * sBackupDir);
static long RegisteRemote(char * sRemoteHost,char * sLocalFile,char * sRemoteFile);
void ShowDatatableInfo(DATATABLE * pdt,char * smsg,long ntype);
#else
long dsTaskRegisteRemote(int nSocket);
long dsTaskRegisteRemoteSuccessed(int nSocket);
long dsTaskSearchAllFileName(int nSocket);
long dsTaskMoveRemoteAfterReceive(int nSocket);
#endif
#endif
