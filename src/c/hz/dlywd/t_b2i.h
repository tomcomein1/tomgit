#ifndef T_B2I_H
#define T_B2I_H

#define CODESIZ	                        6     //�����볤��

#define CTL_LEN  sizeof(struct Ctl_Msg)       //���Ƴ�
#define DATA_LEN sizeof(tongXin_Struct)       //���ݳ�
#define DZDATA_LEN sizeof(duiZhang_Struct)    

/* �����ַ��ṹ��ʽ */
 
union Ctl_Char {
		unsigned  char	ctl;
		struct {
			unsigned morepkt:1;	/* �Ƿ��к�������*/
						/*  = 1, ��
						 *  = 0, û�� */
			unsigned pktype:1;	/*  = 0:��������
						    = 1:�ط��� */
			unsigned unitend:1;	/* ����λ,ĳһ����� */ 
			unsigned reserver:5;	/* ���� */
			} flags;
		};

/* ������Ϣ��ʽ�ṹ */

struct Ctl_Msg {
		char MsgLength [ 7 ] ;     /*  ��Ϣ���� */
		union 	   Ctl_Char CtlChar ;    /*  �����ַ�  */
		char TransCode [ CODESIZ + 1 ] ; /* ������ */
					/* 500101 --- ��ѯ�����ݱ����ţ�*/
					/* 500201 --- �ɷ��ύ*/
					/* 500301 --- �����վ�*/
					/* 500401 --- ���˴���*/
		char status [ 5 ] ;   /* ״̬��Ϣ */
				/* 0000 --- ��ȷ���� */
				/* 0001 --- ������Ƿ� */
				/* 900X --- ���������� */
				/*    |     �����ر��� */
				/*    |->X=tongXin_Struct�ṹ�е�chaXunJieGuo */
		};


/******************************************
* ������Ϣ: ���չ�˾�����е�ͨ����Ϣһ��  *
******************************************/
typedef struct
{
	char	jiaoYiLeiXing[3] ;	/*��������*/
					/*A1--���ݱ����Ų�ѯ*/
					/*B1--�����ύ*/
					/*C1--�����վ�*/
					/*D1--����*/
					/*E1--����*/
	char	yinHangDaiMa[5] ;	/*���д���*/
					/*8381--����*/
					/*8382--ũ��*/
					/*8383--����*/
					/*8384--����*/
                                        /*8385--����*/
	char	wangDianDaiMa[10] ;	/*�������,�������ṩ*/
	char	baoDanHao[23] ;		/*������*/
	char	yingShouFeiRiQi[11] ;	/*Ӧ�շ�����*/
	char	baoXianMingChen[41] ;	/*��������*/
	char	touBaoRen[65] ;		/*Ͷ����(����)*/
	char	jiaoFeiFangShi ;	/*�ɷѷ�ʽ*/
					/*M--�½�   Q--����*/
					/*H--���꽻 Y--�꽻*/
	int	jiaoFeiNianQi ;		/*�ɷ�����*/
	int	jiaoFeiCiShu ;		/*�ɷѴ���*/
	double	yuJiaoBaoFei ;		/*��ɱ���*/
	double	baoFei ;		/*����*/
	double	yuJiaoFeiYuE ;		/*��ɷ����*/
	double	liXi ;			/*��Ϣ*/
	char	shengXiaoRiQi[11] ;	/*��Ч����*/
	int	baoDanNianDu ;		/*����(����)���*/
	char	yeWuYuanDaiMa[6] ;	/*ҵ��Ա����*/
	char	yeWuYuan[21] ;		/*ҵ��Ա(����)*/
	char	caoZuoYuanDaiMa[5] ;	/*����Ա����*/
	char	caoZuoYuan[9] ;		/*����Ա(����)*/
	char	shouFeiShiJian[20] ;	/*�շ�ʱ��*/
	char	chongZhenShiJian[20];	/*����ʱ��*/
	char	shouFeiFangShi ;	/*�շѷ�ʽ*/
	char	zhiPiaoHao[21];		/*֧Ʊ��*/
	char	shouJuYinShuaHao[13] ;	/*�վ�ӡˢ��*/
	char	chaXunJieGuo ;	/*״̬					    */
				/*��һ�����չ�˾������:			    */
				/*	0--��Ӧ�ռ�¼�����ص�һ��	    */
				/*	1--�Ƿ�������			    */
				/*	2--����������			    */
				/*	3--����ʧЧ������		    */
				/*	4--��Ӧ�ռ�¼,��Ҫ���н�������	    */
				/*	5--����δ�ɷ� 			    */
				/*	6--�����ղ���Ԥ�������ʾ���պ�	    */
				/*���������е����չ�˾			    */
				/*�շѣ�0--�����շ� 1--����Ԥ�� 2--�շ��쳣 */
				/*������0--�����շѳ���  1--Ԥ�����ѳ���    */
	char   bank_no [ 11 ] ;	/*������ˮ��*/
} tongXin_Struct ;


/**********
*���˽ṹ *
***********/
typedef struct
{
	char	baoDanHao[23] ;		/*������*/
	char	yinHangDaiMa[5] ;	/*���д���*/
	char	wangDianDaiMa[10] ;	/*�������*/
	char	jiaoYiLeiXing[3] ;	/*��������*/
	char	jiaoYiShiJian[20] ;	/*����ʱ��*/
	char	caoZuoYuan[5] ;		/*����Ա*/
	double	shouFeiJinE ;		/*�շѽ��*/
	char	shouJuYinShuaHao[13] ;	/*�վ�ӡˢ��*/
	char    bank_no [ 11 ] ;	/*������ˮ��*/
} duiZhang_Struct ;


//static long dsBxSendAndRecv(int nFlag);
long dsTaskBxRecallConfirm(char *inbuf,long nxh,char *outbuf);
static void getBxErrorInfo(struct Ctl_Msg rc,char *sErrorInfo); 
long dsBxRequestRePrint(char *sLsh);
long dsTaskBxCheckAccount(char * inbuf,char * outbuf);
static long dsBxSendAndRecv(struct Ctl_Msg *rc,tongXin_Struct *rp);
void getFlData(long nData,long *na,long *nb,long *nc);
static long dsBxDzSendAndRecv(char *sbuffer);
int iIbmOrScoReturn(int num);
double dIbmOrScoReturn(double num);

#endif
