#ifndef BKFX_H
#define BKFX_H
/***********************************************************************
** @Copyright...........1999.3
**filename      文件名: dlpublic.h
**target        目的:定义代理业务系统中主要函数及常量
**create time   生成日期: 1999.3
**original programmer   编码:LiBaoDong 李宝东
**copyright     版本
**edit history  修改历史:
**
****************************************************************************/

//#define HOUTAI

// 请同时更改dlpublic.h和netmain.h里的操作系统定义

// DOS下编译
//#define DOS
//#define DLYWDLL
// SCO UNIX下编译
#define UNIX

// make文件中已定义 HP UNIX下编译
//#define HPUNIX

// make文件中已定义 IBM UNIX下编译
//#define IBMUNIX

#if defined(HPUNIX) || defined(IBMUNIX)
#define UNIX
// 长型个位字节在最后
#define LITTLEEND
#endif

#if defined(DOS) && defined(UNIX)
#error Both DOS and UNIX have been defined.
#endif

#if !defined(DOS) && !defined(UNIX)
#error Neither DOS nor UNIX has been defined.
#endif


/*综合网库*/

//#define DLYWLIB

//显示状态栏
#define SITUATION
#define AUTOCLOSEDATABASE  //当CommitWork和RollbackWork自动关闭数据库以提高效率
                           //每当修改后应删除oracle.o,tools.o然后重新编译

#define  SYS_ERR 1
#define  COMM_ERR 2
#define  DB_ERR 3
#define  YEWU_ERR 4
#define  TASK_ERR 5
#define  TAKE_POINT 6
#define  TAKE_STEP 7
#define  DY_YDGS "dyydgs"

#define	YZNB_LSH  1   //邮政内部流水号:局号9位+15交易码+YYYYMMDD+8位序号,序列名：DLNBLSH
#define	YZLK_LSH  2   //邮政绿卡流水号:局号9位+YYYYMMDD+8位序号,序列名：DLNKLSH
#define	WTF_LSH	  3   //与委托方流水号:8位序号,序列名：DLNBLSH_15位交易码


#ifdef DLYWLIB
#define gotoxy bkgotoxy
#define drawbox bkdrawbox
#define getstring bkgetstring
#include "dlpay.h"
#include "varstand.h"
#endif

/*ocimain*/
#define DATACLIENT
/*bkfxd*/

/*
#define DATASERVER
*/


#if !(defined(DATASERVER)||defined(OCI)||defined(DATACLIENT))
#include "dc.h"
#include "dtype.h"
#else
#include "sql.h"
#endif

#define  JYMCSFB_RECORD  50
#define  GS_DIR_NUM  6   //公司目录下子目录数目
#define  MAX_GS_NUM  40
#define  BOHAO_NUM   4   //用FTP 连接时，如中断 重复拨号次数


#define TERM_VT100 1
#define TERM_VT220 2
#define TERM_ANSI 3

#define OUTPUT_SCREEN 1
#define OUTPUT_PRINTER 2
#define OUTPUT_TEXT 3

#define TYPE_INT 1
#define TYPE_LONG 2
#define TYPE_SHORT 3
#define TYPE_FLOAT 4
#define TYPE_DOUBLE 5
#define TYPE_LONGDOUBLE 6
#define TYPE_PASSWORD   8
#define TYPE_CHAR 11
#define TYPE_STRING 12
#define TYPE_NULL -1

/*安装类型  bkfxcsfb  20号参数*/
#define INST_SJ 1  //省局
#define INST_DJ 2  //地局
#define INST_XJ 3  //县局
#define INST_ZJ 4  //支局

#define PRN_OKI 1
#define PRN_LQ1600K 2

#define UNIX_HOST 1
#define UNIX_TERM 2

#define QX_MANAGER 0 /*代理业务表所在用户(缺省为DLZ)拥有的权限*/
#define QX_XTWH 1 /*系统维护*/
#define QX_DLZCZ 2 /*发行组操作*/
#define QX_SD 3 /*收订*/
#define QX_CX 4 /*查询*/

#define MAX_WIN_NUM 10 /*最大保存的画面数*/

#if !(defined(DATASERVER)||defined(OCI)||defined(DATACLIENT))
#define RunSql sql
#define OpenCursor sel
#define FetchCursor fetch
#define CloseCursor csel
#define RunSelect sel1
#define RunProc prc
#define ConnectDatabase connectdb
#define CloseDatabase() closedb()
#define CommitWork() commit()
#define RollbackWork() rollback()
#define GetSqlErrNo() GetErrNo()
#define GetSqlErrInfo GetErrInfo
#endif
/*
#define RunBatch batch
*/


#define ERRMESS1 "%s\n"
#define ERRMESS2 "错误代码:%ld\n"
#define ERRMESS3 "错误原因:%s\n"
#define ERRMESS4 "正在清除本次数据...\n"
#define MESSAGE010    "正在统计,请您稍候......\n"
#define MESSAGE011    "正在查询,请您稍候......\n"
#define MESSAGE012    "正在打印,请您稍候......\n"
#define MESSAGE013    "正在输出到文本文件,请您稍候......\n"
#define MESSAGE014    "请输入文件名: "
#define MESSAGE015    "文件操作失败! 可能计算机故障, 请核查!"
#define MESSAGE016    "按任意键返回"

#define MESSAGE601    "下班时间,您还在加班. 辛苦了!"
#define MESSAGE602    "天晚了! 早点休息!  "
#define MESSAGE603    "今天是周末!  辛勤加班的您辛苦了!"
#define MESSAGE604    "今天是星期天!  感谢您的辛勤工作!"
#define MESSAGE605    "祝您过一个愉快的周末! "
#define NLINE  "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
#define NLINE79  "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"

#define GET_VAL_LEN 30

#define CZYB_CZYDH_LEN 6
#define CZYB_CZYMC_LEN 8
#define CZYB_BZ_LEN 20

#define GS_GSMC_LEN  51
#define GS_CLXBZ_LEN 11

#define DLYWCS_CS_LEN 20

#define JM_JH_LEN 9
#define JM_JM_LEN 20
#define JH_JH_LEN 9
#define JH_JM_LEN 20
#define JS_JSDH_LEN 9
#define JS_JSMC_LEN 20


#define HANGHAO "行号"
#define YEHAO "页号"

#define DLYWCS_NAME "代理业务参数表"
#define DLYWCSFB_NAME "代理业务参数附表"
#define CZYB_NAME "操作员表"
#define GS_NAME "公司表"
#define JH_NAME "局号表"
#define JS_NAME "局所视图"
#define LSH_NAME "流水号表"
#define JKD_NAME "缴款单表"
#define RBD_NAME "日报单表"
#define KHZ_NAME "款汇总表"


#define CZYB_CZYDH "工  号"
#define CZYB_CZYMC "操作员名称"
#define CZYB_CZYQX "操作员权限"
#define CZYB_JS "  支  局  所  号"
#define CZYB_JSDH "局 所 号"
#define CZYB_JSMC "局所名称"
#define CZYB_JM "局所名称"
#define CZYB_BZ " 备  注 "

#define GS_GSBH "公司编号"
#define GS_GSMC "公  司  名  称"
#define GS_BZ " 备      注"
  
#define YWZL_YWZLBH "业务编号"
#define YWZL_YWZLMC "业 务 种 类 名 称"
#define YWZL_BZ "备                   注"


typedef struct czyb
{
  int hanghao;
  char czydh[7];
  char czymc[9];
  long czyqx;
  char jsdh[10];
  char jsmc[21];
  char bz[21];
  char flag[2];
}CZYB;


typedef struct gs
{
  int hanghao;
  long gsbh;
  char gsmc[51];
  long nbl;
  double dbl;
  char cbl[51];
  char lxbz[11];
  char bz[41];
  char flag[2];
}GS;

#define  YWZL_YWZLMC_LEN  20
  
typedef struct ywzl
{
  int hanghao;
  long ywzlbh;
  char ywzlmc[YWZL_YWZLMC_LEN+1];
  long nbl;
  char cbl[51];
  char bz[40];
  char flag[2];
 }YWZL;

typedef struct 
{
  long x;
  long y;
  long nLen;
  char sValue[201];
  char sOldValue[201]; 
  long nFlag;
}TEXTBOX;

#define JYM_JYM_LEN  15
#define JYM_JYMC_LEN   50 
#define JYM_GSMC_LEN   50
#define JYM_FWM_LEN  10
#define JYM_SJYDZ_LEN 15  
#define JYM_JH_LEN   9
#define JYM_JM_LEN   20
#define JYM_DLJGH_LEN  20
#define JYM_BL_LEN   20
#define JYM_LXBZ_LEN 20
#define JYM_BZ_LEN   200

typedef struct
{
  long nXh;
  char cJym[JYM_JYM_LEN+1]; // 交易码
  char cJymc[JYM_JYMC_LEN+1]; // 交易名称
  char cGsmc[JYM_GSMC_LEN+1]; // 公司名称
  char cFwm[JYM_FWM_LEN+1]; // 范围码
  char cSjydz[JYM_SJYDZ_LEN+1]; // 数据源地址
  char cJh[JYM_JH_LEN+1]; // 局号
  char cJm[JYM_JM_LEN+1]; // 局名
double dSflt; // 收费零头
double dSxf; // 手续费
  char cDljgh[JYM_DLJGH_LEN+1]; // 委托方分配的代理机构号
  long nKzlb;   // 扩展类别
  long nSybz;   // 使用标志
  long nCxms;   // 程序模式
  long nJsms;   //结算模式
  long nJsjzrq; //结算截止日期
double dZkfl;  //折扣费率
  long nLkrq;   // 修改日期
  long nBdxh;   //变动序号
  long nDyszxh; //打印设置序号
  long nBl1;    //保留
  long nBl2;    //保留
  char cBl[JYM_BL_LEN+1];     //保留
double dBl;                   //保留
  char cLxbz[JYM_LXBZ_LEN+1]; // 类型标志
  char cBz[600+1]; // 备注
} JYM; // 交易码结构


typedef struct{
	char	cJym[JYM_JYM_LEN+1];
	char    cSjydz[JYM_SJYDZ_LEN+1];
	long   	nBdxh;
	long    nJymBdxh;
	long  	nJhlb;
	char    cJh[JYM_JH_LEN+1];
	long 	nSyzt;
	long 	nCsbz;
	double  dZkfl;
	double 	dSxf;
	long 	nFlag1;
	long 	nFlag2;
	char	cBl1[21];
	char	cBl2[21];
	long 	nBl;
	double	dBl;
	long	nLkrq;
	long	nSfm;
	char 	cCzydh[9];
	char    cLxbz[11];
	char 	cFresh[21];
	char    cBz[201];
	double  dZekou;
	double  dSxffl;
	char    cGljJh[10];
	char    cGljDz[21];
}JYMKZFB;//交易码扩展附表	

#define YWMX_YHBZ_LEN 20
#define YWMX_YHMC_LEN 80
#define YWMX_JSCZ_LEN 10
#define YWMX_BL_LEN 20
#define YWMX_LXBZ_LEN 10
#define YWMX_BZ_LEN 100
#define YWMX_SFMX_ELEMENT_LEN 25

typedef struct
{
  char cJym[JYM_JYM_LEN+1]; // 交易代码
  long nYsnd; // 年度
  long nYsyf; // 月份
  char cYhbz[YWMX_YHBZ_LEN+1]; // 用户标志
  char cYhbz1[YWMX_YHBZ_LEN+1]; // 用户标志1
  char cYhbz2[YWMX_YHBZ_LEN+1]; // 用户标志2
  char cYhmc[YWMX_YHMC_LEN+1]; // 用户名称
  long nYwlb; // 业务类别
  long nXh; // 序号
  double dSfmx[YWMX_SFMX_ELEMENT_LEN+1]; // 收费明细数组
  char cJscz1[YWMX_JSCZ_LEN+1]; // 计算初值1
  long nJscz2; // 计算初值2
  double dJscz3; // 计算初值3
  double dJscz4; // 计算初值4
  long nSl; // 数量
  double dYjzk; // 应缴总款
  double dXzyjk; // 新增应缴款
  double dSjk; // 实缴款
  double dWjk; // 未缴款
  double dDlsxf; // 代理手续费
  long nYfkfs; // 原付款方式
  long nSfkfs; // 实付款方式
  long nBl1; // 保留1
  long nBl2; // 保留2
  double dBl1; // 保留1
  double dBl2; // 保留2
  char cBl1[YWMX_BL_LEN+1]; // 保留1
  char cBl2[YWMX_BL_LEN+1]; // 保留2
  char cJh[JM_JH_LEN+1]; // 局号
  char cCzydh[CZYB_CZYDH_LEN+1]; // 操作员代号
  long nLkrq; // 日期
  long nSfm; // 时分秒
  char cLxbz[YWMX_LXBZ_LEN+1]; // 类型标志
  long nXzbz; // 销帐标志
  long nCsbz; // 传数标志
  char cBz[YWMX_BZ_LEN+1]; // 备注
  char cBl3[YWMX_BL_LEN+1]; // 保留3
  char cBl4[YWMX_BL_LEN+1]; // 保留4
  char cBl5[YWMX_BL_LEN+1]; // 保留5
  char cBl6[YWMX_BL_LEN+1]; // 保留6
  char cBl7[YWMX_BL_LEN+1]; // 保留7
  char cLsh[41]; 
  char cYlsh[41]; 
} YWMX; // 业务明细结构

#define YWMXGL_ZDM_LEN 10
#define YWMXGL_YZDHY_LEN 40
#define YWMXGL_XZDHY_LEN 40
#define YWMXGL_SFMX_LEN 100
#define YWMXGL_BL_LEN 100
#define YWMXGL_LXBZ_LEN 10

typedef struct
{
  long nXh; // 序号
  char cJym[JYM_JYM_LEN+1]; // 交易代码
  long nLb; // 类别
  char cZdmc[YWMXGL_ZDM_LEN+1]; // 字段名称
  char cYzdhy[YWMXGL_YZDHY_LEN+1]; // 字段含义
  char cXzdhy[YWMXGL_XZDHY_LEN+1]; // 字段含义
  long nSybz; // 使用标志
  char cSfmx[YWMXGL_SFMX_LEN+1]; // 算法明细
  long    nBl; // 保留
  double  dBl; // 保留
  char cBl[YWMXGL_BL_LEN+1]; // 保留
  char cLxbz[YWMXGL_LXBZ_LEN+1]; // 类型标志
} YWMXGL; // 业务明细管理结构

#define ZWMX_JKDH_LEN 15
#define ZWMX_LXBZ_LEN 20
#define ZWMX_BL_LEN 20
#define ZWMX_WBJSDH_LEN 20
#define ZWMX_NBJSDH_LEN 20


typedef struct
{
  char cJym[JYM_JYM_LEN+1]; // 交易代码
  long nYsnd; // 年度
  long nYsyf; // 月份
  char cYhbz[YWMX_YHBZ_LEN+1]; // 用户标志
  char cYhbz1[YWMX_YHBZ_LEN+1]; // 用户标志1
  char cYhbz2[YWMX_YHBZ_LEN+1]; // 用户标志2
  char cYhmc[YWMX_YHMC_LEN+1]; // 用户名称
  long nYwlb; // 业务类别
  long nXh; // 序号
  long nSl; // 数量
  double dYjzk; // 应缴总款
  double dXzyjk; // 新增应缴款
  double dSjk; // 实缴款
  double dDlsxf; // 代理手续费
  double dYbk; // 应补款
  double dYtk; // 应退款
  double dSjkxj; // 实缴款现金
  double dSjkcz; // 实缴款存折
  double dSjkxyk; // 实缴款信用卡
  long nSfkfs; // 实付款方式
  char cJkdh[ZWMX_JKDH_LEN+1]; // 缴款单号
  char cJh[JM_JH_LEN+1]; // 局号
  char cCzydh[CZYB_CZYDH_LEN+1]; // 操作员代号
  long nLkrq; // 日期
  long nSfm; // 时分秒
  char cLxbz[ZWMX_LXBZ_LEN+1]; // 类型标志
  long nXzbz; // 销帐标志
  long nCsbz; //传数标志
  long nCscs; //传数次数
  long nJsbz; //结算标志
  char cWbjsdh[ZWMX_WBJSDH_LEN+1]; //外部结算单号
  char cNbjsdh[ZWMX_NBJSDH_LEN+1]; //内部结算单号
  char cBl[ZWMX_BL_LEN+1];   //保留
  char cBl1[ZWMX_BL_LEN+1];   //保留
  char cBl2[ZWMX_BL_LEN+1];   //保留
  char cBl3[ZWMX_BL_LEN+1];   //保留
  char cBl4[ZWMX_BL_LEN+1];   //保留
  char cBl5[ZWMX_BL_LEN+1];   //保留
  long nBl;                  //保留
  char cLsh[41]; 
  char cYlsh[41]; 
} ZWMX; // 帐务明细结构

typedef struct
{
	char cCzydh[11];
	char cJh[11];//局所代号
	long nLb;//操作员类别  0 代理系统创建  1 综合网本机工号  2  综合网异地登记工号
	long nPassword;//更改密码次数
	long nSuccess;//成功进入系统次数
	long nFail;//失败进入系统次数
	long nSyzt;//使用状态
	char cIntime[21];//进入时间
	char cOuttime[21];//退出时间
	char cLastpassword[21];//上次更改密码时间
	char cLastfail[21];//上次失败进入时间
	char cCreateinfo[51];//操作员创建时间
	char cIp[21];//登录IP地址
	char cLastip[21];//上次不同IP
	char cLastiptime[21];
	char cLastipb[21];//再上次不同IP
	char cLastiptimeb[21];
	char cLastczymc[21];//上次不同czymc
	char cLastczymctime[21];//上次不同czymc的时间
	char cBl1[21];
	char cBl2[21];
	char cBz[41];
}CZYFSXX;


#define KEY_RETURN 13
#define KEY_F1 301
#define KEY_F2 302
#define KEY_F3 303
#define KEY_F4 304
#define KEY_F5 305
#define KEY_F6 306
#define KEY_F7 307
#define KEY_F8 308
#define KEY_F9 309
#define KEY_F10 310
#define KEY_F11 311
#define KEY_F12 312
#define KEY_ESC 320
#define KEY_INSERT 0
#define KEY_DELETE 322
#define KEY_PAGEUP 325
#define KEY_PAGEDOWN 326
#define KEY_HOME 323
#define KEY_END 324 
#define KEY_UP 327
#define KEY_DOWN 328
#define KEY_LEFT 329
#define KEY_RIGHT 330
#define KEY_NUMLOCK 331
#define KEY_YES 1
#define KEY_NO 2
#define KEY_ALL 3
#define KEY_BACKSPACE 8
#define KEY_TAB 9

#define KEY_PRNREADY 351
#define KEY_PRNNOTREADY 352
#define KEY_PRNNOTEXIST 353

void EnglishInput();
void ChineseInput();

void ShowChar(void);
void ReverseChar(void);
void clrscr(void);
void getxy(long *x,long *y);
void gotoxy(long x,long y);
void gotol(long x);
void gotor(long x);
void gotou(long y);
void gotod(long y);
void savewin(void);
void popwin(void);
void popwin1(void);

void setprinton();
void setprintoff();
void outputchar(long ch);
long keyb(void);
long keyn(void);
long keyf(void);
long waitkey(void);
long wkey(void);
void rtrim(char* s);
void alltrim(char* s);
void ReverseXY(TEXTBOX * pText);
void ShowXY(TEXTBOX * pText);
void clearline(long y);

#define putspace(n)   outtext("%*c",n,' ')
#define bell()        putchar('\a')

long output(char *sFormat,...);
long outtext(char *sFormat,...);
void DelEscChar(char *s);
long WeekDay(long nYear,long nMonth,long nDay);

long GetReverseNumber(TEXTBOX * pTextBox);
long GetReverseFloat(TEXTBOX * pTextBox);
long GetReverseString(TEXTBOX * pTextBox);
long GetReversePassword(TEXTBOX * pTextBox);
long GetKeyUsingVt100(long nDataType);
long GetKeyUsingVt220(long nDataType);
long GetKeyUsingAnsi(long nDataType);
void clearrecord(long nCol,long nRow,long nFieldLen[],long nFieldNum);
void getposition(long nCol,long nFieldLen[],long nPosition[],long nFieldNum);
void drawbox(long nCol1,long nRow1,long nCol2,long nRow2);
void drawtable(long nCol,long nRow,long nFieldLen[],long nFieldNum,long nRowCount);
long openfile(char * sFileName);
long closefile(void);
/*
long OutPutWithHandle(long nHandle,char *sFormat,...);
*/
long FirstDay(long nMonth);
long LastDay(long nYear,long nMonth);
long PrevDay(long nDate);
long NextDay(long nDate);
void print_normal(long x,long y,char s[]);

long getstring(TEXTBOX * pTextBox);
long getnumber(TEXTBOX * pTextBox);
long getfloat(TEXTBOX * pTextBox);
void lower(char s[]);
void upper(char s[]);

#ifdef DOS
#define hidechar()    printf("\033[30;40m")
void bigfont(void);
void normalfont(void);
void eject(void);
#define PrintLarge(x,y,s) printf("\033[%d;1H\016[{@d(7)0-%d|%d%s}]",y,(x-1)*8,(y-1)*19,s)
#define PrintBig(x,y,s) printf("\033[%d;1H\016[{@A(7)0-%d|%d%s}]",y,(x-1)*8,(y-1)*19,s)
long GetKeyUnderDos(long nDataType);
void FillAttr(long ch,long n,long attr);
#endif

#ifdef UNIX
#define eject()       putchar('\014')
#define hidechar()    printf("\033[6m")
#define bigfont()     printf("\033W1")
#define normalfont()  printf("\033W0")
#define ShowDraw(n)  printf("\033!C%da",n)
#define BeginDraw(n) printf("\033!K%da",n)
#define EndDraw()    printf("\033!E")

void lockkey(void); 
void unlockkey(void); 
void screento80(void); 
void hidecursor(void);
void showcursor(void);
void ShowRead(void);
void NoShowRead(void);
void EndRead(void);
void PrintLarge(int x,int y,char* s);
void PrintBig(int x,int y,char* s);
void sparkle(int x,int y,char* s);
void sparklesmall(int x,int y,char* s);
void delay(long nSecond);
#endif
#define ERR_NO_SELECT 32766
#define ERR_NO_TCPIP 32767
#define ERR_NO_FETCH 32765
#else

#endif




