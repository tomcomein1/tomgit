#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

long	InsertFileName(long,long);
long	CheckFileName(long,long);
long	UpdateFileName(long,long);
long	InsertFileCwxx(long,long);
long    CheckFileUpdate(long,long);

long InsertFileName(long nInSocket,long nTaskNo)
{
	FILE 	* pFile;
	long	lFNo,lLkrq;
	char	sTemp[111],sFile[51],sFile1[101],sFile2[51],sFile3[51];
  	char 	sDestination[101];         
  	long 	nRouteNum;
  	long 	nRetVal;

  	if(GetFromSocket(nInSocket, "%s %ld", sDestination, &nRouteNum))
    		return -1;
  	if(GetFromSocket(nInSocket, "%s",sFile))
    		return -1;
	if(access("newdata/FileHead.rdx",0))
	{
		if((pFile=fopen("newdata/FileHead.rdx","a+"))==NULL)
		{
			printf("\n打开索引信息文件时出错!!\n");
			return 1;
		}
		lFNo=fileno(pFile);
		if(fchmod(lFNo,0777))
		{
			fclose(pFile);
			remove("newdata/FileHead.rdx");
			printf("\n修改索引信息文件权限时出错!!\n");
			return 1;
		}
		fclose(pFile);
	}
	if((pFile=fopen("newdata/FileHead.rdx","r"))==NULL)
	{
		printf("\n打开索引信息文件时出错!!\n");
		return 1;
	}
	alltrim(sFile);
	strcpy(sFile2,sFile);
	lLkrq=atol(strtok(sFile2,"-"));
	strcpy(sFile3,strtok(NULL,";"));
	alltrim(sFile3);
	for(; ;)
	{
      		if(fgets(sTemp,110,pFile)==NULL)
              		break;
       		if(sTemp[0]=='\0')
               		break;
		strcpy(sFile1,strtok(sTemp,";"));
		alltrim(sFile1);
		if(strstr(sFile1,sFile3))
		{
			fclose(pFile);
      			if(SendLongToSocket(nInSocket, 0))
        			return 1;
			return 0;
		}
	}
	fclose(pFile);
	if((pFile=fopen("newdata/FileHead.rdx","a+"))==NULL)
	{
		printf("\n打开索引信息文件时出错!!\n");
		return 1;
	}
	sprintf(sFile1,"%s--0;\n",sFile);
	if(fprintf(pFile,"%s",sFile1)<=0)
	{
		printf("\n插入索引信息文件时出错!!\n");
		return 1;
	}
	fclose(pFile);
      	if(SendLongToSocket(nInSocket, 0))
        	return 1;
	return 0;
}


long CheckFileUpdate(long nInSocket,long nTaskNo)
{
	FILE 	* pFile;
	long	lFNo;
	char	sTemp[111],sFile[51],sFile1[101];
  	char 	sDestination[101];         
  	long 	nRouteNum;
  	long 	nRetVal;
        char    sCheckstr[10];
        
        
  	if(GetFromSocket(nInSocket, "%s %ld", sDestination, &nRouteNum))
    		return -1;
  	if(GetFromSocket(nInSocket, "%s",sFile))
    		return -1;
	if(access("newdata/FileHead.rdx",0))
	{
		printf("\n不能找到索引信息文件!!\n");
		return 1;
	}
	if((pFile=fopen("newdata/FileHead.rdx","r"))==NULL)
	{
		printf("\n打开索引信息文件时出错!!\n");
		return 1;
	}
	for(; ;)
	{
      		if(fgets(sTemp,110,pFile)==NULL)
              		break;
       		if(sTemp[0]=='\0')
               		break;
		strcpy(sFile1,strtok(sTemp,";"));
		alltrim(sFile1);
		alltrim(sFile);
		strcpy(sCheckstr,"--1");
		if(strstr(sFile1,sFile))
		{
			if(strstr(sFile1,sCheckstr))
			{
			 fclose(pFile);
      			 if(SendLongToSocket(nInSocket, 0))
        			return 1;
      			 return 0;
      			}
      			else if(strstr(sFile1,"--2"))
      			{
      			    fclose(pFile);
      			    if(SendLongToSocket(nInSocket, 0))
        			return 1;
      			    return 0;	
      			}	
		}
		
	}
	fclose(pFile);
      	if(SendLongToSocket(nInSocket, 0))
        	return 1;
	return 1;
}





long CheckFileName(long nInSocket,long nTaskNo)
{
	FILE 	* pFile;
	long	lFNo;
	char	sTemp[111],sFile[51],sFile1[101];
  	char 	sDestination[101];         
  	long 	nRouteNum;
  	long 	nRetVal;

  	if(GetFromSocket(nInSocket, "%s %ld", sDestination, &nRouteNum))
    		return -1;
  	if(GetFromSocket(nInSocket, "%s",sFile))
    		return -1;
	if(access("newdata/FileHead.rdx",0))
	{
		printf("\n不能找到索引信息文件!!\n");
		return 1;
	}
	if((pFile=fopen("newdata/FileHead.rdx","r"))==NULL)
	{
		printf("\n打开索引信息文件时出错!!\n");
		return 1;
	}
	for(; ;)
	{
      		if(fgets(sTemp,110,pFile)==NULL)
              		break;
       		if(sTemp[0]=='\0')
               		break;
		strcpy(sFile1,strtok(sTemp,";"));
		alltrim(sFile1);
		alltrim(sFile);
		if(strstr(sFile1,sFile))
		{
			fclose(pFile);
      			if(SendLongToSocket(nInSocket, 0))
        			return 1;
			return 0;
		}
	}
	fclose(pFile);
      	if(SendLongToSocket(nInSocket, 0))
        	return 1;
	return 1;
}


long UpdateFileName(long nInSocket,long nTaskNo)
{
	FILE 	* pFile;
	long	lFNo;
	char	sTemp[111],sFile[51],sFile1[101];
  	char 	sDestination[101];         
  	long 	nRouteNum;
  	long 	nRetVal;

  	if(GetFromSocket(nInSocket, "%s %ld", sDestination, &nRouteNum))
    		return -1;
  	if(GetFromSocket(nInSocket, "%s",sFile))
    		return -1;
	if(access("newdata/FileHead.rdx",0))
	{
		printf("\n不能找到索引信息文件!!\n");
		return 1;
	}
	if((pFile=fopen("newdata/FileHead.rdx","r+"))==NULL)
	{
		printf("\n打开索引信息文件时出错!!\n");
		return 1;
	}
	for(; ;)
	{
      		if(fgets(sTemp,110,pFile)==NULL)
              		break;
       		if(sTemp[0]=='\0')
               		break;
		strcpy(sFile1,strtok(sTemp,";"));
		alltrim(sFile1);
		alltrim(sFile);
		if(strstr(sFile1,sFile))
		{
			if(fseek(pFile,-3,SEEK_CUR))
			{
				clrscr();
				gotoxy(20,10);
				outtext("修改文件索引信息时文件指针定位出错,请与鸿志公司联系!!");
				fclose(pFile);
				return 1;
			}
			if(fprintf(pFile,"2")<=0)
			{
				clrscr();
				gotoxy(20,10);
				outtext("修改索引信息文件时出错,请与鸿志公司联系!!");
				fclose(pFile);
				return 1;
			}
			if(fseek(pFile,2,SEEK_CUR))
			{
				clrscr();
				gotoxy(20,10);
				outtext("修改文件索引信息时文件指针定位出错,请与鸿志公司联系!!");
				fclose(pFile);
				return 1;
			}
			fclose(pFile);
      			if(SendLongToSocket(nInSocket, 0))
        			return 1;
			return 0;
		}
	}
	fclose(pFile);
      	if(SendLongToSocket(nInSocket, 0))
        	return 1;
	return 1;
}


long InsertFileCwxx(long nInSocket,long nTaskNo)
{
	FILE 	* pFile;
	long	lFNo,lLkrq,lLkrq1;
	char	sTemp[501],sJh[11],sJh1[11],sCwxx[401],sCwxx1[401],sYcwxx[401];
  	char 	sDestination[101],sCdCwxx[401];         
  	long 	nRouteNum;
  	long 	nRetVal;

  	if(GetFromSocket(nInSocket, "%s %ld", sDestination, &nRouteNum))
    		return -1;
  	if(GetFromSocket(nInSocket, "%s",sCdCwxx))
    		return -1;
	if(access("newdata/FileCwxx.rdx",0))
	{
		if((pFile=fopen("newdata/FileCwxx.rdx","a+"))==NULL)
		{
			printf("\n打开错误信息文件时出错!!\n");
			return 1;
		}
		lFNo=fileno(pFile);
		if(fchmod(lFNo,0777))
		{
			fclose(pFile);
			remove("newdata/FileCwxx.rdx");
			printf("\n修改错误信息文件权限时出错!!\n");
			return 1;
		}
		fclose(pFile);
	}
	if((pFile=fopen("newdata/FileCwxx.rdx","r"))==NULL)
	{
		printf("\n打开错误信息文件时出错!!\n");
		return 1;
	}
	strcpy(sYcwxx,sCdCwxx);
	lLkrq1=atol(strtok(sCdCwxx,","));
	strcpy(sJh1,strtok(NULL,","));
	strcpy(sCwxx1,strtok(NULL,","));
	for(; ;)
	{
      		if(fgets(sTemp,500,pFile)==NULL)
              		break;
       		if(sTemp[0]=='\0')
               		break;
		lLkrq=atol(strtok(sTemp,","));
		strcpy(sJh,strtok(NULL,","));
		strcpy(sCwxx,strtok(NULL,","));
		if(!strcmp(sJh,sJh1)&&!strcmp(sCwxx,sCwxx1)&&lLkrq1-lLkrq<=5)
		{
			fclose(pFile);
      			if(SendLongToSocket(nInSocket, 0))
        			return 1;
			return 0;
		}
	}
	fclose(pFile);
	if((pFile=fopen("newdata/FileCwxx.rdx","a+"))==NULL)
	{
		printf("\n打开错误信息文件时出错!!\n");
		return 1;
	}
	sprintf(sCwxx,"%s\n",sYcwxx);
	if(fprintf(pFile,"%s",sCwxx)<=0)
	{
		printf("\n插入错误信息文件时出错!!\n");
		return 1;
	}
	fclose(pFile);
      	if(SendLongToSocket(nInSocket, 0))
        	return 1;
	return 0;
}
