#ifndef TELECOM_H
#define TELECOM_H

#define YADX_PKG_LEN sizeof(yadx_Pkg)       //���ݳ�

typedef   struct   {
    long    lsh;			//����������ˮ�ţ��ش�ʱΪ���ĳ��ȣ�
    char    zh1[22];			//�����ʺţ�(�ش�ʱΪ�շѹ���)
    char    zh2[22];			//�����ʺţ�(�ش�ʱΪ�շ�����YYYYMMDD)
    char    mm1[16];			//���룱
    char    mm2[16];			//���룲
    char    sfzh[19];			//���֤��
    char    dwbh[6];			//���յ�λ��λ���
    char    sfzsy[13];			//�շ����������绰���룩
    char    sffsy[13];			//�շѸ���������ͬ���룩
    char    yhm[17];			//�û���������ʱΪ��������YYYYMMDD��
    double  qfhj;			//Ƿ�Ѻϼ�
    double  dsfhj;			//���շѺϼ�
    double  znj;			//���ɽ�ϼ�
    char    bz[2];			//��־
    long    jyfs;			//���׷�ʽ
    char    dsjg[10];			//���ջ���
    long    sfrq;			//�շ�����
    long    sfsj;			//�շ�ʱ��
    long    czy;			//����Ա
    char    xym[3];			//��Ӧ��
    char    xymms[41];			//��Ӧ������
    double  zje;			//�ܽ��
} yadx_Pkg;

#endif
