/*===========================================================
               global.c(ȫ�ֱ�������ģ��)

*==========================================================*/

#include"dlpublic.h"

#ifdef DLYWLIB
extern char zjusername[20];   
extern char zjpassword[20];
#endif

long nMakeDate=20021225;  //��������
double dDlywVer=4.00;      //����ҵ��汾��
double dHelpVer=2.0;      //�����ĵ��汾��
char sSysIp[16];       //����IP��ַ
char sSysJh[10];       //�����־ֺ�
char sSysJsdh[10];     //����Ա��������
char sSysJsmc[JM_JM_LEN+1];//��������֧�־���
char sUserName[101];   //�û�����
char sPassWord[41];    //�û�����
long nSysLkrq;  //¼������
long nSysSfm;   //ϵͳʱ��  Сʱ ���� ��
long nSysAutoClose=0; //�Ƿ��Զ��ر����ݿ�   dl$dlywcs��10�Ų���
char sSysCzydh[CZYB_CZYDH_LEN+1];//����Ա����
char sSysCzymc[CZYB_CZYMC_LEN+1];//����Ա����
char sSysJm[JM_JM_LEN+1];/*��ҵ�ؾ־���  cs2*/
long nSysCzyqx;//����ԱȨ��
char sSysCzyqx[21];//����ԱȨ�޺��ֽ���
long nSysCzylb;//����Ա���0������Ĭ�ϣ���1���ۺ���  dlyw.ini:czylb=1;
char sSysDsHost[41]; // ���ݿ��ػ���������
char sSysDsService[41]; // ���ݿ��ػ����̶˿ں�
char sSysManager[7];//����ҵ��������û���ȱʡΪDLZ
char sSysPassword[41];//����ҵ��������û��Ŀ��ȱʡΪDDDD
char sSysDlywHost[41]; // ����ҵ���ػ���������
char sSysDlywService[41]; // �ػ����̶˿ں�
long nSysPcType;          //���0������ǰ̨��1��ͨѶ��
extern long nSysMode;
#ifdef UNIX
extern long nTermType;
#endif


