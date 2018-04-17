/******************************************************************************
**
** 文件: lkjy.c
** 目的: 绿卡交易的业务
**
** 生成日期：2001.10.11
** 原始编程人员：何庆
** 修改历史
** 日期        修改人员  原因
**
******************************************************************************/
#include   "dlpublic.h"
#include   "tools.h"
#include   "newtask.h"

#define   CODE_LENGTH   128

extern   char   sTcpipErrInfo[81];

void   tb_menu();
void   tb_register();
void   tb_close();
void   tb_disbures_test();
//void   tb_check();
void   tb_input_key();
//void   tb_input_code();

void   tb_menu()
{
    MENU * pMenu;

    if(!(pMenu=CreateMenu("绿 卡 交 易",1)))   return;
    BindMenu(pMenu,"绿 卡 签 到",NULL,tb_register);
    BindMenu(pMenu,"绿 卡 签 退",NULL,tb_close);
    BindMenu(pMenu,"绿卡缴费(测试)",NULL,tb_disbures_test);
//    BindMenu(pMenu,"绿 卡 对 帐",NULL,tb_check);
    BindMenu(pMenu,"绿 卡 主 密 钥 录 入",NULL,tb_input_key);
//    BindMenu(pMenu,"绿 卡 交 易 局 号 设 置",NULL,tb_input_code);
    AddMenu(pMenu,"  0.退 出",NULL);
    ShowMenu(pMenu);
    DropMenu(pMenu);
}


void   tb_register()
{
    long   ltask_num;
    char   stb_code[CODE_LENGTH+1];

    clrscr();
    printf("\n\n    正在执行签到任务...");
    ltask_num=AutoRunTask("dldj",TASK_LKQD," ","%s",stb_code);
    if(ltask_num!=0)
    {
        printf("\n\n    代理后台通讯错误!!!错误原因:[%s]",sTcpipErrInfo);
        keyb();
        return;
    }
    else
    {
        if(strcmp(stb_code,"00")==0)
        {
            printf("\n\n    向电话银行前置机签到成功( √ )");
        }
        else
        {
            printf("\n\n    绿卡签到失败!!!错误代码:[%s]",stb_code);
            keyb();
            return;
        }
    }
    
    ltask_num=AutoRunTask("dldj",TASK_LKJHMY1,"","%s",stb_code);
    if(ltask_num!=0)
    {
        printf("\n\n    代理后台通讯错误!!!错误原因:[%s]",sTcpipErrInfo);
        keyb();
        return;
    }
	else
	{
        if(strcmp(stb_code,"00")==0)
        {
            printf("\n\n    向电话银行前置机交换MACKEY成功( √ )");
        }
        else
        {
            printf("\n\n    向电话银行前置机交换MACKEY失败!!!错误代码:[%s]",stb_code);
            keyb();
            return;
        }
    }

    ltask_num=AutoRunTask("dldj",TASK_LKJHMY2,"","%s",stb_code);
    if(ltask_num!=0)
    {
        printf("\n\n    代理后台通讯错误!!!错误原因:[%s]",sTcpipErrInfo);
        keyb();
        return;
    }
	else
	{
        if(strcmp(stb_code,"00")==0)
        {
            printf("\n\n    向电话银行前置机交换PINKEY成功( √ )");
        }
        else
        {
            printf("\n\n    向电话银行前置机交换PINKEY失败!!!错误代码:[%s]",stb_code);
            keyb();
            return;
        }
    }
    show_hint(0,20,40,"任务执行完毕，按任意键返回。");
    keyb();
    return;
}


void   tb_close()
{
    long   ltask_num;
    char   stb_code[CODE_LENGTH+1];

    clrscr();
    printf("\n\n    正在执行签退任务...");
    ltask_num=AutoRunTask("dldj",TASK_LKQT," ","%s",stb_code);
    if(ltask_num!=0)
    {
        printf("\n\n    代理后台通讯错误!!!错误原因:[%s]",sTcpipErrInfo);
        keyb();
        return;
    }
    else
    {
        if(strcmp(stb_code,"00")==0)
        {
            printf("\n\n    向电话银行前置机签退成功( √ )");
        }
        else
        {
            printf("\n\n    绿卡签退失败!!!错误代码:[%s]",stb_code);
            keyb();
            return;
        }
    }
    show_hint(0,20,40,"任务执行完毕，按任意键返回。");
    keyb();
    return;	
}

void   tb_disbures_test()
{
    return;	
}


/*---------------------------*
  函数:tb_input_key();
  功能:输入约定密钥;
  参数:无;
  返回值:;
 *---------------------------*/ 
void   tb_input_key()
{
    TEXTBOX  input;
	int      nSql;
    long     nKey;
    char     sYdmy_1[17];
        
	clrscr();
	show_hint(0,40,3,"*****绿卡交易约定传输密钥输入*****");
	for(; ;)
	{
        	input.x=35;
        	input.y=10;
        	input.nLen=16;
        	input.sValue[0]='\0';
        	input.nFlag=0;
		show_hint(-1,input.x,input.y,"绿卡前置机传输密钥:");
		show_hint(0,40,21,"注意:密钥应为0~1和A~F之间的字符,要区分字符大小写!");
		nKey=getpassword(&input);
		if(nKey==KEY_ESC)
			return;
		strcpy(sYdmy_1,input.sValue);
		input.y+=2;
		input.sValue[0]='\0';
		show_hint(-1,input.x,input.y,"再次输入确认:");
		nKey=getpassword(&input);
		if(nKey==KEY_ESC)
			return;
		alltrim(sYdmy_1);
		alltrim(input.sValue);
		if(strcmp(sYdmy_1,input.sValue)==0&&strlen(sYdmy_1)!=0)
			break;
		else
		{
			show_hint(0,40,21,"两次录入的密钥不一致或者为空,请重新录入!");
			keyb();
			clearline(21);
		}
	}
	nSql=RunSql("update dl$dlywcs set ccs=%s where nbh=50",sYdmy_1);
	if(nSql<0)
	{
		//HintError(DB_ERR,"绿卡传输密钥入库%s入库错误!!!",sYdmy_1);
	        return;
	}
	CommitWork();
	show_hint(0,40,21,"绿卡交易传输密钥入库成功!");
	keyb();
}

