//此函数测试用
#include<math.h>
#include<stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<stdarg.h>

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


