/****
**  程序名： fs_invest_prove_trans.ec
**  功能描述： 理财资信交易证明
**  内部函数：
**  程序员：
**  最后修改日期： 2011-11-15
****/

EXEC SQL include "fs_mframe.h";

/*理财资信交易证明*/
int InvestProveTrans()
{
	struct InvestTransReq rbuf;
	struct InvestTransResp sbuf;
	FS_SYSFLOW_T	fsSysFlow;
	
	int ret = FALSE;
	
	memset (&rbuf, 0x00, sizeof(rbuf));
	memset (&sbuf, 0x00, sizeof(sbuf));
	
	//取接收的接口结构
    memcpy(&rbuf, _recvBuf->detailBuf, sizeof(rbuf));
    
    //检查输入项是否为空
    if (CheckProveTrans(rbuf) == FALSE) return FALSE;
    
    _sysDate = PubGetRq(); /*取系统日期*/
    
    updCertID(rbuf.cert_typ, rbuf.cert_id);
     
    memset (&fsSysFlow, 0x00, sizeof(FS_SYSFLOW_T));
    PubStrncpy(fsSysFlow.id_type, rbuf.cert_typ, 10);
    PubStrncpy(fsSysFlow.id_id, rbuf.cert_typ, 18);
    PubStrncpy(fsSysFlow.cust_name, rbuf.cust_name, 40);
    PubStrncpy(fsSysFlow.prodcode, rbuf.prct_cde, 19);
    PubStrncpy(fsSysFlow.ctrt_no, rbuf.bnft_acctno, 28);
    ret = PubProveSysFlow(&fsSysFlow);
    if (ret != TRUE)
    {
    	PUBERR("0690B1105474", "记流水出错");
        return FALSE;
    }
    
    //处理时点证明
    ret = ProcessProveTrans(&rbuf, &sbuf);
	if (ret != TRUE)
    {
        PUBERR("0690B1105474", "业务处理函数[ProcessProveTrans]错");
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
int ProcessProveTrans(struct InvestTransReq* r_buf,
	struct InvestTransResp* s_buf)
{
	struct GroupTransInfo *curr=NULL, *next=NULL;
	PROD_INFO_HQ stProdInfo;
	FS_CONCLUDE_LIST_T con[5];
	char channel_id[10+1];
	int ret = 0;
	int i = 0;
	
	/*客户姓名*/
	PubStrncpy(s_buf->cust_name, r_buf->cust_name, 40);
	/*证件类型*/
	PubStrncpy(s_buf->cert_typ, r_buf->cert_typ, 10);
	/*证件号码*/
	PubStrncpy(s_buf->cert_id, r_buf->cert_id, 18);
	/*起始日期*/
	PubStrncpy(s_buf->strt_dt, r_buf->strt_dt, 8);
	/*终止日期*/
	PubStrncpy(s_buf->end_dt, r_buf->end_dt, 8);

	memset(&stProdInfo, 0x00, sizeof(PROD_INFO_HQ));
	
	PubStrTrim(r_buf->prct_cde);
	PubStrTrim(r_buf->bnft_acctno);
	/*取产品信息*/
	ret = getProdInfoHQ(r_buf->prct_cde, &stProdInfo);
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

	/*取成交产品帐号的交易信息记录*/
	ret = getConcludeTransInfo(r_buf->strt_dt, r_buf->end_dt, 
					r_buf->prct_cde, r_buf->bnft_acctno, con);
	if (ret == FALSE)
	{
		PUBERR("0690B1103011", "查询交易信息失败");
		return FALSE;
	}
	
	for (i=0; i<4; i++)
	{
			if (con[i].sysflowno[0] == '\0') break;

			curr = (struct GroupTransInfo*)gcCalloc(1, sizeof(struct GroupTransInfo));
			if (curr==NULL)
			{
				PUBERR("0690B1103073", "内存分配失败");
				return FALSE;
			}
			PubStrncpy(curr->prct_name, stProdInfo.prod_name, 40);
			PubStrncpy(curr->curr_cod,  stProdInfo.trade_cur, 10);
			rfmtdate(con[i].txdate, "yyyymmdd", curr->txdate);
			PubStrncpy(curr->trans_type, con[i].oper_type, 1);
			curr->trans_amt = con[i].cfm_amt_bknt + con[i].cfm_amt_fnex;
			
			/*链表头*/
			if (i == 0) s_buf->trans_info = curr;
			else
			{ //在链表尾新增数据
				if (next)
					next->next = curr;
			}
        	
			next = curr;
	}
	
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

int CheckProveTrans(struct InvestTransReq r)
{
	if (chkReqPara(r.cust_name, "客户姓名", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.cert_typ, "证件类型", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.cert_id, "证件号码", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.prct_cde, "产品编码", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.bnft_acctno, "资金结算帐号", 'C') == FALSE)
		return FALSE;

	if (chkReqPara(r.strt_dt, "起始日期", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.end_dt, "终止日期", 'C') == FALSE)
		return FALSE;

	return TRUE;
}

/*取范围内最近四条交易记录*/
int getConcludeTransInfo(char* bgndate, char* enddate, 
		char* prod_code,char* bnft_acctno, FS_CONCLUDE_LIST_T con[])
{
	EXEC SQL begin declare section;
	struct fs_conclude_list	conclude;
	int bgn_date, end_date;
	char prodcode[19+1];
	char cust_trans_no[28+1];
	int N=0;
	EXEC SQL end   declare section;
	
	rdefmtdate(&bgn_date, "yyyymmdd", bgndate);
	rdefmtdate(&end_date, "yyyymmdd", enddate);
	PubStrncpy(prodcode, prod_code, 19);
	PubStrncpy(cust_trans_no, bnft_acctno, 28);
	
	EXEC SQL declare provecon1_cur cursor for
			select sysflowno, txdate, cfm_amt_bknt, cfm_amt_fnex
	          from fs_conclude_list
	         where prodcode = :prodcode
	           and cust_trans_no = :cust_trans_no
	           and txdate >= :bgn_date
	           and txdate <= :bgn_date
	         order by txdate desc;
	if (SQLCODE)
    {
        PUBERR( "0690E1120999", "declare provecon1_cur error![%d]", SQLCODE );
        return FALSE;
    }

    EXEC SQL OPEN provecon1_cur;
    if(SQLCODE)
    {
        PUBERR("0690E1120999", "open acctbal_cur1 error![%d]", SQLCODE );
        return FALSE;
    }
	
	/*只取四条记录*/
	for (N=0; N<4; N++)
	{
		memset (&conclude, 0x00, sizeof(conclude));
        EXEC SQL FETCH provecon1_cur into
                 :conclude.sysflowno, :conclude.txdate, 
                 :conclude.cfm_amt_bknt, :conclude.cfm_amt_fnex;
        if (SQLCODE && SQLCODE != NOT_IN_TABLE)
        {
            PUBERR("0690E1120999", "fetch provecon1_cur error![%d]", SQLCODE );
            EXEC SQL CLOSE provecon1_cur;
            EXEC SQL FREE  provecon1_cur;
            return FALSE;
        }
        else if (SQLCODE == NOT_IN_TABLE)
             break;

		memcpy(&con[N], &conclude, sizeof(FS_CONCLUDE_LIST_T));
	}
	con[N].sysflowno[0] = '\0';
	
	EXEC SQL CLOSE provecon1_cur;
    EXEC SQL FREE  provecon1_cur;
	
	return TRUE;
}
