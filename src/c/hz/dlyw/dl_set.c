#include"setting.h"

//ϵͳ����
void SystemSetup()
{
     SETTABLE * pst;

     pst=CreateSetTable(8);
     AddSetItem(pst,"����IP��ַ","ccs",15,"from dl$dlywcs where nbh=3");

//   AddSetItem(pst,"�ϼ���IP��ַ","ccs",16,"from dl$dlywcs where nbh=6");
  
     AddSetItem(pst,"�Ƿ������ۺ�������Ա�Զ��Ǽ�(1:���� 0:������)",
                     "ccs",1,"from dl$dlywcs where nbh=8");
  
     AddSetItem(pst,"�Ƿ���Ҫ��̬�������ݿ�(1:��̬  0:��̬)",
                     "ccs",1,"from dl$dlywcs where nbh=10");

     LoadDefaultSet(pst);
     clrscr();
     outtext("\n                           �� �� ϵ ͳ �� �� �� ��");
     BeginSetting(pst,4);
     DropSetTable(pst);
}

//���ݽ�����ĳ���ģʽ������ز���
void JymSetup()
{

     char sJymc[51],sJym[16];
     SETTABLE * pst;
     long nCxms;

     clrscr();
     
     if(SelectJym(sJym,0)<=0) return;
     	RunSelect("select ncxms,cjymc from dl$jym where cjym=%s into %ld%s",sJym,&nCxms,sJymc);
     pst=CreateSetTable(8);

     //���ݲ�ͬ�ĳ���ģʽ������Ӧ����������;
     if(sJym[8]=='0' && sJym[0]=='0' && sJym[1]=='1')
     {

          AddSetItem(pst,"ί�з��ļ�ͨѶ���û���","ccs",20,
                           "from dl$jymcsfb where nbh=49 and cjym=%s",sJym);

          AddSetItem(pst,"ί�з��ļ�ͨѶ������","ccs",20,
                           "from dl$jymcsfb where nbh=48 and cjym=%s",sJym);
     }
     switch(nCxms)
     {
          case 111:
               AddSetItem(pst,"���������","ccs",8,
                              "from dl$jymcsfb where nbh=39 and cjym=%s",sJym);

               AddSetItem(pst,"Ԥ����ս���ʱ��(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=42 and cjym=%s",sJym);

               AddSetItem(pst,"Ԥ���ɷ��ļ�����Ŀ¼","ccs",30,
                              "from dl$jymcsfb where nbh=47 and cjym=%s",sJym);
     
               AddSetItem(pst,"Ԥ����ս����ļ�����Ŀ¼","ccs",30,
                              "from dl$jymcsfb where nbh=46 and cjym=%s",sJym);
               {
                  long limited=0;
                  char sTitle[50];
                  long index;
                   
                  AddSetItem(pst,"�Ƿ���Ҫ���ƺ����(0:������,1:����)","ccs",1,
                              "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);

                  RunSelect("select to_number(ccs) from dl$jymcsfb "
                  "where cjym=%s and nbh=1 into %ld",sJym,&limited);
 
                  for(index=0;index<30 && limited==1;index++)
                  {
                     sprintf(sTitle,"���ź����%ld",index+1);
                     AddSetItem(pst,sTitle,"ccs",11,
                     "from dl$jymcsfb where nbh=%ld and cjym=%s",index+2,sJym);
                  }  
               }
               
               break;

          case 110:

               AddSetItem(pst,"���������","ccs",8,
                              "from dl$jymcsfb where nbh=39 and cjym=%s",sJym);

               AddSetItem(pst,"����ÿ�ս���ʱ��(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=42 and cjym=%s",sJym);

               AddSetItem(pst,"�½�������(DD)","ccs",2,
                              "from dl$jymcsfb where nbh=41 and cjym=%s",sJym);

               AddSetItem(pst,"���շ��ս����ļ�����Ŀ¼","ccs",30,
                              "from dl$jymcsfb where nbh=47 and cjym=%s",sJym);
     
               AddSetItem(pst,"���շ��½����ļ�����Ŀ¼","ccs",30,
                              "from dl$jymcsfb where nbh=46 and cjym=%s",sJym);

               break;               

          
          case 130://�ƶ���������������
               AddSetItem(pst,"���д���(���ƶ�����)","ccs",8,
                              "from dl$jymcsfb where nbh=36 and cjym=%s",sJym);
               
               AddSetItem(pst,"���׳�ʼ��֤��(���ƶ�����)","ccs",16,
                              "from dl$jymcsfb where nbh=37 and cjym=%s",sJym);
               
               AddSetItem(pst,"���׳�ʼ��������Կ(���ƶ�����)","ccs",8,
                              "from dl$jymcsfb where nbh=38 and cjym=%s",sJym);
               
               AddSetItem(pst,"����ص�ַ��ʶ(���ƶ�����)","ccs",9,
                              "from dl$jymcsfb where nbh=40 and cjym=%s",sJym);

               AddSetItem(pst,"�Զ�����ʱ��(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=35 and cjym=%s",sJym);
               break;                

          case 140://��������
          case 141:
               AddSetItem(pst,"�����û���(�ɵ��ŷ���)","ccs",8,
                              "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
               
               AddSetItem(pst,"��������(�ɵ��ŷ���)","ccs",8,
                              "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
	       if(nCxms==141)
			AddSetItem(pst,"��λ���(�ɵ��ŷ���)","ccs",8,
                             "from dl$jymcsfb where nbh=3 and cjym=%s",sJym);
                              
			AddSetItem(pst,"�Զ�����ʱ��(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=35 and cjym=%s",sJym);
               break;

         case 610://��Ȼ��
               AddSetItem(pst,"��������ÿ�����ɽ���","ccs",8,
                              "from dl$jymcsfb where cjym=%s and nbh=10",sJym);

               AddSetItem(pst,"��������ÿ�����ɽ���","ccs",8,
                              "from dl$jymcsfb where cjym=%s and nbh=11",sJym);
               break;

         case 821:
               AddSetItem(pst,"���д���(�ɱ��շ���)","ccs",4,
                              "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);

               AddSetItem(pst,"�������(����������)","ccs",9,
                              "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
               
               AddSetItem(pst,"�Զ�����ʱ��(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=35 and cjym=%s",sJym);
               break;

         case 1001://��
               AddSetItem(pst,"�Ƿ�ֹͣ��أ�y/n)","ccs",1,
                    "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
               AddSetItem(pst,"�Ƿ��Զ���չ��λ��Ϣ(y/n)","ccs",1,
                    "from dl$jymcsfb where nbh=10 and cjym=%s",sJym);
               AddSetItem(pst,"�Ƿ��Զ���չ��ѯ��Ϣ(y/n)","ccs",1,
                    "from dl$jymcsfb where nbh=11 and cjym=%s",sJym);

 	       //AddSetItem(pst,"ҵ�񼶱�  (1.ʡ 2.�� 3.��)","ccs",1,
           //        "from dl$jymcsfb where nbh=25 and cjym=%s",sJym);
           
           //AddSetItem(pst,"�Ƿ�ָ������Ա����(y/n)","ccs",1,
           //        "from dl$jymcsfb where nbh=21 and cjym=%s",sJym);
           //AddSetItem(pst,"ָ������Ա����","ccs",6,
           //        "from dl$jymcsfb where nbh=22 and cjym=%s",sJym);
           //AddSetItem(pst,"ָ������Ա����","ccs",2,
           //        "from dl$jymcsfb where nbh=23 and cjym=%s",sJym);

           /*AddSetItem(pst,"ָ����ӡ�����ļ���","ccs",10,
                   "from dl$jymcsfb where nbh=50 and cjym=%s",sJym);*/
               break;

         case 230:
         case 220:
         case 210:
               AddSetItem(pst,"127�����Żݼ��µ�һ��","ccs",2,
                    "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
               AddSetItem(pst,"96167�����Żݼ��µ�һ��","ccs",2,
                    "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
               AddSetItem(pst,"129�����Żݼ��µ�һ��","ccs",2,
                    "from dl$jymcsfb where nbh=3 and cjym=%s",sJym);
			   break;
		
         case 521:
         case 522:
               AddSetItem(pst,"��˰���´��ս�ֹ����","ccs",8,
                    "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
		       AddSetItem(pst,"���д���(�ɹ�˰�ַ���)","ccs",10,
		            "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
               AddSetItem(pst,"��ֵ˰Ԥ�㼶��","ccs",16,
                    "from dl$jymcsfb where nbh=3 and cjym=%s",sJym);
               AddSetItem(pst,"��ֵ˰����","ccs",8,
                    "from dl$jymcsfb where nbh=4 and cjym=%s",sJym);
               AddSetItem(pst,"����˰Ԥ�㼶��","ccs",16,
                    "from dl$jymcsfb where nbh=5 and cjym=%s",sJym);
               AddSetItem(pst,"����˰����","ccs",8,
                    "from dl$jymcsfb where nbh=6 and cjym=%s",sJym);
		AddSetItem(pst,"�ɿ����Ƿ��ӡ֧����(Y/N)","ccs",1,
                    "from dl$jymcsfb where nbh=7 and cjym=%s",sJym);
	
               break;
         
         case 123:
         case 124:
              AddSetItem(pst,"���д���(�ɹ���ַ���)","ccs",4,
                              "from dl$jymcsfb where nbh=36 and cjym=%s",sJym);
              AddSetItem(pst,"����ͨ�Ż�����","ccs",8,
              				  "from dl$jymcsfb where nbh=3 and cjym=%s",sJym);
              AddSetItem(pst,"��ҵ���ʻ�(�ɹ���ַ���)","ccs",6,
              				  "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
              AddSetItem(pst,"��ҵ����(�ɹ���ַ���)","ccs",6,
              				  "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
              break;
	
	case 222:
	case 223:
              AddSetItem(pst,"��˰���´��ս�ֹ����","ccs",8,
                    "from dl$jymcsfb where nbh=1 and cjym=%s",sJym);
	      AddSetItem(pst,"���Ʊ������(��)","ccs",12,
                    "from dl$jymcsfb where nbh=2 and cjym=%s",sJym);
	      
	      AddSetItem(pst,"����˰��","ccs",12,
                    "from dl$jymcsfb where nbh=3 and cjym=%s",sJym);
              AddSetItem(pst,"��ʳ,��������˰��","ccs",12,
                    "from dl$jymcsfb where nbh=4 and cjym=%s",sJym);
	      AddSetItem(pst,"���ҵ˰��","ccs",12,
                    "from dl$jymcsfb where nbh=5 and cjym=%s",sJym);
              AddSetItem(pst,"��������ҵ˰��","ccs",12,
                    "from dl$jymcsfb where nbh=6 and cjym=%s",sJym);
	      AddSetItem(pst,"����ҵ˰��","ccs",12,
                    "from dl$jymcsfb where nbh=7 and cjym=%s",sJym);
	      break;
	 case 333:
	       AddSetItem(pst,"����ÿ�ս���ʱ��(hhmmss)","ccs",6,
                              "from dl$jymcsfb where nbh=42 and cjym=%s",sJym);
	       AddSetItem(pst,"���շ��ս����ļ�����Ŀ¼","ccs",30,
                              "from dl$jymcsfb where nbh=47 and cjym=%s",sJym);
               break;	 	  
         case 0:
               AddSetItem(pst,"��Ʒ�����۵��ۣ�Ԫ��","ccs",8,
                              "from dl$jymcsfb where cjym=%s and nbh=1",sJym);
               break;               
     }

		  /////////////////����������Ϣ����ڽ����룺/////////////
                  AddSetItem(pst,"�������(��˫��Э��ȷ��)","ccs",8,
                              "from dl$jymcsfb where nbh=33 and cjym=%s",sJym);
		  ////////////////////////////////////////////////////////// 
     
     LoadDefaultSet(pst);
     clrscr();
     outtext("\n                      [%s]��������",sJymc);
     BeginSetting(pst,4);
     DropSetTable(pst);
}
