/*证明上传产品组信息*/
struct GroupProdCodeInfo {
	char prct_cde[19+1]; //	产品号
	char bnft_acctno[28+1]; //	资金帐号
	struct GroupProdInfo *next;
};

/*理财产品证明(时点)上传结构*/
struct InvestProveReq {
	char cust_name[40+1];  //	客户名
	char cert_typ [10+1];  //	证件类型
	char cert_id  [18+1];  //	证件号
	char strt_dt  [8+1];   //	开立日期
	struct GroupProdCodeInfo *prodcode_info; //产品信息
	int total_qut;
	char doc_no[16+1]; //凭证起始号码
	int doc_num; //	凭证张数
};

struct GroupProdInfo{
	char prct_name[40+1]; //	产品名称	40	
	char curr_cod[10+1];  //	币种	10	CodeString
	double hold_value;	//  持有份额	15，4	N
	double fair_value;  //	资产估值	15,2	N
	struct GroupProdInfo *next;
};

/*理财产品证明(时点)下传结构*/
struct InvestProveResp {
	char cust_name[40+1]; //	客户姓名	40	C
	char cert_typ [10+1]; //	证件类型	10	CodeString
	char cert_id  [18+1]; //	证件号码	18	C
	char strt_dt  [8+1];  //	开立日期	8	C
	char strt_time[19+1]; //	开立时间	19	C
	struct GroupProdInfo *prod_info;
	int total_qut;  //	开立份数	2	N
	char doc_no[16+1]; //	凭证起始号码	16	C
	int doc_num; //	凭证张数	2	N
	char prt_flag[10+1]; //	打印标志	10	C
};

/*理财产品证明(时段)上传结构*/
struct InvestProveSectReq {
	char func_no[1+1]; /*	功能号	1	C	1-开立
	2-撤销
	3-补开
	4-续开 */
	char crc_cod[15+1]; //	资信证明号	15	C	功能号2、3、4，必输
	char cust_name[40+1]; //	客户姓名	40	C	
	char cert_typ [10+1]; //	证件类型	10	CodeString	
	char cert_id [18+1];  //	证件号码	18	C	
	char strt_dt [8+1]; //	起始日期	8	C	
	char end_dt	[8+1]; //终止日期	8	C	
	struct GroupProdCodeInfo *acct_info;
	int total_qut; //	开立份数	2	N	
	char doc_no[16+1]; //	凭证起始号码	16	C	
	int doc_num; //	凭证张数	2	N	
};

struct GroupSectProdInfo{
	char bnft_acctno[28+1]; //	资金结算账户	28	C	
	char curr_cod[10+1]; //	币种	10	CodeString	
	double fair_value;  //	产品估值	15,2	N	含未发放收益
	struct GroupSectProdInfo *next;
};

/*理财产品证明(时段)下传结构*/
struct InvestProveSectResp {
	char crc_cod[15+1]; //	资信证明号	15	C	
	char cust_name[40+1]; //	客户姓名	40	C	
	char cert_typ[10+1]; //	证件类型	10	CodeString	
	char cert_id[18+1]; //	证件号码	18	C	
	char strt_dt[8+1]; //	起始日期	8	C	
	char end_dt[8+1]; //	终止日期	8	C	
	double amt; //	产品总额	15,2	N	
	struct GroupSectProdInfo *prod_info;
	int total_qut;  //	开立份数	2	N	
	char doc_no[16+1];	//凭证起始号码	16	C	
	int doc_num; //	凭证张数	2	N	
	char prt_flag[10+1]; //	打印标志	10	C	需根据每位设置为1的项进行选择打印
};


/*理财产品交易证明上传结构*/
struct InvestTransReq {
	char cust_name[40+1]; //	客户姓名	40	C
	char cert_typ[10+1]; //	证件类型	10	CodeString
	char cert_id[18+1]; //	证件号码	18	C
	char prct_cde[19+1]; //	产品编号	19	C
	char bnft_acctno[28+1]; //	资金结算账户	28	C
	char strt_dt[8+1]; //	起始日期	8	C
	char end_dt[8+1]; //	终止日期	8	C
	int total_qut;  //	开立份数	2	N
	char doc_no[16+1]; //	凭证起始号码	16	C
	int doc_num;   //	凭证张数	2	N
};

/*交易组信息*/
struct GroupTransInfo{
	char prct_name[40+1]; //	产品名称	40	C	
	char curr_cod[10+1]; //	币种	10	CodeString	
	char txdate[8+1];  //	交易日期	8	C	
	char trans_type[1+1]; /*	交易类型	1	C	1  认购
	2  申购
	3  赎回
	4  终止
	5  收益发放
	6  非交易过户－过出
	7  IPO转投转出
	8  IPO返款
	9  红利再投
	A  IPO扣划
	B  IPO转投转入
	F  非交易过户-过入 */
	double trans_amt; //	交易金额	15,2	N	钞汇合计
};

/*理财产品交易证明下传结构*/
struct InvestTransResp{
	char cust_name[40+1]; //	客户姓名	40	C	
	char cert_typ[10+1]; //	证件类型	10	CodeString	
	char cert_id[18+1]; //	证件号码	18	C	
	char strt_dt[8+1]; //	起始日期	8	C	
	char end_dt[8+1]; //	终止日期	8	C	
	struct GroupTransInfo *trans_info;
	int total_qut; //	开立份数	2	N	
	char doc_no[16+1]; //	凭证起始号码	16	C	
	int doc_num;  // 凭证张数	2	N	
	char prt_flag[10+1]; //	打印标志	10	C	需根据每位设置为1的项进行选择打印
};


