/*============================================================*
����������ҵ����ѯ����˳���TASK_DAIBANYW
���ܣ�����ҵ��ͳһ���ģ��
������inbuf:��������ַ��������ĸ�ʽ���£��ڸ��ֶ��á�|��������
      
��ˮ��|����Ա����|10���ַ�������...|10����������...|10���������...|

���أ�>0�ɹ� 
   �ɹ���000000|�ɹ���Ϣ|
   ʧ�ܣ�XXXXXX|������Ϣ|         

*============================================================*/
#include"dlywd.h"

extern long nSysSfm,nSysLkrq;
long dsTaskDaibanYw(char * inbuf)
{
	
     char sJym[16],sJsdh[10];
     long nCxms,nRetVal;
     char outbuf[128];
     
     memset(outbuf,0,sizeof(outbuf));
     
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     nRetVal=RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);
     
     if(nRetVal<=0)
     	return ApplyToClient("%s","300001|����Դ�޴˴��콻��");
     	
     switch(nCxms)
     {
     	case 1001:  //193
     	     dsTaskUnicon193Phone(inbuf,outbuf);
     	     break;
     }
     
     return ApplyToClient("%s",outbuf);

}

long dsTaskUnicon193Phone(char * inbuf,char * outbuf)
{
    
     char buf[256],sLsh[41],sCzydh[7],sJym[16],sJsdh[10],sXh[10];
     long nRetVal,nXh;
     long nValue[10];
     double dValue[10];
     char sValue[10][41];
     
     printf("\naccess 1...");
     fflush(stdout);
     if(InsertDbywmx(inbuf)<0)
     {
     	GetSqlErrInfo(buf);
     	sprintf(outbuf,"000011|%s",buf);
     	return -1;
     }
     
     printf("\naccess 2...");
     fflush(stdout);
     FetchStringToArg(inbuf,
     "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|",
     sLsh,sCzydh,sValue[0],sValue[1],sValue[2],sValue[3],sValue[4],sValue[5],sValue[6],sValue[7],
     sValue[8],sValue[9],&nValue[0],&nValue[1],&nValue[2],&nValue[3],&nValue[4],&nValue[5],&nValue[6],
     &nValue[7],&nValue[8],&nValue[9],&dValue[0],&dValue[1],&dValue[2],&dValue[3],&dValue[4],&dValue[5],
     &dValue[6],&dValue[7],&dValue[8],&dValue[9]);
     
     GenerateLsh(3,NULL,NULL,sXh);
     nXh=atol(sXh);
     
//     ��ˮ��|�û���־|�û�����|�ɿʽ|ʵ���ܿ��|����Ա����|ʱ����|�ɿ��·���{|���|�·�|���|�·�|......}

     printf("\naccess 2...");
     fflush(stdout);
     sprintf(buf,"%-40.40s|%s|%s|1|%.2lf|%s|%06ld|0|",sLsh,sValue[0],sValue[1],dValue[0],sCzydh,nSysSfm);
     nRetVal=InsertZwmxTable(buf,TASK_PAY_CONFIRM,nXh);
     
     if(nRetVal<0)
     {
     	GetSqlErrInfo(buf);
        RollbackWork();
     	sprintf(outbuf,"000011|%s",buf);
     	return -1;
     }
     
     CommitWork();
     strcpy(outbuf,"000000|�������׳ɹ�...");
     return 0;
}


long InsertDbywmx(char * inbuf)
{
     char buf[81],sLsh[41],sCzydh[7],sJym[16],sJsdh[10];
     long nRetVal;
     long nValue[10];
     double dValue[10];
     char sValue[10][41];
     
     
     printf("\naccess 1.2...");
     fflush(stdout);
     FetchStringToArg(inbuf,
     "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|",
     sLsh,sCzydh,sValue[0],sValue[1],sValue[2],sValue[3],sValue[4],sValue[5],sValue[6],sValue[7],
     sValue[8],sValue[9],&nValue[0],&nValue[1],&nValue[2],&nValue[3],&nValue[4],&nValue[5],&nValue[6],
     &nValue[7],&nValue[8],&nValue[9],&dValue[0],&dValue[1],&dValue[2],&dValue[3],&dValue[4],&dValue[5],
     &dValue[6],&dValue[7],&dValue[8],&dValue[9]);
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     
     printf("\naccess 1.3...");
     fflush(stdout);
     GetSysLkrq();
     nRetVal=RunSql("insert into dl$dbmx_%t(clsh,nlkrq,nsfm,cczydh,cvalue1,cvalue2,cvalue3,cvalue4,"
             "cvalue5,cvalue6,cvalue7,cvalue8,cvalue9,cvalue10,nvalue1,nvalue2,nvalue3,nvalue4,"
             "nvalue5,nvalue6,nvalue7,nvalue8,nvalue9,nvalue10,dvalue1,dvalue2,dvalue3,dvalue4,"
             "dvalue5,dvalue6,dvalue7,dvalue8,dvalue9,dvalue10) values(%s,%ld,%ld,%s,%s,%s,%s,%s,"
             "%s,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%lf,%lf,%lf,%lf,%lf,%lf,"
             "%lf,%lf,%lf,%lf)",
             sJym,sLsh,nSysLkrq,nSysSfm,sCzydh,sValue[0],sValue[1],sValue[2],sValue[3],sValue[4],sValue[5],
             sValue[6],sValue[7],sValue[8],sValue[9],nValue[0],nValue[1],nValue[2],nValue[3],
             nValue[4],nValue[5],nValue[6],nValue[7],nValue[8],nValue[9],dValue[0],dValue[1],dValue[2],
             dValue[3],dValue[4],dValue[5],dValue[6],dValue[7],dValue[8],dValue[9]);

     return nRetVal;
}
