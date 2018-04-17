#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include "bkfxd.h"

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
long nTermType=TERM_VT220;
FILE *fHostPrn;
#endif

long nOutPutHandle=OUTPUT_SCREEN;
FILE *fOutPutHandle;
long nPrnDelayTime=15;
char sScreenBuffer[2001];
char sSaveBuffer[5][4001];
long nTotalWin=0;/*�ѱ���Ļ�����*/
long nCursorX=1,nCursorY=1;
char sSysSaveDraw[4];/*yesʹ�ô洢����,no ��ʹ�ô洢����*/

#ifdef DOS
long nCursorAttr=7;
#endif

long nSysMode=UNIX_TERM;

/*************************************
**
**����: ShowChar
**Ŀ��: ����Ϊ�ַ�������ʾ��ʽ
**����: ��
**����ֵ: ��
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

/*************************************
**
**����: ReverseChar
**Ŀ��: ����Ϊ�ַ����Է�ʽ
**����: ��
**����ֵ: ��
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
**����: FillAttr
**Ŀ��: ����ַ�����Ļ,ֻ����outputchar
**����: ch ����ַ�
        n ����ַ�����
        attr ��ɫ����
**����ֵ: ��
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
**����: clrscr
**Ŀ��: ����
**����: ��
**����ֵ: ��
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
**����: getxy
**Ŀ��: ȡ���λ��
**����: x ������
**      y ������
**����ֵ: ��
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
**����: gotoxy
**Ŀ��: ��λ���
**����: xΪ������,yΪ������
**����ֵ: ��
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
**����: gotol
**Ŀ��: ���������һ�л����
**����: x ��������
**����ֵ: ��
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
**����: gotor
**Ŀ��: ���������һ�л����
**����: x ��������
**����ֵ: ��
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
**����: gotou
**Ŀ��: ���������һ�л����
**����: y ��������
**����ֵ: ��
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
**����: gotod
**Ŀ��: ���������һ�л����
**����: y ��������
**����ֵ: ��
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
**����: outputchar
**Ŀ��: ����Ļ���һ�ַ�
**����: ch ��Ҫ������ַ�
**����ֵ: ��
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
**����: output
**Ŀ��: ��ʽ���������Ļ���ӡ�����ı�,���б䳤�б�
**      ִ��setprinton()�������ӡ��
**      ִ��openfile()������ı�
**����: sFormat �����ʽ,ͬprintf�������ʽ 
**����ֵ: ʵ��������ַ���
**
*************************************/

long output(char *sFormat,...)
{
  char sBuffer[2001];
  unsigned char ch;
  long i;
  long nCharCount;/*����ʵ����� ���ֽ���*/
  va_list pVarArgList;/*�䳤�����б�ָ��*/

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
**����: outtext
**Ŀ��: ��ʽ���������Ļ,���б䳤�б�
**����: sFormat �����ʽ,ͬprintf�������ʽ 
**����ֵ: ʵ��������ַ���
**
*************************************/

long outtext(char *sFormat,...)
{
  char sBuffer[2001];
  unsigned char ch;
  long i;
  long nCharCount;/*����ʵ����� ���ֽ���*/
  va_list pVarArgList;/*�䳤�����б�ָ��*/

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
**����: OutPutWithHandle
**Ŀ��: ��ʽ�����,���б䳤�б�
**����: nHandle �������
**      sFormat �����ʽ,ͬprintf�������ʽ 
**����ֵ: ʵ��������ַ���
**
*************************************/

long OutPutWithHandle(long nHandle,char *sFormat,...)
{
  long nErrorFlag=0,/*�﷨�����־,0Ϊfalse,1Ϊtrue*/
      nPrecision,/*����Ϊ*��*/ 
      nPrecisionFlag,/*����Ϊ*�ű�־,0ΪFalse,1Ϊtrue*/
      nCharCount,/*����ʵ����� ���ֽ���*/
      nType;/*�䳤�б��в���������*/
  char* sTop;/*�����ʽͷָ��*/
  char* sBottom;/*�����ʽβָ��*/
  char* sTempFormat;/*��ʱ��ʽ*/
  char* sTemp;/*��ʱ���*/
  char* sOut;/*����ַ���*/
  va_list pVarArgList;/*�䳤�����б�ָ��*/

  sTop=sFormat;
  sBottom=sFormat;
  if((sTempFormat=(char*)malloc(sizeof(sFormat)))==NULL)
  {
    fprintf(stderr,"\n\a�ڴ治��\n");
    keyb();
    return 0;
  }
  if((sTemp=(char*)malloc(2000))==NULL)
  {
    fprintf(stderr,"\n\a�ڴ治��\n");
    free(sTempFormat);
    keyb();
    return 0;
  }
  if((sOut=(char*)malloc(2000))==NULL)
  {
    fprintf(stderr,"\n\a�ڴ治��\n");
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
                fprintf(stderr,"\n\a�﷨����\n");
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
                fprintf(stdout,"\n\a�﷨����\n");
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
                fprintf(stdout,"\n\a�﷨����\n");
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
**����: openfile
**Ŀ��: ָ�������һ���ļ�
**����: sFileName �ļ���
**����ֵ: 0 �ɹ� 1 ����
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
**����: closefile
**Ŀ��: ֹͣ������ļ�
**����: ��
**����ֵ: 0 �ɹ� 1 ����
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
**����: alltrim 
**Ŀ��: ɾ���ַ������ҿո� 
**����: �ַ���ָ�� 
**����ֵ: ��
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
**����: rtrim 
**Ŀ��: ɾ���ַ����ҿո� 
**����: �ַ���ָ�� 
**����ֵ: ��
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
**����: getkey
**Ŀ��: ���ݽ����ַ����ͽ��հ���
**����: nDataType �����ַ�����
**����ֵ: ���ذ���
**
*************************************/

long getkey(long nDataType)
{
  long nKey;
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

/*************************************
**
**����: GetKeyUsingAnsi
**Ŀ��: ��Ansi�¸��ݽ����ַ����ͽ��հ���,ֻ��getkey����
**����: nDataType �����ַ�����
**����ֵ: ���ذ���
**
*************************************/

long GetKeyUsingAnsi(long nDataType)
{
  long i;
  unsigned char s[3];
  fflush(stdin);
  s[0]=s[1]=s[2]='\0';
  read(0,(void *)s,2);
  if (nDataType==TYPE_CHAR&&(s[0]>=0x20&&s[0]<=0x7e||s[0]>=0xa1&&s[0]<=0xfe))
  { 
    if(s[1])
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
  if (s[0]=='\n')
  {
    if(s[1])
      ungetc((int)s[1],stdin);
    return KEY_RETURN;
  }
  if (s[0]==8)
    return KEY_BACKSPACE;
  if (s[0]==9)
    return KEY_TAB;
  if (s[0]==0x1b&&s[1]==0)
    return KEY_ESC;
  if (s[0]==0x1b&&s[1]=='O')  
  {
    read(0,(void *)s,1);
    switch(s[0])   
    {
      case 'P':
        return KEY_F1;
      case 'Q':
        return KEY_F2;
      case 'R':
        return KEY_F3;
      case 'S':
        return KEY_F4;
      case 'A':
        return KEY_UP;
      case 'B':
        return KEY_DOWN;
      case 'C':
        return KEY_RIGHT;
      case 'D':
        return KEY_LEFT;
    }
    fflush(stdin);
  }
}

/*************************************
**
**����: GetKeyUsingVt100
**Ŀ��: ��VT100�¸��ݽ����ַ����ͽ��հ���,ֻ��getkey����
**����: nDataType �����ַ�����
**����ֵ: ���ذ���
**
*************************************/

long GetKeyUsingVt100(long nDataType)
{
  long i;
  int ch;
  unsigned char s[3];
  fflush(stdin);
  s[0]=s[1]=s[2]='\0';
  read(0,(void *)s,2);
  if (nDataType==TYPE_CHAR&&(s[0]>=0x20&&s[0]<=0x7e||s[0]>=0xa1&&s[0]<=0xfe))
  { 
    if(s[1])
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
  if (s[0]=='\n')
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
**����: GetKeyUsingVt220
**Ŀ��: ��VT220�¸��ݽ����ַ����ͽ��հ���,ֻ��getkey����
**����: nDataType �����ַ�����
**����ֵ: ���ذ���
**
*************************************/

long GetKeyUsingVt220(long nDataType)
{
  int ch;
  while(1)
  {
    ch=getchar();
    if (nDataType==TYPE_CHAR&&(ch>=0x20&&ch<=0x7e||ch>=0xa1&&ch<=0xfe))
      return ch;
    if (nDataType==TYPE_INT&&(ch>='0'&&ch<='9'||ch=='-'))
      return ch;
    if (nDataType==TYPE_FLOAT&&(ch>='0'&&ch<='9'||ch=='.'||ch=='-'))
      return ch;
    if (ch=='\n')
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
      }
    }
    fflush(stdin);
  }
}
#endif

#ifdef DOS

/*************************************
**
**����: GetKeyUnderDos
**Ŀ��: ��Dos�¸��ݽ����ַ����ͽ��հ���,ֻ��getkey����
**����: nDataType �����ַ�����
**����ֵ: ���ذ���
**
*************************************/

long GetKeyUnderDos(long nDataType)
{
  int ch;
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
    if (ch==KEY_RETURN)
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
**����: ReverseXY
**Ŀ��: ����Ļ�Ϸ���һ�ַ���,������
**����: pTextBox �ı���ָ��
**����ֵ: ��
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
**����: ShowXY
**Ŀ��: ����Ļ��������ʾһ�ַ���,������
**����: pTextBox �ı���ָ��
**����ֵ: ��
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
**����: setprinton
**Ŀ��: ���ָ���ӡ��
**����: ��
**����ֵ: ��
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
  normalfont();
}

/*************************************
**
**����: setprintoff
**Ŀ��: ֹͣ���ӡ�����
**����:  ��
**����ֵ: ��
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
**����: getnumber
**Ŀ��: ���������ַ���
**����: pTextBox �ı���ָ��
**����ֵ: ���ܼ�
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
**����: GetReverseNumber
**Ŀ��: ���ո����ַ���,ֻ��getnumber����
**����: pTextBox �ı���ָ��
**����ֵ: ���ܼ�
**
*************************************/

long GetReverseNumber(TEXTBOX * pTextBox)
{
  long i,nFlag=0,nPosition=0,nKey,nCurLen;
  strcpy(pTextBox->sOldValue,pTextBox->sValue);
  pTextBox->nFlag=0;
  ReverseXY(pTextBox);
  gotoxy(pTextBox->x,pTextBox->y);
  while(!nFlag)
  {
    nKey=keyn();
    if (nKey>='0'&&nKey<='9'||nKey=='-'&&nPosition==0&&!strchr(pTextBox->sValue,'-'))
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
**����: getfloat
**Ŀ��: ���ո����ַ���
**����: pTextBox �ı���ָ��
**����ֵ: ���ܼ�
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
**����: GetReverseFloat
**Ŀ��: ���ո����ַ���,ֻ��getfloat����
**����: pTextBox �ı���ָ��
**����ֵ: ���ܼ�
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
**����: getstring
**Ŀ��: �����ַ���
**����: pTextBox �ı���ָ��
**����ֵ: ���ܼ�
**
*************************************/

long getstring(TEXTBOX * pTextBox)
{
  long nKey;
  nKey=GetReverseString(pTextBox);
  ShowXY(pTextBox);
  return nKey;
}

/*************************************
**
**����: GetReverseString
**Ŀ��: �����ַ���,ֻ��getstring����
**����: pTextBox �ı���ָ��
**����ֵ: ���ܼ�
**
*************************************/

long GetReverseString(TEXTBOX * pTextBox)
{
  long i,nFlag=0,nPosition=0,nCurLen,nKey;
  ReverseXY(pTextBox);
  gotoxy(pTextBox->x,pTextBox->y);
  while(!nFlag)
  {
    nKey=keyb();
    if(nKey>=0x20&&nKey<=0xfe) 
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
**����: getpassword
**Ŀ��: ���տ���
**����: pTextBox �ı���ָ��
**����ֵ: ���ܼ�
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
**����: GetReversePassword
**Ŀ��: ���տ���,��*�����������ַ�,ֻ��getpassword����
**����: pTextBox �ı���ָ��
**����ֵ: ���ܼ�
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
**����: getyesno
**Ŀ��: ȷ��Yes��No
**����: ��
**����ֵ: KEY_YES��KEY_NO
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
**����: ShowRead
**Ŀ��: ���ý����ַ�ģʽΪ������Ӧ,�л���
**����: ��
**����ֵ: ��
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
**����: NoShowRead()
**Ŀ��: ���ý����ַ�ģʽΪ������Ӧ,�޻���
**����: ��
**����ֵ: ��
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
**����: EndRead
**Ŀ��: ת����ԭ�ַ�ģʽ
**����: ��
**����ֵ: ��
**
*************************************/

void EndRead()
{
  ioctl(1,TCSETAF,&rOldTerm);
}
#endif

/*************************************
**
**����: keyb
**Ŀ��: ���հ���
**����: ��
**����ֵ: ������ֵ
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
**����: keyn
**Ŀ��: �������ְ����͹��ܼ�
**����: ��
**����ֵ: ������ֵ
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
**����: keyf
**Ŀ��: �������֡�С����͹��ܼ�
**����: ��
**����ֵ: ������ֵ
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
**����: wkey
**Ŀ��: ���հ���,�޹����ʾ
**����: ��
**����ֵ: ������ֵ
**
*************************************/

long wkey()
{
  long nKey;
  nKey=getkey(TYPE_CHAR);
  return nKey;
}

/*************************************
**
**����: waitkey
**Ŀ��: ��24����ʾ"�����������...",�ȴ�����
**����: ��
**����ֵ: ��
**
*************************************/

long waitkey()
{
  clearline(24);
  gotoxy(30,24);
  printf("�����������...");
  return keyb();
}

/*************************************
**
**����: SetPrnDelayTime
**Ŀ��: ���ô�ӡ���ӳ�ʱ��
**����: ��ӡ���ӳ�ʱ��
**����ֵ: ��
**
*************************************/

void SetPrnDelayTime(long nDelayTime)
{
  if(nDelayTime>=1)
    nPrnDelayTime=nDelayTime;
}

/*************************************
**
**����: prnready
**Ŀ��: ���Դ�ӡ��״̬
**����: ��
**����ֵ: ��
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
**����: upper
**Ŀ��: ���ַ�����Сд��ĸ��ɴ�д
**����: ��ת�����ַ���
**����ֵ: ��
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
**����: lower
**Ŀ��: ���ַ����д�д��ĸ���Сд
**����: ��ת�����ַ���
**����ֵ: ��
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
**����: EnglishInput
**Ŀ��: �л���Ӣ������,��������DOS
**����: ��
**����ֵ: ��
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
**����: ChineseInput
**Ŀ��: �л�����������,��������DOS
**����: ��
**����ֵ: ��
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
**����: clearline2
**Ŀ��: �������
**����: nLine1 �������ʼ��
**      nLine2 ����Ľ�ֹ��
**����ֵ: ��
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
**����: delay
**Ŀ��: �ӳ�ʱ��
**����: nSecond �����ӳٵ�ʱ��
**����ֵ: ��
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
**����: savewin
**Ŀ��: ���浱ǰ����
**����: ��
**����ֵ: ��
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
    outtext("Ӧ�ó��������ͼ�洢����%d������!",MAX_WIN_NUM);
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
    outtext("Ӧ�ó��������ͼ�洢����%d������!",MAX_WIN_NUM);
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
**����: popwin
**Ŀ��: ������󱣴�Ļ��沢����Ļ����ʾ
**����: ��
**����ֵ: ��
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
    outtext("Ӧ�ó��������ͼ���������洢�����Ĵ���!");
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
    outtext("Ӧ�ó��������ͼ���������洢�����Ĵ���!");
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
**����: popwin1
**Ŀ��: ������󱣴�Ļ��治����Ļ����ʾ
**����: ��
**����ֵ: ��
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
    outtext("Ӧ�ó��������ͼ���������洢�����Ĵ���!");
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
