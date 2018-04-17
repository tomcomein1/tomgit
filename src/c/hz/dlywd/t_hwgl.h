#ifndef T_HWGL_H
#define T_HWGL_H
long dsTaskCommitPurchase(char * sCzydh);
static long ProcessRecord(char * sLsh);
static long CheckOrInsertStock(char * sCzydh,char * sJym,long identfy);
long dsTaskCommitDistribute(char * sCzydh);
long dsTaskAcceptDistribute(char * sLsh);
long dsTaskSellGoods(char * sLsh,char * sCzydh,char * sHwbz,long nCount);
long dsTaskReturnGoods(char * sLsh,char * sCzydh,char * sHwbz,long nCount);
#endif



