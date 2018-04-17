// 2000. 6.20 增加DOS下SetNoWaitRead()和SetWaitRead()函数, 同时更改GetKeyUnderDos
// author libaodong
// 2000.05.30 修改ansi模式及prtready以达到主机使用
 
// author libaodong
// 2000.03.23 修改vt100模式不能输入汉字的问题
// 新增全局变量g_nWeiMa
// 更改函数GetKeyUsingVt100
// 更改函数getstring
// 修改部分被标注为"修改vt100模式不能输入汉字的问题"

// author libaodong
// 1999.12.02 增加SetNoWaitRead()和SetWaitRead()函数
// usage:
// SetNoWaitRead();
// while(1)
// {
//   ...
//   if(keyb() == KEY_F8)
//   {
//     ...
//     break;
//   }
//   ...
// }
// SetWaitRead();
//
// 确保SetNoWaitRead()和SetWaitRead()配对使用
// 执行了SetNoWaitRead()后，如无按键，keyb()立即返回0，如有按键，返回键值

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include "dlpublic.h"

#ifdef UNIX
#include <termio.h>
#include <sys/types.h>
#endif

#ifdef DOS
#include <conio.h>
#include <process.h>
#include <errno.h>
#include <dos.h>
#include <i86.h>
#endif

#ifdef UNIX
struct termio rOldTerm;
long nTermType=TERM_ANSI;
FILE *fHostPrn;
#endif

long nOutPutHandle=OUTPUT_SCREEN;
FILE *fOutPutHandle;
long nPrnDelayTime=15;
char sScreenBuffer[2001];
char sSaveBuffer[5][4001];
long nTotalWin=0;/*已保存的画面数*/
long nCursorX=1,nCursorY=1;
char sSysSaveDraw[4];/*yes使用存储画面,no 不使用存储画面*/
char sfilename[30];//文件名称
long nSysLkrq;  /*录卡日期*/
long nSysSfm;  /*系统时间  小时 分钟 秒*/
long nSysDay;
long nSysMonth;
long nSysYear;
long nSysMinute;
long nSysHour;
long nSysSecond;
char sSysLkrq[16];
char sSysSfm[13];
char seSysLkrq[16];
char seSysSfm[13];
char cSysLkrq[21];
char cSysSfm[21];
char sGetJh[10];
char sText[201];
/*#if defined(DLYWDLL)
extern char sFileName[41];
#else*/
char sFileName[41];
//#endif

extern char sSysCzydh[7];
extern char sSysJsdh[JM_JH_LEN+1];/*支局代号*/

#ifdef DOS
long nCursorAttr=7;
int g_nWeiMa;
#endif


long nSysMode=UNIX_TERM;
long Get12(void);
void SetNoWaitRead();
void SetWaitRead();
void Initstruct_Jym(JYM * pJym);
void Initstruct_Jymkzfb(JYMKZFB * pJymkzfb);
void CheckYwmx(YWMX *pYwmx);
void Initstruct_Ywmx(YWMX * pYwmx);
void Initstruct_Ywmxgl(YWMXGL * pYwmxgl);
void Initstruct_Zwmx(ZWMX * pZwmx);
void LimPrecision(double * pVar);
void  sGetFile(long  nLb);
//参数说明nLb=1 用于月报表
//参数说明nLb=2 用于查寻日对帐
//参数说明nLb=3 用于查操作员
//参数说明nLb=4 用于绿卡帐号管理的支局号输入在线查寻
//参数说明：请使用者自行添加类别并在此记录其用处

#ifdef DOS
long g_bWaitReadFlag = 1; // 1 等待读 0 不等待读
#endif

#ifdef DOS
void SetNoWaitRead()
{
  g_bWaitReadFlag = 0;
}

void SetWaitRead()
{
  g_bWaitReadFlag = 1;
}
#endif

/*************************************
**
**函数: ShowChar
**目的: 设置为字符正常显示方式
**参数: 无
**返回值: 无
**
*************************************/


void ShowChar()
{
  #ifdef UNIX
  printf("\033[0m");
  #endif
  #ifdef DOS
  nCursorAttr=7;
  #endif
}


/*选择录入值为 “Y”的返回 */

long GetYn()
{
  long nKey;
  fflush(stdin);
  EnglishInput();
  while(1) 
  {
    nKey=keyb();
    switch(nKey)
    {
      case 'y':
      case 'Y':
        return KEY_YES;
        break;
      case 'n':
      case 'N':
        return KEY_NO;
        break;
      default:
        outputchar('\a');
        break;
    }
  }
}



long Get12()
{
  long nKey;
  fflush(stdin);
  EnglishInput();
  while(1) 
  {
    nKey=keyb();
    switch(nKey)
    {
      case '1':
        return KEY_YES;
        break;
      case '2':
        return KEY_NO;
        break;
      default:
        outputchar('\a');
        break;
    }
  }
}




/*************************************
**
**函数: ReverseChar
**目的: 设置为字符反显方式
**参数: 无
**返回值: 无
**
*************************************/

void ReverseChar()
{
  #ifdef UNIX
  printf("\033[7m");
  #endif
  #ifdef DOS
  nCursorAttr=0x70;
  #endif
}

#ifdef DOS
/*************************************
**
**函数: FillAttr
**目的: 输出字符到屏幕,只用于outputchar
**参数: ch 输出字符
        n 输出字符个数
        attr 颜色属性
**返回值: 无
**
*************************************/

void FillAttr(long ch,long n,long attr)
{
  union REGS s;
  s.h.ah=0x09;
  s.h.al=(unsigned char)ch;
  s.h.bh=0;
  s.h.bl=(unsigned char)attr;
  s.w.cx=n;
  int386(0x010,&s,&s);
  gotor(n);
}
#endif

/*************************************
**
**函数: clrscr
**目的: 清屏
**参数: 无
**返回值: 无
**
*************************************/

void clrscr()
{
  #ifdef UNIX
  printf("\033[2J");
  #else
  union REGS s;
  s.h.ah = 6;
  s.h.al = 0;
  s.h.bh = 0x07;
  s.h.ch = 0;
  s.h.cl = 0;
  s.h.dh = 24;
  s.h.dl = 79;
  int386(0x10,&s,&s);
  #endif
  gotoxy(1,1);
  memset(sScreenBuffer,' ',2000);
}

/*************************************
**
**函数: getxy
**目的: 取光标位置
**参数: x 横坐标
**      y 纵坐标
**返回值: 无
**
*************************************/

void getxy(long *x,long *y)
{
  #ifdef DOS
  union REGS s;
  s.h.ah=3;
  s.h.bh=0;
  int386(0x10,&s,&s);
  *x=s.h.dl+1;
  *y=s.h.dh+1;
  return;
  #endif
  #ifdef UNIX
  *x=nCursorX;
  *y=nCursorY;
  return;
  #endif
}

/*************************************
**
**函数: gotoxy
**目的: 定位光标
**参数: x为横坐标,y为纵坐标
**返回值: 无
**
*************************************/

void gotoxy(long x,long y)
{
  #ifdef UNIX
  printf("\033[%d;%dH",y,x);
  #endif
  #ifdef DOS
  union REGS s;
  fflush(stdout);
  s.w.ax=0x0200;
  s.h.bh=0;
  s.h.dh=(unsigned char)y-1;
  s.h.dl=(unsigned char)x-1;
  int386(0x10,&s,&s);
  #endif
  nCursorX=x;
  nCursorY=y;
}

/*************************************
**
**函数: gotol
**目的: 光标向左走一行或多行
**参数: x 所走行数
**返回值: 无
**
*************************************/

void gotol(long x)
{
  #ifdef UNIX
  printf("\033[%dD",x);
  #endif
  #ifdef DOS
  long curx,cury;
  getxy(&curx,&cury);
  curx-=x;
  if(curx<1)
    curx=1;
  gotoxy(curx,cury);
  #endif
  nCursorX-=x;
}

/*************************************
**
**函数: gotor
**目的: 光标向右走一行或多行
**参数: x 所走行数
**返回值: 无
**
*************************************/

void gotor(long x)
{
  #ifdef UNIX
  printf("\033[%dC",x);
  #endif
  #ifdef DOS
  long curx,cury;
  getxy(&curx,&cury);
  curx+=x;
  if(curx>80)
    curx=80;
  gotoxy(curx,cury);
  #endif
  nCursorX+=x;
}

/*************************************
**
**函数: gotou
**目的: 光标向上走一行或多行
**参数: y 所走行数
**返回值: 无
**
*************************************/

void gotou(long y)
{
  #ifdef UNIX
  printf("\033[%dA",y);
  #endif
  #ifdef DOS
  long curx,cury;
  getxy(&curx,&cury);
  cury-=y;
  if(cury<1)
    cury=1;
  gotoxy(curx,cury);
  #endif
  nCursorY-=y;
}

/*************************************
**
**函数: gotod
**目的: 光标向下走一行或多行
**参数: y 所走行数
**返回值: 无
**
*************************************/

void gotod(long y)
{
  #ifdef UNIX
  printf("\033[%dB",y);
  #endif
  #ifdef DOS
  long curx,cury;
  getxy(&curx,&cury);
  cury+=y;
  if(cury>25)
    curx=25;
  gotoxy(curx,cury);
  #endif
  nCursorY+=y;
}

/*************************************
**
**函数: outputchar
**目的: 向屏幕输出一字符
**参数: ch 所要输出的字符
**返回值: 无
**
*************************************/

void outputchar(long ch)
{
  if (ch>=0x20&&ch<=0x7e||ch>=0xa1&&ch<=0xfe)
  {
    #ifdef DOS
    FillAttr(ch,1,nCursorAttr);
    #endif
    #ifdef UNIX
    putchar((int)ch);
    #endif
    sScreenBuffer[(nCursorY-1)*80+(nCursorX-1)]=(unsigned char)ch;
    nCursorX++;
    if(nCursorX==81)
    {
      nCursorX=1;
      nCursorY++;
      if(nCursorY==25)
        nCursorY=1;
    }
  }
  else
  {
    if(ch=='\n')
    {
      nCursorX=1;
      nCursorY++;
      if(nCursorY==25)
        nCursorY=1;
    }
    if(ch=='\r')
    {
      nCursorX=1;
    }
    putchar((int)ch);
  }
  fflush(stdout);
  return;
}




/*************************************
**
**函数: output
**目的: 格式化输出到屏幕或打印机或文本,带有变长列表
**      执行setprinton()输出到打印机
**      执行openfile()输出到文本
**参数: sFormat 输出格式,同printf的输出格式 
**返回值: 实际输出的字符数
**
*************************************/

long output(char *sFormat,...)
{
  char sBuffer[2001];
  unsigned char ch;
  long i;
  long nCharCount;/*返回实际输出 的字节数*/
  va_list pVarArgList;/*变长参数列表指针*/

  va_start(pVarArgList,sFormat);

  switch(nOutPutHandle)
  {
    case OUTPUT_SCREEN:
      nCharCount=vsprintf(sBuffer,sFormat,pVarArgList);
      printf(sBuffer);
      for(i=0;i<strlen(sBuffer);i++)
      {
        ch=sBuffer[i];
        if (ch>=0x20&&ch<=0x7e||ch>=0xa1&&ch<=0xfe)
        {
          sScreenBuffer[(nCursorY-1)*80+(nCursorX-1)]=ch;
          nCursorX++;
          if(nCursorX==81)
          {
            nCursorX=1;
            nCursorY++;
            if(nCursorY==25)
              nCursorY=1;
          }
          continue;
        }
        if(ch=='\n')
        {
          nCursorX=1;
          nCursorY++;
          if(nCursorY==25)
            nCursorY=1;
          continue;
        }
        if(ch=='\r')
          nCursorX=1;
      }
      break;
    case OUTPUT_PRINTER:
      #ifdef UNIX
      switch(nSysMode)
      {
        case UNIX_HOST:
          nCharCount=vfprintf(fHostPrn,sFormat,pVarArgList);
          break;
        case UNIX_TERM:
          nCharCount=vprintf(sFormat,pVarArgList);
          break;
      }
      #endif
      #ifdef DOS
      nCharCount=vfprintf(stdprn,sFormat,pVarArgList);
      #endif
      break;
    case OUTPUT_TEXT:
      vsprintf(sBuffer,sFormat,pVarArgList);
      nCharCount=fwrite(sBuffer,1,strlen(sBuffer),fOutPutHandle);
      break;
  }

  va_end(pVarArgList);

  fflush(stdout);

  return nCharCount; 
}

/*************************************
**
**函数: outtext
**目的: 格式化输出到屏幕,带有变长列表
**参数: sFormat 输出格式,同printf的输出格式 
**返回值: 实际输出的字符数
**
*************************************/

long outtext(char *sFormat,...)
{
  char sBuffer[2001];
  unsigned char ch;
  long i;
  long nCharCount;/*返回实际输出 的字节数*/
  va_list pVarArgList;/*变长参数列表指针*/
  #ifdef DOS
  long curx,cury;
  #endif

  va_start(pVarArgList,sFormat);

  nCharCount=vsprintf(sBuffer,sFormat,pVarArgList);

  va_end(pVarArgList);

  #ifdef UNIX
  if(nSysMode==UNIX_TERM&&nOutPutHandle==OUTPUT_PRINTER)
  {
    setprintoff();
    printf(sBuffer);
    setprinton();
  }
  else
    printf(sBuffer);
  #endif
  #ifdef DOS
  getxy(&curx,&cury);
  FillAttr(' ',strlen(sBuffer),nCursorAttr);
  gotoxy(curx,cury);
  printf(sBuffer);
  #endif

  for(i=0;i<strlen(sBuffer);i++)
  {
    ch=sBuffer[i];
    if (ch>=0x20&&ch<=0x7e||ch>=0xa1&&ch<=0xfe)
    {
      if(nCursorX==81)
      {
        nCursorX=1;
        nCursorY++;
        if(nCursorY==25)
          nCursorY=1;
      }
      sScreenBuffer[(nCursorY-1)*80+(nCursorX-1)]=ch;
      nCursorX++;
      continue;
    }
    if(ch=='\n')
    {
      nCursorX=1;
      nCursorY++;
      if(nCursorY==25)
        nCursorY=1;
      continue;
    }
    if(ch=='\r')
      nCursorX=1;
  } 

  fflush(stdout);

  return nCharCount; 
}

#ifdef NOUSE
/*************************************
**
**函数: OutPutWithHandle
**目的: 格式化输出,带有变长列表
**参数: nHandle 输出流向
**      sFormat 输出格式,同printf的输出格式 
**返回值: 实际输出的字符数
**
*************************************/

long OutPutWithHandle(long nHandle,char *sFormat,...)
{
  long nErrorFlag=0,/*语法错误标志,0为false,1为true*/
      nPrecision,/*精度为*号*/ 
      nPrecisionFlag,/*精度为*号标志,0为False,1为true*/
      nCharCount,/*返回实际输出 的字节数*/
      nType;/*变长列表中参数的类型*/
  char* sTop;/*输出格式头指针*/
  char* sBottom;/*输出格式尾指针*/
  char* sTempFormat;/*临时格式*/
  char* sTemp;/*临时输出*/
  char* sOut;/*输出字符串*/
  va_list pVarArgList;/*变长参数列表指针*/

  sTop=sFormat;
  sBottom=sFormat;
  if((sTempFormat=(char*)malloc(sizeof(sFormat)))==NULL)
  {
    fprintf(stderr,"\n\a内存不够\n");
    keyb();
    return 0;
  }
  if((sTemp=(char*)malloc(2000))==NULL)
  {
    fprintf(stderr,"\n\a内存不够\n");
    free(sTempFormat);
    keyb();
    return 0;
  }
  if((sOut=(char*)malloc(2000))==NULL)
  {
    fprintf(stderr,"\n\a内存不够\n");
    free(sTempFormat);
    free(sTemp);
    keyb();
    return 0;
  }
  sOut[0]='\0';
  nType=TYPE_NULL;

  va_start(pVarArgList,sFormat);

  while(sTop&&sTop[0])
  {
    nType=0;
    nPrecisionFlag=0;
    sBottom=(char*)strchr(sTop,'%');
    if(sBottom==NULL)
      nType=TYPE_NULL;
    else
      sBottom++;
    while(nType==0)
    {
      switch(sBottom[0])
      {
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
          nType=TYPE_INT;
          break;
        case 'h':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'd':
              case 'i':
              case 'o':
              case 'x':
              case 'X':
              case 'u':
                nType=TYPE_SHORT;
                break;
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stderr,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'f':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
          nType=TYPE_FLOAT;
          break;
        case 'l':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'd':
              case 'i':
              case 'o':
              case 'x':
              case 'X':
              case 'u':
                nType=TYPE_LONG;
                break;
              case 'f':
              case 'e':
              case 'E':
              case 'g':
              case 'G':
                nType=TYPE_DOUBLE;
                break;  
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stdout,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'L':
          sBottom++;
          while(1)
          {
            switch(sBottom[0])
            {
              case 'f':
              case 'e':
              case 'E':
              case 'g':
              case 'G':
                nType=TYPE_LONGDOUBLE;
                break;  
              case '\0':
                nType=TYPE_NULL;
                break;
              default:
                fprintf(stdout,"\n\a语法错误\n");
                nErrorFlag=1;
                keyb();
                break;
            }
            if (nType)
              break;
            if(sBottom[0]!='\0')
              sBottom++;
          }
          break;
        case 'c':
          nType=TYPE_CHAR;
          break;
        case 's':
          nType=TYPE_STRING; 
          break;
        case '*':
          nPrecisionFlag=1;
          nPrecision=va_arg(pVarArgList,long);
          break;
        case '%':
          sBottom++;
          sBottom=(char*)strchr(sBottom,'%');
          break;
        case '\0':
          nType=TYPE_NULL;
          break;
      }
      if (nType||nErrorFlag)
        break;
      if(sBottom!=NULL&&sBottom[0]!='\0')
        sBottom++;
    }

    if(nErrorFlag)
    {
      va_end(pVarArgList);
      free(sTempFormat);
      free(sTemp);
      free(sOut);
      return 0;
    }

    if(nType!=TYPE_NULL)
    {
      strncpy(sTempFormat,sTop,sBottom-sTop+1);
      sTempFormat[sBottom-sTop+1]='\0';
      sTop=sBottom+1;
    }
    else
    {
      strcpy(sTempFormat,sTop);
      sTop[0]='\0';
    }

    switch(nType)
    { 
      case TYPE_INT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,int));
        break;
      case TYPE_LONG:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long));
        break;
      case TYPE_SHORT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,short));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,short));
        break;
      case TYPE_FLOAT:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,float));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,float));
        break;
      case TYPE_DOUBLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,double));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,double));
        break;
      case TYPE_LONGDOUBLE:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,long double));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,long double));
        break;
      case TYPE_CHAR:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char));
        break;
      case TYPE_STRING:
        if(nPrecisionFlag==0)
          sprintf(sTemp,sTempFormat,va_arg(pVarArgList,char*));
        else
          sprintf(sTemp,sTempFormat,nPrecision,va_arg(pVarArgList,char*));
        break;
      case TYPE_NULL:
        strcpy(sTemp,sTempFormat);
        break;
    }
    strcat(sOut,sTemp);
  }

  va_end(pVarArgList);

  switch(nHandle)
  {
    case OUTPUT_SCREEN:
      nCharCount=fprintf(stdout,sOut);
      break;
    case OUTPUT_PRINTER:
      nCharCount=fprintf(stdout,sOut);
      break;
  }
  fflush(stdout);

  free(sTempFormat);
  free(sTemp);
  free(sOut);

  return nCharCount; 
}
#endif

/*************************************
**
**函数: openfile
**目的: 指定输出到一个文件
**参数: sFileName 文件名
**返回值: 0 成功 1 出错
**
*************************************/
long openfile(char * sFileName)
{
  #ifdef UNIX
  umask(0002);
  #endif
  if((fOutPutHandle=fopen(sFileName,"w+"))==NULL)
    return 1;
  else
  {
    nOutPutHandle=OUTPUT_TEXT;
    return 0;
  }
}

/*************************************
**
**函数: closefile
**目的: 停止输出到文件
**参数: 无
**返回值: 0 成功 1 出错
**
*************************************/
long closefile()
{
  nOutPutHandle=OUTPUT_SCREEN;
  if(fclose(fOutPutHandle)==EOF)
    return 1;
  else
    return 0;
}

/*************************************
**
**函数: drawtable 
**目的: 在指定集团画表格
**参数: nCol 指定横坐标
**      nRow 指定纵坐标 
**      nFieldLen 字段长度数组
**      nFieldNum 字段数 
**      nRowcount 行数
**返回值:无
**
*************************************/

void drawtable(long nCol,long nRow,long nFieldLen[],long nFieldNum,long nRowCount)
{
  char sLine1[3],sLine2[3],sLine3[3],sLine4[3]; 
  long i,j,k;
  for(i=1;i<=2*nRowCount+1;i++)
  {
    gotoxy(nCol,nRow+i-1);
    if (i%2==0)
    {
      strcpy(sLine1,"┃");
      strcpy(sLine2,"  ");
      strcpy(sLine3,"│");
      strcpy(sLine4,"┃");
    }
    else
    {
      if (i==1)
      {
        strcpy(sLine1,"┏");
        strcpy(sLine2,"━");
        strcpy(sLine3,"┯");
        strcpy(sLine4,"┓");
      }
      else
      {
        if (i==2*nRowCount+1)
        {
          strcpy(sLine1,"┗");
          strcpy(sLine2,"━");
          strcpy(sLine3,"┷");
          strcpy(sLine4,"┛");
        }
        else
        {  
          strcpy(sLine1,"┠");
          strcpy(sLine2,"─");
          strcpy(sLine3,"┼");
          strcpy(sLine4,"┨");
        }
      }
    }
    outtext(sLine1);
    for (j=0;j<=nFieldNum-1;j++)
    {
      for (k=1;k<=(nFieldLen[j]+1)/2;k++)
        outtext(sLine2);
      if (j<nFieldNum-1)
        outtext(sLine3);
      else
        outtext(sLine4);
    }
  }
}

/*************************************
**
**函数: drawbox
**目的: 在两个指定点之间画框
**参数: nCol1 起始横坐标
**      nRow1 起始纵坐标 
**      nCol2 终止横坐标
**      nRow2 终止纵坐标
**返回值:无
**
*************************************/

void drawbox(long nCol1,long nRow1,long nCol2,long nRow2)
{
  char sLine1[3],sLine2[3],sLine3[3]; 
  long i,j;
  for(i=nRow1;i<=nRow2;i++)
  {
    gotoxy(nCol1,i);
    if (i==nRow1)
    {
      strcpy(sLine1,"┏");
      strcpy(sLine2,"━");
      strcpy(sLine3,"┓");
    }
    else
    {
      if (i==nRow2)
      {
        strcpy(sLine1,"┗");
        strcpy(sLine2,"━");
        strcpy(sLine3,"┛");
      }
      else
      {  
        strcpy(sLine1,"┃");
        strcpy(sLine2,"  ");
        strcpy(sLine3,"┃");
      }
    }
    outtext(sLine1);
    for (j=nCol1;j<=nCol2-3;j+=2)
      outtext(sLine2);
    outtext(sLine3);
  }
}

/*************************************
**
**函数: getposition
**目的: 获得各字段起始显示列
**参数: nCol 指定横坐标
**      nFieldLen 字段长度数组
**      nPosition 字段横坐标数组
**      nFieldNum 字段数 
**返回值:无
**
*************************************/

void getposition(long nCol,long nFieldLen[],long nPosition[],long nFieldNum)
{
  long i;
  nPosition[0]=nCol+2;
  for (i=1;i<=nFieldNum-1;i++)
    nPosition[i]=nPosition[i-1]+2*((nFieldLen[i-1]+1)/2)+2;
}

/*************************************
**
**函数: clearrecord
**目的: 清空指定行的各字段
**参数: nCol 指定横坐标
**      nRow 指定纵坐标 
**      nFieldLen 字段长度数组
**      nFieldNum 字段数 
**返回值:无
**
*************************************/

void clearrecord(long nCol,long nRow,long nFieldLen[],long nFieldNum)
{
  long i;
  for (i=0;i<=nFieldNum-1;i++)
  {
    gotoxy(nCol+2,nRow);
    nCol+=nFieldLen[i]+2;
    putspace(nFieldLen[i]);
  }
}

/*************************************
**
**函数: alltrim 
**目的: 删除字符串左右空格 
**参数: 字符串指针 
**返回值: 无
**
*************************************/

void alltrim(char *s)
{
  long i;
  char* sTop;
  sTop=s;
  while(*sTop==' '||*sTop==9)
  {
    sTop++;
  } 
  if(*sTop=='\0')
  {
    s[0]='\0';
    return;
  }
  if(sTop!=s)
  {
    strcpy(s,sTop);
  }
  i=strlen(s)-1;
  while(s[i]==' '||s[i]==9)
    i--;
  s[i+1]='\0';
}

/*************************************
**
**函数: rtrim 
**目的: 删除字符串右空格 
**参数: 字符串指针 
**返回值: 无
**
*************************************/

void rtrim(char* s)
{
  long i;
  if(s[0]=='\0')
    return;
  i=strlen(s)-1;
  while(s[i]==' '||s[i]==9)
    i--;
  s[i+1]='\0';
}

/*************************************
**
**函数: getkey
**目的: 根据接收字符类型接收按键
**参数: nDataType 接收字符类型
**返回值: 返回按键
**
*************************************/

long getkey(long nDataType)
{
  long nKey,nLen;
  fflush(stdout);
  #ifdef UNIX
  
  switch(nTermType)
  {
    case TERM_ANSI:
      nKey=GetKeyUsingAnsi(nDataType);
      break;
    case TERM_VT100:
      nKey=GetKeyUsingVt100(nDataType);
      break;
    case TERM_VT220:
      nKey=GetKeyUsingVt220(nDataType);
      break;
  }

  #endif
  #ifdef DOS
  nKey=GetKeyUnderDos(nDataType);
  #endif
  fflush(stdout);
  return nKey;
}

#ifdef UNIX

// 以下修改vt100模式不能输入汉字的问题
int g_nWeiMa = 0;
// 以上修改vt100模式不能输入汉字的问题

/*************************************
**
**函数: GetKeyUsingAnsi
**目的: 在Ansi下根据接收字符类型接收按键,只被getkey调用
**参数: nDataType 接收字符类型
**返回值: 返回按键
**
*************************************/

long GetKeyUsingAnsi(long nDataType)
{
/***	
  long i;
  int ch;
  int n;
  unsigned char s[3];

  int nWeiMa;
  
  nWeiMa = g_nWeiMa;
  g_nWeiMa = 0;
  
  if (nDataType==TYPE_CHAR&&nWeiMa > 0)
  {
    return nWeiMa;
  }
  
  fflush(stdin);
  s[0]=s[1]=s[2]='\0';
  n = read(0,(void *)s,2);

// 用于SetNoWaitRead立即返回
  if(n == 0)
    return 0;

//outtext("\nkey_%s\n",s);
//sleep(2);
  if (nDataType==TYPE_CHAR&&s[0]>=0x20&&s[0]<=0x7e)
  { 
    if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }

  if (nDataType==TYPE_CHAR&&s[0]>=0xa1&&s[0]<=0xfe)
  { 
    if(s[1]>=0xa1&&s[1]<=0xfe)
    {
      g_nWeiMa = s[1];
    }
    else if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }

  if (nDataType==TYPE_INT&&(s[0]>='0'&&s[0]<='9'||s[0]=='-'))
  {
    if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }  
  if (nDataType==TYPE_FLOAT&&(s[0]>='0'&&s[0]<='9'||s[0]=='.'||s[0]=='-'))
  {
    if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }  
  if (s[0]=='\n'||s[0]=='\r')
  {
    if(s[1])
      ungetc((int)s[1],stdin);
    return KEY_RETURN;
  }
  if (s[0]==8)
    return KEY_BACKSPACE;
  if (s[0]==9)
    return KEY_TAB;
  if (s[0]==0x7f)
    return KEY_DELETE;

  if (s[0]==0x1b&&s[1]==0)
    return KEY_ESC;
  if (s[0]==0x1b&&s[1]=='[')  
  {
    read(0,(void *)s,1);
    switch(s[0])   
    {
      case 'A':
        return KEY_UP;
      case 'B':
        return KEY_DOWN;
      case 'C':
        return KEY_RIGHT;
      case 'D':
        return KEY_LEFT;
      case 'F':
        return KEY_END;
      case 'G':
        return KEY_PAGEDOWN;
      case 'H':
        return KEY_HOME;
      case 'I':
        return KEY_PAGEUP;
      case 'L':
        return KEY_INSERT;
      case 'M':
        return KEY_F1;
      case 'N':
        return KEY_F2;
      case 'O':
        return KEY_F3;
      case 'P':
        return KEY_F4;
      case 'Q':
        return KEY_F5;
      case 'R':
        return KEY_F6;
      case 'S':
        return KEY_F7;
      case 'T':
        return KEY_F8;
      case 'U':
        return KEY_F9;
      case 'V':
        return KEY_F10;
      case 'W':
        return KEY_F11;
      case 'X':
        return KEY_F12;
    }
    fflush(stdin);
  }
  ***/
  
  long i;
  int ch;
  int n;
  unsigned char s[3];

  int nWeiMa;
  extern int errno;
  
  nWeiMa = g_nWeiMa;
  g_nWeiMa = 0;
  
  if (nDataType==TYPE_CHAR&&nWeiMa > 0)
  {
    return nWeiMa;
  }
  
  fflush(stdin);
  s[0]=s[1]=s[2]='\0';
  errno=0;
  n = read(0,(void *)s,2);

// 用于SetNoWaitRead立即返回
  if(n == 0)return 0;

  if (nDataType==TYPE_CHAR&&s[0]>=0x20&&s[0]<=0x7e)
  { 
    if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }

  if (nDataType==TYPE_CHAR&&s[0]>=0xa1&&s[0]<=0xfe)
  { 
    if(s[1]>=0xa1&&s[1]<=0xfe)
    {
      g_nWeiMa = s[1];
    }
    else if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }

  if (nDataType==TYPE_INT&&(s[0]>='0'&&s[0]<='9'||s[0]=='-'))
  {
    if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }  
  if (nDataType==TYPE_FLOAT&&(s[0]>='0'&&s[0]<='9'||s[0]=='.'||s[0]=='-'))
  {
    if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }  
  if (s[0]=='\n'||s[0]=='\r')
  {
    if(s[1])
      ungetc((int)s[1],stdin);
    return KEY_RETURN;
  }
  if (s[0]==8)
    return KEY_BACKSPACE;
  if (s[0]==9)
    return KEY_TAB;
  if (s[0]==0x7f)
    return KEY_DELETE;

  if (s[0]==0x1b&&s[1]==0)
    return KEY_ESC;
  if (s[0]==0x1b&&s[1]=='[')  
  {
    read(0,(void *)s,1);
    switch(s[0])   
    {
      case 'A':
        return KEY_UP;
      case 'B':
        return KEY_DOWN;
      case 'C':
        return KEY_RIGHT;
      case 'D':
        return KEY_LEFT;
      case 'F':
        return KEY_END;
      case 'G':
        return KEY_PAGEDOWN;
      case 'H':
        return KEY_HOME;
      case 'I':
        return KEY_PAGEUP;
      case 'L':
        return KEY_INSERT;
      case 'M':
        return KEY_F1;
      case 'N':
        return KEY_F2;
      case 'O':
        return KEY_F3;
      case 'P':
        return KEY_F4;
      case 'Q':
        return KEY_F5;
      case 'R':
        return KEY_F6;
      case 'S':
        return KEY_F7;
      case 'T':
        return KEY_F8;
      case 'U':
        return KEY_F9;
      case 'V':
        return KEY_F10;
      case 'W':
        return KEY_F11;
      case 'X':
        return KEY_F12;
      case '?':
        ch=getchar();
        if(ch=='1')
        {
          ch=getchar();
          getchar();
          switch(ch)
          {
            case '0':
              return KEY_PRNREADY;
            case '1':
              return KEY_PRNNOTREADY;
            case '3':
              return KEY_PRNNOTEXIST;
          }
        }
        break;
    }
    fflush(stdin);
  }

}

/*************************************
**
**函数: GetKeyUsingVt100
**目的: 在VT100下根据接收字符类型接收按键,只被getkey调用
**参数: nDataType 接收字符类型
**返回值: 返回按键
**
*************************************/

long GetKeyUsingVt100(long nDataType)
{
  long i;
  int ch;
  int n;
  unsigned char s[3];

// 以下修改vt100模式不能输入汉字的问题
  int nWeiMa;
  
  nWeiMa = g_nWeiMa;
  g_nWeiMa = 0;
  
  if (nDataType==TYPE_CHAR&&nWeiMa > 0)
  {
    return nWeiMa;
  }

  fflush(stdin);
  s[0]=s[1]=s[2]='\0';
  n = read(0,(void *)s,2);
  
// 用于SetNoWaitRead立即返回
  if(n == 0)
    return 0;

  if (nDataType==TYPE_CHAR&&s[0]>=0x20&&s[0]<=0x7e)
  { 
    if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }

  if (nDataType==TYPE_CHAR&&s[0]>=0xa1&&s[0]<=0xfe)
  { 
    if(s[1]>=0xa1&&s[1]<=0xfe)
    {
      g_nWeiMa = s[1];
    }
    else if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }
// 以上修改vt100模式不能输入汉字的问题

  if (nDataType==TYPE_INT&&(s[0]>='0'&&s[0]<='9'||s[0]=='-'))
  {
    if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }  
  if (nDataType==TYPE_FLOAT&&(s[0]>='0'&&s[0]<='9'||s[0]=='.'||s[0]=='-'))
  {
    if(s[1])
      ungetc((int)s[1],stdin);
    return s[0];
  }  
  if (s[0]=='\n'||s[0]=='\r')
  {
    if(s[1])
      ungetc((int)s[1],stdin);
    return KEY_RETURN;
  }
  if (s[0]==8)
    return KEY_BACKSPACE;
  if (s[0]==9)
    return KEY_TAB;
  if (s[0]==0x1b)  
  {
    switch (s[1])
    {
      case 0:
        return KEY_ESC;
      case '[':
        switch(ch=getchar())
        {
          case '1':
            switch(ch=getchar())
            {
              case '~':
                return KEY_INSERT;
                break;
              case '7':
                if ((ch=getchar())=='~')
                  return KEY_F1;
                  break;
              case '8':
                if ((ch=getchar())=='~')
                  return KEY_F2;
                  break;
              case '9':
                if ((ch=getchar())=='~')
                  return KEY_F3;
                break;
            }
            break;
          case '2':
            switch(ch=getchar())
            {
              case '~':
                return KEY_HOME;
              case '0':
                if ((ch=getchar())=='~')
                  return KEY_F4;
                break;
              case '1':
                if ((ch=getchar())=='~')
                  return KEY_F5;
                break;
              case '3':
                if ((ch=getchar())=='~')
                  return KEY_ESC;
                break;
              case '4':
                if ((ch=getchar())=='~')
                  return KEY_F6;
                break;
              case '5':
                if ((ch=getchar())=='~')
                  return KEY_F7;
                break;
              case '6':
                if ((ch=getchar())=='~')
                  return KEY_F8;
                break;
            }
            break;
          case '3':
            switch(ch=getchar())
            {
              case '~':
                return KEY_PAGEUP;
              case '1':
                if ((ch=getchar())=='~')
                  return KEY_F9;
                break;
              case '2':
                if ((ch=getchar())=='~')
                  return KEY_F10;
                break;
              case '3':
                if ((ch=getchar())=='~')
                  return KEY_F11;
                break;
              case '4':
                if ((ch=getchar())=='~')
                  return KEY_F12;
                break;
            }
            break;
          case '4':
            switch(ch=getchar())
            {
              case '~':
                return KEY_DELETE;
              break;
            }
            break;
          case '5':
            switch(ch=getchar())
            {
              case '~':
                return KEY_END;
                break;
            }
            break;
          case '6':
            switch(ch=getchar())
            {
              case '~':
                return KEY_PAGEDOWN;
                break;
            }
            break;
          case 'A':
            return KEY_UP;
            break;
          case 'B':
            return KEY_DOWN;
            break;
          case 'C':
            return KEY_RIGHT;
            break;
          case 'D':
            return KEY_LEFT;
            break;
          case '?':
            ch=getchar();
            if(ch=='1')
            {
              ch=getchar();
              getchar();
              switch(ch)
              {
                case '0':
                  return KEY_PRNREADY;
                case '1':
                  return KEY_PRNNOTREADY;
                case '3':
                  return KEY_PRNNOTEXIST;
              }
            }
            break;
        }
        break;
      case 'O':
        switch(ch=getchar())
        {
          case 'M':
            return KEY_RETURN;
            break;
          case 'P':
            return KEY_NUMLOCK;
            break;
          case 'Q':
            return '/';
            break;
          case 'R':
            return '*';
            break;
          case 'S':
            return '-'; 
            break;
          case 'm':
            return '+';
            break;
          case 'n':
            return '.';
            break;
          case 'p':
            return '0';
            break;
          case 'q':
            return '1';
            break;
          case 'r':
            return '2';
            break;
          case 's':
            return '3';
            break;
          case 't':
            return '4';
            break;
          case 'u':
            return '5';
            break;
          case 'v':
            return '6';
            break;
          case 'w':
            return '7';
            break;
          case 'x':
            return '8';
            break;
          case 'y':
            return '9';
            break;
        } 
        break;
    }
    fflush(stdin);
  }
}

/*************************************
**
**函数: GetKeyUsingVt220
**目的: 在VT220下根据接收字符类型接收按键,只被getkey调用
**参数: nDataType 接收字符类型
**返回值: 返回按键
**
*************************************/

long GetKeyUsingVt220(long nDataType)
{
  int ch;
  while(1)
  {
    ch=getchar();
    
    // 用于SetNoWaitRead立即返回
    if(ch == -1)
      return 0;
    
    if (nDataType==TYPE_CHAR&&(ch>=0x20&&ch<=0x7e||ch>=0xa1&&ch<=0xfe))
      return ch;
    if (nDataType==TYPE_INT&&(ch>='0'&&ch<='9'||ch=='-'))
      return ch;
    if (nDataType==TYPE_FLOAT&&(ch>='0'&&ch<='9'||ch=='.'||ch=='-'))
      return ch;
    if (ch=='\n' || ch == '\r')
      return KEY_RETURN;
    if (ch==8)
      return KEY_BACKSPACE;
    if (ch==0x1b)
    {
      switch (ch=getchar())
      {
        case '[':
          switch(ch=getchar())
          {
            case '1':
              switch(ch=getchar())
              {
                case '~':
                  return KEY_INSERT;
                case '7':
                  if ((ch=getchar())=='~')
                    return KEY_F1;
                  break;
                case '8':
                  if ((ch=getchar())=='~')
                    return KEY_F2;
                  break;
                case '9':
                  if ((ch=getchar())=='~')
                     return KEY_F3;
                   break;
              }
              break;
            case '2':
              switch(ch=getchar())
              {
                case '~':
                  return KEY_HOME;
                case '0':
                  if ((ch=getchar())=='~')
                    return KEY_F4;
                  break;
                case '1':
                  if ((ch=getchar())=='~')
                    return KEY_F5;
                  break;
                case '3':
                  if ((ch=getchar())=='~')
                    return KEY_ESC;
                  break;
                case '4':
                  if ((ch=getchar())=='~')
                    return KEY_F6;
                  break;
                case '5':
                  if ((ch=getchar())=='~')
                    return KEY_F7;
                  break;
                case '6':
                  if ((ch=getchar())=='~')
                    return KEY_F8;
                  break;
              }
              break;
            case '3':
              switch(ch=getchar())
              {
                case '~':
                  return KEY_PAGEUP;
                case '1':
                  if ((ch=getchar())=='~')
                    return KEY_F9;
                  break;
                case '2':
                  if ((ch=getchar())=='~')
                    return KEY_F10;
                  break;
                case '3':
                  if ((ch=getchar())=='~')
                    return KEY_F11;
                  break;
                case '4':
                  if ((ch=getchar())=='~')
                    return KEY_F12;
                  break;
              }
              break;
            case '4':
              switch(ch=getchar())
              {
                case '~':
                  return KEY_DELETE;
              }
              break;
            case '5':
              switch(ch=getchar())
              {
                case '~':
                  return KEY_END;
              }
              break;
            case '6':
              switch(ch=getchar())
              {
                case '~':
                  return KEY_PAGEDOWN;
              }
              break;
            case 'A':
              return KEY_UP;
            case 'B':
              return KEY_DOWN;
            case 'C':
              return KEY_RIGHT;
            case 'D':
              return KEY_LEFT;
            case '?':
              ch=getchar();
              if(ch=='1')
              {
                ch=getchar();
                getchar();
                switch(ch)
                {
                  case '0':
                    return KEY_PRNREADY;
                  case '1':
                    return KEY_PRNNOTREADY;
                  case '3':
                    return KEY_PRNNOTEXIST;
                }
              }
              break;
          }
          break;
        case 'O':
          switch(ch=getchar())
          {
            case 'M':
              return KEY_RETURN;
            case 'P':
              return KEY_NUMLOCK;
            case 'Q':
              return '/';
            case 'R':
              return '*';
            case 'S':
              return '-'; 
            case 'm':
              return '+';
            case 'n':
              return '.';
            case 'p':
              return '0';
            case 'q':
              return '1';
            case 'r':
              return '2';
            case 's':
              return '3';
            case 't':
              return '4';
            case 'u':
              return '5';
            case 'v':
              return '6';
            case 'w':
              return '7';
            case 'x':
              return '8';
            case 'y':
              return '9';
          }
          break;
//heli 
     default :
         return KEY_ESC;
     }//end switch
    } //end if
    fflush(stdin);
  }//end while 
}
#endif

#ifdef DOS

/*************************************
**
**函数: GetKeyUnderDos
**目的: 在Dos下根据接收字符类型接收按键,只被getkey调用
**参数: nDataType 接收字符类型
**返回值: 返回按键
**
*************************************/

long GetKeyUnderDos(long nDataType)
{
  int ch;

  // 以下更改为SetNoWaitRead()和SetWaitRead()
  if(g_bWaitReadFlag == 0)
  {
    if(kbhit() == 0)
    {
      return 0;
    }
  }
  // 以上更改为SetNoWaitRead()和SetWaitRead()

  while(1)
  {
    ch=getch();
    if (nDataType==TYPE_CHAR&&(ch>=0x20&&ch<=0x7e||ch>=0xa1&&ch<=0xfe))
      return ch;
    if (nDataType==TYPE_INT&&(ch>='0'&&ch<='9'||ch=='-'))
      return ch;
    if (nDataType==TYPE_FLOAT&&(ch>='0'&&ch<='9'||ch=='.'||ch=='-'))
      return ch;
    if (ch==27)
      return KEY_ESC;
    if (ch=='\n' || ch == '\r')
      return KEY_RETURN;
    if (ch==8)
      return KEY_BACKSPACE;
    if (ch==9)
      return KEY_TAB;
    while(1)
    {
      if (ch==0)
      {
        switch (ch=getch())
        {
          case 59:
            return KEY_F1;
            break;
          case 60:
            return KEY_F2;
            break;
          case 61:
            return KEY_F3;
            break;
          case 62:
            return KEY_F4;
            break;
          case 63:
            return KEY_F5;
            break;
          case 64:
            return KEY_F6;
            break;
          case 65:
            return KEY_F7;
            break;
          case 66:
            return KEY_F8;
            break;
          case 67:
            return KEY_F9;
            break;
          case 68:
            return KEY_F10;
            break;
          case 71:
            return KEY_HOME;
            break;
          case 72:
            return KEY_UP;
            break;
          case 73:
            return KEY_PAGEUP;
            break;
          case 75:
            return KEY_LEFT;
            break;
          case 77:
            return KEY_RIGHT;
            break;
          case 79:
            return KEY_END;
            break;
          case 80:
            return KEY_DOWN;
            break;
          case 81:
            return KEY_PAGEDOWN;
            break;
          case 82:
            return KEY_INSERT;
            break;
          case 83:
            return KEY_DELETE;
            break;
          case 133:
            return KEY_F11;
            break;
          case 134:
            return KEY_F12;
            break;
        }
      }
      fflush(stdin);
      break;
    }
  }
}
#endif

/*************************************
**
**函数: ReverseXY
**目的: 在屏幕上反显一字符串,不外用
**参数: pTextBox 文本框指针
**返回值: 无
**
*************************************/

void ReverseXY(TEXTBOX * pTextBox)
{
  #ifdef DOS
  long i;
  #endif

  gotoxy(pTextBox->x,pTextBox->y);
  putspace(strlen(pTextBox->sValue));
  pTextBox->sValue[pTextBox->nLen]='\0';
  if(strlen(pTextBox->sValue)<pTextBox->nLen)
    sprintf(pTextBox->sValue,"%-*s",pTextBox->nLen,pTextBox->sValue);
  ReverseChar();
  gotoxy(pTextBox->x,pTextBox->y);
  #ifdef DOS
  for(i=0;i<pTextBox->nLen;i++)
    outputchar(pTextBox->sValue[i]);
  #endif
  #ifdef UNIX
  printf(pTextBox->sValue);
  #endif
}

/*************************************
**
**函数: ShowXY
**目的: 在屏幕上正常显示一字符串,不外用
**参数: pTextBox 文本框指针
**返回值: 无
**
*************************************/

void ShowXY(TEXTBOX * pTextBox)
{
  #ifdef DOS
  long i,n;
  #endif
  ShowChar();
  gotoxy(pTextBox->x,pTextBox->y);
  #ifdef DOS
  n=strlen(pTextBox->sValue);
  for(i=0;i<pTextBox->nLen;i++)
  {
    if(i<=n-1)
      outputchar(pTextBox->sValue[i]);
    else
      outputchar(' ');
  }
  #endif
  #ifdef UNIX
  printf("%-*s",pTextBox->nLen,pTextBox->sValue);
  #endif
}

/*************************************
**
**函数: setprinton
**目的: 输出指向打印机
**参数: 无
**返回值: 无
**
*************************************/

void setprinton()
{
  #ifdef UNIX
  if(nSysMode==UNIX_TERM)
    printf("\033[5i");
  else if(nSysMode==UNIX_HOST)
  {
    if((fHostPrn=fopen("/dev/lp0","w"))==NULL)
      return;
  }
  #endif
  nOutPutHandle=OUTPUT_PRINTER;
}

/*************************************
**
**函数: setprintoff
**目的: 停止向打印机输出
**参数:  无
**返回值: 无
**
*************************************/

void setprintoff()
{
  #ifdef UNIX
  if(nOutPutHandle==OUTPUT_PRINTER)
  {
    if(nSysMode==UNIX_TERM)
      printf("\033[4i");
    else if(nSysMode==UNIX_HOST)
      fclose(fHostPrn);
  }
  #endif
  nOutPutHandle=OUTPUT_SCREEN;
}

/*************************************
**
**函数: getnumber
**目的: 接收整数字符串
**参数: pTextBox 文本框指针
**返回值: 功能键
**
*************************************/

long getnumber(TEXTBOX * pTextBox)
{
  long nKey;
  nKey=GetReverseNumber(pTextBox);
  ShowXY(pTextBox);
  return nKey;
}

/*************************************
**
**函数: GetReverseNumber
**目的: 接收浮点字符串,只被getnumber调用
**参数: pTextBox 文本框指针
**返回值: 功能键
**
*************************************/

long GetReverseNumber(TEXTBOX * pTextBox)
{
  long i,nFlag=0,nPosition=0,nKey,nCurLen;
  long oldlen=0,j;
  long aa=0;

  strcpy(pTextBox->sOldValue,pTextBox->sValue);
  oldlen=strlen(pTextBox->sOldValue);
  pTextBox->nFlag=0;
  ReverseXY(pTextBox);
  gotoxy(pTextBox->x,pTextBox->y);
  while(!nFlag)
  {
    strcpy(pTextBox->sOldValue,pTextBox->sValue);
    oldlen=strlen(pTextBox->sOldValue);         
    nKey=keyn();
    if (nKey>='0'&&nKey<='9'||nKey=='-'&&nPosition==0&&!strchr(pTextBox->sValue,'-'))
    {
      pTextBox->nFlag=1;

      nCurLen=strlen(pTextBox->sValue);
      /*
      for(i=nCurLen;i>nPosition;i--)
        pTextBox->sValue[i]=pTextBox->sValue[i-1];
      */
      if(nPosition+1<oldlen)
      {
      for(j=nPosition+1;j<oldlen;j++)
      {
              pTextBox->sValue[j]=pTextBox->sOldValue[j];
      }
      pTextBox->sValue[j+1]='\0';
      }
      pTextBox->sValue[nPosition]=(unsigned char)nKey;
    if(nCurLen<pTextBox->nLen)
        pTextBox->sValue[nCurLen+1]='\0';
      else
        pTextBox->sValue[pTextBox->nLen]='\0';
      for(i=nPosition;i<=nCurLen&&i<pTextBox->nLen;i++)
        outputchar(pTextBox->sValue[i]);
      nPosition++;
      if(nPosition>=pTextBox->nLen)
        return KEY_RETURN; 
      else
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
      continue;
    }
    switch(nKey)
    {
      case KEY_RETURN:
      case KEY_ESC:
      case KEY_UP:
      case KEY_DOWN:
      case KEY_F1:
      case KEY_F2:
      case KEY_F3:
      case KEY_F4:
      case KEY_F5:
      case KEY_F6:
      case KEY_F7:
      case KEY_F8:
      case KEY_F9:
      case KEY_F10:
      case KEY_F11:
      case KEY_F12:
      case KEY_PAGEUP:
      case KEY_PAGEDOWN:
      case KEY_TAB:
        return nKey;
      case KEY_RIGHT:
        if(pTextBox->sValue[0]=='\0')
          return KEY_RIGHT;
        if(nPosition<pTextBox->nLen-1&&pTextBox->sValue[nPosition]!=' ')
        {
          gotor(1);
          nPosition++;
        }
        else 
          return KEY_RIGHT;
        break;
      case KEY_LEFT:
        if(nPosition>=1)
        {
          gotol(1);
          nPosition--;
        }
        else
          return KEY_LEFT;
        break;
      case KEY_BACKSPACE:
        if (nPosition==0)
          break;
        pTextBox->nFlag=1;
        gotol(1);
        nCurLen=strlen(pTextBox->sValue);
        for(i=nPosition-1;i<nCurLen-1;i++)
        {
          pTextBox->sValue[i]=pTextBox->sValue[i+1];
          outputchar(pTextBox->sValue[i]); 
        }
        outputchar(' ');
        nPosition--;
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
        pTextBox->sValue[nCurLen-1]=' ';
        break;
      case KEY_DELETE:
        pTextBox->nFlag=1;
        nCurLen=strlen(pTextBox->sValue);
        for(i=nPosition;i<nCurLen-1;i++)
        {
          pTextBox->sValue[i]=pTextBox->sValue[i+1];
          outputchar(pTextBox->sValue[i]);
        }
        outputchar(' ');
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
        pTextBox->sValue[nCurLen-1]=' ';
        break;
      default:
        fflush(stdin);
        break;
    }
  }
  return 0;
}

/*************************************
**
**函数: getfloat
**目的: 接收浮点字符串
**参数: pTextBox 文本框指针
**返回值: 功能键
**
*************************************/

long getfloat(TEXTBOX * pTextBox)
{
  long nKey;
  nKey=GetReverseFloat(pTextBox);
  ShowXY(pTextBox);
  return nKey;
}

/*************************************
**
**函数: GetReverseFloat
**目的: 接收浮点字符串,只被getfloat调用
**参数: pTextBox 文本框指针
**返回值: 功能键
**
*************************************/

long GetReverseFloat(TEXTBOX * pTextBox)
{
  long i,nFlag=0,nPosition=0,nKey,nCurLen;
  strcpy(pTextBox->sOldValue,pTextBox->sValue);
  pTextBox->nFlag=0;
  ReverseXY(pTextBox);
  gotoxy(pTextBox->x,pTextBox->y);
  while(!nFlag)
  {
    nKey=keyf();
    if (nKey>='0'&&nKey<='9'||nKey=='-'&&nPosition==0&&!strchr(pTextBox->sValue,'-'))
    {
      pTextBox->nFlag=1;
      pTextBox->sValue[nPosition]='\0';
      nCurLen=strlen(pTextBox->sValue);
      for(i=nCurLen;i>nPosition;i--)
        pTextBox->sValue[i]=pTextBox->sValue[i-1];
      pTextBox->sValue[nPosition]=(unsigned char)nKey;
      if(nCurLen<pTextBox->nLen)
        pTextBox->sValue[nCurLen+1]='\0';
      else
        pTextBox->sValue[pTextBox->nLen]='\0';
      for(i=nPosition;i<=nCurLen&&i<pTextBox->nLen;i++)
        outputchar(pTextBox->sValue[i]);
      nPosition++;
      if(nPosition>=pTextBox->nLen)
        return KEY_RETURN; 
      else
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
      continue;
    }
    if (nKey=='.'&&!strchr(pTextBox->sValue,'.'))
    {
      pTextBox->nFlag=1;
      nCurLen=strlen(pTextBox->sValue);
      for(i=nCurLen;i>nPosition;i--)
        pTextBox->sValue[i]=pTextBox->sValue[i-1];
      pTextBox->sValue[nPosition]=(unsigned char)nKey;
      if(nCurLen<pTextBox->nLen)
        pTextBox->sValue[nCurLen+1]='\0';
      else
        pTextBox->sValue[pTextBox->nLen]='\0';
      for(i=nPosition;i<=nCurLen&&i<pTextBox->nLen;i++)
        outputchar(pTextBox->sValue[i]);
      nPosition++;
      if(nPosition>=pTextBox->nLen)
        return KEY_RETURN; 
      else
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
      continue;
    }
    switch(nKey)
    {
      case KEY_RETURN:
      case KEY_ESC:
      case KEY_UP:
      case KEY_DOWN:
      case KEY_F1:
      case KEY_F2:
      case KEY_F3:
      case KEY_F4:
      case KEY_F5:
      case KEY_F6:
      case KEY_F7:
      case KEY_F8:
      case KEY_F9:
      case KEY_F10:
      case KEY_F11:
      case KEY_F12:
      case KEY_PAGEUP:
      case KEY_PAGEDOWN:
      case KEY_TAB:
        return nKey;
      case KEY_RIGHT:
        if(pTextBox->sValue[0]=='\0')
          return KEY_RIGHT;
        if(nPosition<pTextBox->nLen-1&&pTextBox->sValue[nPosition]!=' ')
        {
          gotor(1);
          nPosition++;
        }
        else 
          return KEY_RIGHT;
        break;
      case KEY_LEFT:
        if(nPosition>=1)
        {
          gotol(1);
          nPosition--;
        }
        else
          return KEY_LEFT;
        break;
      case KEY_BACKSPACE:
        if (nPosition==0)
          break;
        pTextBox->nFlag=1;
        gotol(1);
        nCurLen=strlen(pTextBox->sValue);
        for(i=nPosition-1;i<nCurLen-1;i++)
        {
          pTextBox->sValue[i]=pTextBox->sValue[i+1];
          outputchar(pTextBox->sValue[i]); 
        }
        outputchar(' ');
        nPosition--;
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
        pTextBox->sValue[nCurLen-1]=' ';
        break;
      case KEY_DELETE:
        pTextBox->nFlag=1;
        nCurLen=strlen(pTextBox->sValue);
        for(i=nPosition;i<nCurLen-1;i++)
        {
          pTextBox->sValue[i]=pTextBox->sValue[i+1];
          outputchar(pTextBox->sValue[i]);
        }
        outputchar(' ');
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
        pTextBox->sValue[nCurLen-1]=' ';
        break;
      default:
        fflush(stdin);
        break;
    }
  }
  return 0;
}

/*************************************
**
**函数: getstring
**目的: 接收字符串
**参数: pTextBox 文本框指针
**返回值: 功能键
**
*************************************/

long getstring(TEXTBOX * pTextBox)
{
  long nKey;

  g_nWeiMa = 0;
  nKey=GetReverseString(pTextBox);
  ShowXY(pTextBox);
  return nKey;
}

/*************************************
**
**函数: GetReverseString
**目的: 接收字符串,只被getstring调用
**参数: pTextBox 文本框指针
**返回值: 功能键
**
*************************************/

long GetReverseString(TEXTBOX * pTextBox)
{
  long i,j,nFlag=0,nPosition=0,nCurLen,nKey;
  long lOldLen=0;
  
  strcpy(pTextBox->sOldValue,pTextBox->sValue);
  lOldLen=strlen(pTextBox->sOldValue);
  ReverseXY(pTextBox);
  gotoxy(pTextBox->x,pTextBox->y);
  while(!nFlag)
  {
    strcpy(pTextBox->sOldValue,pTextBox->sValue);
    lOldLen=strlen(pTextBox->sOldValue);        
    nKey=keyb();
    if(nKey>=0x20&&nKey<=0xfe) 
    {
      pTextBox->nFlag=1;
      nCurLen=strlen(pTextBox->sValue);
/*
      for(i=nCurLen;i>nPosition;i--)
        pTextBox->sValue[i]=pTextBox->sValue[i-1];
    */
      if(nPosition+1<lOldLen)
      {
      for(j=nPosition+1;j<lOldLen;j++)
      {
              pTextBox->sValue[j]=pTextBox->sOldValue[j];
      }
      pTextBox->sValue[j+1]='\0';
      }    
      pTextBox->sValue[nPosition]=(unsigned char)nKey;
      if(nCurLen<pTextBox->nLen)
        pTextBox->sValue[nCurLen+1]='\0';
      else
        pTextBox->sValue[pTextBox->nLen]='\0';
      for(i=nPosition;i<=nCurLen&&i<pTextBox->nLen;i++)
        outputchar(pTextBox->sValue[i]);
      nPosition++;
      if(nPosition>=pTextBox->nLen)
        return KEY_RETURN; 
      else
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
      continue;
    }
    switch(nKey)
    {
      case KEY_RETURN:
      case KEY_ESC:
      case KEY_UP:
      case KEY_DOWN:
      case KEY_F1:
      case KEY_F2:
      case KEY_F3:
      case KEY_F4:
      case KEY_F5:
      case KEY_F6:
      case KEY_F7:
      case KEY_F8:
      case KEY_F9:
      case KEY_F10:
      case KEY_F11:
      case KEY_F12:
      case KEY_PAGEUP:
      case KEY_PAGEDOWN:
      case KEY_TAB:
        return nKey;
      case KEY_RIGHT:
        if(pTextBox->sValue[0]=='\0')
          return KEY_RIGHT;
        if(nPosition<strlen(pTextBox->sValue)-1)
        {
          if(pTextBox->sValue[nPosition]>0)
          {
            gotor(1);
            nPosition++;
          }
          else
          {
            gotor(1);
            nPosition++;
            if(pTextBox->sValue[nPosition]<0)
            {
              gotor(1);
              nPosition++;
            }
          }
        }
        else 
          return KEY_RIGHT;
        break;
      case KEY_LEFT:
        if(nPosition>=1)
        {
          gotol(1);
          nPosition--;
          if(nPosition>0&&pTextBox->sValue[nPosition]<0)
          {
            gotol(1); 
            nPosition--;
          }
        }
        else
          return KEY_LEFT;
        break;
      case KEY_BACKSPACE:
        if (nPosition==0)
          break;
        pTextBox->nFlag=1;
        gotol(1);
        nCurLen=strlen(pTextBox->sValue);
        for(i=nPosition-1;i<nCurLen-1;i++)
        {
          pTextBox->sValue[i]=pTextBox->sValue[i+1];
          outputchar(pTextBox->sValue[i]); 
        }
        outputchar(' ');
        nPosition--;
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
        pTextBox->sValue[nCurLen-1]=' ';
        break;
      case KEY_DELETE:
        pTextBox->nFlag=1;
        nCurLen=strlen(pTextBox->sValue);
        if(pTextBox->sValue[nPosition]<0)
        {
          for(i=nPosition;i<nCurLen-2;i++)
          {
            pTextBox->sValue[i]=pTextBox->sValue[i+2];
            outputchar(pTextBox->sValue[i]);
          }
          outputchar(' ');
          outputchar(' '); 
          gotoxy(pTextBox->x+nPosition,pTextBox->y);
          pTextBox->sValue[nCurLen-2]=' ';
          pTextBox->sValue[nCurLen-1]=' ';
        }
        else
        {
          for(i=nPosition;i<nCurLen-1;i++)
          {
            pTextBox->sValue[i]=pTextBox->sValue[i+1];
            outputchar(pTextBox->sValue[i]);
          }
          outputchar(' ');
          gotoxy(pTextBox->x+nPosition,pTextBox->y);
          pTextBox->sValue[nCurLen-1]=' ';
        }
        break;
      default:
        fflush(stdin);
        break;
    }
  }
  return 0;
}

/*************************************
**
**函数: getpassword
**目的: 接收口令
**参数: pTextBox 文本框指针
**返回值: 功能键
**
*************************************/

long getpassword(TEXTBOX * pTextBox)
{
  long nKey,i,n;
  pTextBox->sValue[0]='\0';
  ReverseChar();
  gotoxy(pTextBox->x,pTextBox->y);
  for(i=0;i<pTextBox->nLen;i++)
    outputchar(' ');
  nKey=GetReversePassword(pTextBox);
  ShowChar();
  gotoxy(pTextBox->x,pTextBox->y);
  n=strlen(pTextBox->sValue);
  for(i=1;i<=pTextBox->nLen;i++)
  {
    if(i<=n)
      outputchar('*');
    else
      outputchar(' ');
  }
  return nKey;
}

/*************************************
**
**函数: GetReversePassword
**目的: 接收口令,以*代替所键入字符,只被getpassword调用
**参数: pTextBox 文本框指针
**返回值: 功能键
**
*************************************/

long GetReversePassword(TEXTBOX * pTextBox)
{
  long i,nFlag=0,nPosition=0,nCurLen,nKey;
  gotoxy(pTextBox->x,pTextBox->y);
  while(!nFlag)
  {
    nKey=keyb();
    if(nKey>=0x30&&nKey<=0x39||nKey>=0x41&&nKey<=0x5A||nKey>=0x61&&nKey<=0x7A||nKey=='_') 
    {
      pTextBox->nFlag=1;
      nCurLen=strlen(pTextBox->sValue);
      for(i=nCurLen;i>nPosition;i--)
        pTextBox->sValue[i]=pTextBox->sValue[i-1];
      pTextBox->sValue[nPosition]=(unsigned char)nKey;
      if(nCurLen<pTextBox->nLen)
        pTextBox->sValue[nCurLen+1]='\0';
      else
        pTextBox->sValue[pTextBox->nLen]='\0';
      for(i=nPosition;i<=nCurLen&&i<pTextBox->nLen;i++)
        outputchar('*');
      nPosition++;
      if(nPosition>=pTextBox->nLen)
        return KEY_RETURN; 
      else
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
      continue;
    }
    switch(nKey)
    {
      case KEY_RETURN:
      case KEY_ESC:
        return nKey;
      case KEY_RIGHT:
        if(pTextBox->sValue[0]=='\0')
          return KEY_RIGHT;
        if(nPosition<=strlen(pTextBox->sValue)-1&&nPosition<pTextBox->nLen-1)
        {
          gotor(1);
          nPosition++;
        }
        else 
          return KEY_RIGHT;
        break;
      case KEY_LEFT:
        if(nPosition>=1)
        {
          gotol(1);
          nPosition--;
        }
        break;
      case KEY_BACKSPACE:
        if (nPosition==0)
          break;
        pTextBox->nFlag=1;
        nCurLen=strlen(pTextBox->sValue);
        for(i=nPosition-1;i<nCurLen-1;i++)
          pTextBox->sValue[i]=pTextBox->sValue[i+1];
        gotoxy(pTextBox->x+nCurLen-1,pTextBox->y);
        outputchar(' ');
        nPosition--;
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
        pTextBox->sValue[nCurLen-1]='\0';
        break;
      case KEY_DELETE:
        nCurLen=strlen(pTextBox->sValue);
        if(nCurLen==0||nPosition>=nCurLen)
          break;
        pTextBox->nFlag=1;
        for(i=nPosition;i<nCurLen-1;i++)
          pTextBox->sValue[i]=pTextBox->sValue[i+1];
        gotoxy(pTextBox->x+nCurLen-1,pTextBox->y);
        outputchar(' ');
        if(nPosition!=0&&nPosition>=nCurLen-1)
          nPosition--;
        gotoxy(pTextBox->x+nPosition,pTextBox->y);
        pTextBox->sValue[nCurLen-1]='\0';
        break;
      default:
        fflush(stdin);
        break;
    }
  }
  return 0;
}

/*************************************
**
**函数: getyesno
**目的: 确认Yes或No
**参数: 无
**返回值: KEY_YES或KEY_NO
**
*************************************/

long getyesno()
{
  long nKey;
  fflush(stdin);
  EnglishInput();
  while(1) 
  {
    nKey=keyb();
    switch(nKey)
    {
      case KEY_RETURN:
      case 'y':
      case 'Y':
        return KEY_YES;
        break;
      case KEY_ESC:
      case 'n':
      case 'N':
        return KEY_NO;
        break;
      default:
        putchar('\a');
        break;
    }
  }
}

#ifdef UNIX

/*************************************
**
**函数: ShowRead
**目的: 设置接收字符模式为立即反应,有回显
**参数: 无
**返回值: 无
**
*************************************/

void ShowRead()
{
  struct termio rTempTerm;
  ioctl(1,TCGETA,&rOldTerm);
  rTempTerm=rOldTerm;
  rTempTerm.c_iflag&=~(ISTRIP|ICRNL|INLCR|BRKINT);
  rTempTerm.c_lflag&=~(ISIG|ICANON); 
  rTempTerm.c_oflag&=~(OCRNL|ONLCR); 
  rTempTerm.c_cflag&=~(PARENB);
  rTempTerm.c_cc[VMIN]=1;
  rTempTerm.c_cc[VTIME]=1;
  ioctl(1,TCSETAF,&rTempTerm);
}

/*************************************
**
**函数: NoShowRead()
**目的: 设置接收字符模式为立即反应,无回显
**参数: 无
**返回值: 无
**
*************************************/

void NoShowRead()
{
  struct termio rTempTerm;
  ioctl(1,TCGETA,&rOldTerm);
  rTempTerm=rOldTerm;
  rTempTerm.c_iflag&=~(ISTRIP|INLCR|BRKINT)|IXON|IXOFF;
  rTempTerm.c_lflag&=~(ISIG|ICANON|ECHO|ECHOE|ECHOK|ECHONL); 
  rTempTerm.c_oflag&=~(OCRNL); 
  rTempTerm.c_cflag&=~(PARENB);
  rTempTerm.c_cc[VMIN]=1;
  rTempTerm.c_cc[VTIME]=1;
  ioctl(1,TCSETAF,&rTempTerm);
}

/*************************************
**
**函数: SetNoWaitRead()
**目的: 接收按键方式设为无论键盘缓冲区有无字符，立即返回, 非阻塞调用
**参数: 无
**返回值: 无
**
*************************************/

void SetNoWaitRead()
{
  struct termio rTempTerm;
  ioctl(1,TCGETA,&rTempTerm);
  rTempTerm.c_cc[VMIN]=0;
  rTempTerm.c_cc[VTIME]=0;
  ioctl(1,TCSETAF,&rTempTerm);
}

/*************************************
**
**函数: SetWaitRead()
**目的: 接收按键方式设为键盘缓冲区有字符时才返回，阻塞调用
**参数: 无
**返回值: 无
**
*************************************/

void SetWaitRead()
{
  struct termio rTempTerm;
  ioctl(1,TCGETA,&rTempTerm);
  rTempTerm.c_cc[VMIN]=1;
  rTempTerm.c_cc[VTIME]=1;
  ioctl(1,TCSETAF,&rTempTerm);
}

/*************************************
**
**函数: EndRead
**目的: 转换回原字符模式
**参数: 无
**返回值: 无
**
*************************************/

void EndRead()
{
  ioctl(1,TCSETAF,&rOldTerm);
}
#endif

/*************************************
**
**函数: ValidDate
**目的: 较验日期是否规范,并在所给月份
**      或之后月份里
**参数: nYear 年
**      nMonth 月
**      nDate 4位月日
**返回值: 0正确 1错误
**
*************************************/

long ValidDate(long nYear,long nMonth,long nDate)
{
  long nLeap;
  long nMonDays[12]={31,28,31,30,31,30,31,31,30,31,30,31};
  long nTempMonth,nTempDay;
  if(nYear%4!=0)
    nLeap=0;
  else
    if(nYear%100!=0)
      nLeap=1;
    else 
      if(nYear%400!=0)
        nLeap=0;
      else
        nLeap=1;
  nMonDays[1]+=nLeap;
  if(nDate<101||nDate>1231)
    return 1;
  nTempMonth=nDate/100;
  nTempDay=nDate%100;
  if(nTempMonth<nMonth||nTempDay==0||nTempDay>nMonDays[nTempMonth-1])
    return 1;
  return 0;
}

/*************************************
**
**函数: FirstDay
**目的: 计算指定月份第一天的月日
**参数: nMonth 月
**返回值: 月日
**
*************************************/

long FirstDay(long nMonth)
{
  return nMonth*100+1;
}

/*************************************
**
**函数: LastDay
**目的: 计算指定年度指定月份月未一天的月日
**参数: nYear 年
**      nMonth 月
**返回值: 月日
**
*************************************/

long LastDay(long nYear,long nMonth)
{
  long nLeap;
  long nMonDays[12]={31,28,31,30,31,30,31,31,30,31,30,31};
  if(nYear%4!=0)
    nLeap=0;
  else
    if(nYear%100!=0)
      nLeap=1;
    else 
      if(nYear%400!=0)
        nLeap=0;
      else
        nLeap=1;
  nMonDays[1]+=nLeap;
  return nMonth*100+nMonDays[nMonth-1];
}

/*************************************
**
**函数: PrevDay
**目的: 计算指定年度指定月份指定日子的前一天
**参数: nDate 年(4位) + 月(2位) + 日(2位)
**返回值: 前一天 年(4位) + 月(2位) + 日(2位)
**        -1 日期错   
*************************************/

long PrevDay(long nDate)
{
  long nLeap;
  long nMonDays[12]={31,28,31,30,31,30,31,31,30,31,30,31};
  long nYear, nMonth, nDay;
  
  nYear = nDate / 10000;
  if(nYear == 0)
    return -1;
  nMonth = nDate % 10000 / 100;
  nDay = nDate % 100;
  if(nYear%4!=0)
    nLeap=0;
  else
    if(nYear%100!=0)
      nLeap=1;
    else 
      if(nYear%400!=0)
        nLeap=0;
      else
        nLeap=1;
  nMonDays[1]+=nLeap;
  if(nMonth > 12 || nMonth == 0)
    return -1;
  if(nDay > nMonDays[nMonth - 1] || nDay == 0)
    return -1;
  if(nDay == 1)
  {
    nMonth --;
    if(nMonth == 0)
    {
        nMonth = 12;
        nYear --;
    }
    nDay = nMonDays[nMonth - 1];
  }
  else
    nDay--;
  return nYear * 10000 + nMonth*100 + nDay;
}

/*************************************
**
**函数: NextDay
**目的: 计算指定年度指定月份指定日子的下一天
**参数: nDate 年(4位) + 月(2位) + 日(2位)
**返回值: 下一天 年(4位) + 月(2位) + 日(2位)
**        -1 日期错   
*************************************/

long NextDay(long nDate)
{
  long nLeap;
  long nMonDays[12]={31,28,31,30,31,30,31,31,30,31,30,31};
  long nYear, nMonth, nDay;
  
  nYear = nDate / 10000;
  if(nYear == 0)
    return -1;
  nMonth = nDate % 10000 / 100;
  nDay = nDate % 100;
  if(nYear%4!=0)
    nLeap=0;
  else
    if(nYear%100!=0)
      nLeap=1;
    else 
      if(nYear%400!=0)
        nLeap=0;
      else
        nLeap=1;
  nMonDays[1]+=nLeap;
  if(nMonth > 12 || nMonth == 0)
    return -1;
  if(nDay > nMonDays[nMonth - 1] || nDay == 0)
    return -1;
  if(nDay == nMonDays[nMonth - 1])
  {
    nDay = 1;
    nMonth++;
    if(nMonth == 13)
    {
        nMonth = 1;
        nYear++;
    }
  }
  else
    nDay++;
  return nYear * 10000 + nMonth*100 + nDay;
}

/*************************************
**
**函数: WeekDay
**目的: 计算指定年月日为星期几
**参数: nYear 年
**      nMonth 月
**      nDay 日
**返回值: 星期,值为1-7
**
*************************************/

long WeekDay(long nYear,long nMonth,long nDay)
{
  long nLeap,nWeek,i;
  long nMonDays[12]={31,28,31,30,31,30,31,31,30,31,30,31};
  if(nYear%4!=0)
    nLeap=0;
  else
    if(nYear%100!=0)
      nLeap=1;
    else 
      if(nYear%400!=0)
        nLeap=0;
      else
        nLeap=1;
  nMonDays[1]+=nLeap;
  nWeek=nYear+nYear/4-nYear/100+nYear/400-1;
  if(nLeap==1)
    nWeek--;
  for(i=0;i<nMonth-1;i++)
    nWeek+=nMonDays[i];
  nWeek+=nDay;
  nWeek=nWeek%7;
  if(nWeek==0)
    nWeek=7;
  return nWeek;
}

/*************************************
**
**函数: FebDays
**目的: 计算二月份天数
**参数: nYear 年度
**返回值: 二月份天数
**
*************************************/

long FebDays(long nYear)
{
  long nLeap;
  long nDays=28;
  //if((nYear%4==0 && nYear%100!=0) || (nYear%400==0) ) nLeap=1;
  //else nLeap=0;
  if(nYear%4!=0)
    nLeap=0;
  else
    if(nYear%100!=0)
      nLeap=1;
    else 
      if(nYear%400!=0)
        nLeap=0;
      else
        nLeap=1;
  nDays+=nLeap;
  return nDays;
}

/*************************************
**
**函数: keyb
**目的: 按收按键
**参数: 无
**返回值: 所按键值
**
*************************************/

long keyb()
{
  long nKey;
  showcursor();
  nKey=getkey(TYPE_CHAR);
  hidecursor(); 
  return nKey;
}

/*************************************
**
**函数: keyn
**目的: 接收数字按键和功能键
**参数: 无
**返回值: 所按键值
**
*************************************/

long keyn()
{
  long nKey;
  showcursor();
  nKey=getkey(TYPE_INT);
  hidecursor(); 
  return nKey;
}

/*************************************
**
**函数: keyf
**目的: 接收数字、小数点和功能键
**参数: 无
**返回值: 所按键值
**
*************************************/

long keyf()
{
  long nKey;
  showcursor();
  nKey=getkey(TYPE_FLOAT);
  hidecursor(); 
  return nKey;
}

/*************************************
**
**函数: wkey
**目的: 按收按键,无光标显示
**参数: 无
**返回值: 所按键值
**
*************************************/

long wkey()
{
  long nKey;
  nKey=getkey(TYPE_CHAR);
  return nKey;
}

long keys()
{
  long nKey;

  gotoxy(20,24);
  outtext("按空格键继续");
  while(1)
  {
    if(keyb() == ' ')
      return 0;
  }
}

/*************************************
**
**函数: waitkey
**目的: 在24行显示"按任意键继续...",等待按键
**参数: 无
**返回值: 无
**
*************************************/

long waitkey()
{
  clearline(24);
  gotoxy(30,24);
  printf("按任意键继续...");
  return keyb();
}

/*************************************
**
**函数: SetPrnDelayTime
**目的: 设置打印机延迟时间
**参数: 打印机延迟时间
**返回值: 无
**
*************************************/

void SetPrnDelayTime(long nDelayTime)
{
  if(nDelayTime>=1)
    nPrnDelayTime=nDelayTime;
}

/*************************************
**
**函数: prnready
**目的: 测试打印机状态
**参数: 无
**返回值: 无
**
*************************************/

#ifdef DOS
long prnready()
{
  union REGS s;
  long i=0;
  for (i=1;i<=nPrnDelayTime;i++)
  {
    s.h.ah=2;
    s.w.dx=0;
    int386(0x17,&s,&s);
    if (s.h.ah==0x90) 
      return 1;
    else
      delay(1);
  }
  return 0;
}
#endif

#ifdef UNIX
long prnready()
{
  long nKey;
  if(nSysMode == UNIX_HOST)
  {
    return 1;
  }
  printf("\033[?15n");
  switch(nKey=keyb())
  {
    case KEY_PRNREADY:
      return 1;
    case KEY_PRNNOTREADY:
    case KEY_PRNNOTEXIST:  
      return 0;
  }
}
#endif

/*************************************
**
**函数: upper
**目的: 把字符串中小写字母变成大写
**参数: 需转换的字符串
**返回值: 无
**
*************************************/

void upper(char s[])
{
  long i=0;
  char ch;
  while((ch=s[i])!='\0')
  {
    if(ch>='a'&&ch<='z')
      s[i]+='A'-'a';
    i++;
  }
}

/*************************************
**
**函数: lower
**目的: 把字符串中大写字母变成小写
**参数: 需转换的字符串
**返回值: 无
**
*************************************/

void lower(char s[])
{
  long i=0;
  char ch;
  while((ch=s[i])!='\0')
  {
    if(ch>='A'&&ch<='Z')
      s[i]+='a'-'A';
    i++;
  }
}

/*************************************
**
**函数: EnglishInput
**目的: 切换到英文输入,仅适用于DOS
**参数: 无
**返回值: 无
**
*************************************/

void EnglishInput()
{
  #if defined(INPUT_WB)
  printf("\016[KB1,0]");
  #elif defined(INPUT_PY)
  printf("\016[KB1,0]");
  #endif
} 

/*************************************
**
**函数: ChineseInput
**目的: 切换到中文输入,仅适用于DOS
**参数: 无
**返回值: 无
**
*************************************/

void ChineseInput()
{
  #if defined(INPUT_WB)
  printf("\016[KB0,5]");
  #elif defined(INPUT_PY)
  printf("\016[KB0,2]");
  #endif
}

/*************************************
**
**函数: clearline2
**目的: 清除多行
**参数: nLine1 清除的起始行
**      nLine2 清除的结止行
**返回值: 无
**
*************************************/

void clearline2(long nLine1,long nLine2)
{
  long i;
  for (i=nLine1;i<=nLine2;i++)
    clearline(i);
}

#ifdef UNIX
/*************************************
**
**函数: delay
**目的: 延迟时间
**参数: nSecond 所需延迟的时间
**返回值: 无
**
*************************************/

void delay(long nSecond)
{
  time_t time1,time2;
  time1=time(0);
  while(1)
  {
    time2=time(0);
    if(time2-time1>=nSecond)
      return;
  }
}
#endif

#ifdef DOS16
void delay(long nSecond)
{
  long a,b,c,d;
  long nSum;
  union REGS s;
  s.h.ah=0x00;
  int386(0x1a,&s,&s);
  a=s.w.cx;
  b=s.w.dx;
  while(1)
  {
    s.h.ah=0x00;
    int386(0x1a,&s,&s);
    c=s.w.cx;
    d=s.w.dx;
    if(c<a)
      nSum=(24+c-a)*65536+d-b+176;
    else
      nSum=(c-a)*65536+d-b;
    if(nSum>nSecond*18.2)
      return;
  }
}
#endif

/*************************************
**
**函数: savewin
**目的: 保存当前画面
**参数: 无
**返回值: 无
**
*************************************/

void savewin()
{
  int i;
/*
  char far *s;
*/

  #ifdef DOS
  char *s;
  #endif

  #ifdef DOS
  if(nTotalWin==MAX_WIN_NUM)
  {  
    outtext("应用程序出错，试图存储超过%d个窗口!",MAX_WIN_NUM);
    keyb();
    return;
  }

/*
  (unsigned long)s=0xb8000000;
*/
  s = (char *)(0xb800<<4);

  memcpy(sSaveBuffer[nTotalWin],s,4000);
  nTotalWin++;
  return;
  #endif

  #ifdef UNIX
  if(!strcmp(sSysSaveDraw,"yes")||!strcmp(sSysSaveDraw,"y"))
    printf("\033!1;1;1;24;80Q");

  if(nTotalWin==MAX_WIN_NUM)
  {  
    outtext("应用程序出错，试图存储超过%d个窗口!",MAX_WIN_NUM);
    keyb();
    return;
  }

  memcpy(sSaveBuffer[nTotalWin],sScreenBuffer,2000);
  nTotalWin++;
  return;
  #endif
}

/*************************************
**
**函数: popwin
**目的: 弹出最后保存的画面并在屏幕上显示
**参数: 无
**返回值: 无
**
*************************************/

void popwin()
{
  #ifdef DOS16
  char far *s;
  #endif

  #ifdef DOS
  char *s;
  #endif


  int i;

  #ifdef DOS
  if(nTotalWin==0)
  {  
    outtext("应用程序出错，试图弹出超过存储个数的窗口!");
    keyb();
    return;
  }
  nTotalWin--;

/*
  (unsigned long)s=0xb8000000;
*/

  s = (char *)(0xb800<<4);

  memcpy(s,sSaveBuffer[nTotalWin],4000);
  return;
  #endif

  #ifdef UNIX
  if(!strcmp(sSysSaveDraw,"yes")||!strcmp(sSysSaveDraw,"y"))
    printf("\033!2Q");

  if(nTotalWin==0)
  {  
    outtext("应用程序出错，试图弹出超过存储个数的窗口!");
    keyb();
    return;
  }
  nTotalWin--;

  memcpy(sScreenBuffer,sSaveBuffer[nTotalWin],2000);
  if(strcmp(sSysSaveDraw,"yes")&&strcmp(sSysSaveDraw,"y"))
  {
    sScreenBuffer[1919]='\0';
    sScreenBuffer[1920]='\0';
    sScreenBuffer[1999]='\0';
    sScreenBuffer[2000]='\0';
    gotoxy(1,1);
    outtext(sScreenBuffer);
  }
  return;
  #endif
}

/*************************************
**
**函数: popwin1
**目的: 弹出最后保存的画面不在屏幕上显示
**参数: 无
**返回值: 无
**
*************************************/

void popwin1()
{
  #ifdef UNIX
  if(!strcmp(sSysSaveDraw,"yes")||!strcmp(sSysSaveDraw,"y"))
    printf("\033!5Q");
  #endif
  if(nTotalWin==0)
  {  
    outtext("应用程序出错，试图弹出超过存储个数的窗口!");
    keyb();
    return;
  }
  nTotalWin--;
}

void clearline(long y)
{
  memset(sScreenBuffer+(y-1)*80,' ',80);
  nCursorX=1;
  if(nCursorY==24)
    nCursorY=1;
  else
    nCursorY++;
  #ifdef UNIX
  if(nSysMode==UNIX_TERM||nSysMode==UNIX_HOST)
  {
    printf("\033[%d;1H\033[K",y);
    return;
  }
  #endif
  gotoxy(1,y);
  putspace(80);
}

void lockkey()
{
  #ifdef UNIX
  if(nSysMode==UNIX_TERM)
    printf("\033[2h\033[?25l");
  #endif
}

void unlockkey()
{
  #ifdef UNIX
  if(nSysMode==UNIX_TERM)
    printf("\033[2l\033[?25h");
  #endif
}

void screento80(void)
{
  #ifdef UNIX
  if(nSysMode==UNIX_TERM)
    printf("\033[?3l");
  #endif
}

void hidecursor()
{
  #ifdef UNIX
  if(nSysMode==UNIX_TERM)
    printf("\033[?25l");
  #endif
}

void showcursor()
{
  #ifdef UNIX
  if(nSysMode==UNIX_TERM)
    printf("\033[?25h");
  #endif
}

void print_normal(long x,long y,char s[])
{
 clearline(y);
 gotoxy(x,y);
 outtext(s);
}

#ifdef UNIX
void PrintLarge(int x,int y,char* s)
{
  printf("\033[%d;%dH\033[13;16m%s\033[%d;%dH\033[14;16m%s\033[0m",y,x,s,y+1,x,s);
} 

void PrintBig(int x,int y,char* s)
{
  printf("\033[%d;%dH\033[13m%s\033[%d;%dH\033[14m%s\033[0m",y,x,s,y+1,x,s);
}

void sparkle(int x,int y,char* s)
{
  printf("\033[%d;%dH\033[13;5m%s\033[%d;%dH\033[14m%s\033[0m",y,x,s,y+1,x,s);
}

void sparklesmall(int x,int y,char* s)
{
  printf("\033[%d;%dH\033[5m%s\033[0m",y,x,s);
}
#endif

#ifdef DOS
void bigfont()
{
  fprintf(stdprn,"\033W1");
  fflush(stdprn);
}

void normalfont()
{
  fprintf(stdprn,"\033W0");
  fflush(stdprn);
}

void eject()
{
  fprintf(stdprn,"\014");
  fflush(stdprn);
}
#endif

void GetSysLkrq()
{
	struct tm now_time;
	time_t now; 
	time(&now);
	now_time=*localtime(&now);
	
	nSysLkrq=(now_time.tm_year+1900)*10000+(now_time.tm_mon+1)*100+now_time.tm_mday;
	nSysSfm=now_time.tm_hour*10000+now_time.tm_min*100+now_time.tm_sec;
	nSysDay=now_time.tm_mday;
	nSysMonth=now_time.tm_mon+1;
	nSysYear=now_time.tm_year+1900;
	nSysSecond=now_time.tm_sec;
	nSysMinute=now_time.tm_min;
	nSysHour=now_time.tm_hour;
	sprintf(sSysLkrq,"%04ld年%02ld月%02ld日",nSysYear,nSysMonth,nSysDay);
	sprintf(sSysSfm,"%02ld时%02ld分%02ld秒",nSysHour,nSysMinute,nSysSecond);

	sprintf(seSysLkrq,"%04ld/%02ld/%02ld",nSysYear,nSysMonth,nSysDay);
	sprintf(seSysSfm,"%02ld:%02ld:%02ld",nSysHour,nSysMinute,nSysSecond);

	sprintf(cSysLkrq,"%ld",nSysLkrq);
	sprintf(cSysSfm,"%ld",nSysSfm);
}


/**************************************************************************
**函数名 FUNCTION NAME: show_hint(long,long,long,char)
**目的          TARGET: 显示提示语句
**参数       ARGUMENTS: kind,row,line,char info 类别,列,行,提示信息
**                      kind    1  某位置后提示
**                             -1  某位置前提示
**                              0  某行置中
**返回值        RETURN:
**              如果正常,返回 0   
**              如果非正常或ESC退出,返回-1   
***************************************************************************/
long show_hint(long kind,long row,long line,char info[])
{
   int length=0;

   if(kind==0)
	clearline(line);
   length=strlen(info);
   if(kind==1)
     gotoxy(row,line);
   else if(kind==-1)
     gotoxy(row-length,line);
   else if(kind==0)
   {
     clearline(line);
     if(length>78)
        gotoxy(1,line);
     else
        gotoxy((80-strlen(info))/2,line);
   }
   else
     ;
   output("%s",info);
   return 0;
}

/**************************************************************************
**函数名 FUNCTION NAME: lline(long kind,long begin ,long number)
**目的          TARGET: 画细线,粗线
**参数       ARGUMENTS: kind    1 细线  2 粗线
**                      begin   空格数  number  线长
**返回值        RETURN:无
***************************************************************************/
long lline(long kind,long begin,long num)
{
   long iLoop;
   for(iLoop=0;iLoop<begin-1;iLoop++)
        output(" ");
   for(iLoop=0;iLoop<num;iLoop++)
   {     
     if(kind==1)
        output("─");
     else
        output("━");
   }
   #ifdef UNIX
   if(num>=40)
     output("\n");
   #endif
   #ifdef DOS
   if(num>=38)
     output("\n");
   #endif
   return 0;
}

long goline(long kind,long row,long line,long num)
{
   long iLoop;

   gotoxy(row,line);
   if(num>40)
     num=40;
   for(iLoop=0;iLoop<num;iLoop++)
   {     
     if(kind==1)
        output("─");
     else
        output("━");
   }
   return 0;
}
/**************************************************************************
**函数名 FUNCTION NAME: print_talbe(long position ,long t0,long t1 .... long t10)
**目的          TARGET: 打印或画出表格线
**参数       ARGUMENTS: position 1 头  2 中间  3 底
**                      t1  第一列
**                      .....
**                      t10 第十列
**返回值        RETURN: 无
***************************************************************************/
void print_table(long position,long t0 ,long t1,long t2,long t3,long t4,long t5,long t6,long t7,long t8 ,long t9, long t10 )
{
/**
position 1  title  2  body  3 bottom
**/
    long i,j,p,flag,num_total=0;
    long num[10];
    
    num[0]=t1/2;
    num[1]=t2/2;
    num[2]=t3/2;
    num[3]=t4/2;
    num[4]=t5/2;
    num[5]=t6/2;
    num[6]=t7/2;
    num[7]=t8/2;
    num[8]=t9/2;
    num[9]=t10/2;
    num_total=(t1+t2+t3+t4+t5+t6+t7+t8+t9+t10)/2;
    for(j=0;j<t0;j++)
       output(" ");
    if(position==1)
        output("┏");
    else if(position==2)
        output("┣");
    else  if(position==3)
        output("┗");
    else
        output("┃");
    for(i=0;i<10;i++)
    {
      if(num[i]<1)
        continue;       
      if(position==2||position==1||position==3)
        lline(2,1,num[i]);
      else
      {
        for(p=0;p<num[i];p++)
          output("%s","  ");
      }  
      if((num_total-num[i])>0)
      {
       if(position==1)
        output("┳");
       else if(position==2)
        output("╋");
       else  if(position==3)
        output("┻");
       else
        output("┃");
       num_total=num_total-num[i];
      }/*end if*/
    }/*end for*/
    if(position==1)
        output("┓\n");
    else if(position==2)
        output("┫\n");
    else  if(position==3)
        output("┛\n");
    else
        output("┃\n");
}

void HiError(long nMySqlVal,char * sMyErrInfo)
{
  char sMySqlErrInfo[MAX_ERR_MSG_LEN+1];

  GetSqlErrInfo(sMySqlErrInfo);
  clearline(23);
  savewin();
  clrscr();
  gotoxy(20,10);
  outtext(ERRMESS1,sMyErrInfo); 
  gotoxy(20,12);
  outtext(ERRMESS2,nMySqlVal);
  gotoxy(20,14);
  outtext(ERRMESS3,sMySqlErrInfo);
  wkey();
  clrscr();
  gotoxy(20,10);
  outtext(ERRMESS4);
  RollbackWork();
  popwin();
} //HiError

void HiErrorNo(long nMySqlVal,char * sMyErrInfo,long nSecond)
{
  char sMySqlErrInfo[MAX_ERR_MSG_LEN+1];
  char sText[401];

  if(nSecond<=1)
    nSecond=1;
  GetSqlErrInfo(sMySqlErrInfo);
  sprintf(sText,"fail: %s--%s",sMyErrInfo,sMySqlErrInfo);
  outtext("\n%s\n",sText); 
  sleep(nSecond);
  RollbackWork(); 
}

/******************************************************************************
**
** 函数: Initstruct_Jym
** 目的: 初始化交易代码结构
** 参数: JYM * pJym 交易代码结构指针
** 返回值: 无
**
******************************************************************************/

void Initstruct_Jym(JYM * pJym)
{
  pJym->cJym[0]='\0';
  pJym->cJymc[0]='\0';
  pJym->cGsmc[0]='\0';
  pJym->cFwm[0]='\0';
  pJym->cSjydz[0]='\0';
  pJym->cJh[0]='\0';
  pJym->cJm[0]='\0';
  pJym->dSflt=0.0;
  pJym->dSxf=0.0;
  pJym->cDljgh[0]='\0';
  pJym->nKzlb=0;
  pJym->nSybz=0;
  pJym->nCxms=0;
  pJym->nJsms=0;
  pJym->nJsjzrq=0;
  pJym->dZkfl=0.0;
  pJym->nLkrq=0;
  pJym->nBdxh=1;
  pJym->nDyszxh=1;
  pJym->nBl1=0;
  pJym->nBl2=0;
  pJym->cBl[0]='\0';
  pJym->dBl=0.0;
  pJym->cLxbz[0]='\0';
  pJym->cBz[0]='\0';
} // Initstruct_Jym

void Initstruct_Jymkzfb(JYMKZFB * pJymkzfb)
{
  pJymkzfb->cJym[0]='\0';
  pJymkzfb->cSjydz[0]='\0';
  pJymkzfb->nBdxh=1;
  pJymkzfb->nJymBdxh=1;
  pJymkzfb->nJhlb=0;
  pJymkzfb->cJh[0]='\0';
  pJymkzfb->nSyzt=1;
  pJymkzfb->nCsbz=1;
  pJymkzfb->dZkfl=0.0;
  pJymkzfb->dSxf=0.0;
  pJymkzfb->nFlag1=0;
  pJymkzfb->nFlag2=0;
  pJymkzfb->cBl1[0]='\0';
  pJymkzfb->cBl2[0]='\0';
  pJymkzfb->nBl=0;
  pJymkzfb->dBl=0.0;
  pJymkzfb->nLkrq=0;
  pJymkzfb->nSfm=0;
  pJymkzfb->cCzydh[0]='\0';
  pJymkzfb->cFresh[0]='\0';
  pJymkzfb->cLxbz[0]='\0';
  pJymkzfb->cBz[0]='\0';
} // Initstruct_Jymkzfb

/******************************************************************************
**
** 函数: CheckYwmx
** 目的: 将c类型的变量附与非空值"."
** 参数: YWMX * pYwmx 业务明细结构指针
** 返回值: 无
**
******************************************************************************/
void CheckYwmx(YWMX *pYwmx)
{
  alltrim(pYwmx->cJym);
  if(strlen(pYwmx->cJym)<1)
   strcpy(pYwmx->cJym,".");

  alltrim(pYwmx->cYhbz);
  if(strlen(pYwmx->cYhbz)<1)
   strcpy(pYwmx->cYhbz,".");

  alltrim(pYwmx->cYhbz1);
  if(strlen(pYwmx->cYhbz1)<1)
   strcpy(pYwmx->cYhbz1,".");

  alltrim(pYwmx->cYhbz2);
  if(strlen(pYwmx->cYhbz2)<1)
   strcpy(pYwmx->cYhbz2,".");

  alltrim(pYwmx->cYhmc);
  if(strlen(pYwmx->cYhmc)<1)
    strcpy(pYwmx->cYhmc,".");

  alltrim(pYwmx->cJscz1);
  if(strlen(pYwmx->cJscz1)<1)
    strcpy(pYwmx->cJscz1,".");

  alltrim(pYwmx->cBl1);
  if(strlen(pYwmx->cBl1)<1)
    strcpy(pYwmx->cBl1,".");

  alltrim(pYwmx->cBl2);
  if(strlen(pYwmx->cBl2)<1)
    strcpy(pYwmx->cBl2,".");

  alltrim(pYwmx->cBl3);
  if(strlen(pYwmx->cBl3)<1)
    strcpy(pYwmx->cBl3,".");

  alltrim(pYwmx->cBl4);
  if(strlen(pYwmx->cBl4)<1)
    strcpy(pYwmx->cBl4,".");

  alltrim(pYwmx->cBl5);
  if(strlen(pYwmx->cBl5)<1)
    strcpy(pYwmx->cBl5,".");

  alltrim(pYwmx->cBl6);
  if(strlen(pYwmx->cBl6)<1)
    strcpy(pYwmx->cBl6,".");

  alltrim(pYwmx->cBl7);
  if(strlen(pYwmx->cBl7)<1)
    strcpy(pYwmx->cBl7,".");

  alltrim(pYwmx->cJh);
  if(strlen(pYwmx->cJh)<1)
   strcpy(pYwmx->cJh,".");


  alltrim(pYwmx->cCzydh);
  if(strlen(pYwmx->cCzydh)<1)
    strcpy(pYwmx->cCzydh,".");

  alltrim(pYwmx->cLxbz);
  if(strlen(pYwmx->cLxbz)<1)
    strcpy(pYwmx->cLxbz,".");

  alltrim(pYwmx->cBz);
  if(strlen(pYwmx->cBz)<1)
    strcpy(pYwmx->cBz,".");
}//CheckYwmx

/******************************************************************************
**
** 函数: Initstruct_Ywmx
** 目的: 初始化业务明细结构
** 参数: YWMX * pYwmx 业务明细结构指针
** 返回值: 无
**
******************************************************************************/

void Initstruct_Ywmx(YWMX * pYwmx)
{
  long nLoop; // 循环计数器

  pYwmx->cJym[0]='\0';
  pYwmx->nYsnd=0;
  pYwmx->nYsyf=0;
  pYwmx->cYhbz[0]='\0';
  pYwmx->cYhbz1[0]='\0';
  pYwmx->cYhbz2[0]='\0';
  pYwmx->cYhmc[0]='\0';
  pYwmx->nYwlb=0;
  pYwmx->nXh=0;
  pYwmx->cJscz1[0]='\0';
  pYwmx->nJscz2=0;
  pYwmx->dJscz3=0.0;
  pYwmx->dJscz4=0.0;
  pYwmx->nSl=0;
  pYwmx->dYjzk=0.0;
  pYwmx->dXzyjk=0.0;
  pYwmx->dSjk=0.0;
  pYwmx->dWjk=0.0;
  pYwmx->dDlsxf=0.0;
  pYwmx->nYfkfs=0;
  pYwmx->nSfkfs=0;
  pYwmx->nBl1=0;
  pYwmx->nBl2=0;
  pYwmx->dBl1=0.0;
  pYwmx->dBl2=0.0;
  pYwmx->cBl1[0]='\0';
  pYwmx->cBl2[0]='\0';
  pYwmx->cJh[0]='\0';
  pYwmx->cCzydh[0]='\0';
  pYwmx->nLkrq=0;
  pYwmx->nSfm=0;
  pYwmx->cLxbz[0]='\0';
  pYwmx->nXzbz=0;
  pYwmx->nCsbz=0;
  pYwmx->cBz[0]='\0';
  pYwmx->cBl3[0]='\0';
  pYwmx->cBl4[0]='\0';
  pYwmx->cBl5[0]='\0';
  pYwmx->cBl6[0]='\0';
  pYwmx->cBl7[0]='\0';

  for (nLoop=0;nLoop<=YWMX_SFMX_ELEMENT_LEN;nLoop++)
  {
    pYwmx->dSfmx[nLoop]=0.0;
  } // end for
} // Initstruct_Ywmx



/******************************************************************************
**
** 函数: Initstruct_Ywmxgl
** 目的: 初始化业务明细管理结构
** 参数: YWMXGL * pYwmxgl 业务明细管理结构指针
** 返回值: 无
**
******************************************************************************/

void Initstruct_Ywmxgl(YWMXGL * pYwmxgl)
{
  pYwmxgl->nXh=0;
  pYwmxgl->cJym[0]='\0';
  pYwmxgl->nLb=0;
  pYwmxgl->cZdmc[0]='\0';
  pYwmxgl->cYzdhy[0]='\0';
  pYwmxgl->cXzdhy[0]='\0';
  pYwmxgl->nSybz=0;
  pYwmxgl->cSfmx[0]='\0';
  pYwmxgl->nBl=0;
  pYwmxgl->dBl=0.0;
  pYwmxgl->cBl[0]='\0';
//  pYwmxgl->cBz[0]='\0';
  pYwmxgl->cLxbz[0]='\0';
} // Initstruct_Ywmxgl



/******************************************************************************
**
** 函数: Initstruct_Zwmx
** 目的: 初始化帐务明细管理结构
** 参数: ZWMX * pZwmx 帐务明细管理结构指针
** 返回值: 无
**
******************************************************************************/

void Initstruct_Zwmx(ZWMX * pZwmx)
{
  pZwmx->cJym[0]='\0';
  pZwmx->nYsnd=0;
  pZwmx->nYsyf=0;
  pZwmx->cYhbz[0]='\0';
  pZwmx->cYhbz1[0]='\0';
  pZwmx->cYhbz2[0]='\0';
  pZwmx->cYhmc[0]='\0';
  pZwmx->nYwlb=0;
  pZwmx->nXh=0;
  pZwmx->nSl=0;
  pZwmx->dYjzk=0.0;
  pZwmx->dXzyjk=0.0;
  pZwmx->dSjk=0.0;
  pZwmx->dDlsxf=0.0;
  pZwmx->dYbk=0.0;
  pZwmx->dYtk=0.0;
  pZwmx->dSjkxj=0.0;
  pZwmx->dSjkcz=0.0;
  pZwmx->dSjkxyk=0.0;
  pZwmx->nSfkfs=0;
  pZwmx->cJkdh[0]='\0';
  pZwmx->cJh[0]='\0';
  pZwmx->cCzydh[0]='\0';
  pZwmx->nLkrq=0;
  pZwmx->nSfm=0;
  pZwmx->cLxbz[0]='\0';
  pZwmx->nXzbz=0;
  pZwmx->nCsbz=0;
  pZwmx->nCscs=0;
  pZwmx->nJsbz=0;
  pZwmx->cWbjsdh[0]='\0';
  pZwmx->cNbjsdh[0]='\0';
  pZwmx->cBl[0]='\0';
  pZwmx->cBl1[0]='\0';
  pZwmx->cBl2[0]='\0';
  pZwmx->cBl3[0]='\0';
  pZwmx->cBl4[0]='\0';
  pZwmx->cBl5[0]='\0';
  pZwmx->nBl=0;
} // Initstruct_Zwmx

void dbShowError(long nMySqlVal,char * sMyErrInfo)
{
  char sMySqlErrInfo[MAX_ERR_MSG_LEN+1];

  bell();
  GetSqlErrInfo(sMySqlErrInfo);
  savewin();
  clrscr();
  gotoxy(20,10);
  outtext(ERRMESS1,sMyErrInfo); 
  gotoxy(20,12);
  outtext(ERRMESS2,nMySqlVal);
  gotoxy(20,14);
  outtext(ERRMESS3,sMySqlErrInfo);
  wkey();
  clrscr();
  gotoxy(20,10);
  outtext(ERRMESS4);
  RollbackWork();
  popwin();
} // dbShowError

void dbShowTaskError(char * sMyTaskErrInfo)
{
  bell();

  savewin();
  clrscr();
  gotoxy(20,10);
  outtext(ERRMESS1,sMyTaskErrInfo);
  wkey();
  clrscr();
  gotoxy(20,10);
  outtext(ERRMESS4);
  RollbackWork();
  popwin();
} // dbShowTaskError

void dbInterrupt(void)
{
  savewin();
  clrscr();
  gotoxy(20,10);
  outtext(ERRMESS4);
  RollbackWork();
  popwin();
} // dbInterrupt


/**************************************************************************
**函数名 FUNCTION NAME: print_ready()
**目的          TARGET: 测试打印机是否准备好
**参数       ARGUMENTS: 
**返回值        RETURN: 
**              如果正常,返回 0
**              如果非正常或ESC退出,返回-1   
***************************************************************************/
long print_ready()
{
        long     nKey;

        for(;;)
        {
                if(prnready()!=1)
                {
                        clearline2(23,24);
                        show_hint(1,20,23,"打印机未准备好!(<ESC> 返回,<ENTER> 继续打印");
                        nKey=keyb();
                        clearline(23);
                        if(nKey==KEY_ESC)
                        {
                                clearline(23);
                                return -1;
                        }
                }
                else
                        return 0;
        }
}

/**************************************************************************
**函数名 FUNCTION NAME: begin_search(long print_flag)
**目的          TARGET: 开始查询或打印
**参数       ARGUMENTS: print_flag 打印标志
**                      1 输出到屏幕
**                      2 到打印机
**                      3 到文本文件
**返回值        RETURN:
**              如果正常,返回 0   
**              如果非正常或ESC退出,返回-1   
***************************************************************************/

long begin_search(long print_flag)
{
    long weekday,hour,nId;

    GetSysLkrq();
    nId=RunSelect("select to_number(to_char(sysdate,'D')),to_number(to_char(sysdate,'HH24')) from dual into %ld%ld",&weekday,&hour);
    clearline(24);

    if(hour>=20&&hour<21)
       show_hint(0,1,24,MESSAGE601);
    else if(hour<3||hour>=22)
       show_hint(0,1,24,MESSAGE602);
    else if(weekday==7&&hour<12)   
       show_hint(0,1,24,MESSAGE603);
    else if(weekday==0)   
       show_hint(0,1,24,MESSAGE604);
    else if (weekday==6 && hour>16)
       show_hint(0,1,24,MESSAGE605);
    else
       ;
    if(print_flag==1)
    {
       clearline2(23,24);
       show_hint(0,20,23,MESSAGE011);
    }
    else  if (print_flag==2)
    {
    //   if(print_ready()==-1)
    //       return -1;
       clearline2(23,24);
      show_hint(0,20,23,MESSAGE012);
       setprinton();
    }
    else
    {
       clearline2(23,24);
       show_hint(0,20,23,MESSAGE013);
       outtext("\n\n");
       if(openfile(sfilename)==1)
       {
          clearline2(23,24);
          show_hint(1,20,23,MESSAGE014);
          return -1;
       }
    }
    return 0;
}      

void close_search(long print_flag)
{
    if (print_flag==2)
    {
       eject();
       setprintoff();
       clrscr();
       show_hint(0,20,23,"打印完毕! 按任意键返回  ");
       wkey();
    }
    else if(print_flag==3)
    {
       closefile();
       clrscr();
       show_hint(0,20,23,"输出到文本文件完毕! 按任意键返回  ");
       wkey();
    }
    else
    {
       clearline2(23,24);
       show_hint(0,20,23,"查阅完毕! 按任意键返回  ");
       wkey();
    }
}      
/**************************************************************************
**函数名 FUNCTION NAME: get_filename(long row,long line)
**目的          TARGET: 取文本文件文件名
**参数       ARGUMENTS: LINE 行  ROW 列
**返回值        RETURN:
**              如果正常,返回 0   
**              如果非正常或ESC退出,返回-1   
***************************************************************************/
Get_FileName(long row,long line) 
{
        int nKey;
        TEXTBOX sub;

        clearline(line);
        clearline(23);
        show_hint(-1,row,line,MESSAGE014);
        gotoxy(row+20,line);
        outtext("%s","(默认名--temp.txt)");
        do{
                sub.x=row;
                sub.y=line;
                sub.sValue[0]='\0';
                sub.nFlag=0;
                sub.nLen=20;
                nKey=getstring(&sub);
                alltrim(sub.sValue);
        }while(nKey!=KEY_ESC&&nKey!=KEY_RETURN);
        if(nKey==KEY_ESC)
                return -1;
        if(sub.nFlag==0&&nKey==KEY_RETURN)
        {
                sprintf(sfilename,"%s","temp.txt");
        }
        else 
        {
                sprintf(sfilename,"%s",sub.sValue);
        }
        gotoxy(row,line);
        output("%s",sfilename);
        return 0;
}

void LimPrecision(double * pVar)
{
  double dVar1,
         dVar2;
  
  *pVar*=100;

  if (*pVar>=0.0)
  {
    *pVar+=0.5;
  }
  else
  {
    *pVar-=0.5;
  } // end if

  dVar2=modf(*pVar,&dVar1);

  *pVar=dVar1/100;
} // LimPrecision

void  sGetFile(long  nLb)
 { 
   char sJh[10];
   GetSysLkrq();
   if(!strcmp(sGetJh,"100000001"))
   strcpy(sGetJh,sSysJsdh);
   else
   strcpy(sGetJh,sGetJh);
   strncpy(sJh,sGetJh+2,7);
   alltrim(sJh);
  #ifdef UNIX
  sprintf(sFileName,"tmp/%ld%s%ld%ld%ld%s",nLb,sJh,nSysHour,nSysMinute,nSysSecond,sSysCzydh);
  #endif
  #ifdef DOS
  sprintf(sFileName,"1%ld%s",nSysSecond,sSysCzydh);
  #endif
  alltrim(sFileName);
} 
void nBell(long nNum)
{
   long iLoop,nCount=0,nKey;
   if(nNum==0)
   {
       SetNoWaitRead();
       while(1)
	{
	   nCount++;
	   bell();
	   nKey=keyb();
           if(nKey==KEY_RETURN)
             break;
           if(nCount==1000000)
		break;
	}
        SetWaitRead();
   }
   else
   {
    for(iLoop=0;iLoop<nNum;iLoop++)
        bell();
    sleep(1);
    for(iLoop=0;iLoop<nNum;iLoop++)
        bell();
    sleep(1);
    for(iLoop=0;iLoop<nNum;iLoop++)
        bell();
   }
   return;
}


void Clear_Space(long row1,long line1,long row2,long line2)
{
   long iLoop1=0,iLoop2=0;
   long i1=0,i2=0;
   long mixrow=0,mixline=0;
   
   mixrow=row1;
   mixline=line1;

   i1=labs(row2-row1);
   i2=labs(line2-line1);

   if(row1>row2)
      mixrow=row2;
   if(line1>line2)
      mixline=line2;

    for(iLoop1=0;iLoop1<i2+1;iLoop1++,mixline++)
    {
       gotoxy(mixrow,mixline);
       for(iLoop2=0;iLoop2<i1+1;iLoop2++)
          outtext("%s"," ");
    }
}

/********
参数说明：
  入口参数：
    sourdate:以整型表示8位日期，是基准日期点
    numberday:间隔天数；
    flag:以基准日期为准，向前向后确定目标日期，-1表示比基准日期小（历史日期），
        1是表示未来日期；

**********/

long GetRandomDate(long sourdate,long numberday,long flag)
{
	char sdescdate[10];
	long id=0;
	
	sdescdate[0]='\0';
	
	id=RunSelect("select to_char(to_date(to_char(%ld),'yyyymmdd')+%ld*%ld,'yyyymmdd') "
					"from dual into %s",sourdate,numberday,flag,sdescdate);
	alltrim(sdescdate);
	
	if( id<=0)  return -1;
	return atol(sdescdate);
}	   

