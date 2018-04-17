
/************************************************************************
make date:  2001.2.20
filename : filechar.h
target:    文件及字符串操作头文件
*************************************************************************/
#ifndef FILECHAR_H
#define FILECHAR_H
#include<stdio.h>
#include<stdarg.h>
#include<string.h>
#include<stdlib.h>

#define MAX_LINE_LEN        4096                    //getline中每行的最大字节数 
long GetTempFileName(char * sfilename);                    //获得一个临时文件名
long GetValueFromFile(char * filename,char * index,...);
long PutValueToFile(char * filename,char * index,...);
long getline(char * sline,FILE * fp);
long FetchFile(FILE * datafp,char * sFormat,...);
char * FetchStringToArg(char * sString,char * sFormat,...);
void alltrim(char *s);
long ASCIItoBCD(char * sASC,int nLen,char * sDest);
long BCDtoASCII(char * sBCD,int nLen,char * sDest);
#endif


