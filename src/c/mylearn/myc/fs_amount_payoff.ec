/*********************************************************
*  程序名:   fs_amount_payoff.ec
*  功能描述: 资金清算结算处理
**  内部函数:
**
**  编程人员:  谭文均
**  最后修改日期: 20090512
*********************************************************/

EXEC SQL include "fs_mframe.h";


/*资金清算数据源处理*/
int payoffDataSource(char *prodid, char *plan_type, int periodtimes, int taskno)
{
EXEC SQL begin declare section;
    struct fs_prod_net_buy_stat pdata;
    char prodcode[19] = "\0";
    int upd_date = 0;
    int oper_date = 0;
EXEC SQL end   declare section;
    
    PROD_INFO_HQ prodinfo;
    struct fs_provinceorglocale provl[MAX_INDEX_VALUE];
    int rows = 0;
    int x = 0;
    int ret = 0;
    
    /* 根据产品性质检查是否执行 */
    memset (&prodinfo, 0x00, sizeof(PROD_INFO_HQ) );
    ret = getProdInfoHQ(prodid, &prodinfo);
    if (ret == FALSE)
    {
        PubErr (-1,__FILE__, __LINE__, CALLFUNCERR, CALLFUNCMSG, 
                        "getProdInfoHQ", prodid, plan_type, periodtimes);
        return FALSE;
    } /*end if*/
    
    ret = getCapitalOperDate(prodinfo.pd_id, "0201", &oper_date);
    if (ret == FALSE)
    {
        PubErr (-1,__FILE__, __LINE__, CALLFUNCERR, CALLFUNCMSG, 
                        "getCapitalOperDate", prodid, plan_type, periodtimes);
        return FALSE;
    } /*end if*/
    else if (ret == RECORD_NOT_FOUND)
    {
        PubErr (-1,__FILE__, __LINE__, CALLFUNCERR, CALLFUNCMSG, 
                        "未设清算帐户数据", prodid, plan_type, periodtimes);
        return TRUE;
    }
    
    /*取销售机构*/
    ret = getSalesOrg(prodinfo.pd_id, prodinfo.releasearea, provl);
    if (ret == FALSE)
    {
        PubErr (-1,__FILE__, __LINE__, CALLFUNCERR, CALLFUNCMSG, 
                        "getSalesOrg", prodid, plan_type, periodtimes);
        return FALSE;
    } /*end if*/
    
    /*取计划日期*/
    ret = get_plan_period_date(prodid, plan_type, periodtimes, &upd_date) ;
    if (ret == FALSE)
    {
       PubErr(0, __FILE__, __LINE__, PXYM, "取period_date ERROR![%d]", SQLCODE);
       return FALSE;
    }
    
    /*取产品号*/
    PubStrncpy(prodcode, prodid, 19);
    
    ret = begin_work();
    if (ret == FALSE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, 
            "打开事物错sqlcode=%ld", SQLCODE);
        return FALSE;
    }
       
    EXEC SQL declare  conc_0 CURSOR FOR 
        Select prodcode, provinceorgid,
            nvl(sum(decode(oper_type,'2',cfm_amt_bknt+cfm_amt_fnex, 'G', cfm_amt_bknt+cfm_amt_fnex)), 0.00)
            as app_amt,
            nvl(sum(decode(oper_type, '3',cfm_amt_bknt+cfm_amt_fnex)), 0.00)
            as redeem_amt,
            nvl(sum(decode(oper_type, '3',bnft_amt_bknt+bnft_amt_fnex)),0.00)
            as bnft_amt
        from fs_conclude_list
         where prodcode = :prodcode 
           and txdate >= :oper_date 
           and txdate <= :upd_date 
           and oper_type in ('2', 'G', '3')
           and netting_status = '0' 
        group by prodcode, provinceorgid;
    
    EXEC SQL open conc_0 ;
    if (SQLCODE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, 
                "open currsor ERROR!sqlcode=%ld", 
                SQLCODE);
        rollback_work();
        return FALSE;
    }
    while (1)
    {
        memset (&pdata, 0, sizeof( struct fs_prod_net_buy_stat pdata));
        EXEC SQL fetch  conc_0 into 
            :pdata.prodcode, :pdata.orgid, :pdata.app_pur_amt,
            :pdata.redm_amt, :pdata.bnft_amt;
        if(SQLCODE && SQLCODE != 100)
        {
            PubErr(0, __FILE__, __LINE__, PXYM, 
                "fetch currsor ERROR!sqlcode=%ld", 
                SQLCODE);
            rollback_work();
            EXEC SQL close conc_0;
            EXEC SQL free conc_0;
            return FALSE;
        }
        else if (SQLCODE == 100) 
        {    
            PubErr(0, __FILE__, __LINE__, PXYM, 
                    "帐务表划帐处理记录数[%ld]", rows); 
            break;
        }
        
        pdata.clear_date = upd_date;
        pdata.periodtimes = periodtimes;
        PubStrncpy(pdata.type, "0201", 4); /*资金清算*/
        PubStrncpy(pdata.prodname, prodinfo.prod_name, sizeof(pdata.prodname)-1);
        pdata.balance_amt = pdata.app_pur_amt - pdata.redm_amt - pdata.bnft_amt;
        pdata.oper_flag[0] = '0'; /*待处理*/
        PubStrncpy(pdata.curr_code, prodinfo.trade_cur, sizeof(pdata.curr_code)-1);
        x = 0;
        while(provl[x].provinceorgid[0] && x<MAX_INDEX_VALUE)
        {
            /*检测到相应销售机构*/
            if (strncmp(provl[x].provinceorgid, pdata.orgid, 9) == 0 )
            {
                PubStrncpy(pdata.orgname, provl[x].orgname, sizeof(pdata.orgname)-1);
                break;
            }
            x++;
        }
        
        /*新增零数据不入表*/
        if (pdata.balance_amt > 0.0005 ||pdata.balance_amt < -0.0005)
        {
            /*入资金清算表*/
            ret = insert_prod_net_buy_stat(pdata);
            if (ret == FALSE)
            {
                PubErr (-1,__FILE__, __LINE__, CALLFUNCERR, CALLFUNCMSG, 
                            "insert_prod_net_buy_stat", prodid, plan_type, periodtimes); 
                rollback_work();
                return FALSE;
            }
        }

        rows++;
    }
    
    EXEC SQL close conc_0;
    EXEC SQL free conc_0;
    
    EXEC SQL insert into fs_concludepaylist(txdate, sysflowno, pay_date, status, prodcode)
             select txdate, sysflowno, today, '0', prodcode
             from fs_conclude_list
         where prodcode = :prodcode 
           and txdate >= :oper_date 
           and txdate <= :upd_date 
           and oper_type in ('2', 'G', '3')
           and netting_status = '0' ;
    if (SQLCODE)
    {
         PubErr (-1,__FILE__, __LINE__, CALLFUNCERR, CALLFUNCMSG, 
                            "fs_concludepaylist", prodid, plan_type, periodtimes); 
         rollback_work();
         return FALSE;
    }
    
    /*提交事物*/
    commit_work();
    PubErr(0, __FILE__, __LINE__, PXYM, 
          "产品[%s]计划[%s] 机构记录数被处理[%ld]", prodid, plan_type, rows);

    return TRUE;
}

/*取资金清算开始日期*/
int getCapitalOperDate(const char *prod_id, const char *typ, int *oper_date)
{
   EXEC SQL BEGIN DECLARE SECTION; 
      char pd_id[27+1]; 
      char type[10+1]; 
      int  date; 
   EXEC SQL END DECLARE SECTION;
   
   PubStrncpy(pd_id, prod_id, sizeof(pd_id)-1);
   PubStrncpy(type, typ, sizeof(type)-1);
   
   EXEC SQL SET CONNECTION CONN_PDDB;
   
   EXEC SQL select nvl(max(oper_date), today)
                   into :date 
             from capital_rule
            where pd_id = :pd_id
              and type = :type;
   if (SQLCODE && SQLCODE != 100)
   {
      PubErr(0,__FILE__,__LINE__,PXYM, "SQLCODE=%d", SQLCODE);
      EXEC SQL SET CONNECTION CONN_IPSSDB;
      return FALSE ;
   }
   else if (SQLCODE == 100)
   {
      EXEC SQL SET CONNECTION CONN_IPSSDB;
      return RECORD_NOT_FOUND;
   }
   EXEC SQL SET CONNECTION CONN_IPSSDB;
   
   *oper_date = date;
   
   return TRUE;
}

/*根据成交入帐修改成交表*/
int updConcludeForPayList(char *prodid, char *plan_type, int periodtimes, int taskno)
{
EXEC SQL begin declare section;
   char prodcode[19] = "\0";
   int txdate = 0;
   char sysflowno[19] ="\0";
   int rows = 0;
EXEC SQL end   declare section;

    PubStrncpy(prodcode, prodid, 19);
    
    ret = begin_work();
    if (ret == FALSE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, 
            "打开事物错sqlcode=%ld", SQLCODE);
        return FALSE;
    }
    
    EXEC SQL declare  conc_1 CURSOR FOR 
       select txdate, sysflowno 
        from fs_concludepaylist
       where pay_date = today
         and status = '0'
         and prodcode = :prodcode;

    EXEC SQL open conc_1 ;
    if (SQLCODE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, 
                "open currsor ERROR!sqlcode=%ld", 
                SQLCODE);
        rollback_work();
        return FALSE;
    }
    while (1)
    {
        memset (sysflowno, 0, sizeof(sysflowno));
        EXEC SQL fetch  conc_1 into 
            :txdate, :sysflowno
        if(SQLCODE && SQLCODE != 100)
        {
            PubErr(0, __FILE__, __LINE__, PXYM, 
                "fetch currsor ERROR!sqlcode=%ld", 
                SQLCODE);
            rollback_work();
            EXEC SQL close conc_1;
            EXEC SQL free conc_1;
            return FALSE;
        }
        else if (SQLCODE == 100) 
        {
            PubErr(0, __FILE__, __LINE__, PXYM, 
                    "修改成交记录数[%ld]", rows); 
            break;
        }

        EXEC SQL update fs_conclude_list 
                    set status = '9'
                  where txdate = :txdate
                    and sysflowno = :sysflowno 
                    and prodcode = :prodcode;
        if (SQLCODE)
        {
            PubErr(0, __FILE__, __LINE__, PXYM, 
                "update conclude ERROR!sqlcode=%ld", 
                SQLCODE);
            rollback_work();
            return FALSE;
        }
        rows++;
    }
    
    EXEC SQL close conc_1;
    EXEC SQL free conc_1;

    /*清理成交清算临时表*/
    EXEC SQL delete from fs_concludepaylist
        where pay_date = today
         and status = '0'
         and prodcode = :prodcode;
    if (SQLCODE)
    {
        PubErr(0, __FILE__, __LINE__, PXYM, 
                "delete concludelist ERROR!sqlcode=%ld", 
                SQLCODE);
        rollback_work();
        return FALSE;
    }

    if (commit_work() != TRUE)
    {
        rollback_work();
        return FALSE;
    }
    
    return TRUE;
}

