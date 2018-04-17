/****
**  ��������fs_invest_prove.ec
**  ���������� �������֤������
**  �ڲ�������
**  ����Ա��
**  ����޸����ڣ� 2011-11-15
****/

EXEC SQL include "fs_mframe.h";

/*�������֤��ʱ��*/
int InvestProveDot()
{
	struct InvestProveReq rbuf;
	struct InvestProveResp sbuf;
	FS_SYSFLOW_T	fsSysFlow;
	
	int ret = FALSE;
	
	memset (&rbuf, 0x00, sizeof(rbuf));
	memset (&sbuf, 0x00, sizeof(sbuf));
	
	//ȡ���յĽӿڽṹ
    memcpy(&rbuf, _recvBuf->detailBuf, sizeof(rbuf));
    
    //����������Ƿ�Ϊ��
    if (CheckProveDot(rbuf) == FALSE) return FALSE;
    _sysDate = PubGetRq(); /*ȡϵͳ����*/
    
    updCertID(rbuf.cert_typ, rbuf.cert_id);
    
    memset (&fsSysFlow, 0x00, sizeof(FS_SYSFLOW_T));
    PubStrncpy(fsSysFlow.id_type, rbuf.cert_typ, 10);
    PubStrncpy(fsSysFlow.id_id, rbuf.cert_typ, 18);
    PubStrncpy(fsSysFlow.cust_name, rbuf.cust_name, 40);
    ret = PubProveSysFlow(&fsSysFlow);
    if (ret != TRUE)
    {
    	PUBERR("0690B1105474", "����ˮ����");
        return FALSE;
    }
    
    //����ʱ��֤��
    ret = ProcessProveDot(&rbuf, &sbuf);
	if (ret != TRUE)
    {
        PUBERR("0690B1105474", "ҵ������[ProcessProveDot]��");
        PubStrncpy(fsSysFlow.txstatus, "2", 1);
        updSysflowInfo(&fsSysFlow);
        return FALSE;
    }
    
	//���´�����
    PubStrncpy(_sendBuf->responseCode, _code, 19);
    PubStrncpy(_sendBuf->errMessage, _desc, 199);

    _sendBuf->detailBuf = (char*)gcCalloc(1, sizeof(sbuf));
    memcpy(_sendBuf->detailBuf, &sbuf, sizeof(sbuf));

    //����
    return TRUE;
}

/*ҵ������*/
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
	
	/*�ͻ�����*/
	PubStrncpy(s_buf->cust_name, r_buf->cust_name, 40);
	/*֤������*/
	PubStrncpy(s_buf->cert_typ, r_buf->cert_typ, 10);
	/*֤������*/
	PubStrncpy(s_buf->cert_id, r_buf->cert_id, 18);
	/*��������*/
	PubStrncpy(s_buf->strt_dt, r_buf->strt_dt, 8);

	pinfo = r_buf->prodcode_info;
	
	PubStrTrim(pinfo->prct_cde);
	PubStrTrim(pinfo->bnft_acctno);
	for (i=0; i<4 && pinfo->prct_cde[0]; i++)
	{
		PUBERR(PXYM, "��Ʒ:[%s] �ʺ�:[%s]", 
			pinfo->prct_cde, pinfo->bnft_acctno);
		memset(&stProdInfo, 0x00, sizeof(PROD_INFO_HQ));
		
		/*ȡ��Ʒ��Ϣ*/
		ret = getProdInfoHQ(pinfo->prct_cde, &stProdInfo);
		if (ret == FALSE)
		{
			PUBERR("0690B1103011", "��ѯ��Ʋ�Ʒ������Ϣ����");
			return FALSE;
		}
		
		/*ת������*/
		/**getStdChannel(_channelID, channel_id);
    	ret = chkChannelValid(channel_id, stProdInfo.pd_id, _transactionID);
    	if (ret == FALSE)
    	{
    	    PUBERR("0690B1101046", "��Ʒ[%s]������[%s]û��[%s]����!", 
    	    	stProdInfo.pd_id, channel_id, _transactionID);
    	    return FALSE;
    	}**/
    	
		/*���ͻ��ʺ���Ϣ����ȡ�����ʻ���Ϣ*/
		ret = chkSelfAcctInfo(r_buf->cust_name, r_buf->cert_typ, r_buf->cert_id, 
							pinfo->bnft_acctno, &trans);
		if (ret == FALSE)
		{
			PUBERR("0690B1103073", "�ͻ��ʻ���Ϣ��һ��");
			return FALSE;
		}
		
		hold_value = 0.00;
		ret = getProdShareInfo(pinfo->prct_cde, trans, &hold_value );
		if (ret == FALSE || hold_value < 0.005)
		{
			PUBERR("0690B1103073", "��Ʒ�ʺ��޷ݶ�");
			return FALSE;
		}
		memset(&pd_value, 0, sizeof(PD_VALUE_T));
		if ( stProdInfo.prod_mode[0] == '1' ) /*��ֵ�Ͳ�Ʒ*/
		{
			ret = getProdNetValue(stProdInfo.pd_id, 1, _sysDate, &pd_value);
			if (ret == FALSE)
    		{
        		PUBERR("0690E1107068", "ȡ��ֵ��Ϣʧ��");
        		return FALSE;
    		}
    	}
    	else pd_value.sel_price = 1.0;

		fair_value = hold_value * pd_value.sel_price;
		/*���䶯̬�ڴ���*/
		curr = (struct GroupProdInfo*)gcCalloc(1, sizeof(struct GroupProdInfo));
		if (curr==NULL) 
		{
			PUBERR("0690B1103073", "�ڴ����ʧ��");
			return FALSE;
		}
		
		/*���´��ṹ��ֵ*/
		PubStrncpy(curr->prct_name, stProdInfo.prod_name, 40);
		PubStrncpy(curr->curr_cod,  stProdInfo.trade_cur, 10);
		curr->hold_value = hold_value;
		curr->fair_value = fair_value;
		curr->next = NULL;
		/*����ͷ*/
		if (i == 0) s_buf->prod_info = curr;
		else
		{ //������β��������
			if (next)
				next->next = curr;
		}

		next = curr;

		pinfo = pinfo->next;
		PubStrTrim(pinfo->prct_cde);
		PubStrTrim(pinfo->bnft_acctno);
	}
	
	/*����ʱ��*/
	PubGetDateTime(s_buf->strt_time, 19);
	/*��������*/
	s_buf->total_qut = (r_buf->total_qut > 0) ? r_buf->total_qut : 1;
	/*ƾ֤��ʼ��*/
	PubStrncpy(s_buf->doc_no , r_buf->doc_no, 16);
	/*ƾ֤����*/
	s_buf->doc_num = (r_buf->doc_num > 0) ? r_buf->doc_num : 1;
	/*��ӡ��־*/
	PubStrncpy(s_buf->prt_flag, "0000010000", 10);

	return TRUE;
}

/*����ˮ*/
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
    flow->txamount = 0.00; /* ������;!!!!*/
    PubStrncpy(flow->debit_orgid, _branchID, 9);
    PubStrncpy(flow->clientsn, _transactionSN, 20);

    ret = insSysflowInfo(flow);
    if (ret == FALSE)
    {
      PUBERR("0690B1103042", "��¼ϵͳ��ˮʧ��");
      return FALSE;
    }
    
    return TRUE;
}

/*���ͻ��ʻ���Ϣ*/
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
		PUBERR("0690E1120999", "��˿ͻ��ʻ���Ϣ����[%d]", SQLCODE);
		return FALSE;
	}
	
	memcpy (trans, &tr, sizeof(struct fs_trans_account));
	return TRUE;
}

/*ȡ�ͻ���Ʒ�ʻ����зݶ�*/
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
		PUBERR("0690E1120999", "ȡ��Ʒ�ʻ����зݶ��[%d]", SQLCODE);
		return FALSE;
	}

	*hold_value = share;
	return TRUE;
}

int CheckProveDot(struct InvestProveReq r)
{
	if (chkReqPara(r.cust_name, "�ͻ�����", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.cert_typ, "֤������", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.cert_id, "֤������", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.strt_dt, "��������", 'C') == FALSE)
		return FALSE;
	
	if (chkReqPara(r.doc_no, "ƾ֤��ʼ����", 'C') == FALSE)
		return FALSE;

	return TRUE;
}

