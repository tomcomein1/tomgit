
GRPNO = 700
XOBJECTS = wappushqry.o wappushbreakdownqry.o agt_batchsend_qry.o \
  rec_qry.o btch_stat.o list_stat.o bank_account_qry.o \
  inn_transfer.o bank_account_breakdown_qry.o cbs_jnlstat_qry.o \
  infowin.o pbbusi700midpay.o rp_rec.o agt_batch_send.o \
  out_tsfee_calculate.o tran_dtl_qry.o loanrethis_qry.o \
  cbs_jnlstat_qry_back.o acct_info_qry.o \
  acct_info_qry_back.o netbank_pl_reginn.o tran_dtl_qry_accstat.o \
  gm_cert_prt.o acctinfo_comb.o qry_deal_bef.o wy_cdmtfix.o \
  acctinfo_filter.o wy_fixtcdm.o wy_calsavtcdm.o chk_cardacc_pwd.o \
  Data_MaxPay_Modify.o Data_Query_Info.o \
  wy_card_loss.o wy_fee_fr.o cbs_finger_auth.o \
  Check_acc_Amount.o sky_wyfront.o agt_gas.o dqd_com.o \
  crm_pub.o Data_Card_Msg_sign.o CashBooking.o pbbusi700qry.o \
  loanNoContarctQry_file.o WXJJ_com.o wappushbreakdownqry_new.o \
  netbank_pl_reginn_new.o agt_batch_send_new.o wy_piecon.o \
  pie_info_qry.o qry_piedeal.o wy_AccNotice.o wy_accnotice_qry.o upbstran.o \
  agt_charge.o net_cert.o pbbnet_tyzf.o bat_qry_prn.o rp_rec.o \
  t_net_telfraud.o 

PUBOBJECTS = $(GRPPUB)/loan_xml.o

XLINKLIB = $(WORKDIR)/detc/olibs/unionBankAPI.a \
  $(PBLIBS) -lbusisvr -lthirdftp -lpubatom -lrulebase -lpqds -lpubic \
  -liconv -lesb -lpbtelfraud

include $(WORKDIR)/appsrc/grpatom/MakeAtomLib.mk

