/***********************************************************************
**filename	�ļ���:filechar.c
**target	Ŀ��:�ļ����ַ�������ʵ���ļ�
**create time	��������: 2001.2.19
**original programmer	����:�²�[chenbo]
**edit  history	�޸���ʷ:
**date     editer    reson
** 
****************************************************************************/

#include"filechar.h"
extern long nSysSfm;
extern char sTcpipErrInfo[81];
/*-----------------------------------------------------------------

����:long getline(char * sline,FILE * fp)
Ŀ��:���ļ������һ�����ݵ�ָ���������Իس�����β
����:sline:������ָ�룬fp:�ļ�ָ��
����:=0�Ѿ�ȡ��   <0ʧ��

*-----------------------------------------------------------------*/

long getline(char * sline,FILE * fp) 
{
   char ch=0;
   long nRec=0;
           
   #ifdef UNIX            
   while(ch!='\n' && ch!=EOF)
   #else
   while(ch!='\n' && !feof(fp))
   #endif
   {
         if (nRec>=MAX_LINE_LEN) break;
         ch=getc(fp);
         sline[nRec++]=ch;
   }    

   if(nRec>=MAX_LINE_LEN) return -1;

   sline[nRec]='\0';                                                          
   if(ch=='\n') return 1;
   #ifdef UNIX
   if(ch==EOF)  return 0;
   #else
   if(feof(fp)) return 0;
   #endif
   return -1;
}       


/*-----------------------------------------------------------------
����:GetTempFileName
Ŀ��:���һ����ʱ�ļ���(�뵱ǰĿ¼�µ��ļ������ظ���
����:sfilename:���ص���ʱ�ļ����ַ���������
����:>0�ɹ�
*-----------------------------------------------------------------*/
long GetTempFileName(char * sfilename)
{
    
    char random=0;
    GetSysLkrq();
    for(;;)
    {
    	sprintf(sfilename,"%08ld.tmp",nSysSfm*100+random++);
    	if(access(sfilename,0)) break;
    }	
    return 1;

}


/*--------------------------------------------------------------------
������GetValueFromFile��
����: ���ļ�filename��ȡ������Ϊindex��ֵ
������filename:�ļ���
      index:����,�������������(%s,%ld,%lf)
      ...:���ز���
���أ�>0���ɹ�
      =0��û��ƥ���ֵ
      <0������ʧ��
          -1��û�й涨�������ͣ�����ﲻ��ȷ��
          -2��Ŀ���ļ�������
          -3���޷����ļ�
*--------------------------------------------------------------------*/
long GetValueFromFile(char * filename,char * index,...)
{
     FILE * fp;
     va_list pvar;
     char buf[MAX_LINE_LEN],indexitem[81];
     char * argtype,* value;
     long * pArg; 

     va_start(pvar,index);
     pArg=va_arg(pvar,long *);
     va_end(pvar);
     memset(indexitem,'\0',sizeof(indexitem));
     strncpy(indexitem,index,80);
     argtype=strchr(indexitem,'%');
     if(!argtype) return -1;
     argtype[0]='\0';
     argtype++;
     if(access(filename,0))      return -2;
     if(!(fp=fopen(filename,"rt"))) return -3;
     
     for(;;)
     {
     	memset(buf,'\0',sizeof(buf));
     	if(getline(buf,fp)<=0) break;
     	alltrim(buf);
     	value=buf;
     	while(value[0]!='\n') value++;
     	value[0]='\0';
     	value=buf;
     	value+=strlen(indexitem);
     	if(!strncmp(buf,indexitem,strlen(indexitem)))
     	{
     		fclose(fp);
     		if(argtype[0]=='d'||(argtype[0]=='l' && argtype[1]=='d'))
     		       (*pArg)=atol(value);
     		else if(argtype[0]=='f'||(argtype[0]=='l' && argtype[1]=='f'))
     		       (*((double *)pArg))=atof(value);
     		else if(argtype[0]=='s')
     		       strcpy((char*)pArg,value);
     		else return -1;
     		return 1; //successfully
     	}
     }//end for
     fclose(fp);
     return 0;  //no data found  
}

/*--------------------------------------------------------------------
������PutValueToFile��
����: ����filename��index���ֵ
������filename:�ļ���
      index:����,�������������(%s,%ld,%lf)
      ...:���ز���
���أ�>=0���ɹ�
          >0��������ֵ�����ѳɹ��޸�
          =0��û��ƥ���ֵ,���ɹ������ļ�β�����Ӹ���
      <0������ʧ��
          -1��û�й涨�������ͣ�����ﲻ��ȷ��
          -2��Ŀ���ļ�������
          -3���޷����ļ�
          -4���ļ���дʧ��
*--------------------------------------------------------------------*/
long PutValueToFile(char * filename,char * index,...)
{

     FILE * fpold,* fpnew;
     va_list pvar;
     char buf[MAX_LINE_LEN],indexitem[81],sSwapFileName[31];
     char * value;
     long nRetVal,nChanged;

     memset(indexitem,'\0',sizeof(indexitem));
     strncpy(indexitem,index,80);
     if(!strchr(indexitem,'%')) return -1;
     value=indexitem;
     while(value[0]!='%') value++;
     value[0]='\0';
     if(GetTempFileName(sSwapFileName)<0) return -2;
     if(access(filename,0))      return -2;
     if(!(fpold=fopen(filename,"rt"))) return -3;
     if(!(fpnew=fopen(sSwapFileName,"wt"))) 
     {
     	fclose(fpold);
     	return -3;
     }
     nRetVal=1;
     nChanged=0;
     while(nRetVal>0)
     {
     	memset(buf,'\0',sizeof(buf));
     	nRetVal=getline(buf,fpold);
     	alltrim(buf);
     	if((!strncmp(buf,indexitem,strlen(indexitem))||nRetVal<=0)&&nChanged==0)
     	{
                //ƥ��ɹ�
                va_start(pvar,index);
                vsprintf(buf,index,pvar);
                va_end(pvar);
                value=buf;
                while(value[0]!='\0') value++;
                value[0]='\n';
                value[1]='\0';
                nChanged=1;
     	}
     	//������ֵд�����ļ���
        if(nRetVal>0) fwrite(buf,strlen(buf),1,fpnew);
     }//end while
     fclose(fpold);
     fclose(fpnew);
     if(rename(sSwapFileName,filename)==0) return 1;
     return -4;  //�ļ�����ʧ��  
}


/*-----------------------------------------------------------------

����:long FetchFile;
Ŀ��:��ȡ�����ļ�
����:datafp:�Ѿ��򿪵������ļ�ָ��   sFormat:��ʽ���ַ���
����:>0�ɹ�    <0ʧ��     =0�ļ��Ѷ���

*-----------------------------------------------------------------*/

long FetchFile(FILE * datafp,char * sFormat,...)
{
  va_list pVar;
  long  nLen=0,nError=0,nPos,nSeparator;
  char ch,sData[MAX_LINE_LEN];
  double  * pDouble;
  long    * pLong;
  char    * pChar;

  if(datafp==NULL) return -1;                  //�ļ���δ��
  if(strchr(sFormat,'%')==NULL) return -2;     //û��ָ��������ʽ
  
  va_start(pVar,sFormat);
     
  while((sFormat=strchr(sFormat,'%'))!=NULL)
  {
     nLen=0;
     sFormat++;
     nSeparator=0;
     
     if(sFormat[0]<='9' && sFormat[0]>='0')             //���ֶ�Ϊ�������ݸ�ʽ
     {
        while(sFormat[0]<='9' && sFormat[0]>='0')       //�õ�����
        {
           nLen=nLen*10+(sFormat[0]-'0');
           sFormat++;
           
        }
     }  
     else                                               //���ֶ�Ϊ�ָ�����ʽ
     {
        nLen=MAX_LINE_LEN;
        nSeparator=sFormat[1];
        if(nSeparator=='%') nSeparator=' ';
        
     }  
     if(nLen>MAX_LINE_LEN)                              //�涨�ĳ��ȳ�������������
     {
        nError=3;
        break;
     }

     switch(sFormat[0])
     {
        case 'd':                                       //������ʮ������ 
            pLong=va_arg(pVar,long *);
            nPos=0;
            while(nLen--)
            {
                 ch=getc(datafp);
                 if(ch==nSeparator) break;
                 if((ch<'0' || ch>'9') && ch!=' ' && ch!='-')    //��������������
                 {
                     nError=4;
                     break;
                 }      
                 sData[nPos++]=ch;
            }
            if(nPos!=0)
            {
                 sData[nPos]='\0';
                 alltrim(sData);
                 if(strlen(sData)==0 || strchr(sData,' ')) strcpy(sData,"0\0");
                 if(nError==0) (* pLong)=atol(sData);
            }     
            break;
        case 'f':
            nPos=0;
            pDouble=va_arg(pVar,double *);                      //˫����ʮ������
            while(nLen--)
            {
                 ch=getc(datafp);
                 if(ch==nSeparator) break;
                 if((ch<'0' || ch>'9') && ch!='.' && ch!=' ' && ch!='-')        //��������������         
                 {
                     nError=4;
                     break;
                 }
                 sData[nPos++]=ch;

            }
            if(nPos!=0)
            {
                 sData[nPos]='\0';
                 alltrim(sData);
                 if(strlen(sData)==0 || strchr(sData,' ')) strcpy(sData,"0\0");
                 if(nError==0) (* pDouble)=atof(sData);
            }
            break;

         case 's':                                              //�ַ���
            pChar=va_arg(pVar,char *);
            nPos=0;
            while(nLen--)
            {   
                ch=getc(datafp);
                if(ch==nSeparator) break;
                pChar[nPos++]=ch;
            }
            if(nPos!=0)
            {   
                pChar[nPos]='\0';
                alltrim(pChar);
            }    
            break;
         default:                                     //ָ�����������ʹ���
            nError=2;
            break;
      }//end switch         
    if(nError>0) break;                              //���ĳһ������������������
   }//end while             
  va_end(pVar);
  if(ch==EOF) return 0;                              //�ļ����
  if(ch=='\n') return 1;                             //����
  return -1;
}


/*-----------------------------------------------------------------

����:char * FetchStringToArg;
Ŀ��:����ַ���
����:sString:�ַ���ָ��  sFormat:��ʽ���ַ���
����:��������ȡ���ַ�λ�ã����򷵻�NULL
*-----------------------------------------------------------------*/
char * FetchStringToArg(char * sString,char * sFormat,...)
{

  va_list pVar;
  long  nLen=0,nError=0,nPos,nSeparator;
  char ch,sData[MAX_LINE_LEN];
  double  * pDouble;
  long    * pLong;
  char    * pChar;
 

  if(sString==NULL) return NULL;                
  if(strchr(sFormat,'%')==NULL) return NULL;     //û��ָ��������ʽ
  
  va_start(pVar,sFormat);
     
  while((sFormat=strchr(sFormat,'%'))!=NULL)
  {
     nLen=0;
     sFormat++;
     nSeparator=0;
     
     if(sFormat[0]<='9' && sFormat[0]>='0')             //���ֶ�Ϊ�������ݸ�ʽ
     {
        while(sFormat[0]<='9' && sFormat[0]>='0')       //�õ�����
        {
           nLen=nLen*10+(sFormat[0]-'0');
           sFormat++;
           
        }
     }  
     else                                               //���ֶ�Ϊ�ָ�����ʽ
     {
        nLen=MAX_LINE_LEN;
        nSeparator=sFormat[1];
        if(nSeparator=='%') nSeparator=' ';             //�����ָ���ָ�������Ĭ��Ϊ�ո�
     }  
     if(nLen>MAX_LINE_LEN)                              //�涨�ĳ��ȳ�������������
     {
        nError=3;
        break;
     }

     switch(sFormat[0])
     {
        case 'd':                                       //������ʮ������ 
            pLong=va_arg(pVar,long *);
            nPos=0;
            while(nLen--)
            {
                 ch=sString[0];
                 sString++;
                 if(ch==nSeparator) break;
                 if((ch<'0' || ch>'9') && ch!=' ' && ch!='-')    //��������������
                 {
                     nError=4;
                     break;
                 }      
                 sData[nPos++]=ch;
            }
            if(nPos!=0)
            {
                 sData[nPos]='\0';
                 alltrim(sData);
                 if(strlen(sData)==0 || strchr(sData,' ')) strcpy(sData,"0\0");
                 if(nError==0) (* pLong)=atol(sData);
            }     
            break;
        case 'f':
            nPos=0;
            pDouble=va_arg(pVar,double *);                      //˫����ʮ������
            while(nLen--)
            {
                 ch=sString[0];
                 sString++;
                 if(ch==nSeparator) break;
                 if((ch<'0' || ch>'9') && ch!='.' && ch!=' ' && ch!='-')        //��������������         
                 {
                     nError=4;
                     break;
                 }
                 sData[nPos++]=ch;

            }
            if(nPos!=0)
            {
                 sData[nPos]='\0';
                 alltrim(sData);
                 if(strlen(sData)==0 || strchr(sData,' ')) strcpy(sData,"0\0");
                 if(nError==0) (* pDouble)=atof(sData);
            }
            break;
         case 's':                                              //�ַ���
         case 'S':
            pChar=va_arg(pVar,char *);
            nPos=0;
            while(nLen--)
            {   ch=sString[0];
                sString++;
                if(ch==nSeparator) break;
                pChar[nPos++]=ch;
            }
            if(nPos!=0)
            {   
                pChar[nPos]='\0';
                alltrim(pChar);
            }    
            break;
            
         default:                                     //ָ�����������ʹ���
            nError=2;
            break;
      }//end switch         
    if(nError>0) break;                              //���ĳһ������������������
   }//end while             
  va_end(pVar);


  if(nError>0)
  {
  	sprintf(sTcpipErrInfo,"\nERROR:%ld,%s",nError,sString);
  	return NULL;
  }

  return (char *)sString;  
}


void  CreateDir(char * path)
{
      char buf[81];

      if(access(path,0))
         mkdir(path,0777);
}