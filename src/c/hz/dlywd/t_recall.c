#include"dlywd.h"
#include"t_mobile.h"

//extern long nSysSfm;
extern long nSysLkrq;
/*=========================================================
������dsTaskRecallConfirm   TASK_RECALL_CONFIRM
���ܣ���������ȷ��
������inbuf:��������������ģ�������������
      ������ˮ��|��������ˮ��|����Ա����|ʱ����|
���أ�0�ɹ�
      [����RECORD]
      �ɹ���000000|������¼��ˮ��|��������¼��ˮ��|
      ʧ�ܣ�XXXXXX|������Ϣ��������|
*========================================================*/

long dsTaskRecallConfirm(char * inbuf)
{

     char outbuf[256];//���ظ��ͻ���
     char sJym[16];
     char sJsdh[10];
     char sLsh[41],sRecallLsh[41];//������ˮ�źͱ�������ˮ��
     char sHead[8],sCzsj[21],sSjh[13];
     long nCxms,nxh;
     
     GetSysLkrq();     
     FetchStringToArg(inbuf,"%s|%s|",sLsh,sRecallLsh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     
     if(strncmp(sLsh+9,sRecallLsh+9,15))
     {
     	ApplyToClient("%s","000013|������ˮ���뱻������ˮ���еĽ����벻һ��|");
        return 0;
     }

     if(!RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms))
     {
     	ApplyToClient("%s","000002|����Դ�޴˽���|");
        return 0;
     }
     
     if(RunSelect("select * from dl$zwmx where clsh=%s",sLsh)>0)
     {
     	ApplyToClient("%s","000003|��ˮ���ظ�|");
        return 0;
     }	     

     if(!RunSelect("select * from dl$zwmx where cjym=%s and clsh=%s and ncsbz<2",sJym,sRecallLsh))
     {
     	ApplyToClient("%s","000010|����Դ�޴���ˮ�ŵĽɷѼ�¼��ñʽɷ��Ѷ���|");
        return 0;
     }	     
     GenerateLsh(3,sJsdh,sJym,outbuf);
     nxh=atol(outbuf);
     
     strcpy(outbuf,"000000");
     switch(nCxms)
     {
     	case 130: //�ƶ�
     	     dsMobileRecallConfirm(inbuf,nxh,outbuf);
     	     break;

        case 140://��������
             dsTelecomRecallConfirm(inbuf,nxh,outbuf);
             break;
                
        case 110://һ�㼯�з�ʽ�Ľɷ�[ע�⣺�벻Ҫӳ��Ԥ���]
        case 610:
        case 520:
        case 521:
             dsGeneralRecallConfirm(inbuf,nxh,outbuf);
             break;
    
        case 210:
        case 220:
        case 230:
             dsTaskBpUserRecallConfirm(inbuf,nxh,outbuf);
             break;
        
        case 1001:
             dsAirRecall(inbuf,nxh,outbuf);
             break;     

		case 821:
	     	dsTaskBxRecallConfirm(inbuf,nxh,outbuf);
 	     	FetchStringToArg(outbuf,"%s|%s|%s|",sHead,sCzsj,sSjh);
	     	break;

		case 123:
	    	//dsTaskPowerRecallConfirm(inbuf,nxh,outbuf);
	     	break;
		
		case 522:
	     	dsTaxRecallConfirm(inbuf,nxh,outbuf);
	    	break;        
     } 
     
     //�����볷��������ϸ��¼���޸�ҵ����ϸ���ʱ�־
     if(!strncmp(outbuf,"000000",6))
     {
        dsModifyZwmxOnRecall(inbuf,outbuf,nxh);
     	CommitWork();
     	sprintf(outbuf,"000000|%s|%s|",sLsh,sRecallLsh);
     }
     else 
     	RollbackWork();
     
     if(nCxms==821 && !strcmp(sHead,"000000") )
     {
        if(RunSql("update dl$zwmx set cbl1=%s,cbl3=%s where clsh=%s",
		sSjh,sCzsj,sLsh)>0)
	  CommitWork();
     }
     return ApplyToClient("%s",outbuf);

}

/*=========================================================
������dsModifyZwmxOnRecall
���ܣ������ɹ�����볷��������ϸ��¼���޸�ҵ����ϸ���ʱ�־
������inbuf:��������������ģ�������������
      ������ˮ��|��������ˮ��|ʵ�ɿ��|����Ա����|ʱ����|
      outbuf:���ظ��ͻ��˵ĳɹ���ʧ����Ϣ
���أ�0�ɹ�������ʧ��
*========================================================*/
long dsModifyZwmxOnRecall(char * inbuf,char * outbuf,long nxh)
{
     char sLsh[41],sRecallLsh[41];  //��ˮ��
     char sBuffer[256];
     char sYhbz[101]; //�û���־
     char sJym[16];  //������
     char sYhmc[51]; //�û�����
     char sCzydh[7]; //����Ա����
     char sJsdh[10]; //�ɷѾ־ֺ�
     long   nFkfs=1;   //���ɷ�ʽ1���ֽ�  2���̿�
     long  nRecallXh;
     double dSjk;  //ʵ�ɿӦ�ɿ�
     long nSfm=0,nSl=0;
	
    
     FetchStringToArg(inbuf,"%s|%s|%s|%d|",sLsh,sRecallLsh,sCzydh,&nSfm);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);

     //GenerateLsh(3,sJsdh,sJym,sBuffer);
     //nxh=atol(sBuffer);

     if(RunSelect("select nxh,cyhbz,cyhmc,nsfkfs,dsjk,nsl from dl$zwmx where cjym=%s and clsh=%s \
                into %ld%s%s%ld%lf%ld",sJym,sRecallLsh,&nRecallXh,sYhbz,sYhmc,&nFkfs,&dSjk,&nSl)<=0)
     {
         GetSqlErrInfo(sBuffer);
         sprintf(outbuf,"000011|��ѯ����ԭʼ��¼����:%s|",sBuffer);
         return -1;
     }
     
     sprintf(sBuffer,"%s|%s|%s|%ld|%.2lf|%s|%ld|%ld|",sLsh,sYhbz,sRecallLsh,nFkfs,
             dSjk,sCzydh,nSfm,nSl);
             
     //����������ϸ������¼
     if(InsertZwmxTable(sBuffer,TASK_RECALL_CONFIRM,nxh)<0)
     {
         GetSqlErrInfo(sBuffer);
         sprintf(outbuf,"000011|����������ϸ������¼����:%s|",sBuffer);
         return -1;
     }    
     
     //�޸�ԭ��¼��־λ����CLXBZ��19λ��2��������¼��nbl�óɱ�������ˮ��
     
     RunSql("update dl$zwmx set nbl=%ld where clsh=%s",nRecallXh,sLsh);
     RunSql("update dl$zwmx set clxbz='00000000000000000020' where clsh=%s",sRecallLsh);
     sprintf(outbuf,"000000|�����ɹ�|");
     return 0;

}


/*=========================================================
������dsGeneralRecallConfirm
���ܣ�һ�㼯��ʽ�ɷѵĳ�������
������inbuf:��������������ģ�������������
      ������ˮ��|��������ˮ��|����Ա����|ʱ����|
      nxh:��ί�з�������¼���
      outbuf:���ظ��ͻ��˵ĳɹ���ʧ����Ϣ
���أ�0�ɹ�������ʧ��
*========================================================*/
long dsGeneralRecallConfirm(char * inbuf,long nxh,char * outbuf)
{
     
     char sLsh[41];
     char sRecallLsh[41];
     char sJym[16];
     char sJsdh[10];
     char sSqlErr[81];
     long nRetVal;
    
     
     FetchStringToArg(inbuf,"%s|%s|",sLsh,sRecallLsh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     
     nRetVal=RunSql("update dl$ywmx_%t set nxzbz=0,clsh='0',dsjk=0.0 where clsh=%s",
                    sJym,sRecallLsh);
     
     /*nRetVal=RunSql("update dl$ywmx_%t set nxzbz=0,dsjk=0 where clsh=%s",
                    sJym,sRecallLsh);*/
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sSqlErr);
     	sprintf(outbuf,"000011|���ݿ����ʧ�ܣ�%s|",sSqlErr);
     	return -1;
     }
     strcpy(outbuf,"000000|�����ɹ�|");
     return 0;	

}
