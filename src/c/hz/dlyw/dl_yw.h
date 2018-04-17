/***********************************************************************
** @Copyright...........1999.3
**filename      文件名: dl_yw.h
**target        目的:定义代理业务系统中收费包头结构
**create time   生成日期: 1999.3
**original programmer   编码:linzhidong
**copyright     版本
**edit history  修改历史:
**
****************************************************************************/
#ifndef DL_YW_H
#define DL_YW_H
#include"dlpublic.h"
#include"tools.h"
MENU * CreateJymMenu(long nlx);  //jym menu
void AddPayMenuItem(MENU * pmn);//append jym menu item call by CreateJymMenu
void DlywPay(long nKey,char *jymc,char *cjym);
void PayRecall(long nKey,char *jymc,char *cjym);
void ReprintBill(long nKey,char *jymc,char *cjym);
void nosetting(long nType,char * sItem,char * sJym);
void prohibit(long nType,char * sItem,char * sJym);
void CheckAccountWithBP(long ntype,char * item,char *sJym);
void TelecomCheckAccount(long nId,char * item,char * sJym);
void UnicomPrint(long ntype,char * cjymc,char * cjym);
void CheckAccountWithBP();
void AirTicket(long number,char * item,char * sJym);
void OutAirTicket(long number,char * item,char * sJym);
//void QueryAirTicket(long number,char * item,char *sJym);
void CheckAirTicket(long number,char * item,char *sJym);
//void CheckDesAirTicket(long number,char * item,char *sJym);
void AirTicketPay(long number,char * item,char *sJym);
void AirTicketDeliver(long number,char * item,char *sJym);
void AirReturnTicketBeg(long number,char * item,char *sJym);
void AirDoReturnTicketBeg(long number,char * item,char *sJym);

void AirSignIn(long number,char * item,char * sJym);
void AirSignOut(long number,char * item,char * sJym);
void ReprintSj(long ntype,char *cjymc,char *cjym);
void B2iCheckAccount(long nId,char * item,char * sJym);
void AirCheckAccount(long number,char * item,char *sJym);
void QueryAirAcc(long number,char * item,char *sJym);
void Unicom193Registe(long number,char *item,char *sJym);
void QueryTax_Info(long number,char *item,char *sJym);
void TaxPrintBill(long number,char *item,char *sJym);
void TaxCheckAccount(long nId,char * item,char * sJym);
void HzDayTaxFile(long nId,char * item,char * sJym);
void CheckPurTicket(long nId,char * item,char * sJym);
void GetJymHost(char *sJym);
void QueryPowerbyJh(long ntype,char * cjymc,char * cjym);
void MenuYwbb(long number,char * item,char *sJym);
void QueryAirSp(long number,char * item,char *sJym);
void QueryAirSd(long number,char * item,char *sJym);
void QueryAirTp(long number,char * item,char *sJym);
void BbAirSd(long number,char * item,char *sJym);
void BbAirXj(long number,char * item,char *sJym);
void QueryAirSpfs(long number,char * item,char *sJym);
void PostGasToFile(long ntype,char *cjymc,char *cjym);
void QueryAirDdxx(long number,char * item,char *sJym);
void AirDdProduct(long nId,char * item,char * sJym);
void QueryTax_Id_Info(long number,char *item,char *sJym);
void TerraPrintBill(long number,char *item,char *sJym);
void TerraJks(long number,char *item,char *sJym);
void Terra_Defp(long number,char *item,char *sJym);
void QueryPower_Info(long number,char *item,char *sJym);

void QueryAccountData(long nType,char * sItem,char * sJym);
void InsertJzzl();
void QueryJzzl();
extern void  MulDlywPay(long ntype,char *cjymc,char *cjym);
void QueryZnj_Info(long number,char *item,char *sJym);
#endif
