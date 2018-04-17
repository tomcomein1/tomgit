/*--------------------------------------------------------------------*

                    General Tools for Programmer

                程序开发人员常用工具实现文件(tools.c)

                           (UNIX/DOS)

Orginal Programmer:陈博
Created date:2001.01.12
注：工具的详细使用规则请参看《工具使用说明》
*--------------------------------------------------------------------*/

#include"dlpublic.h"
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<stdarg.h>
#include<memory.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/times.h>
#include<sys/select.h>
#include<malloc.h>
#include"tools.h"
/*---------------------------------------------------------------*

函数:MENU * CreateMenu;
目的:创建一个菜单对象
参数:sTitle:菜单主题,nStyle=0单列显示，nStyle<>0双列显示
返回:如果成功则返回指向该模块的指针，否则返回NULL

-----------------------------------------------------------------*/

MENU * CreateMenu(char * sTitle,long nStyle)
{
   MENU * pNewNode;
   if((pNewNode=(MENU *)malloc(sizeof(MENU)))!=NULL)
   {
      memset(pNewNode,0,sizeof(MENU));
      pNewNode->nStyle=nStyle;
      sprintf(pNewNode->sPopTitle,"%-60.60s",sTitle);
      alltrim(pNewNode->sPopTitle);
      return pNewNode;
   }
   else return NULL;
}

/*-----------------------------------------------------------------

函数:long AddMenu;
目的:向已经创建的菜单对象中增加菜单选项
参数:pMenu:由CreateMenu()返回的指向菜单对象的指针；
     sMenuItem:菜单选项的内容
     sAddition:附加字符串内容
返回:>0成功    <0失败
-----------------------------------------------------------------*/

long AddMenu(MENU * pMenu,char * sMenuItem,char * sAddition)
{
   MENUITEM  * pNewNode;
   MENUITEM  * pSub;

   if(pMenu==NULL) return -1;
   if(pMenu->nTotal>=999) return -1;

   if((pNewNode=(MENUITEM *)malloc(sizeof(MENUITEM)))!=NULL)
   {
          pNewNode->pNext=NULL;
          pMenu->nTotal++;
          pNewNode->sAddition=NULL;
          pNewNode->pExecute=NULL;
          pNewNode->pSubMenu=NULL;
          if(sAddition)    //动态分配附加字符串
          {
                pNewNode->sAddition=(char*)malloc(strlen(sAddition)+1);
                if(pNewNode->sAddition)
                           strcpy(pNewNode->sAddition,sAddition);
          }

          if(pMenu->nStyle==0)     //单列显示
          {
                if(strchr(sMenuItem,'.'))
                      sprintf(pNewNode->sSubItem,"%-54.54s",sMenuItem);
                else  sprintf(pNewNode->sSubItem,"%3ld.%-50.50s",
                                              pMenu->nTotal,sMenuItem);
          }
          else                   //双列显示
          {
                if(strchr(sMenuItem,'.'))
                      sprintf(pNewNode->sSubItem,"%-24.24s",sMenuItem);
                else  sprintf(pNewNode->sSubItem,"%3ld.%-22.22s",
                                              pMenu->nTotal,sMenuItem);
          }

          pSub=pMenu->pPopMenu;

          if(!pSub)  pMenu->pPopMenu=pNewNode;
          else
          {
             while(pSub->pNext) pSub=pSub->pNext;
             pSub->pNext=pNewNode;
          }
          return 1;
    }else return -1;
}//end Add

/*-----------------------------------------------------------------

函数:long BindMenu;
目的:向菜单中增加选项，当该选项被选中后自动运行它所指向的过程
参数:pMenu菜单对象指针,sMenuItem：菜单选项，pExecute：需执行的函数名
返回:>0成功   <0失败

*-----------------------------------------------------------------*/
long BindMenu(MENU * pMenu,char * sMenuItem,char * sAddition,void (* pExecute)())
{
   MENUITEM * pSearch;

   if(pMenu==NULL || sMenuItem==NULL || pExecute==NULL) return -1;

   if(AddMenu(pMenu,sMenuItem,sAddition))
   {
        pSearch=pMenu->pPopMenu;
        while(pSearch->pNext!=NULL) pSearch=pSearch->pNext;
        pSearch->pExecute=pExecute;
        return 1;
   }
   return -1;

}
/*-----------------------------------------------------------------

函数:long AddMenuEvent;
目的:向菜单中菜单事件处理程序
参数:
     pMenu:菜单指针
     nEvent:表示要处理的事件，它的值可以是：
            MENU_EVENT_FOCUSED  :  某一菜单项正处于激活状态
            MENU_EVENT_CHANGING :  某一菜单项正在失去激活状态
            MENU_EVENT_SELECTED :  某一菜单项被选中
     pAction:处理事件函数的函数体指针
          它有两个参数要传递给事件处理函数，
          即：本菜单指针pmn和当前选项序号nId;
例子：
     AddMenuEvent(pMenu,MENU_EVENT_FOCUSED,theMenuFocused);

返回:>0成功   <0失败

*-----------------------------------------------------------------*/
long AddMenuEvent(MENU * pMenu,long nEvent,void (* pAction)(MENU * pmn,long nId))
{
    if(pMenu==NULL||nEvent<1||nEvent>3) return -1;
    pMenu->pAction[nEvent-1]=pAction;
    return 1;
}


/*-----------------------------------------------------------------

函数:long BindSubMenu;
目的:向菜单中增加选项，当该选项被选中自动弹出级联菜单pSubMenu
参数:pMenu菜单对象指针,pSubMenu：级联菜单指针,sAddition:附加项
返回:>0成功   <0失败

*-----------------------------------------------------------------*/
long BindSubMenu(MENU * pMenu,MENU * pSubMenu,char * sAddition)
{

    MENUITEM * pSearch;

    if(pMenu==NULL||pSubMenu->sPopTitle==NULL||pSubMenu==NULL) return -1;
    if(AddMenu(pMenu,pSubMenu->sPopTitle,sAddition)>0)
    {
        pSearch=pMenu->pPopMenu;
        while(pSearch->pNext!=NULL) pSearch=pSearch->pNext;
        pSearch->pSubMenu=pSubMenu;
        return 1;
    }
    return -1;

}


/*-----------------------------------------------------------------

函数:long ShowMenu;
目的:将已经创建的菜单显示在屏幕上
参数:pMenu菜单对象指针
返回:>0成功   <0失败

*-----------------------------------------------------------------*/
long ShowMenu(MENU * pMenu)
{
   MENUITEM  * pSub,* pSearch;
   long ntopx,ntopy,nbottomx,nbottomy;
   long ndraw,nposx,noffset=0,nKey=0;
   long nDrawMax,nTemp,nSelect;

   extern long nSysAutoClose;


   if(pMenu==NULL) return -1;
   pSub=pMenu->pPopMenu;

   if(pSub==NULL) return -1;

   if(pMenu->nStyle==0) nDrawMax=9;
   else nDrawMax=18;
   ntopx=10;
   nbottomx=70;
   if(pMenu->nTotal<18)
   {
         if(pMenu->nStyle!=0) ntopy=(22-pMenu->nTotal)/2-1;
         else if(pMenu->nTotal<9)
         {
                   ntopy=(21-(pMenu->nTotal*2))/2;
         }
         else ntopy=1;
         if(pMenu->nStyle!=0) nbottomy=ntopy+pMenu->nTotal+(pMenu->nTotal%2)+2;
         else if(pMenu->nTotal<9)
         {
                   nbottomy=ntopy+pMenu->nTotal*2+2;
         }
         else nbottomy=21;
   }
   else
   {
         ntopy=1;
         nbottomy=21;
   }

   clrscr();

   noffset=0;
   for(;;)
   {
      //heli 20001213
      #ifndef AUTORUN
      show_statue();
      #endif


      if(nSysAutoClose) CommitWork();           //关闭数据库连接
      drawbox(ntopx,ntopy,nbottomx,nbottomy);
      nposx=(56-strlen(pMenu->sPopTitle))/2;
      if(nposx%2) nposx++;
      if(strlen(pMenu->sPopTitle)%2) strcat(pMenu->sPopTitle," ");
      gotoxy(ntopx+nposx,ntopy);
      outtext(" %s ",pMenu->sPopTitle);
      pSub=pMenu->pPopMenu;
      ndraw=0;
      for(ndraw=0;ndraw<noffset;ndraw++) pSub=pSub->pNext;
      ndraw=0;
      while(ndraw++<nDrawMax)
      {
           if(pMenu->nStyle==0)
           {
                gotoxy(ntopx+4,ntopy+ndraw*2);
           }
           else
           {
                if(!(ndraw%2))  gotoxy(ntopx+32,ntopy+2+ndraw-((ndraw+1)%2)-1);
                else gotoxy(ntopx+4,ntopy+2+ndraw-((ndraw+1)%2)-1);
           }
           if(pSub==NULL)
           {
               if(pMenu->nStyle!=0) if(ndraw<=pMenu->nTotal) outtext("                        ");
               if(pMenu->nStyle==0) if(ndraw<=pMenu->nTotal)
                                   outtext("                                                ");
           }
           else
           {
               outtext("%s",pSub->sSubItem);
               pSub=pSub->pNext;
           }

       }//end while
       if((pMenu->nStyle==0 && pMenu->nTotal>9)||pMenu->nTotal>18)
       {
           gotoxy(14,24);
           outtext("共有%d项，PageUp向上翻页，PageDown向下翻页，ESC退出",pMenu->nTotal);
       }

       gotoxy(29,22);
       outtext("请输入你的选择:");

       nKey=getchoice(&nSelect,0,pMenu->nTotal);

       switch(nKey)
       {
           case KEY_PAGEUP:
           case KEY_LEFT:
           case KEY_UP:
                if(noffset>=nDrawMax) noffset-=nDrawMax;
                else noffset=0;
                break;
           case KEY_PAGEDOWN:
           case KEY_DOWN:
           case KEY_RIGHT:
                if(noffset<pMenu->nTotal-nDrawMax) noffset+=nDrawMax;
                break;
           case KEY_RETURN:
                if(nSelect>=0 && nSelect<=pMenu->nTotal)
                 {
                        pSearch=pMenu->pPopMenu;
                        nTemp=nSelect;
                        if(nSelect>0)
                        {
                                while(--nTemp) pSearch=pSearch->pNext;
                                if(pSearch->pExecute!=NULL)
                                {
                                        (* pSearch->pExecute)(nSelect,pSearch->sSubItem+4,pSearch->sAddition);
                                        clrscr();
                                }
                                else return nSelect;
                        }
                        if(nSelect==0) return 0;


                 }
                break;
           case KEY_ESC:
                return -1;
                break;
         }//end switch
      }//end for
}

/*-----------------------------------------------------------------

函数:long PopMenu;
目的:在指定位置（nposx,nposy）弹出一个菜单
参数:nposx,nposy:菜单顶点位置，nHeight：菜单高度,pMenu:由CreateMenu建立的菜单
返回:<0 代表功能键的相反数，=0失败    >0返回选中菜单的号码

*-----------------------------------------------------------------*/
long PopMenu(MENU * pMenu,long nposx,long nposy,long nHeight)
{

    long nWidth=0,nPageSize;
    long nTemp,nCurrent,nKey=0,nSelect;
    long mx,my;
    MENUITEM * pSearch,* pSub;
    char sValue[81];


    if(pMenu==NULL) return -1;
    if(pMenu->nTotal<1) return -1;

    nCurrent=1;
    if(nposx<1) nposx=1;
    if(nposx>SCREEN_WIDTH) nposx=SCREEN_WIDTH-10;

    if(nposy<1) nposy=1;
    if(nposy>(SCREEN_LINE-4)) nposy=SCREEN_LINE-4;

    if(nHeight<4) nHeight=4;
    if(nHeight+nposy>SCREEN_LINE) nHeight=SCREEN_LINE-nposy;

    nHeight+=nposy;


    pSearch=pMenu->pPopMenu;
    while(pSearch!=NULL)
    {
        rtrim(pSearch->sSubItem);
        nTemp=strlen(pSearch->sSubItem);
        nWidth=(nWidth<nTemp)?nTemp:nWidth;
        pSearch=pSearch->pNext;
    }

    if((nposx+nWidth+6)>SCREEN_WIDTH)  nposx=SCREEN_WIDTH-(nWidth+6);

    nPageSize=pMenu->nTotal;
    if(nPageSize>(nHeight-2-nposy)) nPageSize=nHeight-2-nposy;


    drawbox(nposx,nposy,nposx+nWidth+4,nposy+nPageSize+1+(nPageSize<pMenu->nTotal));
    pSearch=pMenu->pPopMenu;
    nTemp=nPageSize;

    mx=nposx+2;
    my=nposy+1;

    while(nTemp--)
    {
        memset(sValue,'\0',sizeof(sValue));
        memset(sValue,' ',nWidth);
        strncpy(sValue,pSearch->sSubItem,strlen(pSearch->sSubItem));
        gotoxy(mx,my);
        outtext(sValue);
        if(pSearch->pSubMenu)
        {
                gotoxy(mx+nWidth,my);
                outtext(" >");
        }

        pSearch=pSearch->pNext;
        my++;
    }

    for(;;)
    {
        pSearch=pMenu->pPopMenu;

        memset(sValue,'\0',sizeof(sValue));
        memset(sValue,' ',nWidth);

        if(nPageSize<pMenu->nTotal&&(nCurrent+nPageSize)<=pMenu->nTotal)
        {
             gotoxy(nposx+nWidth/2,nposy+nPageSize+1);
             outtext("↓↑");
        }

        for(nTemp=1;nTemp<nCurrent;nTemp++)  pSearch=pSearch->pNext;
        my=((nCurrent-1)%nPageSize)+nposy+1;
        strncpy(sValue,pSearch->sSubItem,strlen(pSearch->sSubItem));
        
        gotoxy(mx,my);
        ReverseChar();
        outtext(sValue);
        ShowChar();
        if(pMenu->pAction[0]) pMenu->pAction[0](pMenu,nCurrent);
         
        nSelect=nCurrent;
        nKey=getchoice(&nSelect,1,pMenu->nTotal);
        gotoxy(mx,my);
        outtext(sValue);
        switch(nKey)
        {
                case  KEY_RIGHT:
                      if(pSearch->pSubMenu!=NULL)
                      {
                          if(pMenu->pAction[2]) pMenu->pAction[2](pMenu,nCurrent);
                          savewin();
                          PopMenu(pSearch->pSubMenu,nposx+nWidth/2+(nWidth/2)%2,
                                  nposy+nCurrent,SCREEN_LINE);
                          popwin();
                      }
                      else return -nKey;
                      continue;

                case  KEY_UP:

                      if(pMenu->pAction[1]) pMenu->pAction[1](pMenu,nCurrent);
                      if((nCurrent%nPageSize)==1)
                      {
                         nKey=KEY_PAGEUP;
                         break;
                      }
                      else
                      {
                      	 nCurrent--;
                      }
                      continue;
                case  KEY_DOWN:
                      if(pMenu->pAction[1]) pMenu->pAction[1](pMenu,nCurrent);
                      if(nCurrent==pMenu->nTotal)
                      {
                         nCurrent=1;
                         break;
                      }

                      if((nCurrent%nPageSize)==0)
                      {
                         nKey=KEY_PAGEDOWN;
                         break;
                      }
                      else if(nCurrent<pMenu->nTotal) nCurrent++;
                      continue;
                case  KEY_RETURN:
                      if(nSelect<0||nSelect>pMenu->nTotal) continue;
                      nCurrent=nSelect;
                      pSub=pMenu->pPopMenu;

                      nTemp=nSelect;
                      if(nTemp>0)
                      {
                          while(--nTemp) pSub=pSub->pNext;
                          if(pSub->pSubMenu!=NULL)
                          {
                               if(pMenu->pAction[2]) pMenu->pAction[2](pMenu,nCurrent);
                               savewin();
                               PopMenu(pSub->pSubMenu,nposx+nWidth/2+(nWidth/2)%2,
                                           nposy+nCurrent,SCREEN_LINE);
                               popwin();
                               continue;
                          }
                          else if(pSub->pExecute!=NULL)
                          {
                               if(pMenu->pAction[2]) pMenu->pAction[2](pMenu,nCurrent);
                               savewin();
                               (* pSub->pExecute)(nCurrent,pSub->sSubItem+4,pSub->sAddition);
                               popwin();
                               continue;
                          }
                          return nCurrent;

                      }
                      if(nTemp==0) return 0;
                      break;

                case  KEY_PAGEUP:
                case  KEY_PAGEDOWN:
                      if(pMenu->pAction[1]) pMenu->pAction[1](pMenu,nCurrent);
                      break;
                default:
                      return -nKey;

        }
        switch(nKey)
        {
                case KEY_PAGEUP:
                     if(nCurrent<nPageSize)
                     {
                        nCurrent=1;
                        continue;
                     }
                     nCurrent-=nPageSize;
                     nCurrent-=((nCurrent-1)%nPageSize);
                     break;
                case KEY_PAGEDOWN:
                     if(nPageSize==pMenu->nTotal||nCurrent>=pMenu->nTotal)   continue;
                     nCurrent+=nPageSize;
                     nCurrent-=((nCurrent-1)%nPageSize);
                     if(nCurrent>pMenu->nTotal) nCurrent-=nPageSize;
                     break;
        }
        pSearch=pMenu->pPopMenu;
        for(nTemp=1;nTemp<nCurrent;nTemp++)  pSearch=pSearch->pNext;
        nTemp=nPageSize;
        my=nposy+1;
        while(nTemp--)
        {
                memset(sValue,'\0',sizeof(sValue));
                memset(sValue,' ',nWidth);
                gotoxy(mx,my);
                if(pSearch)
                {
                        strncpy(sValue,pSearch->sSubItem,strlen(pSearch->sSubItem));
                        outtext(sValue);
                        if(pSearch->pSubMenu)
                        {
                                gotoxy(mx+nWidth,my);
                                outtext(" >");
                        }
                        pSearch=pSearch->pNext;
                }
                else    outtext(sValue);
                my++;
        }
    }
}


/*-----------------------------------------------------------------

函数:ShowFallMenu
目的:显示多列式瀑布式菜单
参数:pMenu: 菜单对象指针
     nStart:显示行
返回:功能键
*-----------------------------------------------------------------*/
long ShowFallMenu(MENU * pMenu,long nStart)
{

     long nCol[256],nCount,nWidth;
     long nStartCol,nCurrent,nKey=0,nSelect;
     char sMenuTitle[2048];
     char * sOffset;
     char sValue[100];
     MENUITEM * pmi;

     if(!pMenu||nStart<1||nStart>SCREEN_LINE||pMenu->nTotal<1) return 0;

     pmi=pMenu->pPopMenu;
     memset(sMenuTitle,' ',sizeof(sMenuTitle));
     sOffset=sMenuTitle;
     nStartCol=0;
     nCount=0;
     nCurrent=0;
     while(pmi)
     {
     	strcpy(sValue,pmi->sSubItem);
     	alltrim(sValue);
     	nWidth=(strlen(sValue));
     	memcpy(sOffset,sValue,nWidth);
     	nCol[nCount++]=sOffset-sMenuTitle;
     	sOffset+=nWidth+2;
     	pmi=pmi->pNext;
     }
     sOffset[0]='\0';
     nCol[nCount]=nCol[nCount-1]+nWidth+2;

     for(;;)
     {

   	if((nCol[nCurrent+1]-nStartCol-2>SCREEN_WIDTH))
           nStartCol=nCol[nCurrent+1]-(SCREEN_WIDTH-2);

   	if((nCol[nCurrent]-nStartCol<0))
   	   nStartCol=nCol[nCurrent];

        clearline(nStart);
     	memset(sValue,'\0',sizeof(sValue));
     	memcpy(sValue,sMenuTitle+nStartCol,SCREEN_WIDTH-4);
     	gotoxy(1,nStart);
     	outtext(sValue);
     	if((strlen(sMenuTitle)-nStartCol)>SCREEN_WIDTH) outtext(" >>");


        if(nKey==KEY_RETURN||nKey==KEY_DOWN)
        {
             pmi=pMenu->pPopMenu;
             nSelect=nCurrent;
             while(nSelect--) pmi=pmi->pNext;

             if(pmi->pSubMenu)
             {
                clearline(24);
             	savewin();
             	nKey=PopMenu(pmi->pSubMenu,nCol[nCurrent]-nStartCol,
             	             nStart+2,SCREEN_LINE-nStart-3);
             	popwin();
             }
             else if(pmi->pExecute && nKey==KEY_RETURN)
             {
             	savewin();
             	pmi->pExecute(nCurrent,pmi->sSubItem,pmi->sAddition);
             	popwin();
             }
        }

     	memset(sValue,'\0',sizeof(sValue));
     	memcpy(sValue,sMenuTitle+nCol[nCurrent],nCol[nCurrent+1]-nCol[nCurrent]-2);
     	gotoxy(nCol[nCurrent]-nStartCol+1,nStart);
     	ReverseChar();
     	outtext(sValue);
     	ShowChar();

        show_statue();
        nSelect=nCurrent;
     	nKey=getchoice(&nSelect,1,nCount);

     	switch(nKey)
     	{
           case KEY_ESC:
                return KEY_ESC;

           case '0':
           case 0:
     	        return 0;

     	   case KEY_RETURN:
     	        if(nSelect>=1 && nSelect<=nCount) nCurrent=nSelect-1;
     	        if(nSelect==0) return 0;
     	        break;

     	   case KEY_LEFT:
     	        nCurrent--;
     	        if(nCurrent<0) nCurrent=0;
     	        break;

     	   case KEY_RIGHT:
     	        nCurrent++;
     	        if(nCurrent>=nCount) nCurrent=nCount-1;
     	        break;
     	}
     }
}


/*-----------------------------------------------------------------

函数:long DropMenu;
目的:删除一个菜单
参数:pMenu菜单对象指针
返回:>0成功    <0失败

*-----------------------------------------------------------------*/

long DropMenu(MENU * pMenu)
{
   MENUITEM  * pSub;
   MENUITEM  * pSubRm;

   if(pMenu==NULL) return -1;
   pSub=pMenu->pPopMenu;
   while(pSub)
   {
          pSubRm=pSub->pNext;
          if(pSub->sAddition) free(pSub->sAddition);
          if(pSub->pSubMenu)  DropMenu(pSub->pSubMenu);
          free(pSub);
          pSub=pSubRm;
   }
   free(pMenu);
   return 1;
}

/*-----------------------------------------------------------------

函数:char * GetMenu;
目的:得到菜单第nWhere项的选项字符串
参数:pMenu:指向菜单对象的指针，nWhere：菜单选项序号
返回:成功：返回字符串的指针，NULL失败

*-----------------------------------------------------------------*/

char * GetMenu(MENU * pMenu,long nWhere)
{
  MENUITEM * pSub;

  if(pMenu==NULL || pMenu->nTotal<nWhere || nWhere<1) return NULL;

  pSub=pMenu->pPopMenu;
  while(--nWhere) pSub=pSub->pNext;
  return ((char *)(strchr(pSub->sSubItem,'.')+1));

}

/*-----------------------------------------------------------------

函数:char * GetAddition;
目的:得到菜单第nWhere项的附加项字符串
参数:pMenu：指向菜单对象的指针，nWhere菜单选项序号
返回:成功：返回字符串的指针，NULL失败

*-----------------------------------------------------------------*/
char * GetAddition(MENU * pMenu,long nWhere)
{
  MENUITEM * pSub;

  if(pMenu==NULL || pMenu->nTotal<nWhere || nWhere<1) return NULL;

  pSub=pMenu->pPopMenu;
  while(--nWhere) pSub=pSub->pNext;
  return (pSub->sAddition);

}

/*-----------------------------------------------------------*
函数：IsInMenu;
功能：判断某一关键字是否在菜单里
参数：pMenu:被查找的菜单
      sIndex:被查找的字符串
      nType:方式，1：匹配菜单项，0：匹配附加项

返回：0：没找到，>0已经找到

*------------------------------------------------------------*/

long IsInMenu(MENU * pMenu,char * sIndex,long nType)
{

    long nRec=1;
    MENUITEM * pItem;
    char sMenuItem[61];

    if(pMenu==NULL || sIndex==NULL) return 0;
    if(pMenu->nTotal<=0) return 0;
    pItem=pMenu->pPopMenu;
    while(pItem)
    {
    	if(nType)
    	{
    		strcpy(sMenuItem,pItem->sSubItem+4);
    		alltrim(sMenuItem);
    		if(!strcmp(sIndex,sMenuItem)) return nRec;
    	}
    	else
    	{
                if(!strcmp(sIndex,pItem->sAddition)) return nRec;
        }
    	
    	nRec++;
    	pItem=pItem->pNext;
    }
    return 0;

}

/*----------------------------------------------------------------

函数:long getchoice;[内部函数]
目的:接收用户选择[数字键]
参数:nStyle=0,回显，否则不回显
返回:功能键

*-----------------------------------------------------------------*/
static long getchoice(long * nChoose,long nStyle,long nMaxNo)
{

    long nKey,nchoice;

    nchoice=0;
    for(;;)
    {
    	nKey=keyb();

        if(nKey==KEY_BACKSPACE) nchoice/=10;

    	if(nKey=='0' && nchoice==0)
    	{
    	   *nChoose=0;
    	   return KEY_RETURN;
    	}
        if(nKey==KEY_RETURN && !nStyle)
        {
            (*nChoose)=nchoice;
            if(nchoice>=1 && nchoice<=nMaxNo) return KEY_RETURN;
            continue;
        }

    	if(nKey<'0'||nKey>'9')  return nKey;
    	nchoice=nchoice*10+nKey-'0';
        if(nchoice>nMaxNo) nchoice=0;
        (*nChoose)=nchoice;

        if(!nStyle)
        {
            gotoxy(44,22);
            printf("%-ld",nchoice);
        }
        if(nchoice<=nMaxNo/10 || nchoice<=nMaxNo/100) continue;
        if(nchoice>=1 && nchoice<=nMaxNo) return KEY_RETURN;

    }//end for
}




/*-----------------------------------------------------------------

函数:long GetValue;
目的:按要求从键盘上输入值
参数:nPosx:屏幕横标，nPosy:屏幕纵坐标
     sFormat：对值类型及长度格式要求及提示语
     ...:具体参数
返回:返回功能键

*-----------------------------------------------------------------*/

long GetValue(long nPosx,long nPosy,char * sFormat,...)
{
    char * sTemp,* sOut;
    INPUTSTRUCT input;
    va_list pVar;
    long nKey;
    long * pLong;
    double * pDouble;
    char   * pString;


    if(nPosx>SCREEN_WIDTH || nPosx<0 || nPosy>SCREEN_LINE || nPosy<1) return -1;
    input.y=nPosy;
    input.nLength=0;
    sOut=strchr(sFormat,'%');
    va_start(pVar,sFormat);
    sTemp=sOut+1;
    input.x=nPosx+strchr(sFormat,'%')-sFormat;
    while((sTemp[0]>='0' && sTemp[0]<='9')||sTemp[0]=='-')
    {
        if(sTemp[0]!='-')
        {
            input.nLength*=10;
            input.nLength+=(sTemp[0]-'0');
        }
        sTemp++;
    }

    gotoxy(nPosx,nPosy);
    while(sFormat[0]!='%')
    {
        outtext("%c",sFormat[0]);
        sFormat++;
    }

    if(input.nLength<=0)
    {
        va_end(pVar);
        return -1;
    }

    while(!strchr("dsfp",sTemp[0])) sTemp++;
    switch(sTemp[0])
    {
        case 'd':
          pLong=va_arg(pVar,long *);
          sprintf(input.Value,sOut,* pLong);
          input.Value[input.nLength]='\0';
          nKey=GetValueByType(&input,TYPE_LONG);
          if(nKey==KEY_RETURN)  * pLong=atol(input.Value);
          sprintf(input.Value,sOut,* pLong);
          break;

        case 'f':
          pDouble=va_arg(pVar,double *);
          sprintf(input.Value,sOut,* pDouble);
          input.Value[input.nLength]='\0';
          nKey=GetValueByType(&input,TYPE_DOUBLE);
          if(nKey==KEY_RETURN)  * pDouble=atof(input.Value);
          sprintf(input.Value,sOut,* pDouble);
          break;

        case 's':
          pString=va_arg(pVar,char *);
          memset(input.Value,'\0',INPUT_STRING_SIZE);
          memcpy(input.Value,pString,input.nLength);
          nKey=GetValueByType(&input,TYPE_STRING);
          if(nKey==KEY_RETURN)  memcpy(pString,input.Value,input.nLength);
          memcpy(input.Value,pString,input.nLength);
          pString[input.nLength]='\0';
          break;

        case 'p':
          pString=va_arg(pVar,char *);
          memset(input.Value,'\0',sizeof(input.Value));
          nKey=GetValueByType(&input,TYPE_PASSWORD);
          if(nKey==KEY_RETURN)  memcpy(pString,input.Value,input.nLength);
          pString[input.nLength]='\0';
          break;

        default:
          nKey=-1;
          break;
     }
     va_end(pVar);
     if(sTemp[0]!='p')
     {
        gotoxy(input.x,input.y);
        outtext(input.Value);
     }   
     return nKey;
}

/*-----------------------------------------------------------------

函数:long GetMultiValue;
目的:获得多种输入
参数:nPosx:屏幕横标，nPosy:屏幕纵坐标
     nIndex:当前输入条件序号（从1开始）
     sFormat：对值类型及长度格式要求及提示语，每个提示语之间由分号隔开
     ...:具体参数
返回:返回功能键。若输入为TAB键则表示切换输入条件，不返回
     nIndex:实际输入的条件序号

例子：
      
     long nIndex=1;
     
     GetMultiValue(20,10,&nIndex,
     "输入长整型:%-8d;输入浮点型:%10.2f;输入字符串:%20s;",
     &nValue,&dValue,sValue);

*-----------------------------------------------------------------*/
long GetMultiValue(long npx,long npy,long * nIndex,char * sFormat,...)
{
    long nCurrent,nLoop,nFind,nKey,address,nRecycle;
    char sTitle[81];
    va_list pvar;
    char * offset;
    
    nCurrent=*nIndex;
    for(;;)
    {

    	nFind=nCurrent;
    	offset=sFormat;
    	nRecycle=0;
    	
    	va_start(pvar,sFormat);
    	address=va_arg(pvar,long);
    	while(--nFind)
    	{
    	    while(offset[0] && offset[0]!=';') offset++;
    	    if(!strchr(offset,'%'))
    	    {
    	    	nRecycle=1;
    	    	break;
    	    }
      	    address=va_arg(pvar,long);
      	    offset++;
    	}
    	va_end(pvar);
    	if(nRecycle==1)
    	{
            nCurrent=1;
            continue;
        }

    	nLoop=0;
    	memset(sTitle,'\0',sizeof(sTitle));
    	while(offset[0] && offset[0]!=';') sTitle[nLoop++]=offset++[0];

        
        (* nIndex)=nCurrent;
        nKey=GetValue(npx,npy,sTitle,address);
        if(nKey==KEY_RETURN) return KEY_RETURN;
        if(nKey==KEY_TAB)
        {
            nCurrent++;
            continue;
        }
        return nKey;
    }
}



/*-----------------------------------------------------------------

函数:long GetValueByType;
目的:按要求从键盘上输入值
参数:
     input:输入区结构
     type :输入字符的类型
返回:返回功能键

*-----------------------------------------------------------------*/
long GetValueByType(INPUTSTRUCT * input,long type)
{
     long once,offset,nInsert,nQuit,move,valid;
     unsigned long nKey;
     char buf[INPUT_STRING_SIZE];

     strncpy(input->OldValue,input->Value,INPUT_STRING_SIZE-1);
     memset(input->Value,'\0',INPUT_STRING_SIZE);
     memset(input->Value,' ',input->nLength+1);
     strncpy(input->Value,input->OldValue,input->nLength);
     for(offset=0;offset<input->nLength;offset++)
        if((unsigned char)input->Value[offset]<=' ')
              input->Value[offset]=' ';
     input->Value[input->nLength]='\0';

     ReverseChar();
     once=0;
     offset=0;
     nInsert=0;
     nQuit=0;

     for(;;)
     {
         if(offset>=input->nLength||nQuit==1) break;
         gotoxy(input->x,input->y);
         if(type!=TYPE_PASSWORD) outtext(input->Value);
         else
         {
         	strcpy(buf,input->Value);
         	for(move=0;move<input->nLength;move++)
         	      if(buf[move]!=' ') buf[move]='*';
         	outtext(buf);
         }
         gotoxy(input->x+offset,input->y);
         nKey=keyb();
         switch(nKey)
         {
             case KEY_RIGHT:
                  offset++;
                  if(offset>input->nLength) nQuit=1;
                  once=1;
                  continue;


             case KEY_LEFT:
                  offset--;
                  if(offset<0) nQuit=1;
                  once=1;
                  continue;

             case KEY_INSERT:
                  nInsert^=1;
                  continue;

             case KEY_BACKSPACE:
                  if(once==1)
                  {
                    if(offset<1) continue;
                    input->Modified=1;
                  	for(move=offset-1;move<input->nLength;move++)
                  	    input->Value[move]=input->Value[move+1];
                    input->Value[input->nLength-1]=' ';
                    offset--;
                    continue;
                  }

             case KEY_DELETE:
                  if(once==0)
                  {
                  	memset(input->Value,' ',input->nLength);
                  }
                  else
                  {
                  	input->Modified=1;
                  	for(move=offset;move<input->nLength;move++)
                  	    input->Value[move]=input->Value[move+1];
                  	input->Value[input->nLength-1]=' ';
                  }
                  once=1;
                  continue;


             case KEY_END:
                  offset=input->nLength-1;
                  once=1;
                  continue;

             case KEY_HOME:
                  offset=0;
                  once=1;
                  continue;

         }

         if(nKey<0x20 || nKey>0xFF)
         {
            nQuit=1;
            continue;
         }

         valid=1;
         if(type==TYPE_DOUBLE || type==TYPE_LONG)
         {
        	if(nKey<'0' || nKey>'9')
         	{
         	    if(nKey=='.')
         	    {
         	       if(strchr(input->Value,'.')||type==TYPE_LONG) valid=0;
         	    }
         	    else if(nKey=='-')
         	    {
         	      	if(strchr(input->Value,'-') && once==1) valid=0;
         	      	for(move=0;move<=offset;move++)
         	      	{
         	      	   if(input->Value[move]!=' ' && once==1)
         	      	   {
         	      	   	valid=0;
         	      	   	break;
         	      	   }
         	      	}
         	    }
         	    else valid=0;
         	}
         }

         if(!valid) continue;
         
         input->Modified=1;
         if(once==0)
         {
             memset(input->Value,' ',input->nLength);
             input->Value[0]=(unsigned char)nKey;
             offset++;
             once=1;
             nKey=KEY_RETURN;
             continue;
         }
         else
         {
             if(nInsert)
               for(move=input->nLength;move>offset;move--)
                 	input->Value[move]=input->Value[move-1];

             input->Value[offset]=(unsigned char)nKey;
             input->Value[input->nLength]='\0';
             offset++;
             once=1;
             nKey=KEY_RETURN;
         }

     }//end for

     ShowChar();
     input->Value[input->nLength]='\0';
     gotoxy(input->x,input->y);
     if(type!=TYPE_PASSWORD) outtext(input->Value);
     else
     {
         strcpy(buf,input->Value);
         for(move=0;move<input->nLength;move++)
               if(buf[move]!=' ') buf[move]='*';
         outtext(buf);
     }
     return nKey;
}
/*--------------------------------------------------------------
函数：long ScanKey
目的：测试键盘是否被按下
参数：msec:停留的毫秒数 [千分之一秒]
      若msec=0则表示一直等待

返回：键值      
*-------------------------------------------------------------*/

long ScanKey(long msec)
{
     
     #ifdef UNIX
     fd_set rfds;
     struct timeval waittime;
     long nRetVal=0;

     FD_ZERO(&rfds);
     FD_SET(0,&rfds);
     waittime.tv_sec=0;
     waittime.tv_usec=msec*1000;
     select(1,&rfds,(fd_set *)0,(fd_set *)0,&waittime);
     if(FD_ISSET(0,&rfds))
     	read(0,(char*)&nRetVal,4);
     
     return nRetVal;
     #else
     
     if(kbhit()) return 1;
     return 0;
     #endif;
}

/*-----------------------------------------------------------------
函数:long InputString;
目的:长字符串的输入
参数:nPosx:屏幕横标
     nPosy:屏幕纵坐标
     sMention：提示语;
     nLen:最大长度
     sValue:回带值

返回:返回功能键
注意:sizeof(sValue)>=nLen;

*-----------------------------------------------------------------*/

long InputString(long nPosx,long nPosy,char * sMention,long nLen,char * sValue)
{

     long nWidth,nLine,nCurLine,nKey;
     TEXTBOX inp;
     char sOutput[80];

     if(nPosx<0||nPosx>strlen(sMention)+70||nPosy<0||
                 nPosy>20||strlen(sMention)>50)	return KEY_ESC;

     nWidth=SCREEN_WIDTH-(nPosx+strlen(sMention))-6;
     nWidth-=(nWidth%2);
     nLine=nLen/nWidth+(nLen%nWidth!=0);


     gotoxy(nPosx,nPosy);
     outtext(sMention);
     drawbox(nPosx+strlen(sMention),nPosy,SCREEN_WIDTH-6,nPosy+nLine+1);

     for(nCurLine=1;nCurLine<=nLine;nCurLine++)
     {
     	memset(sOutput,0,80);
     	memcpy(sOutput,sValue+(nCurLine-1)*nWidth,nWidth);
     	gotoxy(nPosx+strlen(sMention)+2,nPosy+nCurLine);
     	outtext(sOutput);
     }

     nCurLine=1;
     inp.x=nPosx+strlen(sMention)+2;
     for(;;)
     {
     	inp.y=nPosy+nCurLine;
     	if(nCurLine==nLine) inp.nLen=nLen%nWidth;
     	else inp.nLen=nWidth;

     	memset(inp.sValue,0,sizeof(inp.sValue));
     	memcpy(inp.sValue,sValue+(nCurLine-1)*nWidth,nWidth);
     	nKey=getstring(&inp);
     	alltrim(inp.sValue);
     	memcpy(sValue+(nCurLine-1)*nWidth,inp.sValue,nWidth);
        switch(nKey)
        {
            case KEY_UP:
        	 if(nCurLine<=1) return KEY_UP;
        	 nCurLine--;
        	 break;

            case KEY_DOWN:
        	 if(nCurLine>=nLine) return KEY_DOWN;
        	 nCurLine++;
        	 break;

            case KEY_RETURN:
                 if(strlen(inp.sValue)==inp.nLen && nCurLine<nLine)
                 {
                 	nCurLine++;
                 	break;
                 }
                 else return KEY_RETURN;

            default:return nKey;
        }

     }//end for

}


/*-----------------------------------------------------------------
函数:IncBufSize
目的:分配或更改一个malloc分配的缓冲区大小
参数:
     buf:原来缓冲区的首指针
     size:原来缓冲区的长度（字节）
     increment:新缓冲区的增量
返回:NULL失败，否则返回一个指向新缓冲区的指针
说明：
     如果buf=NULL,则表示第一次分配increment大小的缓冲区
*-----------------------------------------------------------------*/
void * IncBufSize(void * buf,long size,long increment)
{
     void * newbuf;

     if(increment<=0) return buf;
     if(!buf) return malloc(increment);

     newbuf=malloc(size+increment);
     if(!newbuf) return NULL;

     memset(newbuf,0,size+increment);
     memcpy(newbuf,buf,size);
     free(buf);
     return newbuf;

}

/*-----------------------------------------------------------------

函数:TABLE * CreateTable;
目的:建立一个表格对象
参数:nLine:表格行数，sDescribe:表格列信息描述
返回:NULL失败，否则返回一个指向该表格对象的指针

*-----------------------------------------------------------------*/
TABLE * CreateTable(long nLine,char * sDescribe)
{

  TABLE * pt;
  char sHead[2000],sLine[2000],sCross[2000],sLast[2000];
  long offset,nTotal,nSize,nSumSize,draw;
  
  if(nLine>MAX_TABLE_LINE || nLine<1)
  {
        outtext("\n表格行数应在1至%d之内！",MAX_TABLE_LINE);
        keyb();
        return NULL;
  }

  offset=0;
  nTotal=0;
  while(sDescribe[offset])
  {
      if(sDescribe[offset]=='|')
      {
          nTotal++;
          if(offset%2==nTotal%2)
          {
              MessageBox("表格格式字符串中，每一列的宽度应为偶数，以免造成表格线错位！",1);
              return NULL;
          }
      }    
      offset++;
  }

  ////////////////////////////////////////////////////////////////////
  offset=0;
  draw=0;
  nSize=GetTableWidth(sDescribe);
  memset(sHead,'\0',sizeof(sHead));
  memset(sLine,'\0',sizeof(sLine));
  memset(sCross,'\0',sizeof(sCross));
  memset(sLast,'\0',sizeof(sLast));
  while(sDescribe[offset]!='|') offset++;

  while(sDescribe[offset])
  {
      if(sDescribe[offset]=='|')
      {
          nTotal--;
      	  if(draw==0)
      	  {
      	     strcpy(sHead,TOP_LEFT);
      	     strcpy(sLine,VERTIC_BIG);
      	     strcpy(sCross,VER_RIGHT);
      	     strcpy(sLast,BOT_LEFT);
      	  }
      	  else if(nTotal==0)
      	  {
      	     strcpy(sHead+draw,TOP_RIGHT);
      	     strcpy(sLine+draw,VERTIC_BIG);
      	     strcpy(sCross+draw,VER_LEFT);
      	     strcpy(sLast+draw,BOT_RIGHT);
      	     break;
      	  }   
      	  else
      	  {
      	     strcpy(sHead+draw,HOR_DOWN);
      	     strcpy(sLine+draw,VERTIC_SMA);
      	     strcpy(sCross+draw,CROSS);
      	     strcpy(sLast+draw,HOR_UP);
      	  }
      	  offset++;
      }//end if
      else 
      {
     	  strcpy(sHead+draw,HORZEN_BIG);
    	  strcpy(sLine+draw,"  ");
      	  strcpy(sCross+draw,HORZEN_SMA);
      	  strcpy(sLast+draw,HORZEN_BIG);
          offset+=2; 	   
      }
      draw+=2;
  }
  sHead[nSize-1]='\n';
  sLine[nSize-1]='\n';
  sCross[nSize-1]='\n';
  sLast[nSize-1]='\n';


  ////////////////////////////////////////////////////////////////

  nSumSize=nSize*nLine*2+nSize+1;
  pt=(TABLE *)malloc(sizeof(TABLE));
  memset(pt,0,sizeof(TABLE));
  pt->sImage=(char *)malloc(nSumSize);
  memset(pt->sImage,'\0',nSumSize);
  strcpy(pt->sDescribe,sDescribe);
  pt->nLine=nLine;
  pt->npy=-1;
  pt->npx=-1;
  pt->pNext=NULL;
  pt->nVisual=0;

  ////////////////////////////////////////////////////////////////
 
  memcpy(pt->sImage,sHead,nSize);
  offset=nSize;
  
  for(draw=0;draw<nLine;draw++)
  {
     offset=nSize+draw*nSize*2;
     memcpy(pt->sImage+offset,sLine,nSize);
     if(draw==nLine-1)
         memcpy(pt->sImage+offset+nSize,sLast,nSize);
     else         
         memcpy(pt->sImage+offset+nSize,sCross,nSize);
  }       
  
  offset=0;
  draw=0;
  while(sDescribe[offset]!='|') offset++;
  while(sDescribe[offset])
  {
     if(sDescribe[offset]=='|')
     {
     	if(draw==0) strcpy(sHead,VERTIC_BIG);
     	else        strcpy(sHead+draw,VERTIC_SMA);
     	draw+=2;
     }
     else
     {
     	sHead[draw]=sDescribe[offset];
     	draw++;
     }
     offset++;
  }
  memcpy(pt->sImage+nSize,sHead,nSize-3);	
  return pt;   	
 
} //end create

/*-----------------------------------------------------------------

函数:long ShowTable;
目的:显示一个表格
参数:pTable:表格对象指针，(npx,npy):表格左上角屏幕坐标
返回:<0失败,>0成功

*-----------------------------------------------------------------*/

long ShowTable(TABLE * pTable,long npx,long npy)
{
   long nwidth;
   long  nposx,nposy,draw,line;
   char sOutput[2000];
   TABLE * pSearch;

   if(pTable==NULL) return -1;
   nwidth=GetTableWidth(pTable->sDescribe);

   if(npx>SCREEN_WIDTH||npy>SCREEN_LINE||
      (npx+nwidth<=0)||(npy+pTable->nLine*2)<=0) return 0;

   if(nwidth>=SCREEN_WIDTH) npx+=npx%2;
   pTable->npy=npy;
   pTable->npx=npx;
   pTable->nVisual=1;
   nposy=npy-1;
   nposx=npx;

   pSearch=pTable;
   while(pSearch->pNext!=NULL)
   {
        pSearch->pNext->npy=pSearch->npy+pSearch->nLine*2;
        pSearch->pNext->npx=npx;
        pSearch=pSearch->pNext;
        pSearch->nVisual=1;
   }


   #ifdef UNIX
   if(npx<0) npx=0;
   #else
   if(npx<=0) npx=1;
   #endif
   if(npy<=0) npy=1;

   for(;;)
   {
       line=pTable->nLine*2+(pTable->pNext==NULL);

       for(draw=0;draw<line;draw++)
       {

            nposy++;
            if(nposy<1||nposy>SCREEN_LINE) continue;
            memset(sOutput,'\0',sizeof(sOutput));
            memcpy(sOutput,pTable->sImage+draw*nwidth,nwidth-1);

            if(nposx<0)
            {
                memcpy(sOutput,sOutput-nposx,nwidth+nposx);
                sOutput[nwidth+nposx]='\0';
            }
            else sOutput[SCREEN_WIDTH-nposx]='\0';
            sOutput[SCREEN_WIDTH]='\0';

            gotoxy(npx,npy);
            outtext(sOutput);
            npy++;

       }
       if(pTable->pNext==NULL) break;
       pTable=pTable->pNext;
   }
   fflush(stdout);
   return 1;

}



/*-----------------------------------------------------------------

函数:long FillTable;
目的:向表格的某一行填定数据
参数:pTable:表格对象指针  nLine:要填写的行(最小为1)
     sFormat:表格列的格式信息    ...跟要填写的参数
返回:当前填写的最大行数

*-----------------------------------------------------------------*/
long FillTable(TABLE * pTable,long nLine,char * sFormat,...)
{
  va_list  pVar;
  char * sTemp,* sFill;
  char   sOutput[2000],* sRecycle;
  char   sValue[30];
  long   nWhere=1,nposx=0,nposy=0;
  long   nLnRec=0,nRec,nCol=-1;



  if(pTable==NULL||nLine<1||(!strchr(sFormat,'%'))) return -1;
  nLnRec=nLine;

  while(pTable && nLine>pTable->nLine)
  {
      nLine-=pTable->nLine;
      pTable=pTable->pNext;
  }

  if(!pTable) return -2;

  sTemp=pTable->sDescribe;
  while(sTemp[0])
  {
      nCol+=(sTemp[0]=='|');
      sTemp++;
  }

  va_start(pVar,sFormat);
  while(sFormat[0])
  {
     if(nWhere>nCol) break;  //对于多余的列则不填写
     memset(sValue,'\0',sizeof(sValue));
     switch(sFormat[0])
     {
        case '>':            //跳格
          nWhere++;
          sFormat++;
          break;

        case '%':            //输出到表格
          sFormat++;
          sValue[0]='%';
          nposx=1;
          for(;;)
          {
              if(sFormat[0]=='>' || sFormat[0]=='%' || !sFormat[0]) break;
              sValue[nposx]=sFormat[0];
              sFormat++;
              nposx++;
          }
          alltrim(sValue);

          sTemp=sValue;
          while(!strchr("sdf",sTemp[0])) sTemp++;
          switch(sTemp[0])
          {
              case 's':
                sFill=va_arg(pVar,char *);
                if(strlen(sFill)>=GetTableWidth(pTable->sDescribe)*10) break;
                sprintf(sOutput,sValue,sFill);

                FormatString(pTable,nLine,nWhere,sOutput);

                if(!strchr(sOutput,'\n')) break;
                //////////对字符串换行的处理//////////////////
                sFill=sOutput;
                sRecycle=sOutput;

                memset(sValue,'\0',sizeof(sValue));
                memset(sValue,'>',nWhere-1);
                strcat(sValue,"%s");

                nRec=nLine;
                for(;;)
                {
                     ClearTableBox(pTable,nWhere,nRec,2);
                     while(sFill[0]!='\n' && sFill[0]) sFill++;
                     sFill[0]='\0';
                     sFill++;
                     FillTable(pTable,nRec,sValue,sRecycle); //单级递归调用
                     sRecycle=sFill;
                     if(!sFill[0]) break;
                     nRec++;
                }
                if(nRec>nLnRec) nLnRec=nRec;
                break;
              case 'd':
                sprintf(sOutput,sValue,va_arg(pVar,long));
                break;
              case 'f':
                sprintf(sOutput,sValue,va_arg(pVar,double));
                break;
              default:
                return -1;
          }//end switch

          sTemp=pTable->sDescribe;
          nposx=nWhere;
          while(nposx)
          {
               if(sTemp[0]=='|') nposx--;
               sTemp++;
          }
          nposx=(sTemp-(pTable->sDescribe)+nWhere);
          nposy=(nLine-1)*GetTableWidth(pTable->sDescribe)*2+GetTableWidth(pTable->sDescribe);
          memcpy(pTable->sImage+nposx+nposy,sOutput,strlen(sOutput));
          if(pTable->nVisual) ShowTableLine(pTable,nLine);
          nWhere++;
          break;

        default:
          sFormat++;
          break;
     }//end switch
  }//end while
  va_end(pVar);
  return nLnRec;
}

/*-----------------------------------------------------------------

函数:long FetchTable;
目的：从指表格的指定行中由键盘键入数据
参数:pTable:表格对象指针  nLine:要填写的行(最小为1)
     sFormat:表格列的格式信息   nWhere表示目前操作的是第几列
     ...:跟要取得的参数
返回:功能键
*-----------------------------------------------------------------*/


long FetchTable(TABLE * pTable,long nLine,long nWhere,char * sFormat,...)
{
   long nPos,nKey,nRow,nCol,nAddx,nAddy,nLen,pArg,nClean;
   char * sTemp,* sCompute;
   char sValue[30],sOutput[256];
   TABLE * pt;
   va_list pVar;

   if(!pTable || !strchr(sFormat,'%') || nWhere<1) return -1;

   while(pTable && nLine>pTable->nLine)
   {
        nLine-=pTable->nLine;
        pTable=pTable->pNext;
   }
   if(!pTable) return -1;


   sTemp=sFormat;
   nCol=nWhere-1;

   va_start(pVar,sFormat);
   while(sTemp[0] && nCol)
   {
       if(sTemp[0]=='>') nCol--;
       if(sTemp[0]=='%')
       {
           va_arg(pVar,long);
           nCol--;
       }
       sTemp++;
   }
   if(nCol || !sTemp[0]) return -2;

   while(sTemp[0] && sTemp[0]!='%' && sTemp[0]!='>') sTemp++;
   if(sTemp[0]!='%') return -3;

   nKey=0;
   nLen=0;
   nPos=0;

   while(sTemp[0] && !strchr("sdf",sTemp[0]))
   {
       if(nKey==0 && sTemp[0]<='9' && sTemp[0]>='0') nLen=nLen*10+sTemp[0]-'0';
       if(sTemp[0]=='.') nKey=1;
       sValue[nPos]=sTemp[0];
       nPos++;
       sTemp++;
   }

   if(!sTemp[0]) return -4;

   sValue[nPos]=sTemp[0];
   sValue[nPos+1]='\0';

   sCompute=pTable->sDescribe;
   nCol=nWhere;
   while(sCompute[0] && nCol)
   {
   	if(sCompute[0]=='|') nCol--;
   	sCompute++;
   }
   if(!sCompute[0] || nCol) return -5;

   nCol=pTable->npx+(sCompute-(pTable->sDescribe))+nWhere;
   nRow=pTable->npy+(nLine-1)*2+1;
   nAddx=40*((nCol<1)-((nCol+nLen)>=SCREEN_WIDTH));
   nAddy=10*((nRow<1)-(nRow>SCREEN_LINE));

   if(nAddx || nAddy)
   {
        nClean=0;
        pt=pTable;
        while(pt)
        {
           nClean+=pt->nLine*2+1;
           pt=pt->pNext;
        }
        clearline2(pTable->npy,pTable->npy+nClean);
   	ShowTable(pTable,pTable->npx+nAddx,pTable->npy+nAddy);
   	nRow+=nAddy;
   	nCol+=nAddx;
   }

   if(pTable->npx<0) nCol++;
   pArg=va_arg(pVar,long);
   va_end(pVar);
   nKey=GetValue(nCol,nRow,sValue,pArg);
   memset(sOutput,'\0',sizeof(sOutput));
   memset(sOutput,'>',nWhere-1);
   strcat(sOutput,sValue);
   if(strchr(sValue,'s'))
       FillTable(pTable,nLine,sOutput,pArg);
   else if(strchr(sValue,'d'))
       FillTable(pTable,nLine,sOutput,*((long*)pArg));
   if(strchr(sValue,'f'))
       FillTable(pTable,nLine,sOutput,*((double *)pArg));
   return nKey;
}



/*-----------------------------------------------------------------

函数:void ClearTableLine;
目的:清除菜单某一行的内容
参数:pTable:表格对象指针,nLine:指定行
返回:无
*-----------------------------------------------------------------*/
void ClearTableLine(TABLE * pTable,long nLine)
{
   char * sBegin,* pSearch,* sMemory;
   long   nWidth,nWhere;

   if(pTable==NULL||nLine<1) return;
   while(pTable!=NULL)
   {
       if(nLine>pTable->nLine)
       {
            nLine-=pTable->nLine;
            pTable=pTable->pNext;
            continue;
       }
       nWidth=GetTableWidth(pTable->sDescribe);
       sBegin=pTable->sImage+(nLine-1)*nWidth*2+nWidth;
       memset(sBegin+2,' ',nWidth-5);

       pSearch=pTable->sDescribe+1;
       nWhere=0;
       for(;;)
       {
            if(strchr(strchr(pSearch,'|')+1,'|')==NULL) break;
            pSearch=strchr(pSearch,'|');
            nWhere++;
            sMemory=(sBegin+(pSearch-pTable->sDescribe));
            memcpy(sMemory+nWhere,VERTIC_SMA,2);
            pSearch++;
       }

       ShowTableLine(pTable,nLine);
       break;
   } //end while

}

/*-----------------------------------------------------------------

函数:long OutputTable;
目的:将表格输出到打印机
参数:pTable：已建立并填定完成表格
返回:>0成功   <0失败

*-----------------------------------------------------------------*/
long OutputTable(TABLE * pTable)
{
    long nLine=0,nLnRec=0,nPrint;
    long nWidth,nOffset;
    char sWork[2000];

    if(pTable==NULL) return -1;
    nWidth=GetTableWidth(pTable->sDescribe);

    setprinton();
    nOffset=0;

    while(pTable!=NULL)
    {
        nPrint=pTable->nLine;
        while(nPrint--)
        {
             memcpy(sWork,pTable->sImage+nOffset,nWidth);
             sWork[nWidth]='\0';
             output(sWork);
             nOffset+=nWidth;

             memcpy(sWork,pTable->sImage+nOffset,nWidth);
             sWork[nWidth]='\0';
             output(sWork);
             nOffset+=nWidth;
         }//end while

         if(pTable->pNext==NULL)  //the last line of the table
         {
             memcpy(sWork,pTable->sImage+nOffset,nWidth);
             sWork[nWidth]='\0';
             output(sWork);
         }
         pTable=pTable->pNext;
         nOffset=0;

    }// end while
    setprintoff();
    return 1;
}

/*-----------------------------------------------------------------

函数:long MergeTable;
目的:合并两个表
参数:pObject目标表，pSource:源表
返回:>0成功   <0失败

*-----------------------------------------------------------------*/
long MergeTable(TABLE * pObject,TABLE * pSource)
{
   long nSize;
   char * sObj,* sSou;

   if(pObject==NULL||pSource==NULL)
   {
         MessageBox("只有两个已经建立成功的表才能被合并！",1);
         return -1;
   }

   if(GetTableWidth(pObject->sDescribe)!=GetTableWidth(pSource->sDescribe))
   {
         MessageBox("被合并的两个表的总宽度不一致，注意：在实际表格中，一个'|'代表两个字符！",1);
         return -1;
   }

   while(pObject->pNext!=NULL) pObject=pObject->pNext;
   pObject->pNext=pSource;

   nSize=pObject->nLine*GetTableWidth(pObject->sDescribe)*2;

   sObj=pObject->sImage+nSize;
   sSou=pSource->sImage;

   memcpy(sSou,VER_RIGHT,2);
   memcpy(sSou+GetTableWidth(pSource->sDescribe)-3,VER_LEFT,2);

   while(sObj[0]!='\n')
   {
        if(strstr(sObj,HOR_UP)==sObj)
        {
                if(strstr(sSou,HOR_DOWN)==sSou) memcpy(sSou,CROSS,2);
                else memcpy(sSou,HOR_UP_SMA,2);
        }
        if(strstr(sObj,HORZEN_BIG)==sObj)
        {
                if(strstr(sSou,HOR_DOWN)==sSou) memcpy(sSou,HOR_DOWN_SMA,2);
                else memcpy(sSou,HORZEN_SMA,2);
        }
        sObj+=2;
        sSou+=2;
   }
   pObject->sImage[nSize]='\0';
   return 1;
}


/*-----------------------------------------------------------------

函数:void DropTable;
目的:释放一个表格资源
参数:pTable:表格对象指针
返回:无

*-----------------------------------------------------------------*/

void DropTable(TABLE * pTable)
{
   TABLE * pTabRm;


   if(pTable==NULL) return;
   while(pTable!=NULL)
   {
     pTabRm=pTable->pNext;
     if(pTable->sImage!=NULL) free(pTable->sImage);
     free(pTable);
     pTable=pTabRm;
   }
}

/*-----------------------------------------------------------------

函数:void ClearTableBox;[内部函数]
目的:将表格pTable的第nCol列的第npyLine行至nCount行的横向表格线清除
参数:pTable:表格对象指针
返回:无

*-----------------------------------------------------------------*/


static void ClearTableBox(TABLE * pTable,long nCol,long nLine,long nCount)
{

   char * sImage,* sFill,* sDesc;
   long nRec;

   if(pTable==NULL||nCol<1||nLine<1||nCount<2) return;

   while(pTable!=NULL && pTable->nLine<nLine)
   {
        nLine-=pTable->nLine;
        pTable=pTable->pNext;
   }
   nRec=nCount;
   if(pTable==NULL || (nLine+nCount-1)>pTable->nLine) return;
   sImage=pTable->sImage+GetTableWidth(pTable->sDescribe)*nLine*2;
   sFill=sImage;


   sImage+=nCol;
   sDesc=pTable->sDescribe;
   while(nCol--) sDesc=strchr(sDesc,'|')+1;
   if(sDesc==NULL) return;
   sImage=sImage+(sDesc-(pTable->sDescribe));
   sImage=sImage-2;

   if((sImage-sFill)>=GetTableWidth(pTable->sDescribe)) return;

   while(--nRec)
   {
        sFill=sImage;
        if(strstr(sFill,VER_RIGHT)==sFill) memcpy(sFill,VERTIC_BIG,2);
        if(strstr(sFill,CROSS)==sFill) memcpy(sFill,VER_LEFT_SMA,2);
        if(strstr(sFill,VER_RIGHT_SMA)==sFill)  memcpy(sFill,VERTIC_SMA,2);
        sFill+=2;
        while(strstr(sFill,HORZEN_SMA)==sFill)
        {
                memcpy(sFill,"  ",2);
                sFill+=2;
        }
        if(strstr(sFill,VER_LEFT)==sFill) memcpy(sFill,VERTIC_BIG,2);
        if(strstr(sFill,CROSS)==sFill) memcpy(sFill,VER_RIGHT_SMA,2);
        if(strstr(sFill,VER_LEFT_SMA)==sFill)  memcpy(sFill,VERTIC_SMA,2);

        sImage+=GetTableWidth(pTable->sDescribe)*2;
   }

}


/*-----------------------------------------------------------------

函数:long GetTableWidth;[内部函数]
目的:得到表格的宽度
参数:char * sDescribe:表格列信息描述
返回:返回表格宽度
*-----------------------------------------------------------------*/


long GetTableWidth(char * sDescribe)
{
   char * pSearch;
   long nWidth=0;

   pSearch=sDescribe;
   while(pSearch[0])
   {
        nWidth+=(pSearch[0]=='|');
        pSearch++;
   }
   nWidth+=((pSearch-sDescribe)+1);
   return nWidth;
}

/*-----------------------------------------------------------------

函数:long ShowTableLine;[内部函数]
目的:显示表格中某一行的数据到屏幕上
参数:pTable:表格对象指针,nLine:哪一行
返回:<0失败    >0成功

*-----------------------------------------------------------------*/

static long ShowTableLine(TABLE * pTable,long nLine)
{
  char sShow[2000];
  long nposx,nposy,nWidth;
  char * sMemory;



  if(pTable==NULL||pTable->nLine<nLine||nLine<1||pTable->npy<0) return -1;
  if(pTable->nVisual==0) return 0;
  if((nWidth=GetTableWidth(pTable->sDescribe))<2) return -1;
  if((nposy=pTable->npy+(nLine-1)*2+1)>SCREEN_LINE) return -1;
  nposx=pTable->npx;

  sMemory=pTable->sImage+(nLine-1)*nWidth*2+nWidth;
  memset(sShow,'\0',sizeof(sShow));
  memcpy(sShow,sMemory,nWidth-1);

  if(nposx<0)
  {
        memcpy(sShow,sShow-nposx,nWidth+nposx);
        sShow[nWidth+nposx]='\0';
  }
  if(nposy>0)
        sShow[SCREEN_WIDTH-1-nposx]='\0';

  sShow[SCREEN_WIDTH-1]='\0';
  #ifdef UNIX
  gotoxy((nposx>0)?nposx:0,nposy);
  #else
  gotoxy((nposx>0)?nposx:1,nposy);
  #endif

  outtext(sShow);
  return 1;

}
/*-----------------------------------------------------------------

函数:long FormatString;[内部函数]
目的:将字符串sOutput格式化成pTable,第nLine行，nWhere列所需要的宽度
参数:pTable:表格对象指针,nLine:哪一行，nWhere:哪一列，sOutput：原始字符串
返回:返回被格式化后的行数

*-----------------------------------------------------------------*/
long FormatString(TABLE * pTable,long nLine,long nWhere,char * sOutput)
{

    char * sImage,* sPos;
    long nWidth,nRec,nCount;
    long nch;

    if(pTable==NULL||nLine<1||nWhere<1||strlen(sOutput)<2) return -1;
    while(pTable!=NULL && pTable->nLine<nLine)
    {
        nLine-=pTable->nLine;
        pTable=pTable->pNext;
    }
    if(pTable==NULL)    return -1;
    sImage=pTable->sImage+GetTableWidth(pTable->sDescribe);
    sImage+=2;
    while(--nWhere)
    {
        sImage=strstr(sImage,VERTIC_SMA);
        if(sImage!=NULL) sImage+=2;
    }
    if(sImage==NULL) return -1;

    nWidth=0;
    for(;;)
    {
        if((strstr(sImage,VERTIC_SMA)==sImage)||
           (strstr(sImage,VERTIC_BIG)==sImage)) break;
        sImage+=2;
        nWidth+=2;
    }
    if(nWidth<2)  return -1;
    if(strlen(sOutput)<=nWidth) return 1;
    sPos=sOutput;
    nCount=1;
    for(;;)
    {

        if(strlen(sPos)<=nWidth) return nCount;
        nch=0;
        for(nRec=0;nRec<nWidth;nRec++)
        {

                if(!sPos[0]) return nCount;
                if(sPos[0]>0) nch++;
                if(sPos[0]=='\n')
                {
                        nch=0;
                        nRec=-1;
                        nCount++;
                }
                sPos++;
        }
        if(nch<nWidth) sPos-=(nch%2);
        if(sPos[0]!='\n')
        {
                sImage=sOutput+strlen(sOutput);
                sImage[1]=0;
                while(sImage--!=sPos) sImage[1]=sImage[0];
                sPos[0]='\n';
        }

        sPos++;
        nCount++;
   }
}

/*-----------------------------------------------------------------

函数:DATATABLE * CreateDataTable;
目的:建立一个数据表格对象
参数:nLine:表格行数；
     sDescribe:表格列信息描述；
     sFormat:表示每个列的数据类型
返回:NULL失败，否则返回一个指向该表格对象的指针
举例:

 如下面的表格：
┏━━┯━━┯━━┯━━━━━━━━━━━━━━┯━━┯━━┯━━┓
┃操作│ 行 │ 列 │     内       容            │类型│长度│小数┃
┠──┼──┼──┼──────────────┼──┼──┼──┨
┃+   │   1│   5│cjymc                       │1   │  50│ 0  ┃
┠──┼──┼──┼──────────────┼──┼──┼──┨
┃+   │   3│  10│cjym                        │1   │  20│ 0  ┃
┠──┼──┼──┼──────────────┼──┼──┼──┨

 可以用下面的方法建立数据表格：

 DATATABLE * pdt;

 pdt=CreateDataTable(6,
      "|操作| 行 | 列 |     内       容            |类型|长度|小数|",
      "%1s%4d%4d%20.20s%1d%4d%2d");
 if(pdt==NULL) return;

 ImportData(pdt,1,"%s%d%d%s%d%d%d","+",1,5,"cjymc",1,50,0);
 ImportData(pdt,2,"%s%d%d%s%d%d%d","+",3,10,"cjym",1,20,0);
 .......

*-----------------------------------------------------------------*/

DATATABLE * CreateDataTable(long nLine,char * sDescribe,char * sFormat)
{

    DATATABLE * pdt;
    long address,loop;
    char * offset;

    if(!(pdt=(DATATABLE *)malloc(sizeof(DATATABLE)))) return NULL;
    memset(pdt,0,sizeof(DATATABLE));
    strcpy(pdt->sFormat,sFormat);
    if(nLine>0&&sDescribe!=NULL)
    {
    	pdt->pTable=CreateTable(nLine+1,sDescribe);

        if(!pdt->pTable)
        {
    	    free(pdt->sFormat);
    	    free(pdt);
    	    return NULL;
        }
        pdt->nPage=nLine;
    }

    pdt->nsize=0;
    address=0;
    loop=0;
    offset=pdt->sFormat;
    while(offset[0])
    {
       if(!strchr("sdf",offset[0]))
       {
           offset++;
           continue;
       }
       pdt->pArg[loop]=address;
       switch(offset[0])
       {
       	   case 's':
       	      address+=INPUT_STRING_SIZE;
              pdt->nsize+=INPUT_STRING_SIZE;
       	      break;

       	   case 'd':
       	      address+=sizeof(long);
              pdt->nsize+=sizeof(long);
       	      break;

       	   case 'f':
       	      address+=sizeof(double);
              pdt->nsize+=sizeof(double);
       	      break;
       }
       loop++;
       offset++;
    }//end while

    return pdt;
}

/*-----------------------------------------------------------------

函数:long AssignTableFields;
目的:给数据表格字段附别名
参数:pdt:表格对象指针
     sFiled:别名字符串，每个别名用逗号分隔，且排列顺序应与规定格式一致
返回:<0失败,>0成功
*-----------------------------------------------------------------*/
long AssignTableFields(DATATABLE * pdt,char * sFields)
{
     char * sOffset,* sOut;
     long nCount=0,nLoop=0;
     char sColumn[51];
     char sbuf[100];

     if(!pdt||!sFields) return -1;

     sOffset=sFields;
     memset(pdt->sFields,'\0',sizeof(pdt->sFields));

     for(;;)
     {
        memset(sColumn,'\0',sizeof(sColumn));
        nLoop=0;
        for(;;)
        {
            if(sOffset[0]==' ')
            {
            	sOffset++;
            	continue;
            }
            sColumn[nLoop++]=sOffset[0];
     	    if(sOffset[0]==','||!sOffset[0])
     	    {
     	    	nCount++;
     	    	upper(sColumn);
                sOut=pdt->sFields;
                for(;;)
                {
                	sOut=strstr(sOut,sColumn);
                	if(!sOut||sOut==pdt->sFields) break;
                	sOut--;
                	if(sOut[0]==',') break;
                	sOut+=2;
                }

     	    	if(sOut)
     	    	{
                        sprintf(sbuf,"  错误：重复的别名\"%s\"",sColumn);
                        MessageBox(sbuf,1);
     	    		return -1;
     	    	}
     	    	strcat(pdt->sFields,sColumn);
     	    	break;
     	    }
     	    sOffset++;
        }//end for(;;)
        if(!sOffset[0]) break;
        sOffset++;
     }

     sOffset=pdt->sFormat;
     nLoop=0;
     while(sOffset[0])
     {
     	if(sOffset[0]=='%') nLoop++;
     	sOffset++;
     }

     if(nCount!=nLoop)
     {
     	MessageBox("    别名的个数与实际列数不符或别名字符串格式错误!",1);
     	return -2;
     }
     return 1;
}

/*-----------------------------------------------------------------
函数:SetTableStyle
目的:设置数据表格附加属性
参数:pdt:数据表格指针
     nStyle:风格参数,目前有:
           ST_LINENUM      :  显示行号
           ST_MESSAGELINE  :  自动增加1行信息
     nflag:1:增加此风格
           0:删除此风格
说明:此函数必须在显示数据表格之前调用
*-----------------------------------------------------------------*/
void SetTableStyle(DATATABLE * pdt,long nStyle,long nflag)
{
     if(!pdt) return;
     if(nflag) pdt->nStyle|=nStyle;
     else      pdt->nStyle&=~nStyle;
}

/*-----------------------------------------------------------------

函数:long ShowDataTable;
目的:显示一个表格
参数:pdt:表格对象指针，(npx,npy):表格左上角屏幕坐标
返回:<0失败,>0成功

*-----------------------------------------------------------------*/

long ShowDataTable(DATATABLE * pdt,long npx,long npy)
{
    char sFormat[2000];
    long nWidth;

    if((pdt->nStyle&ST_MESSAGELINE||pdt->nStyle&ST_DOUBLELINE) && !pdt->pTable->pNext)
    {
    	memset(sFormat,' ',sizeof(sFormat));
    	nWidth=GetTableWidth(pdt->pTable->sDescribe);
    	sFormat[0]='|';
    	sFormat[nWidth-4]='|';
    	sFormat[nWidth-3]='\0';
    	MergeTable(pdt->pTable,CreateTable(1+((pdt->nStyle&ST_DOUBLELINE)!=0),sFormat));
    	if(pdt->nStyle&ST_DOUBLELINE) FillTable(pdt->pTable->pNext,1,"%s","\n");
    }

    return ShowTable(pdt->pTable,npx,npy);


}

/*-----------------------------------------------------------------

函数:ImportData;
目的:向数据表格中导入数据
参数:pdt:数据表格指针
     nLine:数据表格记录号；
     sFormat:数据据格式字符串或别名序列；
     ...:数据实例
返回:nLine成功，<0失败
说明:如果nLine>0，若表格中没有该行，则表示建立一条新的记录并将这些数
     据导入其中，并返回实际导入的记录号；若存在该行则表示以新的数据
     覆盖原有的数据，若sFormat中有">"表示跳过所对应的列。
*-----------------------------------------------------------------*/
long ImportData(DATATABLE * pdt,long nLine,char * sFormat,...)
{
    char sOutFormat[500];
    long nsearch,naccess,nRetVal,nExchange,nVisual,address;
    DATALINK * pdl;
    char * ps;
    va_list pVar;

    if(!strchr(sFormat,'%'))  //按别名方式导入
    {
    	nVisual=pdt->pTable->nVisual;
    	pdt->pTable->nVisual=0;
    	naccess=1;
        va_start(pVar,sFormat);
    	for(;;)
    	{
    	    nRetVal=TranslateTableFields(pdt,naccess,0,sFormat,sOutFormat);

            if(nRetVal==0) break;
            if(nRetVal<0)
            {
               pdt->pTable->nVisual=nVisual;
               FlushDataLine(pdt,nLine);
               return -1;
            }

            if(strchr(sOutFormat,'f'))
              nExchange=ImportData(pdt,nLine,sOutFormat,va_arg(pVar,double));
            else if(strchr(sOutFormat,'d'))
              nExchange=ImportData(pdt,nLine,sOutFormat,va_arg(pVar,long));
            else if(strchr(sOutFormat,'s'))
              nExchange=ImportData(pdt,nLine,sOutFormat,va_arg(pVar,char *));

            if(nExchange<=0)
            {
                pdt->pTable->nVisual=nVisual;
                FlushDataLine(pdt,nLine);
            	return nExchange;
            }
            naccess++;
        }
        va_end(pVar);
        pdt->pTable->nVisual=nVisual;
        FlushDataLine(pdt,nLine);
        return nExchange;
    }

    if(pdt==NULL||nLine<=0||sFormat==NULL) return -1;

    nsearch=nLine;

    if(pdt->nCount<nLine)
    {
    	if(InsertDataNode(pdt,0)<0) return -2;
    	nsearch=pdt->nCount;
    }


    pdl=pdt->pdata;

    while(--nsearch) pdl=pdl->pNext;

    naccess=0;

    va_start(pVar,sFormat);

    while(sFormat[0])
    {
        address=pdl->address+pdt->pArg[naccess];

    	switch(sFormat[0])
    	{
    	    case '>':
    	        naccess++;
    	        break;

    	    case '%':
    	        sFormat++;
    	        switch(sFormat[0])
    	        {
    	       	   case 's':
    	               ps=va_arg(pVar,char *);
                       memset((char *)address,'\0',INPUT_STRING_SIZE);
                       strncpy((char *)address,ps,INPUT_STRING_SIZE-1);
    	       	       naccess++;
    	       	       break;

    	       	   case 'd':
    	       	       (* (long *)address)=va_arg(pVar,long);
    	       	       naccess++;
    	       	       break;

    	       	   case 'f':
    	       	       (* (double *)address)=va_arg(pVar,double);
    	       	       naccess++;
    	       	       break;
    	       	}
    	       	break;
    	}
    	sFormat++;
    }
    va_end(pVar);

    FlushDataLine(pdt,nLine);
    return nLine;

}

/*-----------------------------------------------------------------

函数:ExportData
目的:从数据表格中导出数据到变量
参数:pdt:数据表格指针
     nLine:目的表格中记录号；
     sFormat:参数类型描述字符串或别名序列；
     ...:目标参数指针；
返回:>0成功，否则失败
说明:nLine所表示的行号在数据表格中必须存在；
     在sFormat中使用">"表示跳过该字段
*-----------------------------------------------------------------*/
long ExportData(DATATABLE * pdt,long nLine,char * sFormat,...)
{
    char sOutFormat[500];
    long nsearch,naccess,nRetVal,nExchange,address;
    char * pChar;
    DATALINK * pdl;
    va_list pVar;


    if(!strchr(sFormat,'%'))  //按别名方式
    {
    	naccess=1;
        va_start(pVar,sFormat);
    	for(;;)
    	{
    	    nRetVal=TranslateTableFields(pdt,naccess,0,sFormat,sOutFormat);

            if(nRetVal==0) break;
            if(nRetVal<0) return -1;
            nExchange=ExportData(pdt,nLine,sOutFormat,va_arg(pVar,long));
            if(nExchange<=0) return nExchange;
            naccess++;
        }
        va_end(pVar);
        return nExchange;
    }

    if(pdt==NULL||nLine<=0||sFormat==NULL||nLine>pdt->nCount) return -1;

    nsearch=nLine;

    pdl=pdt->pdata;
    while(--nsearch) pdl=pdl->pNext;

    naccess=0;

    va_start(pVar,sFormat);
    while(sFormat[0]!='\0')
    {
        address=pdl->address+pdt->pArg[naccess];
    	switch(sFormat[0])
    	{
    	    case '>':
    	        naccess++;
    	        break;

    	    case '%':
    	        sFormat++;
    	        switch(sFormat[0])
    	        {
    	       	   case 's':
    	       	       pChar=va_arg(pVar,char *);
    	       	       strcpy(pChar,(char *)address);
                       if(pdt->nStyle&ST_ALLTRIM) alltrim(pChar);
    	       	       naccess++;
    	       	       break;

    	       	   case 'd':
    	       	       *(va_arg(pVar,long *))=(* (long *)address);
    	       	       naccess++;
    	       	       break;

    	       	   case 'f':
    	       	       *(va_arg(pVar,double *))=(* (double *)address);
    	       	       naccess++;
    	       	       break;
    	       	}
    	       	break;
    	}
    	sFormat++;
    }
    va_end(pVar);
    return nLine;
}


/*-----------------------------------------------------------------

函数:InsertDataNode
目的:在nLine条记录之前插入一条记录
参数:pdt:数据表格指针
返回:若成功则返回实际插入的记录号，否则返回负数

*-----------------------------------------------------------------*/
long InsertDataNode(DATATABLE * pdt,long nLine)
{

    DATALINK * pnew;
    DATALINK * pdl;
    char * format;
    long naccess=0,nsearch;


    if(pdt==NULL||!strchr(pdt->sFormat,'%')) return -1;

    if(nLine>pdt->nCount) nLine=0;
    if(!(pnew=(DATALINK *)malloc(sizeof(DATALINK)))) return -2;
    memset(pnew,0,sizeof(DATALINK));

    if(!(pnew->address=(long)malloc(pdt->nsize))) return -2;
    memset((void*)pnew->address,0,pdt->nsize);

    format=pdt->sFormat;
    naccess=0;
    while(format[0])
    {
        if(!strchr("sdf",format[0]))
        {
            format++;
            continue;
        }
    	if(format[0]=='s')
    	   strcpy((char*)(pdt->pArg[naccess]+pnew->address),".");

    	naccess++;
    	format++;
    }

    pdt->nCount++;
    pdl=pdt->pdata;

    if(pdt->pdata==NULL||pdt->nCount==0)
    {
    	pdt->pdata=pnew;
    	pdt->nCount=1;
    	pdt->nCurrent=1;
    	return 1;
    }
    if(nLine==1)
    {
    	pnew->pNext=pdt->pdata;
    	pdt->pdata=pnew;
        return 1;
    }

    if(nLine<=0)
    {
    	while(pdl->pNext) pdl=pdl->pNext;
    	pdl->pNext=pnew;
        return pdt->nCount;
    }

    nsearch=nLine-1;
    while(--nsearch) pdl=pdl->pNext;
    pnew->pNext=pdl->pNext;
    pdl->pNext=pnew;

    return nLine;
}


long FlushDataLine(DATATABLE * pdt,long nLine)
{

    DATALINK * pdl;
    char * start,* end;
    char sOut[100];
    long nwhere,njump,address;


    if(pdt->pTable==NULL) return 0;
    if(pdt==NULL||nLine<pdt->nCurrent||nLine>=pdt->nCurrent+pdt->nPage) return -1;
    if(!pdt->nCount)
    {
    	ClearTableLine(pdt->pTable,2);
        if((pdt->nStyle&ST_LINENUM) && pdt->pTable->npx>=4 && pdt->pTable->nVisual)
        {
              gotoxy(pdt->pTable->npx-4,pdt->pTable->npy+3);
              outtext("    ");
        }
    	return 0;
    }
    if(nLine<=0||pdt->nCurrent<=0) return -2;
    if(nLine>pdt->nCount)
    {
    	ClearTableLine(pdt->pTable,nLine-pdt->nCurrent+2);
        if((pdt->nStyle&ST_LINENUM) && pdt->pTable->npx>=4 && pdt->pTable->nVisual)
        {
           gotoxy(pdt->pTable->npx-4,
                  pdt->pTable->npy+(nLine%pdt->nPage+
                  ((nLine%pdt->nPage)==0)*pdt->nPage)*2+1);
       	   outtext("    ");
        }
        return 0;
    }

    memset(sOut,'\0',sizeof(sOut));

    nwhere=nLine;
    pdl=pdt->pdata;
    while(--nwhere) pdl=pdl->pNext;

    nwhere=0;
    start=strchr(pdt->sFormat,'%');
    while(start=strchr(start,'%'))
    {
    	end=strchr(start+1,'%');

        memset(sOut,'\0',sizeof(sOut));
        for(njump=0;njump<nwhere;njump++) sOut[njump]='>';

    	if(end!=NULL)
   	     strncpy(sOut+nwhere,start,end-start);
   	else strcpy(sOut+nwhere,start);

        address=pdl->address+pdt->pArg[nwhere];

   	if(strchr(sOut,'s'))
   	     FillTable(pdt->pTable,nLine-pdt->nCurrent+2,sOut,(char*)address);
   	else if(strchr(sOut,'d'))
   	     FillTable(pdt->pTable,nLine-pdt->nCurrent+2,sOut,*(long *)address);
   	else if(strchr(sOut,'f'))
   	     FillTable(pdt->pTable,nLine-pdt->nCurrent+2,sOut,*(double *)address);

   	start++;
        nwhere++;
    }
    if((pdt->nStyle&ST_LINENUM) && pdt->pTable->npx>=4 && pdt->pTable->nVisual)
    {
       gotoxy(pdt->pTable->npx-4,
              pdt->pTable->npy+(nLine%pdt->nPage+
              ((nLine%pdt->nPage)==0)*pdt->nPage)*2+1);
       outtext("%4ld",nLine);
    }
    return nLine;

}

/*-----------------------------------------------------------------

函数:DeleteDataNode
目的:删除数据表格中的指定记录
参数:pdt:数据表格指针；
     nLine:要删除的记录号；
返回:>0,成功，实际的记录号;<0失败

*-----------------------------------------------------------------*/

long DeleteDataNode(DATATABLE * pdt,long nLine)
{

    DATALINK * pdl,* prm;
    long nsearch;

    if(pdt==NULL||nLine>pdt->nCount||nLine<1||pdt->nCount<1) return -1;

    nsearch=nLine-1;
    pdl=pdt->pdata;
    pdt->nCount--;

    if(nLine==1)
    {
        prm=pdt->pdata;
    	pdt->pdata=pdt->pdata->pNext;
    }
    else
    {
    	while(--nsearch) pdl=pdl->pNext;
    	prm=pdl->pNext;
    	pdl->pNext=pdl->pNext->pNext;
    }

    free((void *)prm->address);
    free((void *)prm);

    if(pdt->pTable&&pdt->pTable->nVisual && nLine<pdt->nCurrent+pdt->nPage)
        for(nsearch=nLine;nsearch<pdt->nCurrent+pdt->nPage;nsearch++)
            FlushDataLine(pdt,nsearch);

    return nLine;

}

/*-----------------------------------------------------------------

函数:RefreshDataTable
目的:刷新表格（如在使用InsertDataNode后)
参数:pdt:数据表格指针；
返回:无

*-----------------------------------------------------------------*/
void RefreshDataTable(DATATABLE * pdt)
{
    long nsearch;


    if(pdt==NULL) return;

    for(nsearch=pdt->nCurrent;nsearch<pdt->nCurrent+pdt->nPage;nsearch++)
        FlushDataLine(pdt,nsearch);

}

/*-----------------------------------------------------------------

函数:ScrollTable；
目的:从当前显示位置卷动显示窗口
参数:pdt:数据表格指针；
     nOpt:表示要卷动的行数，正数表示向下卷动，负数表示向上卷动
返回:>0成功，<0失败

*-----------------------------------------------------------------*/
long ScrollTable(DATATABLE * pdt,long nOpt)
{
    long nsearch;


    if(pdt==NULL||nOpt==0) return -1;
    pdt->nCurrent+=nOpt;
    if(pdt->nCurrent<1) pdt->nCurrent=1;
    if(pdt->nCurrent>pdt->nCount) pdt->nCurrent=pdt->nCount;

    for(nsearch=pdt->nCurrent;nsearch<pdt->nCurrent+pdt->nPage;nsearch++)
        FlushDataLine(pdt,nsearch);

    return 1;


}

/*-----------------------------------------------------------------

函数:FetchData
目的:用户输入数据到数据表格中；
参数:pdt:数据表格指针；
     nLine:要存取的数据表格记录号
     nWhere:目的列的序号
返回:>0成功，代表用户所按的功能键，<0失败
说明:sFormat表示要取得数据的类型，每一个类型要给出取数的长度，如%10s
     表示取一个长度为10的字符串型，如果使用">"表示跳过该列。当输入完
     毕后数据已经存在于数据表格中，此时，用户可使用ExportData将数据导
     出以便作各种控制。
*-----------------------------------------------------------------*/
long FetchData(DATATABLE * pdt,long nLine,long nWhere,char * sFormat)
{

    long ntabline,nkey,nsearch,naccess,address;
    DATALINK * pdl;
    long where;
    char sOutput[200],* sOffset;
    char sOutFormat[500];

    if(!strchr(sFormat,'%'))  //按别名方式
    {
        if(TranslateTableFields(pdt,nWhere,1,sFormat,sOutFormat)<=0) return 0;
        naccess=0;
        sOffset=sOutFormat;
        while(sOffset[0])
        {
             if(sOffset[0]=='>') naccess++;
             if(sOffset[0]=='%')
             {
             	naccess++;
             	break;
             }
             sOffset++;
        }
        if(naccess>0) return FetchData(pdt,nLine,naccess,sOutFormat);
        return 0;
    }


    sOffset=sOutput;
    strcpy(sOutput,sFormat);

    where=1;
    for(;;)
    {
    	if(where>=nWhere) break;
    	if(sOffset[0]=='>'||sOffset[0]=='%')
    	{
    		where++;
    		sOffset[0]='>';
    	}
    	sOffset++;
    }

    if(pdt==NULL||nLine<1||nLine>pdt->nCount||!strchr(sFormat,'%')) return -1;

    if(nLine<pdt->nCurrent||nLine>=(pdt->nCurrent+pdt->nPage))
    {
    	pdt->nCurrent=nLine;
        while((pdt->nCurrent-1)%pdt->nPage) pdt->nCurrent--;
    	RefreshDataTable(pdt);
        ntabline=nLine-pdt->nCurrent+2;
    }
    else ntabline=nLine-pdt->nCurrent+2;

    nsearch=nLine;
    pdl=pdt->pdata;
    while(--nsearch) pdl=pdl->pNext;

    address=pdl->address+pdt->pArg[where-1];

    nkey=FetchTable(pdt->pTable,ntabline,nWhere,sOutput,address);
    FlushDataLine(pdt,nLine);
    return nkey;
}

/*-----------------------------------------------------------------
函数:FindDataTable
目的:查询是否有匹配的信息
参数:pdt:数据表格指针
     nStart:起始行号
     sFormat:被匹配字段的类型描述
     ...:被匹配字段的值
返回:>0匹配成功的行号
     =0没有匹配的数据
     <0错误

例如：要查找某数据表中是否存在第二个字段等于"abc",且第三个字段等于10
      的记录：
      nRetVal=FindDataTable(pdt,1," > %s %d","abc",10);
      if(nRetVal>0) printf("\nI got it...");

说明：本函数采取首次匹配算法，可以通过改变起始行号来遍历整个表格
*-----------------------------------------------------------------*/
long FindDataTable(DATATABLE * pdt,long nStart,char * sFormat,...)
{
    va_list pvar;
    long nCurrent,nWhere,nMatch,nRetVal,address;
    DATALINK * pdl;
    char * sOffset;
    char sVal[INPUT_STRING_SIZE];
    char sOutFormat[500];

    if(!pdt||nStart<1||nStart>pdt->nCount) return -1;
    if(!strchr(sFormat,'%'))  //按别名方式,只提供单值查询
    {
        va_start(pvar,sFormat);
        nRetVal=TranslateTableFields(pdt,1,0,sFormat,sOutFormat);
        if(nRetVal<=0) return 0;

        if(strchr(sOutFormat,'f'))
           nRetVal=FindDataTable(pdt,nStart,sOutFormat,va_arg(pvar,double));
        else if(strchr(sOutFormat,'d'))
           nRetVal=FindDataTable(pdt,nStart,sOutFormat,va_arg(pvar,long));
        else if(strchr(sOutFormat,'s'))
           nRetVal=FindDataTable(pdt,nStart,sOutFormat,va_arg(pvar,char *));
        va_end(pvar);
        return nRetVal;
    }


    nCurrent=nStart;
    pdl=pdt->pdata;
    while(--nCurrent) pdl=pdl->pNext;
    for(nCurrent=nStart;nCurrent<=pdt->nCount;nCurrent++)
    {
        nMatch=1;
    	nWhere=0;
    	sOffset=sFormat;
    	va_start(pvar,sFormat);
    	for(;;)
    	{
            if(!sOffset[0]||!nMatch) break;

            address=pdt->pArg[nWhere]+pdl->address;

            switch(sOffset[0])
            {
            	case 's':
            	     strncpy(sVal,(char *)address,INPUT_STRING_SIZE-1);
            	     sVal[INPUT_STRING_SIZE-1]='\0';
            	     alltrim(sVal);
            	     if(strcmp(sVal,va_arg(pvar,char *))) nMatch=0;
            	     break;

            	case 'd':
            	     nMatch=(*((long *)address)==va_arg(pvar,long));
            	     break;

            	case 'f':
            	     nMatch=(*((double *)address)==va_arg(pvar,double));
            	     break;

                case '>':
                     break;

            	default:
            	     sOffset++;
            	     continue;

            }//end switch
            sOffset++;
            nWhere++;
    	}//end for
    	va_end(pvar);
        if(nMatch) return nCurrent;
        pdl=pdl->pNext;
    }//end for
    return 0;
}


/*-----------------------------------------------------------------
函数:SumDataTable
目的:对数据表格中某些字段进行求和
参数:pdt:数据表格指针
     nStart:起始行
     nEnd:结束行
     ... :存储求和值变量的指针
返回:>0成功
     =0起始或结束行号超出范围
     <0失败

例如：要对数据表格中所有记录的第三和第五字段求和：
      nRetVal=SumDataTable(pdt,1,pdt->nCount,"> > %d > %f",&nVal,&dVal);
      if(nRetVal>0)
           printf("\nsum(3)=%ld,sum(5)=%.2lf",nVal,dVal);

说明：对于字符串类型的字段不能作求和运算
*-----------------------------------------------------------------*/
long SumDataTable(DATATABLE * pdt,long nStart,long nEnd,char * sFormat,...)
{
    va_list pvar;
    long nCurrent,nWhere,naccess,nRetVal,nExchange,address;
    DATALINK * pdl;
    char * sOffset;
    char sOutFormat[500];

    if(!pdt) return -1;

    if(!strchr(sFormat,'%'))  //按别名方式
    {
    	naccess=1;
        va_start(pvar,sFormat);
    	for(;;)
    	{
    	    nRetVal=TranslateTableFields(pdt,naccess,0,sFormat,sOutFormat);
            if(nRetVal==0) break;
            if(nRetVal<0) return -1;
            nExchange=SumDataTable(pdt,nStart,nEnd,sOutFormat,va_arg(pvar,long));
            if(nExchange<=0) return nExchange;
            naccess++;
        }
        va_end(pvar);
        if(nRetVal==0) return 1;
        else nExchange;
    }

    if(nStart<1||nStart>pdt->nCount||nEnd<1||nEnd>pdt->nCount||nEnd<nStart) return 0;

    nCurrent=nStart;
    pdl=pdt->pdata;
    while(--nCurrent) pdl=pdl->pNext;
    for(nCurrent=nStart;nCurrent<=nEnd;nCurrent++)
    {
    	nWhere=0;
    	sOffset=sFormat;
    	va_start(pvar,sFormat);
    	for(;;)
    	{
            if(!sOffset[0]) break;
            address=pdt->pArg[nWhere]+pdl->address;
            switch(sOffset[0])
            {
            	case 'd':
                     if(nCurrent==nStart)
                        (* va_arg(pvar,long*))=*((long *)address);
                     else
                        (* va_arg(pvar,long*))+=*((long *)address);

            	     break;

            	case 'f':
                     if(nCurrent==nStart)
                        (* va_arg(pvar,double *))=*((double  *)address);
                     else
                        (* va_arg(pvar,double *))+=*((double *)address);
            	     break;

                case '>':
                     break;

            	default:
            	     sOffset++;
            	     continue;

            }//end switch
            sOffset++;
            nWhere++;
    	}//end for
    	va_end(pvar);
        pdl=pdl->pNext;
    }//end for
    return 1;
}


/*-----------------------------------------------------------------
函数:ShowDatatableInfo
目的:在数据表格的最底部显示信息
参数:pdt:数据表格指针
     smsg:要显示的字符串
     ntype：0：不等待  否则等待
*-----------------------------------------------------------------*/
void ShowDatatableInfo(DATATABLE * pdt,char * smsg,long ntype)
{

     TABLE * pTab;
     char sFormat[20];
     long nLength;
     if(!pdt||!smsg) return;

     if(pTab=pdt->pTable->pNext)
     {
        nLength=GetTableWidth(pdt->pTable->sDescribe)-6;
        sprintf(sFormat,"%%-%ld.%lds",nLength*pTab->nLine,nLength*pTab->nLine);

        FillTable(pdt->pTable,pdt->pTable->nLine+1,sFormat,smsg);
        if(ntype)
        {
     	  keyb();
          FillTable(pdt->pTable,pdt->pTable->nLine+1,sFormat," ");
        }
        fflush(stdout);
     }

}
/*-----------------------------------------------------------------
函数:AutoExtendTable
目的:自动增加表格行
参数:pdt:数据表格指针
     pnCurrent:当前行指针
返回:>0已成功增加，<0失败,=0没有增的需要
*-----------------------------------------------------------------*/
long AutoExtendTable(DATATABLE * pdt,long * pnCurrent)
{
    if(!pdt) return -1;

    if(pdt->nStatus&ST_INSERT)
    {
    	pdt->nStatus^=ST_INSERT;
    	return 1;
    }
    if(*pnCurrent>pdt->nCount||(*pnCurrent)==0)
    {
          InsertDataNode(pdt,0);
          *pnCurrent=pdt->nCount;
          FlushDataLine(pdt,pdt->nCount);
          return pdt->nCount;
    }
    return 0;
}


/*-----------------------------------------------------------------
函数:DefaultTableKey
目的:处理数据表格的功能键
参数:pdt:数据表格指针
     nKey:功能键值
     pnCurrent:当前行的指针
     pnWhere:当前列的指针
返回:>0成功，<0失败,=0没有相应的映射
*-----------------------------------------------------------------*/
long DefaultTableKey(DATATABLE * pdt,long nKey,long * pnCurrent,long * pnWhere)
{

     if(!pdt) return -1;
     switch(nKey)
     {
	   case KEY_LEFT:
		    if((*pnWhere)>1) (*pnWhere)--;
		    return 1;
	   case KEY_UP:
		    if((*pnWhere)!=1 && !(pdt->nStyle&ST_UPDOWN)) return 1;
		    if((*pnCurrent)>1) (*pnCurrent)--;
		    return 1;
	   case KEY_DOWN:
		    if((*pnWhere)!=1 && !(pdt->nStyle&ST_UPDOWN)) return 1;
		    if(((*pnCurrent)<pdt->nCount)) (*pnCurrent)++;
		    return 1;
           case KEY_HOME:
                    return DefaultTableChoice(pdt,'h',pnCurrent,pnWhere,0);
           case KEY_END:
                    return DefaultTableChoice(pdt,'e',pnCurrent,pnWhere,0);
           case KEY_PAGEUP:
                    return DefaultTableChoice(pdt,'u',pnCurrent,pnWhere,0);
           case KEY_PAGEDOWN:
                    return DefaultTableChoice(pdt,'d',pnCurrent,pnWhere,0);
           case KEY_F1:
                    return DefaultTableChoice(pdt,'f',pnCurrent,pnWhere,0);
           case KEY_TAB:
                {
                    long nWidth,nClean;
                    TABLE * pt;

                    pt=pdt->pTable;
                    nClean=0;
                    while(pt)
                    {
                        nClean+=pt->nLine*2+1;
                        pt=pt->pNext;
                    }

                    pt=pdt->pTable;
                    nWidth=GetTableWidth(pt->sDescribe);

                    for(;;)
                    {
                    	if(pt->npx+nWidth<=SCREEN_WIDTH) break;
                    	pt->npx-=40;
                        clearline2(pt->npy,pt->npy+nClean);
                    	ShowTable(pt,pt->npx,pt->npy);
                    	if(keyb()!=KEY_TAB) break;
                    }
                    return 1;
                }

     }
     return 0;
}


/*-----------------------------------------------------------------
函数:DefaultTableChoice
目的:处理数据表格的功能选择
参数:pdt:数据表格指针
     cOpt:功能选项字符
     pnCurrent:当前行的指针
     pnWhere:当前列的指针
返回:>0成功，<0失败,=0没有相应的映射
*-----------------------------------------------------------------*/
long DefaultTableChoice(DATATABLE * pdt,char cOpt,long * pnCurrent,
                        long * pnWhere,long * pnModify)
{
     long nKey,nLine;
     TABLE * pt;
     char sIndex[41];
     if(!pdt) return -1;

     switch(cOpt)
     {
         case 'a':   //append record
	 case 'A':
		 (*pnCurrent)=pdt->nCount+1;
		 return 1;

         case 'i':   //append record
	 case 'I':
                 if(InsertDataNode(pdt,*pnCurrent)>0)
                 {
                     for(nKey=*pnCurrent;nKey<pdt->nCurrent+pdt->nPage;nKey++)
                            FlushDataLine(pdt,nKey);
                     pdt->nStatus|=ST_INSERT;
		     return 1;
		 }
		 return 0;
	 case '+':
	         (* pnWhere)=2;
	         return 1;

         case '-':   //delete record

                 if(!(pdt->nStyle&ST_NOTCONFIRM))  //如果需要确认
                 {
                     ShowDatatableInfo(pdt,"真的要删除该记录吗(y/n)？",0);
                     nKey=keyb();
                     if(nKey!='y'&&nKey!='Y') return 1;
                 }
                 DeleteDataNode(pdt,*pnCurrent);
		 if(pnModify) *pnModify=1;
		 if(*pnCurrent>pdt->nCount) *pnCurrent=pdt->nCount;
		 if(*pnCurrent<=0) (*pnCurrent)=1;
		 RefreshDataTable(pdt);
                 return 1;

         case 'h':   //goto head
         case 'H':
                 *pnCurrent=1;
                 return 1;

         case 'E':   //goto end
         case 'e':
                  *pnCurrent=pdt->nCount;
                  return 1;

         case 'u':  //page up
         case 'U':
                  *pnCurrent-=pdt->nPage;
                  if(*pnCurrent<1) *pnCurrent=1;
                  return 1;

         case 'd':  //page down
         case 'D':
                  *pnCurrent+=pdt->nPage;
                  if(*pnCurrent>pdt->nCount)
                             *pnCurrent=pdt->nCount;
                  return 1;

         case 'g':  //goto to the line you desired
         case 'G':
                  if(!(pt=pdt->pTable->pNext)) return 0;
                  nLine=0;
                  ClearTableLine(pt,1);
                  nKey=GetValue(pt->npx+2,pt->npy+1,
                                "请输入目标行号:%-4d",&nLine);
                  ClearTableLine(pt,1);
                  if(nKey!=KEY_RETURN) return 0;
                  if(nLine<1||nLine>pdt->nCount)
                  {
                     ShowDatatableInfo(pdt,"您输入的行号已超出范围...",1);
                     return 0;
                  }
                  *pnCurrent=nLine;
                  return 1;

         case 'f':  //find a line by index
         case 'F':
                  if(!(pt=pdt->pTable->pNext)) return 0;
                  strcpy(sIndex,".");
                  ClearTableLine(pt,1);
                  nKey=GetValue(pt->npx+2,pt->npy+1,
                                "请输入关键词:%-40.40s",sIndex);
                  ClearTableLine(pt,1);
                  alltrim(sIndex);
                  if(nKey!=KEY_RETURN||strlen(sIndex)<2) return 0;
                  nLine=FindTableByIndex(pdt,*pnCurrent,sIndex);
                  if(nLine<=0)
                  {
                     ShowDatatableInfo(pdt,"没有匹配的信息...",1);
                     return 0;
                  }
                  *pnCurrent=nLine;
                  return 1;

         case 'm':  //show control message
         case 'M':
                  savewin();
                  MessageBox(
                  "              〖表 格 编 辑 控 制 帮 助〗\n"
                  "  A：增加一行        I：插入一行       F: 按关键字查找\n"
                  "  H: 跳到最首行      E：跳到最末行     G：跳到指定行\n"
                  "  D：下翻一页        U：上翻一页       M：控制帮助\n"
                  "  +: 编辑当前行      -：删除当前行     S: 保存数据\n"
                  "   ↑↓←→移动输入位置,[TAB]左右翻屏,[ESC]键返回...",2);
                  keyb();
                  popwin();
                  return 1;

     }
     return 0;
}
static long FindTableByIndex(DATATABLE * pdt,long nCurrent,char * sIndex)
{

     long nLine,naccess,address;
     DATALINK * pdl;
     char * sFormat,sOutput[151];

     if(!pdt||nCurrent<0||nCurrent>pdt->nCount) return -1;
     nLine=nCurrent;
     pdl=pdt->pdata;
     while(--nLine) pdl=pdl->pNext;
     for(nLine=nCurrent+1;nLine<=pdt->nCount;nLine++)
     {
          pdl=pdl->pNext;
          sFormat=pdt->sFormat;
          naccess=0;
          while(sFormat[0])
          {
                address=pdt->pArg[naccess]+pdl->address;

          	if(sFormat[0]=='d')
          	   sprintf(sOutput,"%ld",*(long*)address);
          	else if(sFormat[0]=='f')
          	   sprintf(sOutput,"%12.2lf",*(double*)address);
          	else if(sFormat[0]=='s')
          	   strcpy(sOutput,(char *)address);
          	else
          	{
          	   sFormat++;
          	   continue;
          	}
          	sFormat++;
          	naccess++;
          	if(strstr(sOutput,sIndex)) return nLine;
          }
     }
     return 0;
}

/*-----------------------------------------------------------------
函数:TranslateTableFields[内部函数]
目的:将别名表示的方式转换成域表示方式
参数:pdt:数据表格指针
     nIndex:域序号
     nMode:0:不翻译格式信息，1：翻译格式信息
     sInFormat:输入别名串
     sOutFormat:域格式输出
返回:>0成功，<0失败,=0没有相应的域
*-----------------------------------------------------------------*/
static long TranslateTableFields(DATATABLE * pdt,long nIndex,long nMode,
                                 char * sInFormat,char * sOutFormat)
{

     char * sOffset,* sOut;
     char sField[51],sbuf[100];
     char ctype;
     long nLoop,nFind;

     if(!pdt) return -1;

     sOffset=sInFormat;
     nLoop=nIndex;

     if(nLoop<=0) return 0;
     while(--nLoop)
     {
     	while(sOffset[0]&&sOffset[0]!=',') sOffset++;
     	if(!sOffset[0]) return 0;
     	sOffset++;
     }

     nLoop=0;
     memset(sField,'\0',sizeof(sField));
     while(sOffset[0]&&sOffset[0]!=',')
     {
     	sField[nLoop]=sOffset[0];
     	nLoop++;
        sOffset++;
     }

     upper(sField);
     alltrim(sField);
     sOffset=pdt->sFields;
     for(;;)
     {
        sOffset=strstr(sOffset,sField);
        if(!sOffset||sOffset==pdt->sFields) break;
     	if(sOffset[-1]==',') break;
     	sOffset++;
     }

     nLoop=strlen(sField);

     if(!sOffset||(sOffset[nLoop]!=',' && sOffset[nLoop]))
     {
        sprintf(sbuf,"  错误：非法的数据表格列名\"%s\"",sField);
        MessageBox(sbuf,1);
     	return -4;
     }

     nLoop=1;
     while(sOffset>pdt->sFields)
     {
     	if(sOffset[0]==',') nLoop++;
     	sOffset--;
     }

     sOffset=pdt->sFormat;
     nFind=0;
     for(;;)
     {
     	if(sOffset[0]=='%') nFind++;
     	if(nFind>=nLoop) break;
     	sOffset++;
     }

     memset(sOutFormat,'>',nLoop-1);
     sOut=sOutFormat+nLoop-1;
     sOut[0]='%';
     nLoop=(nMode==0);
     for(;;)
     {
     	ctype=sOffset[0];
     	if(strchr("sdf",ctype)||!ctype) break;
     	if(nMode) sOut[nLoop++]=ctype;
     	sOffset++;
     }
     sOut[nLoop]=ctype;
     sOut[nLoop+1]='\0';
     return 1;
}



/*-----------------------------------------------------------------

函数:DropDataTable
目的:释放一个数据表资源
参数:pdt:数据表格指针
返回:>0成功，<0失败

*-----------------------------------------------------------------*/
long DropDataTable(DATATABLE * pdt)
{
    if(pdt==NULL) return -1;
    if(pdt->pTable) pdt->pTable->nVisual=0;
    while(pdt->nCount>0) DeleteDataNode(pdt,pdt->nCount);
    if(pdt->pTable)      DropTable(pdt->pTable);
    free(pdt);
    return 1;
}

/*---------------------------------------------------------------*
函数:ShowHelp
功能:帮助文档浏览器
参数:sHlpFile:帮助文档文件[必须符合所规定的格式]；
     sIndex:索引项字符串
返回：>0成功
说明：
      [帮助文档的格式]

      1.一个索引项的开始用一对尖括符包含:如：<系统管理帮助>
        它的结束标志为：</系统管理帮助>
        它们之间的内容为要显示的内容，如果在它们中间还有索引项，则
        表示该索引为一个大的索引，当选中它后，将自动弹出所包含的子
        选项；理论上，其索引的级数不受限制。

      2.开始与结束标志必须成对出现，两者不能缺一，不能交错；

      3.在正文中不允许使用"<>",以免造成索引冲突;

      4.两个索引项之间的非索引内容将被忽略;

      5.如果参数sIndex不为NULL，表示用户想直接查看其帮助内容，
        而不需要进入菜单界面；如果其值为NULL表示用户需要查看
        整个帮助文档，本程序将自动建立索引级连菜单，供用户选
        择；

*---------------------------------------------------------------*/

long ShowHelp(char * sHlpFile,char * sIndex)
{
   MENU * pHlpMenu,* pIndexMenu,* pMainMenu;
   char sLine[MAX_LINE_LEN];
   long nRetVal,nFlag;

   if(!(pMainMenu=CreateHelpMenuByFile(sHlpFile))) return 0;

   if(sIndex!=NULL)
   {
   	nFlag=0;
        pIndexMenu=SearchItemByIndex(pMainMenu,sIndex,&nFlag);
        if(pIndexMenu==NULL&&nFlag)
        {
           DropMenu(pMainMenu);
           sprintf(sLine,"%s:0",sHlpFile);
           ShowHlpContents(0,sIndex,sLine);
           return 1;
        }
        pHlpMenu=pIndexMenu;
   }
   else
   {
   	pHlpMenu=pMainMenu;
	AddMenu(pMainMenu,"  0.退出帮助系统",NULL);
   }

   DrawHlpInterface(sHlpFile);
   for(;;)
   {
        nRetVal=PopMenu(pHlpMenu,4,6,16);
        if(nRetVal>=0||nRetVal==-KEY_ESC) break;
   }
   DropMenu(pMainMenu);
   return 1;
}

MENU * CreateHelpMenuByFile(char * sHlpFile)
{
   MENU * pMenu;
   char sIndexFile[81];
   char sLine[MAX_LINE_LEN];
   FILE * fpIndex;
   struct stat file_stat;
   long   nModifyFlag=0;


   clrscr();
   if(access(sHlpFile,0))
   {
        outtext("\n帮助文件%s不存在...",sHlpFile);
        keyb();
        return NULL;
   }
   strcpy(sIndexFile,sHlpFile);
   if(strchr(sIndexFile,'.'))
        strcpy(strchr(sIndexFile,'.'),".idx");
   else strcat(sIndexFile,".idx");

   if(access(sIndexFile,0)) nModifyFlag=1;
   else
   {
        stat(sHlpFile,&file_stat);
        if(!(fpIndex=fopen(sIndexFile,"rt"))) return NULL;
        getline(sLine,fpIndex);
        if(!strstr(sLine,"LASTTIME:")) nModifyFlag=1;
        else
             nModifyFlag=(file_stat.st_mtime!=
                             atol(strstr(sLine,"LASTTIME:")+9));
        fclose(fpIndex);
   }

   if(nModifyFlag && CreateHlpIndex(sHlpFile)<0)
   {
        remove(sIndexFile);
        return NULL;
   }

   if(!(pMenu=CreateHlpMenu(sHlpFile)))
   {
        outtext("\n建立帮助菜单出错...");
        keyb();
        return NULL;
   }

}

static MENU * SearchItemByIndex(MENU * pMenu,char * sIndex,long * pnFlag)
{

    MENUITEM * psItem;
    MENU * psMenu;

    (*pnFlag)=0;
    if(pMenu==NULL||sIndex==NULL) return NULL;

    psItem=pMenu->pPopMenu;

    while(psItem)
    {
        if(strstr(psItem->sSubItem,sIndex))
        {
         	if(psItem->pSubMenu==NULL)
         	{
         		(*pnFlag)=1;
         		return NULL;
         	}
         	else return psItem->pSubMenu;
        }
        if(psItem->pSubMenu)
        {
        	psMenu=SearchItemByIndex(psItem->pSubMenu,sIndex,pnFlag);
        	if(psMenu) return psMenu;
        	if(*pnFlag) return NULL;
        }
        psItem=psItem->pNext;

    }
    return NULL;
}


static long CreateHlpIndex(char * sHlpFile)
{
    char sIndexFile[81];
    char sBuffer[300];
    FILE * fpIndex;
    FILE * fpHlp;
    struct stat file_stat;
    long nFlag;

    outtext("\n正在重构帮助文档索引，请稍等...");

    strcpy(sIndexFile,sHlpFile);
    if(strchr(sIndexFile,'.'))
        strcpy(strchr(sIndexFile,'.'),".idx");
    else strcat(sIndexFile,".idx");

    stat(sHlpFile,&file_stat);
    sprintf(sBuffer,"LASTTIME:%ld     [请不要修改此文件！]\n",file_stat.st_mtime);

    if(!(fpHlp=fopen(sHlpFile,"rt")))
    {
        return -1;
    }
    if(!(fpIndex=fopen(sIndexFile,"wb")))
    {
        fclose(fpHlp);
        return -2;
    }
    fwrite(sBuffer,strlen(sBuffer),1,fpIndex);

    nFlag=SearchHlpFile(fpHlp,fpIndex);
    fclose(fpHlp);
    fclose(fpIndex);

    if(nFlag<0)
    {
        outtext("\n帮助文件索引构造失败，返回码:%ld",nFlag);
        keyb();
    }
    return nFlag;
}


static long SearchHlpFile(FILE * fph,FILE * fpi)
{
    char sItem_end[81];
    char sItem[81];
    char sBuffer[MAX_LINE_LEN];
    char sOutput[300];
    char * start,* end;
    long nLocation,nErrFlag,nFind;
    memset(sItem_end,'\0',sizeof(sItem_end));


    nErrFlag=0;
    nFind=0;
    for(;;)
    {
        memset(sBuffer,'\0',sizeof(sBuffer));

        if(getline(sBuffer,fph)<=0) break;
        if(!(start=strchr(sBuffer,'<'))) continue;
        if(!(end=strchr(sBuffer,'>')))   continue;
        if(end<start) continue;
        if(nErrFlag=((end-start)>75)) break;
        if(nFind==0)
        {
                if(start[1]=='/')
                {
                      fseek(fph,ftell(fph)-(strlen(sBuffer)+1),SEEK_SET);
                      break;
                }

                nFind=1;
                nLocation=ftell(fph);
                memset(sItem,'\0',sizeof(sItem));
                memset(sItem_end,'\0',sizeof(sItem_end));

                strncpy(sItem,start+1,end-start-1);
                sprintf(sItem_end,"</%s>",sItem);
        }
        else
        {
                if(start[1]=='/')
                {
                      if(!strstr(sBuffer,sItem_end))
                      {
                        outtext("\n找不到帮助主题“%s”的结束标志“%s”或结束标志位置错误！",
                                sItem,sItem_end);
                        return -2;
                      }

                      if(strchr(sOutput,'{'))
                             strcpy(sOutput,"}\n");
                      else
                             sprintf(sOutput,"%s:%ld\n",sItem,nLocation);
                      fwrite(sOutput,strlen(sOutput),1,fpi);
                      nFind=0;
                }
                else
                {
                      fseek(fph,ftell(fph)-(strlen(sBuffer)+1),SEEK_SET);
                      sprintf(sOutput,"%s{\n",sItem);
                      fwrite(sOutput,strlen(sOutput),1,fpi);
                      if((nErrFlag=SearchHlpFile(fph,fpi))<0) break;
                }
        }

    }//end for;

    if(nErrFlag) return -1;
    return 1;
}

static void DrawHlpInterface(char * sHlpFile)
{

    FILE * fpIndex;
    char * start,* end;
    char sLine[MAX_LINE_LEN];
    long  line;

    if(fpIndex=fopen(sHlpFile,"rt"))
    {

        line=1;
        for(;;)
        {
                if(getline(sLine,fpIndex)<=0) break;
                start=strchr(sLine,'<');
                end=strchr(sLine,'>');
                if(start && end && end>start) break;
                gotoxy(1,line);
                line++;
                outtext("%s",sLine);
        }
        fclose(fpIndex);
    }

}


static MENU * CreateHlpMenu(char * sHlpFile)
{

    MENU * pMenu;
    char sIndexFile[81];
    FILE * fpIndex;
    char sLine[MAX_LINE_LEN];
    char sIndex[300];
    char * start;
    char * end;

    strcpy(sIndexFile,sHlpFile);
    if(strchr(sIndexFile,'.'))
        strcpy(strchr(sIndexFile,'.'),".idx");
    else strcat(sIndexFile,".idx");
    if(!(fpIndex=fopen(sIndexFile,"rt"))) return NULL;
    if(!(pMenu=CreateMenu("帮助",0)))
    {
        fclose(fpIndex);
        return NULL;
    }

    getline(sLine,fpIndex);
    for(;;)
    {
        memset(sLine,'\0',sizeof(sLine));
        if(getline(sLine,fpIndex)<=0) break;
        alltrim(sLine);
        if(start=strchr(sLine,':'))
        {
                end=start+1;
                start[0]='\0';
                sprintf(sIndex,"%s:%s",sHlpFile,end);
                BindMenu(pMenu,sLine,sIndex,ShowHlpContents);
        }
        else if(start=strchr(sLine,'{'))    //is a sub menu
        {
                start[0]='\0';
                BindSubMenu(pMenu,SearchHlpIndexFile(fpIndex,sLine,sHlpFile),NULL);
        }
        else     //the index file format error.
        {
                DropMenu(pMenu);
                fclose(fpIndex);
                return NULL;
        }

    }
    fclose(fpIndex);
    return pMenu;               //success

}


static MENU * SearchHlpIndexFile(FILE * fpIndex,char * sTitle,char * sHlpFile)
{

    MENU * pMenu;
    char sLine[MAX_LINE_LEN];
    char sIndex[300];
    char * start,* end;

    if(!(pMenu=CreateMenu(sTitle,0)))
    {
        return NULL;
    }

    for(;;)
    {
        memset(sLine,'\0',MAX_LINE_LEN);
        if(getline(sLine,fpIndex)<=0) break;
        alltrim(sLine);
        if(start=strchr(sLine,':'))
        {
                end=start+1;
                start[0]='\0';
                sprintf(sIndex,"%s:%s",sHlpFile,end);
                BindMenu(pMenu,sLine,sIndex,ShowHlpContents);
        }
        else if(start=strchr(sLine,'{'))    //is a sub menu
        {
                start[0]='\0';
                BindSubMenu(pMenu,SearchHlpIndexFile(fpIndex,sLine,sHlpFile),NULL);
        }
        else if(strchr(sLine,'}')) break;
        else
        {
                DropMenu(pMenu);
                return NULL;
        }
    }
    return pMenu;               //success
}

static void ShowHlpContents(long nType,char * sIndex,char * sHlpFile)
{

    char sLine[MAX_LINE_LEN];
    char sIndexFile[81];
    char sFinished[81];
    char sRealHlpFile[81];
    char * start,ch;
    long nOffset=0,nLoop;
    long nPagePos[4000];
    long nCurPage=0,nKey;
    long nRow,nBytes=0;
    FILE * fpr;


    start=strchr(sHlpFile,':');
    nOffset=atol(start+1);

    sprintf(sFinished,"</%s>",sIndex);
    memset(sRealHlpFile,'\0',sizeof(sRealHlpFile));
    strncpy(sRealHlpFile,sHlpFile,start-sHlpFile);


    if(nOffset<=0)
    {
          strcpy(sIndexFile,sHlpFile);
          if(strchr(sIndexFile,'.')) strcpy(strchr(sIndexFile,'.'),".idx");
          else strcat(sIndexFile,".idx");

          if(!(fpr=fopen(sIndexFile,"rt")))
          {
                outtext("\n无法打开帮助索引文件...");
                keyb();
                return;
          }

          for(;;)
          {
                if(getline(sLine,fpr)<=0) break;
                if(strstr(sLine,sIndex))  break;
          }

          fclose(fpr);
          if(strchr(sLine,'{'))
          {
                outtext("\n索引项“%s”不是一个纯节点！",sIndex);
                keyb();
                return;
          }

          start=strstr(sLine,sIndex);
          if(!start)
          {
                outtext("\n帮助文件中找不到帮助主题为“%s”的内容!",sIndex);
                keyb();
                return;
          }
          start+=(strlen(sIndex)+1);

          nOffset=atol(start);
    }


    clrscr();
    gotoxy(1,1);
    outtext("  帮助主题:  %s  ",sIndex);
    gotoxy(1,2);
    for(nLoop=1;nLoop<SCREEN_WIDTH;nLoop++) outtext("-");
    gotoxy(1,SCREEN_LINE-1);
    for(nLoop=1;nLoop<SCREEN_WIDTH;nLoop++) outtext("-");
    gotoxy(1,SCREEN_LINE);
    outtext("  使用上下光标键翻页，'P'键打印，ESC键返回...");


    if(!(fpr=fopen(sRealHlpFile,"rt")))
    {
        gotoxy(1,3);
        outtext("%d",strlen(sRealHlpFile));
        outtext("\n无法打开帮助文件%s",sRealHlpFile);
        keyb();
        return;
    }

    fseek(fpr,nOffset,SEEK_SET);
    nPagePos[0]=nOffset;
    nCurPage=0;
    for(;;)
    {

        clearline2(3,SCREEN_LINE-2);

        memset(sLine,'\0',sizeof(sLine));
        fseek(fpr,nPagePos[nCurPage],SEEK_SET);
        gotoxy(1,3);
        nRow=3;
        nBytes=0;


        for(;;)
        {
                ch=getc(fpr);

                if(strstr(sLine,sFinished)) ch=EOF;
                if(ch==EOF) break;
                if(nBytes<MAX_LINE_LEN-2)
                {
                        sLine[nBytes]=ch;
                        sLine[nBytes+1]='\0';
                }
                nBytes++;

                if((ch=='\n'&&nBytes<SCREEN_WIDTH)||nBytes>=SCREEN_WIDTH)
                {
                        gotoxy(1,nRow);
                        printf("%s",sLine);
                        nRow++;
                        nBytes=0;
                }
                if(nRow>=SCREEN_LINE-1) break;
        }

        for(;;)
        {
                nKey=keyb();

                if(nKey==KEY_ESC||nKey=='0')
                {
                        fclose(fpr);
                        return;
                }
                else if((nKey=='1'||nKey==KEY_UP||nKey==KEY_PAGEUP)&&nCurPage>0)
                {
                        nCurPage--;
                        break;
                }
                else if((nKey=='2'||nKey==KEY_DOWN||nKey==KEY_PAGEDOWN||nKey==KEY_RETURN))
                {
                        if(ch==EOF) continue;
                        nCurPage++;
                        nPagePos[nCurPage]=ftell(fpr);
                        break;
                }
                else if(nKey=='p'||nKey=='P')
                {
                        PrintHlpContents(nPagePos[0],sIndex,sRealHlpFile);
                        continue;

                }

        }//end for

   }
}

static void PrintHlpContents(long nOffset,char * sIndex,char *sHlpFile)
{


    char sEnd[81];
    char sLine[MAX_LINE_LEN];
    FILE * fpr;


    sprintf(sEnd,"</%s>",sIndex);

    if(!(fpr=fopen(sHlpFile,"rt"))) return;
    fseek(fpr,nOffset,SEEK_SET);


    setprinton();
    bigfont();
    output("帮助主题：%s",sIndex);
    normalfont();
    output("\n--------------------------------------------------------------------------\n");
    for(;;)
    {
    	if(getline(sLine,fpr)<=0) break;
    	if(strstr(sLine,sEnd)) break;
    	output("%s",sLine);
    }
    fclose(fpr);
    output("\n--------------------------------------------------------------------------\n");
    setprintoff();
}


/*--------------------------------------------------------------------
函数：long MessageBox；
功能：显示信息框并要求用户输入选择
参数：sTitle:信息提示内容
      nStyle:0:要求用户选择；
             1:只提示信息不需要选择，但需要用户按键确认；
             2:只提示信息不需要用户确认；

返回：用户按键
*--------------------------------------------------------------------*/

long MessageBox(char * sTitle,long nStyle)
{

     TABLE * pTab;
     long  nLength;
     long nLine;


     nLength=strlen(sTitle);
     nLine=nLength/56+((nLength%56)!=0);

     if(nLength<1||nLength>500) return 0;

     if(!(pTab=CreateTable(nLine,
        "|                                                        |"))) return 0;

     FillTable(pTab,1,"%s",sTitle);

     if(nStyle==0)
     {
         if(MergeTable(pTab,CreateTable(1,
                   "|   1.  确认['Y']  |   2.  否['N']  |   3.  放弃[ESC]  |"))<0)
         {
               DropTable(pTab);
               return 0;
         }
     }
     else if(nStyle==1)
     {
         if(MergeTable(pTab,CreateTable(1,
                   "|按任意键返回...                                         |"))<0)
         {
               DropTable(pTab);
               return 0;
         }
     }



     nLength=(SCREEN_LINE-(nLine*2+4))/2;
     clearline2(nLength,nLength+pTab->nLine*2+(nStyle!=2)*2);

     ShowTable(pTab,12,nLength);
     DropTable(pTab);
     if(nStyle>=2||nStyle<0) return 0;
     if(nStyle==1) return keyb();
     for(;;)
     {

        switch(keyb())
        {
          case '1':
          case 'y':
          case 'Y':
          case KEY_RETURN:

               return KEY_YES;

          case '2':
          case 'n':
          case 'N':

               return KEY_NO;

          case '3':
          case KEY_ESC:
               return KEY_ESC;
        }
     }
}
/*--------------------------------------------------------------------
函数：CreateDialog;
功能：建立对话框框架
参数：nWidth:对话框宽度
      nHeight:对话框高度
      nStyle:对话框风格；FRAME:带边框，NORMAL:不带连框
返回：指向一个对话框的指针
*--------------------------------------------------------------------*/

DIALOG * CreateDialog(long nWidth,long nHeight,long nStyle)
{

     DIALOG * pd;
     if(nWidth<10||nHeight<3||nWidth>SCREEN_WIDTH||nHeight>SCREEN_LINE) return NULL;

     if(!(pd=(DIALOG *)malloc(sizeof(DIALOG)))) return NULL;
     memset(pd,0,sizeof(DIALOG));
     if(!(pd->pct=CreateContainer("%d%d%d%s%d%s")))
     {
     	free(pd);
     	return NULL;
     }

     pd->nWidth=nWidth;
     pd->nHeight=nHeight;
     pd->npx=-1;
     pd->npy=-1;
     pd->nFlag=nStyle;

     return pd;

}
/*--------------------------------------------------------------------
函数：ShowDialog；
功能：显示对话框
参数：(npx,npy):对话框左上角所在的屏幕位置；
      pd：指向对话框的指针
返回：无
*--------------------------------------------------------------------*/

void  ShowDialog(DIALOG * pd,long npx,long npy)
{

    long nRow,nCol,nLoop,nFlag;
    long nLen,address,nprint;
    long nRet=0;
    char * sOffset,* start;
    char sValue[141];
    char sOutput[400];
    TEXTBOX rever;

    nCol=npx+2;
    nRow=npy+1;

    if(pd==NULL||npx<0||npx>SCREEN_WIDTH-10||npy>21||npy<0||
                 npx+pd->nWidth>SCREEN_WIDTH||npy+pd->nHeight>SCREEN_LINE)
    {

    	MessageBox("    对话框显示位置或大小超出屏幕范围，请确定显示位置横纵坐标和对话框大小相加后是否在屏幕范围内！",1);
    	return;
    }
    pd->npx=npx;
    pd->npy=npy;

    for(nprint=npy;nprint<=npy+pd->nHeight;nprint++)
    {
    	gotoxy(npx,nprint);
    	for(nRet=0;nRet<pd->nWidth;nRet++) outtext(" ");
    }


    if(pd->nFlag==FRAME) drawbox(npx,npy,npx+pd->nWidth,npy+pd->nHeight);
    memset(&rever,0,sizeof(TEXTBOX));
    for(nLoop=1;nLoop<=pd->pct->nCount;nLoop++)
    {

        memset(sValue,0,sizeof(sValue));
        memset(sOutput,0,sizeof(sOutput));

        ExportData(pd->pct,nLoop,"%d > > %s%d",&nFlag,sValue,&address);

        nRet=0;
        sOffset=sValue;
        while(sOffset[0])
        {
        	if(sOffset[0]=='\n') 	   nRet++;
        	sOffset++;
        }

        switch(nFlag)
        {
             case SPACELINE:
                  nRow++;
                  nCol=npx+2;
                  ImportData(pd->pct,nLoop,"> %d %d > >",nRow,nCol);
                  nRow+=address-1;
                  break;

             case STATIC_TEXT:
                  if((nCol+strlen(sValue))>(pd->nWidth+npx+2))
                  {
                  	nCol=npx+2;
                  	nRow++;
                  }
                  ImportData(pd->pct,nLoop,"> %d %d > >",nRow,nCol);
                  gotoxy(nCol,nRow);
                  outtext(sValue);
                  nCol+=strlen(sValue);
                  break;

             case SEPARATOR:
                  sValue[2]='\0';
                  nRow++;
                  nCol=npx+2;
                  ImportData(pd->pct,nLoop,"> %d %d > >",nRow,nCol);
                  gotoxy(nCol,nRow);
                  for(nprint=2;nprint<pd->nWidth-1;nprint+=2) outtext(sValue);
                  nRow++;
                  break;

             case OUTPUT:
             case INPUT:
                  start=strchr(sValue,'%');
                  if(!start) break;
                  sOffset=start+1;
                  nLen=0;
                  while((sOffset[0]>='0' && sOffset[0]<='9')||sOffset[0]=='-')
                  {
                        if(sOffset[0]!='-')
                  	     nLen=nLen*10+sOffset[0]-'0';
                  	sOffset++;
                  }

                  rever.nLen=nLen;
                  nLen+=start-sValue;
                  if((nLen+nCol)>(pd->nWidth+npx+2))
                  {
                  	nCol=npx+2;
                  	nRow++;
                  }
                  ImportData(pd->pct,nLoop,"> %d %d > >",nRow,nCol);
                  start[0]='\0';
                  gotoxy(nCol,nRow);
                  outtext(sValue);
                  if(nFlag==INPUT)
                  {
                        rever.x=nCol+strlen(sValue);
                        rever.y=nRow;
                        ReverseXY(&rever);
                        rever.x=1;
                        rever.y=26;
                        rever.nLen=1;
                        memset(rever.sValue,0,sizeof(rever.sValue));
                        ShowXY(&rever);
                  }

                  nCol+=nLen;
                  break;


       }
       if(nRet)
       {
       	    nRow+=nRet;
       	    nCol=npx+2;
       }
    }//end for


}
/*--------------------------------------------------------------------
函数：AddDialog
功能：向对话框中加入控件
参数：
      sField:别名，若不需要可为NULL，
      nWhat:加入哪一种控件,已有控件如下（第一种控件都有不同的参数）：

      值            意义           参数             说明
      STATIC_TEXT   静态文本；     文本字符串
      SEPARATOR     分隔符；       分隔字符样式     由两个字符组成
      INPUT         输入区；       提示语及类型，相应参数地址
      OUTPUT        输出区；       提示语及类型，相应参数地址
      SPACELINE     空行；         产生空行的行数


返回：>0成功，否则失败

例子：
      AddDialog(pd,"static",STATIC_TEXT,"这是静态字符...\n");
      AddDialog(pd,"in",INPUT,"请输入一个双精度型：%10.2f",&dValue);
      AddDialog(pd,"out",OUTPUT,"这里显示一个整型：%10d",&nValue);
      AddDialog(pd,"sep1",SPACELINE,5);    //插入5个空行
      AddDialog(pd,"sep2",SEPARATOR,"**"); //这里将由“**”组成一个分隔行
*--------------------------------------------------------------------*/

long AddDialog(DIALOG * pd,char * sField,long nWhat,...)
{

    va_list pVar;
    long nValue,sValue;
    char sName[31];

    if(pd==NULL) return 0;

    if(sField)
    {
    	strcpy(sName,sField);
    	upper(sName);
    	alltrim(sName);
    }
    else strcpy(sName,"\0");

    va_start(pVar,nWhat);
    switch(nWhat)
    {
    	case STATIC_TEXT:
    	case SEPARATOR:
    	     ImportData(pd->pct,pd->pct->nCount+1,"%d%d%d%s%d%s",
    	                nWhat,0,0,va_arg(pVar,char *),0,sName);
    	     break;

        case SPACELINE:
    	     ImportData(pd->pct,pd->pct->nCount+1,"%d%d%d%s%d%s",
    	                nWhat,0,0,"NULL",va_arg(pVar,long),sName);
    	     break;

    	case INPUT:
    	case OUTPUT:
    	     sValue=va_arg(pVar,long);
    	     nValue=va_arg(pVar,long);
    	     ImportData(pd->pct,pd->pct->nCount+1,"%d%d%d%s%d%s",
    	                nWhat,0,0,sValue,nValue,sName);
    	     break;

        default:
             va_end(pVar);
             return 0;
     }
     va_end(pVar);
     return  pd->pct->nCount;

}

/*--------------------------------------------------------------------
函数：ActiveDialog；
功能：激活一个输入控件
参数：
      pd:对话框
      nWhere:目的控件别名在别名字符串中的序号，以1开始；
      sField:别名字符串列表，若有多个别名则用“，”分开
返回：用户按键
*--------------------------------------------------------------------*/

long ActiveDialog(DIALOG * pd,long nWhere,char * sField)
{

     long nCol,nRow,nFlag,address;
     long nKey,nLine,nLoop;
     char sValue[150],sName[31];
     char * sOffset;

     if(pd==NULL||nWhere<=0) return 0;

     nLine=nWhere;
     sOffset=sField;
     while(--nLine)
     {
     	 while(sOffset[0] && sOffset[0]!=',') sOffset++;
         if(!sOffset[0]) return -1;
     	 sOffset++;
     }

     nLoop=0;
     memset(sName,'\0',sizeof(sName));

     while(sOffset[0] && sOffset[0]!=',')
     {
     	sName[nLoop++]=sOffset[0];
     	sOffset++;
     }

     nLine=TranslateDialogFields(pd,sName);
     if(nLine<=0) return -2;

     ExportData(pd->pct,nLine,"%d%d%d%s%d",
                   &nFlag,&nRow,&nCol,sValue,&address);

     switch(nFlag)
     {
     	case INPUT:
     	     nKey=GetValue(nCol,nRow,sValue,address);
     	     RefreshDialog(pd,sName);
     	     return nKey;

     }
     return 0;

}
/*--------------------------------------------------------------------
函数：RefreshDialog；
功能：将相应数据按格式填入到对话框的相应位置上
参数：
      sField:目标控件的别名，若为NULL则表示全部刷新。
返回：无
*--------------------------------------------------------------------*/

void RefreshDialog(DIALOG * pd,char * sField)
{

     TEXTBOX rever;
     long nCol,nRow,nFlag,address;
     long nCount,nWhere;
     char sValue[INPUT_STRING_SIZE+1];
     char * start;

     nWhere=0;
     if(sField)
     {
        nWhere=TranslateDialogFields(pd,sField);
        if(nWhere<=0) return;
     }
     for(nCount=1;nCount<=pd->pct->nCount;nCount++)
     {
     	ExportData(pd->pct,nCount,"%d%d%d%s%d",
     	           &nFlag,&nRow,&nCol,sValue,&address);
        if(nWhere==0||nWhere==nCount)
        {
             switch(nFlag)
             {

             	 case INPUT:
             	 case OUTPUT:
     	              if(nFlag==INPUT)
     	              {
     	              	 start=strchr(sValue,'%');
     	                 if(!start) break;
       	                 rever.x=nCol+(start-sValue);
      	                 rever.y=nRow;
     	              }
     	              else start=sValue;

                      if(strchr(start,'s'))
                            sprintf(rever.sValue,start,(char *)address);
                      else if(strchr(start,'f'))
                            sprintf(rever.sValue,start,*((double *)address));
                      else if(strchr(start,'d'))
                            sprintf(rever.sValue,start,*((long *)address));

     	              start++;
     	              rever.nLen=0;

                      while((start[0]>='0'&&start[0]<='9')||start[0]=='-')
                      {
             	            if(start[0]!='-')
             	                  rever.nLen=rever.nLen*10+start[0]-'0';
             	            start++;
                      }
                      if(nFlag==OUTPUT)
                      {
                      	    gotoxy(nCol,nRow);
                      	    outtext(rever.sValue);
                      }
                      else
                      {
                            ReverseXY(&rever);
                            memset(rever.sValue,'\0',sizeof(rever.sValue));
                            rever.nLen=1;
                            rever.y=26;
                            ShowXY(&rever);
                      }
                      break;

             }
             if(nWhere==nCount) return;
        }

     }

}
/*--------------------------------------------------------------------
函数：SetDialogInfo；
功能：在对话框的最底部显示用户信息
参数：pd:对话框指针
      sInfo:信息字符串指针
返回：无
*--------------------------------------------------------------------*/

void  SetDialogInfo(DIALOG * pd,char * sInfo)
{

     char smsg[81];
     if(pd==NULL||pd->npx<=0||pd->npy<=0) return;
     memset(smsg,'\0',81);
     memset(smsg,' ',pd->nWidth-2);
     gotoxy(pd->npx+2,pd->npy+pd->nHeight-1);
     outtext(smsg);
     strncpy(smsg,sInfo,pd->nWidth-2);
     gotoxy(pd->npx+2,pd->npy+pd->nHeight-1);
     outtext(smsg);
     fflush(stdout);

}

/*--------------------------------------------------------------------
函数：GetDialogPosition；
功能：获得对话框中某个控件的开始屏幕位置
参数：pd:对话框；
      sField：控件别名
      pnRow：回传行位置指针；
      pnCol：回传列位置指针；
返回：>0成功  <0失败
*--------------------------------------------------------------------*/
long  GetDialogPosition(DIALOG * pd,char * sField,long * pnRow,long * pnCol)
{
     long nWhere;
     if(pd==NULL) return 0;

     nWhere=TranslateDialogFields(pd,sField);
     return ExportData(pd->pct,nWhere,"> %d %d > >",pnRow,pnCol);

}

static long TranslateDialogFields(DIALOG * pd,char * sField)
{
     char sName[31],buf[50];
     long nWhere;
     if(!pd||!sField) return -1;

     memset(sName,'\0',sizeof(sName));
     strcpy(sName,sField);
     alltrim(sName);
     upper(sName);
     nWhere=FindDataTable(pd->pct,1,"> > > > > %s",sName);
     if(nWhere<=0)
     {
     	sprintf(buf,"非法的对话框别名:%s",sName);
     	MessageBox(buf,1);
     }
     return nWhere;
}

/*--------------------------------------------------------------------
函数：DropDialog；
功能：释放一个对话框资源
参数：pd:对话框
返回：无
*--------------------------------------------------------------------*/
void DropDialog(DIALOG * pd)
{
     if(pd==NULL) return;
     if(pd->pct) DropContainer(pd->pct);
     free(pd);

}


//其它
/*-------------------------------------------------*
函数:FormSqlWithArgs;
功能:根据源SQL语句和参数列表形成最终的SQL语句；
参数:source:源SQL语句
     va_list:变长参数指针
     dest:目标串指针;
*-------------------------------------------------*/

long FormSqlWithArgs(char * source,va_list pvar,char * dest)
{

    char * sSearch;
    char * sForm;
    long nError;

    nError=0;
    sSearch=source;
    sForm=dest;

    while(sSearch[0]!='\0')
    {
        switch(sSearch[0])
        {
           case '%':
                sSearch++;
                switch(sSearch[0])
                {
                    case 's':
                         sprintf(sForm,"'%s'",va_arg(pvar,char *));
                         while(sForm[0]!='\0') sForm++;
                         sSearch++;
                         break;
                    case 't':
                         sprintf(sForm,"%s",va_arg(pvar,char *));
                         while(sForm[0]!='\0') sForm++;
                         sSearch++;
                         break;
                    case 'l':
                         sSearch++;
                         switch(sSearch[0])
                         {
                             case 'f':
                                  sprintf(sForm,"%-.2lf",va_arg(pvar,double));
                                  while(sForm[0]!='\0') sForm++;
                                  sSearch++;
                                  break;

                             case 'd':
                                  sprintf(sForm,"%ld",va_arg(pvar,long));
                                  while(sForm[0]!='\0') sForm++;
                                  sSearch++;
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
                sForm[0]=sSearch[0];
                sForm++;
                sSearch++;
                break;
        }//end switch
        if(nError)  return -1;

    }//end while
    return 1;
}



/*-------------------------------------------------*
函数:ASCIItoBCD;
功能:将十六进制的ASCII码字符串变成BCD码
参数:sASC:ASCII字符串;
     nLen:ASCII字符串长度[偶数];
     sDest:目标串指针;
注意:被转换后的字符串长度只有原来的一半[nLen/2];
*-------------------------------------------------*/

long ASCIItoBCD(char * sASC,int nLen,char * sDest)
{

     unsigned char cCode;
     char sTable[17]="0123456789ABCDEF";
     int nSrc,nLoop;

     nLoop=0;
     nSrc=0;

     if(nLen<=0) return 1;
     for(nLoop=0;nLoop<nLen;nLoop+=2)  //将十六进制的代码转换成ASCII码
     {
     	cCode=sASC[nLoop];
        if(!strchr(sTable,cCode)) return 1;
        sDest[nSrc]=strchr(sTable,cCode)-sTable;  //得到高位十六进制数据
        sDest[nSrc]<<=4;                        //移向高四位

        cCode=sASC[nLoop+1];                  //得到低四位数据
        if(!strchr(sTable,cCode)) return 1;
        sDest[nSrc]|=(strchr(sTable,cCode)-sTable)&0x0f;//加入低四位数据

        nSrc++;
     }
     sDest[nSrc]='\0';
     return 0;

}

/*-----------------------------------------------------*
函数:BCDtoASCII;
功能:将二进制BCD码转换成一个以十六进制表示的字符串;
参数:sBCD:BCD码原串;
     nLen:BCD码串长度;
     sDest:被转换后的字符串;
注意:目标串的长度是源串长度的2倍;
*----------------------------------------------------*/

long BCDtoASCII(char * sBCD,int nLen,char * sDest)
{
     unsigned char cCode;
     int nLoop,nSrc;
     char sTable[17]="0123456789ABCDEF";

     nLoop=0;
     nSrc=0;

     if(nLen<=0) return 1;
     for(nLoop=0;nLoop<nLen;nLoop++)
     {
          cCode=sBCD[nLoop];
          sDest[nSrc+1]=sTable[cCode&0x0F];
          cCode>>=4;
          sDest[nSrc]=sTable[cCode&0x0F];
          nSrc+=2;
     }
     sDest[nSrc]='\0';
     return 0;

}
