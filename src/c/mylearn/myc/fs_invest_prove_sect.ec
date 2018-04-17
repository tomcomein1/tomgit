/****
**  程序名： fs_invest_prove_sect.ec
**  功能描述：资信证明时段
**  内部函数：
**  程序员： 
**  最后修改日期：2009-7-12
****/

EXEC SQL include "fs_mframe.h";

static struct fs_cust_prod_list _Static_prodlist[100];
static int _Static_no = 0;

#define MAX_STR_LEN 0x2000

/****
 **编程人员：
 **功能：存款证明主程序
 **输入参数：无
 **输出参数：无
 **返回值：TRUE/FALSE
 **修改的全局变量：_sendBuf
****/
int InvestProveSect()
{
    struct InvestProveSectReq     rbuf;
    struct InvestProveSectResp    sbuf;
	char channel[10+1];
    int     ret = FALSE;
    FS_SYSFLOW_T	fsSysFlow;

    //初始化
    memset(&rbuf, 0, sizeof(rbuf));
    memset(&sbuf, 0, sizeof(sbuf));

    //取接收的接口结构
    memcpy(&rbuf, _recvBuf->detailBuf, sizeof(rbuf));
    PubStrTrimAll(rbuf.func_no);
    PubStrTrimAll(rbuf.cert_typ);
    PubStrTrimAll(rbuf.cert_id);
    updCertID(rbuf.cert_typ, rbuf.cert_id);
    PubStrTrimAll(rbuf.strt_dt);
    PubStrTrimAll(rbuf.end_dt);

    //置初始返回信息
    PubStrncpy(sbuf.cert_typ, rbuf.cert_typ, 10);
    PubStrncpy(sbuf.cert_id, rbuf.cert_id, 18);

    //转换渠道
    ret = getStdChannel(_channelID, channel);
    if (ret != TRUE)
    {
        PubErr(-1, __FILE__, __LINE__, "0690B1105474", "转换系统渠道错");
        return FALSE;
    }

    //检查必要参数
    ret = chkMustSectPara(rbuf);
    if (ret == FALSE)
        return FALSE;
	
	memset (&fsSysFlow, 0x00, sizeof(FS_SYSFLOW_T));
    PubStrncpy(fsSysFlow.id_type, rbuf.cert_typ, 10);
    PubStrncpy(fsSysFlow.id_id, rbuf.cert_typ, 18);
    PubStrncpy(fsSysFlow.cust_name, rbuf.cust_name, 40);
    PubStrncpy(fsSysFlow.remark1 , rbuf.crc_cod, 15);
    PubStrncpy(fsSysFlow.func_no, rbuf.func_no, 1);
    ret = PubProveSysFlow(&fsSysFlow);
    if (ret != TRUE)
    {
    	PUBERR("0690B1105474", "记流水出错");
        return FALSE;
    }
    
    //分功能处理
    switch (atoi(rbuf.func_no))
    {
    case 1: //资信证明开立
        ret = proveSect_Open(&rbuf, &sbuf);
        if (ret == FALSE)
            break;
    case 2: //资信证明撤消
        ret = proveSect_Quash(&rbuf, &sbuf);
        break;
	case 3: //资信证明补开
        break;
                
    case 4: //存款证明续开 , 先撤销，再补开
    	ret = proveSect_Quash(&rbuf, &sbuf);
    	if (ret == FALSE)
        ret = proveSect_Open(&rbuf, &sbuf);
        if (ret == FALSE)
        break;
    default:
        ret = FALSE;
        PUBERR("0690B1105409", "存款证明无交易请求%s处理功能", rbuf.func_no);
        PubStrncpy(_desc, "无此功能", 8);
        PubStrncpy(_code, "0690B1105409", 5 );
        break;
    }
    if (ret == FALSE)
    {
        updFlowDesc(fsSysFlow, _desc);
        return FALSE;
    }
    
    //赋下传链表
    PubStrncpy(_sendBuf->responseCode, _code, 19);
    PubStrncpy(_sendBuf->errMessage, _desc, 199);

    _sendBuf->detailBuf = (char *)gcCalloc(1,sizeof(sbuf));
    memcpy(_sendBuf->detailBuf, &sbuf, sizeof(sbuf));

    //返回
    return TRUE;
}

/****
 **编程人员：
 **功能：程序必要参数检查
 **输入参数：上传结构
 **输出参数：无
 **返回值：TRUE/FALSE
 **修改的全局变量：
****/
static int chkMustSectPara(struct InvestProveSectReq strbuf)
{
    int     ret = FALSE;
    int     ifunc = 0;

    //全体功能必输
    ret = chkReqPara(strbuf.func_no, "功能号", 'C');
    if (ret == FALSE)
        return FALSE;
    ret = chkReqPara(strbuf.cert_typ, "证件类型", 'C');
    if (ret == FALSE)
        return FALSE;
    ret = chkReqPara(strbuf.cert_id, "证件号码", 'C');
    if (ret == FALSE)
        return FALSE;
    
    ret = chkReqPara(strbuf.cust_name, "客户姓名", 'C');
    if (ret == FALSE)
        return FALSE;

    ifunc = atoi(strbuf.func_no);
    if(ifunc <1 || ifunc >4)
    {
    	PUBERR("0690B1105409",
    		 "存款证明无交易请求%s处理功能", strbuf.func_no);
    	return FALSE;
 	}
 	
    if (ifunc>1 && ifunc<=4)
    {
    	ret = chkReqPara(strbuf.crc_cod, "资信证明号", 'C');
        if (ret == FALSE)
            return FALSE;
    }
   
    //返回
    return TRUE;
}

/****
 **编程人员：
 **功能：存款证明开立
 **输入参数：上传结构
 **输出参数：客户信息
 **返回值：TRUE/FALSE
 **修改的全局变量：
****/
static int proveSect_Open(
    struct InvestProveSectReq*     r_buf
    ,struct InvestProveSectResp*   s_buf
    )
{
    struct GroupProdCodeInfo *pinfo;
	struct GroupSectProdInfo *curr=NULL, *next=NULL;
	struct GroupSectProdInfo sect[4];
	struct fs_trans_account trans;
	struct fs_prod_account prodacct[4];
	struct fs_certifyinfo cert_info;
	struct fs_certcontent cert_cont;
	PROD_INFO_HQ stProdInfo;
	PD_VALUE_T  pd_value;
	double hold_value=0.00, fair_value=0.00; 
	char channel_id[10+1];
	char prov_id[17+1];
	char conarray[MAX_STR_LEN];
	int ret = 0;
	int i = 0, j=0, flag = 0, iseqno=0;
	
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

	pinfo = r_buf->acct_info;
	
	PubStrTrim(pinfo->prct_cde);
	PubStrTrim(pinfo->bnft_acctno);
	/*凭证起始号码*/
	snprintf(prov_id, sizeof(prov_id), "%s|", r_buf->doc_no);

	memset (conarray, 0x00, sizeof(conarray));
	_Static_no = 0;

	for (i=0; i<4 && pinfo->prct_cde[0]; i++)
	{
		PUBERR(PXYM, "产品:[%s] 帐号:[%s]", 
			pinfo->prct_cde, pinfo->bnft_acctno);
		memset(&stProdInfo, 0x00, sizeof(PROD_INFO_HQ));
		memset(&prodacct, 0x00, sizeof(prodacct));
		
		/*取产品信息*/
		ret = getProdInfoHQ(pinfo->prct_cde, &stProdInfo);
		if (ret == FALSE)
		{
			PUBERR("0690B1103011", "查询理财产品基本信息出错");
			return FALSE;
		}
		//非保证金模式产品，不允许开立存款证明
        if (stProdInfo.fund_tran_mode[0] != FUND_TRAN_MODE_BZJ[0])
        {
            PUBERR("0690B1108076", "产品%s非保证金产品，不能开立存款证明"
                ,stProdInfo.prod_code);
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
		
		/*检查是否已存在证明数据*/
		memset(&cert_info, 0x00, sizeof(cert_info));
		ret = checkSaveCertInfo(trans, &cert_info);
		if (ret == FALSE)
		{
			PUBERR("0690B1103073", "取资信证明登记薄错");
			return FALSE;
		}
		if (ret == TRUE)
		{
			PUBERR("0690B1103073", "该客户已开立存款证还未到期");
			return FALSE;
		}

		/*取产品帐户明细数组*/
		hold_value = 0.00;
		ret = getCustProdListArray(pinfo->prct_cde, trans, conarray, &hold_value);
		if (ret == FALSE || hold_value < 0.005)
		{
			PUBERR("0690B1103073", "[%s][%s]无份额",
					pinfo->prct_cde, trans.cust_trans_no);
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
		
		/*对应的产品帐户数据条件*/
		PubStrncpy(prodacct[i].prodcode, pinfo->prct_cde, 19);
		PubStrncpy(prodacct[i].ta_org, trans.ta_org, 4);
		PubStrncpy(prodacct[i].ta_id, trans.ta_id, 26);
		PubStrncpy(prodacct[i].finance_org, trans.finance_org, 4);
		PubStrncpy(prodacct[i].cust_trans_no, trans.cust_trans_no, 28);
		prodacct[i].share = hold_value;
		
		memset(&sect[i], 0x00, sizeof(struct GroupSectProdInfo));
		/*给下传结构赋值*/
		//PubStrncpy(curr->prct_name, stProdInfo.prod_name, 40);
		PubStrncpy(sect[i].curr_cod,  stProdInfo.trade_cur, 10);
		//curr->hold_value = hold_value;
		PubStrncpy(sect[i].bnft_acctno, pinfo->bnft_acctno, 28);
		sect[i].fair_value = fair_value;
		flag = 1;

		pinfo = pinfo->next;
		PubStrTrim(pinfo->prct_cde);
		PubStrTrim(pinfo->bnft_acctno);
	}
	
	if (flag == 0) 
	{
		PUBERR("0690B1103073", "flag=0");
		return FALSE;
	}
	
/*到dcc冻该客户份额*/
	ret = freezeThawInvestProve("4", r_buf->cust_name, 
		r_buf->cert_typ, r_buf->cert_id, r_buf->strt_dt, r_buf->end_dt, 
		conarray, r_buf->total_qut, r_buf->doc_no, sect[0].curr_cod,
		prov_id );
	if (ret != TRUE)
	{
		PUBERR("0690B1103073", "冻结所有的保证金产品帐户份额失败");
		return FALSE;
	}
	
	EXEC SQL begin work;
	if (SQLCODE)
	{
		PUBERR(PXYM, "begin work[%d]", SQLCODE);
		return FALSE;
	}
	
	for (i=0; i<4; i++)
	{
		memset(&cert_info, 0x00, sizeof(cert_info));
		/*登记资信证明登记薄*/
		cert_info.certinternalno = 0;
    	PubStrncpy(cert_info.ta_org, prodacct[i].ta_org, 4);
    	PubStrncpy(cert_info.ta_id, prodacct[i].ta_id, 26);
    	rdefmtdate((long *)&cert_info.certbgndate, "yyyymmdd", r_buf->strt_dt);
    	rdefmtdate((long *)&cert_info.certenddate, "yyyymmdd", r_buf->end_dt);
    	cert_info.certtotalamt = r_buf->total_qut;
    	PubStrncpy(cert_info.status, "00", 2);
    	cert_info.txdate = _sysDate;
    	PubStrncpy(cert_info.sysflowno, _sysflowno, 19);
    	PubStrncpy(cert_info.openoperid, EAIHInmOrgTellerID, 12);
    	strncpy(cert_info.openorgid, _branchID, 9);
    	PubStrncpy(cert_info.curr_code, sect[i].curr_cod, 10);
    	cert_info.fee_amt = 0.0;
	
    	ret = insCertInfo(&cert_info);
    	if (ret == FALSE)
    	{
    	    PubErr(-1, __FILE__, __LINE__, "0690E1120999",
    	        "记录资产登记簿数据到数据库错");
    	    EXEC SQL rollback work;
    	    return FALSE;
    	}
    	
    	iseqno = 0;
    	for (j=0; j<_Static_no; j++)
    	{
    		if (_Static_prodlist[j].ta_id[0] == '\0') break;
    		memset(&cert_cont, 0x00, sizeof(cert_cont));
    		if (strncmp(_Static_prodlist[j].ta_id, cert_info.ta_id, 10) != 0)
    			continue;
    		cert_cont.certinternalno = cert_info.certinternalno;
        	cert_cont.seqno = iseqno++;
        	PubStrncpy(cert_cont.ta_org, _Static_prodlist[j].ta_org, 4);
        	PubStrncpy(cert_cont.ta_id, _Static_prodlist[j].ta_id, 26);
        	PubStrncpy(cert_cont.finance_org, _Static_prodlist[j].finance_org, 4);
        	PubStrncpy(cert_cont.cust_trans_no, _Static_prodlist[j].cust_trans_no, 28);
        	PubStrncpy(cert_cont.prodcode, _Static_prodlist[j].prodcode, 19);
        	cert_cont.list_no = _Static_prodlist[j].list_no;
        	cert_cont.share = _Static_prodlist[j].share;

        	ret = insCertCont(cert_cont);
        	if (ret == FALSE)
        	{
        	    PubErr(-1, __FILE__, __LINE__, "0690E1120999",
        	        "记录证明内容登记数据到数据库错");
        	    EXEC SQL rollback work;
        	    break;
        	}
    	}
    }

	/*冻结对应的产品帐户份额*/
	ret = freezeProdAcctSPShare(prodacct);
	{
		PUBERR("0690B1103073", "冻结所有的保证金产品帐户份额失败");
		EXEC SQL rollback work;
		return FALSE;
	}

	/*登记存款证明，存款证明内容*/
	
	EXEC SQL commit work;
	if (SQLCODE)
	{
		PUBERR(PXYM, "commit work[%d]", SQLCODE);
		return FALSE;
	}
	
	for (i=0; i<4; i++)
	{
		/*分配动态内存区*/
		if (sect[i].bnft_acctno[0] == '\0') break;

		curr = (struct GroupSectProdInfo*)gcCalloc(1, sizeof(struct GroupSectProdInfo));
		if (curr==NULL) 
		{
			PUBERR("0690B1103073", "内存分配失败");
			return FALSE;
		}
		
		/*给curr赋值*/
		PubStrncpy(curr->curr_cod, sect[i].curr_cod, 10);
		//curr->hold_value = hold_value;
		PubStrncpy(curr->bnft_acctno, sect[i].bnft_acctno, 28);
		curr->fair_value = sect[i].fair_value;
		curr->next = NULL;
		
		/*链表头*/
		if (i == 0) s_buf->prod_info = curr;
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

/*修改对应的产品帐户特殊冻结*/
int freezeProdAcctSPShare(struct fs_prod_account prod_acct[])
{
	EXEC SQL begin declare section;
		struct fs_prod_account	pa;
		int i = 0;
	EXEC SQL end  declare section;
	
	for (i=0; i<4; i++)
	{
		if (prod_acct[i].prodcode[0] == '\0') break;
		memcpy(&pa, &prod_acct[i], sizeof(struct fs_prod_account));
		PubErr(0, __FILE__, __LINE__, PXYM, "%s|%s|%s|%s|%s|%.2f|", 
		  pa.ta_org, pa.ta_id, pa.finance_org, pa.cust_trans_no, pa.prodcode );
		EXEC SQL update fs_prod_account 
		           set share_freeze_sp = share_freeze_sp + :pa.share
		         where ta_org = :pa.ta_org
		           and ta_id = :pa.ta_id
		           and finance_org = :pa.finance_org
		           and cust_trans_no = :pa.cust_trans_no
		           and prodcode = :pa.prodcode ;
		if (SQLCODE)
		{
			PUBERR(PXYM, "update prod_account error[%d]", SQLCODE);
			return FALSE;
		}
	}
	
	return TRUE;
}

/*取产品帐户明细委托单与该帐户总份额*/
int getCustProdListArray(char *prod_code, struct fs_trans_account trans, 
	char* infos, double* sumshare)
{
	EXEC SQL begin declare section;
	struct fs_trans_account	tr;
	struct fs_cust_prod_list plist;
	char prodcode[19+1];
	double sum_share = 0.00;
	EXEC SQL end   declare section;
	
	PubStrncpy(prodcode, prod_code, 19);
	memcpy(&tr, &trans, sizeof(struct fs_trans_account));
	
	EXEC SQL declare prcustlist_cur cursor for
			select ta_org, ta_id, finance_org, cust_trans_no, prodcode, 
			       list_no, share, owing_interest 
	          from fs_cust_prod_list
	         where ta_org = :tr.ta_org
	           and ta_id = :tr.ta_id
	           and finance_org = :tr.finance_org
	           and cust_trans_no = :tr.cust_trans_no
	           and prodcode = :prodcode
	           and status = '0'
	           and share > 0.005 ;
	if (SQLCODE)
    {
        PUBERR( "0690E1120999", "declare prcustlist_cur error![%d]", SQLCODE );
        return FALSE;
    }

    EXEC SQL OPEN prcustlist_cur;
    if(SQLCODE)
    {
        PUBERR("0690E1120999", "open prcustlist_cur error![%d]", SQLCODE );
        return FALSE;
    }
	
	while(1)
	{
		memset (&_Static_prodlist[_Static_no], 0x00, sizeof(FS_CUST_PROD_LIST_T));
		memset (&plist, 0x00, sizeof(FS_CUST_PROD_LIST_T) );

        EXEC SQL FETCH prcustlist_cur into
                 :plist.ta_org, :plist.ta_id, :plist.finance_org, 
                 :plist.cust_trans_no, :plist.prodcode, 
			     :plist.list_no, :plist.share, :plist.owing_interest;
        if (SQLCODE && SQLCODE != NOT_IN_TABLE)
        {
            PUBERR("0690E1120999", "fetch prcustlist_cur error![%d]", SQLCODE );
            EXEC SQL CLOSE prcustlist_cur;
            EXEC SQL FREE  prcustlist_cur;
            return FALSE;
        }
        else if (SQLCODE == NOT_IN_TABLE)
             break;
		
		snprintf(infos + strlen(infos), MAX_STR_LEN, "%s|", plist.consign_id);
		memcpy(&_Static_prodlist[_Static_no], &plist, sizeof(FS_CUST_PROD_LIST_T));
		_Static_no++;
		if (_Static_no >= 100) 
		{
			PUBERR(PXYM, "_Static_no[%d]>=100", _Static_no );
			EXEC SQL CLOSE prcustlist_cur;
    		EXEC SQL FREE  prcustlist_cur;
    		return FALSE;
		}
		sum_share += plist.share;
	}
	
	*sumshare = sum_share;
	PubErr(0, __FILE__, __LINE__, PXYM, "getcustprodlist: [%s][%.2f]", infos, sumshare);
	
	EXEC SQL CLOSE prcustlist_cur;
    EXEC SQL FREE  prcustlist_cur;
	
	return TRUE;
}

/*冻结解冻存款证*/
int freezeThawInvestProve(char* func_no, char* cust_name, 
	char* cert_typ, char* cert_id, char* strt_dt, char* end_dt, 
	char* ctrt_no, int cert_num, char* prt_num, char* curr_cod,
	char* prov_id )
{
	/*ctrt_no, prov_id 必需组织成%s|%s|格式*/
	struct  agrno_point *ap=NULL, *curr=NULL;
	struct  certid_point *cp=NULL, *curr1=NULL;
	struct sd2dcc_savcert_or_impawn_up savecert;
	char *p = NULL;
	int ret = -1;

	PubStrncpy(_transactionSN, _sysflowno, 19);
	PubStrncpy(savecert.func_no, func_no, 2);
	PubStrncpy(savecert.cust_name, cust_name, 40);
	/*转码*/
	transCode("00010501103000","CERT_TYP", cert_typ, savecert.cert_typ, 1);
	
	/*内存分配*/
	ap = (struct agrno_point *)gcCalloc(1, sizeof(struct agrno_point));
	p = ctrt_no;
	while(p = FetchStringToArg(p, "%s|", ap->ctrt_no))
	{
		ap = (struct agrno_point *)gcCalloc(1, sizeof(struct agrno_point));
		if (ap == NULL) return FALSE;
		ap->next = NULL;

		if(curr)
		{
			savecert.a_point = curr = ap;
		}
		else curr->next = ap;

		curr = ap;
	}

	savecert.cert_num = cert_num;
	PubStrncpy(savecert.prt_num, prt_num, 15);
	
	cp = (struct certid_point *)gcCalloc(1, sizeof(struct certid_point));
	p = prov_id;
	while(p = FetchStringToArg(p, "%s|", cp->prov_id))
	{
		cp = (struct certid_point *)gcCalloc(1, sizeof(struct certid_point));
		if (cp == NULL) return FALSE;
		cp->next = NULL;

		if(curr1)
		{
			savecert.c_point = curr1 = cp;
		}
		else curr1->next = cp;

		curr1 = cp;
	}
	
	/*转码*/
	transCode("00019901176101","CURR_COD", curr_cod, savecert.curr_cod,1);
	
	ret = sd2dcc_savcert_or_impawn(&savecert);
	if (ret == FALSE)
	{
		PUBERR(PXYM, "到dcc冻结解冻存款证明失败[%s][%s]", _code, _desc);
		return FALSE;
	}

	/*释放内存*/
	if (savecert.a_point) free(savecert.a_point);
	if (savecert.c_point) free(savecert.c_point);
	
	return TRUE;
}

/*取存款证明数据*/
int checkSaveCertInfo(struct fs_trans_account trans, struct fs_certifyinfo* cert)
{
	EXEC SQL begin declare section;
	struct fs_trans_account	tr;
	struct fs_certifyinfo certify;
	EXEC SQL end   declare section;
	
	memcpy(&tr, &trans, sizeof(struct fs_trans_account));
	
	memset(&certify, 0x00, sizeof(certify));
	EXEC SQL select first 1 	certinternalno, 
					ta_org      ,ta_id       ,
					certbgndate ,certenddate ,
					certtotalamt,status      ,
					txdate      ,sysflowno   ,
					openorgid   ,openoperid  ,
					closedate   ,closeorgid  ,
					closeoperid ,curr_code   ,
					fee_amt 
				into 
					:certify.certinternalno, 
					:certify.ta_org      ,:certify.ta_id       ,
					:certify.certbgndate ,:certify.certenddate ,
					:certify.certtotalamt,:certify.status      ,
					:certify.txdate      ,:certify.sysflowno   ,
					:certify.openorgid   ,:certify.openoperid  ,
					:certify.closedate   ,:certify.closeorgid  ,
					:certify.closeoperid ,:certify.curr_code   ,
					:certify.fee_amt 
				from fs_certifyinfo
				where ta_org = :tr.ta_org
				  and ta_id = :tr.ta_id
				  and status = "00";
	
	if (SQLCODE && SQLCODE!=100)
	{
		PUBERR(PXYM, "取资信登记薄[%d]", SQLCODE);
		return FALSE;
	}
	else if (SQLCODE == 100)
		return SQLCODE;
	
	memcpy(cert, &certify, sizeof(struct fs_certifyinfo));
	
	return TRUE;
}

/*存款证明的撤销*/
static int proveSect_Quash(
    struct InvestProveSectReq*     r_buf
    ,struct InvestProveSectResp*   s_buf )
{
 	struct GroupProdCodeInfo *pinfo;
	struct GroupSectProdInfo *curr=NULL, *next=NULL;
	struct GroupSectProdInfo sect[4];
	struct fs_trans_account trans;
	struct fs_prod_account prodacct[4];
	struct fs_certifyinfo cert_info;
	struct fs_certcontent cert_cont;
	PROD_INFO_HQ stProdInfo;
	PD_VALUE_T  pd_value;
	double hold_value=0.00, fair_value=0.00; 
	char channel_id[10+1];
	char prov_id[17+1];
	char conarray[MAX_STR_LEN];
	int ret = 0;
	int i = 0, j=0, flag = 0, iseqno=0;
	
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

	pinfo = r_buf->acct_info;
	
	PubStrTrim(pinfo->prct_cde);
	PubStrTrim(pinfo->bnft_acctno);
	/*凭证起始号码*/
	snprintf(prov_id, sizeof(prov_id), "%s|", r_buf->doc_no);

	memset (conarray, 0x00, sizeof(conarray));
	_Static_no = 0;

	for (i=0; i<4 && pinfo->prct_cde[0]; i++)
	{
		PUBERR(PXYM, "产品:[%s] 帐号:[%s]", 
			pinfo->prct_cde, pinfo->bnft_acctno);
		memset(&stProdInfo, 0x00, sizeof(PROD_INFO_HQ));
		memset(&prodacct, 0x00, sizeof(prodacct));
		
		/*取产品信息*/
		ret = getProdInfoHQ(pinfo->prct_cde, &stProdInfo);
		if (ret == FALSE)
		{
			PUBERR("0690B1103011", "查询理财产品基本信息出错");
			return FALSE;
		}
		//非保证金模式产品，不允许开立存款证明
        if (stProdInfo.fund_tran_mode[0] != FUND_TRAN_MODE_BZJ[0])
        {
            PUBERR("0690B1108076", "产品%s非保证金产品，不能开立存款证明"
                ,stProdInfo.prod_code);
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
		
		/*检查是否已存在证明数据*/
		memset(&cert_info, 0x00, sizeof(cert_info));
		ret = checkSaveCertInfo(trans, &cert_info);
		if (ret == FALSE)
		{
			PUBERR("0690B1103073", "取资信证明登记薄错");
			return FALSE;
		}
		if (ret == 100)
		{
			PUBERR("0690B1103073", "该客户该ta未开立存款证明");
			return FALSE;
		}

		/*取产品帐户明细数组*/
		hold_value = 0.00;
		ret = getCustProdListArray(pinfo->prct_cde, trans, conarray, &hold_value);
		if (ret == FALSE || hold_value < 0.005)
		{
			PUBERR("0690B1103073", "[%s][%s]无份额",
					pinfo->prct_cde, trans.cust_trans_no);
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
		
		/*对应的产品帐户数据条件*/
		PubStrncpy(prodacct[i].prodcode, pinfo->prct_cde, 19);
		PubStrncpy(prodacct[i].ta_org, trans.ta_org, 4);
		PubStrncpy(prodacct[i].ta_id, trans.ta_id, 26);
		PubStrncpy(prodacct[i].finance_org, trans.finance_org, 4);
		PubStrncpy(prodacct[i].cust_trans_no, trans.cust_trans_no, 28);
		prodacct[i].share = -1 * hold_value; /*减去特殊业务冻结*/
		
		memset(&sect[i], 0x00, sizeof(struct GroupSectProdInfo));
		/*给下传结构赋值*/
		//PubStrncpy(curr->prct_name, stProdInfo.prod_name, 40);
		PubStrncpy(sect[i].curr_cod,  stProdInfo.trade_cur, 10);
		//curr->hold_value = hold_value;
		PubStrncpy(sect[i].bnft_acctno, pinfo->bnft_acctno, 28);
		sect[i].fair_value = fair_value;
		flag = 1;

		pinfo = pinfo->next;
		PubStrTrim(pinfo->prct_cde);
		PubStrTrim(pinfo->bnft_acctno);
	}
	
	/*到dcc冻该客户份额*/
	if (flag == 0) 
	{
		PUBERR("0690B1103073", "flag=0");
		return FALSE;
	}

	ret = freezeThawInvestProve("5", r_buf->cust_name, 
		r_buf->cert_typ, r_buf->cert_id, r_buf->strt_dt, r_buf->end_dt, 
		conarray, r_buf->total_qut, r_buf->doc_no, sect[0].curr_cod,
		prov_id );
	if (ret != TRUE)
	{
		PUBERR("0690B1103073", "解冻所有的保证金产品帐户份额失败");
		return FALSE;
	}
	
	EXEC SQL begin work;
	if (SQLCODE)
	{
		PUBERR(PXYM, "begin work[%d]", SQLCODE);
		return FALSE;
	}
	
	/*解冻对应的产品帐户份额*/
	ret = freezeProdAcctSPShare(prodacct);
	{
		PUBERR("0690B1103073", "冻结所有的保证金产品帐户份额失败");
		EXEC SQL rollback work;
		return FALSE;
	}
	
	for (i=0; i<4; i++)
	{
		if (prodacct[i].prodcode[0] == '\0') break;

		memset(&cert_info, 0x00, sizeof(cert_info));
		ret = checkSaveCertInfo(trans, &cert_info);
		if (ret == FALSE)
		{
			PUBERR("0690B1103073", "取资信证明登记薄错");
			EXEC SQL rollback work;
			return FALSE;
		}
		if (ret == 100)
		{
			PUBERR("0690B1103073", "该客户该ta未开立存款证明");
			EXEC SQL rollback work;
			return FALSE;
		}

		/*登记资信证明登记薄*/
    	PubStrncpy(cert_info.status, "01", 2);
    	cert_info.txdate = _sysDate;
    	PubStrncpy(cert_info.sysflowno, _sysflowno, 19);
    	PubStrncpy(cert_info.openoperid, EAIHInmOrgTellerID, 12);
    	strncpy(cert_info.openorgid, _branchID, 9);

    	ret = updCertInfo(cert_info);
    	if (ret == FALSE)
    	{
    	    PubErr(-1, __FILE__, __LINE__, "0690E1120999",
    	        "记录资产登记簿数据到数据库错");
    	    EXEC SQL rollback work;
    	    return FALSE;
    	}

    }

	EXEC SQL commit work;
	if (SQLCODE)
	{
		PUBERR(PXYM, "commit work[%d]", SQLCODE);
		return FALSE;
	}
	
	for (i=0; i<4; i++)
	{
		/*分配动态内存区*/
		if (sect[i].bnft_acctno[0] == '\0') break;

		curr = (struct GroupSectProdInfo*)gcCalloc(1, sizeof(struct GroupSectProdInfo));
		if (curr==NULL) 
		{
			PUBERR("0690B1103073", "内存分配失败");
			return FALSE;
		}
		
		/*给curr赋值*/
		PubStrncpy(curr->curr_cod, sect[i].curr_cod, 10);
		//curr->hold_value = hold_value;
		PubStrncpy(curr->bnft_acctno, sect[i].bnft_acctno, 28);
		curr->fair_value = sect[i].fair_value;
		curr->next = NULL;
		
		/*链表头*/
		if (i == 0) s_buf->prod_info = curr;
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

