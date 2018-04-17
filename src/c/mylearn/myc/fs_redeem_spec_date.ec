/*********************************************************
*  ������:   fs_redeem_spec_date.ec
*  ��������:  ���ڶ��꽻�׹���
*             �ͻ�ָ��ǩԼ����ϵͳ�������ͻ���Ը
              �����Զ���ع���
**  �ڲ�����: redeemSpecForDateMain(int taskno)
**
**  �����Ա: ̷�ľ�
**  ����޸�����: 20110905
*********************************************************/

EXEC SQL include  "fs_mframe.h";

/*��������������*/
struct regular {
    struct fs_regularredeem regred;
    struct regular *link;
};

typedef struct regular REGULAR;
typedef REGULAR *LISTREGULAR;

static LISTREGULAR first = NULL; /*����ͷ�ڵ�*/
extern static FS_SYSFLOW_T _sysflow_info;
extern static FS_TXREVERSE_T   _rev_info;
extern static FS_FLOWRLTV_T _rltvflow_info;
extern static char _cust_type[2+1];
extern static char _auto_flag[6];
extern static int _seqno = LOCAL_RLTV_SEQ;
extern static char _date_time[19+1];
extern static char run_date[8+1] = "\0";
extern static REDEEM_DETAIL_T _redeem_detail[MAX_REDEEM_DETAIL];
extern static REDEEM_BATCH_T _redeem_batch[MAX_REDEEM_DETAIL];
extern static char _flow_no[19+1];
extern static char _pend_code[12+1];
extern static char _pend_desc[200+1];

/*���ڶ���������*/
int redeemSpecForDateMain(int taskno)
{
    int ret = 0;
    
    /*ȡϵͳ����*/   
    _sysDate = PubGetRq();
    /*ɨ����Ҫ����Ķ������ǩԼ*/
    ret = getRegularRedeem();
    if (ret == FALSE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, "ȡ�������ݴ�:[%ld]", SQLCODE );
        freeListRegular();
        return FALSE;
    }
    
    /*���������*/
    ret = processRegularRedeem();
    if (ret == FALSE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, "���������ݴ�:[%ld]", SQLCODE );
        freeListRegular();
        return FALSE;
    }

    freeListRegular();
    return TRUE;
}

/*���ڶ���ҵ������*/
int processRegularRedeem()
{
    EXEC SQL begin declare section; 
        struct fs_regularredeem rd;
    EXEC SQL end   declare section; 
    
    PROD_INFO_HQ prodinfo;
    char old_prodcode[19+1] = "\0";
    char mode[10+1] = "\0";
    int vflag = 0; /*�����ڱ�־*/
    char bgndate[19+1];
    FS_P_INSTANCE_T fpit;
    int ret = 0;
    
    LISTREGULAR plist = first;
    
    for( ; plist; plist=plist->link)
    {
        memcpy(&rd, plist->regred.prodcode, sizeof(struct fs_regularredeem));
        
        /*Ԥ��ü�¼�Ƿ���Ҫ�ڽ���ִ��*/
        ret = prepareRegularRedeem(rd);
        if (ret == FALSE)
            continue; /*Ԥ��Ǹ��մ����¼����*/

        /*����Ƿ�ͬһ��Ʒ,ȡ��Ӧ��Ʒ����*/
        if (strncmp(old_prodcode, rd.prodcode, strlen(rd.prodcode) !=0)
        {
            memset (&prodinfo, 0x00, sizeof(PROD_INFO_HQ));
            ret = getProdInfoHQ(rd.prodcode, &prodinfo);
            if (ret == FALSE)
            {
                PubErr (-1,__FILE__, __LINE__, PXYM,
                                "getProdInfoHQ [%s]", pl.prodcode);
                return FALSE;
            }
            
            memset (bgndate, 0x00, sizeof(bgndate));
            memset (&fpit, 0x00, sizeof(FS_P_INSTANCE_T));
            vflag = checkInstanceValid(rd.prodcode, "04", _sysDate, bgndate, &fpit);
            if (vflag == FALSE)
            {
                PubErr(0, __FILE__, __LINE__, PXYM, "[%s]���������Ϣ��!",rd.prodcode);
                return FALSE;
            }
            
            /*ȡ����ʵʱģʽ�����ڴ˴���ͬ��Ʒֻȡһ��*/
            memset(mode, 0x00, sizeof(mode));
            ret = getFundBatchTranFlag (prodinfo.pd_id, ACCOUNT_CUSTREDEEM, mode);
            if (ret == FALSE)
            {
                PubErr(0, __FILE__, __LINE__, PXYM, "[%s]ȡ����ʵʱģʽ����!",
                        prodinfo.pd_id);
                return FALSE;
            }
            
            /*���²�Ʒ��*/
            PubStrncpy(old_prodcode, rd.prodcode, sizeof(old_prodcode)-1);
        }
        
        /*��Ʒ������ؿ����ڣ��޸�Ϊʧ��*/
        if (vflag == 1)
        {
            PubErr(0, __FILE__, __LINE__, PXYM, "�������ڲ�����![%s]", 
                  prodinfo.pd_id);
            /*�޸ĸ�ǩԼЭ����Ӧ����*/
            PubStrncpy(_desc, "��Ʒ������ؿ�����,ʧ��", 50);
            setRDFailInfo(&rd);
            if (updateFailRegularRedeem(rd) == FALSE)
            {
                PubErr(0, __FILE__, __LINE__, PXYM, "Э��[%d]�޸Ŀ�ʧ��!",
                    rd.ctrt_no);
                return FALSE;
            }
            continue; /*������һ��*/
        }
        
        /*�������ڼ��*/
        ret = checkSpecialDate(prodinfo.pd_id, _sysDate);            
        if (ret ==  FALSE ) 
        {
            PubErr(0, __FILE__, __LINE__, PXYM, "�������ڲ�����![%s]", 
                  prodinfo.pd_id);
            /*�޸ĸ�ǩԼЭ����Ӧ����*/
            PubStrncpy(_desc, "��Ʒ�������ڲ��������,ʧ��", 50);
            setRDFailInfo(&rd);
            if (updateFailRegularRedeem(rd) == FALSE)
            {
                PubErr(0, __FILE__, __LINE__, PXYM, "Э��[%d]�޸Ŀ�ʧ��!",
                    rd.ctrt_no);
                return FALSE;
            }
            continue; /*������һ��*/
        }

        PubErr(0, __FILE__, __LINE__, PXYM, 
              "20.[%s]ʵʱ����ģʽ[%s]", prodinfo.pd_id, mode);
        if (strncmp(mode, TRANS_FLAG_INST, 1) == 0) /* ʵʱģʽ = "2" */
        {
            /*ʵʱ��Ʒ�Ĵ�����, �ǼǴ�����*/
            /*ʵʱ�ۿ�*/
            ret = realtimeRegRedeem(rd, prodinfo, fpit);
            if (ret ==  FALSE ) 
            {
                PubErr(0, __FILE__, __LINE__, PXYM, "realtimeRegRedeem![%s]", 
                      rd.prodcode);
                setRDFailInfo(&rd);
                if (updateFailRegularRedeem(rd) == FALSE)
                {
                    PubErr(0, __FILE__, __LINE__, PXYM, "Э��[%d]�޸Ŀ�ʧ��!",
                        rd.ctrt_no);
                    return FALSE;
                }
                continue; /*������һ��*/
            }
        }
        else
        {
            /*������Ʒ�Ĵ��������ǼǴ�����*/
            ret = batchRegRedeem(rd, prodinfo, fpit);
            if (ret ==  FALSE ) 
            {
                PubErr(0, __FILE__, __LINE__, PXYM, "batchRegRedeem![%s]", 
                      rd.prodcode);
                setRDFailInfo(&rd);
                if (updateFailRegularRedeem(rd) == FALSE)
                {
                    PubErr(0, __FILE__, __LINE__, PXYM, "Э��[%d]�޸Ŀ�ʧ��!",
                        rd.ctrt_no);
                    return FALSE;
                }
                continue; /*������һ��*/
            }
        }
        /*�ɹ��޸Ķ�ͶǩԼ����سɹ���Ϣ*/
        rd.accu_succ_times ++;
        rd.accu_succ_amt += rd.rdm_share;
        rd.last_invest_date = _sysdate;
        rd.redm_times++;
        rd.last_invest_amt = rd.rdm_share;
        
        ret = begin_work();

        EXEC SQL update fs_regularredeem
                set accu_succ_times = :rd.accu_succ_times,
                    accu_succ_amt = :rd.accu_succ_amt,
                    last_invest_date = :rd.last_invest_date,
                    last_invest_amt = :rd.last_invest_amt,
                    redm_times = :rd.redm_times
              where ctrt_no = :rd.ctrt_no;
        if (SQLCODE)
        {
            PubErr(0, __FILE__, __LINE__, PXYM, "Э��[%d]�޸Ŀ�ʧ��!",
                        rd.ctrt_no);
            rollback_work();
            return FALSE;
        }
        commit_work;
    }
    
    return TRUE;
}

/*Ԥ�촦������*/
int prepareRegularRedeem(struct fs_regularredeem rd)
{
    int runday = 0;
    int rundate = 0;
    int i_date = 0;
    short mdy_array[3];
    
    rdefmtdate(&i_date, "yyyymmdd", "20110502"));
    rjulmdy(_sysDate, mdy_array);
    
    switch(rd.txdm_cycle[0])
    {
        case '1'; /*for week*/
            if (rd.last_invest_date < i_date) /*��һ��ִ��*/
            {
                /*��������ڼ�*/
                runday = rdayofweek(_sysDate); /*����������ܼ�*/
                if (runday == 0) runday = 7 ; /*����*/
                if (rd.txdm_date != runday ) /*����ִ��ǩԼ�ܼ�*/
                    return FALSE;
            }
            else 
            {
                rundate = rd.last_invest_date + rd.txdm_length * 7;
                if (rundate != _sysDate) /*����Ҫִ��*/
                    return FALSE;
            } /*end if*/
            break;
        case '2'; /*for month*/
            if (rd.last_invest_date < i_date) /*��һ��ִ��*/
            {
                if (rd.txdm_date != mdy_array[1]) /*����ǩԼִ����*/
                    return FALSE;
            }
            else
            {
                runday = getMonthDay(rd.last_invest_date, rd.txdm_length);
                rundate = rd.last_invest_date + runday ;
                if (rundate != _sysDate) /*����Ҫִ��*/
                    return FALSE;
            }
            break;
        default:
            break;
    }
    
    return TRUE;
}

/*ȡ��Ӧ�µ�����*/
int getMonthDay(int idate, int length)
{
    
    int monthday[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    short mdy_array[3];
    int i = 0;
    int month = 0;
    int day = 0;
    
    rjulmdy(idate, mdy_array);
    month = mdy_array[0];
    
    for (i=1; i<=length; i++)
    {
        if (month > 12)
            month = 1; /*���������*/
        
        day += monthday[month]; /*ȡ��Ӧ�����ۼ�*/
        
        if (month == 2) /*�����������*/
            day += leap(mdy_array[2]);

        month++;
    }
    return day;
}

/*��ĺ���s*/
int leap(int year)
{
    /*�ܱ�4�������ܱ�100�������ܱ�400����Ϊ����,������29��*/
    if( (year%4 == 0 && year%100 != 0) || (year%400 == 0) )
        return 1;
    else return 0;
}

/*���ù���ʧ����Ϣ����*/
int setRDFailInfo(struct fs_regularredeem *fail_rd)
{
    fail_rd->redm_times++; /*���ִ�д�����1*/
    fail_rd->accu_fail_times++; /*�ۼ���ش�����1*/
    fail_rd->last_invest_date = _sysDate; /*���ִ������*/
    fail_rd->last_invest_amt = 0.00; /*���ִ�н�ʧ��Ϊ��*/
    PubStrncpy(fail_rd->fail_reason, _desc, 50);
    return TRUE;
}

/*�޸Ķ��ڶ���ʧ�����*/
int updateFailRegularRedeem(struct fs_regularredeem fail_rd)
{
    EXEC SQL begin declare section; 
        struct fs_regularredeem rd;
    EXEC SQL end   declare section; 
    
    memcpy(&rd, fail_rd, sizeof(struct fs_regularredeem));
    
    EXEC SQL begin work;
    if (SQLCODE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, "���α����![%d]ctrt_no[%d]", 
                    SQLCODE, rd.ctrt_no);
        return FALSE;
    }
    EXEC SQL update fs_regularredeem
                set redm_times = :rd.redm_times,
                    accu_fail_times = :rd.accu_fail_times,
                    last_invest_date = :rd.last_invest_date,
                    last_invest_amt = :rd.last_invest_amt,
                    fail_reason = :rd.fail_reason
              where ctrt_no = :rd.ctrt_no;
    if (SQLCODE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, "�޸Ķ���ʧ�ܼ�¼��![%d]ctrt_no[%d]", 
                    SQLCODE, rd.ctrt_no);
        EXEC SQL rollback work;
        return FALSE;
    }
    
    EXEC SQL commit work;
    if (SQLCODE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, "�ύ�������![%d]ctrt_no[%d]", 
                    SQLCODE, rd.ctrt_no);
        EXEC SQL rollback work;
        return FALSE;
    }
    
    return TRUE;
}

/*������Դ�ͷ�*/
int freeListRegular()
{
    LISTREGULAR plist;
    
    /*��������Ҫ�ͷţ�ֱ�ӷ���*/
    if (first == NULL) return FALSE;
    /*�ͷŴ�ͷ�ڵ㿪ʼ������������Դ*/
    while (first)
    {
        plist = first->link;
        memset(&first->regred, 0x00, sizeof(struct fs_regularredeem));
        free(first);
        first = plist;
    }

    return TRUE;
}

/*����һ������*/
LISTREGULAR creatListRegular(const struct fs_regularredeem r)
{
    LISTREGULAR pNewNode;
    
    /*����һ���½ڵ�����*/
    if ((pNewNode = (LISTREGULAR)malloc(sizeof(REGULAR)))!=NULL)
    {
       memcpy(&pNewNode->regred, r, sizeof(struct fs_regularredeem));
       pNewNode->link = NULL;
       return pNewNode;
    }
    else return NULL;
}

/*��ʾ����*/
void printListRegular(LISTREGULAR head)
{
    link current = head;
    for( ; current; current=current->link)
    {
        printf("ctrtno = %d\n",  current->regred.ctrt_no);
        printf("acctno = %s\n",  current->regred.cust_trans_no);
        printf("prodcode = %s\n",  current->regred.prodcode);
        printf("redm_share = %d\n",  current->regred.redm_share);
        printf("sgn_org = %s\n",  current->regred.sgn_org);
        printf("txdm_cycle = %s\n",  current->regred.txdm_cycle);
        printf("txdm_date = %d\n",  current->regred.txdm_date);
    }
}

/* ȡ�������ǩԼ���� */
int getRegularRedeem()
{
    EXEC SQL begin declare section; 
        struct fs_regularredeem rd;
    EXEC SQL end   declare section;
    
    LISTREGULAR ptr=NULL, node=NULL;
    int ret;
    int rows = 0;
    
    EXEC SQL declare  regredcur CURSOR with hold for 
             select ctrt_no, 
                    txdm_mode         ,
                    txdm_cycle        ,
                    txdm_length       ,
                    txdm_date         ,
                    redm_seq          ,
                    redm_share        ,
                    status            ,
                    ta_org            ,
                    ta_id             ,
                    finance_org       ,
                    cust_trans_no     ,
                    prodcode          ,
                    accu_succ_times   ,
                    accu_succ_amt     ,
                    suspend_date      ,
                    recover_date      ,
                    provinceorgid     ,
                    sgn_date          ,
                    sgn_flowno        ,
                    sgn_org           ,
                    sgn_oper_id       ,
                    sgn_chan_id       ,
                    terminal_cond     ,
                    redm_times        ,
                    accu_fail_times   ,
                    last_invest_date  ,
                    last_invest_amt   ,
                    cust_level        ,
                    presenter_id_type ,
                    presenter_id_id   ,
                    presenter_name    ,
                    fail_reason       ,
                    remark1           ,
                    remark2
             from fs_regularredeem 
            where status = '0'
         order by prodcode, sgn_org, ctrt_no;
    
    EXEC SQL open regredcur ;
    if (SQLCODE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, "���α����:[%ld]", SQLCODE );
        return FALSE;
    }
    while (1)
    {
        memset(&rd, 0x00, sizeof(struct fs_regularredeem));
        EXEC SQL fetch  regredcur into 
                    rd.ctrt_no, 
                    rd.txdm_mode         ,
                    rd.txdm_cycle        ,
                    rd.txdm_length       ,
                    rd.txdm_date         ,
                    rd.redm_seq          ,
                    rd.redm_share        ,
                    rd.status            ,
                    rd.ta_org            ,
                    rd.ta_id             ,
                    rd.finance_org       ,
                    rd.cust_trans_no     ,
                    rd.prodcode          ,
                    rd.accu_succ_times   ,
                    rd.accu_succ_amt     ,
                    rd.suspend_date      ,
                    rd.recover_date      ,
                    rd.provinceorgid     ,
                    rd.sgn_date          ,
                    rd.sgn_flowno        ,
                    rd.sgn_org           ,
                    rd.sgn_oper_id       ,
                    rd.sgn_chan_id       ,
                    rd.terminal_cond     ,
                    rd.redm_times        ,
                    rd.accu_fail_times   ,
                    rd.last_invest_date  ,
                    rd.last_invest_amt   ,
                    rd.cust_level        ,
                    rd.presenter_id_type ,
                    rd.presenter_id_id   ,
                    rd.presenter_name    ,
                    rd.fail_reason       ,
                    rd.remark1           ,
                    rd.remark2 ;
                    
        if(SQLCODE && SQLCODE != 100)
        {
            PubErr(0, __FILE__, __LINE__, PXYM, "FETCH�α����:[%ld]", SQLCODE ); 
            EXEC SQL close regredcur;
            EXEC SQL free regredcur;
            return FALSE;
        }
        else if (SQLCODE == 100) 
        {
            PubErr(0, __FILE__, __LINE__, PXYM, 
                "���ڶ���[%ld]", rows);
            break;
        }
        
        node = creatListRegular(rd);
        
        if (ptr)
        {
           ptr->link = node;
           ptr = ptr->link;
        }
        else first = ptr = node; /*ָ������ĵ�һ���ڵ�*/
     
        rows++;
    }
    
    EXEC SQL close regredcur;
    EXEC SQL free  regredcur;
   
    return TRUE;
}

/*������ؼ�¼*/
int batchRegRedeem(struct fs_regularredeem rdeem, PROD_INFO_HQ pbasic, FS_P_INSTANCE_T t)
{
    EXEC SQL begin declare section; 
        struct fs_pending_list fpl;
        struct fs_prod_account prod;
        struct fs_sysflow sysflow;
        struct fs_flowrltv flowrltv;
    EXEC SQL end   declare section;
    char cust_type[10+1];
    char auto_flag[6+1] = "\0";
    
    int ret = 0;
    
    memset(&fpl, 0x00, sizeof(struct fs_pending_list));
    memset(&prod, 0x00, sizeof(struct fs_prod_account));
    memset(&sysflow, 0x00, sizeof(struct fs_sysflow));
    memset(&flowrltv, 0x00, sizeof(struct fs_flowrltv));
    memset(cust_type, 0, sizeof(cust_type));
    
    /*���ô�������Ϣֵ*/
    useRegularSetPending(&fpl, rdeem, pbasic);

    ret = begin_work();
    if (ret != TRUE)
    {
        PubErr (0, __FILE__, __LINE__, PXYM,  
          "���������![%ld]", SQLCODE);
        return FALSE;
    }

    /*�˽����������У��������*/
    ret = insert_sysflow_usepend(0, fpl, &sysflow, &flowrltv);
    if (ret == FALSE)
    {
        PubErr(-1,__FILE__,__LINE__,PXYM, "����ˮ��[%s]", fpl.consign_id);
        return FALSE; 
    }

    ret = commit_work();
   
    ret = getProdAccountInfo(fpl.ta_org, fpl.ta_id, fpl.finance_org,
                   fpl.cust_trans_no, fpl.prodcode, &prod);
    if (ret == FALSE)
    {
        PubErr(-1,__FILE__,__LINE__,PXYM, 
            "ȡ��Ʒ�ʻ���¼��org[%s]id[%s]fina[%s]cust[%s]prod[%s]",
            fpl.ta_org, fpl.ta_id, fpl.finance_org,
            fpl.cust_trans_no, fpl.prodcode);
        return FALSE; 
    }
    
    ret = begin_work();
    if (ret != TRUE)
    {
        PubErr (0, __FILE__, __LINE__, PXYM,  
          "���������![%ld]", SQLCODE);
        return FALSE;
    }
    
    /*����Զ���ƹ�ģ*/
    ret = dealAutoSignInfo(fpl.ta_org, fpl.ta_id, fpl.finance_org, 
                           fpl.cust_trans_no, fpl.prodcode, pbasic.pd_id, auto_flag);
    if (ret == FALSE)
    {
        rollback_work();
        
        PubStrncpy(sysflow.txstatus, "2", 1);
        PubStrncpy(sysflow.errcode, "B102006", sizeof(sysflow.errcode)-1);
        PubStrncpy(sysflow.errdesc, "�Զ����ǩԼ��ģ����", sizeof(sysflow.errdesc)-1);
        
        begin_work();
        ret = updSysflowInfo(&sysflow);
        
        PubStrncpy(fpl.exec_status, "31", 2);
        PubStrncpy(fpl.ret_code, sysflow.errcode, sizeof(fpl.ret_code)-1);
        PubStrncpy(fpl.errdesc, sysflow.errdesc, sizeof(fpl.errdesc)-1);

        /** fspendinglist_DAC(&fpl, SETDAC);
        ret = updatePendingListValue(fpl); **/
        PubStrncpy(_desc, sysflow.errdesc, 50);
        commit_work();
        return FALSE;
    }
    
    /*���ݶ�Ϸ��Բ������¶���ݶ�*/
    if (fpl.app_share - (prod.share - prod.share_freeze - prod.share_freeze_sp) > 0.00005 )
    {
        PubErr(-1,__FILE__,__LINE__,PXYM, 
            "����ݶ�[%.2f]>��ǰ�ݶ�[%.2f]-����ݶ�[%.2f]",fpl.app_share, 
            prod.share, prod.share_freeze+prod.share_freeze_sp);
        PubStrncpy(sysflow.txstatus, "2", 1);
        PubStrncpy(sysflow.errcode, "B102006", sizeof(sysflow.errcode)-1);
        PubStrncpy(sysflow.errdesc, "�������ݶ����", sizeof(sysflow.errdesc)-1);
        ret = updSysflowInfo(&sysflow);
        
        PubStrncpy(fpl.exec_status, "31", 2);
        PubStrncpy(fpl.ret_code, sysflow.errcode, sizeof(fpl.ret_code)-1);
        PubStrncpy(fpl.errdesc, sysflow.errdesc, sizeof(fpl.errdesc)-1);

        /** fspendinglist_DAC(&fpl, SETDAC);
        ret = updatePendingListValue(fpl); **/
        PubStrncpy(_desc, sysflow.errdesc, 50);
        commit_work();
        return FALSE; 
    }
    
    /*���޶����*/
    PubStrTrim(fpl.remark2);
    PubStrTrim(fpl.cust_type);
    if(strncmp(fpl.cust_type, "0", 1) == 0)
        PubStrncpy(cust_type, "01", 2);
    else if(strncmp(fpl.cust_type, "1", 1) == 0)
        PubStrncpy(cust_type, "10", 2);
    else
        memset (cust_type, 0, sizeof(cust_type));
    PubStrTrim(cust_type);
    ret = checkRedeemCondtion(pbasic.pd_id, fpl.app_share, prod, fpl.remark2, "0", t, "1",cust_type);
    if (ret == FALSE)
    {
        PubStrncpy(sysflow.txstatus, "2", 1);
        PubStrncpy(sysflow.errcode, _code, sizeof(sysflow.errcode)-1);
        PubStrncpy(sysflow.errdesc, _desc, sizeof(sysflow.errdesc)-1);
        ret = updSysflowInfo(&sysflow);

        PubStrncpy(fpl.exec_status, "31", 2);
        PubStrncpy(fpl.ret_code, sysflow.errcode, sizeof(fpl.ret_code)-1);
        PubStrncpy(fpl.errdesc, sysflow.errdesc, sizeof(fpl.errdesc)-1);

        /** fspendinglist_DAC(&fpl, SETDAC);
        ret = updatePendingListValue(fpl); **/
        PubStrncpy(_desc, sysflow.errdesc, 50);
        commit_work();
        return FALSE; 
    }
    
    /*�޸Ķ���ݶ�*/
    prod.share_freeze = prod.share_freeze + fpl.app_share;
    
    /*��DAC�ֶ�*/
    fsprodaccount_DAC(&prod, SETDAC);

    ret = updProdAccount(&prod);
    if (ret == FALSE)
    {
        PubErr(-1,__FILE__,__LINE__,PXYM, "�����޸Ĳ�Ʒ�ʻ���Ϣʧ��!");
        rollback_work();
        return FALSE;
    }

    /*�����ģ*/
    ret = dealScaleCtrl(fpl.ta_org, fpl.ta_id, fpl.finance_org, fpl.cust_trans_no, fpl.tx_orgid, 
          fpl.app_share, pbasic, OPER_TYPE_REDEEM, "1", sysflow, "0");
    if (ret == FALSE)
    {
        PubErr(-1,__FILE__,__LINE__,PXYM, "������ع�ģ����");
        rollback_work();
        
        begin_work();
        PubStrncpy(sysflow.txstatus, "2", 1);
        PubStrncpy(sysflow.errcode, "B102006", sizeof(sysflow.errcode)-1);
        PubStrncpy(sysflow.errdesc, "������ع�ģ��", sizeof(sysflow.errdesc)-1);
        ret = updSysflowInfo(&sysflow);

        PubStrncpy(fpl.exec_status, "31", 2);
        PubStrncpy(fpl.ret_code, sysflow.errcode, sizeof(fpl.ret_code)-1);
        PubStrncpy(fpl.errdesc, sysflow.errdesc, sizeof(fpl.errdesc)-1);

        /** fspendinglist_DAC(&fpl, SETDAC);
        ret = updatePendingListValue(fpl); **/
        PubStrncpy(_desc, sysflow.errdesc, 50);
        commit_work();
        return FALSE;
    }
    
    PubStrncpy(fpl.exec_status, "00", 2);
    fpl.upd_date = _sysDate;
    memset (fpl.ret_code, 0x00, sizeof(fpl.ret_code));
    memset (fpl.errdesc, 0x00, sizeof(fpl.errdesc));
    PubStrncpy (fpl.txtime, order.bgntime, sizeof(fpl.txtime)-1);
    PubStrncpy (fpl.remark4, auto_flag, sizeof(fpl.remark4)-1);
    
    fspendinglist_DAC(&fpl, SETDAC);
    /*�������*/
    ret = insPendingList(fpl);
    if (ret == FALSE)
    {
        PubErr(-1, __FILE__, __LINE__, PXYM,
                "�޸Ĵ�����ʧ��[%d]", SQLCODE);
        rollback_work();
        return FALSE;
    }
    
    PubStrncpy(sysflow.txstatus, "9", 1);
    ret = updSysflowInfo(&sysflow);

    ret = commit_work();
    
    return TRUE;
}

/*�ö��ڶ���ṹ��������ֵ*/
int useRegularSetPending(struct fs_pending_list *p, struct fs_regularredeem rdeem,
        PROD_INFO_HQ pdinfo )
{
    FS_CUST_PROD_AGR_T cpa;
    char id_type[10+1], id_id[20];
    
    p->txdate = _sysDate; /*ϵͳ����*/
    PubGetSysFlowNo(p->sysflowno); /*��ˮ*/
    p->flow_list_no = 0;
    PubGetConsignID(p->consign_id);  /*ί�е�*/
    PubGetDateTime(p->txtime, sizeof(p->txtime));
    
    PubStrncpy(p->tx_orgid , rdeem.sgn_org, 9);
    PubStrncpy(p->tx_operid , rdeem.sgn_oper_id, 12);
    PubStrncpy(p->tx_channl, rdeem.sgn_chan_id, 10);
    PubStrncpy(p->oper_type, OPER_TYPE_REDEEM, 10);
    
    PubStrncpy(p->ta_org       , rdeem.ta_org       ,4 ) ;
    PubStrncpy(p->ta_id        , rdeem.ta_id        ,26 );
    PubStrncpy(p->finance_org  , rdeem.finance_org  ,4 ) ;
    PubStrncpy(p->cust_trans_no, rdeem.cust_trans_no,28 );
    PubStrncpy(p->prodcode     , rdeem.prodcode     ,19 );
    p->list_no = 0;
    p->asst_code[0] = '\0';
    p->periodtimes = 0;
    p->svc_ctrt_no = rdeem.ctrt_no; /*��Э����*/
    PubStrncpy(p->cust_acct_no, rdeem.cust_trans_no,28 );
    p->bailacctno[0] = '\0';
    
    /*ȡ�ֺ췽ʽ ��ص�ʱ����Ҫ�ֺ췽ʽ��*/
    /** memset (&cpa, 0x00, sizeof(FS_CUST_PROD_AGR_T));
    PubStrncpy(cpa.prodcode, rdeem.prodcode, sizeof(cpa.prodcode)-1);
    PubStrncpy(cpa.ta_org, rdeem.ta_org, sizeof(cpa.ta_org)-1);
    PubStrncpy(cpa.ta_id, rdeem.ta_id, sizeof(cpa.ta_id)-1);
   
    if (strncmp(pdinfo.dividend, "01", 2)==0)
    {
        p->profitmode[0] = '0';
    }
    else if (strncmp(pdinfo.dividend, "10", 2)==0)
    {
        p->profitmode[0] = '1';
    }
    else 
    {
         ret = getCustProdAgr(&cpa);
         if (ret != TRUE)
         {
             PubErr(-1, __FILE__, __LINE__, PXYM, "[%s][%d]ȡ�ֺ췽ʽ��![%d]",
                    cpa.prodcode, ctrt_no, sqlca.sqlcode);
             return FALSE;
         }
         p->profitmode[0] = cpa.profitmode[0];
    }    **/
    /* freeze_date 
        spec_no */
    PubStrncpy(pdl.curr_code, pdinfo.trade_cur, 10); /*��Ʒ*/
    PubStrncpy(p->provinceorgid, rdeem.provinceorgid, 9);
    p->app_amt_bknt = 0.00;
    p->app_amt_fnex = 0.00;
    p->app_share = rdeem.redm_share; /*��طݶ�*/
    p->cfm_amt_bknt = 0.00;
    p->cfm_amt_fnex = 0.00; 
    p->cfm_share = 0.00;
    p->accu_cfm_amt_bknt = 0.00; 
    p->accu_cfm_amt_fnex = 0.00;
    p->accu_cfm_share = 0.00;
    p->fee_amt_bknt = 0.00;
    p->fee_amt_fnex = 0.00;
    p->tax_amt = 0.00;     
    p->bwamt   = 0.00;     
    /* p->min_exec_date */
    PubStrncpy(p-> redm_seq, rdeem.redm_seq, 10); 
    p->amt_cfm_date = _sysDate;
    PubGetSysFlowNo(p->amt_cfm_flowno);
    p->cfm_date = _sysDate;
    PubGetSysFlowNo(p->cfm_flowno);
    
    PubStrncpy(p->exec_status, EXEC_STS_RAW, 10); /*��Ϊ������*/
    PubStrncpy(p->ta_status, TA_STATUS_RAW, 10); /*ta status*/
    PubStrncpy(p->acct_status, ACCT_STATUS_RAW, 10); /*acct status*/
    /* ret_code, errdesc*/
    PubStrncpy(p->cust_level, rdeem.cust_level, 10); /*�ͻ��ȼ�*/
    /** txmediumtype
    operator_id_type
    operator_id_id
    operator_name
    order_valid_day
    colossal_redm_flag  **/
    getCustInfoForTaAccount(rdeem.ta_org, rdeem.ta_id, id_type, id_id, p->cust_type) ;
    PubStrncpy(p->promotion_oper_id  , rdeem.promotion_oper_id , 12);    
    PubStrncpy(p->promotion_oper_org , "", 9);
    Pubstrncpy(p->promotion_oper_name, rdeem.promotion_oper_name, 40); 
 /* promotion_plan_id
sale_discount_rate
adm_discount_rate
discount_apvl_id  */
    PubGetDateTime(p->cfm_time, sizeof(p->cfm_time) );
    PubStrncpy(p->tx_source, "6", 1); /*����������Դ�����ڶ���*/
/* dcc_acct_flowno
fee_way
risk_matching_stat
risk_qust_id
cancel_date
cancel_flowno
itemize_flag
force_redm_reason_flag
chg_acct_date
int_amt_bknt
int_amt_fnex
int_cnvt_share
yield_rate
floating_rate
bnft_amt_bknt
bnft_amt_fnex
presenter_id_type
presenter_id_id
remark1
remark2
remark3
remark4
remark5   DAC */
    p->upd_date = _sysDate;
    
}

/*ʵʱ��Ʒ���괦��*/
int realtimeRegRedeem(struct fs_regularredeem rdeem, PROD_INFO_HQ pbasic, FS_P_INSTANCE_T t)
{
    EXEC SQL begin declare section; 
        struct fs_pending_list pl;
    EXEC SQL end   declare section;
    int ret = 0;
    
    /*���ô�������Ϣֵ*/
    useRegularSetPending(&pl, rdeem, pbasic);

    ret = begin_work();
    fspendinglist_DAC(&pl, SETDAC);
    /*�������*/
    ret = insPendingList(pl);
    if (ret == FALSE)
    {
        rollback_work();
        return FALSE;
    }
    commit_work();
    
    /*�黹����*/
    ret = batchInstRedeem(pbasic, pl);
    if (ret == TRUE)
    {
        /*�޸Ĵ�����*/
        PubStrncpy(pl.exec_status, "91", 2);
        PubStrncpy(pl.acct_status, "9", 1);
        pl.chg_acct_date = _sysDate;
    }
    else
    {
        /*�޸Ĵ�����״̬*/
        PubErr(0, __FILE__, __LINE__, PXYM, "����ʧ��, ί�е���: [%s]", pl.consign_id);
        PubStrncpy(pl.exec_status, "31", 2);
        PubStrncpy(pl.acct_status, "2", 1);
        PubStrncpy(pl.ret_code, _code, sizeof(pl.ret_code)-1);
        PubStrncpy(pl.errdesc, _desc, sizeof(pl.errdesc)-1);
    }
    /*��������*/
    pl.upd_date = _sysDate;
    pl.amt_cfm_date = _sysDate; /*���ȷ������,����ʹ��*/

    fspendinglist_DAC(&pl, SETDAC); /*����DAC*/
    ret = updatePendingListValue(pl);
    if (ret == FALSE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, "%s: [%s][%s]�޸Ĵ������!",
              _sysDate, pl.prodcode, pl.cust_trans_no);
        return FALSE;
    }
    
    return TRUE;
}

