#ifndef TOOLS_H
#define TOOLS_H
/*---------------------------------------------------------------------*
make date:2001.02.20
filename :table.h
target:������ݱ��������������ͷ�ļ�
*---------------------------------------------------------------------*/

#include<stdarg.h>
#include<stdio.h>
#include"filechar.h"

#define MAX_TABLE_LINE     1000                    //���������
#define SCREEN_WIDTH         80                    //��Ļ���п�
#define SCREEN_LINE          24                    //��Ļ������
#define INPUT_ITEM_SIZE     100                    //�����������
#define INPUT_STRING_SIZE   151                    //InputString����������ַ�������


#define MENU_EVENT_FOCUSED      1                  //ĳ�˵������
#define MENU_EVENT_CHANGING     2                  //ĳ�˵���ʧȥ����
#define MENU_EVENT_SELECTED     3                  //�˵���ѡ��

#define TOP_LEFT      "��"
#define TOP_RIGHT     "��"
#define BOT_LEFT      "��"
#define BOT_RIGHT     "��"
#define CROSS         "��"
#define HOR_UP        "��"
#define HOR_DOWN      "��"
#define VER_LEFT      "��"
#define VER_RIGHT     "��"
#define VER_RIGHT_SMA "��"
#define VER_LEFT_SMA  "��"
#define VERTIC_BIG    "��"
#define HORZEN_BIG    "��"
#define VERTIC_SMA    "��"
#define HORZEN_SMA    "��"
#define HOR_UP_SMA    "��"
#define HOR_DOWN_SMA  "��"

#define ST_NONE             0x00000000
#define ST_LINENUM          0x00000001        //��ʾ�к�
#define ST_MESSAGELINE      0x00000002        //������Ϣ��
#define ST_NOTCONFIRM       0x00000004        //ɾ����¼�ǲ���Ҫȷ��
#define ST_ALLTRIM          0x00000008        //�Զ�ɾ���ַ������ҿո�
#define ST_DOUBLELINE       0x00000010        //������Ϣ��ʾ
#define ST_UPDOWN           0x00000020        //����������ʱ����ȫ���༭
#define ALL_STYLE           0xffffffff

#define ST_INSERT           0x00000001        //����״̬
#define ST_MODIFY           0x00000002        //�޸�״̬

#define DOU_COL         1
#define SIG_COL         0

#define NORMAL          1                          //�����߿�ĶԻ���
#define FRAME           2                          //������ĶԻ���

#define STATIC_TEXT     1                          //�����ַ���
#define SEPARATOR       2                          //�ָ���
#define INPUT           3                          //����ؼ�
#define OUTPUT          4                          //����ؼ�
#define SPACELINE       5                          //����

#define CONTAINER DATATABLE
#define CreateContainer(format) (CreateDataTable(0,NULL,format))
#define DropContainer DropDataTable


//  ��̬����ʽ�˵�����
typedef struct menuitem{
                        char sSubItem[60];       //�˵����ַ���
                        struct menuitem * pNext; //ָ����һ���˵���
                        char * sAddition;        //�˵��������ַ�������̬���䣩
                        struct menu * pSubMenu;  //ָ���Ӳ˵���ָ��
                        void ( * pExecute)();    //ָ���ִ�й���
                       }MENUITEM;

typedef struct menu   {
                        char sPopTitle[61];      //�˵�����
                        long nTotal;             //�˵�����
                        long  nStyle;            //��ʾ����
                        MENUITEM  * pPopMenu;    //ָ��˵�������ͷָ��
                        void ( * pAction [3])(); //�¼�ָ��
                      }MENU;

MENU * CreateMenu(char * sTitle,long nStyle);
long AddMenu(MENU * pMenu,char * sMenuItem,char * sAddition);
long BindMenu(MENU * pMenu,char * sMenuItem,char * sAddition,void (* pExecute)());
long BindSubMenu(MENU * pMenu,MENU * pSubMenu,char * sAddition);
long AddMenuEvent(MENU * pMenu,long nEvnet,void (* pAction)(MENU * pmn,long nId));
long ShowMenu(MENU * pMenu);
long DropMenu(MENU * pMenu);
char * GetMenu(MENU * pMenu,long nWhere);
char * GetAddition(MENU * pMenu,long nWhere);
long IsInMenu(MENU * pMenu,char * sIndex,long nType);
long PopMenu(MENU * pMenu,long nposx,long nposy,long nHeight);
long ShowFallMenu(MENU * pMenu,long nStart);
static long  getchoice(long * nChoose,long nStyle,long nMaxNo);

typedef struct inputstruct{
                            char OldValue[INPUT_STRING_SIZE];
                            char Value[INPUT_STRING_SIZE];
                            long Modified;
                            long nLength;
                            long x,y;
                          }INPUTSTRUCT;  

//���ղ���ֵ
long GetValue(long nPosx,long nPosy,char * sFormat,...);
long GetMultiValue(long npx,long npy,long * nIndex,char * sFormat,...);
long GetValueByType(INPUTSTRUCT * input,long type);
long ScanKey(long msec);
long InputString(long nPosx,long nPosy,char * sMention,long nLen,char * sValue);
void * IncBufSize(void * buf,long size,long increment);
//���̬��������д

typedef struct table{
                     long nLine;                 //�������
                     struct table * pNext;       //���ӵ���һ�����
                     char sDescribe[2000];       //��������Ϣ����
                     char * sImage;              //�����ڴ�ӳ��
                     long npy;                   //�����ʾ����ʼ��
                     long npx;                   //�����ʼ��
                     long nVisual;               //��ʾ״̬
                    }TABLE;

TABLE * CreateTable(long nLine,char * sDescribe);
long MergeTable(TABLE * pObject,TABLE * pSource);
long ShowTable(TABLE * pTable,long npx,long npy);
long FillTable(TABLE * pTable,long nLine,char * sFormat,...);
long FetchTable(TABLE * pTable,long nLine,long nWhere,char * sFormat,...);
long OutputTable(TABLE * pTable);
static long GetTableWidth(char * sDescribe);
static long ShowTableLine(TABLE * pTable,long nLine);
static long FormatString(TABLE * pTable,long nLine,long nWhere,char * sOutput);
static void ClearTableBox(TABLE * pTable,long nCol,long nLine,long nCount);
void ClearTableLine(TABLE * pTable,long nLine);
void DropTable(TABLE * pTable);

//����������

typedef struct datalink{
                        long   address;             //���ݶλ���ַ
                        struct datalink * pNext;    //ָ����һ����¼��ָ��
                       }DATALINK;

typedef struct datatable{
                        TABLE * pTable;             //ָ�����ָ��[��ͼ]
                        long nStyle;                //���
                        long nCurrent;              //�����������������λ��
                        unsigned long nStatus;      //״̬
                        long nPage;                 //�����ʾ���Ĵ�С
                        long nCount;                //����������Ĵ�С
                        long nsize;                 //ÿ����¼�Ĵ�С
                        char sFormat[500];          //���ݱ��������ַ���
                        char sFields[1000];         //�������б�
                        long pArg[INPUT_ITEM_SIZE]; //�ֶε�ַƫ��
                        DATALINK * pdata;           //ָ�����ݵ�ָ��[�ĵ�]
                       }DATATABLE;

DATATABLE * CreateDataTable(long nLine,char * sDescribe,char * sFormat);
long AssignTableFields(DATATABLE * pdt,char * sFields);
void SetTableStyle(DATATABLE * pdt,long nStyle,long nflag);
long ShowDataTable(DATATABLE * pdt,long npx,long npy);
void RefreshDataTable(DATATABLE * pdt);
long ImportData(DATATABLE * pdt,long nLine,char * sFormat,...);
long ExportData(DATATABLE * pdt,long nLine,char * sFormat,...);
long FetchData(DATATABLE * pdt,long nLine,long nWhere,char * sFormat);
long DeleteDataNode(DATATABLE * pdt,long nLine);
long ScrollTable(DATATABLE * pdt,long nOpt);
long InsertDataNode(DATATABLE * pdt,long nLine);
long FlushDataLine(DATATABLE * pdt,long nLine);
long FindDataTable(DATATABLE * pdt,long nStart,char * sFormat,...);
long SumDataTable(DATATABLE * pdt,long nStart,long nEnd,char * sFormat,...);
void ShowDatatableInfo(DATATABLE * pdt,char * smsg,long ntype);
long AutoExtendTable(DATATABLE * pdt,long * pnCurrent);
long DefaultTableKey(DATATABLE * pdt,long nKey,long * pnCurrent,long * pnWhere);
long DefaultTableChoice(DATATABLE * pdt,char cOpt,long * pnCurrent,
                        long * pnWhere,long * pnModify);
static long FindTableByIndex(DATATABLE * pdt,long nCurrent,char * sIndex);
static long TranslateTableFields(DATATABLE * pdt,long nIndex,
                        long nMode,char * sInFormat,char * sOutFormat);
long DropDataTable(DATATABLE * pdt);

//�������߻�

long ShowHelp(char * sHlpFile,char * sIndex);
MENU * CreateHelpMenuByFile(char * sHlpFile);
static MENU * SearchItemByIndex(MENU * pMenu,char * sIndex,long * pnFlag);
static long CreateHlpIndex(char * sHlpFile);
static MENU * CreateHlpMenu(char * sHlpIndexFile);
static void DrawHlpInterface(char * sHlpFile);
static void ShowHlpContents(long nType,char * sIndex,char * sHlpFile);
static void PrintHlpContents(long nOffset,char * sIndex,char *sHlpFile);
static long SearchHlpFile(FILE * fph,FILE * fpi);
static MENU * SearchHlpIndexFile(FILE * fpIndex,char * sTitle,char * sHlpFile);


//��Ϣ�򹤾�

long MessageBox(char * sTitle,long nStyle);

//�Ի���
//nFlag:  1����̬�ı�   2���ָ���   3������    4�����    5������
typedef struct dialog{
                      long nHeight;       //�Ի���߶�
                      long nWidth;        //�Ի�����
                      long nFlag;         //�Ի�������
                      long npx;           //�Ի������ϽǺ�����
                      long npy;           //�Ի������Ͻ�������
                      CONTAINER * pct;    //�Ի���ؼ��б�
                     }DIALOG;

DIALOG * CreateDialog(long nWidth,long nHeight,long nStyle);
long AddDialog(DIALOG * pd,char * sField,long nWhat,...);
void ShowDialog(DIALOG * pd,long npx,long npy);
long ActiveDialog(DIALOG * pd,long nWhere,char * sField);
void RefreshDialog(DIALOG * pd,char * sField);
void SetDialogInfo(DIALOG * pd,char * sInfo);
long GetDialogPosition(DIALOG * pd,char * sField,long * pnRow,long * pnCol);
static long TranslateDialogFields(DIALOG * pd,char * sField);
void DropDialog(DIALOG * pd);

//����
long ASCIItoBCD(char * sASC,int nLen,char * sDest);
long BCDtoASCII(char * sBCD,int nLen,char * sDest);
long FormSqlWithArgs(char * source,va_list pvar,char * dest);
#endif
