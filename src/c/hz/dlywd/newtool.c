//�˺���������
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
����:ASCIItoBCD;
����:��ʮ�����Ƶ�ASCII���ַ������BCD��
����:sASC:ASCII�ַ���;
     nLen:ASCII�ַ�������[ż��];
     sDest:Ŀ�괮ָ��;
ע��:��ת������ַ�������ֻ��ԭ����һ��[nLen/2];
*-------------------------------------------------*/

long ASCIItoBCD(char * sASC,int nLen,char * sDest)
{
	
     unsigned char cCode;
     char sTable[17]="0123456789ABCDEF";
     int nSrc,nLoop;
 
     nLoop=0;
     nSrc=0;

     if(nLen<=0) return 1;
     for(nLoop=0;nLoop<nLen;nLoop+=2)  //��ʮ�����ƵĴ���ת����ASCII��
     {
     	cCode=sASC[nLoop];
        if(!strchr(sTable,cCode)) return 1;
        sDest[nSrc]=strchr(sTable,cCode)-sTable;  //�õ���λʮ����������
        sDest[nSrc]<<=4;                        //�������λ 
 
        cCode=sASC[nLoop+1];                  //�õ�����λ����
        if(!strchr(sTable,cCode)) return 1;
        sDest[nSrc]|=(strchr(sTable,cCode)-sTable)&0x0f;//�������λ����
       
        nSrc++;
     }
     sDest[nSrc]='\0';
     return 0;	

}


