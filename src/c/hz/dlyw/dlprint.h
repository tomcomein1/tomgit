#ifndef DLPRINT_H
#define DLPRINT_H
#include "dlpublic.h"
#include "tools.h"

#define MAX_PRINT_PAGE    100    //每次最大打印张数

typedef struct printform{
                         long nCount;
                         char sTable[51];
                         CONTAINER * pf;
                         CONTAINER * pd;
                        }PRINTFORM;

PRINTFORM * CreatePrintForm(char * sPrintName);
long LoadPrintData(PRINTFORM * ppf,char * sWhere,...);
long OutputPrintForm(PRINTFORM * ppf,char * outbuf,long nIndex,...);
void DropPrintForm(PRINTFORM * ppf);
                      

#ifndef HOUTAI
void BillFormatManager();
static void StartWorkSpace(char * sName,char * Choose);
static long LoadBillFormat(DATATABLE * pdt,char * sName);
static long SaveBillFormat(DATATABLE *pdt,char *sName,char *sTable);
static void ShowBillFormat(DATATABLE * pdt);
void ExportPrintTable();
void to_Chinese(double dMoney,char * sValue);
static long ChangeFormPosition(DATATABLE * pdt);
#endif
#endif
