#include"dlywd.h"

extern long nSysLkrq;
//extern long nSysSfm;

/*============================================================*
�������ɷѲ�ѯ����˳���TASK_PAY_CONFIRM
���ܣ������û��Ľɷ�ȡ�Ľ��ɷ�ȷ������,�����ɷѳɹ���ʧ�ܽ�����ظ��ͻ���
������inbuf:��������ַ��������ĸ�ʽ���£��ڸ��ֶ��á�|��������
      
��ˮ��|�û���־|�û�����|�ɿʽ|ʵ���ܿ��|����Ա����|ʱ����|�ɿ��·���{|���|�·�|���|�·�|......}
char(40) char     char    LONG(2) FLOAT(10.2) char(4)   LONG(8)  LONG(2)

ע�����ɿ��·���Ϊ0�����ʾ�����·ݶ��ɣ�����Ϊ����ֻ���������Ⱥ��·ݵķ���
���أ�>0�ɹ� 
     [����RECORD]
           �ɹ���000000|��ˮ��|ʵ�ɿ��|
           ʧ�ܣ�XXXXXX|������Ϣ|         

*============================================================*/
long dsTaskPayConfirm(char * inbuf)
{
     char sLsh[41];  //��ˮ��
     char outbuf[2048];
     char sSqlErr[81];
     char sJsdh[10];     
     char sJym[16];
     char sYhbz[101]; //�û���־
     char sYhmc[51]; //�û�����
     char sCzydh[7]; //����Ա����
     long nMode;     //�ɿʽ 1���ֽ�  2���̿�
     long nCount,limited;
     long nCxms;
     double dSjk;  //ʵ�ɿӦ�ɿ�
     long nSfm=0;


     GetSysLkrq();
     
     FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|%d|",
                       sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSfm,&nCount);
     //��Ͻ�����
     FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);

     if(!RunSelect("select ncxms from dl$jym where cjym=%s into %ld",
                                                                sJym,&nCxms))
     {
     	ApplyToClient("%s","000002|����Դ�޴˽���|");
        return 0;
     }
     
     if(RunSelect("select ROWNUM from dl$zwmx where clsh=%s",sLsh)>0)
     {
     	ApplyToClient("%s","000003|��ˮ���ظ�|");
        return 0;
     }	     
     
    /////////////////// ���Ǽ��з�ʽ��Ԥ���  //////////////////////////////////
     if(nCount>=0 && 
        RunSelect("select ROWNUM from dl$ywmx_%t where clsh=%s",sJym,sLsh)<=0)
     {
        switch(nCxms)
        {
            //����
            case 130:
            case 110:
            case 140:
            case 141:
            case 610:
            case 210:
            case 220:
            case 230:
            case 821:
            case 521:
            case 522:
            case 123:
            	 ApplyToClient("%s","000010|����Դ�޴���ˮ��|");
                 return 0;
                        
            case 111:
                 
                 limited=0;
                 RunSelect("select to_number(ccs) from dl$jymcsfb "
                 "where nbh=1 and cjym=%s into %ld",sJym,&limited);
                 
                 //���ƺ���
                 if(limited==1)
                   if(RunSelect("select * from dl$jymcsfb where cjym=%s "
                      "and nbh between 2 and 32 and %s like ccs",sJym,sYhbz)<=0)
                     	return ApplyToClient("%s","000017|��������ĺ��벻��Ԥ����Χ��...|");

                 if(InserBlankYwmx(inbuf)<0)
                 {
               	       GetSqlErrInfo(sSqlErr);
               	       RollbackWork();
               	       sprintf(outbuf,"000011|����Դ���ݿ��������:%s|",sSqlErr);
               	       ApplyToClient("%s",outbuf);
                       return 0;
                 }	
                 break;

        }    
      }
      
      /////////////////////        ����ɿ�׶�      /////////////////////////
      if(nCount<0) nCxms=0; //���ɿ��·���С��0��Ϊ���ʼ�¼
      memset(outbuf,0,sizeof(outbuf));
      switch(nCxms)
      {
      	 //�ƶ�
      	 case 130:
      	      dsTaskMobilePayConfirm((char *)inbuf,outbuf);
      	      break;
      	 
      	 //���з�ʽ     
      	 case 110:
      	 case 111:
      	 case 610:
      	 case 521:
      	 case 0:
      	      dsTaskGeneralPayConfirm(inbuf,outbuf);
      	      break;

         case 140:
              dsTaskTelcomPayConfirm(inbuf,outbuf);
              break;        
         case 141:
              //dsTaskYaTelPayConfirm(inbuf,outbuf);
              break;        
         //Ѱ��
         case 210:
         case 220:
         case 230:
              dsTaskBpUserPayConfirm(inbuf,outbuf);
              break;
         //����
       	 case 821:
       	      dsTaskBxUserPayConfirm(inbuf,outbuf);
              break;
         case 123:
              dsTaskPowerPayConfirm(inbuf,outbuf);
              break;
         case 522:
              dsTaskTaxPayConfirm(inbuf,outbuf);
              break;
         
         default:
              sprintf(outbuf,"000016|�޷�����Ľ���ģʽ|");
              break;
 
      }

      if(strncmp(outbuf,"111111",6)==0)
      {
      	  RunSql("update dl$zwmx set nxzbz=0 where clsh=%s",sLsh);
          CommitWork();
      }    
      return ApplyToClient("%s",outbuf);
}

/*============================================================
������dsTaskGeneralPayConfirm
���ܣ�һ�㼯��ʽ�ɷ�����
������inbuf:�ɿ������������
      outbuf���ɷѽ�������ַ���
���أ�0�ɹ�������ʧ��
*============================================================*/

long dsTaskGeneralPayConfirm(char *inbuf,char *outbuf)
{
      
     char sLsh[41];  //��ˮ��
     char sYhbz[101]; //�û���־
     char sJym[16];  //������
     char sYhmc[51]; //�û�����
     char sCzydh[7]; //����Ա����
     char sJsdh[10]; //�ɷѾ־ֺ�
     char sBuffer[51];
     char * sOffset;
     long nMode;     //�ɿʽ 1���ֽ�  2���̿�
     long nCount,nYsnd,nYsyf,nxh,nId;
     double dSjk,dYjzk,dSumSjk;  //ʵ�ɿӦ�ɿ�
     long nSfm=0;
     long nCxms=0;
     char sDump[20]=".",sFph[13]=".";

     sOffset=FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|%d|",
                       sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSfm,&nCount);
     
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);//����ģʽ
	 
	 if(nCxms==521) nCount=0;
     
     if(sJym[0]=='0'&&sJym[1]=='1'&&nCount>=0)
     {
        if(RunSelect("select ROWNUM from dl$ywmx_%t where nxzbz=0 and clsh=%s",sJym,sLsh)<=0)
        {
     	   strcpy(outbuf,"000002|�ɷ���ˮ�Ų����ڻ���û��ѽɷ�|");
     	   return -1;
        }
     
     ////////////  ��Ϊѡ���Խɷ���ֻ�޸���Ӧҵ����ϸ��¼    ////////
        if(nCount>0)  //ѡ���Խɷ�
        {
           while(nCount--)
           {

              sOffset=FetchStringToArg(sOffset,"%d|%d|",&nYsnd,&nYsyf);
              if(RunSql("update dl$ywmx_%t set nxzbz=1,dsjk=%lf,nlkrq=%ld,\
                         nsfm=%ld,cczydh=%s where clsh=%s and nysnd=%ld and \
                         nysyf=%ld",sJym,dSjk,nSysLkrq,
                         nSfm,sCzydh,sLsh,nYsnd,nYsyf)<0)
              {
              	
              	    GetSqlErrInfo(sYhbz);
              	    sprintf(outbuf,"000011|�޸�ҵ������%s|",sYhbz);
              	    return -2;
              }
           }
           if(RunSql("update dl$ywmx_%t set nxzbz=0,clsh='0' \
                      where clsh=%s and nxzbz=0",sJym,sLsh)<0)
           {
              	    GetSqlErrInfo(sYhbz);
              	    sprintf(outbuf,"000011|�޸�ҵ������%s|",sYhbz);
              	    return -2;
           }
        }
        else        //�����·�ȫ���ɷ�
        {
           if(RunSql("update dl$ywmx_%t set nxzbz=1,dsjk=%lf,nlkrq=%ld,\
                      nsfm=%ld,cczydh=%s where clsh=%s",
                      sJym,dSjk,nSysLkrq,nSfm,sCzydh,sLsh)<0)
           {
             	
            GetSqlErrInfo(sYhbz);
            sprintf(outbuf,"000011|�޸�ҵ������%s|",sYhbz);
            return -2;
           }
        }       	
      
        /////////////  ��֤ʵ�ɿ��Ƿ���ȷ   ////////////////     
     
        dYjzk=-1.0;
        RunSelect("select sum(dyjzk),count(*) from dl$ywmx_%t where clsh=%s \
               and nxzbz=1 into %lf%ld",sJym,sLsh,&dYjzk,&nCount);
                
        if(dYjzk<0.0)
        {
           GetSqlErrInfo(sYhbz);
           RollbackWork();
           sprintf(outbuf,"000011|�����֤����%s|",sYhbz);
           return -2;
        }
     
        if(dSjk<dYjzk)
        {
           RollbackWork();
           sprintf(outbuf,"000012|�ɷѽ���|");
           return -2;
        }

     ///////////////  �����ֻһ���¼�¼�������޸�ҵ����ϸʵ�ɿ��¼   //////////
        if(nCount>1)
        {
           dSumSjk=dSjk;
           nId=OpenCursor("select distinct nysnd,nysyf,dyjzk from dl$ywmx_%t where clsh=%s \
                        and  nxzbz=1 order by nysnd,nysyf",sJym,sLsh);
           if(nId>0)
           {
             nxh=1;
             for(;;)
             {
                FetchCursor(nId,"%ld%ld%lf",&nYsnd,&nYsyf,&dYjzk);
             	if(nxh++>=nCount)  //���һ��
             	{
             		RunSql("update dl$ywmx_%t set dsjk=%lf where clsh=%s and nysnd=%ld \
             		        and nysyf=%ld",sJym,dSumSjk,sLsh,nYsnd,nYsyf);
                        break;
                }
                else
                {
             		RunSql("update dl$ywmx_%t set dsjk=dyjzk where clsh=%s and nysnd=%ld \
             		        and nysyf=%ld",sJym,sLsh,nYsnd,nYsyf);
             		dSumSjk-=dYjzk;
             	}
             }//end for
           }
        }
     }//end if
     //////////////////  ����������ϸ��  //////////////////////
     GenerateLsh(3,sJsdh,sJym,sBuffer);
     nxh=atol(sBuffer);

     if(InsertZwmxTable(inbuf,TASK_PAY_CONFIRM,nxh)<0)
     {
        GetSqlErrInfo(sYhbz);
        RollbackWork();
        sprintf(outbuf,"000011|����������ϸ�����%s|",sYhbz);
        return -3;
     }
     
     //��ҵ��Ҫ���ݸ���Ӧ���ֶ�ֵ��
     
     alltrim(sFph);
     switch(nCxms)
     {
     	case 521:
     	  nId=OpenCursor("select cyhbz1 from dl$ywmx_%t where clsh=%s ",sJym,sLsh);
    	  if(nId<=0) { CloseCursor(nId); break;}
    	  
    	  for(;;)
		  {
			sDump[0]='\0';
			if(FetchCursor(nId,"%s",sDump)<=0)break;
    	
			sFph[0]='\0';
    		sOffset=FetchStringToArg(sOffset,"%s|",sFph);
     	  
     	    RunSql("update dl$ywmx_%t set cczydh=%s,dsjk=dsfmx5,cbl8=%s,cjh=%s where clsh=%s and cyhbz1=%s",
        	       sJym,sCzydh,sFph,sJsdh,sLsh,sDump);
          }
          CloseCursor(nId);
           
        break;
        
        default:
        RunSql("update dl$ywmx_%t set cczydh=%s,cjh=%s where clsh=%s",sJym,sCzydh,sJsdh,sLsh);
        break;
     }
     CommitWork();   //�ύ����
     sprintf(outbuf,"000000|%s|%.2lf|",sLsh,dSjk);
     return 0;

}
/*===============================================================
������InsertZwmxTable
���ܣ�ͳһ����������ϸ��
������inbuf:�ɿ����ȷ��ģ��[ͬdsTaskPayConfirm�����Ķ���]��

��ˮ��|�û���־|�û�����|�ɿʽ|ʵ���ܿ��|����Ա����|ʱ����|�ɿ��·���|���|�·�|���|�·�|
char(40) char     char    LONG(2) FLOAT(10.2) char(4)   LONG(8)  LONG(2)

      nmode:����һ�ַ�ʽ���磺
            TASK_PAY_CONFIRM:һ�㼯��ʽ�շѲ���
            TASK_MOBILE_PAY_CONFIRM:�ƶ�����������ϸ
            TASK_RECALL_CONFIRM:���볷����¼
      nxh:��ί�з���������ˮ��[��GenerateLsh����]            
���أ�>0�ɹ�������ʧ��            
*===============================================================*/
//����������ϸ��¼
long InsertZwmxTable(char * inbuf,long nmode,long nxh)
{
     char sLsh[41],sRecallLsh[41];  //��ˮ��
     char sYhbz[101]; //�û���־
     char sJym[16];  //������
     char sYhmc[51]; //�û�����
     char sCzydh[7]; //����Ա����
     char sJsdh[10]; //�ɷѾ־ֺ�
     char sLxbz[21];
     double dSjkxj=0.0,dSjkcz=0.0,dSjkxyk=0.0; //ʵ�ɿ��ֽ𡢴��ۡ����ÿ�
     long   nFkfs=1,nCxms;   //���ɷ�ʽ1���ֽ�  2���̿�
     long nCount,nCsbz=0,nXzbz=2,nYsnd,nYsyf;
     double dSjk;  //ʵ�ɿӦ�ɿ�
     long nSfm=0;

     FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|%d|",
                       sLsh,sYhbz,sYhmc,&nFkfs,&dSjk,sCzydh,&nSfm,&nCount);
     strcpy(sRecallLsh,sLsh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     
     if(sJym[1]!='2')  nCount=1;
     RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);
     switch(nCxms)
     {
         case 130:
         case 140:
         case 141:
         case 210:
         case 220:
         case 230:
         case 821:
         case 1001:
         case 123:
         case 522:
              nCsbz=1;
              nXzbz=2;
              break;
         default:
              nCsbz=0;
              nXzbz=2;
	      break;
     }
     switch(nmode)
     {
     	
     	case TASK_PAY_CONFIRM:
     	case TASK_POWER_PAY_CONFIRM:
     	case TASK_MOBILE_PAY_CONFIRM:
             strcpy(sLxbz,"00000000000000000000");
             break;
        case TASK_RECALL_CONFIRM:
             dSjk=-dSjk;
             nCount=-nCount;
             strcpy(sLxbz,"00000000000000000010"); 
             strcpy(sRecallLsh,sYhmc);
             strcpy(sYhmc,".");
             break;
        default:
             strcpy(sLxbz,"00000000000000000000");
             break;

     }
     
     switch(nFkfs)  //���ݸ��ʽ�ж����ֽ�ɿ�����ÿ�
     {
     	case 1:
     	     dSjkxj=dSjk;
     	     break;
        case 2:
             dSjkcz=dSjk;
             break;
     }
     
        
     nYsnd=nSysLkrq/10000;
     nYsyf=(nSysLkrq%10000)/100;
     return RunSql("insert into dl$zwmx(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,nywlb,nxh,nsl,"
     				"dyjzk,dxzyjk,dsjk,ddlsxf,dybk,dytk,dsjkxj,dsjkcz,dsjkxyk,"
     				"nsfkfs,cjkdh,cjh,cczydh,nlkrq,nsfm,clxbz,"
     				"nxzbz,ncsbz,ncscs,njsbz,cwbjsdh,"
                    "cnbjsdh,cbl,nbl,cbl1,cbl2,cbl3,cbl4,cbl5,clsh,cylsh) "
                    "VALUES(%s,%ld,%ld,%s,%s,'0',%s,0,%ld,%ld,"
                    "%lf,0,%lf,%lf,0,0,%lf,%lf,%lf,"
                    "%ld,'000000000000000',%s,%s,%ld,%ld,%s,"
                    "%ld,%ld,0,0,'00000000000000000000',"
                    "'00000000000000000000','0',0,'0','0','0','0','0',%s,%s)",
                    sJym,nYsnd,nYsyf,sYhbz,sYhbz,sYhmc,nxh,nCount,
                    dSjk,dSjk,0.0,dSjkxj,dSjkcz,dSjkxyk,
                    nFkfs,sJsdh,sCzydh,nSysLkrq,nSfm,sLxbz,
                    nXzbz,nCsbz,sLsh,sRecallLsh);

}

/*===============================================================
������InserBlankYwmx
���ܣ��ڼ��з�ʽ��Ԥ���ɷ�ʱ����һ��ҵ����ϸ����
������inbuf:�ɿ���������ַ������䶨����μ�����dsTaskPayConfirm
*===============================================================*/

//����հ�ҵ����ϸ��¼
static long InserBlankYwmx(char * inbuf)
{
     char sBuffer[51];
     char sLsh[41];
     char sYhbz[41]; //�û���־
     char sJym[16];  //������
     char sYhmc[51]; //�û�����
     char sCzydh[7];
     char sJsdh[10];
     long nMode;     //�ɿʽ 1���ֽ�  2���̿�
     double dSjk;
     long nxh,nYsnd,nYsyf;
     long nSfm=0;

     FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|",sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSfm);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     
     //����������������ϸ���
     GenerateLsh(3,sJsdh,sJym,sBuffer);
     nxh=atol(sBuffer);

     nYsnd=nSysLkrq/10000;
     nYsyf=(nSysLkrq%10000)/100;
        
        
     return  RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,\
                        cyhbz1,cyhbz2,cyhmc,nywlb,nxh,\
                        dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,\
                        dsfmx8,dsfmx9,dsfmx10,\
                        dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,\
                        dsfmx17,dsfmx18,dsfmx19,dsfmx20,\
                        dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,\
                        njscz2,djscz3,djscz4,nsl,dyjzk,\
                        dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,\
                        dbl1,dbl2,cbl1,cbl2,cjh,cczydh,nlkrq,nsfm,clxbz,\
                        nxzbz,ncsbz,cbz,cbl3,cbl4,cbl5,cbl6,cbl7,clsh,cylsh) \
             VALUES(%s,%ld,%ld,%s,%s,'0',%s,0,%ld,0,0,0,0,0,0,0,0,0,0,\
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0',0,0,0,0,0,0,0,%lf,0,\
                    1,1,0,0,0,0,'0','0',%s,%s,%ld,%ld,'0',\
                    0,0,'0','0','0','0','0','0',%s,'0')",
                    sJym,sJym,nYsnd,nYsyf,sYhbz,sYhbz,sYhmc,
                    nxh,dSjk,sJsdh,sCzydh,nSysLkrq,nSfm,sLsh);

}


