/***********************************************************
**
** 文件: task6.c
** 目的: 185任务函数
**
** 生成日期：2001.8
** 原始编程人员：何庆
** 修改历史
** 日期        修改人员  原因
**
************************************************************/
#include <time.h>
#include "dlywd.h"

#define  LKHOST		"10.94.12.1"//绿卡通讯主机名
#define  LKSVRNAME	"5008"  //绿卡通讯主机服务端口名
#define  ZDBSM_LK       "00000000" //终端标识码

char lkywcode[3];
char lkywinfo[129];

#define  TEST
#define  LKJY_SUCCESS	"00" //绿卡交易成功
#define  LKJY_ERR_TX	"01" //后台通讯失败
#define  LKJY_ERR_DB	"02" //DLYWD操作数局库失败
#define  LKJY_ERR_LKJY  "03" //绿卡交易错误

#define  BCD_LEN	8
#define  ASCII_LEN	16
#define  LKQD_SEND_LEN  107
#define  LKJH_LEN       20

extern long nSysLkrq;
extern long nSysSfm;
extern char sSysJh[9+1];
extern char sSysErrInfo[81];
static char sequance[5+1];

/*绿卡签到*/
#define  LKQD_BWLXBSF_FD	"0820"
#define  LKQD_BWLXBSF_DF	"0830"
#define  LKQD_JYLXM		"091001"
/*绿卡签退*/
#define  LKQT_BWLXBSF_FD	"0820"
#define  LKQT_BWLXBSF_DF	"0830"
#define  LKQT_JYLXM		"091002"
/*绿卡交换密钥*/
#define  LKJHMY_BWLXBSF_FD	"0800"
#define  LKJHMY_BWLXBSF_DF	"0810"
#define  LKJHMY_JYLXM		"091003"
/*绿卡缴费*/
#define  LKJF_BWLXBSF		"0200"
#define  LKJF_JYLXM		"056031"
/*绿卡取消缴费*/
#define  LKQXJF_BWLXBSF		"0420"
#define  LKQXJF_JYLXM		"056931"
#define  LKQXJF_JFFS		"000"
#define  LKQXJF_FWDTJM		"00"
/*绿卡帐户余额查询*/
#define  LKZHYECX_BWLXBSF	"0200"//报文类型标识符
#define  LKZHYECX_JYLXM		"053070"//交易类型码
/*绿卡总帐对帐*/
#define  LKDZ_BWLXBSF		"0500"//报文类型标识符
#define  LKDZ_JYLXM		"091004"//交易类型码
/*绿卡明细对帐*/
#define  LKMXDZ_BWLXBSF		"0522"//报文类型标识符
#define  LKMXDZ_JYLXM		"091011"//交易类型码

//电话银行缴费
typedef struct lkjf
{
    char   sYyzh[19+1];//语音帐号
    char   sYymm[16+1];//语音密码
    char   sYhh[20+1]; //用户号
    long   nDsfzlh;    //代收付种类号
    double dJyje;      //交易金额
}LKJF;

long dsTask185jhmy();
long daTaskDl185Jyjecx();
long ReadLkxx(char *YDMY,char *MACKEY,char *PINKEY,char *LKJH);
long FormLsh(char *sSequence);
long getlkjybs();
long dsTaskLkjhmy(long nflag);
long dsTaskLkjhmy1();
long dsTaskLkjhmy2();
long dsTaskLkjf(char *input);
void BinaryToHex(char *bin,char *hexASC,long size);
void GetLkJysj(long *nJysj,long *nJyrq);
void inichar(char *array,long n);
void inputydmy();
void lkinfo();
double getlkjyje();

/*绿卡签到任务*/
int dsTaskLkqd()
{
	long     nSql;
	long    nJysj,nJyrq;
	long    nSocket;
	char	WT_B[128+1],WT[32+1];
	char	YDMY[ASCII_LEN+1],MACKEY[ASCII_LEN+1],PINKEY[ASCII_LEN+1];
	char    LKJH[15+1];
	char	YDMY_BCD[BCD_LEN+1],MACKEY_BCD[BCD_LEN+1];
	char	BWAQM_BCD[BCD_LEN+1],BWAQM_ASCII[ASCII_LEN+1];
	char    MAC_BCD[BCD_LEN+1],MAC_ASCII[ASCII_LEN+1];
	char    sSendPackage[108];
	char    buf[129];

	/*计算交易位图*/
	inichar(WT_B,128);
	WT_B[0]='1';
	WT_B[2]='1';
	WT_B[11]='1';
	WT_B[12]='1';
	WT_B[40]='1';
	WT_B[41]='1';
	WT_B[95]='1';
	WT_B[127]='1';
	BinaryToHex(WT_B,WT,16);   /*16 表示字节数*/
	#ifdef  TEST
		//clrscr();
		printf("\n  绿卡签到交易位图: [%s]",WT);
	#endif
	
	/*绿卡交易日期和交易时间*/
	GetLkJysj(&nJysj,&nJyrq);
 	#ifdef	TEST
 	    printf("\n  交易时间 : [%ld] ",nJysj);
 	    printf("\n  交易日期 : [%ld] ",nJyrq);    
 	#endif
	
	/*计算报文安全码*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	#ifdef  TEST
		printf("\n  TRANSKEY IS: [%s] ",YDMY);
		printf("\n  MACKEY IS  : [%s] ",MACKEY);
		printf("\n  PINKEY IS  : [%s] ",PINKEY);
		printf("\n  LKJH IS    : [%s]",LKJH);	
	#endif
        ASCIItoBCD(YDMY,16,YDMY_BCD);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	#ifdef  TEST
		printf("\n  加密主密钥 : [%s]",YDMY_BCD);
		printf("\n  MACKEY     : [%s]",MACKEY_BCD);
	#endif	
        EnCrypt(MACKEY_BCD,YDMY_BCD,BWAQM_BCD);
        //EnCrypt(MACKEY_BCD,"12345678",BWAQM_BCD);
        BCDtoASCII(BWAQM_BCD,8,BWAQM_ASCII);
	#ifdef	TEST
		printf("\n  报文安全码 : [%s]",BWAQM_ASCII);
	#endif
	
	/*合成发送报文*/
	#ifdef TEST
		//printf("\n  绿卡签到报文1 : %s",sSendPackage);
		printf("\n  签到报文字段类容 : [%4s][%32s][%6s][%06ld][%04ld][%8s][%015s][%16s]",
		       LKQD_BWLXBSF_FD,WT,LKQD_JYLXM,nJysj,nJyrq,ZDBSM_LK,LKJH,BWAQM_ASCII);
	#endif
	sprintf(sSendPackage,"%4s%32s%6s%06ld%04ld%8s%015s%16s",
		LKQD_BWLXBSF_FD,WT,LKQD_JYLXM,nJysj,nJyrq,ZDBSM_LK,LKJH,BWAQM_ASCII);

	#ifdef TEST
		printf("\n  绿卡签到报文 : [%s]",sSendPackage);
	#endif
	
	/*生成MAC校验码*/
	GetMac(sSendPackage,strlen(sSendPackage),YDMY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC_ASCII);
	#ifdef	TEST
		printf("\n  报文鉴别码 : [%s]",MAC_ASCII);
	#endif
	strcat(sSendPackage,MAC_ASCII);
	#ifdef	TEST
		printf("\n  绿卡签到报文 : [%s]",sSendPackage);
	#endif	

	/*建立SOCKET连接*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		#ifdef TEST
			printf("\n  nSocket is : %ld 建立SOCKET连接失败!",nSocket);
			printf("\n  Socket ErrorInfo is : %s ",sSysErrInfo);
		#endif
		return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);
	}
	else
	{
		#ifdef TEST
			printf("\n  建立SOCKET连接成功!");
		#endif
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			#ifdef TEST
			   printf("\n  写报文错误!");
			#endif
			return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);
		}
	}
	if(read(nSocket,buf,75)<0)
	{
		#ifdef TEST
		   printf("\n  读返回报文错误!");
		#endif
		return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);	
	}
	#ifdef TEST
		printf("\n  绿卡前置机响应报文 : [%s]",buf);
	#endif
	close(nSocket);	//关闭SOCKET
	
	/*读响应报文*/
	if(strncmp(buf,"0244",4)!=0)
	{
	   sscanf(buf+36,"%2s",lkywcode);
           #ifdef TEST
              printf("\n  LKYWCODE IS : %s ",lkywcode);
           #endif
           lkinfo();
	   nSql=RunSql("update dl$dlywcs set ccs='1' where nbh=50");
	   if(nSql<=0)
	   {
                 printf("\n  操作数据库失败！");
		 return ApplyToClient("%s",LKJY_ERR_DB);
	   }
	   else
	   {
		  CommitWork();
		  #ifdef TEST
		     printf("\n  签到交易成功!");
		  #endif
/*
                  if(lkjhmy(2)<0)
                  {
                  	return ApplyToClient("%s",LKJY_ERR_LKJY);;
                  }
                  if(lkjhmy(1)<0)
                  {
                  	return ApplyToClient("%s",LKJY_ERR_LKJY);;
                  } 
*/
           }
	   return ApplyToClient("%s%s",lkywcode,lkywinfo);
	}
	else
	{
	    #ifdef TEST
	      printf("\n  绿卡签到失败！");
	    #endif
	    return ApplyToClient("%s",buf);
	}
	//close(nSocket);	//关闭SOCKET
}

/*----------------------------*
   代理系统向绿卡前置机签退
 *----------------------------*/
long dsTaskLkqt()
{
	long     nSql;
	long    nJysj,nJyrq;
	long    nSocket;
	char	WT_B[128+1],WT[32+1];
	char	YDMY[ASCII_LEN+1],MACKEY[ASCII_LEN+1],PINKEY[ASCII_LEN+1];
	char    LKJH[15+1];
	char	YDMY_BCD[BCD_LEN+1],MACKEY_BCD[BCD_LEN+1];
	char	BWAQM_BCD[BCD_LEN+1],BWAQM_ASCII[ASCII_LEN+1];
	char    MAC_BCD[BCD_LEN+1],MAC_ASCII[ASCII_LEN+1];
	char    sSendPackage[108];
	char    buf[129];

	/*计算交易位图*/
	inichar(WT_B,128);
	WT_B[0]='1';
	WT_B[2]='1';
	WT_B[11]='1';
	WT_B[12]='1';
	WT_B[40]='1';
	WT_B[41]='1';
	WT_B[95]='1';
	WT_B[127]='1';
	BinaryToHex(WT_B,WT,16);   /*16 表示字节数*/
	#ifdef  TEST
		//clrscr();
		printf("\n  绿卡签退交易位图: [%s]",WT);
	#endif
	
	/*绿卡交易日期和交易时间*/
	GetLkJysj(&nJysj,&nJyrq);
 	#ifdef	TEST
 	    printf("\n  交易时间 : [%ld] ",nJysj);
 	    printf("\n  交易日期 : [%ld] ",nJyrq);    
 	#endif
	
	/*计算报文安全码*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	#ifdef  TEST
		printf("\n  TRANSKEY IS: [%s] ",YDMY);
		printf("\n  MACKEY IS  : [%s] ",MACKEY);
		printf("\n  PINKEY IS  : [%s] ",PINKEY);
		printf("\n  LKJH IS    : [%s]",LKJH);	
	#endif
        ASCIItoBCD(YDMY,16,YDMY_BCD);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	#ifdef  TEST
		printf("\n  加密主密钥 : [%s]",YDMY_BCD);
		printf("\n  MACKEY     : [%s]",MACKEY_BCD);
	#endif	
        EnCrypt(MACKEY_BCD,YDMY_BCD,BWAQM_BCD);
        //EnCrypt(MACKEY_BCD,"12345678",BWAQM_BCD);
        BCDtoASCII(BWAQM_BCD,8,BWAQM_ASCII);
	#ifdef	TEST
		printf("\n  报文安全码 : [%s]",BWAQM_ASCII);
	#endif
	
	/*合成发送报文*/
	#ifdef TEST
		//printf("\n  绿卡签退报文1 : %s",sSendPackage);
		printf("\n  签退报文字段类容 : [%4s][%32s][%6s][%06ld][%04ld][%8s][%015s][%16s]",
		       LKQD_BWLXBSF_FD,WT,LKQD_JYLXM,nJysj,nJyrq,ZDBSM_LK,LKJH,BWAQM_ASCII);
	#endif
	sprintf(sSendPackage,"%4s%32s%6s%06ld%04ld%8s%015s%16s",
		LKQT_BWLXBSF_FD,WT,LKQT_JYLXM,nJysj,nJyrq,ZDBSM_LK,LKJH,BWAQM_ASCII);

	#ifdef TEST
		printf("\n  绿卡签退报文 : [%s]",sSendPackage);
	#endif
	
	/*生成MAC校验码*/
	GetMac(sSendPackage,strlen(sSendPackage),YDMY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC_ASCII);
	#ifdef	TEST
		printf("\n  报文鉴别码 : [%s]",MAC_ASCII);
	#endif
	strcat(sSendPackage,MAC_ASCII);
	#ifdef	TEST
		printf("\n  绿卡签退报文 : [%s]",sSendPackage);
	#endif	

	/*建立SOCKET连接*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		#ifdef TEST
			printf("\n  nSocket is : %ld 建立SOCKET连接失败!",nSocket);
			printf("\n  Socket ErrorInfo is : %s ",sSysErrInfo);
		#endif
		return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);
	}
	else
	{
		#ifdef TEST
			printf("\n  建立SOCKET连接成功!");
		#endif
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			#ifdef TEST
			   printf("\n  写报文错误!");
			#endif
			return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);
		}
	}
	if(read(nSocket,buf,75)<0)
	{
		#ifdef TEST
		   printf("\n  读返回报文错误!");
		#endif
		return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);	
	}
	#ifdef TEST
		printf("\n  绿卡前置机响应报文 : [%s]",buf);
	#endif
	close(nSocket);	//关闭SOCKET
	
	/*读响应报文*/
	if(strncmp(buf,"0244",4)!=0)
	{
	   sscanf(buf+36,"%2s",lkywcode);
           #ifdef TEST
              printf("\n  LKYWCODE IS : %s ",lkywcode);
           #endif
           //lkinfo();
	   nSql=RunSql("update dl$dlywcs set ccs='0' where nbh=50");
	   if(nSql<=0)
	   {
                 printf("\n  操作数据库失败！");
		 return ApplyToClient("%s",LKJY_ERR_DB);
	   }
	   else
	   {
		  CommitWork();
		  #ifdef TEST
		     printf("\n  签退成功!");
		  #endif
           }
	   return ApplyToClient("%s%s",lkywcode,lkywinfo);
	}
	else
	{
	    #ifdef TEST
	      printf("\n  绿卡签退失败！");
	    #endif
	    return ApplyToClient("%s",buf);
	}
}


/*----------------------------*
     绿卡交换密钥函数
     nflag为1表示交换MACKEY
     nflag为2表示交换PINKEY
 *----------------------------*/
long dsTaskLkjhmy(long nFlag)
{
	int  sql;
	long nJyrq,nJysj;
	long nSocket;
	char WT_B[128+1],WT[32+1];
	char YDMY[ASCII_LEN+1],YDMY_BCD[BCD_LEN+1];
	char MACKEY[ASCII_LEN+1],MACKEY_BCD[BCD_LEN+1];
	char PINKEY[ASCII_LEN+1];
	char BWAQM_BCD[BCD_LEN+1],BWAQM_ASCII[ASCII_LEN+1];
	char MAC_BCD[BCD_LEN+1],MAC_ASCII[ASCII_LEN+1];
	char sSendPackage[124];
	char buf[256+1];
	char AQKZXX[ASCII_LEN+1];
	char pointer;
	char LKJH[LKJH_LEN+1];
	char NEWMACKEY[16+1],NEWPINKEY[16+1];
	char NEWMACKEY_BCD[8+1],NEWPINKEY_BCD[8+1];
	char NEWMACKEY_UN[8+1],NEWPINKEY_UN[8+1];
	char NEWMACKEY_ASCII[16+1],NEWPINKEY_ASCII[16+1];

	/*产生交易位图*/
	inichar(WT_B,128);
	WT_B[0]='1';
	WT_B[2]='1';
	WT_B[11]='1';
	WT_B[12]='1';
	WT_B[40]='1';
	WT_B[41]='1';
	WT_B[52]='1';
	WT_B[95]='1';
	WT_B[127]='1';

	BinaryToHex(WT_B,WT,16);
	#ifdef  TEST
	//clrscr();
	printf("\n  交换密钥交易位图 : [%s]",WT);
	#endif	

	/*绿卡交易日期*/
	GetLkJysj(&nJysj,&nJyrq);
 	#ifdef	TEST
 	printf("\n  交易时间 : [%ld] ",nJysj);
 	printf("\n  交易日期 : [%ld] ",nJyrq);    
	#endif	
	
	/*报文安全控制信息*/
	if(nFlag==1)
		strcpy(AQKZXX,"1000000000000000");//表示交换MACKEY
	else
		strcpy(AQKZXX,"2000000000000000");//表示交换PINKEY
	
	/*生成戴氏校验码*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	#ifdef  TEST
		printf("\n  YDMY IS   : [%s] ",YDMY);
		printf("\n  MACKEY IS : [%s] ",MACKEY);
		printf("\n  PINKEY IS : [%s] ",PINKEY);
		printf("\n  LKJH IS   : [%s] ",LKJH);
	#endif
	ASCIItoBCD(YDMY,16,YDMY_BCD);
	printf("\n 主密钥 [%s]",YDMY_BCD);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	printf("\n MACKEY_BCD [%s]",MACKEY_BCD);
	EnCrypt(MACKEY_BCD,YDMY_BCD,BWAQM_BCD);
	BCDtoASCII(BWAQM_BCD,8,BWAQM_ASCII);
	#ifdef TEST
	printf("\n  报文安全码 : [%s]",BWAQM_ASCII);
	#endif
	
	/*合成通讯报文*/
	#ifdef TEST
	//printf("\n 交换密钥发送报文内容 : [%s]",sSendPackage);
	printf("\n 交换密钥报文内容 : [%4s][%32s][%6s][%06ld][%04ld][%-8s][%015s][%16s][%16s]",
	  LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
	  LKJH,AQKZXX,BWAQM_ASCII);
	#endif
	sprintf(sSendPackage,"%4s%32s%6s%06ld%04ld%-8s%015s%16s%16s",
		LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
		LKJH,AQKZXX,BWAQM_ASCII);
	#ifdef TEST
	printf("\n 交换密钥发送报文1 : [%s]",sSendPackage);
	#endif

	/*生成MAC校验码*/
	GetMac(sSendPackage,strlen(sSendPackage),YDMY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC_ASCII);
	strcat(sSendPackage,MAC_ASCII);
       	#ifdef TEST
       	printf("\n 交换密钥发送报文2 : [%s]",sSendPackage);
       	#endif

	/*建立SOCKET连接*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		printf("\n SOCKET错误");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	else
	{
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			printf("\n SOCKET错误");
			return ApplyToClient("%s",LKJY_ERR_TX);
		}
	}
	memset(buf,'\0',257);
	if(read(nSocket,buf,107)<0)
	{
		printf("\n SOCKET错误");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	close(nSocket);
	
	/*读响应报文*/
	if(strncpy("0244",buf,4)!=0)
	{
        	printf("\n 交换密钥收到的报文 [%s]",buf);
		//sscanf(buf+52,"%2s",lkywcode);
                strncpy(lkywcode,buf+52,2);
                lkywcode[2]='\0';
		#ifdef TEST
		printf("\n 交换密钥响应码 : [%s]",lkywcode);
		#endif
		
		/*交换MACKEY*/
		if(nFlag==1)
		{
			memset(NEWMACKEY,'\0',17);
			strncpy(NEWMACKEY,buf+91,16);
			#ifdef TEST
			printf("\n 交换密钥收到得新密钥 [%s]",NEWMACKEY);
			#endif
			//DesDecString(YDMY_BCD,8,NEWMACKEY,16,NEWMACKEY_UN,NEWMACKEY_LEN);	/*解密新MAC*/
			ASCIItoBCD(NEWMACKEY,16,NEWMACKEY_BCD);
			DeCrypt(NEWMACKEY_BCD,YDMY_BCD,NEWMACKEY_UN);
			BCDtoASCII(NEWMACKEY_UN,8,NEWMACKEY_ASCII);	/*将2进制改用十六进制表示*/
			#ifdef TEST
			printf("THE NEW MACKEY IS : [%s]",NEWMACKEY_ASCII);
			#endif
			sql=RunSql("update dl$dlywcs set ccs=%s where nbh=53",NEWMACKEY_ASCII);	/*更新MACKEY*/
			if(sql<=0)
			{
				printf("\n  操作数据库失败！！！");
				//return -1;
				return ApplyToClient("%s",LKJY_ERR_DB);
			}
			//return 0;
			return ApplyToClient("%s%s",lkywcode,lkywinfo);
		}
		/*交换PINKEY*/
		else if(nFlag==2)
		{
			sscanf(buf+91,"%16s",NEWPINKEY);	/*解读返回的PINKEY*/
			printf("\n 交换密钥收到得新密钥 [%s]",NEWPINKEY);			 
			//DesDecString(MACKEY_BCD,8,NEWPINKEY,8,NEWPINKEY_UN,NEWPINKEY_LEN);	/*解密*/
			ASCIItoBCD(NEWPINKEY,16,NEWPINKEY_BCD);
			DeCrypt(NEWPINKEY_BCD,YDMY_BCD,NEWPINKEY_UN);
			BCDtoASCII(NEWPINKEY_UN,8,NEWPINKEY_ASCII);	/*将2进制改用十六进制表示*/
			#ifdef TEST
				printf("THE NEW PINKEY IS : %s",NEWPINKEY_ASCII);
			#endif
			sql=RunSql("update dl$dlywcs set ccs=%s where nbh=54",NEWPINKEY_ASCII);	/*更新NEWPINKEY*/
			if(sql<=0)
			{
				printf("\n  操作数据失败！！！");
				//return -1;
				return ApplyToClient("%s",LKJY_ERR_DB);
		        }
			//return 0;
			return ApplyToClient("%s%s",lkywcode,lkywinfo);
		}
	}
	else
	{
		printf("\n 交换密钥交易失败");
		//return -1;
		return ApplyToClient("%s",buf);
	}
}


/*----------------------------*
     绿卡交换密钥函数
     交换PINKEY
 *----------------------------*/
long dsTaskLkjhmy1()
{
	int  sql;
	long nJyrq,nJysj;
	long nSocket;
	char WT_B[128+1],WT[32+1];
	char YDMY[ASCII_LEN+1],YDMY_BCD[BCD_LEN+1];
	char MACKEY[ASCII_LEN+1],MACKEY_BCD[BCD_LEN+1];
	char PINKEY[ASCII_LEN+1];
	char BWAQM_BCD[BCD_LEN+1],BWAQM_ASCII[ASCII_LEN+1];
	char MAC_BCD[BCD_LEN+1],MAC_ASCII[ASCII_LEN+1];
	char sSendPackage[124];
	char buf[256+1];
	char AQKZXX[ASCII_LEN+1];
	char pointer;
	char LKJH[LKJH_LEN+1];
	char NEWMACKEY[16+1],NEWPINKEY[16+1];
	char NEWMACKEY_BCD[8+1],NEWPINKEY_BCD[8+1];
	char NEWMACKEY_UN[8+1],NEWPINKEY_UN[8+1];
	char NEWMACKEY_ASCII[16+1],NEWPINKEY_ASCII[16+1];

	/*产生交易位图*/
	inichar(WT_B,128);
	WT_B[0]='1';
	WT_B[2]='1';
	WT_B[11]='1';
	WT_B[12]='1';
	WT_B[40]='1';
	WT_B[41]='1';
	WT_B[52]='1';
	WT_B[95]='1';
	WT_B[127]='1';

	BinaryToHex(WT_B,WT,16);
	#ifdef  TEST
	//clrscr();
	printf("\n  交换密钥交易位图 : [%s]",WT);
	#endif	

	/*绿卡交易日期*/
	GetLkJysj(&nJysj,&nJyrq);
 	#ifdef	TEST
 	printf("\n  交易时间 : [%ld] ",nJysj);
 	printf("\n  交易日期 : [%ld] ",nJyrq);    
	#endif	
	
	/*报文安全控制信息*/
	strcpy(AQKZXX,"2000000000000000");//表示交换PINKEY
	
	/*生成戴氏校验码*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	#ifdef  TEST
		printf("\n  YDMY IS   : [%s] ",YDMY);
		printf("\n  MACKEY IS : [%s] ",MACKEY);
		printf("\n  PINKEY IS : [%s] ",PINKEY);
		printf("\n  LKJH IS   : [%s] ",LKJH);
	#endif
	ASCIItoBCD(YDMY,16,YDMY_BCD);
	printf("\n 主密钥 [%s]",YDMY_BCD);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	printf("\n MACKEY_BCD [%s]",MACKEY_BCD);
	EnCrypt(MACKEY_BCD,YDMY_BCD,BWAQM_BCD);
	BCDtoASCII(BWAQM_BCD,8,BWAQM_ASCII);
	#ifdef TEST
	printf("\n  报文安全码 : [%s]",BWAQM_ASCII);
	#endif
	
	/*合成通讯报文*/
	#ifdef TEST
	//printf("\n 交换密钥发送报文内容 : [%s]",sSendPackage);
	printf("\n 交换密钥报文内容 : [%4s][%32s][%6s][%06ld][%04ld][%-8s][%015s][%16s][%16s]",
	  LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
	  LKJH,AQKZXX,BWAQM_ASCII);
	#endif
	sprintf(sSendPackage,"%4s%32s%6s%06ld%04ld%-8s%015s%16s%16s",
		LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
		LKJH,AQKZXX,BWAQM_ASCII);
	#ifdef TEST
	printf("\n 交换密钥发送报文1 : [%s]",sSendPackage);
	#endif

	/*生成MAC校验码*/
	GetMac(sSendPackage,strlen(sSendPackage),YDMY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC_ASCII);
	strcat(sSendPackage,MAC_ASCII);
       	#ifdef TEST
       	printf("\n 交换密钥发送报文2 : [%s]",sSendPackage);
       	#endif

	/*建立SOCKET连接*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		printf("\n SOCKET错误");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	else
	{
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			printf("\n SOCKET错误");
			return ApplyToClient("%s",LKJY_ERR_TX);
		}
	}
	memset(buf,'\0',257);
	if(read(nSocket,buf,256)<0)
	{
		printf("\n SOCKET错误");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	close(nSocket);
	
	/*读响应报文*/
	if(strncpy("0244",buf,4)!=0)
	{
        printf("\n 交换密钥收到的报文 [%s]",buf);                                                        
		//sscanf(buf+52,"%2s",lkywcode);                                                                 
                strncpy(lkywcode,buf+52,2);                                                                      
                lkywcode[2]='\0';                                                                                
		#ifdef TEST                                                                                      
		printf("\n 交换密钥响应码 : [%s]",lkywcode);                                                     
		#endif                                                                                           
		                                                                                                 
		sscanf(buf+93,"%16s",NEWPINKEY);	/*解读返回的PINKEY*/                             
		printf("\n 交换密钥收到得新密钥 [%s]",NEWPINKEY);			                 
		//DesDecString(MACKEY_BCD,8,NEWPINKEY,8,NEWPINKEY_UN,NEWPINKEY_LEN);	/*解密*/         
		ASCIItoBCD(NEWPINKEY,16,NEWPINKEY_BCD);                                                  
		DeCrypt(NEWPINKEY_BCD,YDMY_BCD,NEWPINKEY_UN);                                            
		BCDtoASCII(NEWPINKEY_UN,8,NEWPINKEY_ASCII);	/*将2进制改用十六进制表示*/              
		#ifdef TEST                                                                              
			printf("\n THE NEW PINKEY IS : %s",NEWPINKEY_ASCII);                                
		#endif                                                                                   
		sql=RunSql("update dl$dlywcs set ccs=%s where nbh=54",NEWPINKEY_ASCII);	/*更新NEWPINKEY*/
		if(sql<=0)                                                                               
		{                                                                                        
			printf("\n  操作数据失败！！！");                                                
			//return -1;                                                                     
			return ApplyToClient("%s",LKJY_ERR_DB);                                          
		}                                                                                        
		else
		    return ApplyToClient("%s%s",lkywcode,lkywinfo);                                          
	}
	else
	{
		printf("\n 交换密钥交易失败");
		//return -1;
		return ApplyToClient("%s",buf);
	}
}

/*----------------------------*
     绿卡交换密钥函数
     交换MACKEY
 *----------------------------*/
long dsTaskLkjhmy2()
{
	int  sql;
	long nJyrq,nJysj;
	long nSocket;
	char WT_B[128+1],WT[32+1];
	char YDMY[ASCII_LEN+1],YDMY_BCD[BCD_LEN+1];
	char MACKEY[ASCII_LEN+1],MACKEY_BCD[BCD_LEN+1];
	char PINKEY[ASCII_LEN+1];
	char BWAQM_BCD[BCD_LEN+1],BWAQM_ASCII[ASCII_LEN+1];
	char MAC_BCD[BCD_LEN+1],MAC_ASCII[ASCII_LEN+1];
	char sSendPackage[124];
	char buf[256+1];
	char AQKZXX[ASCII_LEN+1];
	char pointer;
	char LKJH[LKJH_LEN+1];
	char NEWMACKEY[16+1],NEWPINKEY[16+1];
	char NEWMACKEY_BCD[8+1],NEWPINKEY_BCD[8+1];
	char NEWMACKEY_UN[8+1],NEWPINKEY_UN[8+1];
	char NEWMACKEY_ASCII[16+1],NEWPINKEY_ASCII[16+1];

	/*产生交易位图*/
	inichar(WT_B,128);
	WT_B[0]='1';
	WT_B[2]='1';
	WT_B[11]='1';
	WT_B[12]='1';
	WT_B[40]='1';
	WT_B[41]='1';
	WT_B[52]='1';
	WT_B[95]='1';
	WT_B[127]='1';

	BinaryToHex(WT_B,WT,16);
	#ifdef  TEST
	//clrscr();
	printf("\n  交换密钥交易位图 : [%s]",WT);
	#endif	

	/*绿卡交易日期*/
	GetLkJysj(&nJysj,&nJyrq);
 	#ifdef	TEST
 	printf("\n  交易时间 : [%ld] ",nJysj);
 	printf("\n  交易日期 : [%ld] ",nJyrq);    
	#endif	
	
	/*报文安全控制信息*/
	strcpy(AQKZXX,"1000000000000000");//表示交换MACKEY
	
	/*生成戴氏校验码*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	#ifdef  TEST
		printf("\n  YDMY IS   : [%s] ",YDMY);
		printf("\n  MACKEY IS : [%s] ",MACKEY);
		printf("\n  PINKEY IS : [%s] ",PINKEY);
		printf("\n  LKJH IS   : [%s] ",LKJH);
	#endif
	ASCIItoBCD(YDMY,16,YDMY_BCD);
	printf("\n 主密钥 [%s]",YDMY_BCD);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	printf("\n MACKEY_BCD [%s]",MACKEY_BCD);
	EnCrypt(MACKEY_BCD,YDMY_BCD,BWAQM_BCD);
	BCDtoASCII(BWAQM_BCD,8,BWAQM_ASCII);
	#ifdef TEST
	printf("\n  报文安全码 : [%s]",BWAQM_ASCII);
	#endif
	
	/*合成通讯报文*/
	#ifdef TEST
	//printf("\n 交换密钥发送报文内容 : [%s]",sSendPackage);
	printf("\n 交换密钥报文内容 : [%4s][%32s][%6s][%06ld][%04ld][%-8s][%015s][%16s][%16s]",
	  LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
	  LKJH,AQKZXX,BWAQM_ASCII);
	#endif
	sprintf(sSendPackage,"%4s%32s%6s%06ld%04ld%-8s%015s%16s%16s",
		LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
		LKJH,AQKZXX,BWAQM_ASCII);
	#ifdef TEST
	printf("\n 交换密钥发送报文1 : [%s]",sSendPackage);
	#endif

	/*生成MAC校验码*/
	GetMac(sSendPackage,strlen(sSendPackage),YDMY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC_ASCII);
	strcat(sSendPackage,MAC_ASCII);
       	#ifdef TEST
       	printf("\n 交换密钥发送报文2 : [%s]",sSendPackage);
       	#endif

	/*建立SOCKET连接*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		printf("\n SOCKET错误");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	else
	{
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			printf("\n SOCKET错误");
			return ApplyToClient("%s",LKJY_ERR_TX);
		}
	}
	memset(buf,'\0',257);
	if(read(nSocket,buf,256)<0)
	{
		printf("\n SOCKET错误");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	close(nSocket);
	
	/*读响应报文*/
	if(strncpy("0244",buf,4)!=0)
	{
        printf("\n 交换密钥收到的报文 [%s]",buf);
		//sscanf(buf+52,"%2s",lkywcode);
                strncpy(lkywcode,buf+52,2);
                lkywcode[2]='\0';
		#ifdef TEST
		printf("\n 交换密钥响应码 : [%s]",lkywcode);
		#endif
		
		memset(NEWMACKEY,'\0',17);
		strncpy(NEWMACKEY,buf+93,16);
		#ifdef TEST
		printf("\n 交换密钥收到得新密钥 [%s]",NEWMACKEY);
		#endif
		//DesDecString(YDMY_BCD,8,NEWMACKEY,16,NEWMACKEY_UN,NEWMACKEY_LEN);	/*解密新MAC*/
		ASCIItoBCD(NEWMACKEY,16,NEWMACKEY_BCD);
		DeCrypt(NEWMACKEY_BCD,YDMY_BCD,NEWMACKEY_UN);
		
printf("\n 绿卡MAC_BCD [%s]",NEWMACKEY_UN);
		BCDtoASCII(NEWMACKEY_UN,8,NEWMACKEY_ASCII);	/*将2进制改用十六进制表示*/
		#ifdef TEST
		printf("THE NEW MACKEY IS : [%s]",NEWMACKEY_ASCII);
		#endif
		sql=RunSql("update dl$dlywcs set ccs=%s where nbh=53",NEWMACKEY_ASCII);	/*更新MACKEY*/
		if(sql<=0)
		{
			printf("\n  操作数据库失败！！！");
			//return -1;
			return ApplyToClient("%s",LKJY_ERR_DB);
		}
		else
		        return ApplyToClient("%s%s",lkywcode,lkywinfo);

	}
	else
	{
		printf("\n 交换密钥交易失败");
		//return -1;
		return ApplyToClient("%s",buf);
	}
}



/*
 *"%16s%12f%4d%6d%20s%40s"
 *"绿卡帐号，MAC，交易金额，日期，时分秒,用户号"
 */
long dsTaskLkjf(char * input)
{
	char   syhzh[19+1];//用户帐号帐号
	char   spin[16+1];//用户密码
	char   nbdjyrq[4+1];//本地交易日期
	char   nbdjysj[6+1];//本地交易时间
	char   djyje[12+1];//交易金额

	unsigned int pinkey[8];
	unsigned int pin_source[8];
	unsigned int pin_target[8];
	long    i;//循环变量
	long    nyhzh_len;
	long    npinlen;
	char    lkjyje[12+1];
        char    cdllsh[41];
	long    nlsh;
	long    nSocket;
	long    nSql;
	char	WT_B[64+1],WT[16+1];
	char    syhlkzh[21+1];
	char    LKJH[LKJH_LEN+1];
	char    YDMY[ASCII_LEN+1],MACKEY[ASCII_LEN+1],PINKEY[ASCII_LEN+1];
	char    MACKEY_BCD[BCD_LEN+1],PINKEY_BCD[BCD_LEN+1];
	char    spin_bcd[BCD_LEN+1];
	char    PIN_BCD[BCD_LEN+1],PIN[ASCII_LEN+1];
	char    MAC_BCD[BCD_LEN+1],MAC[ASCII_LEN+1];
	char    Des_BCD[BCD_LEN+1],Des_ASCII[ASCII_LEN+1];
	char    sSendPackage[190+1];
	char    sbuf[104+1];
	LKJF    hlkjf;

        printf("\n input is [%s]",input);
	
	/*产生交易位图*/
	inichar(WT_B,64);
	WT_B[1]='1';
	WT_B[2]='1';
	WT_B[3]='1';
	WT_B[10]='1';
	WT_B[11]='1';
	WT_B[12]='1';
	WT_B[40]='1';
	WT_B[41]='1';
	WT_B[51]='1';
        WT_B[62]='1';
	WT_B[63]='1';
	BinaryToHex(WT_B,WT,8);
	#ifdef  TEST
	//clrscr();
	printf("\n  绿卡缴费位图 : [%s]",WT);
	#endif
	
	/*读取业务信息字段类容*/
	FetchStringToArg(input,"%19s%16s%12f%4s%6s%20s%40s",
		hlkjf.sYyzh,hlkjf.sYymm,&hlkjf.dJyje,nbdjyrq,nbdjysj,hlkjf.sYhh,cdllsh);
	strncpy(syhzh,input,19);
	syhzh[19]='\0';
	strncpy(spin,input+19,16);
	spin[16]='\0';
	strncpy(djyje,input+35,12);
	djyje[12]='\0';
	strncpy(nbdjyrq,input+47,4);
	nbdjyrq[4]='\0';
	strncpy(nbdjysj,input+51,6);
	nbdjysj[6]='\0';
	#ifdef TEST
		printf("\n  YHZH IS : [%s]",syhzh);
		printf("\n  PIN IS : [%s]",spin);
		printf("\n  JYJE IS : [%squ]",djyje);
		printf("\n  BDJYRQ IS : [%s]",nbdjyrq);
		printf("\n  BDJYSJ IS : [%s]",nbdjysj);
	#endif

	/*按8583标准形式的绿卡主帐号syhzh*/
        alltrim(syhzh);
	nyhzh_len=strlen(syhzh);
	sprintf(syhlkzh,"%02ld%s",nyhzh_len,syhzh);
	#ifdef TEST
	printf("\n  缴费的绿卡帐号 [%s]",syhlkzh);
	#endif
	
	/*个人密码的戴氏加密*/
        //int gen_pin(unsigned int pinkey[8], unsigned int pin_source[8], 
        //    unsigned int pin_target[8], int flag)
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	ASCIItoBCD(spin,16,spin_bcd);
	ASCIItoBCD(PINKEY,16,PINKEY_BCD);
	for(i=0;i<8;i++)
	   pinkey[i]=(unsigned int)PINKEY_BCD[i];
	for(i=0;i<8;i++)
	   pin_source[i]=(unsigned int)spin_bcd[i];
	gen_pin(pinkey,pin_source,pin_target,1);
	for(i=0;i<8;i++)
	   PIN_BCD[i]=(char)pin_target[i];
        BCDtoASCII(PIN_BCD,8,PIN);
	#ifdef TEST
	printf("\n  加密后的个人PIN码 [%s]",PIN);
	#endif

	/*8583方式表示的交易金额*/
	//lkjyje=(long)(djyje*100);
	memset(lkjyje,'\0',12);
	strncpy(lkjyje,djyje,9);
	strcat(lkjyje,"00");
	#ifdef TEST
	printf("\n  绿卡缴费金额 [%s]",lkjyje);
	#endif
	
	/*产生绿卡系统的流水号*/
	strcpy(sequance,"lklsh");
	nlsh=FormLsh(sequance);
	#ifdef TEST
	printf("\n  本次绿卡交易流水 [%ld]",nlsh);
	#endif
	alltrim(LKJH);
        alltrim(hlkjf.sYhh);
	/*形成绿卡缴费发送包*/
	#ifdef TEST
	printf("\n  绿卡缴费报文内容 [%4s][%16s][%s][%s][%012s][%06ld][%s][%s][%s][%015s][%16s]",
		LKJF_BWLXBSF,WT,syhlkzh,LKJF_JYLXM,lkjyje,nlsh,nbdjysj,nbdjyrq,ZDBSM_LK,LKJH,PIN);
	#endif
alltrim(hlkjf.sYhh);
	sprintf(sSendPackage,"%4s%16s%s%s%012s%06ld%4s%s%s%015s%s%03ld%s%s",
		LKJF_BWLXBSF,WT,syhlkzh,LKJF_JYLXM,lkjyje,nlsh,nbdjysj,nbdjyrq,ZDBSM_LK,LKJH,PIN,strlen(hlkjf.sYhh)+4,"0001",hlkjf.sYhh);
	printf("\n 绿卡缴费请求报文 [%s]",sSendPackage);
	
	/*计算校验码*/
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	GetMac(sSendPackage,strlen(sSendPackage),MACKEY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC);
	strcat(sSendPackage,MAC);
	#ifdef TEST
	printf("\n  MAC校验码 [%s]",MAC);
	#endif
	printf("\n 绿卡缴费发送报文 [%s]",sSendPackage);
	
	/*建立SOCKET连接*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		printf("\n SOCKET错误");
		//return -1;
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	else
	{
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			printf("\n SOCKET错误");
			//return -1;
			return ApplyToClient("%s",LKJY_ERR_TX);
		}
	}
	if(read(nSocket,sbuf,105)<0)
	{
		printf("\n SOCKET错误");
		//return -1;
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	close(nSocket);
        printf("\n 绿卡响应报文是 [%s]",sbuf);
	/*读响应报文*/
	if(strncmp(sbuf,"0244",4)!=0)
	{
	   strncpy(lkywcode,sbuf+53,2);
	   lkywcode[2]='\0';
           #ifdef TEST
              printf("\n  LKYWCODE IS : %s ",lkywcode);
           #endif
           GetSysLkrq();
	   nSql=RunSql("insert into dl$lkzwmx(clkzh,cyhbz,nlklsh,cdllsh, \
	            clkjyjh,cdljyjh,cczydh,cczymc,clxbz,djyje,ndzbz, \
	            ndzrq,nsfm,nlkrq) VALUES (%s,%s,%ld,%s,%s,%s,%s,%s,%s,%lf,%ld,%ld,%ld,%ld)",
	            hlkjf.sYyzh,hlkjf.sYhh,nlsh,cdllsh,LKJH,".","DL185","DL185",
	            ".",hlkjf.dJyje,0,19990101,nSysSfm,nSysLkrq);
	   if(nSql<=0)
             return ApplyToClient("%s",LKJY_ERR_DB);
           return ApplyToClient("%s%s",lkywcode,lkywinfo);
	}
	else
	{
	    #ifdef TEST
	      printf("\n  绿卡缴费失败！");
	    #endif
	    return ApplyToClient("%s",sbuf);
	}
}

/*-------------*
  绿卡恢复交易
 *-------------*/
long dsTaskLkqxjf(char * input)
{
	char   sjym[15+1];//交易码
	char   syhzh[19+1];//用户帐号帐号
        char   syhh[20+1];
	char   spin[16+1];//用户密码
	char   szdbsm[8+1];//终端标识码
	char   sjyjdm[15+1];//交易局代码
	char   sczydh[8+1];//操作员代号
	char   slsh[40+1];//代理系统流水号
        double djyje;//交易金额
	long   nbdjyrq;
	long   nbdjysj;

	long	nyhzhll;
	long    nlsh;
	long    njyje;
	long    nlkls;
	int     npinlen;
	int     DesLen;
	int     nSocket;
	int     nSql;
	char    LKJH[LKJH_LEN+1];
	char	WT_B[128+1],WT[32+1];
	char    sYHZH[21+1];
	char    YDMY[ASCII_LEN+1],MACKEY[ASCII_LEN+1],PINKEY[ASCII_LEN+1];
	char    MACKEY_BCD[BCD_LEN+1],PINKEY_BCD[BCD_LEN+1];
	char    PIN_BCD[BCD_LEN+1],PIN[ASCII_LEN+1];
	char    sDesStr[31+1];//交易金额加用户帐号
	char    Des_BCD[BCD_LEN+1],Des_ASCII[ASCII_LEN+1];
        char    MAC[16+1],MAC_BCD[8+1];
	char    sSendPackage[256+1];
	char    sbuf[104+1];
        char    cdllsh[41];
        long    ylsh;
	
	/*产生交易位图*/
	inichar(WT_B,128);
        WT_B[0]='1';
	WT_B[1]='1';
	WT_B[2]='1';
	WT_B[3]='1';
	WT_B[10]='1';
	WT_B[11]='1';
	WT_B[12]='1';
	WT_B[27]='1';
	WT_B[40]='1';
	WT_B[41]='1';
	WT_B[89]='1';
	WT_B[127]='1';
	BinaryToHex(WT_B,WT,16);
printf("\n input string is [%s]",input);
	FetchStringToArg(input,"%19s%16s%12f%4d%6d%20s%40s",syhzh,PIN,
                         &djyje,&nbdjyrq,&nbdjysj,syhh,cdllsh);
printf("\n  syhzh is [%s]",syhzh);

	/*按8583标准形式的绿卡主帐号syhzh*/
	nyhzhll=strlen(syhzh);
	sprintf(sYHZH,"%02ld%s",nyhzhll,syhzh);
	/*个人密码的戴氏加密*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	/*8583方式表示的交易金额*/
	njyje=(long)(djyje*100);
printf("\n ************************* JYJE IS [%ld]",njyje);
	/*产生绿卡系统的流水号*/
	strcpy(sequance,"lklsh");
	nlsh=FormLsh(sequance);
        GetSysLkrq();
printf("\n #####################  DLLSH IS [%s]",cdllsh);
        RunSelect("select nlklsh from dl$lkzwmx where cdllsh=%s into %ld",
                  cdllsh,&ylsh);
printf("\n ^^^^^^^^^^^^^ ylsh is [%ld]",ylsh);
        printf("\n  &&&&&&&&&&&&&& YLSH IS [%ld]",ylsh);
	/*形成绿卡缴费发送包*/
	sprintf(sSendPackage,"%4s%s%s%s%012ld%06ld%06ld%04ld%s%8s%015s%8ld%07s%07ld",
		LKQXJF_BWLXBSF,WT,sYHZH,LKQXJF_JYLXM,njyje,nlsh,nSysSfm,nSysLkrq%10000,"000000000",ZDBSM_LK,LKJH,nSysLkrq,LKJH,ylsh);
	GetMac(sSendPackage,strlen(sSendPackage),MACKEY_BCD,MAC_BCD);
        BCDtoASCII(MAC_BCD,8,MAC);
        strcat(sSendPackage,MAC);
        /*建立SOCKET连接*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
		return(-1);
	else
		write(nSocket,sSendPackage,strlen(sSendPackage));
	read(nSocket,sbuf,105);
	//nSql=RunSql("insert into dl$lkzwmx values()");
}


/*------------*
   绿卡对帐
 *------------*/
long dsTaskLkdz()
{
	char	WT_B[128+1],WT[32+1];
	char	YDMY[ASCII_LEN+1],MACKEY[ASCII_LEN+1],PINKEY[ASCII_LEN+1];
	char    MACKEY_BCD[BCD_LEN+1];
	char    MAC_BCD[BCD_LEN+1];
	char    MAC_ASCII[ASCII_LEN+1];
	char    sequance[5+1];
	char    DesStr[12+1];
	char    sSendPackage[146+1];
	char    buf[102+1];
	char    LKJYZDBSM[8+1];//终端表识码
	char    LKJH[LKJH_LEN+1];
	long    nlsh;
	long    nJysj,nJyrq;
	long    njybs;
	long    njyje;
	long    nsxf;
	long    nSocket;
	double  djyje;

	/*产生交易位图*/	
	inichar(WT_B,128);
	WT_B[0]='1';
	WT_B[1]='1';
	WT_B[2]='1';
	WT_B[10]='1';
	WT_B[11]='1';
	WT_B[12]='1';
	WT_B[40]='1';
	WT_B[41]='1';
	WT_B[120]='1';
	WT_B[127]='1';
	BinaryToHex(WT_B,WT,16);
	#ifdef TEST
	  printf("\n  WT IS  [%s]",WT);
	#endif
	/*产生绿卡交易流水号（最大6位的序列）*/
	strcpy(sequance,"lklsh");
	nlsh=FormLsh(sequance);
	/*绿卡交易日期和交易时间*/
	GetLkJysj(&nJysj,&nJyrq);
	/*取绿卡对帐数据*/
        RunSql("update dl$lkzwmx set ndzbz=9 where ndzbz=0");
	njybs=getlkjybs();
	djyje=getlkjyje();
	njyje=(long)(djyje*100);
	nsxf=0;
	/*取出绿卡交易用的密钥*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	sprintf(sSendPackage,"%4s%32s190000000000000000000%6s%06ld%06ld%04ld%8s%015s030%06ld%012ld%012ld",
	LKDZ_BWLXBSF,WT,LKDZ_JYLXM,nlsh,nJysj,nJyrq,ZDBSM_LK,LKJH,njybs,njyje,nsxf);
	printf("\n %s \n",sSendPackage);
	//ASCIItoBCD(YDMY,16,YDMY_BCD);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
        printf("MACKEY_BCD = [%s]\n",MACKEY_BCD);
	//ASCIItoBCD(PINKEY,16,PINKEY_BCD);
	GetMac(sSendPackage,strlen(sSendPackage),MACKEY_BCD,MAC_BCD);
        printf("ll is [%ld]\n",strlen(sSendPackage));
        printf("MAC_BCD=[%s]",MAC_BCD);
        BCDtoASCII(MAC_BCD,8,MAC_ASCII);
        printf("\n  MAC校验码 [%s]",MAC_ASCII);

        strcat(sSendPackage,MAC_ASCII);
	printf("\n sendpackge is [%s] ",sSendPackage);
	/*建立SOCKET连接*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
        {       printf("\n  connect socker err !!!");
                //keyb();
	        return ApplyToClient("%s",LKJY_ERR_TX);
        }
	else
	{
		if(write(nSocket,sSendPackage,strlen(sSendPackage))!=0)
	                return ApplyToClient("%s",LKJY_ERR_TX);
	}
	if(read(nSocket,buf,75)<0)
	{
	        return ApplyToClient("%s",LKJY_ERR_TX);
	}
        return ApplyToClient("%s","00");
	close(nSocket);
        RunSql("update dl$lkzwmx set ndzbz=1,ndzrq=%ld where ndzbz=9",nSysLkrq);
	/*取出响应位元*/
	sscanf(buf+52,"%2s",lkywcode);
        lkinfo();
	CommitWork();
}

long dsTask185jhmy()
{
	char key[8];
	char key_hex[16];
	int nsql;

	get_key(key);
	BCDtoASCII(key,8,key_hex);
	nsql=RunSql("update dl$dlywcs set ccs=%s where nbh=57",key_hex);	/*mackey*/
	nsql=RunSql("update dl$dlywcs set ccs=%s where nbh=58",key_hex);	/*pinkey*/
	CommitWork();
	return ApplyToClient("%s",key_hex);
}


long daTaskDl185Jyjecx()
{
	
}


/*--------------------------------------------*/


/*------------------*
    绿卡交易时间
 *------------------*/
void	GetLkJysj(long *nJysj,long *nJyrq)
{
	GetSysLkrq();
	*nJysj=nSysSfm;
	*nJyrq=(nSysLkrq%10000);
}


/*-----------------------------------------------------*
  函数:ReadLkxx;
  功能:取出绿卡接口用的密钥和绿卡局号;
  参数:返回密钥地址指针;
  返回值：-1读取失败，1读取成功
 *-----------------------------------------------------*/ 
long ReadLkxx(char *YDMY,char *MACKEY,char *PINKEY,char *LKJH)
{
	long nId;

	nId=RunSelect("select ccs from dl$dlywcs where nbh=51 into %s",LKJH);
	if(nId<=0)
	{
		return(-1);
	}
	nId=RunSelect("select ccs from dl$dlywcs where nbh=52 into %s",YDMY);
	if(nId<=0)
	{
		return(-1);
	}
	nId=RunSelect("select ccs from dl$dlywcs where nbh=53 into %s",MACKEY);
	if(nId<=0)
	{
		return(-1);
	}
	nId=RunSelect("select ccs from dl$dlywcs where nbh=54 into %s",PINKEY);
	if(nId<=0)
	{
		return(-1);
	}	
	return 1;
}

/*-----------------------------------------------------*
  函数:inichar;
  功能:初始化字符数组;
  参数:无;
  返回值:无
 *-----------------------------------------------------*/ 
void	inichar(char *array,long n)
{
	int	i;
	for(i=0;i<n;i++)
	{
		array[i]='0';
	}
	array[n]='\0';
}

/*-----------------------------------------*
 函数:BinaryToHex;
 功能:将二进制串转换成十六进制的串;
 参数:bin二进制串;
     转换后的十六进制的串;
 注意：size为bin的字节数,最大不能超过256个字节.
 *-----------------------------------------*/
void BinaryToHex(char *bin,char *hexASC,long size)
{
	char *p,hex[257];
	long npos;
	long nform;
	
	memset(hex,0,257);
	p=bin;
	for(npos=0;npos<size;npos++)
	{
		for(nform=0;nform<8;nform++)
		{
			hex[npos]<<=1;
			hex[npos]|=((char)(p[0]=='1')&0x01);
			p++;
		}
	}
	BCDtoASCII(hex,size,hexASC);
}

/*----------------------*
    绿卡交易的错误信息
 *----------------------*/
void  lkinfo()
{
	if(strcmp(lkywcode,"00")==0)
		strcpy(lkywinfo,"交易成功！");
	else if(strcmp(lkywcode,"A1")==0)
		strcpy(lkywinfo,"查询发卡方原因！");
	else if(strcmp(lkywcode,"A4")==0)
		strcpy(lkywinfo,"没收卡！");
	else if(strcmp(lkywcode,"A5")==0)
		strcpy(lkywinfo,"不予承队兑！");
	else if(strcmp(lkywcode,"A9")==0)
		strcpy(lkywinfo,"请求正在处理中！");
	else if(strcmp(lkywcode,"B2")==0)
		strcpy(lkywinfo,"无效交易！");
	else if(strcmp(lkywcode,"B3")==0)
		strcpy(lkywinfo,"无效金额！");
	else if(strcmp(lkywcode,"B4")==0)
		strcpy(lkywinfo,"无效卡号！");
	else if(strcmp(lkywcode,"B5")==0)
		strcpy(lkywinfo,"无此开户方！");
	else if(strcmp(lkywcode,"23")==0)
		strcpy(lkywinfo,"不可接受的手续费！");		
	else if(strcmp(lkywcode,"25")==0)
		strcpy(lkywinfo,"未能找到文件上记录！");				
	else if(strcmp(lkywcode,"29")==0)
		strcpy(lkywinfo,"文件更新不成功！");
	else if(strcmp(lkywcode,"30")==0)
		strcpy(lkywinfo,"格式或信息内容编辑错！");
	else if(strcmp(lkywcode,"33")==0)
		strcpy(lkywinfo,"过期卡！");
	else if(strcmp(lkywcode,"37")==0)
		strcpy(lkywinfo,"受卡方呼受理方安全保密部门（没收）！");
	else if(strcmp(lkywcode,"38")==0)
		strcpy(lkywinfo,"输入的PIN次数超限（没收）！");
	else if(strcmp(lkywcode,"40")==0)
		strcpy(lkywinfo,"请求的功能尚不支持！");
	else if(strcmp(lkywcode,"41")==0)
		strcpy(lkywinfo,"已挂失卡！");
	else if(strcmp(lkywcode,"43")==0)
		strcpy(lkywinfo,"被窃卡！");
	else if(strcmp(lkywcode,"51")==0)
		strcpy(lkywinfo,"余额不足！");
	else if(strcmp(lkywcode,"53")==0)
		strcpy(lkywinfo,"无此帐户！");
	else if(strcmp(lkywcode,"54")==0)
		strcpy(lkywinfo,"过期卡！");
	else if(strcmp(lkywcode,"55")==0)
		strcpy(lkywinfo,"密码错！");		/*重复信息*/
	else if(strcmp(lkywcode,"57")==0)
		strcpy(lkywinfo,"不允许持卡人进行的交易！");
	else if(strcmp(lkywcode,"61")==0)
		strcpy(lkywinfo,"超出提款限额！");
	else if(strcmp(lkywcode,"64")==0)
		strcpy(lkywinfo,"原始金额不正确！");
	else if(strcmp(lkywcode,"65")==0)
		strcpy(lkywinfo,"超出取款次数限制！");
	else if(strcmp(lkywcode,"66")==0)
		strcpy(lkywinfo,"受卡方呼受理方安全保密部门！");
	else if(strcmp(lkywcode,"75")==0)
		strcpy(lkywinfo,"出入的PIN次数超限！");
	else if(strcmp(lkywcode,"76")==0)
		strcpy(lkywinfo,"已挂失卡！");
	else if(strcmp(lkywcode,"77")==0)
		strcpy(lkywinfo,"帐户已冻结！");
	else if(strcmp(lkywcode,"78")==0)
		strcpy(lkywinfo,"帐号已清户！");
	else if(strcmp(lkywcode,"79")==0)
		strcpy(lkywinfo,"原交易已被取消或冲正！");
	else if(strcmp(lkywcode,"80")==0)
		strcpy(lkywinfo,"帐户被临时锁定！");
	else if(strcmp(lkywcode,"81")==0)
		strcpy(lkywinfo,"该帐号已开设网上帐户！");
	else if(strcmp(lkywcode,"83")==0)
		strcpy(lkywinfo,"身份证号码不符！");
	else if(strcmp(lkywcode,"84")==0)
		strcpy(lkywinfo,"存折印刷号有误！");
	else if(strcmp(lkywcode,"85")==0)
		strcpy(lkywinfo,"当日存入的金额不能支取！");
	else if(strcmp(lkywcode,"88")==0)
		strcpy(lkywinfo,"未做卡挂失不能解除卡挂失！");
	else if(strcmp(lkywcode,"89")==0)
		strcpy(lkywinfo,"未做折挂失不能解除折挂失！");
	else if(strcmp(lkywcode,"90")==0)
		strcpy(lkywinfo,"日期切换正在处理！");
	else if(strcmp(lkywcode,"91")==0)
		strcpy(lkywinfo,"开户方不能操作！");
	else if(strcmp(lkywcode,"94")==0)
		strcpy(lkywinfo,"重复交易！");
	else if(strcmp(lkywcode,"99")==0)
		strcpy(lkywinfo,"发明细文件！");
	else if(strcmp(lkywcode,"D0")==0)
		strcpy(lkywinfo,"文件错误！");
	else if(strcmp(lkywcode,"F0")==0)
		strcpy(lkywinfo,"TPE相关错误1！");
	else if(strcmp(lkywcode,"F1")==0)
		strcpy(lkywinfo,"TPE相关错误2！");
	else if(strcmp(lkywcode,"F7")==0)
		strcpy(lkywinfo,"TPE网络通讯错误！");
	else if(strcmp(lkywcode,"F8")==0)
		strcpy(lkywinfo,"此交易在此TPE状态下不能执行！");
	else if(strcmp(lkywcode,"F9")==0)
		strcpy(lkywinfo,"系统错误！");
	else if(strcmp(lkywcode,"MR")==0)
		strcpy(lkywinfo,"TPEROUTE错误！");
	else if(strcmp(lkywcode,"M0")==0)
		strcpy(lkywinfo,"起息日必须小于等于营业日！");
	else if(strcmp(lkywcode,"Z0")==0)
		strcpy(lkywinfo,"全国网关保密子系统失败！");
	else if(strcmp(lkywcode,"Z1")==0)
		strcpy(lkywinfo,"日期错！");
	else if(strcmp(lkywcode,"Z2")==0)
		strcpy(lkywinfo,"MAC鉴别失败！");
	else if(strcmp(lkywcode,"Z3")==0)
		strcpy(lkywinfo,"PIN格式出错！");
	else if(strcmp(lkywcode,"Z4")==0)
		strcpy(lkywinfo,"发卡方保密子系统失败！");
	else if(strcmp(lkywcode,"Z5")==0)
		strcpy(lkywinfo,"密钥验证失败！");
	else if(strcmp(lkywcode,"Z6")==0)
		strcpy(lkywinfo,"与履历电文相异！");
	else if(strcmp(lkywcode,"Z7")==0)
		strcpy(lkywinfo,"交易未按交易应该出现的次序出现！");
	else if(strcmp(lkywcode,"Z8")==0)
		strcpy(lkywinfo,"用户中心未就绪，无法转发！");
	else if(strcmp(lkywcode,"Z9")==0)
		strcpy(lkywinfo,"开户方网关未就绪，无法转发！");
	else if(strcmp(lkywcode,"ZA")==0)
		strcpy(lkywinfo,"全国网关未就绪，无法转发！");
	else if(strcmp(lkywcode,"ZB")==0)
		strcpy(lkywinfo,"全国网关收不到开户方网关的应答！");
	else if(strcmp(lkywcode,"ZD")==0)
		strcpy(lkywinfo,"开户方网关收不到处理中心的应答！");
	else if(strcmp(lkywcode,"ZG")==0)
		strcpy(lkywinfo,"原始交易不成功！");
	else if(strcmp(lkywcode,"T0")==0)
		strcpy(lkywinfo,"没有所请求的明细记录！");
	else if(strcmp(lkywcode,"ZZ")==0)
		strcpy(lkywinfo,"其他未知错误！");
	else strcpy(lkywinfo,"系统未标识的错误！");
}

/*-----------------------*
  函数:FormLs;
  功能:产生递增的序列;
  参数:序列名 sSequence;
  返回值:cLsh;
 *-----------------------*/ 
long FormLsh(char *sSequence)
{
  int nRetVal;
  long nValue;
  
  nRetVal=RunSelect("SELECT %t.nextval FROM dual INTO %ld",
  	sSequence,&nValue);
  if(nRetVal<=0)
  {
	return -1;
  }//endif     
  return nValue;
}

/*-----------------------------------------------------*
  函数:getlkjybs;
  功能:计算绿卡交易的笔数;
  参数:无;
  返回值:交易笔数;
 *-----------------------------------------------------*/ 
long getlkjybs()
{
	int nsql;
	long njybs;
	
	nsql=RunSelect("Select count(*) From dl$lkzwmx where ndzbz=9  into %ld ",&njybs);
	if(nsql<0)
	{
		return(-1);
	}
	else
		return njybs;
}

/*-----------------------------------------------------*
  函数:getlkjyje;
  功能:计算绿卡交易金额;
  参数:无;
  返回值:交易金额;
 *-----------------------------------------------------*/ 
double getlkjyje()
{
	int nsql;
	double djyje;
	
	nsql=RunSelect("Select sum(djyje) From dl$lkzwmx where ndzbz=9 into %lf ",&djyje);
	if(nsql<0)
	{
		return(-1);
	}
	else
		return djyje;
}

/*
 *form a key of 8 byte
 */
int get_key(char *key)
{
	int leap,count;
	time_t Time;
	
	Time=time(NULL);
	srand(Time);
	for(count=0;count<8;count++)
	{
		leap=rand()%94;
		*key++=leap+33;
		*key='\0';
	}
}

