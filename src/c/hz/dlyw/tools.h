#ifndef TOOLS_H
#define TOOLS_H
/*---------------------------------------------------------------------*
make date:2001.02.20
filename :table.h
target:表格、数据表格、数据容器工具头文件
*---------------------------------------------------------------------*/

#include<stdarg.h>
#include<stdio.h>
#include"filechar.h"

#define MAX_TABLE_LINE     1000                    //最大表格行数
#define SCREEN_WIDTH         80                    //屏幕的列宽
#define SCREEN_LINE          24                    //屏幕的行数
#define INPUT_ITEM_SIZE     100                    //输入表格最多列
#define INPUT_STRING_SIZE   151                    //InputString中最大输入字符串长度


#define MENU_EVENT_FOCUSED      1                  //某菜单项被激活
#define MENU_EVENT_CHANGING     2                  //某菜单项失去焦点
#define MENU_EVENT_SELECTED     3                  //菜单被选中

#define TOP_LEFT      "┏"
#define TOP_RIGHT     "┓"
#define BOT_LEFT      "┗"
#define BOT_RIGHT     "┛"
#define CROSS         "┼"
#define HOR_UP        "┷"
#define HOR_DOWN      "┯"
#define VER_LEFT      "┨"
#define VER_RIGHT     "┠"
#define VER_RIGHT_SMA "├"
#define VER_LEFT_SMA  "┤"
#define VERTIC_BIG    "┃"
#define HORZEN_BIG    "━"
#define VERTIC_SMA    "│"
#define HORZEN_SMA    "─"
#define HOR_UP_SMA    "┴"
#define HOR_DOWN_SMA  "┬"

#define ST_NONE             0x00000000
#define ST_LINENUM          0x00000001        //显示行号
#define ST_MESSAGELINE      0x00000002        //附加信息栏
#define ST_NOTCONFIRM       0x00000004        //删除记录是不需要确认
#define ST_ALLTRIM          0x00000008        //自动删除字符串左右空格
#define ST_DOUBLELINE       0x00000010        //两行信息显示
#define ST_UPDOWN           0x00000020        //处在数据区时允许全屏编辑
#define ALL_STYLE           0xffffffff

#define ST_INSERT           0x00000001        //插入状态
#define ST_MODIFY           0x00000002        //修改状态

#define DOU_COL         1
#define SIG_COL         0

#define NORMAL          1                          //不带边框的对话框
#define FRAME           2                          //带连框的对话框

#define STATIC_TEXT     1                          //常量字符串
#define SEPARATOR       2                          //分隔符
#define INPUT           3                          //输入控件
#define OUTPUT          4                          //输出控件
#define SPACELINE       5                          //空行

#define CONTAINER DATATABLE
#define CreateContainer(format) (CreateDataTable(0,NULL,format))
#define DropContainer DropDataTable


//  动态弹出式菜单生成
typedef struct menuitem{
                        char sSubItem[60];       //菜单项字符串
                        struct menuitem * pNext; //指向下一个菜单项
                        char * sAddition;        //菜单附加项字符串（动态分配）
                        struct menu * pSubMenu;  //指向子菜单的指针
                        void ( * pExecute)();    //指向可执行过程
                       }MENUITEM;

typedef struct menu   {
                        char sPopTitle[61];      //菜单主题
                        long nTotal;             //菜单项数
                        long  nStyle;            //显示方法
                        MENUITEM  * pPopMenu;    //指向菜单项链表头指针
                        void ( * pAction [3])(); //事件指针
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

//接收参数值
long GetValue(long nPosx,long nPosy,char * sFormat,...);
long GetMultiValue(long npx,long npy,long * nIndex,char * sFormat,...);
long GetValueByType(INPUTSTRUCT * input,long type);
long ScanKey(long msec);
long InputString(long nPosx,long nPosy,char * sMention,long nLen,char * sValue);
void * IncBufSize(void * buf,long size,long increment);
//表格动态生成与填写

typedef struct table{
                     long nLine;                 //表格行数
                     struct table * pNext;       //连接的下一个表格
                     char sDescribe[2000];       //表格的列信息描述
                     char * sImage;              //表格的内存映像
                     long npy;                   //表格显示的起始行
                     long npx;                   //表格起始列
                     long nVisual;               //显示状态
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

//表格输入输出

typedef struct datalink{
                        long   address;             //数据段基地址
                        struct datalink * pNext;    //指向下一条记录的指针
                       }DATALINK;

typedef struct datatable{
                        TABLE * pTable;             //指向表格的指针[视图]
                        long nStyle;                //风格
                        long nCurrent;              //表格首行在数据区的位置
                        unsigned long nStatus;      //状态
                        long nPage;                 //表格显示区的大小
                        long nCount;                //表格数据区的大小
                        long nsize;                 //每条记录的大小
                        char sFormat[500];          //数据表格的类型字符串
                        char sFields[1000];         //别名域列表
                        long pArg[INPUT_ITEM_SIZE]; //字段地址偏移
                        DATALINK * pdata;           //指向数据的指针[文档]
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

//帮助工具化

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


//信息框工具

long MessageBox(char * sTitle,long nStyle);

//对话框
//nFlag:  1：静态文本   2：分隔符   3：输入    4：输出    5：空行
typedef struct dialog{
                      long nHeight;       //对话框高度
                      long nWidth;        //对话框宽度
                      long nFlag;         //对话框类型
                      long npx;           //对话框左上角横坐标
                      long npy;           //对话框左上角纵坐标
                      CONTAINER * pct;    //对话框控件列表
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

//其它
long ASCIItoBCD(char * sASC,int nLen,char * sDest);
long BCDtoASCII(char * sBCD,int nLen,char * sDest);
long FormSqlWithArgs(char * source,va_list pvar,char * dest);
#endif
