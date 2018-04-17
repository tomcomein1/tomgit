// 请同时更改bkfx.h和netmain.h里的操作系统定义

// DOS下编译
//#define DOS

// SCO UNIX下编译
#ifndef UNIX
//#define UNIX
#endif

// make文件中已定义 HP UNIX下编译
#ifndef HPUNIX
//#define HPUNIX
#endif

// make文件中已定义 IBM UNIX下编译
#ifndef IBMUNIX
#define IBMUNIX
#endif

#if defined(HPUNIX) || defined(IBMUNIX)
#define UNIX
// 长型个位字节在最后
#define LITTLEEND
#endif

#if defined(DOS) && defined(UNIX)
#error Both DOS and UNIX have been defined.
#endif

#if !defined(DOS) && !defined(UNIX)
#error Neither DOS nor UNIX has been defined.
#endif

/*bkfxd*/
#define DATASERVER

#include "sql.h"

#define TYPE_INT 1
#define TYPE_LONG 2
#define TYPE_SHORT 3
#define TYPE_FLOAT 4
#define TYPE_DOUBLE 5
#define TYPE_LONGDOUBLE 6
#define TYPE_CHAR 11
#define TYPE_STRING 12
#define TYPE_TABLE 13
#define TYPE_NULL -1


#define TERM_VT100 1
#define TERM_VT220 2
#define TERM_ANSI 3

#define OUTPUT_SCREEN 1
#define OUTPUT_PRINTER 2
#define OUTPUT_TEXT 3

#define UNIX_HOST 1
#define UNIX_TERM 2

#define MAX_WIN_NUM 5 /*最大保存的画面数*/

typedef struct 
{
  long x;
  long y;
  long nLen;
  char sValue[201];
  char sOldValue[201]; 
  long nFlag;
}TEXTBOX;

#define KEY_RETURN 13
#define KEY_F1 301
#define KEY_F2 302
#define KEY_F3 303
#define KEY_F4 304
#define KEY_F5 305
#define KEY_F6 306
#define KEY_F7 307
#define KEY_F8 308
#define KEY_F9 309
#define KEY_F10 310
#define KEY_F11 311
#define KEY_F12 312
#define KEY_ESC 320
#define KEY_INSERT 321
#define KEY_DELETE 322
#define KEY_PAGEUP 325
#define KEY_PAGEDOWN 326
#define KEY_HOME 323
#define KEY_END 324 
#define KEY_UP 327
#define KEY_DOWN 328
#define KEY_LEFT 329
#define KEY_RIGHT 330
#define KEY_NUMLOCK 331
#define KEY_YES 1
#define KEY_NO 2
#define KEY_ALL 3
#define KEY_BACKSPACE 8
#define KEY_TAB 9

#define KEY_PRNREADY 351
#define KEY_PRNNOTREADY 352
#define KEY_PRNNOTEXIST 353

void EnglishInput();
void ChineseInput();

void ShowChar(void);
void ReverseChar(void);
void clrscr(void);
void getxy(long *x,long *y);
void gotoxy(long x,long y);
void gotol(long x);
void gotor(long x);
void gotou(long y);
void gotod(long y);
void savewin(void);
void popwin(void);
void popwin1(void);

void setprinton();
void setprintoff();
void outputchar(long ch);
long keyb(void);
long keyn(void);
long keyf(void);
long waitkey(void);
long wkey(void);
void rtrim(char* s);
void alltrim(char* s);
void ReverseXY(TEXTBOX * pText);
void ShowXY(TEXTBOX * pText);
void clearline(long y);

#define putspace(n)   outtext("%*c",n,' ')
#define bell()        putchar('\a')

long output(char *sFormat,...);
long outtext(char *sFormat,...);
void DelEscChar(char *s);
long WeekDay(long nYear,long nMonth,long nDay);

long GetReverseNumber(TEXTBOX * pTextBox);
long GetReverseFloat(TEXTBOX * pTextBox);
long GetReverseString(TEXTBOX * pTextBox);
long GetReversePassword(TEXTBOX * pTextBox);
long GetKeyUsingVt100(long nDataType);
long GetKeyUsingVt220(long nDataType);
long GetKeyUsingAnsi(long nDataType);
void clearrecord(long nCol,long nRow,long nFieldLen[],long nFieldNum);
void getposition(long nCol,long nFieldLen[],long nPosition[],long nFieldNum);
void drawbox(long nCol1,long nRow1,long nCol2,long nRow2);
void drawtable(long nCol,long nRow,long nFieldLen[],long nFieldNum,long nRowCount);
long openfile(char * sFileName);
long closefile(void);
/*
long OutPutWithHandle(long nHandle,char *sFormat,...);
*/
long FirstDay(long nMonth);
long LastDay(long nYear,long nMonth);
void print_normal(long x,long y,char s[]);

long getstring(TEXTBOX * pTextBox);
long getnumber(TEXTBOX * pTextBox);
long getfloat(TEXTBOX * pTextBox);
void lower(char s[]);
void upper(char s[]);

#ifdef DOS
#define hidechar()    printf("\033[30;40m")
void bigfont(void);
void normalfont(void);
void eject(void);
#define PrintLarge(x,y,s) printf("\033[%d;1H\016[{@d(7)0-%d|%d%s}]",y,(x-1)*8,(y-1)*19,s)
#define PrintBig(x,y,s) printf("\033[%d;1H\016[{@A(7)0-%d|%d%s}]",y,(x-1)*8,(y-1)*19,s)
long GetKeyUnderDos(long nDataType);
void FillAttr(long ch,long n,long attr);
#endif

#ifdef UNIX
#define eject()       putchar('\014')
#define hidechar()    printf("\033[6m")
#define bigfont()     printf("\033W1")
#define normalfont()  printf("\033W0")
#define ShowDraw(n)  printf("\033!C%da",n)
#define BeginDraw(n) printf("\033!K%da",n)
#define EndDraw()    printf("\033!E")

void lockkey(void); 
void unlockkey(void); 
void screento80(void); 
void hidecursor(void);
void showcursor(void);
void ShowRead(void);
void NoShowRead(void);
void EndRead(void);
void PrintLarge(int x,int y,char* s);
void PrintBig(int x,int y,char* s);
void sparkle(int x,int y,char* s);
void delay(long nSecond);
#endif

#define ERR_NO_SELECT 32766
#define ERR_NO_TCPIP 32767
#define ERR_NO_FETCH 32765
