/***************************************************************************
** @Copyright...........1999.3
** filename       文件名: .c
** target           目的:
** create time  生成日期: 1999.3
** original programmer          原始编程人员:  heli
** copyright        版本:
** edit history 修改历史:
** date日期    editer修改人员   冯中伟  reason原因 增加修改密码函数ChangePassWord()
** 00.1.1
***************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "des.h"

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
