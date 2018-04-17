/******************************************************************************
**
** 文件:                     t_tb.h
** 目的:                     电话银行交易任务
**
** 生成日期:                 2001.8
** 原始编程人员:             何庆
** 修改历史
** 日期        修改人员  原因
**
*******************************************************************************/
#include "dlywd.h"


#define  BCD_LEN	    8
#define  ASCII_LEN	    16

#define  LKJY_SUCCESS   "00"       //绿卡交易成功
#define  LKJY_ERR_TX    "L1"       //后台通讯失败
#define  LKJY_ERR_DB    "L2"       //DLYWD操作数局库失败
#define  LKJY_ERR_LKJY  "L3"       //绿卡交易错误
#define  LKJY_ERRINFO_TX   "和电话银行前置机SOCKET连接错误!!!"


//终端标识码
#define  ZDBSM_LK           "00000000"
/*绿卡签到*/
#define  LKQD_BWLXBSF_FD    "0820"
#define  LKQD_BWLXBSF_DF    "0830"
#define  LKQD_JYLXM         "091001"
/*绿卡签退*/
#define  LKQT_BWLXBSF_FD    "0820"
#define  LKQT_BWLXBSF_DF    "0830"
#define  LKQT_JYLXM         "091002"
/*绿卡交换密钥*/
#define  LKJHMY_BWLXBSF_FD  "0800"
#define  LKJHMY_BWLXBSF_DF  "0810"
#define  LKJHMY_JYLXM       "091003"
/*绿卡缴费*/
#define  LKJF_BWLXBSF_FD    "0200"
#define  LKJF_JYLXM         "056031"

/*绿卡取消缴费*/
#define  LKQXJF_BWLXBSF	    "0420"
#define  LKQXJF_JYLXM       "056931"
/*绿卡帐户余额查询*/
#define  LKZHYECX_BWLXBSF   "0200"//报文类型标识符
#define  LKZHYECX_JYLXM	    "053070"//交易类型码
/*绿卡总帐对帐*/
#define  LKDZ_BWLXBSF       "0520"//报文类型标识符
#define  LKDZ_JYLXM         "091004"//交易类型码
/*绿卡明细对帐*/
#define  LKMXDZ_BWLXBSF	    "0522"//报文类型标识符
#define  LKMXDZ_JYLXM       "091011"//交易类型码


extern long nSysDebug;
extern long nSysLkrq;
extern long nSysSfm;
extern char sSysJh[9+1];
extern char sSysErrInfo[81];
static char sequance[5+1];


char lkywcode[3];
char lkywinfo[129];

//电话银行基础参数
typedef struct lkywcs
{
    char sLkjh[21];
    char sYdmy[16+1];
    char sMacKey[16+1];
    char sPinKey[16+1];
}LKYWCS;

//电话银行缴费
typedef struct lkjf
{
    char   sYyzh[19+1];//语音帐号
    char   sYymm[16+1];//语音密码
    char   sYhh[20+1]; //用户号
    long   nDsfzlh;    //代收付种类号
    double dJyje;      //交易金额
}LKJF;

//绿卡取消缴费
typedef struct lkqxjf
{
	char    sJyls[7+1];//交易流水
	char    sSjy[22+1];//数据源
	char    sYyzh[19+1];//语音帐号
	char    sJyjh[7+1];//交易局号
	double  dJyje;//交易金额
	double  dSxf;//手续费
	long    lJyrq;//交易日期
	long    lJysj;//交易时间
}LKQXJF;


long dsTask185jhmy();
long daTaskDl185Jyjecx();
long lGetLkxx(char *YDMY,char *MACKEY,char *PINKEY,char *LKJH);
long FormLsh(char *sSequence);
long getlkjybs();
long dsTaskLkjhmy1();
long dsTaskLkjhmy2();
long dsTaskLkjf(char *input);
void BinaryToHex(char *bin,char *hexASC,long size);
void GetJysj(long *nJysj,long *nJyrq);
void inichar(char *array,long n);
void inputydmy();
void lkinfo();
double getlkjyje();
