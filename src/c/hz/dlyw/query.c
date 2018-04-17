/*=============================================================
                  标准化查询工具模块实现文件
                         (query.c)

original programmer:chenbo [陈博]
created date:2001.08.15
=============================================================*/

#include<stdlib.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include"netmain.h"
#include"dlpublic.h"
#include"tools.h"
#include"query.h"
#include"filechar.h"
/*=============================================================
函数：CreateQueryTable
功能：创建一个查询表对象
参数：
      sHeader:查询表标题，如果是NULL则表示没有标题信息
      sTitle:表头字符串
      sFormat:对表头列各字段的格式及类型描述
      nStyle:风格，目前有以下几种选项：
             ST_LASTLINE:带有统计行（最末行）
             ST_KEYWORD: 带有关键字查找功能
             
返回：指向查询表对象的指针，若返回NULL则为失败      
例子：
    QUERYTABLE * pqt;
    pqt=CreateQueryTable("  查询信息",
    "  记录号  用户标识             实缴款     操作员     交易日期    交易时间",
    "%06ld %-20.20s %10.2lf %-6.6s %8ld %06ld",
    ST_LASTLINE|ST_KEYWORD);

    nId=OpenCursor("select cyhbz,dsjk,cczydh,nlkrq,nsfm from dl$zwmx");
    nLoop=1;
    clrscr();
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%s%lf%s%ld%ld",sYhbz,&dSjk,cCzydh,&nLkrq,&nSfm);
    	if(ImportQueryTable(pqt,nRetVal,nLoop++,sYhbz,dSjk,cCzydh,nLkrq,nSfm)<=0) break;
    }
    CloseCursor(nId);	
    DropQueryTable(pqt);

效果：
                               查询信息
   -----------------------------------------------------------------------
   记录号  用户标识             实缴款     操作员     交易日期    交易时间
   -----------------------------------------------------------------------
   000001  13508030041           200.00    D001       20010810    081200
   .......
   .......
   -----------------------------------------------------------------------
   000027  .                    1400.20    .          20010810    170000   
   -----------------------------------------------------------------------
   
*============================================================*/

QUERYTABLE * CreateQueryTable(char * sHeader,char * sTitle,char * sFormat,long nStyle)
{
    QUERYTABLE * pqt;
    char * sOffset;
    
    if(!sTitle||!sFormat) return NULL;
    if(!(pqt=(QUERYTABLE *)malloc(sizeof(QUERYTABLE)))) return NULL;

    memset(pqt,0,sizeof(QUERYTABLE));    
    pqt->sTitle=sTitle;
    pqt->sFormat=sFormat;
    pqt->nLength=strlen(sTitle)+1;
    pqt->nStyle=nStyle;
    pqt->nGroupSize=1;
    pqt->nCurrentGroup=1;
    
    sOffset=sFormat;
    while(sOffset[0])
    {
    	pqt->nFieldSize+=(sOffset[0]=='%');
    	sOffset++;
    }

    if(sHeader)
    {
    	strcpy(pqt->sHeader,sHeader);
    	pqt->nStartLine=1;
    	sOffset=sHeader;
    	while(sOffset[0]) if(sOffset++[0]=='\n') pqt->nStartLine++;
    } 
    pqt->nPage=(SCREEN_LINE-8)+((nStyle&ST_LASTLINE)==0)*2
               +((nStyle&ST_KEYWORD)==0)-pqt->nStartLine;
                   
    return pqt; 

}
/*=============================================================
函数：SetQueryColSize
功能: 设置每行的列数
参数：pqt:查询表指针
      nGroupSize:列数值
*============================================================*/
void SetQueryGroupSize(QUERYTABLE * pqt,long nGroupSize)
{
     if((pqt->nFieldSize%nGroupSize)!=0)
     {
     	MessageBox("设置查询分组出错:单组列数不是整数,无法设置分组!",0);
     	return;
     }
     pqt->nGroupSize=nGroupSize;
     
}


/*=============================================================
函数：SetQueryAction
功能: 设置处理函数
参数：pqt:查询表指针
      pAction:处理函数指针
*============================================================*/
void SetQueryAction(QUERYTABLE * pqt,void (*pAction)(char * msgline))
{
     pqt->pAction=pAction;
}

/*=============================================================
函数：ShowQueryTable
功能: 显示查询表
参数：pqt:查询表指针
*============================================================*/
long ShowQueryTable(QUERYTABLE * pqt)
{
    char sShow[640],sLine[640];
    long nLoop,nRetVal;
    
    
    if(!pqt) return -1;
    pqt->nStyle|=ST_VISUAL;
    gotoxy(1,1);
    if(pqt->nStartLine) outtext("%s\n",pqt->sHeader);
    outtext(
    "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    memset(sShow,'\0',sizeof(sShow));
    memcpy(sShow,pqt->sTitle+pqt->nStartCol,SCREEN_WIDTH-1);
    outtext("%s\n",sShow);

    outtext(
    "───────────────────────────────────────");

    gotoxy(1,pqt->nPage+pqt->nStartLine+4);
    if(pqt->nStyle&ST_LASTLINE)
    {

        outtext(
        "───────────────────────────────────────");
        gotoxy(1,pqt->nPage+pqt->nStartLine+6);
        outtext(
        "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    }
    else outtext(
        "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    
    if(pqt->nLength>SCREEN_WIDTH && (pqt->nStartCol<(pqt->nLength-SCREEN_WIDTH)))
    {
       gotoxy(SCREEN_WIDTH-3,1);
       outtext(">>");
    }
    
    if(pqt->nStartCol>0)
    {
       gotoxy(1,1);
       outtext("<<");
    	
    }

    if(pqt->nStyle&ST_LASTLINE && pqt->sLastLine[0])
    {
    	gotoxy(1,pqt->nPage+pqt->nStartLine+5);
        memset(sShow,'\0',sizeof(sShow));
        memcpy(sShow,pqt->sLastLine+pqt->nStartCol,SCREEN_WIDTH-1);
        outtext(sShow);
    }    
    	
    
    if(!pqt->fp) return 0;
    #ifdef DOS
    fseek(pqt->fp,pqt->nCurrent*(pqt->nLength+1),SEEK_SET);
    #else
    fseek(pqt->fp,pqt->nCurrent*pqt->nLength,SEEK_SET);
    #endif    
    
    nRetVal=1;
    
    for(nLoop=1;nLoop<=pqt->nPage;nLoop++)
    {
    	memset(sShow,'\0',sizeof(sShow));
    	memset(sLine,'\0',sizeof(sLine));
        if(nRetVal>0) nRetVal=getline(sLine,pqt->fp);
        if(nRetVal>0)
        {
            gotoxy(1,pqt->nStartLine+nLoop+3);
            memcpy(sShow,sLine+pqt->nStartCol,SCREEN_WIDTH-1);
            if((pqt->nCurrent+nLoop)==pqt->nReverse)
            {
            	 ReverseChar();
                 outtext(sShow);
                 ShowChar();
            }
            else outtext(sShow);
        }
        else clearline(pqt->nStartLine+nLoop+3);
    }        
    return nRetVal;
}

/*=============================================================
函数：ImportLastLine
功能: 向查询表的最后一行（统计行）导入数据
参数：pqt:查询表指针
      ...:被导入的数据，其规则与ImportQueryTable一致
*============================================================*/
long ImportLastLine(QUERYTABLE * pqt,...)
{
    va_list pvar;

    long nGroupSize,nCurrentGroup;
    if(!pqt) return 0;
    nGroupSize=pqt->nGroupSize;
    nCurrentGroup=pqt->nCurrentGroup;
    pqt->nCurrentGroup=1;
    pqt->nGroupSize=1;
    va_start(pvar,pqt);
    FormStringByFormat(pqt,pqt->sLastLine,pvar);
    va_end(pvar);
    pqt->nCurrentGroup=nCurrentGroup;
    pqt->nGroupSize=nGroupSize;
    pqt->nCount--;
    return 1;      
}


/*=============================================================
函数：ImportQueryTable
功能: 向查询表中导入数据
参数：pqt:查询表指针
      nflag:  <=0:表明数据已导入完毕   >0表示还有数据
      ...:被导入的数据，其个数和排列顺序应与建立时的类型描述
          一致
返回：<=0:不需要再导入数据  >0需要导入数据          
说明：本函数自动处理界面显示的一切操作，并提供按关键字查询和
      报表打印功能；
*============================================================*/
long ImportQueryTable(QUERYTABLE * pqt,long nflag,...)
{
    char sOutput[640];
    long nKey,nRetVal;
    va_list pvar;
    
    if(!pqt) return 0;
    
    if(!pqt->fp)
    {
    	GetTempFileName(pqt->sFileName);
    	pqt->fp=fopen(pqt->sFileName,"w+");
    }

    if(nflag>0)
    {
        va_start(pvar,nflag);
        if(FormStringByFormat(pqt,pqt->sStorage,pvar)>0)
        {
            va_end(pvar);
            return 1;
        }
        va_end(pvar);

    	fseek(pqt->fp,0,SEEK_END);
    	fwrite(pqt->sStorage,1,pqt->nLength,pqt->fp);
    	fflush(pqt->fp);
    	if(pqt->nCount%pqt->nPage) return 1;
    }
    else if(pqt->nCurrentGroup>1)
    {
    	fseek(pqt->fp,0,SEEK_END);
    	fwrite(pqt->sStorage,1,pqt->nLength,pqt->fp);
    	fflush(pqt->fp);
    }

    if(!(pqt->nStyle&ST_VISUAL)&&(nflag>0||pqt->nStyle&ST_PRINT)) return nflag;
    if(pqt->nStatus==0)
    {
    	pqt->nCurrent=pqt->nCount-pqt->nPage;
    	if(pqt->nCount%pqt->nPage) 
    	     pqt->nCurrent+=pqt->nPage-(pqt->nCount%pqt->nPage);
    	ShowQueryTable(pqt);
    }
    for(;;)
    {
    	if(pqt->nStatus==0)
    	{
    	      gotoxy(1,SCREEN_LINE);
    	      outtext("请使用[↑][↓]翻页,[ESC]返回,[TAB]左右移动,[P]打印,[C]处理选择行 [第%ld页].",
    	                                                        (pqt->nCurrent/pqt->nPage)+1);
              if(pqt->nStyle&ST_KEYWORD)
    	          nKey=GetValue(1,SCREEN_LINE-1,"请输入关键字:%-40.40s",pqt->sIndex);
    	      else
    	      {  
    	      	  nKey=keyb();    
    	      	  if(nKey=='p' || nKey=='P') nKey=KEY_PRINT;
    	      	  if(nKey=='c' || nKey=='C') nKey=KEY_PROCESS;
    	      	  if(nKey==KEY_RETURN) nKey=KEY_DOWN;
    	      }	  
    	}
    	else if(pqt->nStatus==1) nKey=KEY_RETURN;      
    	else if(pqt->nStatus==2) nKey=KEY_PRINT;      
    	
    	if(nKey==KEY_ESC) return 0;

    	if(nKey==KEY_TAB)
    	{
    	    if(pqt->nLength<SCREEN_WIDTH) continue;
            if(pqt->nStartCol>pqt->nLength-SCREEN_WIDTH) pqt->nStartCol=0; 
    	    else pqt->nStartCol+=60;
            clrscr();
            ShowQueryTable(pqt);
            continue;
        }       

        if(nKey==KEY_RETURN && pqt->nStatus==0)
        {
            alltrim(pqt->sIndex);
            if(strlen(pqt->sIndex)==0) nKey=KEY_DOWN;
            if(strlen(pqt->sIndex)==1)
            {
               if(pqt->sIndex[0]=='p' || pqt->sIndex[0]=='P')
               {
            	  pqt->sIndex[0]='\0';
            	  nKey=KEY_PRINT;
               }
               if(pqt->sIndex[0]=='c' || pqt->sIndex[0]=='C')
               {
            	  pqt->sIndex[0]='\0';
            	  nKey=KEY_PROCESS;
               }
            }
        }    

    	if(nKey==KEY_RETURN)  //查询
    	{
            pqt->nReverse=0;
            if(pqt->nStatus==0)
            { 
    	         alltrim(pqt->sIndex);
                 clearline(SCREEN_LINE);
                 gotoxy(1,SCREEN_LINE);
                 outtext("正在查询，按任意键停止查询...       ");
                 nRetVal=FindQueryTable(pqt);
                 if(nRetVal<=0&&nflag<=0)
                 {
                 	pqt->nStartRow=0;
                        nRetVal=FindQueryTable(pqt);
                 }          	
                 if(nRetVal<=0&&nflag<=0)
                 {
                        ShowQueryTable(pqt);
                        clearline(SCREEN_LINE);
                        gotoxy(1,SCREEN_LINE);
                        pqt->nStartRow=0;
                        outtext("没有匹配的信息...   ");
                        keyb();
                        continue;
                 }
                 if(nRetVal<=0)
                 {
                 	pqt->nStatus=1;
                 	return 1;
                 }	
            }
            else
            {
                 if(nflag>0) pqt->nStartRow=pqt->nCount-pqt->nPage;
                 if(pqt->nStartRow<0) pqt->nStartRow=0;
                 nRetVal=FindQueryTable(pqt);
                 if(nRetVal<=0 && nflag<=0)
                 {
                     pqt->nStartRow=0;
                     nRetVal=FindQueryTable(pqt);
                 }    
                     
                 if(nRetVal<=0)
            	 {
            	     if(nflag<=0||ScanKey(1))
            	     {
                        ShowQueryTable(pqt);
                        pqt->nStatus=0;
                        pqt->nStartRow=0;
                        clearline(SCREEN_LINE);
                        gotoxy(1,SCREEN_LINE);
                        outtext("没有匹配的信息...");
                        keyb();
            	        continue;
            	     }
            	     return 1;   
            	 }
            }	 
            pqt->nStatus=0;
            pqt->nStartRow=nRetVal;
            pqt->nCurrent=(nRetVal-1)-((nRetVal-1)%pqt->nPage);
            pqt->nReverse=nRetVal;
            ShowQueryTable(pqt);
            continue;
        } //end KEY_RETURN


    	if(nKey==KEY_DOWN||nKey==KEY_PAGEDOWN)
    	{
    	    if(pqt->nCurrent+pqt->nPage>=pqt->nCount)
    	    {
    	    	if(nflag>0) return 1;
    	    	else continue;
    	    }	
    	    pqt->nCurrent+=pqt->nPage;
            ShowQueryTable(pqt);
            continue;
        }    

    	if(nKey==KEY_UP||nKey==KEY_PAGEUP)
    	{
    	    if(pqt->nCurrent<=0) continue;
    	    pqt->nCurrent-=pqt->nPage;
            ShowQueryTable(pqt);
            continue;
        }    

        if(nKey==KEY_PRINT)
        {
            if(pqt->nStatus==0)
            {
                clearline(SCREEN_LINE);
                gotoxy(1,SCREEN_LINE);
                outtext("请选择打印范围：1:全部打印  2:打印本页以前的  3:只打印当前页");
                nRetVal=keyb();
                clearline(SCREEN_LINE);
                switch(nRetVal)
                {
                    case '1':
                          if(nflag>0)
                          {
                             clearline(SCREEN_LINE);
                             gotoxy(1,SCREEN_LINE);
                             outtext("正在形成数据，按任意键停止...");
                             pqt->nStatus=2;
                             return 1;
                          }
                          break;
                    case '2':
                    case '3':
                          break;
                    default:continue;      
                }
                PrintQueryTable(pqt,nRetVal-'0');
            }
            else
            {
                if(ScanKey(1))
                {
                   pqt->nStatus=0;
                   continue;
                }
                if(nflag>0) return 1;
                pqt->nStatus=0;
                PrintQueryTable(pqt,1);
            }
        }//end KEY_PRINT
        
        if(nKey==KEY_PROCESS)
        {
            if(pqt->pAction)
            {
                if(pqt->nReverse)
                {
                   #ifdef DOS
                   fseek(pqt->fp,(pqt->nReverse-1)*(pqt->nLength+1),SEEK_SET);
                   #else
                   fseek(pqt->fp,(pqt->nReverse-1)*pqt->nLength,SEEK_SET);
                   #endif    
         	   memset(sOutput,'\0',sizeof(sOutput));
                   getline(sOutput,pqt->fp);
                   pqt->pAction(sOutput);
                }
                else
                {
                    clearline(SCREEN_LINE);
                    gotoxy(1,SCREEN_LINE);
                    outtext("请先输入关键字，找到指定行后再使用本功能...");
                    keyb();
                }//end if
            }//end if           	
        }//end if
    }//end for
}

/*=============================================================
函数：DropQueryTable
功能: 释放一个查询表资源
参数：pqt:查询表指针
*============================================================*/
long DropQueryTable(QUERYTABLE * pqt)
{
     if(!pqt) return 0;
     fclose(pqt->fp);
     remove(pqt->sFileName);
     free(pqt);
     return 1;       
}

//按关键字查询
static long FindQueryTable(QUERYTABLE * pqt)
{
    long nCurrent;
    char sLine[640];
    
    #ifdef DOS
    fseek(pqt->fp,pqt->nStartRow*(pqt->nLength+1),SEEK_SET);
    #else
    fseek(pqt->fp,pqt->nStartRow*pqt->nLength,SEEK_SET);
    #endif
    
    nCurrent=pqt->nStartRow;
    for(;;)
    {
    	memset(sLine,'\0',sizeof(sLine));
    	if(getline(sLine,pqt->fp)<=0) break;
    	if(strstr(sLine,pqt->sIndex)) return nCurrent+1;
    	nCurrent++;
    }
    return 0;	
}

//报表打印
long PrintQueryTable(QUERYTABLE * pqt,long nmode)
{
    long nStart,nEnd;
    char sLine[640];
    
    if(!pqt->nCount) return 0;
    clearline(SCREEN_LINE);
    gotoxy(1,SCREEN_LINE);
    outtext("请准备好打印机，按回车键开始打印或按其它键放弃打印...");
    if(keyb()!=KEY_RETURN) return 0;
    
    if(nmode==1) 
    {
    	nStart=0;
    	nEnd=pqt->nCount;
    }
    	    
    if(nmode==2) 
    {
    	nStart=0;
    	nEnd=pqt->nCurrent+pqt->nPage;
    }
    
    if(nmode==3) 
    {
    	nStart=pqt->nCurrent;
    	nEnd=nStart+pqt->nPage;
    }
    
    setprinton();
    memset(sLine,'\0',sizeof(sLine));
    output("\n");
    if(pqt->nStartLine)
    {
    	output(pqt->sHeader);
    	output("\n");
    }	
    output(pqt->sTitle);
    output("\n");
    memset(sLine,'-',pqt->nLength);
    output(sLine);
    output("\n");
    #ifdef DOS
    fseek(pqt->fp,(pqt->nLength+1)*nStart,SEEK_SET);
    #else
    fseek(pqt->fp,pqt->nLength*nStart,SEEK_SET);
    #endif
    
    for(;nStart<=nEnd;nStart++)
    {
    	getline(sLine,pqt->fp);
    	output(sLine);
    }
    memset(sLine,'-',pqt->nLength);
    output("\n");
    if(pqt->nStyle&ST_LASTLINE && nmode==1)
    {
        output(sLine);
        output("\n");
        output(pqt->sLastLine);
        output("\n");
    }    
    output(sLine);
    output("\n");
    eject();
    setprintoff();
    return 1;
}

//按规定格式形成显示串
static long FormStringByFormat(QUERYTABLE * pqt,char * sOutput,va_list pvar)
{
    char * sOut,* sOffset,* sTitle;
    char sOutFormat[81];
    long nLoop,nGroupCol;
    
    if(!pqt||!sOutput) return 0;

    sOffset=pqt->sFormat;
    sTitle=pqt->sTitle;
    nGroupCol=pqt->nFieldSize/pqt->nGroupSize;
    sOut=sOutput;
    
    if(pqt->nCurrentGroup==1)
    {
    	memset(sOutput,'\0',pqt->nLength);
    	pqt->nCount++;
    }

    nLoop=(pqt->nCurrentGroup-1)*nGroupCol;

    while(nLoop--)
    {
    	while(sOffset[0]!='%') sOffset++;
    	while(sOffset[0]=='%') sOffset++;

        while(sTitle[0]==' ')
        {
            sOut++;
            sTitle++;
        }
        while(sTitle[0]!=' ')
        {
            sOut++;
            sTitle++;
        }
    }	

    while(sOffset[0] && nGroupCol--)
    {
    	while(sOffset[0]!='%')  sOffset++;
    	//字段对齐
        while(sTitle[0]==' ')
        {
            sOut++;
            sTitle++;
        }

    	memset(sOutFormat,'\0',sizeof(sOutFormat));
    	nLoop=0;

        while(sOffset[0])
        {
            sOutFormat[nLoop]=sOffset[0];
            sOffset++;
            nLoop++;
            if(sOffset[0]=='%') break;
        }

        //形成字符串
        if(strchr(sOutFormat,'s')) sprintf(sOut,sOutFormat,va_arg(pvar,char *));
        else if(strchr(sOutFormat,'d')) sprintf(sOut,sOutFormat,va_arg(pvar,long));
        else if(strchr(sOutFormat,'f')) sprintf(sOut,sOutFormat,va_arg(pvar,double));

        while(sTitle[0]!=' ' && sTitle[0])
        {
            sOut++;
            sTitle++;
        }
    }//end while
    sOutput[pqt->nLength]='\0';
    sOutput[pqt->nLength-1]='\n';
    sOut=sOutput+pqt->nLength-2;
    while(sOut>=sOutput)
    {
    	if(((unsigned char)sOut[0])<' ') sOut[0]=' ';
    	sOut--;
    }
    pqt->nCurrentGroup++;
    if(pqt->nCurrentGroup>pqt->nGroupSize)
    {
    	pqt->nCurrentGroup=1;
    	return 0;
    }
    return 1;	
}

/*--------------------------------------------------------------------

[函数]:long QueryLocal(char * sTitle,char * sFormat,char * sSqlExpression,...)
[目的]:标准化本地机查询工具
[参数]:
     sTitle:        查询界面表头
     sFormat:       打开游标区的数据类型描述[要与FetchCursor完全一致]
     sSqlExpression:SQL语名表达式[任何查询语名表达式，包括多表联合查询]
     ...           :SQL查询语名所用到的参数
       
[返回]:>0 成功    <0 失败

[例子]：QueryLocal("年度  月份 报刊代号  报刊名称                   发刊局",
                  "%ld%ld%s%s%s",
                  "select nysnd,nysyf,cbkdh,cbkmc,cfkjm from bk$bkml where nbklb=%ld",
                  nBklb);
[效果]：
                  
     年度  月份 报刊代号  报刊名称                   发刊局
     -------------------------------------------------------------------
     2000   01  1-1       人民日报                   北京
     .....
     .....
    
[说明]：1.查询语句中的列的个数不超过20个;    ITEMSIZE
        2.字符串长度不超过500；              INPUT_STRING_SIZE
        3.解释功能的使用：
          有时需要将查询到的结果以用户可以接受的方式显示，除浮点数据外，都可
          以配置解释功能，具体方法如下：
          比如将对应整数解释成相应的字符串："%ld[1:报纸;2:杂志;default:xxxxx]"
          表示如果该整数的结果为1就显示“报纸”，为2就显示杂志，否则显示"xxxxx"。
          注意每个解释项必须用“；”隔开，解释字符串中不能包括'%',';','[',']','?'。
          如：QueryLocal("  报刊代号       报刊类别       报刊名称  ",
                          "%s%ld[1:报纸;2:杂志]%s",
                          "select cbkdh,nbklb,cbkmc from bk$bkml");
                          
               
*----------------------------------------------------------------------*/
long QueryLocal(char * sTitle,char * sFormat,char * sSqlExpression,...)
{

    QUERYUNIT query;
    FILE * fpIn,* fpOut;
    va_list   pVar;
    long nLoop,nError;
    char sSql[MAX_SQL_SIZE];
    char sLine[100];
    char sResult[INPUT_STRING_SIZE];
    char * sTemp,* sForm;
    char * sPrint,* sExplain;
    char sTempFile[21];
    long nCount,nFind,nId,nRetVal;                             
    long nLine,nRec,nposx,nOffset;
    
    if(strlen(sTitle)<1 || (!strchr(sFormat,'%'))  || strlen(sSqlExpression)<1) return -1;
    
    
    if(strlen(sTitle)>SCREEN_WIDTH)
    {
        outtext("标题栏宽度大于屏幕总宽，不能显示！");
        keyb();
        return -1;
    }

    sExplain=sFormat;
    sForm=query.sFormat;
    nLoop=0;
    while((sExplain=strchr(sExplain,'%'))!=NULL)
    {
        sForm[0]=sExplain[0];
        sForm[1]=sExplain[1];
        if(sForm[1]=='l')
        {
                sForm[2]=sExplain[2];
                sForm++;
                sExplain++;
        }
        sForm+=2;
        sExplain+=2;
        if(++nLoop>ITEMSIZE)
        {
                outtext("数据个数不能超过%ld个，若要增加数据个数，请修改tools.h中ITEMSIZE的值");
                keyb();
                return -1;
        }               
    }                   
    sForm[0]='\0'; 
    for(nLoop=0;nLoop<ITEMSIZE;nLoop++) query.data[nLoop].pLong=NULL;
    nLoop=0;
    nError=0;
    
    
/////////////////////////    数据动态分配        //////////////////////////////        
    
   
    sExplain=sFormat;
    sFormat=query.sFormat;
    while(sFormat=strchr(sFormat,'%'))
    {
        sFormat++;
        switch(sFormat[0])
        {
             case 's':
             case 'S':
                  nError=((query.data[nLoop].pChar=(char *)malloc(INPUT_STRING_SIZE))==NULL);
                  break;
             case 'l':
             case 'L':
                  sFormat++;
                  switch(sFormat[0])
                  {
                        case 'd':
                        case 'D':
                             nError=((query.data[nLoop].pLong=
                                            (long *)malloc(sizeof(long)))==NULL);
                             break;
                        case 'f':
                        case 'F':
                             nError=((query.data[nLoop].pDouble=
                                            (double *)malloc(sizeof(double)))==NULL);
                             break;
                  }
                  break;
        }
        if(nError||(nLoop++)>ITEMSIZE)   //动态分配内存出错
        {
                ReleaseQueryUnit(&query);
                return -2;
        }
     }//end while


////////////////////////////  根据参数形成最终SQL语名  /////////////////    

    nError=0;
    sTemp=sSqlExpression;
    strcpy(sSql,"");
    sForm=sSql;
    va_start(pVar,sSqlExpression);
    
    while(sTemp[0]!='\0')
    {
        switch(sTemp[0])
        {
           case '%':
                sTemp++;
                switch(sTemp[0])
                {
                    case 's':
                         sprintf(sForm,"'%s'",va_arg(pVar,char *));
                         while(sForm[0]!='\0') sForm++;
                         sTemp++;
                         break;
                    case 't':
                         sprintf(sForm,"%s",va_arg(pVar,char *));
                         while(sForm[0]!='\0') sForm++;
                         sTemp++;
                         break;
                    case 'l':
                         sTemp++;
                         switch(sTemp[0])
                         {
                             case 'f':
                                  sprintf(sForm,"%-.2lf",va_arg(pVar,double));
                                  while(sForm[0]!='\0') sForm++;
                                  sTemp++;
                                  break;
                                  
                             case 'd':
                                  sprintf(sForm,"%ld",va_arg(pVar,long));
                                  while(sForm[0]!='\0') sForm++;
                                  sTemp++;
                                  break;
                             
                             default:nError=1;
                         }        
                         break;
                    
                    default:
                         sForm[0]='%';
                         sForm++;
                         sForm[0]='\0';
                         break;         
                 }//end switch
                 break;
                 
           default:          //非‘%’
                sForm[0]=sTemp[0];
                sForm++;
                sTemp++;
                break;
        }//end switch         
        if(nError)
        {
                
                ReleaseQueryUnit(&query);
                outtext("SQL表达式数据格式错误！");
                keyb();
                return -2;
        }

    }//end while
    va_end(pVar);
    sForm[0]='\0';

////////////////////////////   显示用户界面   /////////////////////////////

    clrscr();
    gotoxy(1,1);
    outtext(sTitle);
    gotoxy(1,2);
    for(nLoop=1;nLoop<SCREEN_WIDTH;nLoop++) outtext("-");
    gotoxy(1,SCREEN_LINE-2);
    for(nLoop=1;nLoop<SCREEN_WIDTH;nLoop++) outtext("-");
    gotoxy(1,SCREEN_LINE-1);
    outtext("正在查询数据库，请稍候........");
    
///////////////////////////////////////////////////////////////////////////
     nCount=1;
     if((nId=OpenCursor(sSql))<=0)
     {
        ReleaseQueryUnit(&query);
        HiError(nId,"查询出错！");
        return -5;
     }


     nLine=2;
     nRec=0;
     nError=0;
     
     GetTempFileName(sTempFile);   
     if(!(fpOut=fopen(sTempFile,"w+")))
     {
        ReleaseQueryUnit(&query);
        CloseCursor(nId);
        gotoxy(1,SCREEN_LINE-1);
        outtext("  计算机系统错误！无法建立临时文件，请通知系统管理员。                   ");
        keyb();
        return -5;
     }
               
     #ifdef UNIX
     sprintf(sSql,"rm %s>/dev/null",sTempFile);
     #else
     sprintf(sSql,"del %s",sTempFile);
     #endif     
                    
     for(;;)          //显示查询数据
     {
        nLoop=0;

//////////////////////  处理用户翻页和屏幕暂停功能   //////////////////////

        sFormat=sExplain;
        if(++nLine<SCREEN_LINE-2) clearline(nLine);
        gotoxy(1,SCREEN_LINE);

        if(nLine>SCREEN_LINE-3)
        {
            gotoxy(1,SCREEN_LINE-1);

            if(nCount>SCREEN_LINE-5)
                   outtext("  [按PgUp,PgDn或'1','2'翻页,ESC或'E'返回]  ");
            else   outtext("  查询完毕，按ESC或'E'返回....                                       ");

            switch(keyb())
            {
                case KEY_ESC:
                case 'E':
                case 'e':
                     ReleaseQueryUnit(&query);
                     fclose(fpOut);
                     remove(sTempFile);
                     CloseCursor(nId);
                     return 1;

                case KEY_UP:
                case KEY_PAGEUP:
                case '1':
                     if(ftell(fpIn)/SCREEN_WIDTH<=SCREEN_LINE-5)
                     {
                         continue;
                     }
                        
                     if(nRec>=nCount)
                     {
                        if(nCount%(SCREEN_LINE-5))
                        {
                                nOffset=SCREEN_LINE-5+nCount%(SCREEN_LINE-5);
                        }
                        else
                        {
                                nOffset=(SCREEN_LINE-5)*2;
                        }
                     }
                     else
                     {
                        nOffset=(SCREEN_LINE-5)*2;
                     }  

                     #ifdef UNIX
                     fseek(fpIn,(nRec-nOffset)*SCREEN_WIDTH,SEEK_SET);
                     #else
                     fseek(fpIn,(nRec-nOffset)*(SCREEN_WIDTH+1),SEEK_SET);
                     #endif
                     nRec-=((nOffset%(SCREEN_LINE-5))?nCount%(SCREEN_LINE-5):(SCREEN_LINE-5)); 
                                            
                     gotoxy(1,3);
                     for(nLoop=1;nLoop<=SCREEN_LINE-5;nLoop++)
                     {
                        memset(sLine,'\0',sizeof(sLine));
                        getline(sLine,fpIn);
                        outtext(sLine);
                     }  
                     continue;        

                default:
                     if(nRec>=nCount) continue;
                     nLine=2;
                     gotoxy(1,3);
                     for(nLoop=1;nLoop<=SCREEN_LINE-5;nLoop++)
                     {
                        if(getline(sLine,fpIn)<=0)
                        {
                                if(nRec<nCount)
                                {
                                        fseek(fpIn,0,SEEK_END);   
                                        break;
                                }       
                                else
                                {
                                         for(nLine=nLoop+2;nLine<SCREEN_LINE-2;nLine++)
                                                clearline(nLine);
                                         nLine=SCREEN_LINE;
                                         break;
                                }        
                                         
                        }
                        else
                        {
                                nRec++;                         
                                nLine++;
                                outtext(sLine);
                        }       
                     }  

                     continue;
            }
        }//end if
        

        if(nRec>=nCount)
        {
                if(nLine<SCREEN_LINE-2) clearline(nLine);  
                continue;
        }
        

////////////////// 取得数据并按规定的格式显示在屏幕上 ///////////////////

                
        nRetVal=FetchStruction(nId,&query);
        nCount+=(nRetVal>0);
        
        if(nError==0)
        {
           nRec++;
           memset(sLine,'\0',sizeof(sLine));
           memset(sLine,' ',SCREEN_WIDTH-1);
           if(nRec<nCount)
           {
             while(sFormat=strchr(sFormat,'%'))
             {
                sTemp=sTitle; 
                while(sTemp[0]==' ') sTemp++;  //去除前面的空格
                for(nFind=1;nFind<=nLoop;nFind++)
                {
                        while(sTemp[0]!=' ' && sTemp[0]!='\0') sTemp++;
                        while(sTemp[0]==' ') sTemp++;
                }        
                
                nposx=sTemp-sTitle+1;
                sPrint=sLine+nposx-1;
                
                gotoxy(nposx,nLine);
                                
                sFormat++;
                switch(sFormat[0])
                {
                    case 's':
                        sForm=query.data[nLoop].pChar;
                        
                        StringMatch(sFormat+1,sForm,sResult);    //字符串解释
                        sForm=sResult;
                        while(sTemp[0]!=' '&&sForm[0]!='\0'&&sTemp[0]!='\0')
                        {
                               outtext("%c",sForm[0]);
                               sPrint[0]=sForm[0];
                               sPrint++;
                               sForm++;
                               sTemp++;
                        }
                        while(sTemp[3]==' '&&sForm[0]!='\0'&&sTemp[0]!='\0')
                        {
                               outtext("%c",sForm[0]);
                               sPrint[0]=sForm[0];
                               sPrint++;
                               sForm++;
                               sTemp++;
                        }
                        break;

                    case 'l':
                        sFormat++;
                        switch(sFormat[0])
                        {
                            case 'd':
                                 sprintf(sResult,"%ld",(* query.data[nLoop].pLong));
                                 StringMatch(sFormat+1,sResult,sPrint);
                                 outtext(sPrint);
                                 while(sPrint[0]!='\0') sPrint++;
                                 sPrint[0]=' ';
                                 break;
                            case 'f':
                                 outtext("%.2lf",(* query.data[nLoop].pDouble));
                                 sprintf(sPrint,"%.2lf",(* query.data[nLoop].pDouble));
                                 while(sPrint[0]!='\0') sPrint++;
                                 sPrint[0]=' ';
                                 break;
                        }
                        break;
                } //end switch
                nLoop++;
              }//end while
            }//end if 
            sLine[SCREEN_WIDTH-1]='\n';
            sLine[SCREEN_WIDTH]='\0';
            fwrite(sLine,SCREEN_WIDTH,1,fpOut);
            fflush(fpOut);
            fpIn=fpOut;
                
        }//end if
        else               //错误
        {
            ReleaseQueryUnit(&query);
            fclose(fpOut);
            remove(sTempFile);
            CloseCursor(nId);
            HiError(nError,"取数据出错！");
            return -5;
        }
  
   } //end for(;;)        
                
}

/*------------------------------------------------------------------
 函数：StringMatch;
 功能：字符串匹配函数
 参数：sObject:被匹配的字符集表；
       sKeyWord:匹配关键字；
       sResult:匹配结果；

 例子：
 
     [1] 
          程序：
          StringMatch("[1:one;2:two;3:three;detault:unknown;]","2",sResult);
                   
          printf(sResult);
           
          结果：
                two              
 
     [2]
          程序：
          StringMatch("[1:one;2:two;3:three;detault:unknown;]","x",sResult);
                   
          printf(sResult);
           
          结果：
                unknown
                
                
     [3]                          

          程序：
          StringMatch("[1???:one;2???:two;3???:three;]","3128",sResult);
                   
          printf(sResult);
           
          结果：
                three
     
 
 注意：'?'表示通配符，表示对该位不做比较
          
 *----------------------------------------------------------------*/

long StringMatch(char * sObject,char * sKeyWord,char * sResult)
{
     
     long nLoop;
     char sKey[INPUT_STRING_SIZE+1];
          
     if(sObject==NULL || sKeyWord==NULL || sResult==NULL) return -1;
     while(sObject[0]!='[' && sObject[0]!='%' && sObject[0]!='\0') sObject++;
     if(sObject[0]!='[')          //没有匹配字符串
     {
        strcpy(sResult,sKeyWord);
        return 0;
     }
     sObject++;
     
     sprintf(sKey,"%s:",sKeyWord);
     
     if(strstr(sObject,sKey)!=NULL && strstr(sObject,sKey)<strchr(sObject,']'))
     {
        sObject=strstr(sObject,sKey)+strlen(sKey);
        strncpy(sResult,sObject,strchr(sObject,';')-sObject);
        return 1;
     }
     else
     {
        nLoop=0;
        while(sObject[nLoop]!=']' && sObject[nLoop]!='\0')
        {
                if(sObject[nLoop]=='?')
                {      
                        nLoop++;
                        continue;
                }
                if(sObject[nLoop]!=sKey[nLoop])  //收搜下一条件
                {
                    do{
                        
                           nLoop++;
                      
                      }while(sObject[nLoop]!=';' && sObject[nLoop]!=']' && sObject[nLoop]!='\0');
                        
                    continue;
                }
                        
                if(sObject[nLoop]==':')
                {
                        strncpy(sResult,sObject+nLoop+1,
                                        strchr(sObject+nLoop,';')-(sObject+nLoop+1));
                        return 1;
                }        
                nLoop++;
                                
        }                                               
        //如果没有找到匹配字符，查看是否有缺省选项
        if(strstr(sObject,"default:")!=NULL &&strstr(sObject,"default:")<strchr(sObject,']'))
        {
                strncpy(sResult,strstr(sObject,"default:")+8,
                                 strchr(sObject,']')-(strstr(sObject,"default:")+7));
                
                return 1;
        }       
                                                        
        strcpy(sResult,sKeyWord);
        return 0;
     }
}

void ReleaseQueryUnit(QUERYUNIT * query)
{
    long nLoop=0;
    
    if(query==NULL) return;
    
    while(query->data[nLoop].pLong!=NULL)
    {
        free((void*)query->data[nLoop].pLong);
        if((nLoop++)>ITEMSIZE) return;
    }
}
long FetchStruction(long nCursorId,QUERYUNIT * que)
{
  long nRetVal;
  long col;
  long nColNum;
  long nType;
  int nOciSocket;
  char * sFormat;
  char sTemp[INPUT_STRING_SIZE];
  
  nOciSocket=ReturnSocket();
  if(nOciSocket==0)  return -1012;

  sFormat=que->sFormat;
  if(SendLongToSocket(nOciSocket,(long)TASK_FETCHCURSOR))
  {
     nOciSocket=0;
     return -ERR_NO_TCPIP;
  }

  if(SendLongToSocket(nOciSocket,nCursorId))
  {
     nOciSocket=0;
     return -ERR_NO_TCPIP;
  }

  if(GetLongFromSocket(nOciSocket,&nRetVal))
  {
     nOciSocket=0;
     return -ERR_NO_TCPIP;
  }

  if(nRetVal==1)
  {
     if(GetLongFromSocket(nOciSocket,&nColNum))
     {
        nOciSocket=0;
        return -ERR_NO_TCPIP;
     }
     for (col = 0; col < nColNum; col++)
     {
        sFormat=strchr(sFormat,'%');
        sFormat++;
        if(GetLongFromSocket(nOciSocket,&nType))
        {
             nOciSocket=0;
             return -ERR_NO_TCPIP;
        }
        switch (nType)
        {
             case TYPE_DOUBLE:
                  if(GetDoubleFromSocket(nOciSocket,que->data[col].pDouble))
                  {
                      return -ERR_NO_TCPIP;
                  }
                  break;
             case TYPE_LONG:
                  if(GetLongFromSocket(nOciSocket,que->data[col].pLong))
                  {
                      return -ERR_NO_TCPIP;
                  }
                  break;
             default:
                  if(GetStringFromSocket(nOciSocket,sTemp))
                  { 
                      return -ERR_NO_TCPIP;
                  }
                  if(!strncmp(sFormat,"ld",2)) *(que->data[col].pLong)=atol(sTemp);
                    else if(!strncmp(sFormat,"s",1))  strcpy(que->data[col].pChar,sTemp);
                       else if(!strncmp(sFormat,"lf",2)) *(que->data[col].pDouble)=atof(sTemp);
                  break;        
        }          
      }//end for
    }//end if
  return nRetVal;
}

