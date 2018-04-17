#include "dlywd.h"
extern long nSysDebug;
extern char sTcpipErrInfo[81];

/*----------------------------------------------------------------
����:DispatchTask
����:���������ӳ�䵽���������ĺ���
˵��:TASKSTRUCT���͵�task�����а����˿ͻ��������������:
       LONG_COUNT������������(req.nValue[0..LONG_COUNT-1]);
       DOUBLE_COUNT��˫��������(req.dValue[0..DOUBLE_COUNT-1]);
       STRING_COUNT���ַ���/����������(req.sValue[0..STRING_COUNT-1]);
����:
     �������˳���Ϊ:dsService(long ncs1,char * scs1,double dcs1);
     �������Ϊ:TASK_SRV_TEST
     ��ͻ��˵�����Ϊ��
     AutoRunTask("222.1.1.1",TASK_SRV_TEST,"%ld%s%lf","%s",ncs1,scs1,dcs1,sval);
     
     �򱾺���Ӧ�Ը����������Ӧӳ�䣺
     
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
		//��ѯ�������
		case TASK_PAY_QUERY:
			nRetVal=dsTaskPayQuery(task->sValue[0]);
			break; 
		//�ɷѽ������     
		case TASK_PAY_CONFIRM:
			nRetVal=dsTaskPayConfirm(task->sValue[0]);
			break; 
		//���ν��ѳ������     
		case TASK_RECALL_CONFIRM:
			nRetVal=dsTaskRecallConfirm(task->sValue[0]);
			break; 
        //���ʽ������     
		case TASK_PAY_CHECKACCOUNT:
			nRetVal=dsTaskCheckAccount(task->sValue[0]);
			break;

		case TASK_REQUEST_PRINT:
			nRetVal=dsTaskRequestPrint(task->sValue[0]);
			break;

		//�������     
		case TASK_COMMIT_PURCHASE:
			nRetVal=dsTaskCommitPurchase(task->sValue[0]);
			break;
		//�������
		case TASK_COMMIT_DISTRIBUTE:
			nRetVal=dsTaskCommitDistribute(task->sValue[0]);
			break;
                //�ӻ����
		case TASK_ACCEPT_DISTRIBUTE:
			nRetVal=dsTaskAcceptDistribute(task->sValue[0]);
			break;
		//������Ʒ���
		case TASK_SELL_GOODS:
			nRetVal=dsTaskSellGoods(task->sValue[0],task->sValue[1],
									task->sValue[2],task->nValue[0]);
			break;
		//�ͻ��˻����             
		case TASK_RECALL_GOODS:
			nRetVal=dsTaskRecallGoods(task->sValue[0],task->sValue[1],
				task->sValue[2],task->nValue[0]);
			break;
		//�̿ڱ������     
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
		//�ƶ�ǩ��
		case TASK_MOBILE_SIGN_IN:
			nRetVal=dsTaskMobileSIngIn(task->sValue[0]);
			break;
		//�ƶ�ǩ��
		case TASK_MOBILE_SIGN_OUT:
			nRetVal=dsTaskMobileSIngOut(task->sValue[0]);
			break;
		//�ƶ���ȫ������ӡ     
		case TASK_MOBILE_COMPLETE_PRINT:
			nRetVal=dsTaskMobileCompletePrint(task->sValue[0],task->sValue[1],
							task->sValue[2],task->sValue[3]);
			break;
		
		//���ǩ��ǩ��
		case TASK_POWER_SIGN_IN:
		        nRetVal=dsTaskPowerSignIn(task->sValue[0]);
		        break;
		case TASK_POWER_SIGN_OUT:
		        nRetVal=dsTaskPowerSignOut(task->sValue[0]);
		        break;
		        
		//Ѱ�����Ժ�     
		case TASK_BPUSER_TEST_CALL:
			nRetVal=dsTaskBpTestCall(task->sValue[0],task->sValue[1]);
			break;
		//��Զ���ļ��л��ĳ���������ֵ     
		case TASK_GET_VALUE_FROM_FILE:
			nRetVal=dsTaskGetValueFromFile(task->sValue[0],task->sValue[1],
											task->sValue[2]);
			break;

		case TASK_PUT_VALUE_TO_FILE:
			nRetVal=dsTaskPutValueToFile(task->sValue[0],task->sValue[1],
											task->sValue[2]);
			break;
		//��Զ������һ���ļ�     
		case TASK_GET_FILE_FROM_REMOTE:
			nRetVal=dsTaskGetOneFileFromRemote(task->sValue[0]);
			break;
        
		case TASK_SEND_FILE_TO_REMOTE:
			nRetVal=dsTaskSendFileToRemote(task->sValue[0],task->sValue[1]);
			break;
		
		//ϵͳ����
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

//heqing  �绰����ǰ�û�����
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

