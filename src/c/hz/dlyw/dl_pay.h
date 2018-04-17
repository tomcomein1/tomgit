#ifndef DL_PAY_H
#define DL_PAY_H
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "dlpublic.h"
#include "tools.h"
#include "dl_yw.h"
#include "tasktool.h"
#include "newtask.h"
#include "dlprint.h"

#define YWMX_ELEMENT_LEN 20 
#define JfcxTask  100000     //缴费查寻任务
#define BddjTask  100100     //补打单据查寻
#define DddjTask  100200     //代打联通单据查寻
#define JfqrTask  100300     //缴费确认任务
#define JffxTask  100400     //缴费反销任务


long InitDlywPay();
void EndDlywPay();
void DlywPay(long ntype,char *cjymc,char *cjym);
long Stdpay_Input();
long Stdpay_Show();
long AddPayDetailDialog();
long QulSend(long nTaskNo);     //查寻发送接收
long PaySend(long nTaskNo);     //缴费发送接收
void PayPrint(long nType);     //缴费业务打印
void PrintYdfp(char * sLsh);
void PrintYdsj(char * sLsh);
void PrintTrqfp(char * sLsh);
void PrintXhfp(char * sLsh);
void PrintLtddfp(char * sLsh);
void PrintLtdsfp(char * sLsh);
void PrintLtdssj(char * sLsh);
void PrintGddhfp(char * sLsh);
void PrintMyGddhfp(char * sJym,char * sLsh);
void PrintBxfp(char * sLsh);
void PrintTaxfp(char * sLsh);
void PrintDffp(char * sLsh);
static void PrintGa(long nXh,long nprint);
void AdditionProcess();
long ValidateCheck(long ntype);
void PrintYaGddhfp(char * sLsh);
void PrintPzhGddhfp(char * sJym,char * sLsh);
void PrintTerrafp(char * sLsh);
void PrintzwDffpL(char * sLsh);
void PrintzwDffpB(char * sLsh);
void PrintTpyfp(char * sLsh,long nType);
void PrintTaxJkfp(char * sLsh);
void PrintHyTerrafp(char * sLsh,long nType);
void PrintMsTerrafp(char * sLsh,long nType);
void PrintRsTerrafp(char * sLsh,long nType);
void PrintDyTerrafp(char * sLsh,long nType);
void PrintDlTerrafp(char * sLsh,long nType);
void PrintPxDfsj(char * sLsh);
void PrintPxDffp(char * sLsh);
#endif
