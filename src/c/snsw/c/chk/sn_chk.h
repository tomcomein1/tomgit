#ifndef SN_CHK_H
#define SN_CHK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_DB_LEN   50
#define _DEBUG 1

/**部分宏定义**/
#define SHOWSQLERR do{ \
        WriteLog(ERR_LVL, "SQLCODE=%d, ISAM Error=%6hd\n",\
                          sqlca.sqlcode, sqlca.sqlerrd[1]);\
        } while(0)


EXEC SQL BEGIN DECLARE SECTION;
struct t_upay_file{
    char tx_date[8+1];
    char proxy_brch[11+1];
    long sys_no;
    char tran_time[10+1];
    char acct_no [19+1];
    long tx_amt;
    long fee;
    char msg_type[4+1];
    char tx_code[6+1];
    char svc_code[2+1];
    char cati[8+1];
    long pme_fee;
    long pay_fee;
    char out_actno[19+1];
    char in_actno [19+1];
    char src_file [25+1];
    char load_date[8+1];
};
typedef struct t_upay_file T_UPAY_FILE;

struct t_chkacct_err{
    char tx_date [8+1];
    long sys_no;
    char tran_time [10+1];
    char msg_type[4+1];
    char tx_code [6+1];
    char svc_code [2+1];
    char cati [8+1];
    char acct_no [19+1];
    double tx_amt;
    double pme_fee;
    double pay_fee;
    char sys_date [8+1];
    long sys_ejfno;
    double pt_amt;
    double pt_fee;
    char pt_ac_no [19+1];
    char tel [6+1];
    char chk_date [8+1];
    char err_flag [3+1];
    char err_msg [60+1];
    char sts [32+1];
    char tx_name [60+1];
    char src_file [25+1];
    char hx_ac_no [19+1];
};
typedef struct t_chkacct_err T_CHKACCT_ERR;

struct t_tracelog {
    char sys_date [8+1];
    char sys_ejfno [12+1];
    char sys_time [10+1];
    char node0 [32+1];
    char node1 [32+1];
    char respcd [4+1];
    char tm_respcd [8+1];
    char tm_dscpt [80+1];
    char sts [32+1];
    char respcd2 [4+1];
    char req_date [8+1];
    char req_ejfno [15+1];
    char tm_no [10+1];
    char resp_date [8+1];
    char resp_ejfno [12+1];
    char tx_code [8+1];
    char tx_name [60+1];
    char ac_no [19+1];
    char ac_no1 [19+1];
    double tx_amt  ;
    double fee_amt  ;
    char br_no [7+1];
    char tel [6+1];
    char ct_ind [1+1];
    char dc_ind [1+1];
    char filler1 [60+1];
    char filler2 [60+1];
    char filler3 [60+1];
    char termid [8+1];
};
typedef struct t_tracelog T_TRACELOG;

EXEC SQL END DECLARE SECTION;

#endif
