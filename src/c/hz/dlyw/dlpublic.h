#ifndef BKFX_H
#define BKFX_H
/***********************************************************************
** @Copyright...........1999.3
**filename      �ļ���: dlpublic.h
**target        Ŀ��:�������ҵ��ϵͳ����Ҫ����������
**create time   ��������: 1999.3
**original programmer   ����:LiBaoDong ���
**copyright     �汾
**edit history  �޸���ʷ:
**
****************************************************************************/

//#define HOUTAI

// ��ͬʱ����dlpublic.h��netmain.h��Ĳ���ϵͳ����

// DOS�±���
//#define DOS
//#define DLYWDLL
// SCO UNIX�±���
#define UNIX

// make�ļ����Ѷ��� HP UNIX�±���
//#define HPUNIX

// make�ļ����Ѷ��� IBM UNIX�±���
//#define IBMUNIX

#if defined(HPUNIX) || defined(IBMUNIX)
#define UNIX
// ���͸�λ�ֽ������
#define LITTLEEND
#endif

#if defined(DOS) && defined(UNIX)
#error Both DOS and UNIX have been defined.
#endif

#if !defined(DOS) && !defined(UNIX)
#error Neither DOS nor UNIX has been defined.
#endif


/*�ۺ�����*/

//#define DLYWLIB

//��ʾ״̬��
#define SITUATION
#define AUTOCLOSEDATABASE  //��CommitWork��RollbackWork�Զ��ر����ݿ������Ч��
                           //ÿ���޸ĺ�Ӧɾ��oracle.o,tools.oȻ�����±���

#define  SYS_ERR 1
#define  COMM_ERR 2
#define  DB_ERR 3
#define  YEWU_ERR 4
#define  TASK_ERR 5
#define  TAKE_POINT 6
#define  TAKE_STEP 7
#define  DY_YDGS "dyydgs"

#define	YZNB_LSH  1   //�����ڲ���ˮ��:�ֺ�9λ+15������+YYYYMMDD+8λ���,��������DLNBLSH
#define	YZLK_LSH  2   //�����̿���ˮ��:�ֺ�9λ+YYYYMMDD+8λ���,��������DLNKLSH
#define	WTF_LSH	  3   //��ί�з���ˮ��:8λ���,��������DLNBLSH_15λ������


#ifdef DLYWLIB
#define gotoxy bkgotoxy
#define drawbox bkdrawbox
#define getstring bkgetstring
#include "dlpay.h"
#include "varstand.h"
#endif

/*ocimain*/
#define DATACLIENT
/*bkfxd*/

/*
#define DATASERVER
*/


#if !(defined(DATASERVER)||defined(OCI)||defined(DATACLIENT))
#include "dc.h"
#include "dtype.h"
#else
#include "sql.h"
#endif

#define  JYMCSFB_RECORD  50
#define  GS_DIR_NUM  6   //��˾Ŀ¼����Ŀ¼��Ŀ
#define  MAX_GS_NUM  40
#define  BOHAO_NUM   4   //��FTP ����ʱ�����ж� �ظ����Ŵ���


#define TERM_VT100 1
#define TERM_VT220 2
#define TERM_ANSI 3

#define OUTPUT_SCREEN 1
#define OUTPUT_PRINTER 2
#define OUTPUT_TEXT 3

#define TYPE_INT 1
#define TYPE_LONG 2
#define TYPE_SHORT 3
#define TYPE_FLOAT 4
#define TYPE_DOUBLE 5
#define TYPE_LONGDOUBLE 6
#define TYPE_PASSWORD   8
#define TYPE_CHAR 11
#define TYPE_STRING 12
#define TYPE_NULL -1

/*��װ����  bkfxcsfb  20�Ų���*/
#define INST_SJ 1  //ʡ��
#define INST_DJ 2  //�ؾ�
#define INST_XJ 3  //�ؾ�
#define INST_ZJ 4  //֧��

#define PRN_OKI 1
#define PRN_LQ1600K 2

#define UNIX_HOST 1
#define UNIX_TERM 2

#define QX_MANAGER 0 /*����ҵ��������û�(ȱʡΪDLZ)ӵ�е�Ȩ��*/
#define QX_XTWH 1 /*ϵͳά��*/
#define QX_DLZCZ 2 /*���������*/
#define QX_SD 3 /*�ն�*/
#define QX_CX 4 /*��ѯ*/

#define MAX_WIN_NUM 10 /*��󱣴�Ļ�����*/

#if !(defined(DATASERVER)||defined(OCI)||defined(DATACLIENT))
#define RunSql sql
#define OpenCursor sel
#define FetchCursor fetch
#define CloseCursor csel
#define RunSelect sel1
#define RunProc prc
#define ConnectDatabase connectdb
#define CloseDatabase() closedb()
#define CommitWork() commit()
#define RollbackWork() rollback()
#define GetSqlErrNo() GetErrNo()
#define GetSqlErrInfo GetErrInfo
#endif
/*
#define RunBatch batch
*/


#define ERRMESS1 "%s\n"
#define ERRMESS2 "�������:%ld\n"
#define ERRMESS3 "����ԭ��:%s\n"
#define ERRMESS4 "���������������...\n"
#define MESSAGE010    "����ͳ��,�����Ժ�......\n"
#define MESSAGE011    "���ڲ�ѯ,�����Ժ�......\n"
#define MESSAGE012    "���ڴ�ӡ,�����Ժ�......\n"
#define MESSAGE013    "����������ı��ļ�,�����Ժ�......\n"
#define MESSAGE014    "�������ļ���: "
#define MESSAGE015    "�ļ�����ʧ��! ���ܼ��������, ��˲�!"
#define MESSAGE016    "�����������"

#define MESSAGE601    "�°�ʱ��,�����ڼӰ�. ������!"
#define MESSAGE602    "������! �����Ϣ!  "
#define MESSAGE603    "��������ĩ!  ���ڼӰ����������!"
#define MESSAGE604    "������������!  ��л�������ڹ���!"
#define MESSAGE605    "ף����һ��������ĩ! "
#define NLINE  "������������������������������������������������������������������������������\n"
#define NLINE79  "����������������������������������������������������������������������������\n"

#define GET_VAL_LEN 30

#define CZYB_CZYDH_LEN 6
#define CZYB_CZYMC_LEN 8
#define CZYB_BZ_LEN 20

#define GS_GSMC_LEN  51
#define GS_CLXBZ_LEN 11

#define DLYWCS_CS_LEN 20

#define JM_JH_LEN 9
#define JM_JM_LEN 20
#define JH_JH_LEN 9
#define JH_JM_LEN 20
#define JS_JSDH_LEN 9
#define JS_JSMC_LEN 20


#define HANGHAO "�к�"
#define YEHAO "ҳ��"

#define DLYWCS_NAME "����ҵ�������"
#define DLYWCSFB_NAME "����ҵ���������"
#define CZYB_NAME "����Ա��"
#define GS_NAME "��˾��"
#define JH_NAME "�ֺű�"
#define JS_NAME "������ͼ"
#define LSH_NAME "��ˮ�ű�"
#define JKD_NAME "�ɿ��"
#define RBD_NAME "�ձ�����"
#define KHZ_NAME "����ܱ�"


#define CZYB_CZYDH "��  ��"
#define CZYB_CZYMC "����Ա����"
#define CZYB_CZYQX "����ԱȨ��"
#define CZYB_JS "  ֧  ��  ��  ��"
#define CZYB_JSDH "�� �� ��"
#define CZYB_JSMC "��������"
#define CZYB_JM "��������"
#define CZYB_BZ " ��  ע "

#define GS_GSBH "��˾���"
#define GS_GSMC "��  ˾  ��  ��"
#define GS_BZ " ��      ע"
  
#define YWZL_YWZLBH "ҵ����"
#define YWZL_YWZLMC "ҵ �� �� �� �� ��"
#define YWZL_BZ "��                   ע"


typedef struct czyb
{
  int hanghao;
  char czydh[7];
  char czymc[9];
  long czyqx;
  char jsdh[10];
  char jsmc[21];
  char bz[21];
  char flag[2];
}CZYB;


typedef struct gs
{
  int hanghao;
  long gsbh;
  char gsmc[51];
  long nbl;
  double dbl;
  char cbl[51];
  char lxbz[11];
  char bz[41];
  char flag[2];
}GS;

#define  YWZL_YWZLMC_LEN  20
  
typedef struct ywzl
{
  int hanghao;
  long ywzlbh;
  char ywzlmc[YWZL_YWZLMC_LEN+1];
  long nbl;
  char cbl[51];
  char bz[40];
  char flag[2];
 }YWZL;

typedef struct 
{
  long x;
  long y;
  long nLen;
  char sValue[201];
  char sOldValue[201]; 
  long nFlag;
}TEXTBOX;

#define JYM_JYM_LEN  15
#define JYM_JYMC_LEN   50 
#define JYM_GSMC_LEN   50
#define JYM_FWM_LEN  10
#define JYM_SJYDZ_LEN 15  
#define JYM_JH_LEN   9
#define JYM_JM_LEN   20
#define JYM_DLJGH_LEN  20
#define JYM_BL_LEN   20
#define JYM_LXBZ_LEN 20
#define JYM_BZ_LEN   200

typedef struct
{
  long nXh;
  char cJym[JYM_JYM_LEN+1]; // ������
  char cJymc[JYM_JYMC_LEN+1]; // ��������
  char cGsmc[JYM_GSMC_LEN+1]; // ��˾����
  char cFwm[JYM_FWM_LEN+1]; // ��Χ��
  char cSjydz[JYM_SJYDZ_LEN+1]; // ����Դ��ַ
  char cJh[JYM_JH_LEN+1]; // �ֺ�
  char cJm[JYM_JM_LEN+1]; // ����
double dSflt; // �շ���ͷ
double dSxf; // ������
  char cDljgh[JYM_DLJGH_LEN+1]; // ί�з�����Ĵ��������
  long nKzlb;   // ��չ���
  long nSybz;   // ʹ�ñ�־
  long nCxms;   // ����ģʽ
  long nJsms;   //����ģʽ
  long nJsjzrq; //�����ֹ����
double dZkfl;  //�ۿ۷���
  long nLkrq;   // �޸�����
  long nBdxh;   //�䶯���
  long nDyszxh; //��ӡ�������
  long nBl1;    //����
  long nBl2;    //����
  char cBl[JYM_BL_LEN+1];     //����
double dBl;                   //����
  char cLxbz[JYM_LXBZ_LEN+1]; // ���ͱ�־
  char cBz[600+1]; // ��ע
} JYM; // ������ṹ


typedef struct{
	char	cJym[JYM_JYM_LEN+1];
	char    cSjydz[JYM_SJYDZ_LEN+1];
	long   	nBdxh;
	long    nJymBdxh;
	long  	nJhlb;
	char    cJh[JYM_JH_LEN+1];
	long 	nSyzt;
	long 	nCsbz;
	double  dZkfl;
	double 	dSxf;
	long 	nFlag1;
	long 	nFlag2;
	char	cBl1[21];
	char	cBl2[21];
	long 	nBl;
	double	dBl;
	long	nLkrq;
	long	nSfm;
	char 	cCzydh[9];
	char    cLxbz[11];
	char 	cFresh[21];
	char    cBz[201];
	double  dZekou;
	double  dSxffl;
	char    cGljJh[10];
	char    cGljDz[21];
}JYMKZFB;//��������չ����	

#define YWMX_YHBZ_LEN 20
#define YWMX_YHMC_LEN 80
#define YWMX_JSCZ_LEN 10
#define YWMX_BL_LEN 20
#define YWMX_LXBZ_LEN 10
#define YWMX_BZ_LEN 100
#define YWMX_SFMX_ELEMENT_LEN 25

typedef struct
{
  char cJym[JYM_JYM_LEN+1]; // ���״���
  long nYsnd; // ���
  long nYsyf; // �·�
  char cYhbz[YWMX_YHBZ_LEN+1]; // �û���־
  char cYhbz1[YWMX_YHBZ_LEN+1]; // �û���־1
  char cYhbz2[YWMX_YHBZ_LEN+1]; // �û���־2
  char cYhmc[YWMX_YHMC_LEN+1]; // �û�����
  long nYwlb; // ҵ�����
  long nXh; // ���
  double dSfmx[YWMX_SFMX_ELEMENT_LEN+1]; // �շ���ϸ����
  char cJscz1[YWMX_JSCZ_LEN+1]; // �����ֵ1
  long nJscz2; // �����ֵ2
  double dJscz3; // �����ֵ3
  double dJscz4; // �����ֵ4
  long nSl; // ����
  double dYjzk; // Ӧ���ܿ�
  double dXzyjk; // ����Ӧ�ɿ�
  double dSjk; // ʵ�ɿ�
  double dWjk; // δ�ɿ�
  double dDlsxf; // ����������
  long nYfkfs; // ԭ���ʽ
  long nSfkfs; // ʵ���ʽ
  long nBl1; // ����1
  long nBl2; // ����2
  double dBl1; // ����1
  double dBl2; // ����2
  char cBl1[YWMX_BL_LEN+1]; // ����1
  char cBl2[YWMX_BL_LEN+1]; // ����2
  char cJh[JM_JH_LEN+1]; // �ֺ�
  char cCzydh[CZYB_CZYDH_LEN+1]; // ����Ա����
  long nLkrq; // ����
  long nSfm; // ʱ����
  char cLxbz[YWMX_LXBZ_LEN+1]; // ���ͱ�־
  long nXzbz; // ���ʱ�־
  long nCsbz; // ������־
  char cBz[YWMX_BZ_LEN+1]; // ��ע
  char cBl3[YWMX_BL_LEN+1]; // ����3
  char cBl4[YWMX_BL_LEN+1]; // ����4
  char cBl5[YWMX_BL_LEN+1]; // ����5
  char cBl6[YWMX_BL_LEN+1]; // ����6
  char cBl7[YWMX_BL_LEN+1]; // ����7
  char cLsh[41]; 
  char cYlsh[41]; 
} YWMX; // ҵ����ϸ�ṹ

#define YWMXGL_ZDM_LEN 10
#define YWMXGL_YZDHY_LEN 40
#define YWMXGL_XZDHY_LEN 40
#define YWMXGL_SFMX_LEN 100
#define YWMXGL_BL_LEN 100
#define YWMXGL_LXBZ_LEN 10

typedef struct
{
  long nXh; // ���
  char cJym[JYM_JYM_LEN+1]; // ���״���
  long nLb; // ���
  char cZdmc[YWMXGL_ZDM_LEN+1]; // �ֶ�����
  char cYzdhy[YWMXGL_YZDHY_LEN+1]; // �ֶκ���
  char cXzdhy[YWMXGL_XZDHY_LEN+1]; // �ֶκ���
  long nSybz; // ʹ�ñ�־
  char cSfmx[YWMXGL_SFMX_LEN+1]; // �㷨��ϸ
  long    nBl; // ����
  double  dBl; // ����
  char cBl[YWMXGL_BL_LEN+1]; // ����
  char cLxbz[YWMXGL_LXBZ_LEN+1]; // ���ͱ�־
} YWMXGL; // ҵ����ϸ����ṹ

#define ZWMX_JKDH_LEN 15
#define ZWMX_LXBZ_LEN 20
#define ZWMX_BL_LEN 20
#define ZWMX_WBJSDH_LEN 20
#define ZWMX_NBJSDH_LEN 20


typedef struct
{
  char cJym[JYM_JYM_LEN+1]; // ���״���
  long nYsnd; // ���
  long nYsyf; // �·�
  char cYhbz[YWMX_YHBZ_LEN+1]; // �û���־
  char cYhbz1[YWMX_YHBZ_LEN+1]; // �û���־1
  char cYhbz2[YWMX_YHBZ_LEN+1]; // �û���־2
  char cYhmc[YWMX_YHMC_LEN+1]; // �û�����
  long nYwlb; // ҵ�����
  long nXh; // ���
  long nSl; // ����
  double dYjzk; // Ӧ���ܿ�
  double dXzyjk; // ����Ӧ�ɿ�
  double dSjk; // ʵ�ɿ�
  double dDlsxf; // ����������
  double dYbk; // Ӧ����
  double dYtk; // Ӧ�˿�
  double dSjkxj; // ʵ�ɿ��ֽ�
  double dSjkcz; // ʵ�ɿ����
  double dSjkxyk; // ʵ�ɿ����ÿ�
  long nSfkfs; // ʵ���ʽ
  char cJkdh[ZWMX_JKDH_LEN+1]; // �ɿ��
  char cJh[JM_JH_LEN+1]; // �ֺ�
  char cCzydh[CZYB_CZYDH_LEN+1]; // ����Ա����
  long nLkrq; // ����
  long nSfm; // ʱ����
  char cLxbz[ZWMX_LXBZ_LEN+1]; // ���ͱ�־
  long nXzbz; // ���ʱ�־
  long nCsbz; //������־
  long nCscs; //��������
  long nJsbz; //�����־
  char cWbjsdh[ZWMX_WBJSDH_LEN+1]; //�ⲿ���㵥��
  char cNbjsdh[ZWMX_NBJSDH_LEN+1]; //�ڲ����㵥��
  char cBl[ZWMX_BL_LEN+1];   //����
  char cBl1[ZWMX_BL_LEN+1];   //����
  char cBl2[ZWMX_BL_LEN+1];   //����
  char cBl3[ZWMX_BL_LEN+1];   //����
  char cBl4[ZWMX_BL_LEN+1];   //����
  char cBl5[ZWMX_BL_LEN+1];   //����
  long nBl;                  //����
  char cLsh[41]; 
  char cYlsh[41]; 
} ZWMX; // ������ϸ�ṹ

typedef struct
{
	char cCzydh[11];
	char cJh[11];//��������
	long nLb;//����Ա���  0 ����ϵͳ����  1 �ۺ�����������  2  �ۺ�����صǼǹ���
	long nPassword;//�����������
	long nSuccess;//�ɹ�����ϵͳ����
	long nFail;//ʧ�ܽ���ϵͳ����
	long nSyzt;//ʹ��״̬
	char cIntime[21];//����ʱ��
	char cOuttime[21];//�˳�ʱ��
	char cLastpassword[21];//�ϴθ�������ʱ��
	char cLastfail[21];//�ϴ�ʧ�ܽ���ʱ��
	char cCreateinfo[51];//����Ա����ʱ��
	char cIp[21];//��¼IP��ַ
	char cLastip[21];//�ϴβ�ͬIP
	char cLastiptime[21];
	char cLastipb[21];//���ϴβ�ͬIP
	char cLastiptimeb[21];
	char cLastczymc[21];//�ϴβ�ͬczymc
	char cLastczymctime[21];//�ϴβ�ͬczymc��ʱ��
	char cBl1[21];
	char cBl2[21];
	char cBz[41];
}CZYFSXX;


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
#define KEY_INSERT 0
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
long PrevDay(long nDate);
long NextDay(long nDate);
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
void sparklesmall(int x,int y,char* s);
void delay(long nSecond);
#endif
#define ERR_NO_SELECT 32766
#define ERR_NO_TCPIP 32767
#define ERR_NO_FETCH 32765
#else

#endif




