
#define TOTAL_PACKAGE_SIZE 20000


#define TASK_PAY_QUERY            100         //�ɷѲ�ѯ����
#define TASK_PAY                  101         //�ɷ�����
#define TASK_PAY_CONFIRM          102         //�ɷѳ�������    
#define TASK_RECALL               103         //��������
#define TASK_RECALL_CONFIRM       104         //����ȷ������
#define TASK_PAY_CHECKACCOUNT     105         //ǰ̨������������
#define TASK_CREATE_DS_JYM        106         //�������ս�����
#define TASK_CREATE_SP_JYM        107         //�������۽�����
#define TASK_CREATE_DB_JYM        108         //�������콻����
#define TASK_DELETE_JYM           109         //ɾ��������
#define TASK_OPEN_JYM             110         //�����뿪��
#define TASK_CZYJK_CONFIRM        111         //����Ա�ɿ�
#define TASK_DAIBANYW             112         //����ҵ������
#define TASK_REQUEST_PRINT        120         //�����ӡ
#define TASK_CHECK_OPEN           121         //���Զ���������

#define TASK_GET_YD_JYM           130         //��ȡ������ؽ�����
#define TASK_CREATE_YD_JYM        131      	  //������ؽ�����
#define TASK_GET_YD_JYM_MX        132         //��ȡ��ؽ�������ϸ

#define TASK_0 200 // ������
#define CALL_0 400

//////////!!!����:�벻Ҫ�ı���������ŵ�ֵ!!!!///////////
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
//////////!!!�벻Ҫ�ı�400-408������ŵ�ֵ!!!///////////
#define TASK_BPUSER_TEST_CALL                 410

#define TASK_MOBILE_OPERATION          500  //�ֻ�������������
#define TASK_MOBILE_PACKAGE            501  //�ֻ����հ�����
#define TASK_BPUSER_PACKAGE            502
//////////!!!����:�벻Ҫ�ı���������ŵ�ֵ!!!!///////////

#define TASK_COMMIT_PURCHASE           510  //����ȷ�����     ����
#define TASK_COMMIT_DISTRIBUTE         511  //�ֻ�ȷ�����     �ֻ�
#define TASK_ACCEPT_DISTRIBUTE         512  //���շֻ�         �ӻ�
#define TASK_COMMIT_SENDBACK           513  //��ί�з��˻�ȷ�� �˻�
#define TASK_SELL_GOODS                514  //�������
#define TASK_RECALL_GOODS              515  //�ͻ��˻�
#define TASK_WASTE_GOODS               516  //����
#define TASK_RETURN_GOODS              517  //��ί�з��˻�

//////////!!!����:�벻Ҫ�ı���������ŵ�ֵ!!!!///////////
#define TASK_POWER_SIGN_IN                   600     //ǩ������
#define TASK_POWER_SIGN_OUT                  601     //ǩ�˲���
#define TASK_POWER_PAY_QUERY                 602     //���Ѳ�ѯ����
#define TASK_POWER_PAY_CHECK                 603     //������֤����
#define TASK_POWER_PAY_CONFIRM               604     //�ֽ𽻷�ȷ�ϲ���
#define TASK_POWER_PAY_COMPARE               605     //�����ʽ���
#define TASK_POWER_RECALL                    606     //ɾ�ʽ���
#define TASK_POWER_RECALL_CONFIRM            607     //ɾ�ʽ���ȷ��
#define TASK_POWER_PAY_BUTTON                608     //��ȫ������ѯ
#define TASK_POWER_REPRINT                   609     //�ѽ��Ѳ�ѯ
//////////!!!�벻Ҫ�ı�600-609������ŵ�ֵ!!!///////////

//1000-1999 Ϊί�е������
#define TASK_WT_CX_GYSFHJ     1000
#define TASK_WT_CX_DYSFJD     1001 	
#define TASK_WT_CX_TRQYMX     1002 
#define TASK_GET_SPBH         1003
#define TASK_GET_FILE_FROM_REMOTE  1005
#define TASK_SEND_FILE_TO_REMOTE   1006
#define TASK_SYSTEM_BACKUP         1007
#define TASK_SYSTEM_RESTORE        1008
#define TASK_LK_QD                 1009

#define TASK_GET_VALUE_FROM_FILE   1010
#define TASK_PUT_VALUE_TO_FILE     1011
#define TASK_CLEANUP_HISTORY       1012
#define TASK_GET_ALL_JSDH          1013 

#define TASK_GET_ALL_CITY          			1014
#define TASK_QUERY_AIRLINE         			1015
#define TASK_QUERY_AIR_PRICE       			1016
#define TASK_AIR_PURCHASE          			1017
#define TASK_AIR_CONFIRM           			1018
#define TASK_AIR_RECALL            			1019
#define TASK_AIR_RETURN_TICKET     			1020
#define TASK_QUERY_AIR_ROUTE       			1021
#define TASK_AIR_SIGN_IN           			2021
#define TASK_AIR_SIGN_OUT          			2022
#define TASK_OUT_AIR_TICKET        			2023
#define TASK_QUERY_AIR_BERTH       			2024
#define TASK_AIR_ONLINE            			2025
#define TASK_AIR_CHECK            			2026
#define TASK_LOAD_CHECK_TICKET	   			2027
#define TASK_SAVE_CHECK_RESULT 	   			2028
#define TASK_CHECK_DES_AIRTICKET   			2029
#define TASK_GET_DES_AIRTICKET     			2030
#define TASK_CHECK_RETURN_DES_AIRTICKET  	2031
#define TASK_QUERY_TICKET	   	   			2032
#define TASK_AIR_PAY    	   	   			2033
#define TASK_LOAD_PAY_TICKET	   			2034
#define TASK_SAVE_TICKET_PAY_RESULT			2035
#define TASK_LOAD_DELIVER_TICKET   			2036
#define TASK_SAVE_TICKET_DELIVER_RESULT 	2037
#define TASK_LOAD_RETURN_TICKET_BEG		 	2038
#define TASK_SAVE_RETURN_TICKET_BEG		 	2039
#define TASK_SAVE_CANCEL_RETURN_TICKET_BEG	2040
#define TASK_LOAD_DO_RETURN_TICKET_BEG		2041
#define TASK_SAVE_DO_RETURN_TICKET_RESULT 	2042
#define TASK_SAVE_REDO_RETURN_TICKET_RESULT	2043

#define TASK_PUBLIC_QUERY					2044

#define TASK_LOAD_WATCH_TICKET		    	2045

//2000-2999 Ϊ��������bk�������

//3000-3999 Ϊ��������dl�������
#define TASK_DL185_JHMY            3009   //jiaohuanmiyao
#define TASK_DL185_CXZZ            3010   //jiaoyizongzhangchaxun
#define TASK_LKQD                  3011   //�̿�ǩ��
#define TASK_LKQT                  3012   //�̿�ǩ��
#define TASK_LKJF                  3013   //�̿��ɷ�
#define TASK_LKJHMY                3014   //�̿�������Կ
#define TASK_LKJHMY1               3015
#define TASK_LKJHMY2               3016
#define TASK_LKDZ                  3017
#define TASK_LKQXJF                3018

