/****
**  程序名：fs_invest_prove.ec
**  功能描述： 理财资信证明交易
**  内部函数：
**  程序员：
**  最后修改日期： 2011-11-15
****/

EXEC SQL include "fs_mframe.h";

/*理财资信证明时点*/
int InvestProveDot()
{
	struct InvestProveReq rbuf;
	struct InvestProveResp sbuf;
	FS_SYSFLOW_T	fsSysFlow;
	
	int ret = FALSE;
	
	memset (&rbuf, 0x00, sizeof(rbuf));
	memset (&sbuf, 0x00, sizeof(sbuf));
	
	//取接收的接口结构
    memcpy(&rbuf, _recvBuf->detailBuf, sizeof(rbuf));
    
    //检查输入项是否为空
    if (CheckProveDot(rbuf) == FALSE) return FALSE;
    _sysDate = PubGetRq(); /*取系统日期*/
    
    updCertID(rbuf.cert_typ, rbuf.cert_id);
    
    memset (&fsSysFlow, 0x00, sizeof(FS_SYSFLOW_T));
    PubStrncpy(fsSysFlow.id_type, rbuf.cert_typ, 10);
    PubStrncpy(fsSysFlow.id_id, rbuf.cert_typ, 18);
    PubStrncpy(fsSysFlow.cust_name, rbuf.cust_name, 40);
    ret = PubProveSysFlow(&fsSysFlow);
    if (ret != TRUE)
    {
    	PUBERR("0690B1105474", "记流水出错");
        return FALSE;
    }
    
    //处理时点证明
    ret = ProcessProveDot(&rbuf, &sbuf);
	if (ret != TRUE)
    {
        PUBERR("0690B1105474", "业务处理函数[ProcessProveDot]错");
        PubStrncpy(fsSysFlow.txstatus, "2", 1);
        updSysflowInfo(&fsSysFlow);
        return FALSE;
    }
    
	//赋下传链表
    PubStrncpy(_sendBuf->responseCode, _code, 19);
    PubStrncpy(_sendBuf->errMessage, _desc, 199);

    _sendBuf->detailBuf = (char*)gcCalloc(1, sizeof(sbuf));
    memcpy(_sendBuf->detailBuf, &sbuf, sizeof(sbuf));

    //返回
    return TRUE;
}

/*业务处理函数*/
int ProcessProveDot(struct InvestProveReq* r_buf,
	struct InvestProveResp* s_buf)
{
	struct GroupProdCodeInfo *pinfo;
	struct GroupProdInfo *curr=NULL, *next=NULL;
	struct fs_trans_account trans;
	PROD_INFO_HQ stProdInfo;
	PD_VALUE_T  pd_value;
	double hold_value=0.00, fair_value=0.00; 
	char channel_id[10+1];
	int ret = 0;
	int i = 0;
	
	/*客户姓名*/
	PubStrncpy(s_buf->cust_name, r_buf->cust_name, 40);
	/*证件类型*/
	PubStrncpy(s_buf->cert_typ, r_buf->cert_typ, 10);
	/*证件号码*/
	PubStrncpy(s_buf->cert_id, r_buf->cert_id, 18);
	/*开立日期*/
	PubStrncpy(s_buf->strt_dt, r_buf->strt_dt, 8);

	pinfo = r_buf->prodcode_info;
	
	PubStrTrim(pinfo->prct_cde);
	PubStrTrim(pinfo->bnft_acctno);
	for (i=0; i<4 && pinfo->prct_cde[0]; i++)
	{
		PUBERR(PXYM, "产品:[%s] 帐号:[%s]", 
			pinfo->prct_cde, pinfo->bnft_acctno);
		memset(&stProdInfo, 0x00, sizeof(PROD_INFO_HQ));
		
		/*取产品信息*/
		ret = getProdInfoHQ(pinfo->prct_cde, &stProdInfo);
		if (ret == FALSE)
		{
			PUBERR("0690B1103011", "查询理财产品基本信息出错");
			return FALSE;
		}
		
		/*转换渠道*/
		/**getStdChannel(_channelID, channel_id);
    	ret = chkChannelValid(channel_id, stProdInfo.pd_id, _transactionID);
    	if (ret == FALSE)
    	{
    	    PUBERR("0690B1101046", "产品[%s]在渠道[%s]没有[%s]功能!", 
    	    	stProdInfo.pd_id, channel_id, _transactionID);
    	    return FALSE;
    	}**/
    	
		/*检查客户帐号信息，或取交易帐户信息*/
		ret = chkSelfAcctInfo(r_buf->cust_name, r_buf->cert_typ, r_buf->cert_id, 
							pinfo->bnft_acctno, &trans);
		if (ret == FALSE)
		{
			PUBERR("0690B1103073", "客户帐户信息不一致");
			return FALSE;
		}
		
		hold_value = 0.00;
		ret = getProdShareInfo(pinfo->prct_cde, trans, &hold_value );
		if (ret == FALSE || hold_value < 0.005)
		{
			PUBERR("0690B1103073", "产品帐号无份额");
			return FALSE;
		}
		memset(&pd_value, 0, sizeof(PD_VALUE_T));
		if ( stProdInfo.prod_mode[0] == '1' ) /*净值型产品*/
		{
			ret = getProdNetValue(stProdInfo.pd_id, 1, _sysDate, &pd_value);
			if (ret == FALSE)
    		{
        		PUBERR("0690E1107068", "取净值信息失败");
        		return FALSE;
    		}
    	}
    	else pd_value.sel_price = 1.0;

		fair_value = hold_value * pd_value.sel_price;
		/*分配动态内存区*/
		curr = (struct GroupProdInfo*)gcCalloc(1, sizeof(struct GroupProdInfo));
		if (curr==NULL) 
		{
			PUBERR("0690B1103073", "内存分配失败");
			return FALSE;
		}
		
		/*给下传结构赋值*/
		PubStrncpy(curr->prct_name, stProdInfo.prod_name, 40);
		PubStrncpy(curr->curr_cod,  stProdInfo.trade_cur, 10);
		curr->hold_value = hold_value;
		curr->fair_value = fair_value;
		curr->next = NULL;
		/*链表头*/
		if (i == 0) s_buf->prod_info = curr;
		else
		{ //在链表尾新增数据
			if (next)
				next->next = curr;
		}

		next = curr;

		pinfo = pinfo->next;
		PubStrTrim(pinfo->prct_cde);
		PubStrTrim(pinfo->bnft_acctno);
	}
	
	/*开立时间*/
	PubGetDateTime(s_buf->strt_time, 19);
	/*开立份数*/
	s_buf->total_qut = (r_buf->total_qut > 0) ? r_buf->total_qut : 1;
	/*凭证起始号*/
	PubStrncpy(s_buf->doc_no , r_buf->doc_no, 16);
	/*凭证张数*/
	s_buf->doc_num = (r_buf->doc_num > 0) ? r_buf->doc_num : 1;
	/*打印标志*/
	PubStrncpy(s_buf->prt_flag, "0000010000", 10);

	return TRUE;
}

/*记流水*/
int PubProveSysFlow(FS_SYSFLOW_T* flow)
{
	int ret = 0;
	
	PubStrncpy(flow->sysflowno, _sysflowno, 19);
	flow->txdate = _sysDate;
    flow->flow_list_no = 0;
    PubStrncpy(flow->txtime, _dateTime, 19);
    PubStrncpy(flow->txtype, _transactionID, 6);
    /* PubStrncpy(flow->func_no, rbuf->func_no, 1); */
    PubStrncpy(flow->chan_id, _channelID, 10);
    PubStrncpy(flow->txstatus, "9", 1);
   	PubStrncpy(flow->orgid, _branchID, 9);
    
    PubStrncpy(flow->operid, EAIHInmOrgTellerID, 12);
    PubStrncpy(flow->auth_operid, "", 0);
    PubStrncpy(flow->auth_oper_rank, "", 0);
    PubStrncpy(flow->curr_code, "0", 10);
    flow->txamount = 0.00; /* 计算金额;!!!!*/
    PubStrncpy(flow->debit_orgid, _branchID, 9);
    PubStrncpy(flow->clientsn, _transactionSN, 20);

    ret = insSysflowInfo(flow);
    if (ret == FALSE)
    {
      PUBERR("0690B1103042", "记录系统流水失败");
      return FALSE;
    }
    
    return TRUE;
}

/*检查客户帐户信息*/
int chkSelfAcctInfo(char* cust_name,char* id_type, char* id_id, 
					char* acctno, struct fs_trans_account* trans)
{
	EXEC SQL begin declare section;
	struct fs_trans_account	tr;
	struct fs_ta_account ta;
	char account_number[28+1];
	EXEC SQL end   declare section;
	
	PubStrncpy(ta.cust_name, cust_name, 40);
	PubStrncpy(ta.id_type, id_type, 10);
	PubStrncpy(ta.id_id, id_id, 18);
	PubStrncpy(account_number, acctno, 28);
	
	memset (&tr, 0x00, sizeof(struct fs_trans_account));
	EXEC SQL select B.ta_org,  B.ta_id, B.finance_org,
				B.cust_trans_no, B.account_number 
			into :tr.ta_org, :tr.ta_id, :tr.finance_org,
				:tr.cust_trans_no, :tr.account_number
		from fs_ta_account A, fs_trans_account B
		where A.ta_org = B.ta_org
		  and A.ta_id = B.ta_id
		  and A.cust_name = :ta.cust_name
		  and A.id_type = :ta.id_type
		  and A.id_id = :ta.id_id
		  and B.account_number = :account_number;
	if (SQLCODE)
	{
		PUBERR("0690E1120999", "检核客户帐户信息错误[%d]", SQLCODE);
		return FALSE;
	}
	
	memcpy (trans, &tr, sizeof(struct fs_trans_account));
	return TRUE;
}

/*取客户产品帐户持有份额*/
int getProdShareInfo(char* prod_code, struct fs_trans_account trans,
			double* hold_value )
{
	EXEC SQL begin declare section;
	struct fs_trans_account	tr;
	char prodcode[19+1];
	double share=0.00;
	EXEC SQL end   declare section;
	
	memcpy(&tr, &trans, sizeof(struct fs_trans_account));
	PubStrncpy(prodcode, prod_code, 19);

	EXEC SQL select sum(share - share_freeze - share_freeze_sp + owing_interest)
					into :share
		from fs_prod_account
		where ta_org = :tr.ta_org
		  and ta_id  = :tr.ta_id
		  and finance_org = :tr.finance_org
		  and cust_trans_no = :tr.cust_trans_no
		  and prodcode = :prodcode
		  and share - share_freeze - share_freeze_sp > 0.005 ;
	if (SQLCODE)
	{
		PUBERR("0690E1120999", "取产品帐户持有份额错[%d]", SQLCODE);
		return FALSE;
	}

	*hold_value = share;
	return TRUE;
}

int CheckProveDot(struct InvestProveReq r)
{
	if (chkReqPara(r.cust_name, "客户姓名", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.cert_typ, "证件类型", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.cert_id, "证件号码", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.strt_dt, "开立日期", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.doc_no, "凭证起始号码", 'C') == FALSE)
		return FALSE;

	return TRUE;
}

