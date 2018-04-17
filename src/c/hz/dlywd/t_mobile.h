#ifndef T_MOBILE_H
#define T_MOBILE_H

typedef struct chargedetail{                     //缴费明细
                            long   nYear;          //年份
                            long   nMonth;         //月份
                            double dFee;         //本月应收
                            double dBchf;         //应收款
                            double dZnj;          //滞纳金
                            double dYhk;          //优惠款
                            double dYckhb;        //预存款划拨
                            double dXjk;          //新缴款
                            double dYzf;          //月租费;
                            double dTff;          //特服费；
                            double dBwf;          //本网费；
                            double dMyf;          //漫游费；
                            double dCtf;          //长途费；
                            double dFjf;          //附加费;
                            double dQtf;          //其他费；
                            double dXxf;          //信息费；
                            double dPzf;          //频占费；
                            double dNhf;          //农话费；
                            double dBsf;          //补收费；
                            }CHARGEDETAIL;

typedef struct mobileanswer{                 //移动应答用户查询
                             char sReturn[5];   //返回码
                             char sBargain[16]; //合同号
                             char sRegion[3];   //地区代码
                             char sCountry[3];  //市县代码
                             char sCompany[60]; //单位名称
                             char sUserName[13];//用户名称
                             long nWay;         //付款方式
                             char sStatus[3];      //运行状态
                             double dwczhf;     //未出帐话费
                             double dxyd;       //信誉度
                             double dyck;       //预存款
                             double dbsyzf;     //补收月租费 
                             char   smljqs[5];  //模拟机器数
                             char   sszjqs[5];  //数字机器数
                             long   nCount;     //交费明细记录数[可由此判断是否还有其它话费]
                           }MOBILEANSWER;      

long dsMobilePayQuery(char * inbuf,RECORD * Client);
static long MobileInsertYwmx(char * slsh,char * sCzydh,char * sPhone,char * sJym,char * sDetail,RECORD * Client);
long dsTaskMobileSIngIn(char * sJym);
long dsTaskMobileSIngOut(char * sJym);
long dsTaskMobileCompletePrint(char * sJym,char * sphone,char * nstarttime,char * endtime);
long ASCIItoBCD(char * sASC,int nLen,char * sDest);
long BCDtoASCII(char * sBCD,int nLen,char * sDest);
void preDESconvert(char * sKey,char * sStream,char * sDest);
static int  dsMobileSendAndRecv(int nsock,char * sinbuf,long nopt,char * sSendAdd,char * soutbuf);
static void MobileFormatTransfer(char * sAnswer,long nTaskNo);
static void GetMobileErrorInfo(char * sError,char * sInfo);
static long MobileInsertCompleteYwmx(char * slsh,char * sPhone,char * sJym,char * sDetail);
long dsTaskMobilePayConfirm(char * inbuf,char * outbuf);
long dsMobileCheckAccount(char * inbuf,char * outbuf);
static long dsMobileFormAccountFile(char * inbuf,char * sFileName,long * pnSize);
void dsMobileSendPayDetail(int nSocket,char * sAnswer,long nFileSize);
long dsMobileRecallConfirm(char * inbuf,long nxh,char * outbuf);
long  dsMobileParsePrint(char * filename,char * data);
#endif


