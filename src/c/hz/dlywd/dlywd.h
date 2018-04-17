/************************************************************************
make date:  2001.2.20
filename :  public.h
target:    �ļ���Ҫ����
*************************************************************************/

#ifndef DLYWD_H 
#define DLYWD_H 
#include<math.h>
#include<stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<stdarg.h>

#define QLEN       5  /* maximum connection queue length  */
#define SERVICE  "dlyw"
#define DEBUG

#define MAX_CONNECT_NUM    200    //���ͬʱ������
#define MIN_CONNECT_NUM     10    //��Сͬʱ������
#define MAX_DELAY_TIME    6000    //DLYWD�������ͣ��ʱ��(��)
#define MIN_DELAY_TIME     500    //DLYWD��������ͣ��ʱ��(��)

typedef struct processtab{
                          pid_t  pid;
                          long   nStartTime;
                         }PROCESSTAB; 

#include"dlpublic.h"
#include"netmain.h"
#include"task.h"
#include"newtask.h"
#include"des.h"
#include"tasktool.h"
#include"filechar.h"
#include"t_query.h"
#include"t_pay.h"
#include"t_recall.h"
#include"t_hwgl.h"
#include"t_mobile.h"
#include"t_print.h"
#include"t_other.h"
#include"t_jymgl.h"
#include"t_dbyw.h"
#include"t_telcom.h"
#include"t_air.h"
#include"t_b2i.h"
#include"t_tax.h"
#include"t_power.h"
#include"dispatch.h"

long GetDlzPassword();
void cleanup(int sig);
long ReadIni();
long GetChildPosition();
#endif
