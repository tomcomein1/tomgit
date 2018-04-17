#ifndef T_B2I_H
#define T_B2I_H

#define CODESIZ	                        6     //交易码长度

#define CTL_LEN  sizeof(struct Ctl_Msg)       //控制长
#define DATA_LEN sizeof(tongXin_Struct)       //数据长
#define DZDATA_LEN sizeof(duiZhang_Struct)    

/* 控制字符结构格式 */
 
union Ctl_Char {
		unsigned  char	ctl;
		struct {
			unsigned morepkt:1;	/* 是否有后续报文*/
						/*  = 1, 有
						 *  = 0, 没有 */
			unsigned pktype:1;	/*  = 0:正常发包
						    = 1:重发包 */
			unsigned unitend:1;	/* 如置位,某一类结束 */ 
			unsigned reserver:5;	/* 保留 */
			} flags;
		};

/* 控制信息格式结构 */

struct Ctl_Msg {
		char MsgLength [ 7 ] ;     /*  信息长度 */
		union 	   Ctl_Char CtlChar ;    /*  控制字符  */
		char TransCode [ CODESIZ + 1 ] ; /* 交易码 */
					/* 500101 --- 查询（根据保单号）*/
					/* 500201 --- 缴费提交*/
					/* 500301 --- 补打收据*/
					/* 500401 --- 对账处理*/
		char status [ 5 ] ;   /* 状态信息 */
				/* 0000 --- 正确返回 */
				/* 0001 --- 交易码非法 */
				/* 900X --- 数据有问题 */
				/*    |     需做特别处理 */
				/*    |->X=tongXin_Struct结构中的chaXunJieGuo */
		};


/******************************************
* 交易信息: 保险公司与银行的通信信息一至  *
******************************************/
typedef struct
{
	char	jiaoYiLeiXing[3] ;	/*交易类型*/
					/*A1--根据报单号查询*/
					/*B1--交费提交*/
					/*C1--补打收据*/
					/*D1--对账*/
					/*E1--冲正*/
	char	yinHangDaiMa[5] ;	/*银行代码*/
					/*8381--建行*/
					/*8382--农行*/
					/*8383--工行*/
					/*8384--中行*/
                                        /*8385--邮政*/
	char	wangDianDaiMa[10] ;	/*网点代码,由银行提供*/
	char	baoDanHao[23] ;		/*保单号*/
	char	yingShouFeiRiQi[11] ;	/*应收费日期*/
	char	baoXianMingChen[41] ;	/*保险名称*/
	char	touBaoRen[65] ;		/*投保人(姓名)*/
	char	jiaoFeiFangShi ;	/*缴费方式*/
					/*M--月交   Q--季交*/
					/*H--半年交 Y--年交*/
	int	jiaoFeiNianQi ;		/*缴费年期*/
	int	jiaoFeiCiShu ;		/*缴费次数*/
	double	yuJiaoBaoFei ;		/*予缴保费*/
	double	baoFei ;		/*保费*/
	double	yuJiaoFeiYuE ;		/*予缴费余额*/
	double	liXi ;			/*利息*/
	char	shengXiaoRiQi[11] ;	/*生效日期*/
	int	baoDanNianDu ;		/*保单(经过)年度*/
	char	yeWuYuanDaiMa[6] ;	/*业务员代码*/
	char	yeWuYuan[21] ;		/*业务员(姓名)*/
	char	caoZuoYuanDaiMa[5] ;	/*操作员代码*/
	char	caoZuoYuan[9] ;		/*操作员(姓名)*/
	char	shouFeiShiJian[20] ;	/*收费时间*/
	char	chongZhenShiJian[20];	/*冲正时间*/
	char	shouFeiFangShi ;	/*收费方式*/
	char	zhiPiaoHao[21];		/*支票号*/
	char	shouJuYinShuaHao[13] ;	/*收据印刷号*/
	char	chaXunJieGuo ;	/*状态					    */
				/*（一）保险公司到银行:			    */
				/*	0--有应收记录，返回第一笔	    */
				/*	1--非法报单号			    */
				/*	2--报单不存在			    */
				/*	3--报单失效或销户		    */
				/*	4--无应收记录,需要银行进行续收	    */
				/*	5--主险未缴费 			    */
				/*	6--附加险不能预交，请出示主险号	    */
				/*（二）银行到保险公司			    */
				/*收费：0--正常收费 1--保费预交 2--收费异常 */
				/*冲正：0--正常收费冲正  1--预交保费冲正    */
	char   bank_no [ 11 ] ;	/*银行流水号*/
} tongXin_Struct ;


/**********
*对账结构 *
***********/
typedef struct
{
	char	baoDanHao[23] ;		/*保单号*/
	char	yinHangDaiMa[5] ;	/*银行代码*/
	char	wangDianDaiMa[10] ;	/*网点代码*/
	char	jiaoYiLeiXing[3] ;	/*交易类型*/
	char	jiaoYiShiJian[20] ;	/*交易时间*/
	char	caoZuoYuan[5] ;		/*操作员*/
	double	shouFeiJinE ;		/*收费金额*/
	char	shouJuYinShuaHao[13] ;	/*收据印刷号*/
	char    bank_no [ 11 ] ;	/*银行流水号*/
} duiZhang_Struct ;


//static long dsBxSendAndRecv(int nFlag);
long dsTaskBxRecallConfirm(char *inbuf,long nxh,char *outbuf);
static void getBxErrorInfo(struct Ctl_Msg rc,char *sErrorInfo); 
long dsBxRequestRePrint(char *sLsh);
long dsTaskBxCheckAccount(char * inbuf,char * outbuf);
static long dsBxSendAndRecv(struct Ctl_Msg *rc,tongXin_Struct *rp);
void getFlData(long nData,long *na,long *nb,long *nc);
static long dsBxDzSendAndRecv(char *sbuffer);
int iIbmOrScoReturn(int num);
double dIbmOrScoReturn(double num);

#endif
