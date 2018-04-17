#ifndef DL_GIVE_H
#define DL_GIVE_H
#include "tools.h"

void DistributeGoods();
static long SaveDistributeData(DATATABLE * pdt);
static long LoadDistrubuteData(DATATABLE * pdt);
static long CommitDistributeData(DATATABLE * pdt);
MENU * CreateCzyMenu();
MENU * CreateIdentMenu(char * sJym);

#endif
