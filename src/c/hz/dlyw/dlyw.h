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


static long LoginDatabase(); //��¼���ݿ�
static long GetDlywInfo();   //���ϵͳ������Ա��Ϣ
static void ReadDlywIni();   //�������ļ�
static void UpgradeProg();   //��������
static void Finished();
#endif

