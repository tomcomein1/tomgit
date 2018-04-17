/***********************************************************
**
** �ļ�: dl185.c
** Ŀ��: 
**
** �������ڣ�2001.2.23
** ԭʼ�����Ա������
** �޸���ʷ
** ����        �޸���Ա  ԭ��
**
************************************************************/
#include "dlpublic.h"
#include "netmain.h"
#include "tasktool.h"
#include "newtask.h"
#include "task.h"
#include "dlsrv.h"
#include "8583.h"
#define  TEST


typedef struct dl185xx{
	char bbh[3+1];	 //3λ�汾��
	char rwbh[3+1];	 //3λ������
	char mac[16+1];	 //У����
	char lsh[40+1];  //��ˮ��
	char jh[9+1];    //���ش������ľֺ�
	char jym[15+1];  //������
	char year[4];    //���
	char rq[4];      //����
	char qqjg[15+1]; //�������
	char czydh[5+1]; //����Ա����
	char sfm[6+1];	 //HHMMSSʱ����
}DL185XX;


double dYzf;
extern char sTcpipErrInfo[81];
extern char sSysDlywService[41];

DL185XX Dl185xx;


int WriteToCallcenter(int ssock,char *package,long length); /*�������*/
long get_dl185tn(char *packagehead); /*ȡ�ñ���������*/
int  dl185jf(char *pkg,char *ans_pkg);
void dl185_task(char *package,char *ans_pkg);

/*this is server process grammer for Call_Center (185)*/
void dl185main()
{
  int alen;     /* length of client's address */
  int msock;      /* master server socket   */
  int ssock;      /* slave server socket    */
  int dlywsock=0;
  int n=0;
  char sbuf[257];
  char package[TOTAL_PACKAGE_SIZE+1];
  char *pPackage;
  char ans_pkg[TOTAL_PACKAGE_SIZE+1],ans_pkg1[TOTAL_PACKAGE_SIZE+1];
  struct sockaddr_in fsin;  /* the address of a client  */

  printf("\n  DlywServer for Call_Center exchanger started... \
          \n  Copyright HongZhi Software CO.,LTD. 2001/06\n");

  msock = passiveTCP("6677", 5);
  setsid();
  signal(SIGHUP,SIG_IGN);
  memset(sbuf,0,sizeof(sbuf));
  for(; ;)
  {
    alen = sizeof(fsin);
    signal(SIGCLD,SIG_IGN);
    ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
    #ifdef TEST
    printf("\n\n\n  Child srv start...\n");
    #endif
    /*----------------------------------------------------------------* 
      Create a child process to continue accepting client's connection.
      and the parent process to be the server.
     *-----------------------------------------------------------------*/
    if(ssock<0)
    {
        printf("\n  I can't get the dl185's socket!!!\n");
        exit(0);
    }
    if(fork())
    {
    	close(ssock);
    	continue;
    }	
    close(msock);
    /*above sentence for starting service*/
    for(; ;)
    {
    	read(ssock,sbuf,256);
        n++;
        #ifdef TEST
           printf("\n  ���ձ��Ĵ��� : [%ld]",n);
           printf("\n  ���յ��ı��������� : [%s] ",sbuf);
    	#endif
    	memcpy(package,sbuf+1,255);	/*get out of package's 'more'*/
    	if(strncmp(sbuf,"0",1)==0)
    		break;
	//if(sizeof(package)>=TOTAL_PACKAGE_SIZE)
        if(n>=10)	/*20020705*/
	{
		write(ssock,"0                                                                                        1000000�����ĳ������Ĵ���",93);   /*�����ĳ������Ĵ���!*/
		#ifdef TEST
		   printf("\n  The clent's package is so longer than define that server close child process!\n");
		#endif
		close(ssock);
		exit(-1);
	}
     }//end if
     dl185_task(package,ans_pkg);   /*185���������*/
     #ifdef TEST
        printf("\n  ��ͻ��˷��صı��������� : [%s] ",ans_pkg);
        printf("\n  ��ͻ��˷��صı��ĳ����� : [%ld] ",strlen(ans_pkg));
     #endif
     sprintf(ans_pkg1,"0%s",ans_pkg);
     write(ssock,ans_pkg1,strlen(ans_pkg1));
     //WriteToCallcenter(ssock,ans_pkg,strlen(ans_pkg));
     /*
      *����CALL CENTER ���͵ı��Ķ�ʧ��ʧ�ܣ�Ӧ��Ϊ��ȷ���Ľ��ס�
      *�ɿͻ����Ͳ�ѯ����ȷ�Ͻ��׽��������ϵͳ�����κδ���
      */
     close(ssock);
     exit(0);
 }//end for
}


/*--------------------------------*
 *����<->185ͨѶ���ݰ�ר�÷��ͺ���
 *ÿ�ζ���256���ֽ�
 *���󷵻� -1
 *�ɹ����� 0
 *--------------------------------*/
int WriteToCallcenter(int ssock,char *package,long length)
{
	long i=0;
	char more[1+1];
	char tmpbuf[255+1];
	char sendbuf[256+1];

	strcpy(more,"1");	/*ini more*/
	for(; ;)
	{
		i=i+1;
		if(i*255>=length)
			strcpy(more,"0");
		memset(tmpbuf,'\0',255);
		memcpy(tmpbuf,package+(i-1)*255,255);
		sprintf(sendbuf,"%s%s",more,tmpbuf);
		if(write(ssock,sendbuf,256)<0)
		{
			return -1;
		}
		if(i*255>=length)
		   break;
	}
	return 0;
}


/*------------------------*
 *��ȡDL-185���׵�������
 *>0��ʾ�ɹ�
 *<0��ʾ����
 *------------------------*/
long get_dl185tn(char *pkg)
{
	char tasknum1[3+1];
	long tasknum2;
	
	strncpy(tasknum1,pkg+3,3);
	tasknum2=atol(tasknum1);
	return tasknum2;
}


/*------------------------------*
 *��ȡ���Ĺ�����Ϣ�ֶ�
 *------------------------------*/
void read_185info(char *pkg)
{
	strncpy(Dl185xx.bbh,pkg,3);
	strncpy(Dl185xx.rwbh,pkg+3,3);
	strncpy(Dl185xx.mac,pkg+6,16);
	strncpy(Dl185xx.lsh,pkg+22,40);
	
	strncpy(Dl185xx.jh,pkg+22,9);
	strncpy(Dl185xx.jym,pkg+31,15);
	strncpy(Dl185xx.year,pkg+46,4);
	strncpy(Dl185xx.rq,pkg+50,4);
	
	strncpy(Dl185xx.qqjg,pkg+62,15);
	strncpy(Dl185xx.czydh,pkg+77,5);
	strncpy(Dl185xx.sfm,pkg+82,6);
}


void dl185_task(char *package,char *ans_pkg)
{
	char packagehead[301];
	char string[301];
	char MAC_RECEIVE[17];
	char MACKEY[17];
	unsigned char MACKEY_BIN[9];
	char MAC_BIN[9];
	char MAC_HEX[17];
	char KEY[17];
	unsigned int key1[8];
	char PINKEY[17];
	long ntask;
	int  i;
	char str1[257],str_tmp1[101],str_tmp2[201];
	char tmpkey[9];
	unsigned int tmpkey1[8];
	FILE *f_trans;
	FILE *f_mac;
	char ans_pkg_tmp[TOTAL_PACKAGE_SIZE+1];
	char ans_pkg1[TOTAL_PACKAGE_SIZE+1];
	char tmp[257];


        read_185info(package);
        #ifdef TEST
           printf("\n �汾�� : [%s]",Dl185xx.bbh);
	   printf("\n ������ : [%s]",Dl185xx.rwbh);
	   printf("\n У���� : [%s]",Dl185xx.mac);
	   printf("\n ��ˮ�� : [%s]",Dl185xx.lsh);
	
	   printf("\n ����ֺ� : [%s]",Dl185xx.jh);
	   printf("\n ������ : [%s]",Dl185xx.jym);
	   printf("\n ��� : [%s]",Dl185xx.year);
	   printf("\n ���� : [%s]",Dl185xx.rq);
	
	   printf("\n ��������� : [%s]",Dl185xx.qqjg);
	   printf("\n ����Ա���� : [%s]",Dl185xx.czydh);
	   printf("\n ʱ���� : [%s]",Dl185xx.sfm);
        #endif

        memset(MAC_RECEIVE,'\0',17);
	strcpy(MAC_RECEIVE,package+6,16);
	MAC_RECEIVE[16]='\0';
	printf("\n MAC_RECEIVE is : [%s]",MAC_RECEIVE);
	memset(packagehead,'\0',301);
	memcpy(packagehead,package,88);
	printf("\n RECEIVE PACKAGEHEAD IS : [%s]",packagehead);
	memset(string,'\0',sizeof(string));
	memcpy(string,package,6);
	strcat(string,package+22);
	printf("\n �����ܴ� : [%s]  \n �����ܴ����� ��[%ld] \n",string,strlen(string));	
	ntask=get_dl185tn(packagehead);

//
	memset(MACKEY_BIN,'\0',9);
	if(ntask==100)
	{
		f_trans=fopen("transkey.txt","r");
		printf("\n our MACKEY IS [%s] \n",MACKEY);
		for(i=0;i<8;i++)
		{
			fscanf(f_trans,"%2x",&MACKEY_BIN[i]);
		}
		fclose(f_trans);
	}
	else
	{
		f_trans=fopen("mac.key","r");
		printf("\n our MACKEY IS [%s] \n",KEY);
		for(i=0;i<8;i++)
		{
			fscanf(f_trans,"%2x",&MACKEY_BIN[i]);
		}
		fclose(f_trans);
	}

//
	printf("\n MACKEY_BIN  LENGTH IS : [%ld] \n",strlen(MACKEY_BIN));
	printf("\n MACKEY_BIN  IS : [%s] \n",MACKEY_BIN);
	for(i=0;i<8;i++)
	   tmpkey1[i]=(unsigned int)MACKEY_BIN[i];
	GetMac(string,strlen(string),tmpkey1,MAC_BIN);
	BCDtoASCII(MAC_BIN,8,MAC_HEX);
	printf("\n CHECK MAC is :[%s] \n",MAC_HEX);
	alltrim(MAC_HEX);
	alltrim(MAC_RECEIVE);

        printf("\n MAC_RECEIVE is :%s",MAC_RECEIVE);
        printf("\n CHECK MAC is :%s \n",MAC_HEX);
	if(strcmp(MAC_HEX,MAC_RECEIVE)!=0)
	{
                printf("\n hello heqing1 ");
		sprintf(ans_pkg,"%s200000MACУ�������",packagehead);
                printf("\n MACУ����� %s! \n",ans_pkg);
		return;
	}
	/*���齻�����Ƿ񿪷�*/
	/*��ѯӦ�ɿ�*/
        printf("\n tasknumber is %ld ",ntask);
	switch(ntask)
	{
	   case 100://������Կ
	   	changemy(packagehead,ans_pkg,MACKEY_BIN,KEY);
	   	return;
	   case 200://��ѯӦ�ɷ�
	   	cxyjk(package,ans_pkg_tmp);
                break;
	   case 201://��ѯ
	   	cxye(package,ans_pkg_tmp);
	   	break;
	   case 300://�ɷ�
	   	dl185jf(package,ans_pkg_tmp);
	   	break;
	   case 400://ȡ���ɷ�(��ʱ����)
	   	break;
	   case 500://��ѯ��������
	   	cxjyzz(package,ans_pkg_tmp);
	   	break;
	   default:
		sprintf(ans_pkg_tmp,"%s230000���������������",packagehead);
		break;
	}
	/*���ܷ��ش�*/
	f_mac=fopen("mac.key","r");
	for(i=0;i<8;i++)
	   fscanf(f_mac,"%2x",&key1[i]);
	fclose(f_mac);
	memset(tmp,'\0',257);
	memcpy(tmp,ans_pkg_tmp,6);
	strcat(tmp,ans_pkg_tmp+22);
	printf("\n �����ܴ��� ��[%s]",tmp);
	printf("\n �����ܴ������� ��[%ld]",strlen(tmp));
	memset(MAC_HEX,'\0',17);
	GetMac(tmp,strlen(tmp),key1,MAC_BIN);
	BCDtoASCII(MAC_BIN,8,MAC_HEX);
        printf("\n MAC HEX IS : [%s] ",MAC_HEX);
	memset(ans_pkg,'\0',sizeof(ans_pkg));
	memcpy(ans_pkg1,ans_pkg_tmp,6);
        printf("\n ans_pkg is %s ",ans_pkg);
	strcat(ans_pkg1,MAC_HEX);
	ans_pkg1[22]='\0';
        printf("\n ans_pkg is %s ",ans_pkg);
	strcat(ans_pkg1,ans_pkg_tmp+22);
        printf("\n ans_pkg is %s ",ans_pkg);
	sprintf(ans_pkg,"%s",ans_pkg1);
}

/*DL185������Կ*/
int changemy(char *pkghead,char *anspkg,char *ydmy,char *mackey)
{
	char receive[17];
	char receive1[9];
	char receive2[9];
	char answer[40+1];
	char str[128];
	char rtkey[17];
	unsigned int key[9];
	char MAC_BIN[9];
	char MAC_HEX[17];
	char ans_pkg[256];
	int  i;
	unsigned int key1[8];
	unsigned int source[8];
	unsigned int target[8];


	char he[17];
	char he1[9];
	char tt2[9];
	char tt3[17];
	unsigned int tt[8];
	unsigned int tt1[8];
	FILE *f_mac,*f_pin;

	if(AutoRunTask("dldj",TASK_DL185_JHMY," ","%s",receive)!=0)
	{
                printf("\n �����̨ͨѶ�������!sTcpipErrInfo is [%s]",sTcpipErrInfo);
		sprintf(anspkg,"%s110000����ϵͳ����",pkghead);
		return -1;	/*�������*/
	}
	else
	{
    		f_mac=fopen("mac.key","w+");
		fprintf(f_mac,"%s",receive);
		fclose(f_mac);
		f_pin=fopen("pin.key","w+");
    	 	fprintf(f_pin,"%s",receive);
    		fclose(f_pin);
    		
                printf("\n dlywd form mackey is %s ",receive);

		ASCIItoBCD(receive,16,receive1);
		
		for(i=0;i<8;i++)
			key1[i]=(unsigned int)ydmy[i];
		for(i=0;i<8;i++)
			source[i]=(unsigned int)receive1[i];
		gen_pin(key1,source,target,1);
		memset(receive2,'\0',8);
		for(i=0;i<8;i++)
			receive2[i]=(char)target[i];

printf("\n\n+++++++target=%s", target);
		BCDtoASCII(receive2,8,rtkey);
printf("\n DES DES IS %s ",rtkey);
		
		memset(str,'\0',sizeof(str));
		memcpy(str,pkghead,6);
		strcat(str,pkghead+22);
		sprintf(str,"%s000000%-16s%-16s",str,rtkey,rtkey);
		//ASCIItoBCD(receive,16,key);
		
		f_mac=fopen("mac.key","r");
		for(i=0;i<8;i++)
		   fscanf(f_mac,"%2x",&key[i]);
		fclose(f_mac);
		printf("\n �����ܴ��� ��[%s]",str);
		printf("\n �����ܴ������� ��[%ld]",strlen(str));
		GetMac(str,strlen(str),key,MAC_BIN);
		BCDtoASCII(MAC_BIN,8,MAC_HEX);
printf("\n MAC HEX IS : [%s] ",MAC_HEX);
		memset(ans_pkg,'\0',sizeof(ans_pkg));
		
		memcpy(ans_pkg,str,6);
printf("\n ans_pkg is %s ",ans_pkg);
		strcat(ans_pkg,MAC_HEX);
		ans_pkg[22]='\0';
printf("\n ans_pkg is %s ",ans_pkg);
		strcat(ans_pkg,str+6);
printf("\n ans_pkg is %s ",ans_pkg);
		sprintf(anspkg,"%s",ans_pkg);
	}
}


/*185�ɷ�*/
int dl185jf(char *pkg,char *ans_pkg)
{
	/*�̿��ɷѱ��ĸ�ʽ       "%19s%16s%12f%4d%6d" 55λ �û��ʺţ��û�PIN�����׽��������ڣ�����ʱ��*/
	/*����ϵͳ�ɷѱ��ĸ�ʽ   ��ˮ��|�û���־|�û�����|�ɿʽ|ʵ���ܿ��|����Ա����|ʱ����|�ɿ��·���|���|�·�|���|�·�|......*/

	char rec[257];
	char sRet[7];
	RECORD dljf;
	double dSjk;
	char  pin[17],pin_un[9],pin_asc[17];
	char  pin_bcd[9],pinkey_bcd[9];
	unsigned int uspin[8],uspinkey[8],target[8];
	char  lkywcode[3];
	
	char lkjf_pkg[100+1];
	char lkjf_yhzh[19+1];
	char lkjf_jyje[11+1];
	char lkjf_receive[128+1];
	
	char pkg_head[201];
	char clsh[41];
	char cyhbz[101];
	char cyjmc[81];
	long njkfs=2;//lkjf
	char djkje[12];
	char cczydh[8];
	char sfm[7];
	char jfyf[2];
	char year[5];
	char mounth[3];
	int  i;
	char yhzh[20];
	char PINKEY_BIN[8+1];
        char LKZH[20];
	FILE *f_pin;
	
	/*����PIN KEY*/
	f_pin=fopen("pin.key","r");
	for(i=0;i<8;i++)
	{
	   fscanf(f_pin,"%2x",&PINKEY_BIN[i]);
	}
	fclose(f_pin);
	/*ȡ������ͷ*/
	memset(pkg_head,'\0',200);
	strncpy(pkg_head,pkg,88);
	#ifdef TEST
        printf("\n pkg head is [%s] ",pkg_head);
	#endif
	
	/*�⿪�û�����*/
	memset(pin,'\0',16);
	memcpy(pin,pkg+140,16);
	#ifdef TEST
         printf("\n receive pin is [%s] ",pin);
	#endif
	ASCIItoBCD(pin,16,pin_bcd);
	for(i=0;i<8;i++)
		uspin[i]=(unsigned int)pin_bcd[i];
	for(i=0;i<8;i++)
        {
		uspinkey[i]=(unsigned int)PINKEY_BIN[i];
		printf("\npinkey i:%d", uspinkey[i]);
        }
	gen_pin(uspinkey,uspin,target,0);
	for(i=0;i<8;i++)
	  pin_un[i]=(char)target[i];
	BCDtoASCII(pin_un,8,pin_asc);
	memset(clsh,'\0',40);
	memcpy(clsh,pkg+22,40);
	#ifdef TEST
	 printf("\n clsh is [%s]",clsh);
	#endif
	memset(cyhbz,' ',100);
	memcpy(cyhbz,pkg+88,20);
        cyhbz[20]='\0';
	#ifdef TEST
	printf("\n cyhbz is [%s]",cyhbz);
	#endif
        memset(djkje,'\0',12);
	memcpy(djkje,pkg+110,11);
	#ifdef TEST
	printf("\n djkje is [%s]",djkje);
	#endif
	memcpy(sfm,pkg+82,6);
	#ifdef TEST
	printf("\n HHMMSS is [%s]",sfm);
	#endif
	memcpy(year,pkg+156,4);
	#ifdef TEST
	printf("\n year is [%s]",year);
	#endif
	memcpy(mounth,pkg+160,2);
	#ifdef TEST
	printf("\n mounth is [%s]",mounth);
	#endif
        strncpy(LKZH,pkg+121,19);
        LKZH[19]='\0';
	
	Dl185xx.rq[4]='\0';
	Dl185xx.sfm[6]='\0';
	/*�̿��ɷ�����*/

        sprintf(lkjf_pkg,"%19s%16s%012s%4s%6s%20s%40s",LKZH,pin_asc,djkje,Dl185xx.rq,sfm,cyhbz,clsh);

        if(AutoRunTask("dldj",TASK_LKJF,"%s","%s",lkjf_pkg,lkjf_receive)!=0)
        { 
    		    #ifdef TEST
		       printf("\n ���Ӵ����̨�����������");
		    #endif
		    sprintf(ans_pkg,"%s110000����ϵͳ����",pkg_head);
		    return -1;
        }
        else
        {
       	    printf("\n �̿�ǰ�û���Ӧ���� [%s] ",lkjf_receive);
	    if(strncmp("0244",lkjf_receive,4)==0)
       	    {
		   strncpy(lkywcode,lkjf_receive+20,2);
		   lkywcode[2]='\0';
		   sprintf(ans_pkg,"%s3000%s",pkg_head,lkywcode);
		   return -1;
	    }
            /*��ˮ��|�û���־|�û�����|�ɿʽ|ʵ���ܿ��|����Ա����|ʱ����|�ɿ��·���|���|�·�|���|�·�|......*/
            sprintf(rec,"%40.40s|%s|DL185|1|%s|DL185|%s|1|%s|%s|",clsh,cyhbz,djkje,sfm,year,mounth);
           //strcpy(sSysDlywService,"6667");
           if(AutoRunTask("dldj",TASK_PAY_CONFIRM,"%s","%r",rec,&dljf)!=0)
           {
              //strcpy(sSysDlywService,"6678");
    	      #ifdef TEST
	      printf("\n ���Ӵ����̨�����������");
	      #endif
	      sprintf(ans_pkg,"%s110000����ϵͳ����",pkg_head);
              AutoRunTask("dldj",TASK_LKQXJF,"%s","%s",lkjf_pkg,lkjf_receive);
	      return -1;
            }
            //strcpy(sSysDlywService,"6678");
            ExportRecord(&dljf,"%s",sRet);
            alltrim(sRet);
            if(strcmp(sRet,"000000")==0)
            {
	      ExportRecord(&dljf,"%s%lf",rec,&dSjk);
	      DropRecord(&dljf);
	      sprintf(ans_pkg,"%s000000%s%s",pkg_head,djkje,LKZH);
              printf("\n JF ans_pkg is [%s]",ans_pkg);
            }
            else
            {
                   AutoRunTask("dldj",TASK_LKQXJF,"%s","%s",lkjf_pkg,lkjf_receive);
		   ExportRecord(&dljf,"%s",sTcpipErrInfo); 
		   printf("\n Dlywd's err info is [%s]",sTcpipErrInfo);
		   DropRecord(&dljf);
		   //sprintf(ans_pkg,"%s%s%s",pkg_head,sRet,sTcpipErrInfo);
	   	   sprintf(ans_pkg,"%s000000%s%s",pkg_head,djkje,LKZH);
		   return -1;
            }
    }//end else
}

/*��ѯ��������*/
int cxjyzz(char *package,char *ans_pkg)
{
	char pkg_head[201];
	
	memset(pkg_head,'\0',200);
	memcpy(pkg_head,package,88);
	sprintf(ans_pkg,"%s000000%31s",pkg_head," ");
printf("ans_pkg length is  %ld ",strlen(ans_pkg));

}

int cxyjk(char *pkg,char *ans_pkg)
{
   RECORD receive;
   YWMX rYwmx[20+1];
   char pkg_head[101];
   char clsh[41];
   char jstj[101];
   char str[101];
   char package_send[256+1];
   char sRet[6+1];
   char tmp[10001];
   char cYhmc[21];
   char cYhbz1[21];
   double dBl2=0.0;
   double dyjzk=0.0;
   long nJls=0;
   long iLoop;
   long nBl3[21];
   long nBl4[21];
   
   cYhmc[0]='\0';
   cYhbz1[0]='\0';
   
   memset(pkg_head,'\0',101);
   memcpy(pkg_head,pkg,88);
   printf("\n pkg is %s !!!!!",pkg);
   printf("\n pkg_head is %s ",pkg_head);

   memset(clsh,'\0',sizeof(clsh));
   memcpy(clsh,pkg+22,40);
   memset(jstj,' ',100);
   memcpy(jstj,pkg+88,60);
   
   printf("\n clsh is %s ",clsh);
   printf("\n jstj is %s ",jstj);
   memset(package_send,'\0',256);
   sprintf(package_send,"%s|10000|%100.100s|dl185|",clsh,jstj);
   printf("\n package_send is %s",package_send);

   if(AutoRunTask("dldj",TASK_PAY_QUERY,"%s","%r",package_send,&receive)!=0)
   {      
         printf("\n error ͨѶ����!!! ");
    	 return -1;
   }
   ExportRecord(&receive,"%s",sRet);
   alltrim(sRet);
   if(strcmp(sRet,"000000")==0)
   {
     ExportRecord(&receive,"%s%s%lf%ld",rYwmx[0].cYhmc,rYwmx[0].cYhbz1,&rYwmx[0].dBl2,&nJls);
     printf("\n nJls is [%ld]",nJls);
     for(iLoop=0;iLoop<nJls;iLoop++)
     {
         ExportRecord(&receive,"%s%ld%ld%lf[25]%lf%ld(4)%s(4)",
         rYwmx[iLoop].cYhbz,&rYwmx[iLoop].nYsnd,&rYwmx[iLoop].nYsyf,
         rYwmx[iLoop].dSfmx,&rYwmx[iLoop].dYjzk,&rYwmx[iLoop].nBl1,
         &rYwmx[iLoop].nBl2,&nBl3[iLoop],&nBl4[iLoop],rYwmx[iLoop].cBl1,
         rYwmx[iLoop].cBl2,rYwmx[iLoop].cBl3,rYwmx[iLoop].cBl4);
         sprintf(tmp,"%ld%ld%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf%20.2lf",
                 rYwmx[iLoop].dSfmx[0],rYwmx[iLoop].dSfmx[1],rYwmx[iLoop].dSfmx[2],rYwmx[iLoop].dSfmx[3],rYwmx[iLoop].dSfmx[4],rYwmx[iLoop].dSfmx[5],rYwmx[iLoop].dSfmx[6],rYwmx[iLoop].dSfmx[7],rYwmx[iLoop].dSfmx[8],
                 rYwmx[iLoop].dSfmx[9],rYwmx[iLoop].dSfmx[10],rYwmx[iLoop].dSfmx[11],rYwmx[iLoop].dSfmx[12],rYwmx[iLoop].dSfmx[13],rYwmx[iLoop].dSfmx[14],rYwmx[iLoop].dSfmx[15],rYwmx[iLoop].dSfmx[16],rYwmx[iLoop].dSfmx[17],
                 rYwmx[iLoop].dSfmx[18],rYwmx[iLoop].dSfmx[19],rYwmx[iLoop].dSfmx[20],rYwmx[iLoop].dSfmx[21],rYwmx[iLoop].dSfmx[22],rYwmx[iLoop].dSfmx[23],rYwmx[iLoop].dSfmx[24]);
         //strcat(cywmx,tmp);
         dyjzk+=rYwmx[iLoop].dYjzk;
         if(strncmp(clsh+9,"01003",5))
         	break;
     }
printf("\n clsh is [%s]",clsh);
if(strncmp(clsh+9,"01002",5)==0)
{
	dyjzk=20;
	dYzf=20;
}
     DropRecord(&receive);
     sprintf(ans_pkg,"%s000000%80s%11.2lf%4ld%02ld",pkg_head,rYwmx[0].cYhmc,dyjzk,rYwmx[0].nYsnd,rYwmx[0].nYsyf);
   }
   else
   {
      ExportRecord(&receive,"%s",sTcpipErrInfo);
      DropRecord(&receive);
      printf("\n  sTcpipErrInfo is  [%-70.70s]",sTcpipErrInfo);
      sprintf(ans_pkg,"%s%s%s",pkg_head,sRet,sTcpipErrInfo);
      return -1;
   }
   return 0;
}


int cxye(char *pkg,char *ans_pkg)
{
   RECORD receive;
   YWMX rYwmx[20+1];
   char pkg_head[101];
   char clsh[41];
   char jstj[101];
   char str[101];
   char package_send[256+1];
   char sRet[6+1];
   char tmp[10001];
   char cYhmc[21];
   char cYhbz1[21];
   double dBl2=0.0;
   double dyjzk=0.0;
   long nJls=0;
   long iLoop;
   long nBl3[21];
   long nBl4[21];
   
   cYhmc[0]='\0';
   cYhbz1[0]='\0';
   
   memset(pkg_head,'\0',101);
   memcpy(pkg_head,pkg,88);
   printf("\n pkg is %s !!!!!",pkg);
   printf("\n pkg_head is %s ",pkg_head);

   memset(clsh,'\0',sizeof(clsh));
   memcpy(clsh,pkg+22,40);
   memset(jstj,' ',100);
   memcpy(jstj,pkg+88,60);
   
   printf("\n clsh is %s ",clsh);
   printf("\n jstj is %s ",jstj);
   memset(package_send,'\0',256);
   sprintf(package_send,"%s|10000|%100.100s|dl185|",clsh,jstj);
   printf("\n package_send is %s",package_send);

   if(AutoRunTask("dldj",TASK_PAY_QUERY,"%s","%r",package_send,&receive)!=0)
   {      
         printf("\n error ͨѶ����!!! ");
    	 return -1;
   }
   ExportRecord(&receive,"%s",sRet);
   alltrim(sRet);
   if(strcmp(sRet,"000000")==0)
   {
     ExportRecord(&receive,"%s%s%lf%ld",rYwmx[0].cYhmc,rYwmx[0].cYhbz1,&rYwmx[0].dBl2,&nJls);
     DropRecord(&receive);
     sprintf(ans_pkg,"%s000000%80s%11.2lf%4ld%02ld",pkg_head,rYwmx[0].cYhmc,rYwmx[0].dBl2,dyjzk,rYwmx[0].nYsnd,rYwmx[0].nYsyf);
   }
   else
   {
      ExportRecord(&receive,"%s",sTcpipErrInfo);
      DropRecord(&receive);
      printf("\n  sTcpipErrInfo is  [%-70.70s]",sTcpipErrInfo);
      sprintf(ans_pkg,"%s%s%s",pkg_head,sRet,sTcpipErrInfo);
      return -1;
   }
   return 0;
}

