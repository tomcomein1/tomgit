/******************************************************************************
**
** �ļ�: lkjy.c
** Ŀ��: �̿����׵�ҵ��
**
** �������ڣ�2001.10.11
** ԭʼ�����Ա������
** �޸���ʷ
** ����        �޸���Ա  ԭ��
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

    if(!(pMenu=CreateMenu("�� �� �� ��",1)))   return;
    BindMenu(pMenu,"�� �� ǩ ��",NULL,tb_register);
    BindMenu(pMenu,"�� �� ǩ ��",NULL,tb_close);
    BindMenu(pMenu,"�̿��ɷ�(����)",NULL,tb_disbures_test);
//    BindMenu(pMenu,"�� �� �� ��",NULL,tb_check);
    BindMenu(pMenu,"�� �� �� �� Կ ¼ ��",NULL,tb_input_key);
//    BindMenu(pMenu,"�� �� �� �� �� �� �� ��",NULL,tb_input_code);
    AddMenu(pMenu,"  0.�� ��",NULL);
    ShowMenu(pMenu);
    DropMenu(pMenu);
}


void   tb_register()
{
    long   ltask_num;
    char   stb_code[CODE_LENGTH+1];

    clrscr();
    printf("\n\n    ����ִ��ǩ������...");
    ltask_num=AutoRunTask("dldj",TASK_LKQD," ","%s",stb_code);
    if(ltask_num!=0)
    {
        printf("\n\n    �����̨ͨѶ����!!!����ԭ��:[%s]",sTcpipErrInfo);
        keyb();
        return;
    }
    else
    {
        if(strcmp(stb_code,"00")==0)
        {
            printf("\n\n    ��绰����ǰ�û�ǩ���ɹ�( �� )");
        }
        else
        {
            printf("\n\n    �̿�ǩ��ʧ��!!!�������:[%s]",stb_code);
            keyb();
            return;
        }
    }
    
    ltask_num=AutoRunTask("dldj",TASK_LKJHMY1,"","%s",stb_code);
    if(ltask_num!=0)
    {
        printf("\n\n    �����̨ͨѶ����!!!����ԭ��:[%s]",sTcpipErrInfo);
        keyb();
        return;
    }
	else
	{
        if(strcmp(stb_code,"00")==0)
        {
            printf("\n\n    ��绰����ǰ�û�����MACKEY�ɹ�( �� )");
        }
        else
        {
            printf("\n\n    ��绰����ǰ�û�����MACKEYʧ��!!!�������:[%s]",stb_code);
            keyb();
            return;
        }
    }

    ltask_num=AutoRunTask("dldj",TASK_LKJHMY2,"","%s",stb_code);
    if(ltask_num!=0)
    {
        printf("\n\n    �����̨ͨѶ����!!!����ԭ��:[%s]",sTcpipErrInfo);
        keyb();
        return;
    }
	else
	{
        if(strcmp(stb_code,"00")==0)
        {
            printf("\n\n    ��绰����ǰ�û�����PINKEY�ɹ�( �� )");
        }
        else
        {
            printf("\n\n    ��绰����ǰ�û�����PINKEYʧ��!!!�������:[%s]",stb_code);
            keyb();
            return;
        }
    }
    show_hint(0,20,40,"����ִ����ϣ�����������ء�");
    keyb();
    return;
}


void   tb_close()
{
    long   ltask_num;
    char   stb_code[CODE_LENGTH+1];

    clrscr();
    printf("\n\n    ����ִ��ǩ������...");
    ltask_num=AutoRunTask("dldj",TASK_LKQT," ","%s",stb_code);
    if(ltask_num!=0)
    {
        printf("\n\n    �����̨ͨѶ����!!!����ԭ��:[%s]",sTcpipErrInfo);
        keyb();
        return;
    }
    else
    {
        if(strcmp(stb_code,"00")==0)
        {
            printf("\n\n    ��绰����ǰ�û�ǩ�˳ɹ�( �� )");
        }
        else
        {
            printf("\n\n    �̿�ǩ��ʧ��!!!�������:[%s]",stb_code);
            keyb();
            return;
        }
    }
    show_hint(0,20,40,"����ִ����ϣ�����������ء�");
    keyb();
    return;	
}

void   tb_disbures_test()
{
    return;	
}


/*---------------------------*
  ����:tb_input_key();
  ����:����Լ����Կ;
  ����:��;
  ����ֵ:;
 *---------------------------*/ 
void   tb_input_key()
{
    TEXTBOX  input;
	int      nSql;
    long     nKey;
    char     sYdmy_1[17];
        
	clrscr();
	show_hint(0,40,3,"*****�̿�����Լ��������Կ����*****");
	for(; ;)
	{
        	input.x=35;
        	input.y=10;
        	input.nLen=16;
        	input.sValue[0]='\0';
        	input.nFlag=0;
		show_hint(-1,input.x,input.y,"�̿�ǰ�û�������Կ:");
		show_hint(0,40,21,"ע��:��ԿӦΪ0~1��A~F֮����ַ�,Ҫ�����ַ���Сд!");
		nKey=getpassword(&input);
		if(nKey==KEY_ESC)
			return;
		strcpy(sYdmy_1,input.sValue);
		input.y+=2;
		input.sValue[0]='\0';
		show_hint(-1,input.x,input.y,"�ٴ�����ȷ��:");
		nKey=getpassword(&input);
		if(nKey==KEY_ESC)
			return;
		alltrim(sYdmy_1);
		alltrim(input.sValue);
		if(strcmp(sYdmy_1,input.sValue)==0&&strlen(sYdmy_1)!=0)
			break;
		else
		{
			show_hint(0,40,21,"����¼�����Կ��һ�»���Ϊ��,������¼��!");
			keyb();
			clearline(21);
		}
	}
	nSql=RunSql("update dl$dlywcs set ccs=%s where nbh=50",sYdmy_1);
	if(nSql<0)
	{
		//HintError(DB_ERR,"�̿�������Կ���%s������!!!",sYdmy_1);
	        return;
	}
	CommitWork();
	show_hint(0,40,21,"�̿����״�����Կ���ɹ�!");
	keyb();
}

