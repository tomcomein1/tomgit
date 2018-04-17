
#define TOTAL_PACKAGE_SIZE 20000


#define TASK_PAY_QUERY            100         //缴费查询任务
#define TASK_PAY                  101         //缴费任务
#define TASK_PAY_CONFIRM          102         //缴费撤单任务    
#define TASK_RECALL               103         //撤单请求
#define TASK_RECALL_CONFIRM       104         //撤单确认任务
#define TASK_PAY_CHECKACCOUNT     105         //前台单条对帐请求
#define TASK_CREATE_DS_JYM        106         //建立代收交易码
#define TASK_CREATE_SP_JYM        107         //建立代售交易码
#define TASK_CREATE_DB_JYM        108         //建立代办交易码
#define TASK_DELETE_JYM           109         //删除交易码
#define TASK_OPEN_JYM             110         //交易码开放
#define TASK_CZYJK_CONFIRM        111         //操作员缴款
#define TASK_DAIBANYW             112         //代办业务申请
#define TASK_REQUEST_PRINT        120         //申请打印
#define TASK_CHECK_OPEN           121         //检查远方开放情况

#define TASK_GET_YD_JYM           130         //获取所有异地交易码
#define TASK_CREATE_YD_JYM        131      	  //建立异地交易码
#define TASK_GET_YD_JYM_MX        132         //获取异地交易码明细

#define TASK_0 200 // 测试用
#define CALL_0 400

//////////!!!警告:请不要改变以下任务号的值!!!!///////////
#define TASK_MOBILE_SIGN_IN                   400     //签到操作
#define TASK_MOBILE_SIGN_OUT                  401     //签退操作
#define TASK_MOBILE_PAY_QUERY                 402     //交费查询操作
#define TASK_MOBILE_PAY_CHECK                 403     //交费验证操作
#define TASK_MOBILE_PAY_CONFIRM               404     //现金交费确认操作
#define TASK_MOBILE_PAY_COMPARE               405     //对总帐交易
#define TASK_MOBILE_RECALL                    406     //删帐交易
#define TASK_MOBILE_RECALL_CONFIRM            407     //删帐交易确认
#define TASK_MOBILE_COMPLETE_PRINT            408     //完全划拨查询
#define TASK_MOBILE_ALREADY_PAID              409     //已交费查询
//////////!!!请不要改变400-408号任务号的值!!!///////////
#define TASK_BPUSER_TEST_CALL                 410

#define TASK_MOBILE_OPERATION          500  //手机代收数据请求
#define TASK_MOBILE_PACKAGE            501  //手机代收包请求
#define TASK_BPUSER_PACKAGE            502
//////////!!!警告:请不要改变以下任务号的值!!!!///////////

#define TASK_COMMIT_PURCHASE           510  //进货确认入库     进货
#define TASK_COMMIT_DISTRIBUTE         511  //分货确认入库     分货
#define TASK_ACCEPT_DISTRIBUTE         512  //接收分货         接货
#define TASK_COMMIT_SENDBACK           513  //向委托方退货确认 退货
#define TASK_SELL_GOODS                514  //销货入库
#define TASK_RECALL_GOODS              515  //客户退货
#define TASK_WASTE_GOODS               516  //报损
#define TASK_RETURN_GOODS              517  //向委托方退货

//////////!!!警告:请不要改变以下任务号的值!!!!///////////
#define TASK_POWER_SIGN_IN                   600     //签到操作
#define TASK_POWER_SIGN_OUT                  601     //签退操作
#define TASK_POWER_PAY_QUERY                 602     //交费查询操作
#define TASK_POWER_PAY_CHECK                 603     //交费验证操作
#define TASK_POWER_PAY_CONFIRM               604     //现金交费确认操作
#define TASK_POWER_PAY_COMPARE               605     //对总帐交易
#define TASK_POWER_RECALL                    606     //删帐交易
#define TASK_POWER_RECALL_CONFIRM            607     //删帐交易确认
#define TASK_POWER_PAY_BUTTON                608     //完全划拨查询
#define TASK_POWER_REPRINT                   609     //已交费查询
//////////!!!请不要改变600-609号任务号的值!!!///////////

//1000-1999 为委托的任务号
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

//2000-2999 为电子商务bk的任务号

//3000-3999 为电子商务dl的任务号
#define TASK_DL185_JHMY            3009   //jiaohuanmiyao
#define TASK_DL185_CXZZ            3010   //jiaoyizongzhangchaxun
#define TASK_LKQD                  3011   //绿卡签到
#define TASK_LKQT                  3012   //绿卡签退
#define TASK_LKJF                  3013   //绿卡缴费
#define TASK_LKJHMY                3014   //绿卡交换密钥
#define TASK_LKJHMY1               3015
#define TASK_LKJHMY2               3016
#define TASK_LKDZ                  3017
#define TASK_LKQXJF                3018

