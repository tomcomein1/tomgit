#ifndef POLICY_H
#define POLICY_H

#define CODESIZ	                        6     //�����볤��

#define CTL_PKG_LEN  sizeof(struct Ctl_Msg)       //���Ƴ�
#define DAT_PKG_LEN sizeof(data_Pkg)       //���ݳ�
#define ACC_PKG_LEN sizeof(account_Pkg)    //���ʰ�

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
					/* 500501 --- ����/
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
typedef   struct   {
	char   trans_code[7];		// ������
	char   bank_code[5];		//���д���
	char  agent_region_code[11]; // ������
	char  agent_sub_code[11];    // �������
	char  b2i_policyno[23];     //���ձ�����
	char  b2i_pol_name[41];     //��������
	char  b2i_insured_name[21]; // Ͷ��������
	char   b2i_mg_branch[7];    //�������  
	char   b2i_print_type[3];   //��ӡ��־  
	//��GT��JT�����ӡ�ڷ�Ʊ�ϣ�
	char   b2i_pay_itrvl[2];    //�ɷѷ�ʽ
	char   b2i_pay_dur[5];      // �ɷ��ڼ�
	char  b2i_pay_amnt[15];     // �ɷ��ܽ��
	char   b2i_pay_num[3];      // �ɷ���ϸ����
	//�������������ڣ�
	char b2i_pay_text[600];     // �ɷ���ϸ��������Ŀ��
	char   b2i_print_num[3];    //��Ʊ����
	char  b2i_bclk_code[12];    // ҵ��Ա����
	char  b2i_bclk_name[21];    // ҵ��Ա����
	char   b2i_op_code[8];      // ����Ա����
	char   b2i_op_name[9];      // ����Ա����
	char  b2i_op_time[20];      // ҵ��ʱ��
	char  bank_no[16];          //������ˮ

}data_Pkg;

typedef   struct   {
   char   trans_code[7];  	// ������
   char   bank_code[5];   	//     ���д���
   char   agent_region_code[11]; // ������
   char   agent_sub_code[11];   // �������
   char   b2i_policyno[23];     //  ������
   char   b2i_mg_branch[7];    //  �������
   char   b2i_pay_amnt[15];     //  �ɷѽ��
   char   b2i_pay_text[600];    //  �ɷ���ϸ
   char   b2i_op_time[20];      //  ҵ��ʱ��
   char   bank_no[16];          //  ������ˮ
   char   b2i_op_code[5];       //  ����
}account_Pkg;

#endif
