#include<string.h>
#include "setting.h"

/*--------------------------------------------------------------------
������AddSetItem��
���ܣ�����һ��������
����: pst:��CreateSetTable���������ñ�ָ�룻
      sTitle:�����ò���������˵����
      sField:���޸ı������
      nLength:��������
      sWhere:ȷ�������е�����
      ...�����������Ĳ���
      
���ӣ�
      SETTABLE * pst;
      pst=CreateSetTable(10);  //����һ����ÿҳ������10������������ñ��
      AddSetItem(pst,"���־���","ccs",20,"from dl$dlywcs where nbh=%ld",3);
      //����һ����������޸ĵ��ֶ�ΪCCS  ����Ϊ����  ����Ϊ20  �ӱ�DL$DLYWCS��3�Ų���ȡ��
      LoadDefaultSet(pst);  //�����ݿ���ԭ�е�ֵ�������ñ���
      BeginSetting(pst,4);//��ʼ���ã��������ñ�Ķ��ж�λ����Ļ�ĵ�4�д�
      DropSetTable(pst);
          
���أ���
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
������BeginSetting��
����: ��ʾ���ñ���ʼ��������ѭ��
������pst:���ñ����ָ��
      nLine:��ʾ����Ļ�ڼ��п�ʼ��ʾ���ñ�
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
     memcpy(sTitle,"|��������",9);
     memcpy(sTitle+nTitleLen+1,"|����ֵ",7);
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
                     	nKey=MessageBox("  ���õ������Ѿ��޸ģ�Ҫ������",0);
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
������LoadDefaultSet��
����: �����е�ԭ�����ݵ��뵽���ñ���
������pst:���ñ����ָ��
���أ�������ɹ�
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
������SaveSetValue��
����: �������õ����ݴ������
������pst:���ñ����ָ��
���أ�������ɹ�
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
