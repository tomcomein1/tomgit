/***********************************************************
**
** 文件名 : 8583.h
** 目的   : 将业务数据形成标准的8583报文
**
** 生成日期 ：2001.8
** 原始编程人员 ：何庆
** 修改历史 :
** 日期        修改人员  原因
**
************************************************************/
#define		NORM	0
#define		LLVAR	1
#define		LLLVAR	2
#define		LVAR	4
#define 	MAXLEN  1024
#define		MAPCOL  66

int maptable[][3]={
    {1,16,NORM},
    {2,18,LLVAR},
    {3,6,NORM},
    {4,12,NORM},
    {7,10,NORM},
    {11,6,NORM},
    {12,6,NORM},
    {13,4,NORM},
    {14,4,NORM},
    {15,4,NORM},
    {18,4,NORM},
    {22,3,NORM},
    {25,2,NORM},
    {26,2,NORM},
    {28,9,NORM},
    {30,9,NORM},
    {32,11,LLVAR},
    {33,11,LLVAR},
    {35,37,LLVAR},
    {36,104,LLLVAR},
    {37,12,NORM},
    {38,6,NORM},
    {39,2,NORM},
    {41,8,NORM},
    {42,15,NORM},
    {43,40,NORM},
    {44,22,LLVAR},
    {48,198,LLLVAR},
    {49,3,NORM},
    {50,3,NORM},
    {52,16,NORM},
    {53,16,NORM},
    {54,60,LLLVAR},
    {60,7,LVAR},
    {61,30,LLLVAR},
    {62,984,LLLVAR},
    {63,16,NORM},
    {64,16,NORM},
    {66,1,NORM},
    {70,3,NORM},
    {74,10,NORM},
    {75,10,NORM},
    {76,10,NORM},
    {77,10,NORM},
    {78,10,NORM},
    {81,10,NORM},
    {82,12,NORM},
    {83,12,NORM},
    {84,12,NORM},
    {85,12,NORM},
    {86,16,NORM},
    {87,16,NORM},
    {88,16,NORM},
    {89,16,NORM},
    {90,42,NORM},
    {96,16,NORM},
    {97,17,NORM},
    {100,11,LLVAR},
    {101,17,NORM},
    {102,18,LLVAR},
    {103,28,LLVAR},
    {120,999,LLLVAR},
    {121,30,LLLVAR},
    {122,999,LLLVAR},
    {123,30,LLLVAR},
    {128,16,NORM}
};


//设置8583报文的位图
void setbitmap(unsigned char *bitmap,int position)
{
   int m,n;
   char k;

   m=position / 8;
   n=position % 8;

   if (n==0)
   {m-=1;
    n=8;
   }

   k=0x01;
   k=k<<(8-n);
   bitmap[m] = bitmap[m] | k;
}


//寻找8583报文位图中已置1的位
int issetbit(unsigned char *bitmap,int position)
{
   int m,n;
   //unsigned char k;

   m=position / 8;
   n=position % 8;

   if (n==0)
   {m-=1;
    n=8;
   }

   return ((bitmap[m]>>(8-n))&0x01);

}


//BIT转化成字符
void bctran(unsigned char *bitmapin,int bitlenin,char *bitmapout)
{
   int i,j;
   j=0;
   for (i=0;i<bitlenin;i++)
   {
      if ((bitmapin[i]>>4)>=10)
         bitmapout[j]=((bitmapin[i]>>4)&0x0F) -10 + 'A';
      else 
         bitmapout[j]=((bitmapin[i]>>4)&0x0F) + '0';
   	  j++;
      if ((bitmapin[i]&0x0F)>=10)
         bitmapout[j]=(bitmapin[i]&0x0F) -10 + 'A';
      else 
         bitmapout[j]=(bitmapin[i]&0x0F) + '0';
      j++;
   }
}


//字符转化成BIT
void cbtran(char *bitmapin, int bitlenin,unsigned char *bitmapout)
{
   int i,j;
   j=0;
   for (i=0;i<bitlenin;i++)
   {
      if (bitmapin[i]>='A')
         bitmapout[j]=bitmapin[i]-'A' + 10;
      else 
         bitmapout[j]=bitmapin[i]-'0';
	        
      bitmapout[j]=bitmapout[j]<<4;
	  i++;
      if (bitmapin[i]>='A')
         bitmapout[j]=bitmapout[j] | (bitmapin[i] - 'A' + 10);
      else 
         bitmapout[j]=bitmapout[j] | (bitmapin[i] - '0');
      j++;
   }
}


//将数据打入8583报文中
void pack8583(unsigned char *bitmap,int position,char *deststr,char *sourstr,int *stp, int interval,int attr)
{
	char tmplen[4];

	setbitmap(bitmap,position);
	if (attr==LVAR)
	{
	    memset(tmplen,0,sizeof(tmplen));	
		sprintf(tmplen,"%1.1d",interval);
		memcpy(deststr+*stp,tmplen,1);
		memcpy(deststr+1+*stp,sourstr,interval);
		*stp=*stp+interval+1;
	}
	else if (attr==LLVAR)
	{
	    memset(tmplen,0,sizeof(tmplen));	
		sprintf(tmplen,"%2.2d",interval);
		memcpy(deststr+*stp,tmplen,2);
		memcpy(deststr+2+*stp,sourstr,interval);
		*stp=*stp+interval+2;
	}
	else if (attr==LLLVAR)
	{
	    memset(tmplen,0,sizeof(tmplen));	
		sprintf(tmplen,"%3.3d",interval);
		memcpy(deststr+*stp,tmplen,3);
		memcpy(deststr+*stp+3,sourstr,interval);
		*stp=*stp+interval+3;
	}
	else
	{
	    memcpy(deststr+*stp,sourstr,interval);
		*stp=*stp+interval;
	}
}


//从8583报文中寻找指定数据
int findp(unsigned char *bitmap,int zoon,char *inbuff)
{
   int i,j,tmp=0,sum=0;
   char caLen[4];

   for (i=1;i<zoon;i++)
   {
      if (issetbit(bitmap,i))
      {
         for (j=0;j<MAPCOL;j++)
         {
            if (maptable[j][0]==i)
			{ 				
               memset(caLen,0,sizeof(caLen));
               switch (maptable[j][2])
               {
                  case LVAR:
                     memcpy(caLen,inbuff+sum,1);
                     sum=sum+atoi(caLen)+1;
                     break;
                  case LLVAR:
                     memcpy(caLen,inbuff+sum,2);
                     sum=sum+atoi(caLen)+2;
                     break;
                  case LLLVAR:
                     memcpy(caLen,inbuff+sum,3);
                     sum=sum+atoi(caLen)+3;
                     break;
                  default:
                     sum=sum+maptable[j][1];
                     break;
               }//end switch
               break;
            }//end if
         }//end for
      }//end if
   }//end for
   return sum;
}

