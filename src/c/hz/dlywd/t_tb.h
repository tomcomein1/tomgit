/******************************************************************************
**
** �ļ�:                     t_tb.h
** Ŀ��:                     �绰���н�������
**
** ��������:                 2001.8
** ԭʼ�����Ա:             ����
** �޸���ʷ
** ����        �޸���Ա  ԭ��
**
*******************************************************************************/
#include "dlywd.h"


#define  BCD_LEN	    8
#define  ASCII_LEN	    16

#define  LKJY_SUCCESS   "00"       //�̿����׳ɹ�
#define  LKJY_ERR_TX    "L1"       //��̨ͨѶʧ��
#define  LKJY_ERR_DB    "L2"       //DLYWD�������ֿ�ʧ��
#define  LKJY_ERR_LKJY  "L3"       //�̿����״���
#define  LKJY_ERRINFO_TX   "�͵绰����ǰ�û�SOCKET���Ӵ���!!!"


//�ն˱�ʶ��
#define  ZDBSM_LK           "00000000"
/*�̿�ǩ��*/
#define  LKQD_BWLXBSF_FD    "0820"
#define  LKQD_BWLXBSF_DF    "0830"
#define  LKQD_JYLXM         "091001"
/*�̿�ǩ��*/
#define  LKQT_BWLXBSF_FD    "0820"
#define  LKQT_BWLXBSF_DF    "0830"
#define  LKQT_JYLXM         "091002"
/*�̿�������Կ*/
#define  LKJHMY_BWLXBSF_FD  "0800"
#define  LKJHMY_BWLXBSF_DF  "0810"
#define  LKJHMY_JYLXM       "091003"
/*�̿��ɷ�*/
#define  LKJF_BWLXBSF_FD    "0200"
#define  LKJF_JYLXM         "056031"

/*�̿�ȡ���ɷ�*/
#define  LKQXJF_BWLXBSF	    "0420"
#define  LKQXJF_JYLXM       "056931"
/*�̿��ʻ�����ѯ*/
#define  LKZHYECX_BWLXBSF   "0200"//�������ͱ�ʶ��
#define  LKZHYECX_JYLXM	    "053070"//����������
/*�̿����ʶ���*/
#define  LKDZ_BWLXBSF       "0520"//�������ͱ�ʶ��
#define  LKDZ_JYLXM         "091004"//����������
/*�̿���ϸ����*/
#define  LKMXDZ_BWLXBSF	    "0522"//�������ͱ�ʶ��
#define  LKMXDZ_JYLXM       "091011"//����������


extern long nSysDebug;
extern long nSysLkrq;
extern long nSysSfm;
extern char sSysJh[9+1];
extern char sSysErrInfo[81];
static char sequance[5+1];


char lkywcode[3];
char lkywinfo[129];

//�绰���л�������
typedef struct lkywcs
{
    char sLkjh[21];
    char sYdmy[16+1];
    char sMacKey[16+1];
    char sPinKey[16+1];
}LKYWCS;

//�绰���нɷ�
typedef struct lkjf
{
    char   sYyzh[19+1];//�����ʺ�
    char   sYymm[16+1];//��������
    char   sYhh[20+1]; //�û���
    long   nDsfzlh;    //���ո������
    double dJyje;      //���׽��
}LKJF;

//�̿�ȡ���ɷ�
typedef struct lkqxjf
{
	char    sJyls[7+1];//������ˮ
	char    sSjy[22+1];//����Դ
	char    sYyzh[19+1];//�����ʺ�
	char    sJyjh[7+1];//���׾ֺ�
	double  dJyje;//���׽��
	double  dSxf;//������
	long    lJyrq;//��������
	long    lJysj;//����ʱ��
}LKQXJF;


long dsTask185jhmy();
long daTaskDl185Jyjecx();
long lGetLkxx(char *YDMY,char *MACKEY,char *PINKEY,char *LKJH);
long FormLsh(char *sSequence);
long getlkjybs();
long dsTaskLkjhmy1();
long dsTaskLkjhmy2();
long dsTaskLkjf(char *input);
void BinaryToHex(char *bin,char *hexASC,long size);
void GetJysj(long *nJysj,long *nJyrq);
void inichar(char *array,long n);
void inputydmy();
void lkinfo();
double getlkjyje();
