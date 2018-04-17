#ifndef T_MOBILE_H
#define T_MOBILE_H

typedef struct chargedetail{                     //�ɷ���ϸ
                            long   nYear;          //���
                            long   nMonth;         //�·�
                            double dFee;         //����Ӧ��
                            double dBchf;         //Ӧ�տ�
                            double dZnj;          //���ɽ�
                            double dYhk;          //�Żݿ�
                            double dYckhb;        //Ԥ����
                            double dXjk;          //�½ɿ�
                            double dYzf;          //�����;
                            double dTff;          //�ط��ѣ�
                            double dBwf;          //�����ѣ�
                            double dMyf;          //���ηѣ�
                            double dCtf;          //��;�ѣ�
                            double dFjf;          //���ӷ�;
                            double dQtf;          //�����ѣ�
                            double dXxf;          //��Ϣ�ѣ�
                            double dPzf;          //Ƶռ�ѣ�
                            double dNhf;          //ũ���ѣ�
                            double dBsf;          //���շѣ�
                            }CHARGEDETAIL;

typedef struct mobileanswer{                 //�ƶ�Ӧ���û���ѯ
                             char sReturn[5];   //������
                             char sBargain[16]; //��ͬ��
                             char sRegion[3];   //��������
                             char sCountry[3];  //���ش���
                             char sCompany[60]; //��λ����
                             char sUserName[13];//�û�����
                             long nWay;         //���ʽ
                             char sStatus[3];      //����״̬
                             double dwczhf;     //δ���ʻ���
                             double dxyd;       //������
                             double dyck;       //Ԥ���
                             double dbsyzf;     //��������� 
                             char   smljqs[5];  //ģ�������
                             char   sszjqs[5];  //���ֻ�����
                             long   nCount;     //������ϸ��¼��[���ɴ��ж��Ƿ�����������]
                           }MOBILEANSWER;      

long dsMobilePayQuery(char * inbuf,RECORD * Client);
static long MobileInsertYwmx(char * slsh,char * sCzydh,char * sPhone,char * sJym,char * sDetail,RECORD * Client);
long dsTaskMobileSIngIn(char * sJym);
long dsTaskMobileSIngOut(char * sJym);
long dsTaskMobileCompletePrint(char * sJym,char * sphone,char * nstarttime,char * endtime);
long ASCIItoBCD(char * sASC,int nLen,char * sDest);
long BCDtoASCII(char * sBCD,int nLen,char * sDest);
void preDESconvert(char * sKey,char * sStream,char * sDest);
static int  dsMobileSendAndRecv(int nsock,char * sinbuf,long nopt,char * sSendAdd,char * soutbuf);
static void MobileFormatTransfer(char * sAnswer,long nTaskNo);
static void GetMobileErrorInfo(char * sError,char * sInfo);
static long MobileInsertCompleteYwmx(char * slsh,char * sPhone,char * sJym,char * sDetail);
long dsTaskMobilePayConfirm(char * inbuf,char * outbuf);
long dsMobileCheckAccount(char * inbuf,char * outbuf);
static long dsMobileFormAccountFile(char * inbuf,char * sFileName,long * pnSize);
void dsMobileSendPayDetail(int nSocket,char * sAnswer,long nFileSize);
long dsMobileRecallConfirm(char * inbuf,long nxh,char * outbuf);
long  dsMobileParsePrint(char * filename,char * data);
#endif


