
/************************************************************************
make date:  2001.2.20
filename : filechar.h
target:    �ļ����ַ�������ͷ�ļ�
*************************************************************************/
#ifndef FILECHAR_H
#define FILECHAR_H
#include<stdio.h>
#include<stdarg.h>
#include<string.h>
#include<stdlib.h>

#define MAX_LINE_LEN        500                    //getline��ÿ�е�����ֽ��� 
long GetTempFileName(char * sfilename);                    //���һ����ʱ�ļ���
long GetValueFromFile(char * filename,char * index,...);
long PutValueToFile(char * filename,char * index,...);
long getline(char * sline,FILE * fp);
long FetchFile(FILE * datafp,char * sFormat,...);
char * FetchStringToArg(char * sString,char * sFormat,...);
#endif


