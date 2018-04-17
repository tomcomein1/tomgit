/***********************************************************************
**filename	文件名:filechar.c
**target	目的:文件及字符串处理实现文件
**create time	生成日期: 2001.2.19
**original programmer	编码:陈博[chenbo]
**edit  history	修改历史:
**date     editer    reson
** 
****************************************************************************/

#include"filechar.h"


/*-----------------------------------------------------------------

函数:long getline(char * sline,FILE * fp)
目的:从文件中提出一行数据到指定的区域，以回车符结尾
参数:sline:缓冲区指针，fp:文件指针
返回:=0已经取完   <0失败

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
         if (nRec>=MAX_LINE_LEN-1) break;
         ch=getc(fp);
         sline[nRec++]=ch;
   }    

   if(nRec>=MAX_LINE_LEN)
   {
   	 sline[MAX_LINE_LEN-1]='\0';
   	 return 1;
   }	 

   sline[nRec]='\0';                                                          
   if(ch=='\n') return 1;
   #ifdef UNIX
   if(ch==EOF)  return 0;
   #else
   if(feof(fp)) return 0;
   #endif
   return -1;
}       

/*--------------------------------------------------------------------
函数：GetValueFromFile；
功能: 从文件filename中取出标题为index的值
参数：filename:文件名
      index:主键,及其参数的类型(%s,%ld,%lf)
      ...:返回参数
返回：>0：成功
      =0：没有匹配的值
      <0：操作失败
          -1：没有规定数据类型，或表达不正确；
          -2：目标文件不存在
          -3：无法打开文件
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

/*************************************
**
**函数: alltrim 
**目的: 删除字符串左右空格 
**参数: 字符串指针 
**返回值: 无
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

/*--------------------------------------------------------------------
函数：PutValueToFile；
功能: 设置filename中index项的值
参数：filename:文件名
      index:主键,及其参数的类型(%s,%ld,%lf)
      ...:返回参数
返回：>=0：成功
          >0：有区配值，并已成功修改
          =0：没有匹配的值,并成功地在文件尾部增加该项
      <0：操作失败
          -1：没有规定数据类型，或表达不正确；
          -2：目标文件不存在
          -3：无法打开文件
          -4：文件读写失败
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
                //匹配成功
                va_start(pvar,index);
                vsprintf(buf,index,pvar);
                va_end(pvar);
                value=buf;
                while(value[0]!='\0') value++;
                value[0]='\n';
                value[1]='\0';
                nChanged=1;
     	}
     	//将参数值写入新文件中
        fwrite(buf,strlen(buf),1,fpnew);
     }//end while
     fclose(fpold);
     fclose(fpnew);
     if(rename(sSwapFileName,filename)==0) return 1;
     return -4;  //文件操作失败  
}


/*-----------------------------------------------------------------

函数:long FetchFile;
目的:读取定长文件
参数:datafp:已经打开的数据文件指针   sFormat:格式化字符串
返回:>0成功    <0失败     =0文件已读完

*-----------------------------------------------------------------*/

long FetchFile(FILE * datafp,char * sFormat,...)
{
  va_list pVar;
  long  nLen=0,nError=0,nPos,nSeparator;
  char ch,sData[MAX_LINE_LEN];
  double  * pDouble;
  long    * pLong;
  char    * pChar;

  if(datafp==NULL) return -1;                  //文件尚未打开
  if(strchr(sFormat,'%')==NULL) return -2;     //没有指定读数格式
  
  va_start(pVar,sFormat);
     
  while((sFormat=strchr(sFormat,'%'))!=NULL)
  {
     nLen=0;
     sFormat++;
     nSeparator=0;
     
     if(sFormat[0]<='9' && sFormat[0]>='0')             //此字段为定长数据格式
     {
        while(sFormat[0]<='9' && sFormat[0]>='0')       //得到长度
        {
           nLen=nLen*10+(sFormat[0]-'0');
           sFormat++;
           
        }
     }  
     else                                               //此字段为分隔符形式
     {
        nLen=MAX_LINE_LEN;
        nSeparator=sFormat[1];
        if(nSeparator=='%') nSeparator=' ';
        
     }  
     if(nLen>MAX_LINE_LEN)                              //规定的长度超出缓冲区长度
     {
        nError=3;
        break;
     }

     switch(sFormat[0])
     {
        case 'd':                                       //长整型十进制数 
            pLong=va_arg(pVar,long *);
            nPos=0;
            while(nLen--)
            {
                 ch=getc(datafp);
                 if(ch==nSeparator) break;
                 if((ch<'0' || ch>'9') && ch!=' ' && ch!='-')    //不是数字型数据
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
            pDouble=va_arg(pVar,double *);                      //双精度十进制数
            while(nLen--)
            {
                 ch=getc(datafp);
                 if(ch==nSeparator) break;
                 if((ch<'0' || ch>'9') && ch!='.' && ch!=' ' && ch!='-')        //不是数字型数据         
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

         case 's':                                              //字符串
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
         default:                                     //指定的数据类型错误
            nError=2;
            break;
      }//end switch         
    if(nError>0) break;                              //如果某一处出错将忽略其它项
   }//end while             
  va_end(pVar);
  if(ch==EOF) return 0;                              //文件完毕
  if(ch=='\n') return 1;                             //正常
  return -1;
}


/*-----------------------------------------------------------------

函数:char * FetchStringToArg;
目的:解读字符串
参数:sString:字符串指针  sFormat:格式化字符串
返回:返回最后读取的字符位置，否则返回NULL
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
  if(strchr(sFormat,'%')==NULL) return NULL;     //没有指定读数格式
  
  va_start(pVar,sFormat);
     
  while((sFormat=strchr(sFormat,'%'))!=NULL)
  {
     nLen=0;
     sFormat++;
     nSeparator=0;
     
     if(sFormat[0]<='9' && sFormat[0]>='0')             //此字段为定长数据格式
     {
        while(sFormat[0]<='9' && sFormat[0]>='0')       //得到长度
        {
           nLen=nLen*10+(sFormat[0]-'0');
           sFormat++;
           
        }
     }  
     else                                               //此字段为分隔符形式
     {
        nLen=MAX_LINE_LEN;
        nSeparator=sFormat[1];
        if(nSeparator=='%') nSeparator=' ';             //如果不指定分隔符，则默认为空格
     }  
     if(nLen>MAX_LINE_LEN)                              //规定的长度超出缓冲区长度
     {
        nError=3;
        break;
     }

     switch(sFormat[0])
     {
        case 'd':                                       //长整型十进制数 
            pLong=va_arg(pVar,long *);
            nPos=0;
            while(nLen--)
            {
                 ch=sString[0];
                 sString++;
                 if(ch==nSeparator) break;
                 if((ch<'0' || ch>'9') && ch!=' ' && ch!='-')    //不是数字型数据
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
            pDouble=va_arg(pVar,double *);                      //双精度十进制数
            while(nLen--)
            {
                 ch=sString[0];
                 sString++;
                 if(ch==nSeparator) break;
                 if((ch<'0' || ch>'9') && ch!='.' && ch!=' ' && ch!='-')        //不是数字型数据         
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
         case 's':                                              //字符串
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
            
         default:                                     //指定的数据类型错误
            nError=2;
            break;
      }//end switch         
    if(nError>0) break;                              //如果某一处出错将忽略其它项
   }//end while             
  va_end(pVar);


  if(nError>0)
  {
  	printf("\nERROR:%ld,%s",nError,sString);
  	return NULL;
  }

  return (char *)sString;  
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
/*-----------------------------------------------------------------
函数:GetTempFileName
目的:获得一个临时文件名(与当前目录下的文件名不重复）
参数:sfilename:返回的临时文件名字符串缓冲区
返回:>0成功
*-----------------------------------------------------------------*/
long GetTempFileName(char * sfilename)
{
    extern long nSysSfm;
    
    char random=0;
    for(;;)
    {
        GetSysLkrq();
    	sprintf(sfilename,"%06ld%02ld.tmp",nSysSfm,(random++)%100);
    	if(access(sfilename,0)) break;
    }	
    return 1;

}
