#include"dlywd.h"
#include"t_accnt.h"

extern long nSysLkrq;
extern long nSysSfm;

/*============================================================
������dsTaskPayValidate    TASK_PAY_VALIDATE
���ܣ���֧�֣��ؾ֣������շѼ�¼��֤
������inbuf:��������ַ���
      ��ˮ��  char(40)
      ��ˮ�ţ�[9λ�ֺ�+15λ������+8λ����+8λ���]
���أ�0�ɹ�
      [����RECORD]
      �ɹ���000000|��ˮ��|���ʱ�־|
      ʧ�ܣ�XXXXXX|������Ϣ���ֽ���|
*============================================================*/

long dsTaskPayValidate(char * inbuf)
{
     
     long nXzbz;
     char sSqlErr[81];
     char outbuf[101];
     
     if(RunSelect("select nxzbz from dl$zwmx where clsh=%s into %ld",inbuf,&nXzbz)>0)
     {
     	if(nXzbz<2)
     	{
     		RunSql("update dl$zwmx set nxzbz=2 where clsh=%s",inbuf);
     	        CommitWork();
     	}
     	sprintf(outbuf,"000000|%s|%ld|",inbuf,nXzbz);
     }
     else
     {
     	GetSqlErrInfo(sSqlErr);
     	sprintf(outbuf,"000014|%s|",sSqlErr);
     }
     return ApplyToClient("%s",outbuf);

}

/*============================================================
������dsTaskCheckAccount    TASK_CHECK_ACCOUNT
���ܣ�����Դ��ί�з��Ķ���
������inbuf:��������ַ���
      ������  char(15)
���أ�0�ɹ�
      [����RECORD]
      �ɹ���000000|�����ܱ���|���ɷѽ��|
      ʧ�ܣ�XXXXXX|������Ϣ���ֽ���|
*============================================================*/
long dsTaskCheckAccount(char * inbuf)
{
     
     char outbuf[256];
     long nCxms;

     GetSysLkrq();
     
     if(!RunSelect("select ncxms from dl$jym where cjym=%s into %ld",inbuf,&nCxms))
     {
     	ApplyToClient("%s","000002|����Դ�޴˽���|");
        return -1;
     }
     memset(outbuf,0,sizeof(outbuf));
     switch(nCxms)
     {
     	case 110://һ�㼯��ʽ�Ķ���
     	case 111:
     	case 610:
     	     dsGeneralCheckAccount(inbuf,outbuf);
     	     break;
     	     
     	case 130:
     	     dsMobileCheckAccount(inbuf,outbuf);
     	     break;

     	case 140:
     	     dsTelecomCheckAccount(inbuf,outbuf);
     	     break;

        case 210:
        case 220:
        case 230:
             dsTaskBpCheckAccount(inbuf,outbuf);
             break;
     }
     CommitWork();
     return ApplyToClient("%s",outbuf);
}


long dsGeneralCheckAccount(char * inbuf,char * outbuf)
{
     strcpy(outbuf,"000000|0|0|");
     return 0;
}



