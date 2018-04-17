#include "dlywd.h"
extern long nSysDebug;
extern char sTcpipErrInfo[81];

/*----------------------------------------------------------------
函数:DispatchTask
功能:根据任务号映射到处理该任务的函数
说明:TASKSTRUCT类型的task包括中包含了客户机的所有输参数:
       LONG_COUNT个长整型数据(req.nValue[0..LONG_COUNT-1]);
       DOUBLE_COUNT个双精度数据(req.dValue[0..DOUBLE_COUNT-1]);
       STRING_COUNT个字符串/数据流数据(req.sValue[0..STRING_COUNT-1]);
举例:
     假设服务端程序为:dsService(long ncs1,char * scs1,double dcs1);
     其任务号为:TASK_SRV_TEST
     则客户端的引用为：
     AutoRunTask("222.1.1.1",TASK_SRV_TEST,"%ld%s%lf","%s",ncs1,scs1,dcs1,sval);
     
     则本函数应对该任务进行相应映射：
     
     switch(task->nTaskNo)
     {
         case TASK_SRV_TEST:
              nRetVal=dsService(task->nValue[0],task->sValue[0],task->dValue[0]);
              break;       
         default:return -1;
     }
     
*---------------------------------------------------------------*/

long DispatchTask(TASKSTRUCT * task)
{
	long nRetVal=1;

	if(nSysDebug)
	{
		printf("\nTaskNo=%ld",task->nTaskNo);
		printf("\nLONG[0..%ld]=\n[",LONG_COUNT-1);
		for(nRetVal=0;nRetVal<LONG_COUNT;nRetVal++) printf("%ld,",task->nValue[nRetVal]);
		printf("]\n");

		printf("\nDOUBLE[0..%ld]=\n[",DOUBLE_COUNT-1);
		for(nRetVal=0;nRetVal<DOUBLE_COUNT;nRetVal++) printf("%.2lf,",task->dValue[nRetVal]);
		printf("]\n");
    
		printf("\nSTRING=\n");
		for(nRetVal=0;nRetVal<STRING_COUNT;nRetVal++)
			if(task->nLength[nRetVal]>0) printf("%s\n",task->sValue[nRetVal]);
	}
        
	switch(task->nTaskNo)
	{
		//查询交易入口
		case TASK_PAY_QUERY:
			nRetVal=dsTaskPayQuery(task->sValue[0]);
			break; 
		//缴费交易入口     
		case TASK_PAY_CONFIRM:
			nRetVal=dsTaskPayConfirm(task->sValue[0]);
			break; 
		//本次交费撤消入口     
		case TASK_RECALL_CONFIRM:
			nRetVal=dsTaskRecallConfirm(task->sValue[0]);
			break; 
        //对帐交易入口     
		case TASK_PAY_CHECKACCOUNT:
			nRetVal=dsTaskCheckAccount(task->sValue[0]);
			break;

		case TASK_REQUEST_PRINT:
			nRetVal=dsTaskRequestPrint(task->sValue[0]);
			break;

		//进货入库     
		case TASK_COMMIT_PURCHASE:
			nRetVal=dsTaskCommitPurchase(task->sValue[0]);
			break;
		//发货入库
		case TASK_COMMIT_DISTRIBUTE:
			nRetVal=dsTaskCommitDistribute(task->sValue[0]);
			break;
                //接货入库
		case TASK_ACCEPT_DISTRIBUTE:
			nRetVal=dsTaskAcceptDistribute(task->sValue[0]);
			break;
		//销售商品入库
		case TASK_SELL_GOODS:
			nRetVal=dsTaskSellGoods(task->sValue[0],task->sValue[1],
									task->sValue[2],task->nValue[0]);
			break;
		//客户退货入库             
		case TASK_RECALL_GOODS:
			nRetVal=dsTaskRecallGoods(task->sValue[0],task->sValue[1],
				task->sValue[2],task->nValue[0]);
			break;
		//商口报损入库     
		case TASK_WASTE_GOODS:
			nRetVal=dsTaskWasteGoods(task->sValue[0],task->sValue[1],
									task->sValue[2],task->nValue[0]);
			break;

		case TASK_RETURN_GOODS:
			nRetVal=dsTaskReturnGoods(task->sValue[0],task->sValue[1],
										task->sValue[2],task->nValue[0]);
		break;

		case TASK_GET_SPBH:
		//	nRetVal=nQdSpbh(task->sValue[0]);
			break;
		//移动签到
		case TASK_MOBILE_SIGN_IN:
			nRetVal=dsTaskMobileSIngIn(task->sValue[0]);
			break;
		//移动签退
		case TASK_MOBILE_SIGN_OUT:
			nRetVal=dsTaskMobileSIngOut(task->sValue[0]);
			break;
		//移动完全划拨打印     
		case TASK_MOBILE_COMPLETE_PRINT:
			nRetVal=dsTaskMobileCompletePrint(task->sValue[0],task->sValue[1],
							task->sValue[2],task->sValue[3]);
			break;
		
		//电费签到签退
		case TASK_POWER_SIGN_IN:
		        nRetVal=dsTaskPowerSignIn(task->sValue[0]);
		        break;
		case TASK_POWER_SIGN_OUT:
		        nRetVal=dsTaskPowerSignOut(task->sValue[0]);
		        break;
		        
		//寻呼测试呼     
		case TASK_BPUSER_TEST_CALL:
			nRetVal=dsTaskBpTestCall(task->sValue[0],task->sValue[1]);
			break;
		//从远地文件中获得某个索引项的值     
		case TASK_GET_VALUE_FROM_FILE:
			nRetVal=dsTaskGetValueFromFile(task->sValue[0],task->sValue[1],
											task->sValue[2]);
			break;

		case TASK_PUT_VALUE_TO_FILE:
			nRetVal=dsTaskPutValueToFile(task->sValue[0],task->sValue[1],
											task->sValue[2]);
			break;
		//从远地下载一个文件     
		case TASK_GET_FILE_FROM_REMOTE:
			nRetVal=dsTaskGetOneFileFromRemote(task->sValue[0]);
			break;
        
		case TASK_SEND_FILE_TO_REMOTE:
			nRetVal=dsTaskSendFileToRemote(task->sValue[0],task->sValue[1]);
			break;
		
		//系统备份
		case TASK_SYSTEM_BACKUP:
			nRetVal=dsTaskSystemBackup();
			break;

		case TASK_SYSTEM_RESTORE:
			nRetVal=dsTaskSystemRestore(task->sValue[0]);
			break;

		case TASK_CLEANUP_HISTORY:
			nRetVal=dsTaskCleanupHistory(task->nValue[0],task->nValue[1]);
			break;
        
		case TASK_CREATE_DS_JYM:
			nRetVal=dsTaskCreateDsJym(task->sValue[0],task->sValue[1],
										task->sValue[2],task->nValue[0]);
			break;                          

		case TASK_CREATE_SP_JYM:
			nRetVal=dsTaskCreateSpJym(task->sValue[0],task->sValue[1],
										task->nValue[0]);
			break;                          

		case TASK_CREATE_DB_JYM:
			nRetVal=dsTaskCreateDbJym(task->sValue[0],task->sValue[1],
										task->sValue[2],task->nValue[0]);
			break;                          

		case TASK_GET_YD_JYM:
			nRetVal=dsTaskGetYdJym(task->sValue[0]);
			break; 
		
		case TASK_CREATE_YD_JYM:
			//nRetVal=dsTaskCreateYdJym(task->sValue[0],task->sValue[1],task->sValue[2]);
			break;
		
		case TASK_GET_YD_JYM_MX:
			//nRetVal=dsTaskGetYdJymMx(task->sValue[0],task->sValue[1]);
			break;
		
		case TASK_CHECK_OPEN:
		     nRetVal=dsTaskCheckOpen(task->sValue[0],task->sValue[1]);
		     break;

		case TASK_OPEN_JYM:
			nRetVal=dsTaskOpenJym(task->sValue[0],task->sValue[1],task->sValue[2],task->nValue[0]);
			//nRetVal=dsTaskOpenJym(task->sValue[0]);
			break;                          
	
		case TASK_DELETE_JYM:
			nRetVal=dsTaskDeleteJym(task->sValue[0]);
			break;                          

		case TASK_DAIBANYW:
			nRetVal=dsTaskDaibanYw(task->sValue[0]);
			break;
 
		case TASK_GET_ALL_JSDH:
			nRetVal=dsTaskGetAllJsdh(task->sValue[0]);
			break;

		case TASK_GET_ALL_CITY:
			nRetVal=dsTaskGetAllCity();
			break;

		case TASK_QUERY_AIRLINE:
		/*	nRetVal=dsTaskQueryAirLine(task->sValue[0],task->sValue[1],task->sValue[2],
										task->nValue[0],task->nValue[1],task->sValue[3]);*/
			break;

		case TASK_QUERY_AIR_PRICE:
			nRetVal=dsTaskQueryAirPrice(task->sValue[0],task->sValue[1],
										task->sValue[2],task->nValue[0]);
			break;

		case TASK_QUERY_AIR_ROUTE:
			nRetVal=dsTaskQueryAirRoute(task->sValue[0],task->nValue[0]);
			break;

		case TASK_AIR_PURCHASE:
			nRetVal=dsTaskAirPurchase(task->sValue[0]);
			break;

		case TASK_AIR_CONFIRM:
			nRetVal=dsTaskAirConfirm(task->sValue[0]);
			break;

		case TASK_AIR_RETURN_TICKET:
			nRetVal=dsTaskAirReturnTicket(task->sValue[0]);
			break;

		case TASK_AIR_SIGN_IN:
			nRetVal=dsTaskAirSignIn(task->sValue[0]);
			break;

		case TASK_AIR_SIGN_OUT:
			nRetVal=dsTaskAirSignOut();
			break;

		case TASK_OUT_AIR_TICKET:
			nRetVal=dsTaskOutAirTicket(task->sValue[0],task->nValue[0]);
			break;
             
		case TASK_QUERY_AIR_BERTH:
			//nRetVal=dsTaskQueryAirBerth(task->sValue[0],task->nValue[0]);
			break;
		case TASK_AIR_ONLINE:
			nRetVal=dsTaskAirOnLine(task->sValue[0]);
			break;
		case TASK_AIR_CHECK:
			nRetVal=dsTaskAirCheck(task->sValue[0]);
			break;

		case TASK_LOAD_CHECK_TICKET:     
			nRetVal=dsTaskLoadCheckTicketData(task->sValue[0]);
			break;
		case TASK_SAVE_CHECK_RESULT:
			nRetVal=dsTaskSaveCheckResult(task->sValue[0]);
			break;
		
		case TASK_GET_DES_AIRTICKET:     
			nRetVal=dsTaskGetDesAirData(task->sValue[0]);
			break;
		case TASK_CHECK_DES_AIRTICKET:     
			nRetVal=dsTaskCheckDesAirTicket(task->sValue[0]);
			break;	
		case TASK_CHECK_RETURN_DES_AIRTICKET:     
			nRetVal=dsTaskCheckReturnDesAirTicket(task->sValue[0]);
			break;
		
	    case TASK_QUERY_TICKET:     
			nRetVal=dsTaskQueryAirTicket(task->sValue[0]);
			break;
	    case TASK_AIR_PAY:     
			nRetVal=dsTaskSaveAirPay(task->sValue[0]);
			break;

           //case TASK_0:
           //      break;	                     

//heqing  电话银行前置机任务
        case TASK_LKQD:
             nRetVal=dsTaskLkqd();
             break;
        
        case TASK_LKQT:
             nRetVal=dsTaskLkqt();
             break;

        case TASK_LKJHMY1: 
             nRetVal=dsTaskLkjhmy1();
             break;

        case TASK_LKJHMY2:
             nRetVal=dsTaskLkjhmy2();
             break;

        case TASK_LKJF:
             nRetVal=dsTaskLkjf(task->sValue[0]);
             break;

        case TASK_LKQXJF:
             nRetVal=dsTaskLkqxjf(task->sValue[0]);
             break;

        case TASK_LKDZ:
             nRetVal=dsTaskLkdz(task->sValue[0]);
             break;
/*        
        case TASK_LKDZQX:
             nRetVal=dsTaskLkdzqx(task->sValue[0]);
             break;
        default:return -1;
*/
	}
	if(nRetVal) printf("\nError:%s",sTcpipErrInfo);
	return nRetVal;   	
}

