long GetSysUserName();
long GetSysPassWord();
void ShowInstallError(char * str);
void InstallDlyw();   //代理系统初始化安装
void UnInstallDlyw(); //卸装代理系统
long CreateDlywTable();//建表
MENU * CreateJhMenu();
void Finished();
void ReadIni();
long CreatePrint();     //打印单据
long CreateCzyb();      //操作员
long CreateJymkzfb();   //交易开展
long CreateJymcsfb();   //交易参数
long CreateJym();       //交易码
long CreateZwmx();      //帐务明细
long CreateJkd();       //缴款单
long CreateGs();        //公司编号及名称
long CreateDlywcs();    //代理系统参数
long CreateYwzl();      //业务种类
long CreateHwjcb();     //货物进出表
long CreateHwkcb();     //货物库存表
long CreateLkzhgl();    //绿卡帐号
long CreateLkzjhd();    //绿卡资金
long CreateRoute();     //路由表
long CreateSequence();  //建立序列
long CreaetJh();        //局号
long CreateAirCity();   //航空城市对照表
long CreateAirRebate(); //折扣管理表
long CreateAirCompany();
