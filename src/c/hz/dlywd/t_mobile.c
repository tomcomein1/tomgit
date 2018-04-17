//与移动的相关的后台任务模块
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
     //将认证码转换成BCD码
     ASCIItoBCD(sAuthentication,16,sTemp);
     //将认证码用交换密钥加密
     DesEncString(sTransKey,8,sTemp,8,sAuthentication,&ndeslen);
     //将加密后的认证码转换成ASCII码
     BCDtoASCII(sAuthentication,8,sTemp);
     strcpy(sDataTrans,sBankCode);
     strcpy(sDataTrans+2,sTemp);
     
     //发送签到包
     if(dsMobileSendAndRecv(-1,sDataTrans,TASK_MOBILE_SIGN_IN,sSendAdd,sDataTrans)<0)
     	return ApplyToClient("%s","111111|向通讯机发送/接收数据出错|");

     if(strncmp(sDataTrans,"0000",4))
     {
        GetMobileErrorInfo(sDataTrans,sDataTrans);
        ApplyToClient("%s",sDataTrans);
        return 0;
     }
         
     ndeslen=8;
     //将MAC密钥转换成BCD码
     ASCIItoBCD(sDataTrans+20,16,sTemp);
     //将MAC密钥用传输密钥解密
     DesDecString(sTransKey,8,sTemp,8,sMACkey,ndeslen);
     //将MAC密钥转换成ASCII形式以便在数据库中存贮而不会引起字符冲突 
     BCDtoASCII(sMACkey,8,sTemp);
     //将今日密钥保存在数据库中
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
     //将认证码转换成BCD码
     ASCIItoBCD(sAuthentication,16,sTemp);
     //将认证码用交换密钥加密
     DesEncString(sTransKey,8,sTemp,8,sAuthentication,&ndeslen);
     //将加密后的认证码转换成ASCII码
     BCDtoASCII(sAuthentication,8,sTemp);
     strcpy(sDataTrans,sBankCode);
     strcpy(sDataTrans+2,sTemp);
     
     //发送签到包
     if(dsMobileSendAndRecv(-1,sDataTrans,TASK_MOBILE_SIGN_OUT,sSendAdd,sDataTrans)<0)
     	return ApplyToClient("%s","111111|向通讯机发送/接数据收出错");
 
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
     	return ApplyToClient("%s","111111|向通讯机发送/接数据收出错");
 
     if(strncmp(sDataTrans,"0000",4))
     {
        GetMobileErrorInfo(sDataTrans,sDataTrans);
        ApplyToClient("%s",sDataTrans);
        return 0;
     }
     
     //此为完全划拨原格式
     /*MobileFormatTransfer(sDataTrans,TASK_MOBILE_COMPLETE_PRINT);
     MobileInsertCompleteYwmx(sLsh,sphone,sJym,sDataTrans);
     sprintf(sDataTrans,"000000|%s|",sLsh);
     return ApplyToClient("%s",sDataTrans);*/    

     /*流水号|用户标志|用户名称|缴款方式|实缴总款额|操作员代号|时分秒|缴款月份数*/
     sprintf(sJsdh,"%-6.6s001",sJym+9);
     GenerateLsh(1,sJsdh,sJym,sLsh);
     GenerateLsh(3,sJsdh,sJym,sTemp);
     nxh=atol(sTemp);
     
     sprintf(inbuf,"%-40.40s|%s|%s|%ld|%.2lf|%s|%ld|%ld|%s",
                    sLsh,sphone,sphone,4,0.0,"0",nSysSfm,nCount,"0");
                    
     if(InsertZwmxTable(inbuf,TASK_MOBILE_PAY_CONFIRM,nxh)<0)
     {
        ApplyToClient("%s","000011|插入帐务明细表出错");
        return -3;
     }
 
     //新格式 
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
		ApplyToClient("%s","000010|返回的发票数据出错");
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

   sOffset=FetchStringToArg(sOffset,   //解读汇总头
   "%4s%15s%59s%12s%1d%1s%12f%12f%12f%4d",
                 ans.sReturn,ans.sBargain,
                 ans.sCompany,ans.sUserName,&ans.nWay,ans.sStatus,
                 &ans.dwczhf,&ans.dxyd,&ans.dyck,&ans.nCount);    //将符串的值分
   strcpy(ans.sRegion,".");
   strcpy(ans.sCountry,".");
   det.dZnj=0.0;
   ans.dbsyzf=0.0;
   sprintf(sJsdh,"%-6.6s001",sJym+9);
   if(ans.nCount==0)    return 0;

   GenerateLsh(1,sJsdh,sJym,slsh);
   for(nId=1;nId<=ans.nCount;nId++)
   {
        //将明细内容解析
        sOffset=FetchStringToArg(sOffset,
                        "%4d%2d%12f%12f%12f%12f%12f%8s%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f",
                        &det.nYear,&det.nMonth,&det.dFee,&det.dBchf,&det.dYhk,
            		&det.dYckhb,&det.dXjk,sStatus,&det.dYzf,&det.dTff,&det.dBwf,
            		&det.dMyf,&det.dCtf,&det.dFjf,&det.dQtf,&det.dXxf,&det.dPzf,
            		&det.dNhf,&det.dBsf); 
        //插入业务明细表结构
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
        ans.dbsyzf=0;  //补收月租费只算在第一个月内
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

函数:preDESconvert;
目的:对长于8字节的数据进行DES加密形成8字节长的MAC序列验证
参数:sKey:   加密密钥[8位BCD码]
     sStream:被加密的字符流,如果其长度不是8的整数倍,将自
             动在其后面补'\0';
     sDest:  被加密后的目标字符串[8位BCD码]              
*----------------------------------------------------*/
void preDESconvert(char * sKey,char * sStream,char * sDest)
{
     
     int nLength,nLoop,nReset;
     char sTemp[9]; 
     char * sOffset;
     int ndeslen;
     sOffset=sStream;
     nLength=strlen(sStream);
     //将字符串扩展成8的整数倍,不够8的整数倍后面补'\0';
     if(nReset=(nLength%8))
       for(nLoop=nLength;nLoop<=nLength+nReset;nLoop++) sStream[nLoop]='\0'; 
     //计算出源字符串共有多少组[每8字节算一组]: 
     nLoop=nLength/8+((nLength%8)!=0);
     //将目标串初始化成8位'\0'
     memset(sDest,'\0',8);
     for(nReset=0;nReset<nLoop;nReset++)
     {    //目标串与源串进行8位异或
          for(nLength=0;nLength<8;nLength++)
          {
               sDest[nLength]^=sOffset[0];
               sOffset++;
          }
          ndeslen=8;
          //将异或后的串用今日密钥加密
          DesEncString(sKey,8,sDest,8,sTemp,&ndeslen); 
          //将加密后的串放入目标串中作为下次异或的初值
          memcpy(sDest,sTemp,8);
     }
}


/*-------------------------------------------------*
函数:ASCIItoBCD;
功能:将十六进制的ASCII码字符串变成BCD码
参数:sASC:ASCII字符串;
     nLen:ASCII字符串长度[偶数];
     sDest:目标串指针;
注意:被转换后的字符串长度只有原来的一半[nLen/2];
*-------------------------------------------------*/

long ASCIItoBCD(char * sASC,int nLen,char * sDest)
{
	
     unsigned char cCode;
     char sTable[17]="0123456789ABCDEF";
     int nSrc,nLoop;
 
     nLoop=0;
     nSrc=0;

     if(nLen<=0) return 1;
     for(nLoop=0;nLoop<nLen;nLoop+=2)  //将十六进制的代码转换成ASCII码
     {
     	cCode=sASC[nLoop];
        if(!strchr(sTable,cCode)) return 1;
        sDest[nSrc]=strchr(sTable,cCode)-sTable;  //得到高位十六进制数据
        sDest[nSrc]<<=4;                        //移向高四位 
 
        cCode=sASC[nLoop+1];                  //得到低四位数据
        if(!strchr(sTable,cCode)) return 1;
        sDest[nSrc]|=(strchr(sTable,cCode)-sTable)&0x0f;//加入低四位数据
       
        nSrc++;
     }
     sDest[nSrc]='\0';
     return 0;	

}

/*-----------------------------------------------------*
函数:BCDtoASCII;
功能:将二进制BCD码转换成一个以十六进制表示的字符串;
参数:sBCD:BCD码原串;
     nLen:BCD码串长度;
     sDest:被转换后的字符串;
注意:目标串的长度是源串长度的2倍;
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
函数：dsMobileSendAndRecv
功能：向通讯机发送包并等待接收;  
参数：nsock:事先建好的socket连接，若nsock<0则由其内部自动建立
      sinbuf:发送包内容
      nopt:交易任务号
      soutstr:返回字符串区域
返回：<0失败；=0成功 >0:建立成功的socket句柄
说明：若soutbuf!=NULL则需要接收返回数据包，否则不需要
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
函数：GetMobileErrorInfo
功能：得到移动返回错误的汉字解释
参数：sError:错误代码[4位]
      sInfo:返回相应错误信息
*===================================================================*/
static void GetMobileErrorInfo(char * sError,char * sInfo)
{
    char sErrCode[5]; 
    int nLen;
    char * sBegin;
    char sMsgTab[]= "1000:101000|银行代码不合法|;"
                    "1001:101001|您所输入的手机号码不存在|;"
                    "1002:101002|未向移动方签到|;"
                    "1003:000012|非签到状态|;"
                    "1004:000012|签到状态，不允许再次签到|;"
                    "1005:101005|未对帐，或签到不允许签到|;"
                    "1006:101006|签到后已经交费，不允许再次签到|;"
                    "1007:101007|非签退状态|;"
                    "1014:101014|对帐时未接收到对帐文件，或对帐文件无法打开|;"
                    "1100:101100|认证码不符!请询问移动方认证码是否有变动|;"
                    "1101:101101|MAC校检不符!请通知成都鸿志公司|;"
                    "1102:101102|交费不足|;"
                    "1103:101103|银行流水号重复|;"
                    "1104:101104|未交费查询交易就进行交费验证|;"
                    "1105:101105|对帐时非法的操作代码|;"
                    "1202:101202|无现金验证交易，就交费确认|;"
                    "1203:101203|确认时交费不足|;"
                    "1204:101204|系统已经签退!|;"
                    "1301:101301|无缴费记录,无法退费|;"
                    "2000:102000|移动方数据库操作失败|;"
                    "2001:102001|移动方无法连接数据库|;"
		    "2002:102002|数据库连接错|;"
		    "2042:102042|生成流水错|;"
		    "2043:102043|修改签到数据失败|;"
		    "2044:102044|插入签到数据失败|;"
		    "2046:102046|对帐时地区代码错|;"
		    "3000:103000|中间件错|;"
		    "3401:103401|手机已销号|;"
                    "7001:107001|数据源服务器等待通讯机返回包失败|;"
                    "8001:108001|无法与移动通讯机建立连接|;"
                    "8004:108004|通讯机接收移动方数据包丢失|;"
                    "8005:108005|接收缓冲区溢出|;"
                    "8007:108006|等待移动公司返回数据包超时|;"
                    "8006:108007|通讯机等待的处理过多，请稍候再试|;"
                    "9001:109001|数据源数据库操作出错|;"
                    "1111:101111|与移动通讯机通信失败|;"
                    "9997:109997|无法建立今日对帐明细文件|;"
                    "9994:109994|无法与通讯机进程mobiled建立连接|;";                

    strncpy(sErrCode,sError,4);
    sErrCode[4]='\0';
    sBegin=strstr(sMsgTab,sErrCode);
    
    if(sBegin&&sBegin[4]==':')
    {
           nLen=strchr(sBegin,';')-sBegin-5;    
           strncpy(sInfo,sBegin+5,nLen);
           sInfo[nLen]='\0';
    }       
    else   sprintf(sInfo,"109999|未知的错误[%s]|",sErrCode);

}


/*================================================
函数:MobileFormatTransfer;
目的:移动代收转换数据包金额格式,将所有金额除以100
参数:sAnswer:移动返回的数据包
     nTaskNo:任务号
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
 	     sOffset=FetchStringToArg(sOffset,            //解读汇总头
                 "%4s%15s%2s%2s%59s%12s%1d%1s%12f%12f%12f%12f%4s%4s%4d",
                 ans.sReturn,ans.sBargain,ans.sRegion,ans.sCountry,
                 ans.sCompany,ans.sUserName,&ans.nWay,ans.sStatus,
                 &ans.dwczhf,&ans.dxyd,&ans.dyck,&ans.dbsyzf,ans.smljqs,
                 ans.sszjqs,&ans.nCount);    //将符串的值分别装入变量中

                 sprintf(sWrite,
                 "%-4.4s%-15.15s%-2.2s%-2.2s%-59.59s%-12.12s%1ld%-1.1s%12.2lf%12.2lf%12.2lf%12.2lf%-4.4s%-4.4s%4d",
                 ans.sReturn,ans.sBargain,ans.sRegion,ans.sCountry,
                 ans.sCompany,ans.sUserName,ans.nWay,ans.sStatus,
                 ans.dwczhf/100.0,ans.dxyd/100.0,ans.dyck/100.0,ans.dbsyzf/100.0,ans.smljqs,
                 ans.sszjqs,ans.nCount);
                 
                 while(sWrite[0]!='\0') sWrite++;
                 //解读明细帐
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
 	     sOffset=FetchStringToArg(sOffset,            //解读汇总头
                 "%4s%15s%59s%12s%1d%1s%12f%12f%12f%4d",
                 ans.sReturn,ans.sBargain,ans.sCompany,ans.sUserName,&ans.nWay,
                 ans.sStatus,&ans.dwczhf,&ans.dxyd,&ans.dyck,&ans.nCount);    //将符串的值分别装入变量中
                 sprintf(sWrite,
                 "%-4.4s%-15.15s%-59.59s%-12.12s%1ld%-1.1s%12.2lf%12.2lf%12.2lf%4d",
                 ans.sReturn,ans.sBargain,ans.sCompany,ans.sUserName,ans.nWay,ans.sStatus,
                 ans.dwczhf/100.0,ans.dxyd/100.0,ans.dyck/100.0,ans.nCount);

                 while(sWrite[0]!='\0') sWrite++;
                 //解读明细帐
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
函数：dsTaskMobilePayQuery
功能：移动手机代收费查询程序
参数：slsh:流水号
      smap:检索条件位图[5字符]
      syhbz:检索条件字符串[5*20字符]
      sJym:交易码[15位]
      Client:回带参数记录指针
*=================================================================*/
long dsMobilePayQuery(char * inbuf,RECORD * Client)
{

     char sBzmap[6],sJym[16],sLsh[41],sYhbz[101],sJsdh[10],sCzydh[7];
     char sBuffer[TOTAL_PACKAGE_SIZE];
     char sBankCode[9];   //银行代码（dl$jymcsfb,nbh=36)
     char sSendAdd[10];
     CHARGEDETAIL det;
     MOBILEANSWER ans;
  
     FetchStringToArg(inbuf,"%s|%s|%s|%s|",sLsh,sBzmap,sYhbz,sCzydh);
     FetchStringToArg(inbuf,"%9s%15s",sJsdh,sJym);

     sYhbz[11]='\0';
     memset(sBuffer,'\0',TOTAL_PACKAGE_SIZE);
   
     //正面的函数提供按交易码进行互斥操作，CommitWork/RollbackWork解锁
          
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=36 into %s",sJym,sBankCode);
     RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=40 into %s",sJym,sSendAdd);
   
     sprintf(sBuffer,"b000%-11.11s%-8.8s",sYhbz,sBankCode);

     if(dsMobileSendAndRecv(-1,sBuffer,TASK_MOBILE_PAY_QUERY,sSendAdd,sBuffer)<0)
     {
     	ImportRecord(Client,"%s%s","111111","向通讯机发送/接数据收出错");
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
函数：MobileInsertYwmx
功能：移动将查询结果放入业务明细表
参数： slsh:流水号
      sPhone:电话号码
      sJym:交易码
      sCzydh:查询操作员代号
      sDetail:移动返回的详细内容
      Client:返回给客户端的RECORD结构

*===============================================================*/

static long MobileInsertYwmx(char * slsh,char * sCzydh,char * sPhone,char * sJym,char * sDetail,RECORD * Client)
{
   CHARGEDETAIL det;
   MOBILEANSWER ans;
   
   char sCode[]="ABDGHIGKLab";
   char * curstat[]={"正常","冒高","欠停","报停","挂失","预销","预拆","强开","强关","销号","局拆"};
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
   sOffset=FetchStringToArg(sOffset,            //解读汇总头
                "%4s%15s%2s%2s%59s%12s%1d%1s%12f%12f%12f%12f%4s%4s%4d",
                 ans.sReturn,ans.sBargain,ans.sRegion,ans.sCountry,
                 ans.sCompany,ans.sUserName,&ans.nWay,ans.sStatus,
                 &ans.dwczhf,&ans.dxyd,&ans.dyck,&ans.dbsyzf,ans.smljqs,
                 ans.sszjqs,&ans.nCount);    //将符串的值分别装入变量中

   strncpy(sStatus,curstat[strchr(sCode,ans.sStatus[0])-sCode],4);
   sStatus[4]='\0';
   FetchStringToArg(slsh,"%9s",sJsdh);
   ImportRecord(Client,"%s%s%s%lf%ld","000000",ans.sCompany,ans.sBargain,ans.dyck,((ans.nCount==0)?1:ans.nCount));
   nId=((ans.nCount==0)?0:1);

   det.nYear=(nSysLkrq/10000);
   det.nMonth=(nSysLkrq%10000)/100;
   dAddition=0.0;
   //if(ans.nCount>0 && ans.nWay==9) dAddition=(ans.dwczhf+50-ans.dyck);//预存话费用户
   if(ans.nCount>0 && ans.nWay==9) dAddition=(ans.dwczhf-ans.dyck);//新预存话费用户
   nValue[0]=ans.nWay;   //长整型保留字段
   
   for(;nId<=ans.nCount;nId++)
   {
        if(ans.nCount>0)
        {
            //将明细内容解析
            sOffset=FetchStringToArg(sOffset,
            "%4d%2d%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f%12f",
            &det.nYear,&det.nMonth,&det.dFee,&det.dBchf,&det.dZnj,&det.dYhk,
            &det.dYckhb,&det.dXjk,&det.dYzf,&det.dTff,&det.dBwf,&det.dMyf,
            &det.dCtf,&det.dFjf,&det.dQtf,&det.dXxf,&det.dPzf,&det.dNhf,&det.dBsf); 
        }
        //并将明细内容填充到返回结构中
        //用户实缴款=本月欠费+滞纳金+补收月租费(只加第一个月)+预存用户新增(只加最后一个月)
        dSjk=det.dFee+det.dZnj+ans.dbsyzf;
        dTemp=dSjk;
        if(nId==ans.nCount) dSjk+=dAddition;
        //应缴总款(发票单大写)=本次通话费-充值卡划拨+滞纳金+补收月租费-优惠款
        dYjzk=det.dBchf-det.dXjk+det.dZnj+ans.dbsyzf-det.dYhk;
        if(dYjzk<0.0) dYjzk=0.0;
        if((dSjk-(long)dSjk)>0.0) dSjk=(double)((long)dSjk+1);
        //导入客户端RECORD结构
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
      
        //插入业务明细表结构
        nLoop=2;
        dTotalSjk+=dSjk;
        //加入2次业务明细插入循环，以保证移动数所包存在非法字符时也能正确插入
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
        ans.dbsyzf=0.0;  //补收月租费只算在第一个月内
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
函数：dsTaskMobilePayConfirm
功能：移动缴款请求模块
参数：inbuf:缴款输入参数字符串序列
      outbuf:缴费结果返回
返回：0成功，否则失败
*============================================================*/
long dsTaskMobilePayConfirm(char * inbuf,char * outbuf)
{
     char sTemp[10000],sMAC[17],sMACkey[17],sPrName[20];     
     char sLsh[41];  //流水号
     char sYhbz[101]; //用户标志
     char sJym[16];  //交易码
     char sYhmc[51]; //用户名称
     char sCzydh[7]; //操作员代号
     char sJsdh[10]; //缴费局局号
     char sSendAdd[10],sBankCode[9];
     char sBuffer[10000];
     char sRegion[3],sCountry[3];  //地区代码与市县代码
     char * sOffset;
     long nMode;     //缴款方式 1：现金  2：绿卡
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
     
     //得到帐务明细外部流水号
     
     GenerateLsh(3,sJsdh,sJym,sBuffer);
     nxh=atol(sBuffer);
     
     //生成MAC验证序列
     sprintf(sTemp,"%8.8s%08ld%11.11s%12ld%08ld%06ld",
                    sBankCode,nxh,sYhbz,(long)dSjk*100,nSysLkrq,nSfm);
     //生成MAC校检
     preDESconvert(sMACkey,sTemp,sMAC);
     //将最终的MAC校检序列转换成ASCII序列以便传输
     BCDtoASCII(sMAC,8,sTemp);
     //将加密的数据附在侍传数据
     sprintf(sBuffer,
             "b000%-2.2s%-2.2s%-8.8s%08ld%-11.11s%12ld%08ld%06ld%-16.16s",
             sRegion,sCountry,sBankCode,nxh,sYhbz,(long)dSjk*100,
             nSysLkrq,nSfm,sTemp);

     //将数据发往移动进行验证
     
     if(dsMobileSendAndRecv(-1,sBuffer,TASK_MOBILE_PAY_CHECK,sSendAdd,sTemp)<0)
     {
        RollbackWork();
     	strcpy(outbuf,"111111|向通讯机发送/接数据收出错|");
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
        sprintf(outbuf,"000011|插入帐务明细表出错：%s|",sTemp);
        return -3;
     }
     CommitWork();

     /* 新的发票打印方式 */

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

     //建立移动对帐文件
     nPkgCount=dsMobileFormAccountFile(inbuf,sFileName,&nsize);
     if(nPkgCount<=0)
     {
        strcpy(outbuf,"000015|建立移动对帐文件文件出错|");
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
     	strcpy(outbuf,"100008|向通讯机发送/接数据收出错|");
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
     	  //新增日志[写入dlywd下mob_accnt.log文件中
     	 sprintf(sBuffer,"[%ld]对帐错误代码:%s\n",nSysLkrq,sFileName);
     	 if(!(fp=fopen("mob_accnt.log","at"))) return -1;
     	 fwrite(sBuffer,strlen(sBuffer),1,fp);
     	 fclose(fp);
     }
     return 0;
}


static long dsMobileFormAccountFile(char * inbuf,char * sFileName,long * pnSize)
{
     char sTemp[256],sMAC[17],sMACkey[17],sBuffer[256];     
     char sRegion[3],sCountry[3],sFile[31],sBankCode[10];  //地区代码与市县代码
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

        if(nCzlb==0 || nCzlb==2)  //缴费
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
         else if(nCzlb==1) //撤单
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
函数:dsMobileSendPayDetail;
目的:与移动对明细帐;
参数:nSocket:与通讯机建立的连接;
     sAnswer:输入对帐文件名,输出移动应答; 
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
函数：dsMobileRecallConfirm
功能：移动撤单确认请求
参数：inbuf:撤单请求参数报文，具体内容如下
      交易流水号|被撤单流水号|操作员代号|时分秒|
      nxh:与委托方撤单记录序号
      outbuf:返回给客户端的成功或失败信息
      
返回：0成功，否则失败
*========================================================*/
long dsMobileRecallConfirm(char * inbuf,long nxh,char * outbuf)
{
     char sTemp[256],sMAC[17],sMACkey[17];     
     char sLsh[41],sRecallLsh[41];  //流水号
     char sYhbz[101]; //用户标志
     char sJym[16];  //交易码
     char sCzydh[7]; //操作员代号
     char sJsdh[10]; //缴费局局号
     char sSendAdd[10],sBankCode[9];
     char sBuffer[256];
     char sRegion[3],sCountry[3];  //地区代码与市县代码
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

     //撤单申请  
     GetSysLkrq();
     nSfm=nSysSfm;
     sprintf(sBuffer,
             "b002%-2.2s%-2.2s%-8.8s%08ld%-11.11s%08ld%06ld",
             sRegion,sCountry,sBankCode,nRecallXh,sYhbz,nSysLkrq,nSfm);
             
     if(dsMobileSendAndRecv(-1,sBuffer,TASK_MOBILE_RECALL,sSendAdd,sBuffer)<0)
     {
        RollbackWork();
     	strcpy(outbuf,"111111|向通讯机发送/接数据收出错|");
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
     	strcpy(outbuf,"111111|向通讯机发送/接收数据收出错|");
        return -1;
     }
          
     if(strncmp(sBuffer,"0000",4))
     {
     	GetMobileErrorInfo(sBuffer,outbuf);
     	RollbackWork();
     	return -2;
     }
     sprintf(outbuf,"000000|移动撤单成功|");
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
           switch(max_row)  //此处是加行空格处
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
           	case '-':     //左靠齐
           	     increment = 0;
           	     break;
           	case '+':     //右靠齐
           	     increment =width-strlen(value);
           	     break;
           	default:      //居中
           	     increment = (width-strlen(value))/2;
           	     break;
           }
           if(increment<0) increment=0;
           strcpy(pointer+increment+3,value);  //此处加列空格数
           pointer+=width;
        }   	
    }//end for   
    return count;
}
