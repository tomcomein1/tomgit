/***************************************************************************
** @Copyright...........1999.3
** filename       �ļ���: .c
** target           Ŀ��:
** create time  ��������: 1999.3
** original programmer          ԭʼ�����Ա:  heli
** copyright        �汾:
** edit history �޸���ʷ:
** date����    editer�޸���Ա   ����ΰ  reasonԭ�� �����޸����뺯��ChangePassWord()
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
