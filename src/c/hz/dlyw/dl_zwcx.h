#ifndef DL_QUERY_H
#define DL_QUERY_H
#include "tasktool.h"
static MENU * CreateQueryMenu();
static MENU * CreateXjMenu(RECORD rec);
static MENU * CreateZjMenu(char * sXjJsdh,RECORD rec);
static void SetQueryDate();
static void QueryAllZwmx();
static void QueryXjZwmxForm();
static void QueryZwmxForm();
static void QueryZwmxDetail();
static void DlywJkdCx();
static void RecordQueryJym(MENU * pmn,long nId);
static void RecordQueryJsdh(MENU * pmn,long nId);
static long GetJsmcByJsdh(char * sJsdh,char * sJsmc);
static long GetJymcByJym(char * sJym,char * sJymc);
#endif
