#ifndef REGFILE_H
#define REGFILE_H

#define MODE_NONE_DIR    0x00000000   //不搜索子目录
#define MODE_LOCAL_DIR   0x00000001   //要搜索子目录
#define MODE_REMOTE_DIR  0x00000002   //传文件时形成远方目录

#define MODE_FROM_DISK     0x00000004  //允许从目录中选出文件

#define DATASERVER   //如是在BKFXD中编译请放开此项

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
