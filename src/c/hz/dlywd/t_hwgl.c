/*=========================================================
������dsTaskCommitPurchase   TASK_COMMIT_PURCHASE
���ܣ�����Ա������¼���
������sCzydh:����Ա����
���أ�0���ɹ�������ʧ��
      [����RECORD]
      �ɹ���000000|����Ա����|
      ʧ�ܣ�XXXXXX|ʧ����Ϣ����˵��|
*========================================================*/
#include<stdio.h>
#include "t_hwgl.h"

extern long nSysLkrq;
extern long nSysSfm;


long dsTaskCommitPurchase(char * sCzydh)
{

    char sInfo[201],sSqlErr[81],sLsh[41];
    long nCount,nId,nRetVal;

    nId=OpenCursor("select cLsh from dl$hwjcb where nczlb=1 and njlzt=0 and \
                    cjhczydh=%s",sCzydh);

    if(nId<0)
    {
    	GetSqlErrInfo(sSqlErr);
    	sprintf(sInfo,"000011|���α����:%s|",sSqlErr);
    	return ApplyToClient("%s",sInfo);
    }
    
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%s",sLsh);
    	if(nRetVal==0)
    	{
    		nRetVal=1;
    		break;
    	}
        //�޸ļ�¼״̬Ϊ������
        if(nRetVal>0)
               nRetVal=RunSql("update dl$hwjcb set njlzt=1 where cLsh=%s",sLsh);
        //������ˮ�ʼ�¼�޸Ŀ����
        if(nRetVal>0)  
               nRetVal=ProcessRecord(sLsh);
        if(nRetVal<=0) break;        	      	

    }
    CloseCursor(nId);
    if(nRetVal<=0)
    {
    	GetSqlErrInfo(sSqlErr);
    	RollbackWork();
    	sprintf(sInfo,"000011|������ˮ���ݳ���:%s|",sSqlErr);
    	return ApplyToClient("%s",sInfo);
    }
    CommitWork();
    sprintf(sInfo,"000000|%s|",sCzydh);
    return ApplyToClient("%s",sInfo);

}


static long ProcessRecord(char * sLsh)
{
    
    char sJym[16],sFhczydh[21],sJhczydh[21];
    long nCzlb,nCount,identify,nstatus;
    long nRetVal=1;
    
    
    GetSysLkrq();
    if(RunSelect("select nczlb,cjym,nsl,cfhczydh,cjhczydh,\
                  to_number(substr(clxbz,1,1)),njlzt from \
                  dl$hwjcb where clsh=%s into %ld%s%ld%s%s%ld%ld",
                  sLsh,&nCzlb,sJym,&nCount,sFhczydh,sJhczydh,&identify,&nstatus)<=0) return -1;
    
    //������¼�Ƿ���ڣ��䲻��������һ���հ׼�¼
    if(nCzlb!=7&&nCzlb!=2&&nCzlb!=6)
          if(CheckOrInsertStock(sJhczydh,sJym,identify)<0) return -1;            
                 
    switch(nCzlb)
    {
    	case 1://����,���ӱ�����Ա���
    	     switch(nstatus)
    	     {
    	     	case 1:
                     return RunSql("update dl$hwkcb set nkcl=nkcl+%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz=%s and cjym=%s",
                            nCount,nSysLkrq,nSysSfm,sJhczydh,sJym);
             }
             break;

        case 3://�������ӻ�
             if(CheckOrInsertStock(NULL,sJym,identify)<0) return -1;
             switch(nstatus)
             {
             	case 1://ȷ�Ϸ��������ٱ�����Ա��棬���ӹ��ÿ��[PUBLIC]
                   nRetVal=RunSql("update dl$hwkcb set nkcl=nkcl-%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz=%s and cjym=%s",
                            nCount,nSysLkrq,nSysSfm,sFhczydh,sJym);
                   if(nRetVal>0)
                      return RunSql("update dl$hwkcb set nkcl=nkcl+%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz='PUBLIC' and cjym=%s and substr(clxbz,1,1)='1'",
                            nCount,nSysLkrq,nSysSfm,sJym);
                   return nRetVal;
                   
                case 2://���ܷ��������ӱ�����Ա��棬���ٹ��ÿ��[PUBLIC]
                   nRetVal=RunSql("update dl$hwkcb set nkcl=nkcl+%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz=%s and cjym=%s",
                            nCount,nSysLkrq,nSysSfm,sJhczydh,sJym);
                   if(nRetVal>0)
                      return RunSql("update dl$hwkcb set nkcl=nkcl-%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz='PUBLIC' and cjym=%s and substr(clxbz,1,1)='1'",
                            nCount,nSysLkrq,nSysSfm,sJym);
                   return nRetVal;
             }                     
             break;

       case 2://��ί�з��˻�
       case 6://����
       case 7://�۳�
             return RunSql("update dl$hwkcb set nkcl=nkcl-%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz=%s and cjym=%s",
                            nCount,nSysLkrq,nSysSfm,sFhczydh,sJym);
                            
       case 8://�ͻ��˻�
             return RunSql("update dl$hwkcb set nkcl=nkcl+%ld,nlkrq=%ld,nsfm=%ld \
                            where csyz=%s and cjym=%s",
                            nCount,nSysLkrq,nSysSfm,sJhczydh,sJym);
                            
    }                        
        
}

static long CheckOrInsertStock(char * sCzydh,char * sJym,long identfy)
{

    char cLxbz[11]="0000000000";
    char sJsdh[10]=".";
    char sHwsyz[21];
    long nRetVal;
    
   
    if(sCzydh)
    {
    	 nRetVal=RunSelect("select * from dl$hwkcb where csyz=%s and cjym=%s",sCzydh,sJym);
    	 cLxbz[0]='0';  //˽�е�
    	 strcpy(sHwsyz,sCzydh);
    }
    else 
    {
    	 nRetVal=RunSelect("select * from dl$hwkcb where cjym=%s and \
    	                    substr(clxbz,1,1)='1'",sJym);
         cLxbz[0]='1';  //���е�
    	 strcpy(sHwsyz,"PUBLIC");
    }
    
    if(nRetVal>0) return 1;     //�Ѵ��ڿ���¼

    RunSelect("select cjh from dl$czyb where cczydh=%s into %s",sHwsyz,sJsdh); 
    //�������һ���հ׼�¼
    return  RunSql("insert into dl$hwkcb(cjym,csyz,nkcl,nlkrq,nsfm,clxbz,cjh,nbl,dbl,cbl) \
                    values(%s,%s,0,%ld,%ld,%s,%s,0,0.0,'0')",
                    sJym,sHwsyz,nSysLkrq,nSysSfm,cLxbz,sJsdh);

}


/*=========================================================
������dsTaskCommitDistribute  TASK_COMMIT_DISTRIBUTE
���ܣ�����Ա�ֻ���¼���
������sCzydh:����Ա����
���أ�0���ɹ�������ʧ��
      [����RECORD]
      �ɹ���000000|����Ա����|
      ʧ�ܣ�XXXXXX|ʧ����Ϣ����˵��|
*========================================================*/


long dsTaskCommitDistribute(char * sCzydh)
{
    char sInfo[201],sSqlErr[81],sLsh[41],sHwbz[21],sJym[16];
    long nCount,nId,nRetVal,identify;

    nId=OpenCursor("select cLsh,to_number(substr(clxbz,1,1)),chwbz,cjym from \
                    dl$hwjcb where nczlb=3 and njlzt=0 and cfhczydh=%s",sCzydh);

    if(nId<0)
    {
    	GetSqlErrInfo(sSqlErr);
    	sprintf(sInfo,"000011|���α����:%s|",sSqlErr);
    	return ApplyToClient("%s",sInfo);
    }
    
    for(;;)
    {
    	nRetVal=FetchCursor(nId,"%s%ld%s%s",sLsh,&identify,sHwbz,sJym);
    	if(nRetVal==0)
    	{
    		nRetVal=1;
    		break;
    	}
    	
        //�޸ļ�¼״̬Ϊ������
        if(nRetVal>0)
        {
               nRetVal=RunSql("update dl$hwjcb set njlzt=1 where cLsh=%s",sLsh);
               if(nRetVal>0&&identify)
                   nRetVal=RunSql("update dl$hwjcb set njlzt=3 where cjhczydh=%s \
                                   and chwbz=%s and cjym=%s and (nczlb=1 or nczlb=3)",
                                   sCzydh,sHwbz,sJym);
        }                           
               
        //������ˮ�ʼ�¼�޸Ŀ����
        if(nRetVal>0)  
               nRetVal=ProcessRecord(sLsh);
        if(nRetVal<=0) break;        	      	

    }
    CloseCursor(nId);
    if(nRetVal<=0)
    {
    	GetSqlErrInfo(sSqlErr);
    	RollbackWork();
    	sprintf(sInfo,"000011|������ˮ���ݳ���:%s|",sSqlErr);
    	return ApplyToClient("%s",sInfo);
    }
    CommitWork();
    sprintf(sInfo,"000000|%s|",sCzydh);
    return ApplyToClient("%s",sInfo);

}


/*=========================================================
������dsTaskAcceptDistribute  TASK_ACCEPT_DISTRIBUTE
���ܣ�����Ա���ֻܷ�
������sLsh:������ˮ��
���أ�0���ɹ�������ʧ��
      [����RECORD]
      �ɹ���000000|��ˮ��|
      ʧ�ܣ�XXXXXX|ʧ����Ϣ����˵��|
*========================================================*/

long dsTaskAcceptDistribute(char * sLsh)
{

     char sbuf[21],sTemp[101];
     long nRetVal;
     

     nRetVal=RunSql("update dl$hwjcb set njlzt=2 where clsh=%s and njlzt=1",sLsh);
     if(nRetVal>0)  nRetVal=ProcessRecord(sLsh);
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sbuf);
     	RollbackWork();
     	sprintf(sTemp,"000011|%s|",sbuf);
     }
     else
     {
     	CommitWork();
     	sprintf(sTemp,"000000|%s|",sLsh);
     }
     return ApplyToClient("%s",sTemp);	   	
      
}

/*=========================================================
������dsTaskSellGoods   TASK_SELL_GOODS
���ܣ������������
������sLsh:��ˮ��
      sCzydh:����Ա����
      sHwbz:�����־
      nCount:����
���أ�0���ɹ�������ʧ��
      [����RECORD]
      �ɹ���000000|��ˮ��|
      ʧ�ܣ�XXXXXX|ʧ����Ϣ����˵��|
*========================================================*/
long dsTaskSellGoods(char * sLsh,char * sCzydh,char * sHwbz,long nCount)
{

     char sJym[16],sbuf[81],sJymc[51]=".";
     char sJsdh[10],cLxbz[21]="00000000000000000000";
     long identify=0,nRetVal;
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     RunSelect("select to_number(substr(clxbz,5,1)),cjymc from dl$jym \
                where cjym=%s into %ld%s",sJym,&identify,sJymc);
     nRetVal=1;
     if(identify)
     {
     	   nRetVal=RunSql("update dl$hwjcb set njlzt=3 where cjym=%s and cjhczydh=%s \
                     and chwbz=%s and ((nczlb=1 and njlzt=1) or (nczlb=3 and njlzt=2))",
                     sJym,sCzydh,sHwbz);
           cLxbz[0]='1';
     }
        

     if(nRetVal>0)
         nRetVal=RunSql("insert into dl$hwjcb(cjym,cjymc,clsh,djhdj,nsl,chwbz,\
                        cfhczydh,cjhczydh,cfhjh,cjhjh,njhrq,njhsfm,nfhrq,nfhsfm,\
                        nczlb,njlzt,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,clxbz) \
                        values(%s,%s,%s,0,%ld,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,\
                        7,1,0,0,0,0,'0','0',%s)",
                        sJym,sJymc,sLsh,nCount,sHwbz,sCzydh,"�ͻ�",sJsdh,
                        ".",nSysLkrq,nSysSfm,nSysLkrq,nSysSfm,cLxbz);
     
     if(nRetVal>0) nRetVal=ProcessRecord(sLsh);
     
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sJymc);
     	RollbackWork();
     	sprintf(sbuf,"000011|%s|",sJymc);
     	return ApplyToClient("%s",sbuf);
     }
     
     CommitWork();
     sprintf(sbuf,"000000|%s|",sLsh);
     return ApplyToClient("%s",sbuf);

}


/*=========================================================
������dsTaskRecallGoods   TASK_RECALL_GOODS
���ܣ��ͻ��������
������sLsh:��ˮ��
      sCzydh:����Ա����
      sHwbz:�����־
      nCount:����
���أ�0���ɹ�������ʧ��
      [����RECORD]
      �ɹ���000000|��ˮ��|
      ʧ�ܣ�XXXXXX|ʧ����Ϣ����˵��|
*========================================================*/
long dsTaskRecallGoods(char * sLsh,char * sCzydh,char * sHwbz,long nCount)
{

     char sJym[16],sbuf[81],sJymc[51]=".",sYlsh[41]=".";
     char sJsdh[10],cLxbz[21]="00000000000000000000";
     long identify=0,nRetVal,nCzlb,nJlzt;
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     RunSelect("select to_number(substr(clxbz,5,1)),cjymc from dl$jym \
                where cjym=%s into %ld%s",sJym,&identify,sJymc);

     if(identify)
     {
          
           nRetVal=RunSelect("select nczlb,clsh from dl$hwjcb where cjym=%s and cjhczydh=%s \
                      and chwbz=%s and ((nczlb=1 and njlzt=3) or (nczlb=3 and njlzt=3)) \
                      into %ld%s",sJym,sCzydh,sHwbz,&nCzlb,sYlsh);

           if(nCzlb==1) nJlzt=1;
           else nJlzt=2;
           
     	   if(nRetVal>0)
     	      nRetVal=RunSql("update dl$hwjcb set njlzt=%ld where clsh=%s",nJlzt,sYlsh);

           cLxbz[0]='1';
     }

     nRetVal=1;
     if(nRetVal>0)
         nRetVal=RunSql("insert into dl$hwjcb(cjym,cjymc,clsh,djhdj,nsl,chwbz,\
                        cfhczydh,cjhczydh,cfhjh,cjhjh,njhrq,njhsfm,nfhrq,nfhsfm,\
                        nczlb,njlzt,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,clxbz) \
                        values(%s,%s,%s,0,%ld,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,\
                        8,1,0,0,0,0,'0','0',%s)",
                        sJym,sJymc,sLsh,nCount,sHwbz,"�ͻ�",sCzydh,".",sJsdh,
                        nSysLkrq,nSysSfm,nSysLkrq,nSysSfm,cLxbz);
     
     if(nRetVal>0) nRetVal=ProcessRecord(sLsh);
     
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sJymc);
     	RollbackWork();
     	sprintf(sbuf,"000011|%s|",sJymc);
     	return ApplyToClient("%s",sbuf);
     }
     CommitWork();
     sprintf(sbuf,"000000|%s|",sLsh);
     return ApplyToClient("%s",sbuf);
}

/*=========================================================
������dsTaskWasteGoods   TASK_WASTE_GOODS
���ܣ����ﱨ��
������sLsh:��ˮ��
      sCzydh:����Ա����
      sHwbz:�����־
      nCount:����
���أ�0���ɹ�������ʧ��
      [����RECORD]
      �ɹ���000000|��ˮ��|
      ʧ�ܣ�XXXXXX|ʧ����Ϣ����˵��|
*========================================================*/
long dsTaskWasteGoods(char * sLsh,char * sCzydh,char * sHwbz,long nCount)
{
     char sJym[16],sbuf[81],sJymc[51]=".";
     char sJsdh[10],cLxbz[21]="00000000000000000000";
     long identify=0,nRetVal;
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     RunSelect("select to_number(substr(clxbz,5,1)),cjymc from dl$jym \
                where cjym=%s into %ld%s",sJym,&identify,sJymc);
     nRetVal=1;
     if(identify)
     {
     	   nRetVal=RunSql("update dl$hwjcb set njlzt=3 where cjym=%s and cjhczydh=%s \
                     and chwbz=%s and ((nczlb=1 and njlzt=1) or (nczlb=3 and njlzt=2))",
                     sJym,sCzydh,sHwbz);
           cLxbz[0]='1';
     }
        

     if(nRetVal>0)
         nRetVal=RunSql("insert into dl$hwjcb(cjym,cjymc,clsh,djhdj,nsl,chwbz,\
                        cfhczydh,cjhczydh,cfhjh,cjhjh,njhrq,njhsfm,nfhrq,nfhsfm,\
                        nczlb,njlzt,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,clxbz) \
                        values(%s,%s,%s,0,%ld,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,\
                        6,1,0,0,0,0,'0','0',%s)",
                        sJym,sJymc,sLsh,nCount,sHwbz,sCzydh,"NONE",sJsdh,
                        ".",nSysLkrq,nSysSfm,nSysLkrq,nSysSfm,cLxbz);
     
     if(nRetVal>0) nRetVal=ProcessRecord(sLsh);
     
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sJymc);
     	RollbackWork();
     	sprintf(sbuf,"000011|%s|",sJymc);
     	return ApplyToClient("%s",sbuf);
     }
     
     CommitWork();
     sprintf(sbuf,"000000|%s|",sLsh);
     return ApplyToClient("%s",sbuf);
}

/*=========================================================
������dsTaskReturnGoods   TASK_RETURN_GOODS
���ܣ���ί�з��˻�
������sLsh:��ˮ��
      sCzydh:����Ա����
      sHwbz:�����־
      nCount:����
���أ�0���ɹ�������ʧ��
      [����RECORD]
      �ɹ���000000|��ˮ��|
      ʧ�ܣ�XXXXXX|ʧ����Ϣ����˵��|
*========================================================*/
long dsTaskReturnGoods(char * sLsh,char * sCzydh,char * sHwbz,long nCount)
{
     char sJym[16],sbuf[81],sJymc[51]=".";
     char sJsdh[10],cLxbz[21]="00000000000000000000";
     long identify=0,nRetVal;
     
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
     RunSelect("select to_number(substr(clxbz,5,1)),cjymc from dl$jym \
                where cjym=%s into %ld%s",sJym,&identify,sJymc);
     nRetVal=1;
     if(identify)
     {
     	   nRetVal=RunSql("update dl$hwjcb set njlzt=3 where cjym=%s and cjhczydh=%s \
                     and chwbz=%s and ((nczlb=1 and njlzt=1) or (nczlb=3 and njlzt=2))",
                     sJym,sCzydh,sHwbz);
           cLxbz[0]='1';
     }
        

     if(nRetVal>0)
         nRetVal=RunSql("insert into dl$hwjcb(cjym,cjymc,clsh,djhdj,nsl,chwbz,\
                        cfhczydh,cjhczydh,cfhjh,cjhjh,njhrq,njhsfm,nfhrq,nfhsfm,\
                        nczlb,njlzt,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,clxbz) \
                        values(%s,%s,%s,0,%ld,%s,%s,%s,%s,%s,%ld,%ld,%ld,%ld,\
                        2,1,0,0,0,0,'0','0',%s)",
                        sJym,sJymc,sLsh,nCount,sHwbz,sCzydh,"NONE",sJsdh,
                        ".",nSysLkrq,nSysSfm,nSysLkrq,nSysSfm,cLxbz);
     
     if(nRetVal>0) nRetVal=ProcessRecord(sLsh);
     
     if(nRetVal<=0)
     {
     	GetSqlErrInfo(sJymc);
     	RollbackWork();
     	sprintf(sbuf,"000011|%s|",sJymc);
     	return ApplyToClient("%s",sbuf);
     }
     
     CommitWork();
     sprintf(sbuf,"000000|%s|",sLsh);
     return ApplyToClient("%s",sbuf);
}


