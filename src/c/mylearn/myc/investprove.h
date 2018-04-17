/*֤���ϴ���Ʒ����Ϣ*/
struct GroupProdCodeInfo {
	char prct_cde[19+1]; //	��Ʒ��
	char bnft_acctno[28+1]; //	�ʽ��ʺ�
	struct GroupProdInfo *next;
};

/*��Ʋ�Ʒ֤��(ʱ��)�ϴ��ṹ*/
struct InvestProveReq {
	char cust_name[40+1];  //	�ͻ���
	char cert_typ [10+1];  //	֤������
	char cert_id  [18+1];  //	֤����
	char strt_dt  [8+1];   //	��������
	struct GroupProdCodeInfo *prodcode_info; //��Ʒ��Ϣ
	int total_qut;
	char doc_no[16+1]; //ƾ֤��ʼ����
	int doc_num; //	ƾ֤����
};

struct GroupProdInfo{
	char prct_name[40+1]; //	��Ʒ����	40	
	char curr_cod[10+1];  //	����	10	CodeString
	double hold_value;	//  ���зݶ�	15��4	N
	double fair_value;  //	�ʲ���ֵ	15,2	N
	struct GroupProdInfo *next;
};

/*��Ʋ�Ʒ֤��(ʱ��)�´��ṹ*/
struct InvestProveResp {
	char cust_name[40+1]; //	�ͻ�����	40	C
	char cert_typ [10+1]; //	֤������	10	CodeString
	char cert_id  [18+1]; //	֤������	18	C
	char strt_dt  [8+1];  //	��������	8	C
	char strt_time[19+1]; //	����ʱ��	19	C
	struct GroupProdInfo *prod_info;
	int total_qut;  //	��������	2	N
	char doc_no[16+1]; //	ƾ֤��ʼ����	16	C
	int doc_num; //	ƾ֤����	2	N
	char prt_flag[10+1]; //	��ӡ��־	10	C
};

/*��Ʋ�Ʒ֤��(ʱ��)�ϴ��ṹ*/
struct InvestProveSectReq {
	char func_no[1+1]; /*	���ܺ�	1	C	1-����
	2-����
	3-����
	4-���� */
	char crc_cod[15+1]; //	����֤����	15	C	���ܺ�2��3��4������
	char cust_name[40+1]; //	�ͻ�����	40	C	
	char cert_typ [10+1]; //	֤������	10	CodeString	
	char cert_id [18+1];  //	֤������	18	C	
	char strt_dt [8+1]; //	��ʼ����	8	C	
	char end_dt	[8+1]; //��ֹ����	8	C	
	struct GroupProdCodeInfo *acct_info;
	int total_qut; //	��������	2	N	
	char doc_no[16+1]; //	ƾ֤��ʼ����	16	C	
	int doc_num; //	ƾ֤����	2	N	
};

struct GroupSectProdInfo{
	char bnft_acctno[28+1]; //	�ʽ�����˻�	28	C	
	char curr_cod[10+1]; //	����	10	CodeString	
	double fair_value;  //	��Ʒ��ֵ	15,2	N	��δ��������
	struct GroupSectProdInfo *next;
};

/*��Ʋ�Ʒ֤��(ʱ��)�´��ṹ*/
struct InvestProveSectResp {
	char crc_cod[15+1]; //	����֤����	15	C	
	char cust_name[40+1]; //	�ͻ�����	40	C	
	char cert_typ[10+1]; //	֤������	10	CodeString	
	char cert_id[18+1]; //	֤������	18	C	
	char strt_dt[8+1]; //	��ʼ����	8	C	
	char end_dt[8+1]; //	��ֹ����	8	C	
	double amt; //	��Ʒ�ܶ�	15,2	N	
	struct GroupSectProdInfo *prod_info;
	int total_qut;  //	��������	2	N	
	char doc_no[16+1];	//ƾ֤��ʼ����	16	C	
	int doc_num; //	ƾ֤����	2	N	
	char prt_flag[10+1]; //	��ӡ��־	10	C	�����ÿλ����Ϊ1�������ѡ���ӡ
};


/*��Ʋ�Ʒ����֤���ϴ��ṹ*/
struct InvestTransReq {
	char cust_name[40+1]; //	�ͻ�����	40	C
	char cert_typ[10+1]; //	֤������	10	CodeString
	char cert_id[18+1]; //	֤������	18	C
	char prct_cde[19+1]; //	��Ʒ���	19	C
	char bnft_acctno[28+1]; //	�ʽ�����˻�	28	C
	char strt_dt[8+1]; //	��ʼ����	8	C
	char end_dt[8+1]; //	��ֹ����	8	C
	int total_qut;  //	��������	2	N
	char doc_no[16+1]; //	ƾ֤��ʼ����	16	C
	int doc_num;   //	ƾ֤����	2	N
};

/*��������Ϣ*/
struct GroupTransInfo{
	char prct_name[40+1]; //	��Ʒ����	40	C	
	char curr_cod[10+1]; //	����	10	CodeString	
	char txdate[8+1];  //	��������	8	C	
	char trans_type[1+1]; /*	��������	1	C	1  �Ϲ�
	2  �깺
	3  ���
	4  ��ֹ
	5  ���淢��
	6  �ǽ��׹���������
	7  IPOתͶת��
	8  IPO����
	9  ������Ͷ
	A  IPO�ۻ�
	B  IPOתͶת��
	F  �ǽ��׹���-���� */
	double trans_amt; //	���׽��	15,2	N	����ϼ�
};

/*��Ʋ�Ʒ����֤���´��ṹ*/
struct InvestTransResp{
	char cust_name[40+1]; //	�ͻ�����	40	C	
	char cert_typ[10+1]; //	֤������	10	CodeString	
	char cert_id[18+1]; //	֤������	18	C	
	char strt_dt[8+1]; //	��ʼ����	8	C	
	char end_dt[8+1]; //	��ֹ����	8	C	
	struct GroupTransInfo *trans_info;
	int total_qut; //	��������	2	N	
	char doc_no[16+1]; //	ƾ֤��ʼ����	16	C	
	int doc_num;  // ƾ֤����	2	N	
	char prt_flag[10+1]; //	��ӡ��־	10	C	�����ÿλ����Ϊ1�������ѡ���ӡ
};


