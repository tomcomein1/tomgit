/*===========================================================
               global.c(全局变量定义模块)

*==========================================================*/

#include"dlpublic.h"

#ifdef DLYWLIB
extern char zjusername[20];   
extern char zjpassword[20];
#endif

long nMakeDate=20021225;  //制作日期
double dDlywVer=4.00;      //代理业务版本号
double dHelpVer=2.0;      //帮助文档版本号
char sSysIp[16];       //本局IP地址
char sSysJh[10];       //地区局局号
char sSysJsdh[10];     //操作员局所代号
char sSysJsmc[JM_JM_LEN+1];//本人所属支局局名
char sUserName[101];   //用户名称
char sPassWord[41];    //用户口令
long nSysLkrq;  //录卡日期
long nSysSfm;   //系统时间  小时 分钟 秒
long nSysAutoClose=0; //是否自动关闭数据库   dl$dlywcs第10号参数
char sSysCzydh[CZYB_CZYDH_LEN+1];//操作员代号
char sSysCzymc[CZYB_CZYMC_LEN+1];//操作员名称
char sSysJm[JM_JM_LEN+1];/*现业县局局名  cs2*/
long nSysCzyqx;//操作员权限
char sSysCzyqx[21];//操作员权限汉字解释
long nSysCzylb;//操作员类别：0：代理（默认），1：综合网  dlyw.ini:czylb=1;
char sSysDsHost[41]; // 数据库守护进程主机
char sSysDsService[41]; // 数据库守护进程端口号
char sSysManager[7];//代理业务表所在用户，缺省为DLZ
char sSysPassword[41];//代理业务表所在用户的口令，缺省为DDDD
char sSysDlywHost[41]; // 代理业务守护进程主机
char sSysDlywService[41]; // 守护进程端口号
long nSysPcType;          //类别：0：代理前台；1：通讯机
extern long nSysMode;
#ifdef UNIX
extern long nTermType;
#endif


