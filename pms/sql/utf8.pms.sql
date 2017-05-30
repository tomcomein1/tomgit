/*--use db: poptomdb*/
create database IF NOT EXISTS poptomdb;

create user 'poptom'@'localhost' identified by 'poptom321';
grant all on poptomdb.* to 'poptom'@'localhost'; 
FLUSH PRIVILEGES;
use poptomdb;

/*--用户信息表*/
DROP TABLE IF EXISTS p_user;
CREATE TABLE IF NOT EXISTS p_user
(
    userid  smallint(5) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户编号', 
    user_phone varchar(30) NOT NULL COMMENT '用户电话',  
    user_name varchar(30) NOT NULL COMMENT '用户姓名', 
    user_pass varchar(30) COMMENT '用户密码',
    user_auth char(1) NOT NULL default '0' COMMENT '用户权限',  
    PRIMARY KEY(userid)
)engine=MyISAM; 

/*--渠道表*/
DROP TABLE IF EXISTS p_channel ;
CREATE TABLE IF NOT EXISTS p_channel
(
    chnlid  smallint(5) unsigned NOT NULL AUTO_INCREMENT COMMENT '渠道ID',
    chnl  char(10) NOT NULL COMMENT '渠道类型',
    chnl_name  varchar(30) COMMENT '渠道名称', 
    PRIMARY KEY(chnlid)
)engine=MyISAM;

/*--套系表*/
DROP TABLE IF EXISTS p_suit ;
CREATE TABLE IF NOT EXISTS p_suit
(
    suitid smallint(5) unsigned NOT NULL AUTO_INCREMENT COMMENT '套系ID',
    suit  char(10) NOT NULL COMMENT '套系类型',
    suit_name  varchar(30) COMMENT '套系名称', 
    suit_amt decimal(10,2) COMMENT '套系金额',
    PRIMARY KEY(suitid)
)engine=MyISAM;

/*--商品信息}*/
DROP TABLE IF EXISTS p_goods;
CREATE TABLE IF NOT EXISTS p_goods
(
    goodsid smallint(5) unsigned NOT NULL AUTO_INCREMENT COMMENT '商品编号', 
    goods_name char(30) NOT NULL COMMENT '商品名称',
    goods_number smallint(5) COMMENT '商品数量',
    goods_stand char(20) COMMENT '商品规格',
    goods_memo char(60) COMMENT '商品备注',
    PRIMARY KEY (goodsid)
)engine=MyISAM;
/*--会员表*/
DROP TABLE IF EXISTS p_member;
CREATE TABLE IF NOT EXISTS p_member
(
    mbid smallint(5) unsigned NOT NULL AUTO_INCREMENT COMMENT '会员ID',
    mb_no char(22) NOT NULL COMMENT '会员编号',
    mb_level smallint(3) COMMENT '会员等级',
    mb_explain char(20) COMMENT '会员说明',
    PRIMARY KEY(mbid)
)engine=MyISAM;

/*-- 客户信息表p_custom */
DROP TABLE IF EXISTS p_custom;
CREATE TABLE IF NOT EXISTS p_custom
(
    custid mediumint(8) unsigned NOT NULL AUTO_INCREMENT COMMENT '客户编号',
    cust_name  char(60) NOT NULL COMMENT '客户姓名',
    cust_phone char(30) NOT NULL COMMENT '客户电话',
    baby_name char(60) COMMENT '宝贝姓名',
    nick_name char(20) COMMENT '宝贝小名',
    baby_birth char(10) COMMENT '宝贝生日',
    baby_sex char(1) default '0' COMMENT '宝贝性别', 
    qq varchar(20) default '' COMMENT '客户QQ号码',
    addr varchar(128) COMMENT '客户地址',
    chnl char(10) COMMENT '来源渠道',
    mb_no char(22) COMMENT '会员编号', 
    memo varchar(120) COMMENT '备注信息',
    reg_date char(10) COMMENT  '登记日期',
    datetime char(22) COMMENT '最后修改时间',
    PRIMARY KEY (custid)
)engine=MyISAM;
CREATE INDEX idx_cust_name ON p_custom(cust_name);

/*--预约登记表this.custid=custom.custid,this.chnl_type=channl.chnl_type*/
DROP TABLE IF EXISTS p_booking;
CREATE TABLE IF NOT EXISTS p_booking
( 
    bookid mediumint(8) unsigned NOT NULL AUTO_INCREMENT COMMENT '预约编号',
    book_date char(10) NOT NULL COMMENT '预约拍摄日期',
    book_time char(6)  COMMENT '预约拍摄时间',
    suit char(20)  COMMENT '预约套系',
    chnl char(10) COMMENT '渠道类型',
    order_no char(20) NOT NULL COMMENT '订单编号',
    custid  mediumint(8) NOT NULL COMMENT '客户编号',
    book_memo varchar(60) COMMENT '预约备注',
    datetime char(22) COMMENT '最后修改时间',
    PRIMARY KEY(bookid)
)engine=MyISAM;
CREATE INDEX idx_book_oc ON p_booking(order_no, custid);

/*--拍摄情况登记表,this.bookid=pbooking.bookid,this.custid=custom.custid*/
DROP TABLE IF EXISTS p_photography;
CREATE TABLE IF NOT EXISTS p_photography 
( 
    photoid mediumint(8) unsigned NOT NULL AUTO_INCREMENT COMMENT '拍摄编号', 
    photo_date  char(10) NOT NULL COMMENT '拍摄日期',
    photo_man varchar(10) COMMENT '拍摄人',
    photo_memo varchar(120) COMMENT '拍摄备注',
    bookid mediumint(8) COMMENT '预约编号',
    custid  mediumint(8) NOT NULL COMMENT '客户编号',
    order_no char(20) COMMENT '订单编号', 
    datetime char(22) COMMENT '最后修改时间',
    PRIMARY KEY(photoid)
)engine=MyISAM;
CREATE INDEX idx_photo_cbo ON p_photography(custid, bookid, order_no);

/*--账务登记薄*/
DROP TABLE IF EXISTS p_acctreg;
CREATE TABLE IF NOT EXISTS p_acctreg
( 
    regid mediumint(8) unsigned NOT NULL AUTO_INCREMENT COMMENT '账务编号', 
    acct_date char(10) NOT NULL COMMENT '账务日期', 
    dr_cr char(1) NOT NULL default '2' COMMENT '借贷类型', 
    acct_amt  decimal(15,2) NOT NULL COMMENT '发生金额', 
    pay_mode varchar(10) COMMENT '收付方式',
    pay_man  varchar(10) COMMENT '收付款人',
    reg_man  varchar(10) COMMENT '记账人',
    user_phone varchar(15) COMMENT '用户号',
    reg_memo varchar(120) COMMENT '记账备注',
    reg_date char(10) COMMENT '登记日期', 
    is_valid char(1) default 'Y' COMMENT '账务有效标志', 
    datetime char(22) COMMENT '最后修改时间',
    PRIMARY KEY(regid)
)engine=MyISAM;
CREATE INDEX idx_acct_date ON p_acctreg(acct_date);

/*--客户订单信息跟踪表*/
DROP TABLE IF EXISTS p_order;
CREATE TABLE IF NOT EXISTS p_order
(
    orderid mediumint(8) unsigned NOT NULL AUTO_INCREMENT COMMENT '订单ID',
    order_no char(20) NOT NULL COMMENT '订单编号', 
    order_date char(10) NOT NULL COMMENT '订单日期',
    order_cont char(60) COMMENT '订单内容',
    first_date char(10) COMMENT '初修日期',
    cp_date char(10) COMMENT '选片日期',
    second_date char(10) COMMENT '精修日期',
    pm_date char(10) COMMENT '制版日期',
    get_photo char(10) COMMENT '取件日期',
    get_method char(22) COMMENT '取件方式',
    order_stat char(1) default '0' COMMENT '订单状态',
    order_memo char(120) COMMENT '订单备注',
    datetime char(22) COMMENT '最后修改时间',
    PRIMARY KEY(orderid)
)engine=MyISAM;
CREATE INDEX idx_order_no ON p_order(order_no);
