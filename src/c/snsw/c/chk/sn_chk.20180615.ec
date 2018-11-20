#include "para_macro.h"
EXEC SQL include sn_chk.h;

#define SECOND_DAYS 1
char THIS_YEAR[4+1]="2018"; 

void usage()
{
   printf("**************************\n");
   printf(" please input check date\n");
   printf("**************************\n");
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

   /* test_getdate(argv[1]);
   return 0; */

   if (connect_database(NULL) != 0) 
   {
      return -1;
   }

   strncpy(THIS_YEAR, argv[1], 4);
   /* printf("this year is [%s]\n", THIS_YEAR); */
   /* 预处理t_tracelog表 */
   if(strcmp(argv[2], "PRE")==0) {
     insert_tracelog(argv[1]);
   } else if(strcmp(argv[2], "ITL")==0) {
     insert_day_tracelog(argv[1]);
     goto END_CHK;
   } else if(strcmp(argv[2], "SEC")==0) {
     EXEC SQL begin work;
     if(process_second_chk(argv[1]) == 0) {
        EXEC SQL commit work;
     } else {
        EXEC SQL rollback work;
     }
     
     goto END_CHK;
   }

   delete_chkerr(argv[1]);
   process_chk(argv[1]);
   
END_CHK:
   close_database(); 
   printf("End snbank check account.\n");
   return 0;
}

int test_getdate(char *date) 
{
  char end_date[4+1];
  char beg_date[4+1];
  memset(beg_date,0,sizeof(beg_date));
  memset(end_date,0,sizeof(end_date));
  get_beg_date(date, SECOND_DAYS, beg_date);
  get_end_date(date, SECOND_DAYS, end_date);
  return 0;
}

int get_beg_date(char *sdate, int day, char *bdate)
{
  char mm[2+1],dd[2+1];
  int m, d;

  memset(mm,0,sizeof(mm));
  memset(dd,0,sizeof(dd));
  mm[0]=sdate[0], mm[1]=sdate[1];
  dd[0]=sdate[2], dd[1]=sdate[3];

  m=atol(mm), d=atol(dd); 
  if (d-day < 1) {
    m=m-1;
    if (m==1 || m==3 || m==5 || m==7 || m==8 || m==10 || m==12) {
      d=31-day + 1;
    } else if(m==2) {
      d=28-day + 1;
    } else {
      d=30-day + 1;
    }
    if (m==0) m=1;
    sprintf(bdate, "%02d%02d", m, d);
  } else {
    d-=day;
    sprintf(bdate, "%02d%02d", m, d);
  }

/*printf("sdate=%s bdate=%s\n", sdate, bdate);*/
  return 0;
}

int get_end_date(char *sdate, int day, char *edate)
{
  char mm[2+1],dd[2+1];
  int m, d;

  memset(mm,0,sizeof(mm));
  memset(dd,0,sizeof(dd));
  mm[0]=sdate[0], mm[1]=sdate[1];
  dd[0]=sdate[2], dd[1]=sdate[3];

  m=atol(mm), d=atol(dd); 

  /** 31days **/
  if (m==1 || m==3 || m==5 || m==7 || m==8 || m==10 || m==12) {
     if (d+day > 31) {
       m=m+1;
       d=1;
     } else {
       d+=day;
     }
     sprintf(edate, "%02d%02d", m, d);
  } else if(m==2) {
     if (d+day > 28) {
       m=m+1;
       d=1;
     } else {
       d+=day;
     }
     sprintf(edate, "%02d%02d", m, d);
  } else {
     if (d+day > 30) {
       m=m+1;
       d=1;
     } else {
       d+=day;
     }
     sprintf(edate, "%02d%02d", m, d);
  }

/*printf("sdate=%s edate=%s\n", sdate, edate);*/
  return 0;
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
  char end_date[4+1];
  char beg_date[4+1];
  char beg_sys_date[8+1];
  char end_sys_date[8+1];
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
      memset(req_ejfno,0,sizeof(req_ejfno));
      sprintf(req_ejfno, "%015ld", chk.sys_no);
      memset(sys_ejfno,0,sizeof(sys_ejfno));
      sprintf(sys_ejfno, "%012ld", chk.sys_no);
 /* printf("+++++%s|%d|%s|%s|\n", chk.tx_date,chk.sys_no,chk.acct_no, req_ejfno); */
      memset(beg_date,0,sizeof(beg_date));
      memset(end_date,0,sizeof(end_date));
      get_beg_date(chk.tx_date, SECOND_DAYS, beg_date);
      get_end_date(chk.tx_date, SECOND_DAYS, end_date);

      memset(beg_sys_date,0,sizeof(beg_sys_date));
      memset(end_sys_date,0,sizeof(end_sys_date));
      if(strncmp(chk.tx_date,"1231",4)==0) {
         sprintf(beg_sys_date, "%ld%s", atol(THIS_YEAR)-1, chk.tx_date); 
      } else {
         sprintf(beg_sys_date, "%s%s", THIS_YEAR, beg_date); 
      }
      sprintf(end_sys_date, "%s%s", THIS_YEAR, end_date); 

      memset(&trlog, 0, sizeof(trlog));
/**行外转账 + 非 A*/
      if( (strncmp(chk.msg_type,"0200",4)==0 && (strncmp(chk.tx_code,"40", 2)==0 || strncmp(chk.tx_code,"46",2)==0))
          || chk.src_file[strlen(chk.src_file)-4]!='A') {
          EXEC SQL select first 1 * into :trlog from t_tracelog 
                where sys_date >= :beg_sys_date
                  and sys_date <= :end_sys_date
                  and req_ejfno= :req_ejfno
                  and ac_no = :chk.acct_no 
                  and tm_no = :chk.cati
                  and sts <> '0'
                  and respcd='0000';
      } else {
          EXEC SQL select * into :trlog from t_tracelog 
                where sys_date >= :beg_sys_date
                  and sys_date <= :end_sys_date
                  and sys_ejfno= :sys_ejfno
                  and ac_no = :chk.acct_no 
                  and tm_no = :chk.cati
                  and sts <> '0'
                  and respcd='0000';
      }
      if(sqlca.sqlcode != 0 && sqlca.sqlcode != 100) {
         printf("t_tracelog,req_date>='%s',req_date<='%s',req_ejfno='%s',ac_no='%s',sys_ejfno=%s.sqlcode=%d\n",
                 beg_date, end_date, req_ejfno, chk.acct_no,sys_ejfno,sqlca.sqlcode);
         SHOWSQLERR;
         EXEC SQL close chk_cursor;
         EXEC SQL free  chk_cursor;
         return -1;
      } else if (sqlca.sqlcode == 100) {
         /* WriteLog(ERR_LVL,"ERR:[%c],%s|%s|req_ejfno:%s|ac_no:%s|sys_ejfno:%s|tm_no:%s|",
                 chk.src_file[strlen(chk.src_file)-4], beg_sys_date, end_sys_date, req_ejfno, chk.acct_no,sys_ejfno,chk.cati);
         */
         printf("[%c],%s|%s|req_ejfno:%s|ac_no:%s|sys_ejfno:%s|tm_no:%s|\n",
                 chk.src_file[strlen(chk.src_file)-4], beg_sys_date, end_sys_date, req_ejfno, chk.acct_no,sys_ejfno,chk.cati);
      } else {
         rtrim(trlog.req_date);
         rtrim(trlog.ac_no);
         rtrim(trlog.tel);
         rtrim(trlog.sts);
         strcpy(chk.pt_tx_code, rtrim(trlog.tx_code));
         strcpy(chk.tx_name, rtrim(trlog.tx_name));

         strcpy(chk.sys_date, trlog.sys_date);
         chk.sys_ejfno=atol(trlog.sys_ejfno);
         chk.pt_amt = trlog.tx_amt;
         strcpy(chk.pt_ac_no, trlog.ac_no);
         strcpy(chk.tel, trlog.tel);
         chk.err_flag[1]='1';
         strcpy (chk.err_msg, "S银联有，平台有");

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
                         ,sts=:trlog.sts, ct_ind=:trlog.ct_ind
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

   printf("second process record: %d\n", num);
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
  EXEC SQL select tx_amt, ac_no 
	    into :tx_amt, :ac_no
	  from t_chnl_self_trade
	 where dz_date=:chk.sys_date
	   and (stra=:chk.sys_ejfno or strb=:chk.sys_ejfno)
	   and tel = :chk.tel ;
  if(sqlca.sqlcode != 100 && sqlca.sqlcode) {
    return -1;
  }
  *amt=tx_amt;
  strcpy(acno, ac_no); 

  return sqlca.sqlcode;
}

int insert_tracelog(char *chk_date)
{
EXEC SQL BEGIN DECLARE SECTION;
  char chkdate[8+1];
EXEC SQL END DECLARE SECTION;

   memset(chkdate, 0, sizeof(chkdate));
   strcpy (chkdate, chk_date);

   EXEC SQL begin work;
   EXEC SQL set isolation to dirty read;
   EXEC SQL delete from t_tracelog where 1=1;

   EXEC SQL insert into t_tracelog
            select * from tracelog
             where tx_amt != 0
               and node0 in ('atmc', 'pos', 'cups','cupnew') 
               and sys_date in (select unique (current year to year ||tx_date) from t_upay_file where load_date=:chkdate );

   if(sqlca.sqlcode) {
      SHOWSQLERR;
      EXEC SQL rollback work;
      return -1;
   }

   EXEC SQL commit work;
   return sqlca.sqlcode;
}

/** 处理指定日期平台流水表 **/
int insert_day_tracelog(char *chk_date)
{
EXEC SQL BEGIN DECLARE SECTION;
  char chkdate[8+1];
EXEC SQL END DECLARE SECTION;

   memset(chkdate, 0, sizeof(chkdate));
   strcpy (chkdate, chk_date);

   EXEC SQL begin work;
   EXEC SQL set isolation to dirty read;

   EXEC SQL insert into t_tracelog
            select * from tracelog
             where tx_amt != 0
               and node0 in ('atmc', 'pos', 'cups','cupnew') 
               and sys_date = :chkdate;

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
      strcpy (chk.cati, upay.cati);
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
      sprintf(sys_date,"%s%s", THIS_YEAR, upay.tx_date);
/* printf("+++++%s|%d|%s|%s|\n", upay.tx_date,upay.sys_no,upay.acct_no, req_ejfno); */
      memset(&trlog, 0, sizeof(trlog));
/**行外转账 通过 req_ejfno=req_ejfno */
/**行外转账 + 非 A*/
      if( (strncmp(chk.msg_type,"0200",4)==0 && (strncmp(chk.tx_code,"40", 2)==0 || strncmp(chk.tx_code,"46",2)==0))
          || chk.src_file[strlen(chk.src_file)-4] != 'A' ) { 
          EXEC SQL select first 1 * into :trlog from t_tracelog 
                where sys_date = :sys_date
		  and req_ejfno= :req_ejfno
                  and ac_no = :upay.acct_no 
                  and tm_no=:upay.cati 
                  and sts <> '0'
                  and respcd='0000'
                  and tx_amt=:chk.tx_amt;
      } else {
          EXEC SQL select * into :trlog from t_tracelog 
                where sys_date = :sys_date
		  and sys_ejfno= :sys_ejfno 
                  and ac_no = :upay.acct_no 
                  and tm_no=:upay.cati 
                  and sts <> '0'
                  and respcd='0000'
                  and tx_amt=:chk.tx_amt;
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
         strcpy (chk.ct_ind, rtrim(trlog.ct_ind));
         strcpy(chk.pt_tx_code, rtrim(trlog.tx_code));
         strcpy(chk.tx_name, rtrim(trlog.tx_name));

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
         }
      }

      num++;
      EXEC SQL insert into t_chkacct_err values(:chk);
      if(sqlca.sqlcode != 0) {
         printf("%d|%d|%s|%s|%d\n", num, chk.sys_no, chk.tx_date, chk.err_flag, sqlca.sqlcode);
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

