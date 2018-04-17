#ifndef DLYW_H
#define DLYW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef UNIX
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include <stdarg.h>
#ifdef DOS
#include <conio.h>
#include <io.h>
#include <errno.h>
#endif

#include "netmain.h"
#include "dlpublic.h"


static long LoginDatabase(); //登录数据库
static long GetDlywInfo();   //获得系统及操作员信息
static void ReadDlywIni();   //读配置文件
static void UpgradeProg();   //程序升级
static void Finished();
#endif

