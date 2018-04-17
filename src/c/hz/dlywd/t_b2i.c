//���ٱ����շ�ģ��

#include "dlywd.h"

extern long nSysLkrq;
extern long nSysSfm;

//���ٱ��ղ�ѯ���񣬲����뿴t_query.c
long dsTaskBxUserQuery(char * inbuf,RECORD * Client)
{
    struct Ctl_Msg rc;
    tongXin_Struct rp;
    char sLsh[41],sWt[6],sYhbz[23],sCzydh[7];
    char sUser[10],sTemp[50]=".";
    char sJym[16],sJsdh[10];
    long nFlag,nXh;
    char sBuffer[1024];
    char sErrorInfo[200];
    double dsfmx[25];
    long   nValue[10],nYear,nMonth,nDay;
    long   nXs,nFz,nMz;
    
    memset(&rc,0,CTL_LEN);
    memset(&rp,0,DATA_LEN); 
    memset(dsfmx,0,sizeof(dsfmx));
    memset(nValue,0,sizeof(nValue));
     
    FetchStringToArg(inbuf,"%s|%s|%s|%s|",sLsh,sWt,sYhbz,sCzydh);
    FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
    alltrim(sYhbz);
    alltrim(sCzydh);

    //��֯��ѯ��������
    strcpy(rc.TransCode,"500101");
    //rc.CtlChar.flags.morepkt=1;
    sprintf(rc.MsgLength,"%ld",DATA_LEN);
   
    /*******************/    
    strcpy(rp.jiaoYiLeiXing,"A1");
    if(RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=1 into %s",
			sJym,rp.yinHangDaiMa)<0)
	printf("Dead");
    strcpy(rp.wangDianDaiMa,sJsdh);
    strcpy(rp.baoDanHao,sYhbz);     //������
    strcpy(rp.caoZuoYuanDaiMa,sCzydh);
    RunSelect("SELECT cczymc FROM dl$czyb WHERE cczydh=%s into %s ",
              sCzydh,rp.caoZuoYuan);
    
    getFlData(nSysLkrq,&nYear,&nMonth,&nDay);
    getFlData(nSysSfm,&nXs,&nFz,&nMz);

    sprintf(rp.shouFeiShiJian,"%ld-%02ld-%02ld %02ld:%02ld:%02ld",nYear,nMonth,nDay,nXs,nFz,nMz);
    if(dsBxSendAndRecv(&rc,&rp))
    {
          getBxErrorInfo(rc,sErrorInfo);
          ImportRecord(Client,"%s%s",rc.status,sErrorInfo);
          return 0;
    }
    
    switch(rp.chaXunJieGuo)
    {
   	case '0':
   	case '4':
                GenerateLsh(3,sJsdh,sJym,sTemp);
                nXh=atol(sTemp);

  		strcpy(rp.chongZhenShiJian,".");
		if(rp.chaXunJieGuo=='0')
			strcpy(sUser,"���ձ���");
		else
		{
		    strcpy(sUser,"�������");
		    strcpy(rp.yingShouFeiRiQi,".");
		}		
		if(rp.jiaoFeiFangShi=='M')
			  strcpy(sTemp,"�½�");
		else if(rp.jiaoFeiFangShi=='Y')
			  strcpy(sTemp,"�꽻");
		else if(rp.jiaoFeiFangShi=='H')
			  strcpy(sTemp,"���꽻");
		else if(rp.jiaoFeiFangShi=='Q')
			  strcpy(sTemp,"����"); 
		else strcpy(sTemp,"�ұ���");
			             
                nFlag=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,\
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
                 VALUES(%s,%ld,%ld,%s,%s,%s,%s,0,%ld,%lf,%lf,%lf,%lf,0,0,0,\
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'0',0,0,0,0,0,\
                        0,0,0,0,0,%ld,%ld,%ld,0,0,%s,%s,%s,%s,%ld,%ld,'0',\
                        0,0,'0',%s,%s,%s,%s,%s,%s,'0')",
                  sJym,sJym,nYear,nMonth,sYhbz,
                  rp.yingShouFeiRiQi,rp.baoXianMingChen,rp.touBaoRen,nXh,
                  rp.baoFei,rp.liXi,rp.yuJiaoFeiYuE,rp.yuJiaoBaoFei,
                  rp.jiaoFeiNianQi,rp.baoDanNianDu,rp.jiaoFeiCiShu,
                  rp.yeWuYuanDaiMa,rp.yeWuYuan,sJsdh,sCzydh,nSysLkrq,nSysSfm,
                  sTemp,rp.chongZhenShiJian,rp.shengXiaoRiQi,
                  sUser,rp.shouFeiShiJian,sLsh);
        
	          if(nFlag<0)
                  {
                     RollbackWork();
        	     GetSqlErrInfo(sTemp);
        	     sprintf(sBuffer,"����ҵ����ϸ����%s",sTemp);
        	     ImportRecord(Client,"%s%s","000011",sBuffer);
        	     return 0;
                  }
                  CommitWork();
                  
                  dsfmx[0]=rp.yuJiaoBaoFei; dsfmx[1]=rp.yuJiaoFeiYuE;
                  nValue[0]=rp.jiaoFeiCiShu; nValue[1]=rp.jiaoFeiNianQi;

   	      ImportRecord(Client,"%s%s%s%lf%ld%s%ld(2)%lf%lf[25]%ld[4]%s(4)","000000",rp.touBaoRen,
    	          rp.baoXianMingChen,rp.baoFei,1,sYhbz,
    	          nYear,nMonth,0.0,dsfmx,nValue,
    	          rp.yeWuYuan,sTemp,rp.shengXiaoRiQi,rp.yingShouFeiRiQi);
    	             
             break;
        case '1':
              ImportRecord(Client,"%s%s","000001","�Ƿ��ı�����");
              break;
        case '2':
              ImportRecord(Client,"%s%s","000002","����������");
              break;
        case '3':
              ImportRecord(Client,"%s%s","000003","����ʧЧ������");
              break;
	
        case '5':
              ImportRecord(Client,"%s%s","000005","����δ����");
              break;
        case '6':
              ImportRecord(Client,"%s%s","000006","�����ղ���Ԥ����");
              break;
        default:
              return 0;
    }//end swic
    return 0;            
}

//�ɷ�ȷ�ϣ�������μ�t_pay.c
long dsTaskBxUserPayConfirm(char *inbuf,char *outbuf)
{
    struct Ctl_Msg rc;
    tongXin_Struct rp;
    char sLsh[41];  //��ˮ��
    char sYhbz[23]; //�û���־
    char sJym[16];  //������
    char sYhmc[65]; //�û�����
    char sCzydh[7]; //����Ա����
    char sJsdh[10]; //�ɷѾ־ֺ�
    char sDate[11],sBuffer[1024]=".";
    char * sOffset;
    long nMode,nFlag,nCxms;     //�ɿʽ 1���ֽ�  2���̿�
    long nCount,nxh,nId,nRetVal,nFs;
    char cFs[10],cJg[10],sSjh[13]=".",sData[10]=".";
    long nNq,nCs,nNd;
    double dSjk;
    long nSfm=0;

    strcpy(sBuffer,inbuf);
    FetchStringToArg(sBuffer,"%s|%s|%s|%d|%f|%s|%d|%d|%s|%s|",
                       sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSfm,&nCount,sData,sSjh);
    FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);

    printf("\nTEST:(%s)",sLsh);
    GenerateLsh(3,sJsdh,sJym,sBuffer);
    nxh=atol(sBuffer);
    alltrim(sYhbz);
    alltrim(sYhmc);
    alltrim(sCzydh);
     
    memset(&rc,0,CTL_LEN);
    memset(&rp,0,DATA_LEN);
    
    //��֯�ɿ������
    strcpy(rc.TransCode,"500201");
    sprintf(rc.MsgLength,"%ld",DATA_LEN);
    /*******************/    
    strcpy(rp.jiaoYiLeiXing,"B1");
    RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=1 into %s",
			sJym,rp.yinHangDaiMa);
    strcpy(rp.wangDianDaiMa,sJsdh);
    strcpy(rp.baoDanHao,sYhbz);     //������
    strcpy(rp.caoZuoYuanDaiMa,sCzydh);
    rp.baoFei=dSjk;
    RunSelect("SELECT cczymc FROM dl$czyb WHERE cczydh=%s into %s ",
              sCzydh,rp.caoZuoYuan);
    
    printf("\nTEST:baoFei=%lf",rp.baoFei);
    nId=RunSelect("select to_char(nxh),cyhbz1,cyhbz2,cyhmc,\
			dsfmx2,dsfmx3,dsfmx4,\
                        nsfkfs,nbl1,nbl2,cbl1,cbl2,\
                        nlkrq,nsfm,cbl3,cbl4,cbl5,cbl6,cbl7 \
                        from dl$ywmx_%t where clsh=%s into \
                        %s%s%s%s,%lf%lf%lf,\
                        %ld%ld%ld,%s%s,%ld%ld,%s%s%s%s%s",
    		  sJym,sLsh,
    		  rp.bank_no,rp.yingShouFeiRiQi,rp.baoXianMingChen,rp.touBaoRen,
                  &rp.liXi,&rp.yuJiaoFeiYuE,&rp.yuJiaoBaoFei,
		  &nNq,&nNd,&nCs,
                  rp.yeWuYuanDaiMa,rp.yeWuYuan,
                  &nSysLkrq,&nSfm,
                  cFs,rp.chongZhenShiJian,rp.shengXiaoRiQi,
                  cJg,rp.shouFeiShiJian);
    
    rp.jiaoFeiNianQi=nNq;
    rp.baoDanNianDu=nNd;
    rp.jiaoFeiCiShu=nCs;
    alltrim(cFs);
    rp.jiaoFeiFangShi=cFs[0];
    if(rp.chongZhenShiJian[0]=='.') rp.chongZhenShiJian[0]='\0';
    if(rp.yingShouFeiRiQi[0]=='.') rp.yingShouFeiRiQi[0]='\0';
    		if(!strcmp(cFs,"�½�"))
    			rp.jiaoFeiFangShi='M';
		else if(!strcmp(cFs,"�꽻"))
    			rp.jiaoFeiFangShi='Y';
		else if(!strcmp(cFs,"���꽻"))
			rp.jiaoFeiFangShi='H';
		else if(!strcmp(cFs,"����"))
			rp.jiaoFeiFangShi='Q';
			   
    nMode-=1;
    sprintf(cFs,"%1ld",nMode);
    rp.shouFeiFangShi=cFs[0];
    if(nMode!=2) rp.zhiPiaoHao[0]='\0';
    strcpy(rp.shouJuYinShuaHao,sSjh);
    
    if(!strcmp(cJg,"���ձ���"))
    	rp.chaXunJieGuo='0';
    else if(!strcmp(cJg,"�������"))
    	rp.chaXunJieGuo='1';
    else rp.chaXunJieGuo='2';
    printf("\nTEST:nId=%d|%c|%c|%c|",nId,rp.jiaoFeiFangShi,rp.shouFeiFangShi,rp.chaXunJieGuo);
        			    
    if(dsBxSendAndRecv(&rc,&rp))
    {
        getBxErrorInfo(rc,outbuf);
    	return 0;
    }
 
    if(!strcmp(rc.status,"0000"))
    {
	nMode+=1;
        sprintf(sBuffer,"%-40.40s|%s|%s|%ld|%.2lf|%s|%ld|0|",
                        sLsh,sYhbz,sYhmc,nMode,dSjk,sCzydh,nSfm);

        //sBuffer=(��ˮ��,����,ʱ����,����ȵ�)
        if(InsertZwmxTable(sBuffer,TASK_PAY_CONFIRM,nxh)<0)
        {
           GetSqlErrInfo(sYhbz);
           RollbackWork();
           sprintf(outbuf,"000011|����������ϸ�����%s|",sYhbz);
           return -3;
        }
        
        sprintf(sDate,"%04ld/%02ld/%02ld",nSysLkrq/10000,(nSysLkrq%10000)/100,nSysLkrq%100);
        RunSql("update dl$ywmx_%t set dyjzk=%lf,dsjk=%lf where clsh=%s",
               sJym,dSjk,dSjk,sLsh);
    
        RunSql("update dl$zwmx set dsjk=%lf,cbl1=%s,cbl2=%s,cbl3=%s \
		where clsh=%s",
        	dSjk,sSjh,sDate,rp.shouFeiShiJian,sLsh);
        CommitWork();
        sprintf(outbuf,"000000|%s|%.2lf|",sLsh,dSjk);
	printf("\n......<%s>",outbuf);
    }//end success
    else getBxErrorInfo(rc,outbuf);
    
    return 0;            
}

//���ճ�������
long dsTaskBxRecallConfirm(char *inbuf,long nxh,char *outbuf)
{
     struct Ctl_Msg rc;
     tongXin_Struct rp;
     char sLsh[41],sRecallLsh[41],sDate[11];  //��ˮ��
     char sYhbz[101]; //�û���־
     char sJym[16];  //������
     char sCzydh[7]; //����Ա����
     char sJsdh[10]; //�ɷѾ־ֺ�
     long nRecallXh,nFlag,nId;
     long nNd,nNq,nCs,nMode;
     char cFs[10]=".",cJg[10]=".",sSjh[13]=".";
     long   nXs,nFz,nMz,nYear,nMonth,nDay;
     double dSjk=0;
         
     memset(&rc,0,CTL_LEN);
     memset(&rp,0,DATA_LEN);
 
     FetchStringToArg(inbuf,"%s|%s|",sLsh,sRecallLsh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
     RunSelect("select nxh,cyhbz,cczydh,dsjk,cbl1,nsfkfs \
		from dl$zwmx where cjym=%s and \
                clsh=%s into %ld%s%s%lf%s%ld",
                sJym,sRecallLsh,&nRecallXh,sYhbz,sCzydh,&dSjk,sSjh,&nMode);

      
    //��֯����
    strcpy(rc.TransCode,"500501");
    sprintf(rc.MsgLength,"%ld",DATA_LEN);
    /*******************/    
    strcpy(rp.jiaoYiLeiXing,"E1");
    RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=1 into %s",
			sJym,rp.yinHangDaiMa);
    strcpy(rp.wangDianDaiMa,sJsdh);
    strcpy(rp.baoDanHao,sYhbz);     //������
    strcpy(rp.caoZuoYuanDaiMa,sCzydh);
    rp.baoFei=dSjk;
    RunSelect("SELECT cczymc FROM dl$czyb WHERE cczydh=%s into %s ",
             sCzydh,rp.caoZuoYuan);
    
    printf("\nTEST:baoFei=%lf",rp.baoFei);
    nId=RunSelect("select to_char(nxh),cyhbz1,cyhbz2,cyhmc,\
			dsfmx2,dsfmx3,dsfmx4,\
                        nsfkfs,nbl1,nbl2,cbl1,cbl2,\
                        nlkrq,nsfm,cbl3,cbl4,cbl5,cbl6,cbl7 \
                        from dl$ywmx_%t where clsh=%s into \
                        %s%s%s%s,%lf%lf%lf,\
                        %ld%ld%ld,%s%s,%ld%ld,%s%s%s%s%s",
    		  sJym,sRecallLsh,
    		  rp.bank_no,rp.yingShouFeiRiQi,rp.baoXianMingChen,rp.touBaoRen,
                  &rp.liXi,&rp.yuJiaoFeiYuE,&rp.yuJiaoBaoFei,
		  &nNq,&nNd,&nCs,
                  rp.yeWuYuanDaiMa,rp.yeWuYuan,
                  &nSysLkrq,&nSysSfm,
                  cFs,rp.chongZhenShiJian,rp.shengXiaoRiQi,
                  cJg,rp.shouFeiShiJian);
    
    rp.jiaoFeiNianQi=nNq;
    rp.baoDanNianDu=nNd;
    rp.jiaoFeiCiShu=nCs;

    		if(!strcmp(cFs,"�½�"))
    			rp.jiaoFeiFangShi='M';
		else if(!strcmp(cFs,"�꽻"))
    			rp.jiaoFeiFangShi='Y';
		else if(!strcmp(cFs,"���꽻"))
			rp.jiaoFeiFangShi='H';
		else if(!strcmp(cFs,"����"))
			rp.jiaoFeiFangShi='Q';
 
    getFlData(nSysLkrq,&nYear,&nMonth,&nDay);
    getFlData(nSysSfm,&nXs,&nFz,&nMz);
    sprintf(rp.chongZhenShiJian,"%ld-%02ld-%02ld %02ld:%02ld:%02ld",nYear,nMonth,nDay,nXs,nFz,nMz);

    nMode-=1;
    sprintf(cFs,"%1ld",nMode);
    rp.shouFeiFangShi=cFs[0];
    if(nMode!=2) rp.zhiPiaoHao[0]='\0';
    strcpy(rp.shouJuYinShuaHao,sSjh);
    
    if(!strcmp(cJg,"���ձ���"))
    	rp.chaXunJieGuo='0';
    else if(!strcmp(cJg,"�������"))
    {
       	rp.chaXunJieGuo='1';
    	if(rp.yingShouFeiRiQi[0]=='.') rp.yingShouFeiRiQi[0]='\0';
    }
    else rp.chaXunJieGuo='2';
    
    if(dsBxSendAndRecv(&rc,&rp))
    {
 	getBxErrorInfo(rc,outbuf);       
    	return 0;
    }
     
    if(!strcmp(rc.status,"0000"))
      	sprintf(outbuf,"000000|%s|%s|",rp.chongZhenShiJian,sSjh);
    else
     	strcpy(outbuf,"111111|����ʧ��|");
    
    return 0;	
}

//���ղ��򵥾�...
long dsBxRequestRePrint(char *sLsh)
{
    struct Ctl_Msg rc;
    tongXin_Struct rp;
     char sDate[11];  //��ˮ��
     char sYhbz[101]; //�û���־
     char sJym[16];  //������
     char sCzydh[7]; //����Ա����
     char sJsdh[10]; //�ɷѾ־ֺ�
     long nId;
     long nNd,nNq,nCs,nMode;
     char cFs[10]=".",cJg[10]=".",sSjh[13]=".";
     double dSjk=0;
     
    FetchStringToArg(sLsh,"%9s%15s",sJsdh,sJym);
    memset(&rc,0,CTL_LEN);
    memset(&rp,0,DATA_LEN);
    
    RunSelect("select cyhbz,cczydh,dsjk,cbl1,nsfkfs,cbl3 \
		from dl$zwmx where cjym=%s and \
                clsh=%s into %s%s%lf%s%ld%s",
                sJym,sLsh,sYhbz,sCzydh,&dSjk,sSjh,&nMode,rp.chongZhenShiJian);

    strcpy(rc.TransCode,"500301");
    sprintf(rc.MsgLength,"%ld",DATA_LEN);
    /*******************/    
    strcpy(rp.jiaoYiLeiXing,"C1");
    RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=1 into %s",
			sJym,rp.yinHangDaiMa);
    strcpy(rp.wangDianDaiMa,sJsdh);
    strcpy(rp.baoDanHao,sYhbz);     //������
    strcpy(rp.caoZuoYuanDaiMa,sCzydh);
    rp.baoFei=dSjk;
    RunSelect("SELECT cczymc FROM dl$czyb WHERE cczydh=%s into %s ",
             sCzydh,rp.caoZuoYuan);
    
    printf("\nTEST:baoFei=%lf",rp.baoFei);
    nId=RunSelect("select to_char(nxh),cyhbz1,cyhbz2,cyhmc,\
			dsfmx2,dsfmx3,dsfmx4,\
                        nsfkfs,nbl1,nbl2,cbl1,cbl2,\
                        nlkrq,nsfm,cbl3,cbl4,cbl5,cbl6,cbl7 \
                        from dl$ywmx_%t where clsh=%s into \
                        %s%s%s%s,%lf%lf%lf,\
                        %ld%ld%ld,%s%s,%ld%ld,%s%s%s%s%s",
    		  sJym,sLsh,
    		  rp.bank_no,rp.yingShouFeiRiQi,rp.baoXianMingChen,rp.touBaoRen,
                  &rp.liXi,&rp.yuJiaoFeiYuE,&rp.yuJiaoBaoFei,
		  &nNq,&nNd,&nCs,
                  rp.yeWuYuanDaiMa,rp.yeWuYuan,
                  &nSysLkrq,&nSysSfm,
                  cFs,rp.chongZhenShiJian,rp.shengXiaoRiQi,
                  cJg,rp.shouFeiShiJian);
    
    rp.jiaoFeiNianQi=nNq;
    rp.baoDanNianDu=nNd;
    rp.jiaoFeiCiShu=nCs;
       		if(!strcmp(cFs,"�½�"))
    			rp.jiaoFeiFangShi='M';
		else if(!strcmp(cFs,"�꽻"))
    			rp.jiaoFeiFangShi='Y';
		else if(!strcmp(cFs,"���꽻"))
			rp.jiaoFeiFangShi='H';
		else if(!strcmp(cFs,"����"))
			rp.jiaoFeiFangShi='Q';
 
    if(rp.chongZhenShiJian[0]=='.') rp.chongZhenShiJian[0]='\0';
    
    nMode-=1;
    sprintf(cFs,"%1ld",nMode);
    rp.shouFeiFangShi=cFs[0];
    if(nMode!=2) rp.zhiPiaoHao[0]='\0';
    strcpy(rp.shouJuYinShuaHao,sSjh);
    if(!strcmp(cJg,"���ձ���"))
    	rp.chaXunJieGuo='0';
    else if(!strcmp(cJg,"�������"))
    {
       	rp.chaXunJieGuo='1';
    	if(rp.yingShouFeiRiQi[0]=='.') rp.yingShouFeiRiQi[0]='\0';
    }
    else rp.chaXunJieGuo='2';
    
    //�������ݵ�BX_GS: 
    dsBxSendAndRecv(&rc,&rp);
    printf("\n...print success");
    return ApplyToClient("%s","000000|���չ�˾�Ѵ���|");
}

//���ٱ��շ��ͽ������ݰ�
static long dsBxSendAndRecv(struct Ctl_Msg *rc,tongXin_Struct *rp)
{
   int nsock;
   long nLength,nRetVal;
   char outbuf[1000];
   long nLen,nNd,nCs,nNq;
   
   nsock=connectTCP("dl_tx_9","bxuser");
   if(nsock<0) return -1;
   
   //send (CTL)data to dlsrv
   printf("\nTEST:SOCK=%d",nsock);
   if(SendLongToSocket(nsock,CTL_LEN)) 
   {
   	close(nsock);
   	return -2;
   }

   if(SendRecordToSocket(nsock,(char *)rc,CTL_LEN)) 
   {
   	close(nsock);
   	return -2;
   }
   
   //send data to dlsrv
   if(SendLongToSocket(nsock,DATA_LEN)) 
   {
        close(nsock);
        return -2;
   }
   //==================================================
   rp->jiaoFeiNianQi=iIbmOrScoReturn(rp->jiaoFeiNianQi);
   rp->jiaoFeiCiShu=iIbmOrScoReturn(rp->jiaoFeiCiShu);
   rp->baoDanNianDu=iIbmOrScoReturn(rp->baoDanNianDu);
   rp->yuJiaoBaoFei=dIbmOrScoReturn(rp->yuJiaoBaoFei);
   rp->baoFei=dIbmOrScoReturn(rp->baoFei);
   rp->yuJiaoFeiYuE=dIbmOrScoReturn(rp->yuJiaoFeiYuE);
   rp->liXi=dIbmOrScoReturn(rp->liXi);
   //===================================================
   if(SendRecordToSocket(nsock,(char *)rp,DATA_LEN)) 
   {
        close(nsock);
        return -2;
   }
   SendLongToSocket(nsock,0);   //��������־
   
   //get data fro dlsrv
   nRetVal=-1;
   GetLongFromSocket(nsock,&nRetVal);
   if(nRetVal)
   {
   	close(nsock);
   	return -2;
   }

   memset(rc,0,CTL_LEN);
   nLen=GetOneRecordFromSocket(nsock,rc,CTL_LEN);
   printf("\nTEST:recv_ctl_len=%ld",nLen);
   printf("\n---rc.status=%s\n",rc->status);
   
   if(strcmp(rc->TransCode,"500101"))
   {
	close(nsock);
	return 0;
   }
    
   memset(rp,0,DATA_LEN);
   printf("\nTEST:SOCK=%d",nsock);
   nLen=GetOneRecordFromSocket(nsock,rp,DATA_LEN);
   //====================================================================
   GetFromSocket(nsock,"%ld%ld%ld,%lf%lf%lf%lf",
		&nNq,&nCs,&nNd,
                &rp->yuJiaoBaoFei,&rp->baoFei,&rp->yuJiaoFeiYuE,&rp->liXi);

   rp->jiaoFeiNianQi=nNq;
   rp->jiaoFeiCiShu=nCs;
   rp->baoDanNianDu=nNd;
   //=====================================================================
   printf("\nTEST:recv_data_len=%ld",nLen);
   if(nLen!=DATA_LEN)
   {
      close(nsock);
      return -2;
   }
       //�����ǽ��յ������ݣ�д��bxrecv�ļ���
   	/*{
	FILE *fp;
 	fp=fopen("bxrecv","wb");
	fwrite((unsigned char *)rc,CTL_LEN,1,fp);
	fwrite("\n---\n",5,1,fp);
	fwrite((unsigned char *)rp,nLen,1,fp);
	fclose(fp);
   	}*/
   close(nsock);
   printf("\n<SUCCESSFUL> :)");
   return 0;
}

//�õ�������Ϣ
static void getBxErrorInfo(struct Ctl_Msg rc,char *sErrorInfo)
{

     if(!strcmp(rc.status,"0001"))
     	  sprintf(sErrorInfo,"%s","������Ƿ�");
     else if(strstr(rc.status,"900"))
          sprintf(sErrorInfo,"%s","�����쳣");
     else if(!strcmp(rc.status,"5000"))
          sprintf(sErrorInfo,"%s","�Ƿ���������");
     else if(!strcmp(rc.status,"5001"))
          sprintf(sErrorInfo,"%s","����ʧ��");
     else if(!strcmp(rc.status,"5002"))
          sprintf(sErrorInfo,"%s","��Ԥ�ռ�¼,����ʱҪ���ȳ���Ԥ��");
     else
     	  sprintf(sErrorInfo,"%s","�������ѽɷѻ򲻴���");
  
}//end getError

void getFlData(long nData,long *na,long *nb,long *nc)
{
    *na=nData/10000;
    *nb=(nData%10000)/100;
    *nc=(nData%10000)%100;
}//end

//�������񣬲�����μ�t_accnt.c
long dsTaskBxCheckAccount(char * inbuf,char * outbuf)
{
    long nId,nXh,nLkrq,nSfm,nFlag,nRet;
    char bdh[23],yhdm[5],wddm[10],jylx[3],jysj[20],czy[5];
    char sjysh[13],lsh[11];
    char sJsdh[10],sDate[11],sTemp[256]; //�ɷѾ־ֺ�
    char sBuffer[4096];
    char buf[4096];
    double dsjk;
    
    memset(sBuffer,0,sizeof(sBuffer));
    memset(buf,0,sizeof(buf));
           
    nId=OpenCursor("select to_char(nxh),cyhbz,cbl1,cbl3,cczydh,dsjk \
	 	 from dl$zwmx  where cjym=%s and ncsbz=1 ",inbuf);
    if(nId<0)
    {
    	sprintf(outbuf,"000011|����Դ�������ݿ����%s|",inbuf);
    	return 0;
    }
    //|||||||||||||||||||||||||||||
    //strcpy(jylx,"B1");
    RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=1 into %s",
			inbuf,yhdm);
    RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=2 into %s",
    			inbuf,wddm);
    nRet=1;
    for(;;)
    {
        if(FetchCursor(nId,"%s%s%s%s%s%lf",lsh,bdh,sjysh,jysj,czy,&dsjk)<=0)
	   break;

        if(dsjk>0) strcpy(jylx,"B1");
        else
        {
          strcpy(jylx,"E1");
          dsjk=(-1*dsjk);
        }

        sprintf(buf,"%s%s|%s|%s|%s|%s|%s|%lf|%s|%s|",
		buf,bdh,yhdm,wddm,jylx,jysj,czy,dsjk,sjysh,lsh);
        nRet++;
        
    }//end for
    CloseCursor(nId);

    sprintf(sBuffer,"%06ld|%s",nRet,buf);
    if(sBuffer[0]=='\0') 
    {
    	sprintf(outbuf,"000011|�޶�������!|");
    	return 0;
    }
    if(dsBxDzSendAndRecv(sBuffer)!=0)
    {
        sprintf(outbuf,"111111|%s|",sBuffer);
    	return 0;
    }

    {
    RunSelect("select  count(*),sum(dsjk) from dl$zwmx where cjym=%s and "
		"ncsbz=1 into %ld%lf",inbuf,&nId,&dsjk);
    RunSql("update dl$zwmx set ncsbz=2 where cjym=%s and ncsbz=1",inbuf);
    RunSql("update dl$jymcsfb set ccs='%ld' where cjym=%s and nbh=34",
		nSysLkrq,inbuf);
	CommitWork();
    }
    
    sprintf(outbuf,"000000|%ld|%lf|",nId,dsjk);
    return 0;
}

static long dsBxDzSendAndRecv(char *sbuffer)
{
   int nsock;
   long nLen,nLength,nRetVal;
     
   nsock=connectTCP("dl_tx_9","bxdz");
   if(nsock<0) return -1;
   
   nLen=strlen(sbuffer);
   //send (CTL)data to dlsrv
   if(SendLongToSocket(nsock,nLen)) 
   {
   	close(nsock);
   	return -2;
   }
   printf("\nsbuffer[%ld]=[%s]",nLen,sbuffer);
   if(SendRecordToSocket(nsock,sbuffer,nLen)) 
   {
   	close(nsock);
   	return -2;
   }
   
   nRetVal=-1;
   nLength=GetLongFromSocket(nsock,&nRetVal); 
   printf("\nGet nLength=%ld\n",nLength);
   switch(nRetVal)
   {
      case 0: break;
      case 1:
        strcpy(sbuffer,"�����ļ�������!");
        break;
      case 2:
        strcpy(sbuffer,"д�ļ���!");
        break;
      case 4:
        strcpy(sbuffer,"�����Զ���!!");
        break;
      default:
        strcpy(sbuffer,"ͨ�Ŵ�����!!");
        break;
   }
   close(nsock);

   return nRetVal;
   //return 0;
}

int iIbmOrScoReturn(int num)
{
   char sNum[4];
   char cT;
   
   memcpy(sNum,&num,4);
   cT=sNum[0];
   sNum[0]=sNum[3];
   sNum[3]=cT;
   cT=sNum[1];
   sNum[1]=sNum[2];
   sNum[2]=cT;
   memcpy(&num,sNum,4);
   
   return num;
}

double dIbmOrScoReturn(double num)
{
   char sNum[8];
   char cT;
   
   memcpy(sNum,&num,8);
   cT=sNum[0];
   sNum[0]=sNum[7];
   sNum[7]=cT;
   cT=sNum[1];
   sNum[1]=sNum[6];
   sNum[6]=cT;
   cT=sNum[2];
   sNum[2]=sNum[5];
   sNum[5]=cT;
   cT=sNum[3];
   sNum[3]=sNum[4];
   sNum[4]=cT;
   memcpy(&num,sNum,8);
   
   return num;
}

