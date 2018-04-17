//申请打印模块

#include "dlywd.h"

long dsTaskRequestPrint(char * sLsh)
{
	
     char sJym[16],sJsdh[10];
     long nCxms=0;
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);
     
     switch(nCxms)
     {
     	case 130:
     	   return dsMobileRequestPrint(sLsh);
     	   break;
     }
}

static long dsMobileRequestPrint(char * sLsh)
{
	
     char filename[21];
     long nCount;
     long nRetVal;
     char buffer[10000];
     char result[10000];
     long nLkrq,nSfm,nDate,nTime,size;
     char * offset,* check;
     FILE * fp;
     
     
     nRetVal = RunSelect("select nlkrq,nsfm,nbl,cbl5 from dl$zwmx where clsh=%s into %ld%ld%ld%s",
               sLsh,&nLkrq,&nSfm,&nCount,filename);

     if(nRetVal<=0)
     	return ApplyToClient("%s","000001|查询出错|");
     
     if(!(fp= fopen(filename,"rb")))
     	return ApplyToClient("%s","000002|无法打开指定的文件|");

     memset(result,'\0',sizeof(result));
     memset(buffer,'\0',sizeof(buffer));

     fread(buffer,15,1,fp);
     FetchStringToArg(buffer,"%8d%6d",&nDate,&nTime);
     
     if(nDate!=nLkrq) // || nTime!=nSfm)
     	return ApplyToClient("%s","000003|申请打印时间超出预定值|");

     sprintf(result,"000000|%ld|",nCount);
     fseek(fp,0,SEEK_END);
     size=ftell(fp)-15;
     fseek(fp,15,SEEK_SET);
     fread(buffer,size,1,fp);
     
     check=buffer;
     while(check[0])
     {
     	if(check[0]=='|') check[0]='!';
     	check++;
     }
     
     offset=buffer;
     check=buffer;
     for(;;)
     {
     	check = strstr(check,"END_OF_PAGE");
     	if(!check) break;
     	check[0]='\0';
     	strcat(result,offset);
     	strcat(result,"|");
     	check+=12;
     	offset=check;
     }
     return ApplyToClient("%s",result);
}


