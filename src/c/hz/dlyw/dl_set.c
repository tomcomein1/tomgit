#include"setting.h"

//系统设置
void SystemSetup()
{
     SETTABLE * pst;

     pst=CreateSetTable(8);
     AddSetItem(pst,"本局IP地址","ccs",15,"from dl$dlywcs where nbh=3");

//   AddSetItem(pst,"上级局IP地址","ccs",16,"from dl$dlywcs where nbh=6");
  
     AddSetItem(pst,"是否允许综合网操作员自动登记(1:允许 0:不允许)",
                     "ccs",1,"from dl$dlywcs where nbh=8");
  
     AddSetItem(pst,"是否需要动态连接数据库(1:动态  0:静态)",
                     "ccs",1,"from dl$dlywcs where nbh=10");

     LoadDefaultSet(pst);
     clrscr();
     outtext("\n                           代 理 系 统 参 数 设 置");
     BeginSetting(pst,4);
     DropSetTable(pst);
}

//根据交易码的程序模式设置相关参数
void JymSetup()
{

     char sJymc[51],sJym[16];
     SETTABLE * pst;
     long nCxms;

     clrscr();
     
     if(SelectJym(sJym,0)<=0) return;
     	RunSelect("select ncxms,cjymc from dl$jym where cjym=%s into %ld%s",sJym,&nCxms,sJymc);
     pst=CreateSetTable(8);

     //根据不同的程序模式增加相应的设置内容;
     if(sJym[8]=='0' && sJym[0]=='0' && sJym[1]=='1')
     {

          AddSetItem(pst,"委托方文件通讯机用户名","ccs",20,
                           "from dl$jymcsfb where nbh=49 and cjym=%s",sJym);

          AddSetItem(pst,"委托方文件通讯机口令","ccs",20,
                           "from dl$jymcsfb where nbh=48 and cjym=%s",sJym);
     }
     switch(nCxms)
     {
          case 111:
               AddSetItem(pst,"代理机构号","ccs",8,
                              "from dl$jymcsfb where nbh=39 and cjym=%s",sJym);

               AddSetItem(pst,"预存款日结帐时间(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=42 and cjym=%s",sJym);

               AddSetItem(pst,"预存款缴费文件接收目录","ccs",30,
                              "from dl$jymcsfb where nbh=47 and cjym=%s",sJym);
     
               AddSetItem(pst,"预存款日结帐文件接收目录","ccs",30,
                              "from dl$jymcsfb where nbh=46 and cjym=%s",sJym);
               {
                  long limited=0;
                  char sTitle[50];
                  long index;
                   
                  AddSetItem(pst,"是否需要限制号码段(0:不限制,1:限制)","ccs",1,
                              "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);

                  RunSelect("select to_number(ccs) from dl$jymcsfb "
                  "where cjym=%s and nbh=1 into %ld",sJym,&limited);
 
                  for(index=0;index<30 && limited==1;index++)
                  {
                     sprintf(sTitle,"开放号码段%ld",index+1);
                     AddSetItem(pst,sTitle,"ccs",11,
                     "from dl$jymcsfb where nbh=%ld and cjym=%s",index+2,sJym);
                  }  
               }
               
               break;

          case 110:

               AddSetItem(pst,"代理机构号","ccs",8,
                              "from dl$jymcsfb where nbh=39 and cjym=%s",sJym);

               AddSetItem(pst,"代收每日结帐时间(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=42 and cjym=%s",sJym);

               AddSetItem(pst,"月结帐日期(DD)","ccs",2,
                              "from dl$jymcsfb where nbh=41 and cjym=%s",sJym);

               AddSetItem(pst,"代收费日结帐文件接收目录","ccs",30,
                              "from dl$jymcsfb where nbh=47 and cjym=%s",sJym);
     
               AddSetItem(pst,"代收费月结帐文件接收目录","ccs",30,
                              "from dl$jymcsfb where nbh=46 and cjym=%s",sJym);

               break;               

          
          case 130://移动交易码的相关设置
               AddSetItem(pst,"银行代码(由移动分配)","ccs",8,
                              "from dl$jymcsfb where nbh=36 and cjym=%s",sJym);
               
               AddSetItem(pst,"交易初始认证码(由移动分配)","ccs",16,
                              "from dl$jymcsfb where nbh=37 and cjym=%s",sJym);
               
               AddSetItem(pst,"交易初始化传输密钥(由移动分配)","ccs",8,
                              "from dl$jymcsfb where nbh=38 and cjym=%s",sJym);
               
               AddSetItem(pst,"发起地地址标识(由移动分配)","ccs",9,
                              "from dl$jymcsfb where nbh=40 and cjym=%s",sJym);

               AddSetItem(pst,"自动对帐时间(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=35 and cjym=%s",sJym);
               break;                

          case 140://银海电信
          case 141:
               AddSetItem(pst,"邮政用户名(由电信分配)","ccs",8,
                              "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
               
               AddSetItem(pst,"邮政密码(由电信分配)","ccs",8,
                              "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
	       if(nCxms==141)
			AddSetItem(pst,"单位编号(由电信分配)","ccs",8,
                             "from dl$jymcsfb where nbh=3 and cjym=%s",sJym);
                              
			AddSetItem(pst,"自动对帐时间(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=35 and cjym=%s",sJym);
               break;

         case 610://天然气
               AddSetItem(pst,"民用气量每日滞纳金率","ccs",8,
                              "from dl$jymcsfb where cjym=%s and nbh=10",sJym);

               AddSetItem(pst,"商用气量每日滞纳金率","ccs",8,
                              "from dl$jymcsfb where cjym=%s and nbh=11",sJym);
               break;

         case 821:
               AddSetItem(pst,"银行代码(由保险分配)","ccs",4,
                              "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);

               AddSetItem(pst,"网点代码(由邮政分配)","ccs",9,
                              "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
               
               AddSetItem(pst,"自动对帐时间(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=35 and cjym=%s",sJym);
               break;

         case 1001://民航
               AddSetItem(pst,"是否停止监控（y/n)","ccs",1,
                    "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
               AddSetItem(pst,"是否自动扩展航位信息(y/n)","ccs",1,
                    "from dl$jymcsfb where nbh=10 and cjym=%s",sJym);
               AddSetItem(pst,"是否自动扩展查询信息(y/n)","ccs",1,
                    "from dl$jymcsfb where nbh=11 and cjym=%s",sJym);

 	       //AddSetItem(pst,"业务级别  (1.省 2.地 3.县)","ccs",1,
           //        "from dl$jymcsfb where nbh=25 and cjym=%s",sJym);
           
           //AddSetItem(pst,"是否指定操作员代号(y/n)","ccs",1,
           //        "from dl$jymcsfb where nbh=21 and cjym=%s",sJym);
           //AddSetItem(pst,"指定操作员代号","ccs",6,
           //        "from dl$jymcsfb where nbh=22 and cjym=%s",sJym);
           //AddSetItem(pst,"指定操作员数量","ccs",2,
           //        "from dl$jymcsfb where nbh=23 and cjym=%s",sJym);

           /*AddSetItem(pst,"指定打印单据文件名","ccs",10,
                   "from dl$jymcsfb where nbh=50 and cjym=%s",sJym);*/
               break;

         case 230:
         case 220:
         case 210:
               AddSetItem(pst,"127机型优惠几月当一年","ccs",2,
                    "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
               AddSetItem(pst,"96167机型优惠几月当一年","ccs",2,
                    "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
               AddSetItem(pst,"129机型优惠几月当一年","ccs",2,
                    "from dl$jymcsfb where nbh=3 and cjym=%s",sJym);
			   break;
		
         case 521:
         case 522:
               AddSetItem(pst,"国税本月代收截止日期","ccs",8,
                    "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
		       AddSetItem(pst,"银行代码(由国税局分配)","ccs",10,
		            "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
               AddSetItem(pst,"增值税预算级次","ccs",16,
                    "from dl$jymcsfb where nbh=3 and cjym=%s",sJym);
               AddSetItem(pst,"增值税编码","ccs",8,
                    "from dl$jymcsfb where nbh=4 and cjym=%s",sJym);
               AddSetItem(pst,"消费税预算级次","ccs",16,
                    "from dl$jymcsfb where nbh=5 and cjym=%s",sJym);
               AddSetItem(pst,"消费税编码","ccs",8,
                    "from dl$jymcsfb where nbh=6 and cjym=%s",sJym);
		AddSetItem(pst,"缴款书是否打印支局名(Y/N)","ccs",1,
                    "from dl$jymcsfb where nbh=7 and cjym=%s",sJym);
	
               break;
         
         case 123:
         case 124:
              AddSetItem(pst,"银行代码(由供电局分配)","ccs",4,
                              "from dl$jymcsfb where nbh=36 and cjym=%s",sJym);
              AddSetItem(pst,"邮政通信机密码","ccs",8,
              				  "from dl$jymcsfb where nbh=3 and cjym=%s",sJym);
              AddSetItem(pst,"电业局帐户(由供电局分配)","ccs",6,
              				  "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
              AddSetItem(pst,"电业密码(由供电局分配)","ccs",6,
              				  "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
              break;
	
	case 222:
	case 223:
              AddSetItem(pst,"地税本月代收截止日期","ccs",8,
                    "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
	      AddSetItem(pst,"定额发票工本费(本)","ccs",12,
                    "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
	      
	      AddSetItem(pst,"超额税率","ccs",12,
                    "from dl$jymcsfb where nbh=3 and cjym=%s",sJym);
              AddSetItem(pst,"饮食,美容美发税率","ccs",12,
                    "from dl$jymcsfb where nbh=4 and cjym=%s",sJym);
	      AddSetItem(pst,"广告业税率","ccs",12,
                    "from dl$jymcsfb where nbh=5 and cjym=%s",sJym);
              AddSetItem(pst,"其他服务业税率","ccs",12,
                    "from dl$jymcsfb where nbh=6 and cjym=%s",sJym);
	      AddSetItem(pst,"娱乐业税率","ccs",12,
                    "from dl$jymcsfb where nbh=7 and cjym=%s",sJym);
	      break;
	 case 333:
	       AddSetItem(pst,"代收每日结帐时间(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=42 and cjym=%s",sJym);
	       AddSetItem(pst,"代收费日结帐文件接收目录","ccs",30,
                              "from dl$jymcsfb where nbh=47 and cjym=%s",sJym);
               break;	 	  
         case 0:
               AddSetItem(pst,"商品的零售单价（元）","ccs",8,
                              "from dl$jymcsfb where cjym=%s and nbh=1",sJym);
               break;               
     }

		  /////////////////新增公用信息项，关于交易码：/////////////
                  AddSetItem(pst,"结算费率(由双方协商确定)","ccs",8,
                              "from dl$jymcsfb where nbh=33 and cjym=%s",sJym);
		  ////////////////////////////////////////////////////////// 
     
     LoadDefaultSet(pst);
     clrscr();
     outtext("\n                      [%s]参数设置",sJymc);
     BeginSetting(pst,4);
     DropSetTable(pst);
}
