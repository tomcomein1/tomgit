#ifndef DLSET_H
#define DLSET_H
#include"dlpublic.h"
#include"tools.h"

#define SETTABLE  DATATABLE
#define CreateSetTable(line) CreateDataTable(0,NULL,"%s%s%s%s%d%d");
#define DropSetTable(pdt) DropDataTable(pdt);

//参数设置工具
long AddSetItem(SETTABLE * pst,char * sTitle,char * sField,long nLength,char * sWhere,...);
long BeginSetting(SETTABLE * pst,long nLine);
long LoadDefaultSet(SETTABLE * pst);
static long SaveSetValue(SETTABLE * pst);

#endif

