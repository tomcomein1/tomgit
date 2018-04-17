#ifndef TELECOM_H
#define TELECOM_H

#define YADX_PKG_LEN sizeof(yadx_Pkg)       //数据长

typedef   struct   {
    long    lsh;			//银行主机流水号（回传时为包的长度）
    char    zh1[22];			//储蓄帐号１(回传时为收费工号)
    char    zh2[22];			//储蓄帐号２(回传时为收费日期YYYYMMDD)
    char    mm1[16];			//密码１
    char    mm2[16];			//密码２
    char    sfzh[19];			//身份证号
    char    dwbh[6];			//代收单位单位编号
    char    sfzsy[13];			//收费主索引（电话号码）
    char    sffsy[13];			//收费副索引（合同号码）
    char    yhm[17];			//用户名（对帐时为对帐日期YYYYMMDD）
    double  qfhj;			//欠费合计
    double  dsfhj;			//代收费合计
    double  znj;			//滞纳金合计
    char    bz[2];			//标志
    long    jyfs;			//交易方式
    char    dsjg[10];			//代收机构
    long    sfrq;			//收费日期
    long    sfsj;			//收费时间
    long    czy;			//操作员
    char    xym[3];			//响应码
    char    xymms[41];			//响应码描述
    double  zje;			//总金额
} yadx_Pkg;

#endif
