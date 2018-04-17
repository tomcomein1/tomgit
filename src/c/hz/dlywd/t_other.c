#include"dlywd.h"

extern long nSysLkrq;
extern long nSysSfm;  
extern char sSysDlywdPath[31];
extern char sDefaultPass[31];

long dsTaskGetOneFileFromRemote(char * sFile)
{
     struct stat fstat;
     RECORD rec;
     char sFileName[100];

     sprintf(sFileName,"%s%s",sSysDlywdPath,sFile);
     memset(&rec,0,sizeof(RECORD));
     if(access(sFileName,0))
          return ApplyToClient("%ld%r",-1,rec);

     stat(sFileName,&fstat);
     rec.nsize=fstat.st_size;
     rec.fp=(FILE *)1;
     rec.filename=sFileName;
     return ApplyToClient("%ld%r",0,rec);  

}

long dsTaskSendFileToRemote(char * sFileName,char * sTempFile)
{
     if(!access(sFileName,0))   remove(sFileName);
     return ApplyToClient("%ld",(long)rename(sTempFile,sFileName));  

}

/*============================================================
函数：MutualExclusion
功能：提供按业务的互斥功能
参数：sJym:交易码
*===========================================================*/
void MutualExclusion(char *sJym)
{
  RunSql("update dl$jymcsfb set ccs='0' where cjym=%s and nbh=41",sJym);
}

long GenerateLsh(long nLb,char *cjh,char *cjym,char *clsh)
{

  long nRetVal,nValue;
  char sSequence[41]="1";

  GetSysLkrq();
  switch(nLb)
  {
    case 1:
    case 2:
         strcpy(sSequence,"DLNBLSH");
         break;
    case 3:
         strcpy(sSequence,"DLWTLSH");
         break;
  }

  nRetVal=RunSelect("SELECT %t.nextval FROM dual INTO %ld",sSequence,&nValue);
  if(nRetVal<=0) return 0;
    
  switch(nLb)
  {
    case 1:
         sprintf(clsh,"%-9.9s%-15.15s%08ld%08ld",cjh,cjym,nSysLkrq,nValue);
         break;
    case 2:
         sprintf(clsh,"%-9.9s%08ld%08ld",cjh,nSysLkrq,nValue);
         break;
    case 3:
         sprintf(clsh,"%08ld",nValue);
         break;
  }
  return 1;  

}



long dsTaskSystemBackup()
{
  char sbuf[256];
  char sBackupFileName[101];

  GetSysLkrq();

  sprintf(sBackupFileName,"bf%08ld.dmp",nSysLkrq);
  sprintf(sbuf,"exp dlz/%s file=%s",sDefaultPass,sBackupFileName);
  if(system(sbuf))
      return ApplyToClient("%ld%s",2,"运行备份命令失败");
  sprintf(sbuf,"compress -H -F %s",sBackupFileName);
  system(sbuf);

  strcat(sBackupFileName,".Z");
  return ApplyToClient("%ld%s",0,sBackupFileName);
}

int DlPasswordEnc(char* sPassWord,char * sNewPassWord)
{
    char keyword[9]="19780616";
    char dest[9];
    int  len;
    DesEncString(keyword,8,sPassWord,8,dest,&len);
    BCDtoASCII(dest,8,sNewPassWord);
    return 1;

}

int DlPasswordDec(char* sPassWord,char* sNewPassWord)
{

    char keyword[9]="19780616";
    char dest[9];
    
    upper(sPassWord); 
    ASCIItoBCD(sPassWord,16,dest);
    DesDecString(keyword,8,dest,8,sNewPassWord,0);
    return 1;
}    


long dsTaskGetValueFromFile(char * sInitFile,char* sTitleFlag,char* sDefault)
{

  long nRetVal;
  char sBuffer[256]="\0",sValue[256]="\0";
  
  sprintf(sBuffer,"%s%%s",sTitleFlag);
  nRetVal=GetValueFromFile(sInitFile,sBuffer,sValue);
  if(nRetVal<=0) strcpy(sValue,sDefault);
  return ApplyToClient("%ld%s",nRetVal,sValue);
   
}

long dsTaskPutValueToFile(char * sInitFile,char* sTitleFlag,char* sValue)
{
  char sBuffer[256];
  
  sprintf(sBuffer,"%s%%s",sTitleFlag);
  return ApplyToClient("%ld",PutValueToFile(sInitFile,sBuffer,sValue));
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

void GetSysLkrq()
{
	struct tm now_time;

	time_t now; 
	time(&now);
	now_time=*localtime(&now);
	
	nSysLkrq=(now_time.tm_year+1900)*10000+(now_time.tm_mon+1)*100+now_time.tm_mday;
	nSysSfm=now_time.tm_hour*10000+now_time.tm_min*100+now_time.tm_sec;
}

/*************************************
**
**函数: upper
**目的: 把字符串中小写字母变成大写
**参数: 需转换的字符串
**返回值: 无
**
*************************************/

void upper(char * s)
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
**函数: lower
**目的: 把字符串中大写字母变成小写
**参数: 需转换的字符串
**返回值: 无
**
*************************************/

void lower(char * s)
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

long dsTaskSystemRestore(char * sBackupFileName)
{

  char sbuf[256],* sOffset;
  
  if(access(sBackupFileName,0))
    return ApplyToClient("%ld%s",-1,"所给定的备份文件名不存在");
  
  if(sOffset=strstr(sBackupFileName,".Z"))
  {
        sprintf(sbuf,"uncompress -f %s",sBackupFileName);
        system(sbuf);
        sOffset[0]='\0';
  }

  sprintf(sbuf,"imp dlz/%s file=%s ignore=y full=y",sDefaultPass,sBackupFileName);
  if(system(sbuf))
      return ApplyToClient("%ld%s",-2,"运行系统恢复出错");

  return ApplyToClient("%ld%s",0,"系统恢复成功");

}

long dsTaskCleanupHistory(long nDate,long nMode)
{

  long nRetVal=1,nId; 
  char sbuf[256];
  char sJym[16];
  
  if(nMode&1)
  {
  	RunSql("delete dl$zwmx where nlkrq<=%ld",nDate);
        CommitWork();  		
  }		
  if(nMode&2)
  {
         
        nId=OpenCursor("select cjym  from dl$jym where substr(cjym,1,2)='01'");
        
        for(;;)
        {
        	if(FetchCursor(nId,"%s",sJym)<=0) break;
       	        RunSql("delete dl$ywmx_%t where nlkrq<=%ld",sJym,nDate);
       	        CommitWork();
  	}
  	CloseCursor(nId);
  }		
  	
  if(nMode&3)
  {
         
         RunSql("delete dl$czyb where cjh not in(select cjh from dl$jh)"); 
         RunSql("delete dl$jymkzfb where cjym not in(select cjym from dl$jym)");
         CommitWork();
          
  }		
  
  return ApplyToClient("%ld%s",0,"清除历史数据成功");
}

long dsTaskGetAllJsdh(char * sJh)
{
	
  RECORD rec;
  long nId,nLb,nRetVal,nZjAllow;
  char sJsdh[10],sJsmc[21];
  
  rec=CreateRecord();
  
  nId=OpenCursor("select cjh,cjm,nlb from dl$jh where nlb in(3,4) "
                 "and substr(cjh,1,4)=substr(%s,1,4)",sJh);
  for(;;)
  {
     if(FetchCursor(nId,"%s%s%ld",sJsdh,sJsmc,&nLb)<=0) break;
     ImportRecord(&rec,"%s%s%ld",sJsdh,sJsmc,nLb);
  }
  CloseCursor(nId);
  ImportRecord(&rec,"%s%s%ld",".",".",0);

  nRetVal=ApplyToClient("%r",rec);
  DropRecord(&rec);
  return nRetVal;
}

long FindRoute(char * sDestination,char * sGateway)
{

     if(RunSelect("select lower(sgateway) from dl$route where "
        "sdestination=%s into %s",sDestination,sGateway)>0) return 1;
        
     if(RunSelect("select lower(sgateway) from dl$route where "
        "upper(sdestination)='DEFAULT' into %s",sGateway)>0) return 1;
        
     return 0;   

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

//去除字符串中的非法字符，以便能正常入库
void TreatementString(char * str)
{
     long nbytes,npos;
     
     npos=0;
     nbytes=0;
     
     while(str[npos])
     {
     	nbytes+=str[npos]<128;
     	if((unsigned char)str[npos]<32 || str[npos]=='\'') str[npos]=' ';
     	npos++;
     }
     
     if(npos%2!=nbytes%2) 
     {
     	npos--;
     	while(str[npos]==' ') npos--;
     	str[npos]=' ';
     }
     
     alltrim(str);
}
