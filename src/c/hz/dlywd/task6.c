/***********************************************************
**
** �ļ�: task6.c
** Ŀ��: 185������
**
** �������ڣ�2001.8
** ԭʼ�����Ա������
** �޸���ʷ
** ����        �޸���Ա  ԭ��
**
************************************************************/
#include <time.h>
#include "dlywd.h"

#define  LKHOST		"10.94.12.1"//�̿�ͨѶ������
#define  LKSVRNAME	"5008"  //�̿�ͨѶ��������˿���
#define  ZDBSM_LK       "00000000" //�ն˱�ʶ��

char lkywcode[3];
char lkywinfo[129];

#define  TEST
#define  LKJY_SUCCESS	"00" //�̿����׳ɹ�
#define  LKJY_ERR_TX	"01" //��̨ͨѶʧ��
#define  LKJY_ERR_DB	"02" //DLYWD�������ֿ�ʧ��
#define  LKJY_ERR_LKJY  "03" //�̿����״���

#define  BCD_LEN	8
#define  ASCII_LEN	16
#define  LKQD_SEND_LEN  107
#define  LKJH_LEN       20

extern long nSysLkrq;
extern long nSysSfm;
extern char sSysJh[9+1];
extern char sSysErrInfo[81];
static char sequance[5+1];

/*�̿�ǩ��*/
#define  LKQD_BWLXBSF_FD	"0820"
#define  LKQD_BWLXBSF_DF	"0830"
#define  LKQD_JYLXM		"091001"
/*�̿�ǩ��*/
#define  LKQT_BWLXBSF_FD	"0820"
#define  LKQT_BWLXBSF_DF	"0830"
#define  LKQT_JYLXM		"091002"
/*�̿�������Կ*/
#define  LKJHMY_BWLXBSF_FD	"0800"
#define  LKJHMY_BWLXBSF_DF	"0810"
#define  LKJHMY_JYLXM		"091003"
/*�̿��ɷ�*/
#define  LKJF_BWLXBSF		"0200"
#define  LKJF_JYLXM		"056031"
/*�̿�ȡ���ɷ�*/
#define  LKQXJF_BWLXBSF		"0420"
#define  LKQXJF_JYLXM		"056931"
#define  LKQXJF_JFFS		"000"
#define  LKQXJF_FWDTJM		"00"
/*�̿��ʻ�����ѯ*/
#define  LKZHYECX_BWLXBSF	"0200"//�������ͱ�ʶ��
#define  LKZHYECX_JYLXM		"053070"//����������
/*�̿����ʶ���*/
#define  LKDZ_BWLXBSF		"0500"//�������ͱ�ʶ��
#define  LKDZ_JYLXM		"091004"//����������
/*�̿���ϸ����*/
#define  LKMXDZ_BWLXBSF		"0522"//�������ͱ�ʶ��
#define  LKMXDZ_JYLXM		"091011"//����������

//�绰���нɷ�
typedef struct lkjf
{
    char   sYyzh[19+1];//�����ʺ�
    char   sYymm[16+1];//��������
    char   sYhh[20+1]; //�û���
    long   nDsfzlh;    //���ո������
    double dJyje;      //���׽��
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

/*�̿�ǩ������*/
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

	/*���㽻��λͼ*/
	inichar(WT_B,128);
	WT_B[0]='1';
	WT_B[2]='1';
	WT_B[11]='1';
	WT_B[12]='1';
	WT_B[40]='1';
	WT_B[41]='1';
	WT_B[95]='1';
	WT_B[127]='1';
	BinaryToHex(WT_B,WT,16);   /*16 ��ʾ�ֽ���*/
	#ifdef  TEST
		//clrscr();
		printf("\n  �̿�ǩ������λͼ: [%s]",WT);
	#endif
	
	/*�̿��������ںͽ���ʱ��*/
	GetLkJysj(&nJysj,&nJyrq);
 	#ifdef	TEST
 	    printf("\n  ����ʱ�� : [%ld] ",nJysj);
 	    printf("\n  �������� : [%ld] ",nJyrq);    
 	#endif
	
	/*���㱨�İ�ȫ��*/
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
		printf("\n  ��������Կ : [%s]",YDMY_BCD);
		printf("\n  MACKEY     : [%s]",MACKEY_BCD);
	#endif	
        EnCrypt(MACKEY_BCD,YDMY_BCD,BWAQM_BCD);
        //EnCrypt(MACKEY_BCD,"12345678",BWAQM_BCD);
        BCDtoASCII(BWAQM_BCD,8,BWAQM_ASCII);
	#ifdef	TEST
		printf("\n  ���İ�ȫ�� : [%s]",BWAQM_ASCII);
	#endif
	
	/*�ϳɷ��ͱ���*/
	#ifdef TEST
		//printf("\n  �̿�ǩ������1 : %s",sSendPackage);
		printf("\n  ǩ�������ֶ����� : [%4s][%32s][%6s][%06ld][%04ld][%8s][%015s][%16s]",
		       LKQD_BWLXBSF_FD,WT,LKQD_JYLXM,nJysj,nJyrq,ZDBSM_LK,LKJH,BWAQM_ASCII);
	#endif
	sprintf(sSendPackage,"%4s%32s%6s%06ld%04ld%8s%015s%16s",
		LKQD_BWLXBSF_FD,WT,LKQD_JYLXM,nJysj,nJyrq,ZDBSM_LK,LKJH,BWAQM_ASCII);

	#ifdef TEST
		printf("\n  �̿�ǩ������ : [%s]",sSendPackage);
	#endif
	
	/*����MACУ����*/
	GetMac(sSendPackage,strlen(sSendPackage),YDMY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC_ASCII);
	#ifdef	TEST
		printf("\n  ���ļ����� : [%s]",MAC_ASCII);
	#endif
	strcat(sSendPackage,MAC_ASCII);
	#ifdef	TEST
		printf("\n  �̿�ǩ������ : [%s]",sSendPackage);
	#endif	

	/*����SOCKET����*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		#ifdef TEST
			printf("\n  nSocket is : %ld ����SOCKET����ʧ��!",nSocket);
			printf("\n  Socket ErrorInfo is : %s ",sSysErrInfo);
		#endif
		return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);
	}
	else
	{
		#ifdef TEST
			printf("\n  ����SOCKET���ӳɹ�!");
		#endif
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			#ifdef TEST
			   printf("\n  д���Ĵ���!");
			#endif
			return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);
		}
	}
	if(read(nSocket,buf,75)<0)
	{
		#ifdef TEST
		   printf("\n  �����ر��Ĵ���!");
		#endif
		return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);	
	}
	#ifdef TEST
		printf("\n  �̿�ǰ�û���Ӧ���� : [%s]",buf);
	#endif
	close(nSocket);	//�ر�SOCKET
	
	/*����Ӧ����*/
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
                 printf("\n  �������ݿ�ʧ�ܣ�");
		 return ApplyToClient("%s",LKJY_ERR_DB);
	   }
	   else
	   {
		  CommitWork();
		  #ifdef TEST
		     printf("\n  ǩ�����׳ɹ�!");
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
	      printf("\n  �̿�ǩ��ʧ�ܣ�");
	    #endif
	    return ApplyToClient("%s",buf);
	}
	//close(nSocket);	//�ر�SOCKET
}

/*----------------------------*
   ����ϵͳ���̿�ǰ�û�ǩ��
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

	/*���㽻��λͼ*/
	inichar(WT_B,128);
	WT_B[0]='1';
	WT_B[2]='1';
	WT_B[11]='1';
	WT_B[12]='1';
	WT_B[40]='1';
	WT_B[41]='1';
	WT_B[95]='1';
	WT_B[127]='1';
	BinaryToHex(WT_B,WT,16);   /*16 ��ʾ�ֽ���*/
	#ifdef  TEST
		//clrscr();
		printf("\n  �̿�ǩ�˽���λͼ: [%s]",WT);
	#endif
	
	/*�̿��������ںͽ���ʱ��*/
	GetLkJysj(&nJysj,&nJyrq);
 	#ifdef	TEST
 	    printf("\n  ����ʱ�� : [%ld] ",nJysj);
 	    printf("\n  �������� : [%ld] ",nJyrq);    
 	#endif
	
	/*���㱨�İ�ȫ��*/
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
		printf("\n  ��������Կ : [%s]",YDMY_BCD);
		printf("\n  MACKEY     : [%s]",MACKEY_BCD);
	#endif	
        EnCrypt(MACKEY_BCD,YDMY_BCD,BWAQM_BCD);
        //EnCrypt(MACKEY_BCD,"12345678",BWAQM_BCD);
        BCDtoASCII(BWAQM_BCD,8,BWAQM_ASCII);
	#ifdef	TEST
		printf("\n  ���İ�ȫ�� : [%s]",BWAQM_ASCII);
	#endif
	
	/*�ϳɷ��ͱ���*/
	#ifdef TEST
		//printf("\n  �̿�ǩ�˱���1 : %s",sSendPackage);
		printf("\n  ǩ�˱����ֶ����� : [%4s][%32s][%6s][%06ld][%04ld][%8s][%015s][%16s]",
		       LKQD_BWLXBSF_FD,WT,LKQD_JYLXM,nJysj,nJyrq,ZDBSM_LK,LKJH,BWAQM_ASCII);
	#endif
	sprintf(sSendPackage,"%4s%32s%6s%06ld%04ld%8s%015s%16s",
		LKQT_BWLXBSF_FD,WT,LKQT_JYLXM,nJysj,nJyrq,ZDBSM_LK,LKJH,BWAQM_ASCII);

	#ifdef TEST
		printf("\n  �̿�ǩ�˱��� : [%s]",sSendPackage);
	#endif
	
	/*����MACУ����*/
	GetMac(sSendPackage,strlen(sSendPackage),YDMY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC_ASCII);
	#ifdef	TEST
		printf("\n  ���ļ����� : [%s]",MAC_ASCII);
	#endif
	strcat(sSendPackage,MAC_ASCII);
	#ifdef	TEST
		printf("\n  �̿�ǩ�˱��� : [%s]",sSendPackage);
	#endif	

	/*����SOCKET����*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		#ifdef TEST
			printf("\n  nSocket is : %ld ����SOCKET����ʧ��!",nSocket);
			printf("\n  Socket ErrorInfo is : %s ",sSysErrInfo);
		#endif
		return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);
	}
	else
	{
		#ifdef TEST
			printf("\n  ����SOCKET���ӳɹ�!");
		#endif
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			#ifdef TEST
			   printf("\n  д���Ĵ���!");
			#endif
			return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);
		}
	}
	if(read(nSocket,buf,75)<0)
	{
		#ifdef TEST
		   printf("\n  �����ر��Ĵ���!");
		#endif
		return ApplyToClient("%s%s",LKJY_ERR_TX,sSysErrInfo);	
	}
	#ifdef TEST
		printf("\n  �̿�ǰ�û���Ӧ���� : [%s]",buf);
	#endif
	close(nSocket);	//�ر�SOCKET
	
	/*����Ӧ����*/
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
                 printf("\n  �������ݿ�ʧ�ܣ�");
		 return ApplyToClient("%s",LKJY_ERR_DB);
	   }
	   else
	   {
		  CommitWork();
		  #ifdef TEST
		     printf("\n  ǩ�˳ɹ�!");
		  #endif
           }
	   return ApplyToClient("%s%s",lkywcode,lkywinfo);
	}
	else
	{
	    #ifdef TEST
	      printf("\n  �̿�ǩ��ʧ�ܣ�");
	    #endif
	    return ApplyToClient("%s",buf);
	}
}


/*----------------------------*
     �̿�������Կ����
     nflagΪ1��ʾ����MACKEY
     nflagΪ2��ʾ����PINKEY
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

	/*��������λͼ*/
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
	printf("\n  ������Կ����λͼ : [%s]",WT);
	#endif	

	/*�̿���������*/
	GetLkJysj(&nJysj,&nJyrq);
 	#ifdef	TEST
 	printf("\n  ����ʱ�� : [%ld] ",nJysj);
 	printf("\n  �������� : [%ld] ",nJyrq);    
	#endif	
	
	/*���İ�ȫ������Ϣ*/
	if(nFlag==1)
		strcpy(AQKZXX,"1000000000000000");//��ʾ����MACKEY
	else
		strcpy(AQKZXX,"2000000000000000");//��ʾ����PINKEY
	
	/*���ɴ���У����*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	#ifdef  TEST
		printf("\n  YDMY IS   : [%s] ",YDMY);
		printf("\n  MACKEY IS : [%s] ",MACKEY);
		printf("\n  PINKEY IS : [%s] ",PINKEY);
		printf("\n  LKJH IS   : [%s] ",LKJH);
	#endif
	ASCIItoBCD(YDMY,16,YDMY_BCD);
	printf("\n ����Կ [%s]",YDMY_BCD);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	printf("\n MACKEY_BCD [%s]",MACKEY_BCD);
	EnCrypt(MACKEY_BCD,YDMY_BCD,BWAQM_BCD);
	BCDtoASCII(BWAQM_BCD,8,BWAQM_ASCII);
	#ifdef TEST
	printf("\n  ���İ�ȫ�� : [%s]",BWAQM_ASCII);
	#endif
	
	/*�ϳ�ͨѶ����*/
	#ifdef TEST
	//printf("\n ������Կ���ͱ������� : [%s]",sSendPackage);
	printf("\n ������Կ�������� : [%4s][%32s][%6s][%06ld][%04ld][%-8s][%015s][%16s][%16s]",
	  LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
	  LKJH,AQKZXX,BWAQM_ASCII);
	#endif
	sprintf(sSendPackage,"%4s%32s%6s%06ld%04ld%-8s%015s%16s%16s",
		LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
		LKJH,AQKZXX,BWAQM_ASCII);
	#ifdef TEST
	printf("\n ������Կ���ͱ���1 : [%s]",sSendPackage);
	#endif

	/*����MACУ����*/
	GetMac(sSendPackage,strlen(sSendPackage),YDMY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC_ASCII);
	strcat(sSendPackage,MAC_ASCII);
       	#ifdef TEST
       	printf("\n ������Կ���ͱ���2 : [%s]",sSendPackage);
       	#endif

	/*����SOCKET����*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		printf("\n SOCKET����");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	else
	{
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			printf("\n SOCKET����");
			return ApplyToClient("%s",LKJY_ERR_TX);
		}
	}
	memset(buf,'\0',257);
	if(read(nSocket,buf,107)<0)
	{
		printf("\n SOCKET����");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	close(nSocket);
	
	/*����Ӧ����*/
	if(strncpy("0244",buf,4)!=0)
	{
        	printf("\n ������Կ�յ��ı��� [%s]",buf);
		//sscanf(buf+52,"%2s",lkywcode);
                strncpy(lkywcode,buf+52,2);
                lkywcode[2]='\0';
		#ifdef TEST
		printf("\n ������Կ��Ӧ�� : [%s]",lkywcode);
		#endif
		
		/*����MACKEY*/
		if(nFlag==1)
		{
			memset(NEWMACKEY,'\0',17);
			strncpy(NEWMACKEY,buf+91,16);
			#ifdef TEST
			printf("\n ������Կ�յ�������Կ [%s]",NEWMACKEY);
			#endif
			//DesDecString(YDMY_BCD,8,NEWMACKEY,16,NEWMACKEY_UN,NEWMACKEY_LEN);	/*������MAC*/
			ASCIItoBCD(NEWMACKEY,16,NEWMACKEY_BCD);
			DeCrypt(NEWMACKEY_BCD,YDMY_BCD,NEWMACKEY_UN);
			BCDtoASCII(NEWMACKEY_UN,8,NEWMACKEY_ASCII);	/*��2���Ƹ���ʮ�����Ʊ�ʾ*/
			#ifdef TEST
			printf("THE NEW MACKEY IS : [%s]",NEWMACKEY_ASCII);
			#endif
			sql=RunSql("update dl$dlywcs set ccs=%s where nbh=53",NEWMACKEY_ASCII);	/*����MACKEY*/
			if(sql<=0)
			{
				printf("\n  �������ݿ�ʧ�ܣ�����");
				//return -1;
				return ApplyToClient("%s",LKJY_ERR_DB);
			}
			//return 0;
			return ApplyToClient("%s%s",lkywcode,lkywinfo);
		}
		/*����PINKEY*/
		else if(nFlag==2)
		{
			sscanf(buf+91,"%16s",NEWPINKEY);	/*������ص�PINKEY*/
			printf("\n ������Կ�յ�������Կ [%s]",NEWPINKEY);			 
			//DesDecString(MACKEY_BCD,8,NEWPINKEY,8,NEWPINKEY_UN,NEWPINKEY_LEN);	/*����*/
			ASCIItoBCD(NEWPINKEY,16,NEWPINKEY_BCD);
			DeCrypt(NEWPINKEY_BCD,YDMY_BCD,NEWPINKEY_UN);
			BCDtoASCII(NEWPINKEY_UN,8,NEWPINKEY_ASCII);	/*��2���Ƹ���ʮ�����Ʊ�ʾ*/
			#ifdef TEST
				printf("THE NEW PINKEY IS : %s",NEWPINKEY_ASCII);
			#endif
			sql=RunSql("update dl$dlywcs set ccs=%s where nbh=54",NEWPINKEY_ASCII);	/*����NEWPINKEY*/
			if(sql<=0)
			{
				printf("\n  ��������ʧ�ܣ�����");
				//return -1;
				return ApplyToClient("%s",LKJY_ERR_DB);
		        }
			//return 0;
			return ApplyToClient("%s%s",lkywcode,lkywinfo);
		}
	}
	else
	{
		printf("\n ������Կ����ʧ��");
		//return -1;
		return ApplyToClient("%s",buf);
	}
}


/*----------------------------*
     �̿�������Կ����
     ����PINKEY
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

	/*��������λͼ*/
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
	printf("\n  ������Կ����λͼ : [%s]",WT);
	#endif	

	/*�̿���������*/
	GetLkJysj(&nJysj,&nJyrq);
 	#ifdef	TEST
 	printf("\n  ����ʱ�� : [%ld] ",nJysj);
 	printf("\n  �������� : [%ld] ",nJyrq);    
	#endif	
	
	/*���İ�ȫ������Ϣ*/
	strcpy(AQKZXX,"2000000000000000");//��ʾ����PINKEY
	
	/*���ɴ���У����*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	#ifdef  TEST
		printf("\n  YDMY IS   : [%s] ",YDMY);
		printf("\n  MACKEY IS : [%s] ",MACKEY);
		printf("\n  PINKEY IS : [%s] ",PINKEY);
		printf("\n  LKJH IS   : [%s] ",LKJH);
	#endif
	ASCIItoBCD(YDMY,16,YDMY_BCD);
	printf("\n ����Կ [%s]",YDMY_BCD);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	printf("\n MACKEY_BCD [%s]",MACKEY_BCD);
	EnCrypt(MACKEY_BCD,YDMY_BCD,BWAQM_BCD);
	BCDtoASCII(BWAQM_BCD,8,BWAQM_ASCII);
	#ifdef TEST
	printf("\n  ���İ�ȫ�� : [%s]",BWAQM_ASCII);
	#endif
	
	/*�ϳ�ͨѶ����*/
	#ifdef TEST
	//printf("\n ������Կ���ͱ������� : [%s]",sSendPackage);
	printf("\n ������Կ�������� : [%4s][%32s][%6s][%06ld][%04ld][%-8s][%015s][%16s][%16s]",
	  LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
	  LKJH,AQKZXX,BWAQM_ASCII);
	#endif
	sprintf(sSendPackage,"%4s%32s%6s%06ld%04ld%-8s%015s%16s%16s",
		LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
		LKJH,AQKZXX,BWAQM_ASCII);
	#ifdef TEST
	printf("\n ������Կ���ͱ���1 : [%s]",sSendPackage);
	#endif

	/*����MACУ����*/
	GetMac(sSendPackage,strlen(sSendPackage),YDMY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC_ASCII);
	strcat(sSendPackage,MAC_ASCII);
       	#ifdef TEST
       	printf("\n ������Կ���ͱ���2 : [%s]",sSendPackage);
       	#endif

	/*����SOCKET����*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		printf("\n SOCKET����");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	else
	{
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			printf("\n SOCKET����");
			return ApplyToClient("%s",LKJY_ERR_TX);
		}
	}
	memset(buf,'\0',257);
	if(read(nSocket,buf,256)<0)
	{
		printf("\n SOCKET����");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	close(nSocket);
	
	/*����Ӧ����*/
	if(strncpy("0244",buf,4)!=0)
	{
        printf("\n ������Կ�յ��ı��� [%s]",buf);                                                        
		//sscanf(buf+52,"%2s",lkywcode);                                                                 
                strncpy(lkywcode,buf+52,2);                                                                      
                lkywcode[2]='\0';                                                                                
		#ifdef TEST                                                                                      
		printf("\n ������Կ��Ӧ�� : [%s]",lkywcode);                                                     
		#endif                                                                                           
		                                                                                                 
		sscanf(buf+93,"%16s",NEWPINKEY);	/*������ص�PINKEY*/                             
		printf("\n ������Կ�յ�������Կ [%s]",NEWPINKEY);			                 
		//DesDecString(MACKEY_BCD,8,NEWPINKEY,8,NEWPINKEY_UN,NEWPINKEY_LEN);	/*����*/         
		ASCIItoBCD(NEWPINKEY,16,NEWPINKEY_BCD);                                                  
		DeCrypt(NEWPINKEY_BCD,YDMY_BCD,NEWPINKEY_UN);                                            
		BCDtoASCII(NEWPINKEY_UN,8,NEWPINKEY_ASCII);	/*��2���Ƹ���ʮ�����Ʊ�ʾ*/              
		#ifdef TEST                                                                              
			printf("\n THE NEW PINKEY IS : %s",NEWPINKEY_ASCII);                                
		#endif                                                                                   
		sql=RunSql("update dl$dlywcs set ccs=%s where nbh=54",NEWPINKEY_ASCII);	/*����NEWPINKEY*/
		if(sql<=0)                                                                               
		{                                                                                        
			printf("\n  ��������ʧ�ܣ�����");                                                
			//return -1;                                                                     
			return ApplyToClient("%s",LKJY_ERR_DB);                                          
		}                                                                                        
		else
		    return ApplyToClient("%s%s",lkywcode,lkywinfo);                                          
	}
	else
	{
		printf("\n ������Կ����ʧ��");
		//return -1;
		return ApplyToClient("%s",buf);
	}
}

/*----------------------------*
     �̿�������Կ����
     ����MACKEY
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

	/*��������λͼ*/
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
	printf("\n  ������Կ����λͼ : [%s]",WT);
	#endif	

	/*�̿���������*/
	GetLkJysj(&nJysj,&nJyrq);
 	#ifdef	TEST
 	printf("\n  ����ʱ�� : [%ld] ",nJysj);
 	printf("\n  �������� : [%ld] ",nJyrq);    
	#endif	
	
	/*���İ�ȫ������Ϣ*/
	strcpy(AQKZXX,"1000000000000000");//��ʾ����MACKEY
	
	/*���ɴ���У����*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	#ifdef  TEST
		printf("\n  YDMY IS   : [%s] ",YDMY);
		printf("\n  MACKEY IS : [%s] ",MACKEY);
		printf("\n  PINKEY IS : [%s] ",PINKEY);
		printf("\n  LKJH IS   : [%s] ",LKJH);
	#endif
	ASCIItoBCD(YDMY,16,YDMY_BCD);
	printf("\n ����Կ [%s]",YDMY_BCD);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	printf("\n MACKEY_BCD [%s]",MACKEY_BCD);
	EnCrypt(MACKEY_BCD,YDMY_BCD,BWAQM_BCD);
	BCDtoASCII(BWAQM_BCD,8,BWAQM_ASCII);
	#ifdef TEST
	printf("\n  ���İ�ȫ�� : [%s]",BWAQM_ASCII);
	#endif
	
	/*�ϳ�ͨѶ����*/
	#ifdef TEST
	//printf("\n ������Կ���ͱ������� : [%s]",sSendPackage);
	printf("\n ������Կ�������� : [%4s][%32s][%6s][%06ld][%04ld][%-8s][%015s][%16s][%16s]",
	  LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
	  LKJH,AQKZXX,BWAQM_ASCII);
	#endif
	sprintf(sSendPackage,"%4s%32s%6s%06ld%04ld%-8s%015s%16s%16s",
		LKJHMY_BWLXBSF_FD,WT,LKJHMY_JYLXM,nSysSfm,nJyrq,ZDBSM_LK,
		LKJH,AQKZXX,BWAQM_ASCII);
	#ifdef TEST
	printf("\n ������Կ���ͱ���1 : [%s]",sSendPackage);
	#endif

	/*����MACУ����*/
	GetMac(sSendPackage,strlen(sSendPackage),YDMY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC_ASCII);
	strcat(sSendPackage,MAC_ASCII);
       	#ifdef TEST
       	printf("\n ������Կ���ͱ���2 : [%s]",sSendPackage);
       	#endif

	/*����SOCKET����*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		printf("\n SOCKET����");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	else
	{
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			printf("\n SOCKET����");
			return ApplyToClient("%s",LKJY_ERR_TX);
		}
	}
	memset(buf,'\0',257);
	if(read(nSocket,buf,256)<0)
	{
		printf("\n SOCKET����");
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	close(nSocket);
	
	/*����Ӧ����*/
	if(strncpy("0244",buf,4)!=0)
	{
        printf("\n ������Կ�յ��ı��� [%s]",buf);
		//sscanf(buf+52,"%2s",lkywcode);
                strncpy(lkywcode,buf+52,2);
                lkywcode[2]='\0';
		#ifdef TEST
		printf("\n ������Կ��Ӧ�� : [%s]",lkywcode);
		#endif
		
		memset(NEWMACKEY,'\0',17);
		strncpy(NEWMACKEY,buf+93,16);
		#ifdef TEST
		printf("\n ������Կ�յ�������Կ [%s]",NEWMACKEY);
		#endif
		//DesDecString(YDMY_BCD,8,NEWMACKEY,16,NEWMACKEY_UN,NEWMACKEY_LEN);	/*������MAC*/
		ASCIItoBCD(NEWMACKEY,16,NEWMACKEY_BCD);
		DeCrypt(NEWMACKEY_BCD,YDMY_BCD,NEWMACKEY_UN);
		
printf("\n �̿�MAC_BCD [%s]",NEWMACKEY_UN);
		BCDtoASCII(NEWMACKEY_UN,8,NEWMACKEY_ASCII);	/*��2���Ƹ���ʮ�����Ʊ�ʾ*/
		#ifdef TEST
		printf("THE NEW MACKEY IS : [%s]",NEWMACKEY_ASCII);
		#endif
		sql=RunSql("update dl$dlywcs set ccs=%s where nbh=53",NEWMACKEY_ASCII);	/*����MACKEY*/
		if(sql<=0)
		{
			printf("\n  �������ݿ�ʧ�ܣ�����");
			//return -1;
			return ApplyToClient("%s",LKJY_ERR_DB);
		}
		else
		        return ApplyToClient("%s%s",lkywcode,lkywinfo);

	}
	else
	{
		printf("\n ������Կ����ʧ��");
		//return -1;
		return ApplyToClient("%s",buf);
	}
}



/*
 *"%16s%12f%4d%6d%20s%40s"
 *"�̿��ʺţ�MAC�����׽����ڣ�ʱ����,�û���"
 */
long dsTaskLkjf(char * input)
{
	char   syhzh[19+1];//�û��ʺ��ʺ�
	char   spin[16+1];//�û�����
	char   nbdjyrq[4+1];//���ؽ�������
	char   nbdjysj[6+1];//���ؽ���ʱ��
	char   djyje[12+1];//���׽��

	unsigned int pinkey[8];
	unsigned int pin_source[8];
	unsigned int pin_target[8];
	long    i;//ѭ������
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
	
	/*��������λͼ*/
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
	printf("\n  �̿��ɷ�λͼ : [%s]",WT);
	#endif
	
	/*��ȡҵ����Ϣ�ֶ�����*/
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

	/*��8583��׼��ʽ���̿����ʺ�syhzh*/
        alltrim(syhzh);
	nyhzh_len=strlen(syhzh);
	sprintf(syhlkzh,"%02ld%s",nyhzh_len,syhzh);
	#ifdef TEST
	printf("\n  �ɷѵ��̿��ʺ� [%s]",syhlkzh);
	#endif
	
	/*��������Ĵ��ϼ���*/
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
	printf("\n  ���ܺ�ĸ���PIN�� [%s]",PIN);
	#endif

	/*8583��ʽ��ʾ�Ľ��׽��*/
	//lkjyje=(long)(djyje*100);
	memset(lkjyje,'\0',12);
	strncpy(lkjyje,djyje,9);
	strcat(lkjyje,"00");
	#ifdef TEST
	printf("\n  �̿��ɷѽ�� [%s]",lkjyje);
	#endif
	
	/*�����̿�ϵͳ����ˮ��*/
	strcpy(sequance,"lklsh");
	nlsh=FormLsh(sequance);
	#ifdef TEST
	printf("\n  �����̿�������ˮ [%ld]",nlsh);
	#endif
	alltrim(LKJH);
        alltrim(hlkjf.sYhh);
	/*�γ��̿��ɷѷ��Ͱ�*/
	#ifdef TEST
	printf("\n  �̿��ɷѱ������� [%4s][%16s][%s][%s][%012s][%06ld][%s][%s][%s][%015s][%16s]",
		LKJF_BWLXBSF,WT,syhlkzh,LKJF_JYLXM,lkjyje,nlsh,nbdjysj,nbdjyrq,ZDBSM_LK,LKJH,PIN);
	#endif
alltrim(hlkjf.sYhh);
	sprintf(sSendPackage,"%4s%16s%s%s%012s%06ld%4s%s%s%015s%s%03ld%s%s",
		LKJF_BWLXBSF,WT,syhlkzh,LKJF_JYLXM,lkjyje,nlsh,nbdjysj,nbdjyrq,ZDBSM_LK,LKJH,PIN,strlen(hlkjf.sYhh)+4,"0001",hlkjf.sYhh);
	printf("\n �̿��ɷ������� [%s]",sSendPackage);
	
	/*����У����*/
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	GetMac(sSendPackage,strlen(sSendPackage),MACKEY_BCD,MAC_BCD);
	BCDtoASCII(MAC_BCD,8,MAC);
	strcat(sSendPackage,MAC);
	#ifdef TEST
	printf("\n  MACУ���� [%s]",MAC);
	#endif
	printf("\n �̿��ɷѷ��ͱ��� [%s]",sSendPackage);
	
	/*����SOCKET����*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
	{
		printf("\n SOCKET����");
		//return -1;
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	else
	{
		if(write(nSocket,sSendPackage,strlen(sSendPackage))<0)
		{
			printf("\n SOCKET����");
			//return -1;
			return ApplyToClient("%s",LKJY_ERR_TX);
		}
	}
	if(read(nSocket,sbuf,105)<0)
	{
		printf("\n SOCKET����");
		//return -1;
		return ApplyToClient("%s",LKJY_ERR_TX);
	}
	close(nSocket);
        printf("\n �̿���Ӧ������ [%s]",sbuf);
	/*����Ӧ����*/
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
	      printf("\n  �̿��ɷ�ʧ�ܣ�");
	    #endif
	    return ApplyToClient("%s",sbuf);
	}
}

/*-------------*
  �̿��ָ�����
 *-------------*/
long dsTaskLkqxjf(char * input)
{
	char   sjym[15+1];//������
	char   syhzh[19+1];//�û��ʺ��ʺ�
        char   syhh[20+1];
	char   spin[16+1];//�û�����
	char   szdbsm[8+1];//�ն˱�ʶ��
	char   sjyjdm[15+1];//���׾ִ���
	char   sczydh[8+1];//����Ա����
	char   slsh[40+1];//����ϵͳ��ˮ��
        double djyje;//���׽��
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
	char    sDesStr[31+1];//���׽����û��ʺ�
	char    Des_BCD[BCD_LEN+1],Des_ASCII[ASCII_LEN+1];
        char    MAC[16+1],MAC_BCD[8+1];
	char    sSendPackage[256+1];
	char    sbuf[104+1];
        char    cdllsh[41];
        long    ylsh;
	
	/*��������λͼ*/
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

	/*��8583��׼��ʽ���̿����ʺ�syhzh*/
	nyhzhll=strlen(syhzh);
	sprintf(sYHZH,"%02ld%s",nyhzhll,syhzh);
	/*��������Ĵ��ϼ���*/
	ReadLkxx(YDMY,MACKEY,PINKEY,LKJH);
	ASCIItoBCD(MACKEY,16,MACKEY_BCD);
	/*8583��ʽ��ʾ�Ľ��׽��*/
	njyje=(long)(djyje*100);
printf("\n ************************* JYJE IS [%ld]",njyje);
	/*�����̿�ϵͳ����ˮ��*/
	strcpy(sequance,"lklsh");
	nlsh=FormLsh(sequance);
        GetSysLkrq();
printf("\n #####################  DLLSH IS [%s]",cdllsh);
        RunSelect("select nlklsh from dl$lkzwmx where cdllsh=%s into %ld",
                  cdllsh,&ylsh);
printf("\n ^^^^^^^^^^^^^ ylsh is [%ld]",ylsh);
        printf("\n  &&&&&&&&&&&&&& YLSH IS [%ld]",ylsh);
	/*�γ��̿��ɷѷ��Ͱ�*/
	sprintf(sSendPackage,"%4s%s%s%s%012ld%06ld%06ld%04ld%s%8s%015s%8ld%07s%07ld",
		LKQXJF_BWLXBSF,WT,sYHZH,LKQXJF_JYLXM,njyje,nlsh,nSysSfm,nSysLkrq%10000,"000000000",ZDBSM_LK,LKJH,nSysLkrq,LKJH,ylsh);
	GetMac(sSendPackage,strlen(sSendPackage),MACKEY_BCD,MAC_BCD);
        BCDtoASCII(MAC_BCD,8,MAC);
        strcat(sSendPackage,MAC);
        /*����SOCKET����*/
	nSocket=connectTCP(LKHOST,LKSVRNAME);
	if(nSocket<0)
		return(-1);
	else
		write(nSocket,sSendPackage,strlen(sSendPackage));
	read(nSocket,sbuf,105);
	//nSql=RunSql("insert into dl$lkzwmx values()");
}


/*------------*
   �̿�����
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
	char    LKJYZDBSM[8+1];//�ն˱�ʶ��
	char    LKJH[LKJH_LEN+1];
	long    nlsh;
	long    nJysj,nJyrq;
	long    njybs;
	long    njyje;
	long    nsxf;
	long    nSocket;
	double  djyje;

	/*��������λͼ*/	
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
	/*�����̿�������ˮ�ţ����6λ�����У�*/
	strcpy(sequance,"lklsh");
	nlsh=FormLsh(sequance);
	/*�̿��������ںͽ���ʱ��*/
	GetLkJysj(&nJysj,&nJyrq);
	/*ȡ�̿���������*/
        RunSql("update dl$lkzwmx set ndzbz=9 where ndzbz=0");
	njybs=getlkjybs();
	djyje=getlkjyje();
	njyje=(long)(djyje*100);
	nsxf=0;
	/*ȡ���̿������õ���Կ*/
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
        printf("\n  MACУ���� [%s]",MAC_ASCII);

        strcat(sSendPackage,MAC_ASCII);
	printf("\n sendpackge is [%s] ",sSendPackage);
	/*����SOCKET����*/
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
	/*ȡ����ӦλԪ*/
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
    �̿�����ʱ��
 *------------------*/
void	GetLkJysj(long *nJysj,long *nJyrq)
{
	GetSysLkrq();
	*nJysj=nSysSfm;
	*nJyrq=(nSysLkrq%10000);
}


/*-----------------------------------------------------*
  ����:ReadLkxx;
  ����:ȡ���̿��ӿ��õ���Կ���̿��ֺ�;
  ����:������Կ��ַָ��;
  ����ֵ��-1��ȡʧ�ܣ�1��ȡ�ɹ�
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
  ����:inichar;
  ����:��ʼ���ַ�����;
  ����:��;
  ����ֵ:��
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
 ����:BinaryToHex;
 ����:�������ƴ�ת����ʮ�����ƵĴ�;
 ����:bin�����ƴ�;
     ת�����ʮ�����ƵĴ�;
 ע�⣺sizeΪbin���ֽ���,����ܳ���256���ֽ�.
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
    �̿����׵Ĵ�����Ϣ
 *----------------------*/
void  lkinfo()
{
	if(strcmp(lkywcode,"00")==0)
		strcpy(lkywinfo,"���׳ɹ���");
	else if(strcmp(lkywcode,"A1")==0)
		strcpy(lkywinfo,"��ѯ������ԭ��");
	else if(strcmp(lkywcode,"A4")==0)
		strcpy(lkywinfo,"û�տ���");
	else if(strcmp(lkywcode,"A5")==0)
		strcpy(lkywinfo,"����жӶң�");
	else if(strcmp(lkywcode,"A9")==0)
		strcpy(lkywinfo,"�������ڴ����У�");
	else if(strcmp(lkywcode,"B2")==0)
		strcpy(lkywinfo,"��Ч���ף�");
	else if(strcmp(lkywcode,"B3")==0)
		strcpy(lkywinfo,"��Ч��");
	else if(strcmp(lkywcode,"B4")==0)
		strcpy(lkywinfo,"��Ч���ţ�");
	else if(strcmp(lkywcode,"B5")==0)
		strcpy(lkywinfo,"�޴˿�������");
	else if(strcmp(lkywcode,"23")==0)
		strcpy(lkywinfo,"���ɽ��ܵ������ѣ�");		
	else if(strcmp(lkywcode,"25")==0)
		strcpy(lkywinfo,"δ���ҵ��ļ��ϼ�¼��");				
	else if(strcmp(lkywcode,"29")==0)
		strcpy(lkywinfo,"�ļ����²��ɹ���");
	else if(strcmp(lkywcode,"30")==0)
		strcpy(lkywinfo,"��ʽ����Ϣ���ݱ༭��");
	else if(strcmp(lkywcode,"33")==0)
		strcpy(lkywinfo,"���ڿ���");
	else if(strcmp(lkywcode,"37")==0)
		strcpy(lkywinfo,"�ܿ�����������ȫ���ܲ��ţ�û�գ���");
	else if(strcmp(lkywcode,"38")==0)
		strcpy(lkywinfo,"�����PIN�������ޣ�û�գ���");
	else if(strcmp(lkywcode,"40")==0)
		strcpy(lkywinfo,"����Ĺ����в�֧�֣�");
	else if(strcmp(lkywcode,"41")==0)
		strcpy(lkywinfo,"�ѹ�ʧ����");
	else if(strcmp(lkywcode,"43")==0)
		strcpy(lkywinfo,"���Կ���");
	else if(strcmp(lkywcode,"51")==0)
		strcpy(lkywinfo,"���㣡");
	else if(strcmp(lkywcode,"53")==0)
		strcpy(lkywinfo,"�޴��ʻ���");
	else if(strcmp(lkywcode,"54")==0)
		strcpy(lkywinfo,"���ڿ���");
	else if(strcmp(lkywcode,"55")==0)
		strcpy(lkywinfo,"�����");		/*�ظ���Ϣ*/
	else if(strcmp(lkywcode,"57")==0)
		strcpy(lkywinfo,"������ֿ��˽��еĽ��ף�");
	else if(strcmp(lkywcode,"61")==0)
		strcpy(lkywinfo,"��������޶");
	else if(strcmp(lkywcode,"64")==0)
		strcpy(lkywinfo,"ԭʼ����ȷ��");
	else if(strcmp(lkywcode,"65")==0)
		strcpy(lkywinfo,"����ȡ��������ƣ�");
	else if(strcmp(lkywcode,"66")==0)
		strcpy(lkywinfo,"�ܿ�����������ȫ���ܲ��ţ�");
	else if(strcmp(lkywcode,"75")==0)
		strcpy(lkywinfo,"�����PIN�������ޣ�");
	else if(strcmp(lkywcode,"76")==0)
		strcpy(lkywinfo,"�ѹ�ʧ����");
	else if(strcmp(lkywcode,"77")==0)
		strcpy(lkywinfo,"�ʻ��Ѷ��ᣡ");
	else if(strcmp(lkywcode,"78")==0)
		strcpy(lkywinfo,"�ʺ����廧��");
	else if(strcmp(lkywcode,"79")==0)
		strcpy(lkywinfo,"ԭ�����ѱ�ȡ���������");
	else if(strcmp(lkywcode,"80")==0)
		strcpy(lkywinfo,"�ʻ�����ʱ������");
	else if(strcmp(lkywcode,"81")==0)
		strcpy(lkywinfo,"���ʺ��ѿ��������ʻ���");
	else if(strcmp(lkywcode,"83")==0)
		strcpy(lkywinfo,"���֤���벻����");
	else if(strcmp(lkywcode,"84")==0)
		strcpy(lkywinfo,"����ӡˢ������");
	else if(strcmp(lkywcode,"85")==0)
		strcpy(lkywinfo,"���մ���Ľ���֧ȡ��");
	else if(strcmp(lkywcode,"88")==0)
		strcpy(lkywinfo,"δ������ʧ���ܽ������ʧ��");
	else if(strcmp(lkywcode,"89")==0)
		strcpy(lkywinfo,"δ���۹�ʧ���ܽ���۹�ʧ��");
	else if(strcmp(lkywcode,"90")==0)
		strcpy(lkywinfo,"�����л����ڴ���");
	else if(strcmp(lkywcode,"91")==0)
		strcpy(lkywinfo,"���������ܲ�����");
	else if(strcmp(lkywcode,"94")==0)
		strcpy(lkywinfo,"�ظ����ף�");
	else if(strcmp(lkywcode,"99")==0)
		strcpy(lkywinfo,"����ϸ�ļ���");
	else if(strcmp(lkywcode,"D0")==0)
		strcpy(lkywinfo,"�ļ�����");
	else if(strcmp(lkywcode,"F0")==0)
		strcpy(lkywinfo,"TPE��ش���1��");
	else if(strcmp(lkywcode,"F1")==0)
		strcpy(lkywinfo,"TPE��ش���2��");
	else if(strcmp(lkywcode,"F7")==0)
		strcpy(lkywinfo,"TPE����ͨѶ����");
	else if(strcmp(lkywcode,"F8")==0)
		strcpy(lkywinfo,"�˽����ڴ�TPE״̬�²���ִ�У�");
	else if(strcmp(lkywcode,"F9")==0)
		strcpy(lkywinfo,"ϵͳ����");
	else if(strcmp(lkywcode,"MR")==0)
		strcpy(lkywinfo,"TPEROUTE����");
	else if(strcmp(lkywcode,"M0")==0)
		strcpy(lkywinfo,"��Ϣ�ձ���С�ڵ���Ӫҵ�գ�");
	else if(strcmp(lkywcode,"Z0")==0)
		strcpy(lkywinfo,"ȫ�����ر�����ϵͳʧ�ܣ�");
	else if(strcmp(lkywcode,"Z1")==0)
		strcpy(lkywinfo,"���ڴ�");
	else if(strcmp(lkywcode,"Z2")==0)
		strcpy(lkywinfo,"MAC����ʧ�ܣ�");
	else if(strcmp(lkywcode,"Z3")==0)
		strcpy(lkywinfo,"PIN��ʽ����");
	else if(strcmp(lkywcode,"Z4")==0)
		strcpy(lkywinfo,"������������ϵͳʧ�ܣ�");
	else if(strcmp(lkywcode,"Z5")==0)
		strcpy(lkywinfo,"��Կ��֤ʧ�ܣ�");
	else if(strcmp(lkywcode,"Z6")==0)
		strcpy(lkywinfo,"�������������죡");
	else if(strcmp(lkywcode,"Z7")==0)
		strcpy(lkywinfo,"����δ������Ӧ�ó��ֵĴ�����֣�");
	else if(strcmp(lkywcode,"Z8")==0)
		strcpy(lkywinfo,"�û�����δ�������޷�ת����");
	else if(strcmp(lkywcode,"Z9")==0)
		strcpy(lkywinfo,"����������δ�������޷�ת����");
	else if(strcmp(lkywcode,"ZA")==0)
		strcpy(lkywinfo,"ȫ������δ�������޷�ת����");
	else if(strcmp(lkywcode,"ZB")==0)
		strcpy(lkywinfo,"ȫ�������ղ������������ص�Ӧ��");
	else if(strcmp(lkywcode,"ZD")==0)
		strcpy(lkywinfo,"�����������ղ����������ĵ�Ӧ��");
	else if(strcmp(lkywcode,"ZG")==0)
		strcpy(lkywinfo,"ԭʼ���ײ��ɹ���");
	else if(strcmp(lkywcode,"T0")==0)
		strcpy(lkywinfo,"û�����������ϸ��¼��");
	else if(strcmp(lkywcode,"ZZ")==0)
		strcpy(lkywinfo,"����δ֪����");
	else strcpy(lkywinfo,"ϵͳδ��ʶ�Ĵ���");
}

/*-----------------------*
  ����:FormLs;
  ����:��������������;
  ����:������ sSequence;
  ����ֵ:cLsh;
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
  ����:getlkjybs;
  ����:�����̿����׵ı���;
  ����:��;
  ����ֵ:���ױ���;
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
  ����:getlkjyje;
  ����:�����̿����׽��;
  ����:��;
  ����ֵ:���׽��;
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

