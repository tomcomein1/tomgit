//���ƶ�����صĺ�̨����ģ��
#include"dlywd.h"

extern long nSysLkrq;
extern long nSysSfm;

long dsTaskMobileSIngIn(char * sJym)
{
     char sAuthentication[17],sTransKey[17];
     char sBankCode[9],sSendAdd[10],sMACkey[17];
     char sDataTrans[256],sTemp[100];
     int ndeslen;     
     long nRetVal;
     
     
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and \
                              nbh=37 into %s",sJym,sAuthentication); 
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and \
                              nbh=38 into %s",sJym,sTransKey); 
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
 
     memset(sDataTrans,0,sizeof(sDataTrans));
     memset(sTemp,0,sizeof(sTemp));
     
     ndeslen=8;
     //����֤��ת����BCD��
     ASCIItoBCD(sAuthentication,16,sTemp);
     //����֤���ý�����Կ����
     DesEncString(sTransKey,8,sTemp,8,sAuthentication,&ndeslen);
     //�����ܺ����֤��ת����ASCII��
     BCDtoASCII(sAuthentication,8,sTemp);
     strcpy(sDataTrans,sBankCode);
     strcpy(sDataTrans+2,sTemp);
     
     //����ǩ����
     if(dsMobileSendAndRecv(-1,sDataTrans,TASK_MOBILE_SIGN_IN,sSendAdd,sDataTrans)<0)
     	return ApplyToClient("%s","111111|��ͨѶ������/�������ݳ���|");

     if(strncmp(sDataTrans,"0000",4))
     {
        GetMobileErrorInfo(sDataTrans,sDataTrans);
        ApplyToClient("%s",sDataTrans);
        return 0;
     }
         
     ndeslen=8;
     //��MAC��Կת����BCD��
     ASCIItoBCD(sDataTrans+20,16,sTemp);
     //��MAC��Կ�ô�����Կ����
     DesDecString(sTransKey,8,sTemp,8,sMACkey,ndeslen);
     //��MAC��Կת����ASCII��ʽ�Ա������ݿ��д��������������ַ���ͻ 
     BCDtoASCII(sMACkey,8,sTemp);
     //��������Կ���������ݿ���
     RunSql("update dl$jymcsfb set ccs=%s where cjym=%s and nbh=39",
          			   sTemp,sJym);
     CommitWork();
     return ApplyToClient("%s","000000|");
     
}

long dsTaskMobileSIngOut(char * sJym)
{
     char sAuthentication[17],sTransKey[17];
     char sBankCode[9],sSendAdd[10];
     char sDataTrans[256],sTemp[100];
     int ndeslen;     
     long nRetVal;
     
     
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and \
                              nbh=37 into %s",sJym,sAuthentication); 
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and \
                              nbh=38 into %s",sJym,sTransKey); 
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
 
     memset(sDataTrans,0,sizeof(sDataTrans));
     memset(sTemp,0,sizeof(sTemp));
     
     ndeslen=8;
     //����֤��ת����BCD��
     ASCIItoBCD(sAuthentication,16,sTemp);
     //����֤���ý�����Կ����
     DesEncString(sTransKey,8,sTemp,8,sAuthentication,&ndeslen);
     //�����ܺ����֤��ת����ASCII��
     BCDtoASCII(sAuthentication,8,sTemp);
     strcpy(sDataTrans,sBankCode);
     strcpy(sDataTrans+2,sTemp);
     
     //����ǩ����
     if(dsMobileSendAndRecv(-1,sDataTrans,TASK_MOBILE_SIGN_OUT,sSendAdd,sDataTrans)<0)
     	return ApplyToClient("%s","111111|��ͨѶ������/�������ճ���");
 
     if(strncmp(sDataTrans,"0000",4))
     {
        GetMobileErrorInfo(sDataTrans,sDataTrans);
        ApplyToClient("%s",sDataTrans);
        return 0;
     }
     return ApplyToClient("%s","000000|");

}

long dsTaskMobileCompletePrint(char * sJym,char * sphone,char * starttime,char * endtime)
{
     char sBankCode[9],sSendAdd[10],sLsh[41]=".";
     char inbuf[1024],sJsdh[10];
     char sDataTrans[TOTAL_PACKAGE_SIZE];
     char sPrName[20];
     char sTemp[41]=".";
     int ndeslen;     
     long nRetVal;
     long nxh,nTotal,nCount;
     
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
 
     sprintf(sDataTrans,"b005%-11.11s%-8.8s%-14.14s%-14.14s",
                         sphone,sBankCode,starttime,endtime);

     if(dsMobileSendAndRecv(-1,sDataTrans,TASK_MOBILE_COMPLETE_PRINT,sSendAdd,sDataTrans)<0)
     	return ApplyToClient("%s","111111|��ͨѶ������/�������ճ���");
 
     if(strncmp(sDataTrans,"0000",4))
     {
        GetMobileErrorInfo(sDataTrans,sDataTrans);
        ApplyToClient("%s",sDataTrans);
        return 0;
     }
     
     //��Ϊ��ȫ����ԭ��ʽ
     /*MobileFormatTransfer(sDataTrans,TASK_MOBILE_COMPLETE_PRINT);
     MobileInsertCompleteYwmx(sLsh,sphone,sJym,sDataTrans);
     sprintf(sDataTrans,"000000|%s|",sLsh);
     return ApplyToClient("%s",sDataTrans);*/    

     /*��ˮ��|�û���־|�û�����|�ɿʽ|ʵ���ܿ��|����Ա����|ʱ����|�ɿ��·���*/
     sprintf(sJsdh,"%-6.6s001",sJym+9);
     GenerateLsh(1,sJsdh,sJym,sLsh);
     GenerateLsh(3,sJsdh,sJym,sTemp);
     nxh=atol(sTemp);
     
     sprintf(inbuf,"%-40.40s|%s|%s|%ld|%.2lf|%s|%ld|%ld|%s",
                    sLsh,sphone,sphone,4,0.0,"0",nSysSfm,nCount,"0");
                    
     if(InsertZwmxTable(inbuf,TASK_MOBILE_PAY_CONFIRM,nxh)<0)
     {
        ApplyToClient("%s","000011|����������ϸ�����");
        return -3;
     }
 
     //�¸�ʽ 
     if(strlen(sDataTrans)>64)
     {
     	sprintf(sPrName,"prdata/com%04ld",nxh%2000);
     	nCount = dsMobileParsePrint(sPrName,sDataTrans+64);
     	if(nCount>0)
     	{
            RunSql("update dl$zwmx set cbl5=%s,nbl=%ld,ncsbz=3 where clsh=%s",
                                       sPrName,nCount,sLsh);
            CommitWork();
     	}    	
	else
	{
		RollbackWork();
		ApplyToClient("%s","000010|���صķ�Ʊ���ݳ���");
		return -3;
	}
     } 
     sprintf(sDataTrans,"000000|%s|",sLsh);
     return ApplyToClient("%s",sDataTrans);
     
}

static long MobileInsertCompleteYwmx(char * slsh,char * sPhone,char * sJym,char * sDetail)
{
   CHARGEDETAIL det;
   MOBILEANSWER ans;
   
   char sJsdh[10],sStatus[10];
   char * sOffset;
   long nRetVal,nId;
   
   sOffset=sDetail;

   sOffset=FetchStringToArg(sOffset,   //�������ͷ
   "%4s%15s%59s%12s%1d%1s%12f%12f%12f%4d",
                 ans.sReturn,ans.sBargain,
                 ans.sCompany,ans.sUserName,&ans.nWay,ans.sStatus,
                 &ans.dwczhf,&ans.dxyd,&ans.dyck,&ans.nCount);    //��������ֵ��
   strcpy(ans.sRegion,".");
   strcpy(ans.sCountry,".");
   det.dZnj=0.0;
   ans.dbsyzf=0.0;
   sprintf(sJsdh,"%-6.6s001",sJym+9);
   if(ans.nCount==0)    return 0;

   GenerateLsh(1,sJsdh,sJym,slsh);
   for(nId=1;nId<=ans.nCount;nId++)
   {
        //����ϸ���ݽ���
        sOffset=FetchStringToArg(sOffset,
                        "%4d%2d%12f%12f%12f%12f%12f%8s%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f",
                        &det.nYear,&det.nMonth,&det.dFee,&det.dBchf,&det.dYhk,
            		&det.dYckhb,&det.dXjk,sStatus,&det.dYzf,&det.dTff,&det.dBwf,
            		&det.dMyf,&det.dCtf,&det.dFjf,&det.dQtf,&det.dXxf,&det.dPzf,
            		&det.dNhf,&det.dBsf); 
        //����ҵ����ϸ��ṹ
        nRetVal=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,\
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
        	VALUES(%s,%ld,%ld,%s,%s,'0',%s,0,%ld,\
	                %lf,%lf,%lf,%lf,%lf,%lf,%lf,0,0,%lf,\
        	        %lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,0,0,\
	                0,0,0,0,0,'0',0,0,0,0,%lf,0,%lf,0,0,\
 	                1,1,0,0,%lf,0,'0','0','0','0',%ld,%ld,'0',\
                        2,1,'0','0','0','0',%s,%s,%s,'0')",
           sJym,sJym,det.nYear,det.nMonth,sPhone,ans.sBargain,
           ans.sCompany,nId,det.dYzf,det.dPzf,det.dBwf,det.dMyf,
           det.dCtf,det.dFjf,det.dXxf,det.dZnj,det.dNhf,det.dBsf,
           det.dQtf,det.dYhk,det.dYckhb,det.dXjk,det.dTff,ans.dbsyzf,
           det.dBchf-det.dYhk-det.dXjk+det.dZnj,0.0,
           ans.dbsyzf+det.dZnj+det.dFee,
           nSysLkrq,nSysSfm,ans.sRegion,ans.sCountry,slsh);
        
        if(nRetVal<0) break;
        ans.dbsyzf=0;  //���������ֻ���ڵ�һ������
   }//end for;
   if(nRetVal>0)  CommitWork();  
   else
   {
      RollbackWork();
      strcpy(slsh,".");
   }
   return nRetVal;
} 

/*-----------------------------------------------------

����:preDESconvert;
Ŀ��:�Գ���8�ֽڵ����ݽ���DES�����γ�8�ֽڳ���MAC������֤
����:sKey:   ������Կ[8λBCD��]
     sStream:�����ܵ��ַ���,����䳤�Ȳ���8��������,����
             ��������油'\0';
     sDest:  �����ܺ��Ŀ���ַ���[8λBCD��]              
*----------------------------------------------------*/
void preDESconvert(char * sKey,char * sStream,char * sDest)
{
     
     int nLength,nLoop,nReset;
     char sTemp[9]; 
     char * sOffset;
     int ndeslen;
     sOffset=sStream;
     nLength=strlen(sStream);
     //���ַ�����չ��8��������,����8�����������油'\0';
     if(nReset=(nLength%8))
       for(nLoop=nLength;nLoop<=nLength+nReset;nLoop++) sStream[nLoop]='\0'; 
     //�����Դ�ַ������ж�����[ÿ8�ֽ���һ��]: 
     nLoop=nLength/8+((nLength%8)!=0);
     //��Ŀ�괮��ʼ����8λ'\0'
     memset(sDest,'\0',8);
     for(nReset=0;nReset<nLoop;nReset++)
     {    //Ŀ�괮��Դ������8λ���
          for(nLength=0;nLength<8;nLength++)
          {
               sDest[nLength]^=sOffset[0];
               sOffset++;
          }
          ndeslen=8;
          //������Ĵ��ý�����Կ����
          DesEncString(sKey,8,sDest,8,sTemp,&ndeslen); 
          //�����ܺ�Ĵ�����Ŀ�괮����Ϊ�´����ĳ�ֵ
          memcpy(sDest,sTemp,8);
     }
}


/*-------------------------------------------------*
����:ASCIItoBCD;
����:��ʮ�����Ƶ�ASCII���ַ������BCD��
����:sASC:ASCII�ַ���;
     nLen:ASCII�ַ�������[ż��];
     sDest:Ŀ�괮ָ��;
ע��:��ת������ַ�������ֻ��ԭ����һ��[nLen/2];
*-------------------------------------------------*/

long ASCIItoBCD(char * sASC,int nLen,char * sDest)
{
	
     unsigned char cCode;
     char sTable[17]="0123456789ABCDEF";
     int nSrc,nLoop;
 
     nLoop=0;
     nSrc=0;

     if(nLen<=0) return 1;
     for(nLoop=0;nLoop<nLen;nLoop+=2)  //��ʮ�����ƵĴ���ת����ASCII��
     {
     	cCode=sASC[nLoop];
        if(!strchr(sTable,cCode)) return 1;
        sDest[nSrc]=strchr(sTable,cCode)-sTable;  //�õ���λʮ����������
        sDest[nSrc]<<=4;                        //�������λ 
 
        cCode=sASC[nLoop+1];                  //�õ�����λ����
        if(!strchr(sTable,cCode)) return 1;
        sDest[nSrc]|=(strchr(sTable,cCode)-sTable)&0x0f;//�������λ����
       
        nSrc++;
     }
     sDest[nSrc]='\0';
     return 0;	

}

/*-----------------------------------------------------*
����:BCDtoASCII;
����:��������BCD��ת����һ����ʮ�����Ʊ�ʾ���ַ���;
����:sBCD:BCD��ԭ��;
     nLen:BCD�봮����;
     sDest:��ת������ַ���;
ע��:Ŀ�괮�ĳ�����Դ�����ȵ�2��;
*----------------------------------------------------*/ 

long BCDtoASCII(char * sBCD,int nLen,char * sDest)
{
     unsigned char cCode;
     int nLoop,nSrc;
     char sTable[17]="0123456789ABCDEF";
     
     nLoop=0;
     nSrc=0;
 
     if(nLen<=0) return 1;
     for(nLoop=0;nLoop<nLen;nLoop++)
     {
          cCode=sBCD[nLoop];
          sDest[nSrc+1]=sTable[cCode&0x0F];
          cCode>>=4;
          sDest[nSrc]=sTable[cCode&0x0F];
          nSrc+=2;  
     }
     sDest[nSrc]='\0';
     return 0;	

}

/*==================================================================
������dsMobileSendAndRecv
���ܣ���ͨѶ�����Ͱ����ȴ�����;  
������nsock:���Ƚ��õ�socket���ӣ���nsock<0�������ڲ��Զ�����
      sinbuf:���Ͱ�����
      nopt:���������
      soutstr:�����ַ�������
���أ�<0ʧ�ܣ�=0�ɹ� >0:�����ɹ���socket���
˵������soutbuf!=NULL����Ҫ���շ������ݰ���������Ҫ
*=================================================================*/
static int dsMobileSendAndRecv(int nsock,char * sinbuf,long nopt,char * sSendAdd,char * soutbuf)
{
     int nerror;
     long nRetVal,nLength;
     
     if(nsock<0) nsock=connectTCP("dl_tx_3","mobile");
     if(nsock<0) return -1;

     nerror=SendToSocket(nsock,"%ld%s%ld",nopt,sSendAdd,(long)strlen(sinbuf));
		
     if(!nerror) nerror=SendStringToSocket(nsock,sinbuf);
     if(nerror)
     {
     	close(nsock);
     	return -2;
     }
     if(!soutbuf) return nsock;
     nerror=GetLongFromSocket(nsock,&nRetVal);
     if(!nerror&&!nRetVal)
        nerror=GetStringFromSocket(nsock,soutbuf);
     
     close(nsock);
     return nerror;	

}
/*==================================================================-
������GetMobileErrorInfo
���ܣ��õ��ƶ����ش���ĺ��ֽ���
������sError:�������[4λ]
      sInfo:������Ӧ������Ϣ
*===================================================================*/
static void GetMobileErrorInfo(char * sError,char * sInfo)
{
    char sErrCode[5]; 
    int nLen;
    char * sBegin;
    char sMsgTab[]= "1000:101000|���д��벻�Ϸ�|;"
                    "1001:101001|����������ֻ����벻����|;"
                    "1002:101002|δ���ƶ���ǩ��|;"
                    "1003:000012|��ǩ��״̬|;"
                    "1004:000012|ǩ��״̬���������ٴ�ǩ��|;"
                    "1005:101005|δ���ʣ���ǩ��������ǩ��|;"
                    "1006:101006|ǩ�����Ѿ����ѣ��������ٴ�ǩ��|;"
                    "1007:101007|��ǩ��״̬|;"
                    "1014:101014|����ʱδ���յ������ļ���������ļ��޷���|;"
                    "1100:101100|��֤�벻��!��ѯ���ƶ�����֤���Ƿ��б䶯|;"
                    "1101:101101|MACУ�첻��!��֪ͨ�ɶ���־��˾|;"
                    "1102:101102|���Ѳ���|;"
                    "1103:101103|������ˮ���ظ�|;"
                    "1104:101104|δ���Ѳ�ѯ���׾ͽ��н�����֤|;"
                    "1105:101105|����ʱ�Ƿ��Ĳ�������|;"
                    "1202:101202|���ֽ���֤���ף��ͽ���ȷ��|;"
                    "1203:101203|ȷ��ʱ���Ѳ���|;"
                    "1204:101204|ϵͳ�Ѿ�ǩ��!|;"
                    "1301:101301|�޽ɷѼ�¼,�޷��˷�|;"
                    "2000:102000|�ƶ������ݿ����ʧ��|;"
                    "2001:102001|�ƶ����޷��������ݿ�|;"
		    "2002:102002|���ݿ����Ӵ�|;"
		    "2042:102042|������ˮ��|;"
		    "2043:102043|�޸�ǩ������ʧ��|;"
		    "2044:102044|����ǩ������ʧ��|;"
		    "2046:102046|����ʱ���������|;"
		    "3000:103000|�м����|;"
		    "3401:103401|�ֻ�������|;"
                    "7001:107001|����Դ�������ȴ�ͨѶ�����ذ�ʧ��|;"
                    "8001:108001|�޷����ƶ�ͨѶ����������|;"
                    "8004:108004|ͨѶ�������ƶ������ݰ���ʧ|;"
                    "8005:108005|���ջ��������|;"
                    "8007:108006|�ȴ��ƶ���˾�������ݰ���ʱ|;"
                    "8006:108007|ͨѶ���ȴ��Ĵ�����࣬���Ժ�����|;"
                    "9001:109001|����Դ���ݿ��������|;"
                    "1111:101111|���ƶ�ͨѶ��ͨ��ʧ��|;"
                    "9997:109997|�޷��������ն�����ϸ�ļ�|;"
                    "9994:109994|�޷���ͨѶ������mobiled��������|;";                

    strncpy(sErrCode,sError,4);
    sErrCode[4]='\0';
    sBegin=strstr(sMsgTab,sErrCode);
    
    if(sBegin&&sBegin[4]==':')
    {
           nLen=strchr(sBegin,';')-sBegin-5;    
           strncpy(sInfo,sBegin+5,nLen);
           sInfo[nLen]='\0';
    }       
    else   sprintf(sInfo,"109999|δ֪�Ĵ���[%s]|",sErrCode);

}


/*================================================
����:MobileFormatTransfer;
Ŀ��:�ƶ�����ת�����ݰ�����ʽ,�����н�����100
����:sAnswer:�ƶ����ص����ݰ�
     nTaskNo:�����
*===============================================*/
static void MobileFormatTransfer(char * sAnswer,long nTaskNo)
{
    char sChange[TOTAL_PACKAGE_SIZE];
    char sFinal[TOTAL_PACKAGE_SIZE];
    char sStatus[10];
    char * sOffset,* sWrite;
    MOBILEANSWER ans;
    CHARGEDETAIL det;
    long nRec;
    
      
    if(sAnswer==NULL) return;
    
    sOffset=sAnswer;
    sWrite=sFinal;
   
    switch(nTaskNo)
    {
    	case TASK_MOBILE_PAY_QUERY:
 	     sOffset=FetchStringToArg(sOffset,            //�������ͷ
                 "%4s%15s%2s%2s%59s%12s%1d%1s%12f%12f%12f%12f%4s%4s%4d",
                 ans.sReturn,ans.sBargain,ans.sRegion,ans.sCountry,
                 ans.sCompany,ans.sUserName,&ans.nWay,ans.sStatus,
                 &ans.dwczhf,&ans.dxyd,&ans.dyck,&ans.dbsyzf,ans.smljqs,
                 ans.sszjqs,&ans.nCount);    //��������ֵ�ֱ�װ�������

                 sprintf(sWrite,
                 "%-4.4s%-15.15s%-2.2s%-2.2s%-59.59s%-12.12s%1ld%-1.1s%12.2lf%12.2lf%12.2lf%12.2lf%-4.4s%-4.4s%4d",
                 ans.sReturn,ans.sBargain,ans.sRegion,ans.sCountry,
                 ans.sCompany,ans.sUserName,ans.nWay,ans.sStatus,
                 ans.dwczhf/100.0,ans.dxyd/100.0,ans.dyck/100.0,ans.dbsyzf/100.0,ans.smljqs,
                 ans.sszjqs,ans.nCount);
                 
                 while(sWrite[0]!='\0') sWrite++;
                 //�����ϸ��
                 for(nRec=1;nRec<=ans.nCount;nRec++) 
                 {
                        sOffset=FetchStringToArg(sOffset,
                        "%4d%2d%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f",
                        &det.nYear,&det.nMonth,&det.dFee,&det.dBchf,&det.dZnj,&det.dYhk,
            		&det.dYckhb,&det.dXjk,&det.dYzf,&det.dTff,&det.dBwf,&det.dMyf,
            		&det.dCtf,&det.dFjf,&det.dQtf,&det.dXxf,&det.dPzf,&det.dNhf,&det.dBsf); 
                
                        sprintf(sWrite,
                        "%4ld%2ld%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf",
                        det.nYear,det.nMonth,det.dFee/100.0,det.dBchf/100.0,det.dZnj/100.0,
                        det.dYhk/100.0,	det.dYckhb/100.0,det.dXjk/100.0,det.dYzf/100.0,
                        det.dTff/100.0,det.dBwf/100.0,det.dMyf/100.0,det.dCtf/100.0,det.dFjf/100.0,
                        det.dQtf/100.0,det.dXxf/100.0,det.dPzf/100.0,det.dNhf/100.0,det.dBsf/100.0); 
                        while(sWrite[0]!='\0') sWrite++;

                 }//end for(;;)
                 
                 strcpy(sAnswer,sFinal);
                 break;

    	case TASK_MOBILE_COMPLETE_PRINT:
 	     sOffset=FetchStringToArg(sOffset,            //�������ͷ
                 "%4s%15s%59s%12s%1d%1s%12f%12f%12f%4d",
                 ans.sReturn,ans.sBargain,ans.sCompany,ans.sUserName,&ans.nWay,
                 ans.sStatus,&ans.dwczhf,&ans.dxyd,&ans.dyck,&ans.nCount);    //��������ֵ�ֱ�װ�������
                 sprintf(sWrite,
                 "%-4.4s%-15.15s%-59.59s%-12.12s%1ld%-1.1s%12.2lf%12.2lf%12.2lf%4d",
                 ans.sReturn,ans.sBargain,ans.sCompany,ans.sUserName,ans.nWay,ans.sStatus,
                 ans.dwczhf/100.0,ans.dxyd/100.0,ans.dyck/100.0,ans.nCount);

                 while(sWrite[0]!='\0') sWrite++;
                 //�����ϸ��
                 for(nRec=1;nRec<=ans.nCount;nRec++) 
                 {
                        sOffset=FetchStringToArg(sOffset,
                        "%4d%2d%12f%12f%12f%12f%12f%8s%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f",
                        &det.nYear,&det.nMonth,&det.dFee,&det.dBchf,&det.dYhk,
            		&det.dYckhb,&det.dXjk,sStatus,&det.dYzf,&det.dTff,&det.dBwf,
            		&det.dMyf,&det.dCtf,&det.dFjf,&det.dQtf,&det.dXxf,&det.dPzf,
            		&det.dNhf,&det.dBsf); 
                
                        sprintf(sWrite,
                        "%4ld%2ld%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%-8.8s%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf%12.2lf",
                        det.nYear,det.nMonth,det.dFee/100.0,det.dBchf/100.0,det.dYhk/100.0,
                        det.dYckhb/100.0,det.dXjk/100.0,sStatus,det.dYzf/100.0,
                        det.dTff/100.0,det.dBwf/100.0,det.dMyf/100.0,det.dCtf/100.0,det.dFjf/100.0,
                        det.dQtf/100.0,det.dXxf/100.0,det.dPzf/100.0,det.dNhf/100.0,det.dBsf/100.0); 
                        while(sWrite[0]!='\0') sWrite++;

                 }//end for(;;)
                 
                 strcpy(sAnswer,sFinal);
                 break;
     }//end switch                      

}


/*==================================================================
������dsTaskMobilePayQuery
���ܣ��ƶ��ֻ����շѲ�ѯ����
������slsh:��ˮ��
      smap:��������λͼ[5�ַ�]
      syhbz:���������ַ���[5*20�ַ�]
      sJym:������[15λ]
      Client:�ش�������¼ָ��
*=================================================================*/
long dsMobilePayQuery(char * inbuf,RECORD * Client)
{

     char sBzmap[6],sJym[16],sLsh[41],sYhbz[101],sJsdh[10],sCzydh[7];
     char sBuffer[TOTAL_PACKAGE_SIZE];
     char sBankCode[9];   //���д��루dl$jymcsfb,nbh=36)
     char sSendAdd[10];
     CHARGEDETAIL det;
     MOBILEANSWER ans;
  
     FetchStringToArg(inbuf,"%s|%s|%s|%s|",sLsh,sBzmap,sYhbz,sCzydh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);

     sYhbz[11]='\0';
     memset(sBuffer,'\0',TOTAL_PACKAGE_SIZE);
   
     //����ĺ����ṩ����������л��������CommitWork/RollbackWork����
          
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
   
     sprintf(sBuffer,"b000%-11.11s%-8.8s",sYhbz,sBankCode);

     if(dsMobileSendAndRecv(-1,sBuffer,TASK_MOBILE_PAY_QUERY,sSendAdd,sBuffer)<0)
     {
     	ImportRecord(Client,"%s%s","111111","��ͨѶ������/�������ճ���");
        return 0;
     }
          
     if(strncmp(sBuffer,"0000",4))
     {
        GetMobileErrorInfo(sBuffer,sBuffer);
        ImportRecord(Client,"%s",sBuffer);      
        return 0;
     }
     MobileFormatTransfer(sBuffer,TASK_MOBILE_PAY_QUERY);
     return MobileInsertYwmx(sLsh,sCzydh,sYhbz,sJym,sBuffer,Client);
          
}


/*===============================================================
������MobileInsertYwmx
���ܣ��ƶ�����ѯ�������ҵ����ϸ��
������ slsh:��ˮ��
      sPhone:�绰����
      sJym:������
      sCzydh:��ѯ����Ա����
      sDetail:�ƶ����ص���ϸ����
      Client:���ظ��ͻ��˵�RECORD�ṹ

*===============================================================*/

static long MobileInsertYwmx(char * slsh,char * sCzydh,char * sPhone,char * sJym,char * sDetail,RECORD * Client)
{
   CHARGEDETAIL det;
   MOBILEANSWER ans;
   
   char sCode[]="ABDGHIGKLab";
   char * curstat[]={"����","ð��","Ƿͣ","��ͣ","��ʧ","Ԥ��","Ԥ��","ǿ��","ǿ��","����","�ֲ�"};
   char sSqlErr[51],sSequence[11],sJsdh[10],sStatus[5];
   char * sOffset;
   long nRec,nRetVal=1,nLoop;
   long nId=1,nxh=-1;
   double dSjk,dYjzk,dAddition,dTotalSjk,dTemp;
   long   nValue[4];
      
   memset(&det,0,sizeof(CHARGEDETAIL));
   memset(&ans,0,sizeof(MOBILEANSWER));
   memset(nValue,0,sizeof(nValue));

   dTotalSjk=0.0; 
   sOffset=sDetail;
   sOffset=FetchStringToArg(sOffset,            //�������ͷ
                "%4s%15s%2s%2s%59s%12s%1d%1s%12f%12f%12f%12f%4s%4s%4d",
                 ans.sReturn,ans.sBargain,ans.sRegion,ans.sCountry,
                 ans.sCompany,ans.sUserName,&ans.nWay,ans.sStatus,
                 &ans.dwczhf,&ans.dxyd,&ans.dyck,&ans.dbsyzf,ans.smljqs,
                 ans.sszjqs,&ans.nCount);    //��������ֵ�ֱ�װ�������

   strncpy(sStatus,curstat[strchr(sCode,ans.sStatus[0])-sCode],4);
   sStatus[4]='\0';
   FetchStringToArg(slsh,"%9s",sJsdh);
   ImportRecord(Client,"%s%s%s%lf%ld","000000",ans.sCompany,ans.sBargain,ans.dyck,((ans.nCount==0)?1:ans.nCount));
   nId=((ans.nCount==0)?0:1);

   det.nYear=(nSysLkrq/10000);
   det.nMonth=(nSysLkrq%10000)/100;
   dAddition=0.0;
   //if(ans.nCount>0 && ans.nWay==9) dAddition=(ans.dwczhf+50-ans.dyck);//Ԥ�滰���û�
   if(ans.nCount>0 && ans.nWay==9) dAddition=(ans.dwczhf-ans.dyck);//��Ԥ�滰���û�
   nValue[0]=ans.nWay;   //�����ͱ����ֶ�
   
   for(;nId<=ans.nCount;nId++)
   {
        if(ans.nCount>0)
        {
            //����ϸ���ݽ���
            sOffset=FetchStringToArg(sOffset,
            "%4d%2d%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f",
            &det.nYear,&det.nMonth,&det.dFee,&det.dBchf,&det.dZnj,&det.dYhk,
            &det.dYckhb,&det.dXjk,&det.dYzf,&det.dTff,&det.dBwf,&det.dMyf,
            &det.dCtf,&det.dFjf,&det.dQtf,&det.dXxf,&det.dPzf,&det.dNhf,&det.dBsf); 
        }
        //������ϸ������䵽���ؽṹ��
        //�û�ʵ�ɿ�=����Ƿ��+���ɽ�+���������(ֻ�ӵ�һ����)+Ԥ���û�����(ֻ�����һ����)
        dSjk=det.dFee+det.dZnj+ans.dbsyzf;
        dTemp=dSjk;
        if(nId==ans.nCount) dSjk+=dAddition;
        //Ӧ���ܿ�(��Ʊ����д)=����ͨ����-��ֵ������+���ɽ�+���������-�Żݿ�
        dYjzk=det.dBchf-det.dXjk+det.dZnj+ans.dbsyzf-det.dYhk;
        if(dYjzk<0.0) dYjzk=0.0;
        if((dSjk-(long)dSjk)>0.0) dSjk=(double)((long)dSjk+1);
        //����ͻ���RECORD�ṹ
        ImportRecord(Client,"%s%ld(2)%lf(26)%ld[4]%s(4)",sPhone,
                         det.nYear,det.nMonth,det.dYzf,det.dPzf,
                         det.dBwf,det.dMyf,det.dCtf,det.dFjf,det.dXxf,
                         0.0,0.0,det.dZnj,det.dNhf,det.dBsf,
                         det.dQtf,det.dYhk,det.dYckhb,det.dXjk,det.dTff,
                         ans.dbsyzf,dSjk,ans.dwczhf,ans.dxyd,0.0,0.0,
                         0.0,(ans.nWay==9)?dYjzk:dTemp,
                         dSjk,nValue,sStatus, "." , "." , ".");

        GenerateLsh(3,sJsdh,sJym,sSequence);
        nxh=atol(sSequence);
      
        //����ҵ����ϸ��ṹ
        nLoop=2;
        dTotalSjk+=dSjk;
        //����2��ҵ����ϸ����ѭ�����Ա�֤�ƶ����������ڷǷ��ַ�ʱҲ����ȷ����
        while(nLoop--)
        {
           nRetVal=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,\
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
                 VALUES(%s,%ld,%ld,%s,%s,'0',%s,0,%ld,\
                	%lf,%lf,%lf,%lf,%lf,%lf,%lf,0,0,%lf,\
                  	%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,0,0,\
                  	0,0,0,0,0,'0',0,0,0,0,%lf,0,%lf,0,0,\
                  	1,1,%ld,0,0,0,'0','0','0',%s,%ld,%ld,'0',\
                  	2,0,'0','0','0','0',%s,%s,%s,'0')",
           sJym,sJym,det.nYear,det.nMonth,sPhone,ans.sBargain,
           ans.sCompany,nxh,det.dYzf,det.dPzf,det.dBwf,det.dMyf,
           det.dCtf,det.dFjf,det.dXxf,det.dZnj,det.dNhf,det.dBsf,
           det.dQtf,det.dYhk,det.dYckhb,det.dXjk,det.dTff,ans.dbsyzf,
           dYjzk,((nId==ans.nCount)?dTotalSjk:0.0),ans.nWay,sCzydh,nSysLkrq,nSysSfm,ans.sRegion,ans.sCountry,slsh);
           if(nRetVal>0) break;
           strcpy(ans.sCompany,".");
        }//end while
        
        if(nRetVal<0) break;
        ans.dbsyzf=0.0;  //���������ֻ���ڵ�һ������
   }//end for;

   if(nRetVal>0)  CommitWork();  
   else
   {
   	RollbackWork();
   	GetSqlErrInfo(sSqlErr);
   	sprintf(Client->sdata,"101007|%s|",sSqlErr);
   }
   return nRetVal;
} 


/*============================================================
������dsTaskMobilePayConfirm
���ܣ��ƶ��ɿ�����ģ��
������inbuf:�ɿ���������ַ�������
      outbuf:�ɷѽ������
���أ�0�ɹ�������ʧ��
*============================================================*/
long dsTaskMobilePayConfirm(char * inbuf,char * outbuf)
{
     char sTemp[10000],sMAC[17],sMACkey[17],sPrName[20];     
     char sLsh[41];  //��ˮ��
     char sYhbz[101]; //�û���־
     char sJym[16];  //������
     char sYhmc[51]; //�û�����
     char sCzydh[7]; //����Ա����
     char sJsdh[10]; //�ɷѾ־ֺ�
     char sSendAdd[10],sBankCode[9];
     char sBuffer[10000];
     char sRegion[3],sCountry[3];  //�������������ش���
     char * sOffset;
     long nMode;     //�ɿʽ 1���ֽ�  2���̿�
     long nxh,nTotal,nCount;
     int  nsock;
     double dSjk,dTemp;
     long nSfm=0;


     sOffset=FetchStringToArg(inbuf,"%s|%s|%s|%d|%f|%s|%d|",
                         sLsh,sYhbz,sYhmc,&nMode,&dSjk,sCzydh,&nSfm);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);
    
     sYhbz[11]='\0'; 
     RunSelect("select distinct cbl6,cbl7 from dl$ywmx_%t \
                where clsh=%s into %s%s",sJym,sLsh,sRegion,sCountry);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=39 into %s",sJym,sMAC);
       
     RunSql("update dl$ywmx_%t set dsjk=%lf where clsh=%s and dSjk>0.0",sJym,dSjk,sLsh);

     ASCIItoBCD(sMAC,16,sMACkey);
     
     //�õ�������ϸ�ⲿ��ˮ��
     
     GenerateLsh(3,sJsdh,sJym,sBuffer);
     nxh=atol(sBuffer);
     
     //����MAC��֤����
     sprintf(sTemp,"%8.8s%08ld%11.11s%12ld%08ld%06ld",
                    sBankCode,nxh,sYhbz,(long)dSjk*100,nSysLkrq,nSfm);
     //����MACУ��
     preDESconvert(sMACkey,sTemp,sMAC);
     //�����յ�MACУ������ת����ASCII�����Ա㴫��
     BCDtoASCII(sMAC,8,sTemp);
     //�����ܵ����ݸ����̴�����
     sprintf(sBuffer,
             "b000%-2.2s%-2.2s%-8.8s%08ld%-11.11s%12ld%08ld%06ld%-16.16s",
             sRegion,sCountry,sBankCode,nxh,sYhbz,(long)dSjk*100,
             nSysLkrq,nSfm,sTemp);

     //�����ݷ����ƶ�������֤
     
     if(dsMobileSendAndRecv(-1,sBuffer,TASK_MOBILE_PAY_CHECK,sSendAdd,sTemp)<0)
     {
        RollbackWork();
     	strcpy(outbuf,"111111|��ͨѶ������/�������ճ���|");
        return -1;
     }
     
     if(strncmp(sTemp,"0000",4))
     {
     	GetMobileErrorInfo(sTemp,outbuf);
     	RollbackWork();
     	return -2;
     }

     if(InsertZwmxTable(inbuf,TASK_MOBILE_PAY_CONFIRM,nxh)<0)
     {
        GetSqlErrInfo(sTemp);
        sprintf(outbuf,"000011|����������ϸ�����%s|",sTemp);
        return -3;
     }
     CommitWork();

     /* �µķ�Ʊ��ӡ��ʽ */

     if(strlen(sTemp)>64)
     {
     	sprintf(sPrName,"prdata/pr%04ld",nxh%2000);
     	nCount = dsMobileParsePrint(sPrName,sTemp+64);
     	if(nCount>0)
     	{
            RunSql("update dl$zwmx set cbl5=%s,nbl=%ld where clsh=%s",sPrName,nCount,sLsh);
            CommitWork();
     	}    	
     }

     nsock=dsMobileSendAndRecv(-1,sBuffer,TASK_MOBILE_PAY_CONFIRM,
                                          sSendAdd,NULL);
     if(nsock>0) close(nsock);
     sprintf(outbuf,"000000|%s|%.2lf|",sLsh,dSjk);
     return 0;

}

long dsMobileCheckAccount(char * inbuf,char * outbuf)
{

     char sSendAdd[10],sBankCode[9];
     char sBuffer[256],sFileName[101];
     long nPkgCount,nCount,nError,nsize;
     int  nsock;
     double dMoney;
     FILE * fp;

     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",inbuf,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",inbuf,sSendAdd);

     //�����ƶ������ļ�
     nPkgCount=dsMobileFormAccountFile(inbuf,sFileName,&nsize);
     if(nPkgCount<=0)
     {
        strcpy(outbuf,"000015|�����ƶ������ļ��ļ�����|");
        return -1;
     }
     RunSelect("select count(*),sum(dsjk) from dl$zwmx where ncsbz=1 \
                and cjym=%s into %ld%lf",inbuf,&nCount,&dMoney);
                
     sprintf(sBuffer,"%-2.2s%6ld%12ld",sBankCode,nCount,(long)(dMoney*100));

     nsock=dsMobileSendAndRecv(-1,sBuffer,TASK_MOBILE_PAY_COMPARE,sSendAdd,NULL);
     if(nsock>0)
        nError=SendLongToSocket(nsock,nPkgCount);
          
     if(nsock<0||nError)
     {
     	strcpy(outbuf,"100008|��ͨѶ������/�������ճ���|");
        return -2;
     }
     dsMobileSendPayDetail(nsock,sFileName,nsize);
     close(nsock);
   
     if(!strncmp(sFileName,"0000",4))
     {
     	 RunSql("update dl$zwmx set ncsbz=2 where ncsbz=1 and cjym=%s",inbuf);
     	 RunSql("update dl$jymcsfb set ccs='%ld' where cjym=%s and nbh=34",nSysLkrq,inbuf);
         CommitWork();
         sprintf(outbuf,"000000|%ld|%.2lf",nCount,dMoney);
     }
     else
     {
     	 RollbackWork();
     	 GetMobileErrorInfo(sFileName,outbuf);
     	  //������־[д��dlywd��mob_accnt.log�ļ���
     	 sprintf(sBuffer,"[%ld]���ʴ������:%s\n",nSysLkrq,sFileName);
     	 if(!(fp=fopen("mob_accnt.log","at"))) return -1;
     	 fwrite(sBuffer,strlen(sBuffer),1,fp);
     	 fclose(fp);
     }
     return 0;
}


static long dsMobileFormAccountFile(char * inbuf,char * sFileName,long * pnSize)
{
     char sTemp[256],sMAC[17],sMACkey[17],sBuffer[256];     
     char sRegion[3],sCountry[3],sFile[31],sBankCode[10];  //�������������ش���
     char sLsh[41],sYlsh[41],sYhbz[31];
     long nXh,nYxh,nId,nLkrq,nSfm,nCzlb,nsize,nRetVal;
     double dSjk;
     FILE * fp;
     
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",inbuf,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=39 into %s",inbuf,sMAC);
     
     ASCIItoBCD(sMAC,16,sMACkey);
     GetSysLkrq();
     
     sprintf(sFile,"YD_%-2.2s_%08ld%06ld",sBankCode,nSysLkrq,nSysSfm);
     sprintf(sFileName,"account/%s",sFile);
     
     if(!(fp=fopen(sFileName,"wt"))) return -1;
     nId=OpenCursor("select clsh,cylsh,nxh,cyhbz,dsjk,to_number(substr(clxbz,19,1)),\
                     nlkrq,nsfm from dl$zwmx where ncsbz=1 and cjym=%s order by nxh",inbuf);
     if(nId<0)
     {
        GetSqlErrInfo(sBuffer);
        printf(sBuffer);
     	fclose(fp);
     	return -1;
     }
     memset(sBuffer,'\0',28);
     strncpy(sBuffer,sFile,20);
     fwrite(sBuffer,28,1,fp);
     nsize=28;
     for(;;)
     {
     	nRetVal=FetchCursor(nId,"%s%s%ld%s%lf%ld%ld%ld",sLsh,sYlsh,&nXh,
     	                        sYhbz,&dSjk,&nCzlb,&nLkrq,&nSfm);
        if(nRetVal==0) break;
        if(nRetVal<0)
        {
        	CloseCursor(nId);
        	fclose(fp);
        	return -2;
        }
        memset(sTemp,'\0',sizeof(sTemp));
        memset(sMAC,'\0',16);
        strcpy(sRegion,"g");
        strcpy(sCountry,"a");

        if(nCzlb==0 || nCzlb==2)  //�ɷ�
        {
              sprintf(sTemp,
              "%-8.8s%08ld%-11.11s%12ld%08ld%06ld",
              sBankCode,nXh,sYhbz,(long)(dSjk*100),nLkrq,nSfm);

              preDESconvert(sMACkey,sTemp,sMAC);
              BCDtoASCII(sMAC,8,sTemp); 
    
              
              RunSelect("select cbl6,cbl7 from dl$ywmx_%t where clsh=%s into %s%s",
                         inbuf,sLsh,sRegion,sCountry);

              sprintf(sBuffer,
         	"%-16.16s|b000|%-2.2s|%-2.2s|%-8.8s|%08ld|%-11.11s|%08ld%06ld|%12ld\n",
                    sTemp,sRegion,sCountry,sBankCode,nXh,
                    sYhbz,nLkrq,nSfm,(long)(dSjk*100));

              fwrite(sBuffer,strlen(sBuffer),1,fp);
              nsize+=strlen(sBuffer);
         }
         else if(nCzlb==1) //����
         { 

            RunSelect("select cbl6,cbl7 from dl$ywmx_%t where clsh=%s into %s%s",
                         inbuf,sYlsh,sRegion,sCountry);
            RunSelect("select nxh from dl$zwmx where clsh=%s into %ld",sYlsh,&nYxh);
            
            sprintf(sTemp,
            "%-8.8s%08ld%08ld%-11.11s%08ld%06ld",sBankCode,nXh,nYxh,sYhbz,nLkrq,nSfm);
            
            preDESconvert(sMACkey,sTemp,sMAC);
            BCDtoASCII(sMAC,8,sTemp); 
            
            sprintf(sBuffer,
         "%-16.16s|b002|%-2.2s|%-2.2s|%-8.8s|%08ld|%-11.11s|%08ld%06ld|%08ld\n",
                    sTemp,sRegion,sCountry,sBankCode,nXh,
                    sYhbz,nLkrq,nSfm,nYxh);
  
            fwrite(sBuffer,strlen(sBuffer),1,fp);
            nsize+=strlen(sBuffer); 
         }//end else
     }//end for	
     	
     fclose(fp);
     CloseCursor(nId);  
     
     * pnSize=nsize;
     return (long)(nsize/216+((nsize%216)!=0));

}


/*-----------------------------------------------------*
����:dsMobileSendPayDetail;
Ŀ��:���ƶ�����ϸ��;
����:nSocket:��ͨѶ������������;
     sAnswer:��������ļ���,����ƶ�Ӧ��; 
*------------------------------------------------------*/
void dsMobileSendPayDetail(int nSocket,char * sAnswer,long nFileSize)
{
    FILE * fp;
    char sBuffer[216];
    long nLoop,nRetVal;
    char ch;
    
    if(!(fp=fopen(sAnswer,"rt")))  return;
    for(;;)
    {
        memset(sBuffer,'\0',216);
        nRetVal=0;
        for(nLoop=0;nLoop<216;nLoop++)
        {
             ch=getc(fp);
             if((nFileSize--)<=0)
             {
                 fclose(fp);
                 break;
             }
             nRetVal++;
             sBuffer[nLoop]=ch;
        }
        if(nRetVal>0)
        {

             SendLongToSocket(nSocket,nRetVal);
             SendRecordToSocket(nSocket,sBuffer,nRetVal);

        }
        if(nFileSize<=0) break;
    }          
    
    GetFromSocket(nSocket,"%ld%ld",&nRetVal,&nLoop);
    GetOneRecordFromSocket(nSocket,sAnswer,nLoop);    
    
}    


/*=========================================================
������dsMobileRecallConfirm
���ܣ��ƶ�����ȷ������
������inbuf:��������������ģ�������������
      ������ˮ��|��������ˮ��|����Ա����|ʱ����|
      nxh:��ί�з�������¼���
      outbuf:���ظ��ͻ��˵ĳɹ���ʧ����Ϣ
      
���أ�0�ɹ�������ʧ��
*========================================================*/
long dsMobileRecallConfirm(char * inbuf,long nxh,char * outbuf)
{
     char sTemp[256],sMAC[17],sMACkey[17];     
     char sLsh[41],sRecallLsh[41];  //��ˮ��
     char sYhbz[101]; //�û���־
     char sJym[16];  //������
     char sCzydh[7]; //����Ա����
     char sJsdh[10]; //�ɷѾ־ֺ�
     char sSendAdd[10],sBankCode[9];
     char sBuffer[256];
     char sRegion[3],sCountry[3];  //�������������ش���
     long nRecallXh;
     int nsock;
     double dSjk;
 	long nSfm=0;

     FetchStringToArg(inbuf,"%s|%s|",sLsh,sRecallLsh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);

     RunSelect("select distinct cbl6,cbl7 from dl$ywmx_%t \
                where clsh=%s into %s%s",sJym,sRecallLsh,sRegion,sCountry);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=39 into %s",sJym,sMAC);
     RunSelect("select nxh,cyhbz from dl$zwmx where cjym=%s and \
                clsh=%s into %ld%s",sJym,sRecallLsh,&nRecallXh,sYhbz);

     ASCIItoBCD(sMAC,16,sMACkey);

     //��������  
     GetSysLkrq();
     nSfm=nSysSfm;
     sprintf(sBuffer,
             "b002%-2.2s%-2.2s%-8.8s%08ld%-11.11s%08ld%06ld",
             sRegion,sCountry,sBankCode,nRecallXh,sYhbz,nSysLkrq,nSfm);
             
     if(dsMobileSendAndRecv(-1,sBuffer,TASK_MOBILE_RECALL,sSendAdd,sBuffer)<0)
     {
        RollbackWork();
     	strcpy(outbuf,"111111|��ͨѶ������/�������ճ���|");
        return -1;
     }

     if(strncmp(sBuffer,"0000",4))
     {
     	GetMobileErrorInfo(sBuffer,outbuf);
     	RollbackWork();
     	return -2;
     }

     sprintf(sTemp,"%-8.8s%08ld%08ld%-11.11s%08ld%06ld",
                     sBankCode,nxh,nRecallXh,sYhbz,nSysLkrq,nSfm);
 
     preDESconvert(sMACkey,sTemp,sMAC);
     BCDtoASCII(sMAC,8,sMACkey);

     sprintf(sBuffer,"b002%-2.2s%-2.2s%s%-16.16s",
             sRegion,sCountry,sTemp,sMACkey);

     if(dsMobileSendAndRecv(-1,sBuffer,TASK_MOBILE_RECALL_CONFIRM,sSendAdd,sBuffer)<0)
     {
        RollbackWork();
     	strcpy(outbuf,"111111|��ͨѶ������/���������ճ���|");
        return -1;
     }
          
     if(strncmp(sBuffer,"0000",4))
     {
     	GetMobileErrorInfo(sBuffer,outbuf);
     	RollbackWork();
     	return -2;
     }
     sprintf(outbuf,"000000|�ƶ������ɹ�|");
     return 0;
}


long  dsMobileParsePrint(char * filename,char * data)
{
     long row,width,count,max_row,increment;
     char value[100],align[3],type[3];
     char parse[200],prdata[1024];
     char * offset,* pointer,* valid;
     FILE * fpw;
     
     offset = data;
     pointer = prdata;
     count=1;
     max_row=1;
     if(!(fpw = fopen(filename,"wb"))) return -1;
     sprintf(prdata,"%08ld%06ld\n",nSysLkrq,nSysSfm);
     fwrite(prdata,15,1,fpw);
     memset(prdata,'\0',sizeof(prdata));

     for(;;)
     {
     	memset(parse,'\0',sizeof(parse));
     	offset = FetchStringToArg(offset,"%s\n",parse);

        if(strlen(offset)<8)
        {
           while(pointer>=prdata)
           {
           	if((unsigned char)pointer[0]<' ') pointer[0]=' ';
           	pointer--;
           }
           fwrite(prdata,strlen(prdata),1,fpw);
           memset(prdata,'\n',1);
           fwrite(prdata,1,1,fpw);

           fwrite("\nEND_OF_PAGE",12,1,fpw);
           fclose(fpw);
           break;
        }

     	if(parse[0]<'0' || parse[0]>'9'||strlen(parse)<10) continue;

        memset(value,'\0',sizeof(value));     	
     	valid = FetchStringToArg(parse,"%4d%4d%2s%2s%s",&row,&width,align,type,value);
     	if(row>max_row)
     	{
           while(pointer>=prdata)
           {
           	if((unsigned char)pointer[0]<' ') pointer[0]=' ';
           	pointer--;
           }
           fwrite(prdata,strlen(prdata),1,fpw);
           memset(prdata,'\n',row-max_row);
           fwrite(prdata,row-max_row,1,fpw);
           switch(max_row)  //�˴��Ǽ��пո�
           {
		case 8:
	        case 28:
                  memset(prdata,'\n',1);
                  fwrite(prdata,1,1,fpw);
                break;
		default: break;
           }
           memset(prdata,'\0',sizeof(prdata));
           max_row = row;
           pointer = prdata;
        }
        
        if(row<max_row)
        {
           while(pointer>=prdata)
           {
           	if((unsigned char)pointer[0]<' ') pointer[0]=' ';
           	pointer--;
           }
           fwrite(prdata,strlen(prdata),1,fpw);
           memset(prdata,'\n',1);
           fwrite(prdata,1,1,fpw);

           count++;
           fwrite("END_OF_PAGE",12,1,fpw);
           max_row=1;
           memset(prdata,'\0',sizeof(prdata));
           pointer = prdata;
        }

        if(valid)
        {
           alltrim(value);
           value[width]='\0';
           switch(align[0])
           {
           	case '-':     //����
           	     increment = 0;
           	     break;
           	case '+':     //�ҿ���
           	     increment =width-strlen(value);
           	     break;
           	default:      //����
           	     increment = (width-strlen(value))/2;
           	     break;
           }
           if(increment<0) increment=0;
           strcpy(pointer+increment+3,value);  //�˴����пո���
           pointer+=width;
        }   	
    }//end for   
    return count;
}
