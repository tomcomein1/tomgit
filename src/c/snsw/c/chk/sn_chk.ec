#include "para_macro.h"
EXEC SQL include sn_chk.h;

#define AAA "ACOM"
#define NEXT_LINE 100
#define SHOW_CHAR "*"
char CHK_YEAR[4+1]="2018"; 
int KAO_KAO=0;

void usage()
{
   printf("**********************************************\n");
   printf(" please input check date\n");
   printf(" sn_chk <date> [PRE|ITL|SEC|THI|FOU|ZZ] [D|year]\n");
   printf("**********************************************\n");
}

int main(int argc, char *argv[])
{
   int i=0; 
   
   printf("Start snbank check account.\n");
   if (argc < 2) 
   {
      usage();
      exit(0);
   }


   if (connect_database(NULL) != 0) 
   {
      return -1;
   }

   strncpy(CHK_YEAR, argv[1], 4);
   /* printf("this year is [%s]\n", CHK_YEAR); */
   /* 预处理t_tracelog表 */
   if(strcmp(argv[2], "PRE")==0) {
     if(insert_tracelog(argv[1], argv[3])!=0) goto END_CHK;
   } else if(strcmp(argv[2], "ITL")==0) {
     insert_day_tracelog(argv[1]);
     goto END_CHK;
   } else if(strcmp(argv[2], "SEC")==0) {
     if(strlen(argv[3])==4) {
        KAO_KAO=atol(argv[3]);
        printf("last year %d\n", KAO_KAO);
     }

     process_second_chk(argv[1]);
     goto END_CHK;
   } else if(strcmp(argv[2], "THI")==0) {
     process_third_chk(argv[1]);
     goto END_CHK;
   } else if(strcmp(argv[2], "FOU")==0) {
     process_fourth_chk(argv[1]);
     goto END_CHK;
   } else if(strcmp(argv[2], "ZZ")==0) {
     process_zz_chk(argv[1]);
     goto END_CHK;
   }

   delete_chkerr(argv[1]);
   process_chk(argv[1]);
   
END_CHK:
   close_database(); 
   printf("End snbank check account.\n");
   return 0;
}

int get_other_date(char *curr_date, char *befor_date, char *last_date)
{
EXEC SQL BEGIN DECLARE SECTION;
   char cdate[8+1];
   char bdate[8+1];
   char ldate[8+1];
EXEC SQL END DECLARE SECTION;

   memset(cdate,0,sizeof(cdate));
   memset(bdate,0,sizeof(bdate));
   memset(ldate,0,sizeof(ldate));
   strcpy(cdate, curr_date);

   EXEC SQL select to_char(extend(to_date(:cdate,'%Y%m%d'), year to day )-interval(2) day to day,'%Y%m%d')
                   ,to_char(extend(to_date(:cdate,'%Y%m%d'), year to day )+interval(1) day to day,'%Y%m%d')
                   into :bdate,:ldate 
              from dual;

   strcpy(befor_date, bdate);
   strcpy(last_date, ldate);
   /* if (_DEBUG) printf("befor date=%s,sqlcode=%d\n", befor_date,sqlca.sqlcode); */

   return sqlca.sqlcode;
}

int process_second_chk(char *chk_date)
{
EXEC SQL BEGIN DECLARE SECTION;
  char chkdate[8+1];
  T_TRACELOG trlog;
  T_CHKACCT_ERR chk;
  double tx_amt;
  char ac_no[19+1];
  char req_ejfno[15+1];
  char sys_ejfno[12+1];
  char beg_sys_date[8+1];
  char end_sys_date[8+1];
  char sys_date[8+1];
  char fn4[4+1];
EXEC SQL END DECLARE SECTION;
  int num=0;
  int ret = 0;

   memset(chkdate, 0, sizeof(chkdate));
   strcpy (chkdate, chk_date);

   EXEC SQL declare chk_cursor cursor with hold for
            select * from t_chkacct_err
               where chk_date=:chkdate
                 and err_flag='100';

   EXEC SQL open chk_cursor;
   if(sqlca.sqlcode) 
   {
      SHOWSQLERR;
      return -1;
   }

   for(;;) 
   {
      memset(&chk, 0, sizeof(chk) );    
      EXEC SQL fetch chk_cursor into :chk;
      if(sqlca.sqlcode !=0 && sqlca.sqlcode !=100) {
         SHOWSQLERR;
         EXEC SQL close chk_cursor;
         EXEC SQL free  chk_cursor;
         return -1;
      }
      else if (sqlca.sqlcode == 100)
            break;
       
      rtrim(chk.tx_date);
      rtrim(chk.acct_no);
      rtrim(chk.src_file);
      rtrim(chk.cati);
      memset(req_ejfno,0,sizeof(req_ejfno));
      sprintf(req_ejfno, "%015ld", chk.sys_no);
      memset(sys_ejfno,0,sizeof(sys_ejfno));
      sprintf(sys_ejfno, "%012ld", chk.sys_no);
 /* printf("+++++%s|%d|%s|%s|\n", chk.tx_date,chk.sys_no,chk.acct_no, req_ejfno); */

      memset(beg_sys_date,0,sizeof(beg_sys_date));
      memset(end_sys_date,0,sizeof(end_sys_date));

      if(strcmp(chk.tx_date, "1231") == 0) {
             sprintf(CHK_YEAR, "%d", KAO_KAO);
             printf("year:last year is %s\n", CHK_YEAR);
      }
      
      /* sprintf(sys_date, "%s%s", CHK_YEAR, chk.tx_date); */
      strcpy(sys_date, chk.tx_date);
      get_other_date(sys_date, beg_sys_date, end_sys_date);

      memset(&trlog, 0, sizeof(trlog));
      memset(fn4, 0, sizeof(fn4));
      strncpy(fn4, chk.src_file+strlen(chk.src_file)-4,4);
      /** 1. III-他代本 2. AAA-本代他 **/
      if( 0 != strcmp(fn4, AAA) ) {
          if(strncmp(chk.msg_type,"0420", 4)==0) {
            EXEC SQL select first 1 * into :trlog from t_tracelog 
                where sys_date >= :beg_sys_date
                  and sys_date <= :end_sys_date
                  and req_ejfno= :req_ejfno
                  and ac_no = :chk.acct_no 
                  and trim(tm_no) = :chk.cati
                  and sts <> '0'
		  and substr(tx_code,-2)='cz'
                  and respcd='0000';
          } else {
            EXEC SQL select first 1 * into :trlog from t_tracelog 
                where sys_date >= :beg_sys_date
                  and sys_date <= :end_sys_date
                  and req_ejfno= :req_ejfno
                  and ac_no = :chk.acct_no 
                  and trim(tm_no) = :chk.cati
                  and sts <> '0'
		  and substr(tx_code,-2)!='cz'
                  and respcd='0000';
          }
      } else {
          if(strncmp(chk.msg_type,"0420", 4)==0) {
             EXEC SQL select * into :trlog from t_tracelog 
                where sys_date >= :beg_sys_date
                  and sys_date <= :end_sys_date
                  and sys_ejfno= :sys_ejfno
                  and ac_no = :chk.acct_no 
                  and trim(tm_no) = :chk.cati
                  and sts <> '0'
		  and substr(tx_code,-2)='cz'
                  and respcd='0000';
	  } else {
             EXEC SQL select * into :trlog from t_tracelog 
                where sys_date >= :beg_sys_date
                  and sys_date <= :end_sys_date
                  and sys_ejfno= :sys_ejfno
                  and ac_no = :chk.acct_no 
                  and trim(tm_no) = :chk.cati
                  and sts <> '0'
		  and substr(tx_code,-2)!='cz'
                  and respcd='0000';
	  }
      }
      if(sqlca.sqlcode != 0 && sqlca.sqlcode != 100) {
         printf("t_tracelog,sys_date>='%s',sys_date<='%s',req_ejfno='%s',ac_no='%s',sys_ejfno=%s.sqlcode=%d\n",
                 beg_sys_date, end_sys_date, req_ejfno, chk.acct_no,sys_ejfno,sqlca.sqlcode);
         SHOWSQLERR;
         EXEC SQL close chk_cursor;
         EXEC SQL free  chk_cursor;
         return -1;
      } else if (sqlca.sqlcode == 100) {
         /* WriteLog(ERR_LVL,"ERR:[%s],%s|%s|req_ejfno:%s|ac_no:%s|sys_ejfno:%s|tm_no:%s|",
                 fn4, beg_sys_date, end_sys_date, req_ejfno, chk.acct_no,sys_ejfno,chk.cati);
         */
         printf("[%s],%s|%s|req_ejfno:%s|ac_no:%s|sys_ejfno:%s|tm_no:%s|\n",
                 fn4, beg_sys_date, end_sys_date, req_ejfno, chk.acct_no,sys_ejfno,chk.cati);
      } else {
         rtrim(trlog.req_date);
         rtrim(trlog.ac_no);
         rtrim(trlog.tel);
         rtrim(trlog.sts);
         strcpy(chk.pt_tx_code, rtrim(trlog.tx_code));
         sprintf(chk.tx_name, "%s:%s", rtrim(trlog.node0),rtrim(trlog.tx_name));

         strcpy(chk.sys_date, trlog.sys_date);
         chk.sys_ejfno=atol(trlog.sys_ejfno);
         chk.pt_amt = trlog.tx_amt;
         strcpy(chk.pt_ac_no, trlog.ac_no);
         strcpy(chk.tel, trlog.tel);
         chk.err_flag[1]='1';
         strcpy (chk.err_msg, "S银联有，平台有");
         /* if dfqr get sys_ejfno is filler1 */
         if (strcmp(chk.pt_tx_code, "dfqr")==0) {
            WriteLog(ERR_LVL, "dfqr:sys_ejfno=%d, filler1=%s", chk.sys_ejfno, trlog.filler1);
            chk.sys_ejfno=atol(rtrim(trlog.filler1));
         }

         ret = chk_self_trade(chk, &tx_amt, ac_no);
         if(ret<0) {
            SHOWSQLERR;
            EXEC SQL close chk_cursor;
            EXEC SQL free  chk_cursor;
            return -1;
         } else if (ret==0) {
            rtrim(ac_no);
            if ((tx_amt != chk.pt_amt) || (ac_no[0]!='9' && strcmp(chk.pt_ac_no, ac_no)) ){
printf("SECOND-hx:sys_date=%s,sys_ejfno=%ld,amt=%.2f,ac_no=%s;pt:amt=%.2f,ac_no=%s\n", chk.sys_date,chk.sys_ejfno,tx_amt, ac_no, chk.pt_amt, chk.pt_ac_no);
               strcpy (chk.err_flag, "122");
               strcpy (chk.err_msg, "S银联有，平台有,要素不符");
               strcpy (chk.hx_ac_no, ac_no);
            } else {
               chk.err_flag[2]='1';
               strcpy (chk.err_msg, "S银联有，平台有，核心有");
               strcpy (chk.hx_ac_no, ac_no);
            }
         } else {
            WriteLog(ERR_LVL,"SECOND: chk_self_trade:ret[%d]: sys_date[%s]stra[%d]tel[%s]", ret, chk.sys_date, chk.sys_ejfno, chk.tel );
         }
          
         
         /* WriteLog(ERR_LVL,"FIND: tx_date=%s,sys_no=%ld;req_date=%s,req_ejfno=%s,sys_date=%s,sys_ejfno=%d,ac_no=%s,amt=%.2lf,err_flag=%s", 
                chk.tx_date, chk.sys_no, trlog.req_date, trlog.req_ejfno,chk.sys_date, chk.sys_ejfno, trlog.ac_no, trlog.tx_amt, chk.err_flag); */
         printf("FIND: tx_date=%s,sys_no=%ld;req_date=%s,req_ejfno=%s,sys_date=%s,sys_ejfno=%d,ac_no=%s,amt=%.2lf,err_flag=%s\n", 
                chk.tx_date, chk.sys_no, trlog.req_date, trlog.req_ejfno,chk.sys_date, chk.sys_ejfno,trlog.ac_no, trlog.tx_amt, chk.err_flag);

         EXEC SQL update t_chkacct_err 
                     set sys_date=:trlog.sys_date, sys_ejfno=:chk.sys_ejfno
                         ,pt_amt=:trlog.tx_amt, pt_fee=:trlog.fee_amt
                         ,pt_ac_no=:trlog.ac_no, tel=:trlog.tel
                         ,err_flag=:chk.err_flag, err_msg=:chk.err_msg
                         ,sts=:trlog.sts, ct_ind=:trlog.dc_ind
                         ,pt_tx_code=:chk.pt_tx_code, tx_name=:chk.tx_name
                         ,hx_ac_no = :chk.hx_ac_no
                  where tx_date=:chk.tx_date
                    and sys_no=:chk.sys_no
                    and tran_time=:chk.tran_time
                    and acct_no=:chk.acct_no
                    and chk_date=:chk.chk_date;
         if(sqlca.sqlcode) {
            SHOWSQLERR;
            EXEC SQL close chk_cursor;
            EXEC SQL free  chk_cursor;
            return -1;
         }
         num++;
      }
   }

   printf("Second process record: %d\n", num);
   EXEC SQL close chk_cursor;
   EXEC SQL free  chk_cursor;
   return 0; 
}

/**check hex**/
int chk_self_trade(T_CHKACCT_ERR chkacct,double *amt, char *acno)
{
EXEC SQL BEGIN DECLARE SECTION;
  T_CHKACCT_ERR chk;
  double tx_amt=0.00;
  char ac_no[19+1];
EXEC SQL END DECLARE SECTION;
  memset(&chk, 0, sizeof(chk));
  memcpy(&chk, &chkacct, sizeof(chk));

  memset(ac_no, 0, sizeof(ac_no)); 
  EXEC SQL select first 1 tx_amt, ac_no 
	    into :tx_amt, :ac_no
	  from t_chnl_self_trade
	 where dz_date=:chk.sys_date
	   and (stra=:chk.sys_ejfno or strb=:chk.sys_ejfno)
	   and tel = :chk.tel ;
  if(sqlca.sqlcode != 100 && sqlca.sqlcode) {
    WriteLog(ERR_LVL,"chk_self_trade:sqlcode[%d]: sys_date[%s]stra[%d]tel[%s]", sqlca.sqlcode, chk.sys_date, chk.sys_ejfno, chk.tel );
    return -1;
  }
  *amt=tx_amt;
  strcpy(acno, ac_no); 

  return sqlca.sqlcode;
}

int insert_tracelog(char *chk_date, char *oper_flag)
{
EXEC SQL BEGIN DECLARE SECTION;
  char chkdate[8+1];
  char chk_year[4+1];
EXEC SQL END DECLARE SECTION;

   memset(chkdate, 0, sizeof(chkdate));
   strcpy (chkdate, chk_date);
   strcpy (chk_year, CHK_YEAR);

   if(oper_flag[0] == 'D') {
      if(drop_crt_trlog() != 0) {
         SHOWSQLERR;
         return -1;
      }
   }

   EXEC SQL begin work;
   EXEC SQL set isolation to dirty read;
   EXEC SQL delete from t_tracelog where 1=1;

   EXEC SQL insert into t_tracelog
            select * from tracelog
             where tx_amt != 0
               /* and node0 in ('atmc', 'pos', 'cups','cupnew') */
                and (
                   (node0 in ('cups','cupnew') and node1='cupsp' )
                or (node0 in ('atmc', 'pos') and node1='accc' and tm_dscpt='cups')
                   )
               /* and sys_date in (select unique (:chk_year || tx_date) from t_upay_file where load_date=:chkdate ); */
               and respcd='0000' 
               and sts <> '0'
               /* and sys_date in (select unique tx_date from t_upay_file where load_date=:chkdate ); */
               and sys_date >= (select min(tx_date) from t_upay_file where load_date=:chkdate )
               and sys_date < :chkdate; 

   if(sqlca.sqlcode) {
      SHOWSQLERR;
      EXEC SQL rollback work;
      return -1;
   }

   EXEC SQL commit work;
   return sqlca.sqlcode;
}

/** 表太大删除重建 **/
int drop_crt_trlog(){
    EXEC SQL drop table t_tracelog;
   if(sqlca.sqlcode) return sqlca.sqlcode;

    EXEC SQL create table t_tracelog
    (sys_date varchar(8),
    sys_ejfno varchar(12),
    sys_time varchar(10),
    node0 varchar(32),
    node1 varchar(32),
    respcd varchar(4),
    tm_respcd varchar(8),
    tm_dscpt varchar(80),
    sts varchar(32),
    respcd2 varchar(4),
    req_date varchar(8),
    req_ejfno varchar(15),
    tm_no varchar(10),
    resp_date varchar(8),
    resp_ejfno varchar(12),
    tx_code varchar(8),
    tx_name varchar(60),
    ac_no varchar(19),
    ac_no1 varchar(19),
    tx_amt decimal(17,2),
    fee_amt decimal(17,2),
    br_no varchar(7),
    tel varchar(6),
    ct_ind varchar(1),
    dc_ind varchar(1),
    filler1 varchar(60),
    filler2 varchar(60),
    filler3 varchar(60),
    termid varchar(8)
  );
   if(sqlca.sqlcode) return sqlca.sqlcode;

   EXEC SQL create index idx_trlogm on t_tracelog(sys_date, sys_ejfno, req_ejfno, ac_no) using btree;
   if(sqlca.sqlcode) return sqlca.sqlcode;

   EXEC SQL create unique cluster index idx_trlog1 on t_tracelog
    (sys_date,sys_ejfno) using btree ;

  printf("RE create t_tracelog is ok!");
  return sqlca.sqlcode;
    
}

/** 处理指定日期平台流水表 **/
int insert_day_tracelog(char *chk_date)
{
EXEC SQL BEGIN DECLARE SECTION;
  char sys_date[8+1];
EXEC SQL END DECLARE SECTION;

   memset(sys_date, 0, sizeof(sys_date));
   strcpy (sys_date, chk_date);

   EXEC SQL begin work;
   EXEC SQL set isolation to dirty read;

   EXEC SQL insert into t_tracelog
            select * from tracelog
             where tx_amt != 0
               /* and node0 in ('atmc', 'pos', 'cups','cupnew') */
               and (
                   (node0 in ('cups','cupnew') and node1='cupsp' )
                or (node0 in ('atmc', 'pos') and node1='accc' and tm_dscpt='cups')
                   )
               and sts <> '0'
               and respcd='0000'
               and sys_date = :sys_date;

   if(sqlca.sqlcode) {
      SHOWSQLERR;
      EXEC SQL rollback work;
      return -1;
   }

   EXEC SQL commit work;
   return sqlca.sqlcode;
}

int delete_chkerr(char *chk_date) 
{
EXEC SQL BEGIN DECLARE SECTION;
  char chkdate[8+1];
EXEC SQL END DECLARE SECTION;
   
   memset(chkdate, 0, sizeof(chkdate));
   strcpy (chkdate, chk_date);
   EXEC SQL begin work;

   EXEC SQL delete from t_chkacct_err where chk_date=:chkdate;
   if (sqlca.sqlcode) {
      SHOWSQLERR;
      EXEC SQL rollback work;
      return -1;
   }

   EXEC SQL commit work;
   return sqlca.sqlcode;
}

int process_chk(char *chk_date)
{
   
EXEC SQL BEGIN DECLARE SECTION;
   T_UPAY_FILE upay;
   char chkdate[8+1];
   T_TRACELOG trlog;
   T_CHKACCT_ERR chk;
   double tx_amt;
   char ac_no[19+1];
   char req_ejfno[15+1];
   char sys_ejfno[12+1];
   char sys_date[8+1];
   char fn4[4+1];
EXEC SQL END DECLARE SECTION;
   int num=0;
   int ret=0;  

   memset(chkdate, 0, sizeof(chkdate));
   strcpy (chkdate, chk_date);
   
   EXEC SQL declare upay_cursor cursor with hold for
            select * from t_upay_file
               where load_date=:chkdate
                 and tx_amt != 0;

   EXEC SQL open upay_cursor;
   if(sqlca.sqlcode) {
      SHOWSQLERR;
      return -1;
   }

   for(;;) 
   {
      memset(&upay, 0, sizeof(upay) );    
      EXEC SQL fetch upay_cursor into :upay;
      if(sqlca.sqlcode !=0 && sqlca.sqlcode !=100) {
         SHOWSQLERR;
         EXEC SQL close upay_cursor;
         EXEC SQL free  upay_cursor;
         return -1;
      }
      else if (sqlca.sqlcode == 100)
            break;

      rtrim(upay.tx_date);
      rtrim(upay.tran_time);
      rtrim(upay.acct_no);

      /*给差错表赋值*/
      memset(&chk, 0, sizeof(chk));
      strcpy (chk.tx_date, upay.tx_date);
      chk.sys_no = upay.sys_no;
      strcpy(chk.tran_time, upay.tran_time);
      strcpy (chk.msg_type, upay.msg_type);
      strcpy (chk.tx_code, upay.tx_code);
      strcpy (chk.svc_code, upay.svc_code);
      strcpy (chk.cati, rtrim(upay.cati));
      strcpy (chk.acct_no, upay.acct_no);
      chk.tx_amt = (double)upay.tx_amt/100;
      chk.pme_fee = (double)upay.pme_fee/100;
      chk.pay_fee = (double)upay.pay_fee/100;
      strcpy (chk.chk_date, chkdate);
      strcpy (chk.src_file, rtrim(upay.src_file));

      memset(req_ejfno,0,sizeof(req_ejfno));
      sprintf(req_ejfno, "%015ld", upay.sys_no);
      memset(sys_ejfno,0,sizeof(sys_ejfno));
      sprintf(sys_ejfno, "%012ld", upay.sys_no);
      memset(sys_date, 0,sizeof(sys_date));
      /* sprintf(sys_date,"%s%s", CHK_YEAR, upay.tx_date); */
      strcpy(sys_date, upay.tx_date);
/* printf("+++++%s|%d|%s|%s|\n", upay.tx_date,upay.sys_no,upay.acct_no, req_ejfno); */
      memset(&trlog, 0, sizeof(trlog));
/**行外转账 通过 req_ejfno=req_ejfno */
      memset(fn4, 0, sizeof(fn4));
      strncpy(fn4, chk.src_file+strlen(chk.src_file)-4,4);
      /** 1. III-他代本 2. AAA-本代他 **/
      if( 0 != strcmp(fn4, AAA) ) { 
          if(strncmp(chk.msg_type,"0420", 4)==0) {
            EXEC SQL select first 1 * into :trlog from t_tracelog 
                where sys_date = :sys_date
		  and req_ejfno= :req_ejfno
                  and ac_no = :upay.acct_no 
                  and trim(tm_no)=:upay.cati 
                  and sts <> '0'
                  and respcd='0000'
		  and substr(tx_code,-2)='cz'
                  and tx_amt=:chk.tx_amt;
	  } else {
            EXEC SQL select first 1 * into :trlog from t_tracelog 
                where sys_date = :sys_date
		  and req_ejfno= :req_ejfno
                  and ac_no = :upay.acct_no 
                  and trim(tm_no)=:upay.cati 
                  and sts <> '0'
                  and respcd='0000'
		  and substr(tx_code,-2)!='cz'
                  and tx_amt=:chk.tx_amt;
	  } 
      } else {
          if(strncmp(chk.msg_type,"0420", 4)==0) {
            EXEC SQL select * into :trlog from t_tracelog 
                where sys_date = :sys_date
		  and sys_ejfno= :sys_ejfno 
                  and ac_no = :upay.acct_no 
                  and trim(tm_no)=:upay.cati 
                  and sts <> '0'
                  and respcd='0000'
		  and substr(tx_code,-2)='cz'
                  and tx_amt=:chk.tx_amt;
	  }else {
            EXEC SQL select * into :trlog from t_tracelog 
                where sys_date = :sys_date
		  and sys_ejfno= :sys_ejfno 
                  and ac_no = :upay.acct_no 
                  and trim(tm_no)=:upay.cati 
                  and sts <> '0'
                  and respcd='0000'
		  and substr(tx_code,-2)!='cz'
                  and tx_amt=:chk.tx_amt;
	  }
      }

      if(sqlca.sqlcode != 0 && sqlca.sqlcode != 100) {
         printf("t_tracelog,sys_date='%s',req_ejfno='%s',ac_no='%s',sys_ejfno=%s.sqlcode=%d\n",
                 sys_date, req_ejfno, chk.acct_no,sys_ejfno,sqlca.sqlcode);
         SHOWSQLERR;
         EXEC SQL close upay_cursor;
         EXEC SQL free  upay_cursor;
         return -1;
      } else if (sqlca.sqlcode == 100) {
         strcpy (chk.err_flag, "100");
         strcpy (chk.err_msg, "银联有，平台无");
      } else {
         rtrim(trlog.sys_date);
         rtrim(trlog.ac_no);
         rtrim(trlog.tel);

         strcpy (chk.sys_date, trlog.sys_date);
         chk.sys_ejfno=atol(trlog.sys_ejfno);
         chk.pt_amt = trlog.tx_amt;
         chk.pt_fee = trlog.fee_amt;
         strcpy (chk.pt_ac_no, trlog.ac_no);
         strcpy (chk.tel, trlog.tel);

         if(chk.tx_amt != chk.pt_amt) {
            strcpy (chk.err_flag, "220");
            strcpy (chk.err_msg, "银联有，平台有,要素不符");
         } else {
            strcpy (chk.err_flag, "110");
            strcpy (chk.err_msg, "银联有，平台有");
         }
         strcpy (chk.sts, rtrim(trlog.sts));
         strcpy (chk.ct_ind, rtrim(trlog.dc_ind));
         strcpy(chk.pt_tx_code, rtrim(trlog.tx_code));
         sprintf(chk.tx_name, "%s:%s", rtrim(trlog.node0),rtrim(trlog.tx_name));
         /* if dfqr get sys_ejfno is filler1 */
         if (strcmp(chk.pt_tx_code, "dfqr")==0) {
            WriteLog(ERR_LVL, "SECOND-dfqr:sys_ejfno=%d, filler1=%s", chk.sys_ejfno, trlog.filler1);
            chk.sys_ejfno=atol(rtrim(trlog.filler1));
         }

 /* printf("============[%s][%d][%s]",chk.sys_date, chk.sys_ejfno, chk.tel); */
         ret = chk_self_trade(chk, &tx_amt, ac_no);
         if(ret<0) {
            SHOWSQLERR;
            EXEC SQL close upay_cursor;
            EXEC SQL free  upay_cursor;
            return -1;
         } else if (ret==0) {
            rtrim(ac_no);
            if ((tx_amt != chk.pt_amt) || (ac_no[0]!='9' && strcmp(chk.pt_ac_no, ac_no)) ){
printf("hx:amt=%.2f,ac_no=%s;pt:amt=%.2f,ac_no=%s\n", tx_amt, ac_no, chk.pt_amt, chk.pt_ac_no);
               strcpy (chk.err_flag, "122");
               strcpy (chk.err_msg, "银联有，平台有,要素不符");
               strcpy (chk.hx_ac_no, ac_no);
            } else {
               chk.err_flag[2]='1';
               strcpy (chk.err_msg, "银联有，平台有，核心有");
               strcpy (chk.hx_ac_no, ac_no);
            }
         } else {
            WriteLog(ERR_LVL,"chk_self_trade:ret[%d]: sys_date[%s]stra[%d]tel[%s]", ret, chk.sys_date, chk.sys_ejfno, chk.tel );
         }
      }

      num++;
      EXEC SQL insert into t_chkacct_err values(:chk);
      if(sqlca.sqlcode != 0) {
         printf("%d|%d|%s|%s|%d\n", num, chk.sys_no, chk.tx_date, chk.err_flag, sqlca.sqlcode);
      }

      if(_DEBUG) {
         printf("%s", SHOW_CHAR);
         if(num%NEXT_LINE==0) {printf("\n"); fflush(stdout);}
      }
   }
   printf("process record: %d\n", num);
 
   EXEC SQL close upay_cursor;
   EXEC SQL free  upay_cursor;

   return 0;
}


/*连接数据库*/
int connect_database(char *db_name)
{
    EXEC SQL BEGIN DECLARE SECTION;
      char dbname[MAX_DB_LEN];
    EXEC SQL END DECLARE SECTION;

    memset(dbname, 0x00, sizeof(dbname) );
    if(db_name==NULL) {
        strcpy(dbname, (char *)getenv("DBNAME") );          
    } 
    else {
        strcpy (dbname, db_name) ;
    }

    EXEC SQL connect to : dbname ;
    if (sqlca.sqlcode != 0) {
	SHOWSQLERR;
        printf ("连接数据库错[%s]\n", dbname);
    }

    if (_DEBUG)
	printf ("Connect database [%s] successful!\n", dbname);

    return sqlca.sqlcode;
}

/*关闭数据库*/
int close_database()
{
    EXEC SQL disconnect current;
    if (sqlca.sqlcode) {
	    SHOWSQLERR;
	    printf ("关闭数据库错\n");
    }

    return sqlca.sqlcode;
}


/* 重做核心核对信息 */
int process_third_chk(char *chk_date)
{
EXEC SQL BEGIN DECLARE SECTION;
  char chkdate[8+1];
  T_TRACELOG trlog;
  T_CHKACCT_ERR chk;
  double tx_amt;
  char ac_no[19+1];
  char req_ejfno[15+1];
  char sys_ejfno[12+1];
  char beg_sys_date[8+1];
  char end_sys_date[8+1];
  char sys_date[8+1];
EXEC SQL END DECLARE SECTION;
  int num=0;
  int ret = 0;

   memset(chkdate, 0, sizeof(chkdate));
   strcpy (chkdate, chk_date);

   EXEC SQL declare chk_thi_cursor cursor with hold for
            select * from t_chkacct_err
               where chk_date=:chkdate
                 and err_flag in ('110', '010');

   EXEC SQL open chk_thi_cursor;
   if(sqlca.sqlcode) 
   {
      SHOWSQLERR;
      return -1;
   }

   for(;;) 
   {
      memset(&chk, 0, sizeof(chk) );    
      EXEC SQL fetch chk_thi_cursor into :chk;
      if(sqlca.sqlcode !=0 && sqlca.sqlcode !=100) {
         SHOWSQLERR;
         EXEC SQL close chk_thi_cursor;
         EXEC SQL free  chk_thi_cursor;
         return -1;
      }
      else if (sqlca.sqlcode == 100)
            break;
       
      rtrim(chk.tx_date);
      rtrim(chk.acct_no);
      rtrim(chk.src_file);
      rtrim(chk.cati);

      ret = chk_self_trade(chk, &tx_amt, ac_no);
      if(ret<0) {
            SHOWSQLERR;
            EXEC SQL close chk_thi_cursor;
            EXEC SQL free  chk_thi_cursor;
            return -1;
      } else if (ret==0) {
            rtrim(ac_no);
            if ((tx_amt != chk.pt_amt) || (ac_no[0]!='9' && strcmp(chk.pt_ac_no, ac_no)) ){
printf("THIRD-hx:sys_date=%s,sys_ejfno=%ld,amt=%.2f,ac_no=%s;pt:amt=%.2f,ac_no=%s\n", chk.sys_date,chk.sys_ejfno,tx_amt, ac_no, chk.pt_amt, chk.pt_ac_no);
               strcpy (chk.err_flag, "122");
               strcpy (chk.err_msg, "T银联有，平台有,要素不符");
               strcpy (chk.hx_ac_no, ac_no);
            } else {
               chk.err_flag[2]='1';
               strcpy (chk.err_msg, "T银联有，平台有，核心有");
               strcpy (chk.hx_ac_no, ac_no);
            }
          
         
         printf("FIND: tx_date=%s,sys_no=%ld;sys_date=%s,sys_ejfno=%d,ac_no=%s,amt=%.2lf,err_flag=%s\n", 
                chk.tx_date, chk.sys_no, chk.sys_date, chk.sys_ejfno, ac_no, tx_amt, chk.err_flag);

         EXEC SQL update t_chkacct_err 
                     set err_flag=:chk.err_flag, err_msg=:chk.err_msg
                         ,hx_ac_no = :chk.hx_ac_no
                  where tx_date=:chk.tx_date
                    and sys_no=:chk.sys_no
                    and tran_time=:chk.tran_time
                    and acct_no=:chk.acct_no
                    and chk_date=:chk.chk_date;
         if(sqlca.sqlcode) {
            SHOWSQLERR;
            EXEC SQL close chk_thi_cursor;
            EXEC SQL free  chk_thi_cursor;
            return -1;
         }
      } else {
         printf("THIRD: tx_date=%s,sys_no=%ld;sys_date=%s,sys_ejfno=%d,ac_no=%s,amt=%.2lf,err_flag=%s\n", 
                chk.tx_date, chk.sys_no, chk.sys_date, chk.sys_ejfno, ac_no, tx_amt, chk.err_flag);
      }
      num++;
   }

   printf("Third process record: %d\n", num);
   EXEC SQL close chk_thi_cursor;
   EXEC SQL free  chk_thi_cursor;
   return 0; 
}


/** 第四次核对排除非行内账户差错 **/
int process_fourth_chk(char *chk_date)
{
EXEC SQL BEGIN DECLARE SECTION;
  char chkdate[8+1];
  T_CHKACCT_ERR chk;
  char acc6[6+1];
  char acc8[8+1];
  char acc10[10+1];
EXEC SQL END DECLARE SECTION;
  int num=0;
  int ret = 0;

   memset(chkdate, 0, sizeof(chkdate));
   strcpy (chkdate, chk_date);

   EXEC SQL declare chk_fou_cursor cursor with hold for
            select * from t_chkacct_err
               where chk_date=:chkdate
                 and err_flag='100';

   EXEC SQL open chk_fou_cursor;
   if(sqlca.sqlcode) 
   {
      SHOWSQLERR;
      return -1;
   }

   for(;;) 
   {
      memset(&chk, 0, sizeof(chk) );    
      EXEC SQL fetch chk_fou_cursor into :chk;
      if(sqlca.sqlcode !=0 && sqlca.sqlcode !=100) {
         SHOWSQLERR;
         EXEC SQL close chk_fou_cursor;
         EXEC SQL free  chk_fou_cursor;
         return -1;
      }
      else if (sqlca.sqlcode == 100)
            break;
       
      memset(acc6, 0, sizeof(acc6));
      strncpy(acc6, chk.acct_no, 6);

      memset(acc8, 0, sizeof(acc8));
      strncpy(acc8, chk.acct_no, 8);

      memset(acc10, 0, sizeof(acc10));
      strncpy(acc10, chk.acct_no, 10);

      printf("acc6=[%s]acc8=[%s]acc10=[%s]\n", acc6, acc8, acc10);
      EXEC SQL select * from cdsh_gmt_bin 
                where type='1' 
                  and bin in (:acc6, :acc8, :acc10);
      if(sqlca.sqlcode==100) {
          strcpy(chk.err_flag, "200");
          strcpy(chk.err_msg, "F行外卡，非差错");
          EXEC SQL update t_chkacct_err 
                     set err_flag=:chk.err_flag, err_msg=:chk.err_msg
                  where tx_date=:chk.tx_date
                    and sys_no=:chk.sys_no
                    and tran_time=:chk.tran_time
                    and acct_no=:chk.acct_no
                    and chk_date=:chk.chk_date;
          if(sqlca.sqlcode) {
              SHOWSQLERR;
              EXEC SQL close chk_fou_cursor;
              EXEC SQL free  chk_fou_cursor;
              return -1;
          }
          num++;
      }
   }

   printf("Fourth process record: %d\n", num);
   EXEC SQL close chk_fou_cursor;
   EXEC SQL free  chk_fou_cursor;
   return 0; 
}

/** 取前后三天日期 **/
int get_other_ndate(char *curr_date, char *befor_date, char *last_date)
{
EXEC SQL BEGIN DECLARE SECTION;
   char cdate[8+1];
   char bdate[8+1];
   char ldate[8+1];
EXEC SQL END DECLARE SECTION;

   memset(cdate,0,sizeof(cdate));
   memset(bdate,0,sizeof(bdate));
   memset(ldate,0,sizeof(ldate));
   strcpy(cdate, curr_date);

   EXEC SQL select to_char(extend(to_date(:cdate,'%Y%m%d'), year to day )-interval(3) day to day,'%Y%m%d')
                   ,to_char(extend(to_date(:cdate,'%Y%m%d'), year to day )+interval(3) day to day,'%Y%m%d')
                   into :bdate,:ldate 
              from dual;

   strcpy(befor_date, bdate);
   strcpy(last_date, ldate);
   /* if (_DEBUG) printf("befor date=%s,sqlcode=%d\n", befor_date,sqlca.sqlcode); */

   return sqlca.sqlcode;
}

/**  取前一天的系统日期 **/
int get_bone_date(char *curr_date, char *befor_date)
{
EXEC SQL BEGIN DECLARE SECTION;
   char cdate[8+1];
   char bdate[8+1];
EXEC SQL END DECLARE SECTION;

   memset(cdate,0,sizeof(cdate));
   memset(bdate,0,sizeof(bdate));
   strcpy(cdate, curr_date);

   EXEC SQL select to_char(extend(to_date(:cdate,'%Y%m%d'), year to day )-interval(1) day to day,'%Y%m%d') into :bdate
              from dual;

   strcpy(befor_date, bdate);
   /* if (_DEBUG) printf("befor date=%s,sqlcode=%d\n", befor_date,sqlca.sqlcode); */

   return sqlca.sqlcode;
}
/** 核对转账文件 **/
int process_zz_chk(char *chk_date)
{
EXEC SQL BEGIN DECLARE SECTION;
  char chkdate[8+1];
  T_TRACELOG trlog;
  T_CHKACCT_ERR chk;
  char sys_date[8+1];
  char bdays3[8+1];
  char edays3[8+1];
  int  record=0;
  double tx_amt;
  char ac_no[19+1];
EXEC SQL END DECLARE SECTION;
  int num=0;
  int ret=0;
  
  get_bone_date(chk_date, sys_date);
  printf("===========chk_date=%s, sys_date=%s\n", chk_date, sys_date);
  /** 根据对账日期取前一天系统日期 **/  
  if(delete_t_tracelog(sys_date) != 0) {
     printf("delete t_tracelog [%s] ERROR!\n", sys_date);
     return -1;
  }
  if(insert_day_tracelog(sys_date) != 0) {
     printf("insert t_tracelog [%s] ERROR!\n", sys_date);
     return -1;
  }
  
  get_other_ndate(sys_date, bdays3, edays3);
  printf("===========befor date=%s, end date=%s\n", bdays3, edays3);

  EXEC SQL declare trlog_zc cursor with hold for
       select * from t_tracelog 
            where sys_date=:sys_date
              and respcd='0000'
              and tx_code in ('zc', 'zccz', 'zr')
              and tx_amt > 0
              and sts<>'0'
              and sys_ejfno not in (select sys_ejfno from t_chkacct_err 
                                      where sys_date=:sys_date );
   EXEC SQL open trlog_zc;
   if(sqlca.sqlcode) 
   {
      SHOWSQLERR;
      return -1;
   }

   for(;;) 
   {
      memset(&trlog, 0, sizeof(trlog) );    
      memset(&chk, 0, sizeof(chk));
      EXEC SQL fetch trlog_zc into :trlog;
      if(sqlca.sqlcode !=0 && sqlca.sqlcode !=100) {
         SHOWSQLERR;
         EXEC SQL close trlog_zc;
         EXEC SQL free  trlog_zc;
         WriteLog(ERR_LVL,"process_zz_chk:chk_date[%s]sys_date[%s]sqlcode[%d]", chk_date, sys_date, sqlca.sqlcode);
         return -1;
      }
      else if (sqlca.sqlcode == 100)
            break;
      rtrim(trlog.tm_dscpt);
      rtrim(trlog.ac_no);
      rtrim(trlog.req_ejfno);
      rtrim(trlog.tx_code);
      if(strcmp(trlog.tx_code, "zc")==0) {
         strcpy(chk.msg_type, "0200"); 
         strcpy(chk.tx_code, "46");
      } else if (strcmp(trlog.tx_code, "zccz")==0) {
         strcpy(chk.msg_type, "0420"); 
         strcpy(chk.tx_code, "46");
      } else if (strcmp(trlog.tx_code, "zr")==0 || strcmp(trlog.tx_code, "zrqr")==0) {
         strcpy(chk.msg_type, "0200"); 
         strcpy(chk.tx_code, "47");
      }

      printf("sys_date=%s, sys_ejfno=%s, req_ejfno=%s,tm_dscpt=%s,ac_no=%s,tx_amt=%.2f\n", trlog.sys_date, trlog.sys_ejfno, trlog.req_ejfno, trlog.tm_dscpt, trlog.ac_no, trlog.tx_amt);
      /** 查找三天前后银联流水文件是否有该笔记录 **/
      EXEC SQL select count(*) into :record from t_upay_file 
                where tx_date>=:bdays3 and tx_date<=:edays3
                  and sys_no=:trlog.req_ejfno+0
                  and msg_type=:chk.msg_type
                  and tx_code[1,2] in ('40', :chk.tx_code)
                  and acct_no=:trlog.ac_no
                  and tx_amt=:trlog.tx_amt*100;
      if(sqlca.sqlcode && sqlca.sqlcode!=100) {
         WriteLog(ERR_LVL,"process_zz_chk:found t_upay_file error.chk_date[%s]sys_date[%s]sqlcode[%d]", chk_date, sys_date, sqlca.sqlcode);
         EXEC SQL close trlog_zc;
         EXEC SQL free  trlog_zc;
         return -1;
      } 

      if(record==0) {
         ret=check_zccz_info(trlog);
         if (ret<0) {
            WriteLog(ERR_LVL,"check_zccz_info ERROR:sys_date[%s]sys_ejfno[%s]sqlcode[%d]", sys_date, trlog.sys_ejfno, sqlca.sqlcode);
            EXEC SQL close trlog_zc;
            EXEC SQL free  trlog_zc;
            return -1;
         } else if (ret==0){
            WriteLog(ERR_LVL,"check_zccz_info FOUND ZCCZ or ZC:sys_date[%s]sys_ejfno[%s]req_ejfno=[%s]",
                    sys_date, trlog.sys_ejfno, trlog.req_ejfno);
            printf("check_zccz_info FOUND ZCCZ or ZC:sys_date[%s]sys_ejfno[%s]req_ejfno=[%s]", 
                    trlog.sys_date, trlog.sys_ejfno, trlog.req_ejfno);
            continue;
         }
         
         printf("这条记录在三天前后未找到银联流水\n");
         strcpy (chk.chk_date, chk_date);
         strcpy (chk.tx_date, trlog.sys_date); 
         strcpy (chk.sys_date, trlog.sys_date);
         strcpy (chk.tran_time, rtrim(trlog.sys_time));
         chk.sys_ejfno=atol(trlog.sys_ejfno);
         chk.pt_amt = trlog.tx_amt;
         chk.pt_fee = trlog.fee_amt;
         strcpy (chk.pt_ac_no, trlog.ac_no);
         strcpy (chk.tel, trlog.tel);
         strcpy (chk.err_flag, "010");
         strcpy (chk.err_msg, "平台有,银联无");
         strcpy (chk.sts, rtrim(trlog.sts));
         strcpy (chk.ct_ind, rtrim(trlog.dc_ind));
         strcpy(chk.pt_tx_code, rtrim(trlog.tx_code));
         sprintf(chk.tx_name, "%s:%s", rtrim(trlog.node0),rtrim(trlog.tx_name));
         /** 核对核心 **/
         ret = chk_self_trade(chk, &tx_amt, ac_no);
         if(ret<0) {
            SHOWSQLERR;
            EXEC SQL close trlog_zc;
            EXEC SQL free  trlog_zc;
            return -1;
         } else if (ret==0) {
            rtrim(ac_no);
            if ((tx_amt != chk.pt_amt) || (ac_no[0]!='9' && strcmp(chk.pt_ac_no, ac_no)) ){
printf("ZZ-hx:sys_date=%s,sys_ejfno=%ld,amt=%.2f,ac_no=%s;pt:amt=%.2f,ac_no=%s\n", chk.sys_date,chk.sys_ejfno,tx_amt, ac_no, chk.pt_amt, chk.pt_ac_no);
               strcpy (chk.err_flag, "022");
               strcpy (chk.err_msg, "平台与核心要素不符, 银联无");
               strcpy (chk.hx_ac_no, ac_no);
            } else {
               chk.err_flag[2]='1';
               strcpy (chk.err_msg, "平台有，核心有，银联无");
               strcpy (chk.hx_ac_no, ac_no);
            }
         }
         EXEC SQL insert into t_chkacct_err values(:chk);
         if(sqlca.sqlcode != 0) {
             printf("%d|%s|%d|%s|%s|%d\n", num, chk.sys_date, chk.sys_ejfno, trlog.req_ejfno, chk.err_flag, sqlca.sqlcode);
         }
      }
      
      num++;
   }

   printf("ZZ process record: %d\n", num);
   EXEC SQL close trlog_zc;
   EXEC SQL free  trlog_zc;

   return 0;
}

/** 删除指定日期临时表 **/
int delete_t_tracelog(char *chk_date) 
{
EXEC SQL BEGIN DECLARE SECTION;
  char sys_date[8+1];
EXEC SQL END DECLARE SECTION;
   
   memset(sys_date, 0, sizeof(sys_date));
   strcpy (sys_date, chk_date);
   EXEC SQL begin work;

   EXEC SQL delete from t_tracelog where sys_date=:sys_date;
   if (sqlca.sqlcode) {
      SHOWSQLERR;
      EXEC SQL rollback work;
      return -1;
   }

   EXEC SQL commit work;
   return sqlca.sqlcode;
}

/** 查找当日是否有冲正或原交易记录 **/
int check_zccz_info(T_TRACELOG trlog)
{
EXEC SQL BEGIN DECLARE SECTION;
    T_TRACELOG trace;
    char tx_code[8+1];
EXEC SQL END DECLARE SECTION;
   
    memset(tx_code,0,sizeof(tx_code));
    memset(&trace, 0, sizeof(trace));
    memcpy(&trace, &trlog, sizeof(trace));

    if(strcmp(trace.tx_code, "zc")==0) {
       strcpy(tx_code, "zccz");
    } else if(strcmp(trace.tx_code, "zccz")==0) {
       strcpy(tx_code, "zc");
    } else {
       return 1;
    }
    EXEC SQL select * from t_tracelog 
                     where sys_date=:trace.sys_date
                       and req_ejfno=:trace.req_ejfno
                       and tm_no=:trace.tm_no
                       and tx_code=:tx_code
                       and ac_no=:trace.ac_no
                       and tx_amt=:trace.tx_amt
                       and respcd='0000'
                       and sts<>'0';
    if (sqlca.sqlcode && sqlca.sqlcode!=100) {
       SHOWSQLERR;
       return -1;
    }
    return sqlca.sqlcode;
}

