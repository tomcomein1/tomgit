/*==========================================================

frame.h: 定义菜单中所有的文件说明     


*=========================================================*/

long dlywxt();                  //代理系统主入口
void FileExchange();            //文件交换管理 
void SystemManage();            //系统管理
void Transaction();             //业务办理
void GoodsManage();             //代售商品管理
void QueryAndStatistic();       //查询与统计
void AccountManage();           //帐务管理
void SystemHelp();              //系统帮助
void ChargeConfirm();           //缴款
void ReprintCzyJkd();           //打印缴款单
void SystemSetup();             //系统设置
void JymSetup();                //交易码
void RouteManage();             //路由管理
void SystemMaintenance();       //系统维护
void JymManage();               //交易码管理
void OpenManage();              //交易开放管理
void CompanyManage();           //公司管理
void YwzlManage();              //业务种类管理
void BillFormatManager();       //套打单据管理
void ExportPrintTable();        //套打单据导出
void UserManage();              //操作员管理
void JuHaoManage();             //局号管理
void DaiShouYw();               //代收业务办理
void SellGoods();               //代售商品办理
void DaiBanYw();                //代办业务办理
void AddDsJym();                //增加代收交易
void AddSpJym();                //增加代售交易
void AddDbJym();                //增加代办交易
void AddYdJym();				//增加异地交易
void DeleteJym();               //删除交易
void CxExistJym();              //已存在交易码查询
void XianJuJuHao();             //县局局号管理
void ZhiJuJuHao();              //支局局号管理
void XianJuHaoDuiZhao();        //县局局号对照设置
void ZhiJuHaoDuiZhao();         //支局局号对照设置
void PurchaseGoods();           //进货
void ReturnGoods();             //向委托方退货
void AcceptGoods();             //接货
void DistributeGoods();         //发货
void RecallGoods();             //用户退货
void WastingGoods();            //报损
void ChangeUserPassword();      //更改操作员口令
void SpecialOperation();        //特殊录入及处理
void DatabaseBackup();          //数据库备份
void DatabaseRestore();         //恢复备份
void CleanupHistory();          //清空历史数据
void Others();                  //其它
void tb_menu();                 //电话银行前置机业务
void AirInfoManage();
void AirCityManage();           //城市代码管理
void DownLoadAirInfo();         //城市局接收省局民航资料
void AirRebateManage();          //机票折扣管理
void AirCompanyManage();     
void FileDownload();           //国税文件生成
void Tax_Xjkzb();                //国税对照表
void Tax_Khhgl();				//国税开户行管理
void AccessAccount();
void Terra_Jc();

