#define TASK_MOBILE_SIGN_IN                   400     //签到操作
#define TASK_MOBILE_SIGN_OUT                  401     //签退操作
#define TASK_MOBILE_PAY_QUERY                 402     //交费查询操作
#define TASK_MOBILE_PAY_CHECK                 403     //交费验证操作
#define TASK_MOBILE_PAY_CONFIRM               404     //现金交费确认操作
#define TASK_MOBILE_PAY_COMPARE               405     //对总帐交易
#define TASK_MOBILE_RECALL                    406     //删帐交易
#define TASK_MOBILE_RECALL_CONFIRM            407     //删帐交易确认
#define TASK_MOBILE_COMPLETE_PRINT            408     //完全划拨查询
#define TASK_MOBILE_ALREADY_PAID              409     //已交费查询
///////////////////////////////////////////////////////////////////
#define TASK_POWER_SIGN_IN                   600     //签到操作
#define TASK_POWER_SIGN_OUT                  601     //签退操作
#define TASK_POWER_PAY_QUERY                 602     //交费查询操作
#define TASK_POWER_PAY_CHECK                 603     //交费验证操作
#define TASK_POWER_PAY_CONFIRM               604     //现金交费确认操作
#define TASK_POWER_PAY_COMPARE               605     //对总帐交易
#define TASK_POWER_RECALL                    606     //删帐交易
#define TASK_POWER_RECALL_CONFIRM            607     //删帐交易确认
#define TASK_POWER_PAY_BUTTON                608     //打扣交易
#define TASK_POWER_REPRINT                   609     //补打发票查询


#define CODESIZE                         6
#define ORDERSIZE                        9
#define MAXDATASIZE                    216
#define TOTAL_PACKAGE_SIZE           20000
#define MAX_PROCEDURE                   20    //最大请求个数
#define MAX_MOB_TIME                    40    //与移动通信的最大时延
#define MAX_DLYW_TIME                   20    //与DLYWD数据通信的最大时延
#define MAX_DELAY_TIME                  90    //等待返回最大等待时间

typedef struct{
              unsigned char  morepkt;    //还有更多的包吗？1：有  0：没有
              unsigned char  pktype;     //包类型          1：文件包  0：数据包
              unsigned char  unitend;    //此种类型的包是否完毕 1：完毕 0：没有
              unsigned char  save;       //保留
              unsigned short sequence;   //包序号
              unsigned short length;     //数据区长度
              unsigned long  msgtype;    //发起地进程号
              char code[CODESIZE];       //交易码
              char d_add[ORDERSIZE];     //接收地地址标识
              char l_add[ORDERSIZE];     //发起地地址标识
              char datatrans[MAXDATASIZE];   //数据区
             }MOBILEPACKAGE;

typedef struct{
              unsigned long processid;              //发起过程序号
              int socket;                          //套接字号
              long starttime;                      //开始时间
              }MOBILEMSG;

void mobmain();
static long dsRecvDlywAndSendMob(int dlywsock);
static long dsRecvMobAndSendDlyw(int mobsock);
static long getptablespace();
void GetNowTime();
static void CleanupSpace();

