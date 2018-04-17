#include<string.h>
#include "setting.h"

/*--------------------------------------------------------------------
函数：AddSetItem；
功能：增加一个设置项
参数: pst:由CreateSetTable建立的设置表指针；
      sTitle:被设置参数的文字说明；
      sField:被修改表的列名
      nLength:列名长度
      sWhere:确定参数列的条件
      ...：条件所带的参数
      
列子：
      SETTABLE * pst;
      pst=CreateSetTable(10);  //建立一个可每页可容纳10个设置项的设置表格
      AddSetItem(pst,"本局局名","ccs",20,"from dl$dlywcs where nbh=%ld",3);
      //加入一个设置项，被修改的字段为CCS  意义为局名  长度为20  从表DL$DLYWCS第3号参数取出
      LoadDefaultSet(pst);  //将数据库中原有的值导入设置表中
      BeginSetting(pst,4);//开始设置，并将设置表的顶行定位在屏幕的第4行处
      DropSetTable(pst);
          
返回：无
*--------------------------------------------------------------------*/
long AddSetItem(SETTABLE * pst,char * sTitle,char * sField,long nLength,char * sWhere,...)
{
     va_list pvar;
     char sSql[256];

     va_start(pvar,sWhere);
     memset(sSql,0,sizeof(sSql));
     if(FormSqlWithArgs(sWhere,pvar,sSql)>0)
     {
        va_end(pvar);
        return ImportData(pst,pst->nCount+1,"%s%s%s%s%d%d",sTitle,".",sField,sSql,nLength,0);
     }
     va_end(pvar);
     return -1;
}

/*--------------------------------------------------------------------
函数：BeginSetting；
功能: 显示设置表并开始进入设置循环
参数：pst:设置表对象指针
      nLine:表示从屏幕第几行开始显示设置表
*--------------------------------------------------------------------*/
long BeginSetting(SETTABLE * pst,long nLine)
{
     long nCurrent,nLength,nModifyFlag,nKey,nValueLen=0,nTitleLen=0,npx;
     char sFormat[21],sTitle[256];
  
     if(!pst->nCount) return 0;
     for(nCurrent=1;nCurrent<=pst->nCount;nCurrent++)
     {
     	ExportData(pst,nCurrent,"%s > > > %d",sTitle,&nLength);
     	alltrim(sTitle);
     	if(nLength>nValueLen) nValueLen=nLength;
        nLength=strlen(sTitle);
     	if(nLength>nTitleLen) nTitleLen=nLength;
     }

     if(nValueLen<6) nValueLen=6;
     if(nTitleLen<8) nTitleLen=8;
     
     nTitleLen+=(nTitleLen%2);
     nValueLen+=(nValueLen%2);
     sprintf(pst->sFormat,"%%-%ld.%lds %%-%ld.%lds %%s %%s %%d %%d",nTitleLen,nTitleLen,nValueLen,nValueLen);
     
     memset(sTitle,'\0',sizeof(sTitle));
     memset(sTitle,' ', nTitleLen+nValueLen+2);
     memcpy(sTitle,"|参数意义",9);
     memcpy(sTitle+nTitleLen+1,"|参数值",7);
     sTitle[nTitleLen+nValueLen+2]='|';
     
     if(!(pst->pTable=CreateTable(9,sTitle))) return 0; 
     pst->nPage=8;
     npx=((SCREEN_WIDTH-(nTitleLen+nValueLen))/2)-2;
     if(npx<2) npx=2;


     RefreshDataTable(pst);
     ShowDataTable(pst,npx,nLine);
     nCurrent=1;
     nModifyFlag=0;
     for(;;)
     {
     	
     	ExportData(pst,nCurrent,"> > > > %d",&nLength);
     	sprintf(sFormat,"> %%-%ld.%lds",nLength,nLength);
    
        if(pst->pTable->npx<0)
        {
            if(nLength<=22)
            {
        	clearline2(nLine,pst->pTable->nLine*2+5);
        	ShowDataTable(pst,2,nLine);
            } 
        }
       
     	switch(FetchData(pst,nCurrent,2,sFormat))
     	{
     		case KEY_ESC:
                     if(nModifyFlag)
                     {
                     	nKey=MessageBox("  设置的内容已经修改，要保存吗？",0);
                     	if(nKey==KEY_ESC)
                     	{
                     		ShowDataTable(pst,2,nLine);
                     		continue;       
                     	}
                     	if(nKey==KEY_YES) SaveSetValue(pst);
                     }
                     break;
                case KEY_UP:
                     if(nCurrent>1) nCurrent--;
                     continue;
                case KEY_RETURN:
                     nModifyFlag=1;
                     ImportData(pst,nCurrent,"> > > > > %d",1);
                case KEY_DOWN:     
                     if(nCurrent<pst->nCount) nCurrent++;
                     continue;
                default:continue;
        }
        break;             
     }   
     return 1;
}
/*--------------------------------------------------------------------
函数：LoadDefaultSet；
功能: 将表中的原有数据导入到设置表中
参数：pst:设置表对象指针
返回：大于零成功
*--------------------------------------------------------------------*/
long LoadDefaultSet(SETTABLE * pst)
{
     char sWhere[256],sValue[81],sField[31];
     long nCount;
     
     if(pst==NULL)   return -1;
     
     SetTableStyle(pst,ST_ALLTRIM,1);
     nCount=pst->nCount;
     while(nCount)
     {
     	ExportData(pst,nCount,"> > %s %s",sField,sWhere);
     	RunSelect("select %t %t into %s",sField,sWhere,sValue);
     	alltrim(sValue);
     	ImportData(pst,nCount,"> %s",sValue);
     	nCount--;
     }	
     CommitWork();
     return 1;
}

/*--------------------------------------------------------------------
函数：SaveSetValue；
功能: 将新设置的内容存入表中
参数：pst:设置表对象指针
返回：大于零成功
*--------------------------------------------------------------------*/
static long SaveSetValue(SETTABLE * pst)
{
     char sWhere[256],sValue[81],sField[31],sSql[512],sTableName[31];
     long nCount,nModifyFlag;
     char * stable;
     
     if(pst==NULL)   return -1;
     
     nCount=pst->nCount;
     while(nCount)
     {
     	ExportData(pst,nCount,"> %s %s %s > %d",sValue,sField,sWhere,&nModifyFlag);
     	if(nModifyFlag)
     	{
     	      stable=strstr(sWhere,"from");
     	      if(!stable) stable=strstr(sWhere,"FROM");
     	      if(!stable) continue;
     	      stable+=4;
     	      while(stable[0]==' ') stable++;
     	      memset(sTableName,'\0',sizeof(sTableName));
     	      strncpy(sTableName,stable,strchr(stable,' ')-stable);
     	      while(stable[0]!=' ') stable++;
              alltrim(sValue);
     	      sprintf(sSql,"update %s  set %s='%s' %s",sTableName,sField,sValue,stable);
     	      RunSql(sSql);
     	}     
     	nCount--;
     }	
     CommitWork();
     return 1;
}
