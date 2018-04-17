#define TASK_MOBILE_SIGN_IN                   400     //ǩ������
#define TASK_MOBILE_SIGN_OUT                  401     //ǩ�˲���
#define TASK_MOBILE_PAY_QUERY                 402     //���Ѳ�ѯ����
#define TASK_MOBILE_PAY_CHECK                 403     //������֤����
#define TASK_MOBILE_PAY_CONFIRM               404     //�ֽ𽻷�ȷ�ϲ���
#define TASK_MOBILE_PAY_COMPARE               405     //�����ʽ���
#define TASK_MOBILE_RECALL                    406     //ɾ�ʽ���
#define TASK_MOBILE_RECALL_CONFIRM            407     //ɾ�ʽ���ȷ��
#define TASK_MOBILE_COMPLETE_PRINT            408     //��ȫ������ѯ
#define TASK_MOBILE_ALREADY_PAID              409     //�ѽ��Ѳ�ѯ
///////////////////////////////////////////////////////////////////
#define TASK_POWER_SIGN_IN                   600     //ǩ������
#define TASK_POWER_SIGN_OUT                  601     //ǩ�˲���
#define TASK_POWER_PAY_QUERY                 602     //���Ѳ�ѯ����
#define TASK_POWER_PAY_CHECK                 603     //������֤����
#define TASK_POWER_PAY_CONFIRM               604     //�ֽ𽻷�ȷ�ϲ���
#define TASK_POWER_PAY_COMPARE               605     //�����ʽ���
#define TASK_POWER_RECALL                    606     //ɾ�ʽ���
#define TASK_POWER_RECALL_CONFIRM            607     //ɾ�ʽ���ȷ��
#define TASK_POWER_PAY_BUTTON                608     //��۽���
#define TASK_POWER_REPRINT                   609     //����Ʊ��ѯ


#define CODESIZE                         6
#define ORDERSIZE                        9
#define MAXDATASIZE                    216
#define TOTAL_PACKAGE_SIZE           20000
#define MAX_PROCEDURE                   20    //����������
#define MAX_MOB_TIME                    40    //���ƶ�ͨ�ŵ����ʱ��
#define MAX_DLYW_TIME                   20    //��DLYWD����ͨ�ŵ����ʱ��
#define MAX_DELAY_TIME                  90    //�ȴ��������ȴ�ʱ��

typedef struct{
              unsigned char  morepkt;    //���и���İ���1����  0��û��
              unsigned char  pktype;     //������          1���ļ���  0�����ݰ�
              unsigned char  unitend;    //�������͵İ��Ƿ���� 1����� 0��û��
              unsigned char  save;       //����
              unsigned short sequence;   //�����
              unsigned short length;     //����������
              unsigned long  msgtype;    //����ؽ��̺�
              char code[CODESIZE];       //������
              char d_add[ORDERSIZE];     //���յص�ַ��ʶ
              char l_add[ORDERSIZE];     //����ص�ַ��ʶ
              char datatrans[MAXDATASIZE];   //������
             }MOBILEPACKAGE;

typedef struct{
              unsigned long processid;              //����������
              int socket;                          //�׽��ֺ�
              long starttime;                      //��ʼʱ��
              }MOBILEMSG;

void mobmain();
static long dsRecvDlywAndSendMob(int dlywsock);
static long dsRecvMobAndSendDlyw(int mobsock);
static long getptablespace();
void GetNowTime();
static void CleanupSpace();

