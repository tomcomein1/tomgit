#ifndef POLICY_H
#define POLICY_H

#define CODESIZ	                        6     //交易码长度

#define CTL_PKG_LEN  sizeof(struct Ctl_Msg)       //控制长
#define DAT_PKG_LEN sizeof(data_Pkg)       //数据长
#define ACC_PKG_LEN sizeof(account_Pkg)    //对帐包

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
					/* 500501 --- 冲正/
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
typedef   struct   {
	char   trans_code[7];		// 交易码
	char   bank_code[5];		//银行代码
	char  agent_region_code[11]; // 地区码
	char  agent_sub_code[11];    // 网点代码
	char  b2i_policyno[23];     //主险保单号
	char  b2i_pol_name[41];     //险种名称
	char  b2i_insured_name[21]; // 投保人姓名
	char   b2i_mg_branch[7];    //管理机构  
	char   b2i_print_type[3];   //打印标志  
	//（GT或JT必须打印在发票上）
	char   b2i_pay_itrvl[2];    //缴费方式
	char   b2i_pay_dur[5];      // 缴费期间
	char  b2i_pay_amnt[15];     // 缴费总金额
	char   b2i_pay_num[3];      // 缴费明细个数
	//（包括主险在内）
	char b2i_pay_text[600];     // 缴费明细（新增项目）
	char   b2i_print_num[3];    //发票张数
	char  b2i_bclk_code[12];    // 业务员工号
	char  b2i_bclk_name[21];    // 业务员姓名
	char   b2i_op_code[8];      // 操作员代码
	char   b2i_op_name[9];      // 操作员姓名
	char  b2i_op_time[20];      // 业务时间
	char  bank_no[16];          //银行流水

}data_Pkg;

typedef   struct   {
   char   trans_code[7];  	// 交易码
   char   bank_code[5];   	//     银行代码
   char   agent_region_code[11]; // 地区码
   char   agent_sub_code[11];   // 网点代码
   char   b2i_policyno[23];     //  保单号
   char   b2i_mg_branch[7];    //  管理机构
   char   b2i_pay_amnt[15];     //  缴费金额
   char   b2i_pay_text[600];    //  缴费明细
   char   b2i_op_time[20];      //  业务时间
   char   bank_no[16];          //  银行流水
   char   b2i_op_code[5];       //  工号
}account_Pkg;

#endif
