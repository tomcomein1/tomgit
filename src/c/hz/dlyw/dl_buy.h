#ifndef DL_BUY_H
#define DL_BUY_H
#include"tools.h"
#include"newtask.h"

MENU * CreateGoodsMenu();
MENU * CreateTransActorMenu();
MENU * CreateCompanyMenu();
MENU * CreateIdentMenu(char *);
static long CommitPurchaseData(DATATABLE * pdt);
static long LoadPurchaseData(DATATABLE * pdt);
static long SavePurchaseData(DATATABLE * pdt);
void GenerateLsh(long,char *,char * ,char * );
void PurchaseGoods();
void AcceptGoods();
static long LoadAcceptData(DATATABLE * pdt);
static long CommitAcceptData(DATATABLE * pdt);
#endif


