#ifndef T_B2I_H
#define T_B2I_H

#define CODESIZ	                        6     //交易码长度
/******************************************
* 交易信息: 保险公司与银行的通信信息一至  *
******************************************/
typedef struct
{
       /*****************************************
	* 控制结构
       *****************************************/
       char MsgLength[7];        //信息长度
       char CtlChar;                //控制字符号
       char TransCode[CODESIZ+1];   //交易码
            /*500101-----查询(根据保单号)*/
            /*500201-----缴费提交*/
            /*500301-----补打收据*/
            /*500401-----对账处理*/
            /*500501-----冲正处理*/
       char status[5];             //状态信息
            /*0000 --- 正确返回*/
            /*0001 --- 交易码非法*/
            /*900x --- 数据异常*/ 
                   //需做特别处理:
           /*  -X=tongXin_Struct结构中的chaXunJieGuo */
           /*  5000 -- 非法冲正数据 
               5001 -- 冲正失败
               5002 -- 有预收记录,冲正时要求先冲正预收,再冲正正常续收 */  
      //---------------------------------------------------------------         
        char sJylx[3]; 		      //交易类型   
             /*A1--根据保单号查询     B1--交费提交*/
             /*C1--补打收据           D1--对账    */
             /*E1--冲正                           */ 
        char sYhdm[5];                //银行代码
             /*8381--建行             8382--农行*/
             /*8383--工行             8384--中行*/
        char sWddm[10],		      //网点代码,银行提供
             sBdh[23],		      //保单号
             sYsfrq[11],              //应收费日期 
             sBxmc[41],               //保险名称
             sTbr[65];                //投保人
        char cJffs;                   //缴费方式 M--月交 Q--季交 H--半年交 Y--年交
        long nJfnq;      //缴费年期
        long nJfcs;      //缴费次数
        double dYjbf,             //予交保费
               dBf,               //保费
               dYjfye,            //予交费余额
               dLx;               //利息
        char sSxrq[11];                //生效日期
        long nBdnd;      //保单(经过)年度
        char sYwydm[6],                //业务员代码
             sYwy[21],                 //业务员
             sCzydm[5],                //操作员代码
             sCzy[9],                  //操作员姓名
             sSfsj[20],                //收费时间
             sCzsj[20],                //冲正时间
             sSffs,                    //收费方式 
                                       /*银行收费方式:0-现金 1-支票*/
				       /*             2-转账支票   */
             sZph[21],                 //  支票号       
             sSjysh[13];               //  收据印刷号
        char cCxjg;                    //  查询结果
        char bank_no[11];              //银行流水号            
} DATA_PKG;  

/**********
*对账结构 *
***********/
typedef struct
{
        char sBdh[23],
             sYhdm[5],
             sWddm[10],
             sJylx[3],
             sJysj[20],
             sCzydm[5];
        double dSfje;
        char sSjysh[13];
        char bank_no[11];
} duiZhang_Struct;

static long dsBxSendAndRecv();
static void getBxErrorInfo(char *sErrorInfo); 

  #ifndef TEST
    #define TEST
  #endif

#endif
