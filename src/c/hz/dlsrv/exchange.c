#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/timeb.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>

#include"dlsrv.h"
#include"exchange.h"
#include"newtask.h"
#include"tasktool.h"
#include"filechar.h"
#include"sql.h"

//#define bell() putchar('\a')
extern long nSysLkrq,nSysSfm;

void dltxmain()
{
 
   passiveTCP("6700",1);

   if(fork()==0) dsUnicomFileTrans();   //联通文件通信
   if(fork()==0) dsMobileAccount();     //移动自动对帐进程
   //if(fork()==0) dsB2iAccount();       //保险自动对帐进程
   if(fork()==0) dsTelAccount();	//电信自动对帐进程
   if(fork()==0) dsTpyFileTrans();       //太平洋文件通信
   exit(0);
   
}

long SendFileByFtp(char * host,char * user,char * password,char * remote,char * local)
{
	
   FILE * fp;
   char scribefile[21],buf[100];
      
   GetTempFileName(scribefile);
   fp=fopen(scribefile,"wb");
   if(!fp) return -1;
   
   fprintf(fp,"ftp -v -n -i %s <<HEAR\n",host);
   fprintf(fp,"user %s %s\n",user,password);
   fprintf(fp,"bin\n");
   fprintf(fp,"put %s %s\n",local,remote);
   fprintf(fp,"quit\n");
   fprintf(fp,"HEAR\n");
   fclose(fp);
   
   sprintf(buf,"chmod +x %s",scribefile);
   system(buf);
   
   system(scribefile);
   remove(scribefile);
   
   sprintf(buf,"%s.check",local);
   
   if(GetFileByFtp(host,user,password,remote,buf)==0)
   {
   	remove(buf);
   	return 0;
   }
   return -1;

}

long GetFileByFtp(char * host,char * user,char * password,char * remote,char * local)
{
	
   FILE * fp;
   char scribefile[21],buf[100];
      
   GetTempFileName(scribefile);
   fp=fopen(scribefile,"wb");
   if(!fp) return -1;
   
   fprintf(fp,"ftp -v -n -i %s <<HEAR\n",host);
   fprintf(fp,"user %s %s\n",user,password);
   fprintf(fp,"bin\n");
   fprintf(fp,"get %s %s\n",remote,local);
   fprintf(fp,"quit\n");
   fprintf(fp,"HEAR\n");
   fclose(fp);
   
   sprintf(buf,"chmod +x %s",scribefile);
   system(buf);
   
   system(scribefile);
   remove(scribefile);
   if(!access(local,0)) return 0;
   return -1;
   
}

void dsMobileAccount()
{
   long nRetVal,nAccountTime=0,nLastTime=0;
   char sJym[16],sResult[256];
   static long nFlag=0;
   
   signal(SIGHUP,SIG_IGN);
   printf("\nMobile Account server started...\n移动自动对帐进程启动完毕...\n");
   for(;;)
   {
       printf("\n正在连接数据库...");

       if(Dlsrv_ConnectDatabase()==0)
       {
       	   printf("\n连接数据库失败,等待10分钟...");
           sleep(600);
       	   continue;
       }

       nRetVal=RunSelect("select cjym from dl$jym where ncxms=130 into %s",sJym);
       
       if(nRetVal<=0)
       {
       	   printf("\n没有移动代收费交易，程序退出...");
       	   CloseDatabase();
       	   return;
       }
       
       nRetVal=RunSelect("select * from dl$zwmx where ncsbz=1 and cjym=%s",sJym);
       
       if(nRetVal>0)
            nRetVal=RunSelect("select to_number(ccs) from dl$jymcsfb "
                         "where cjym=%s and nbh=35 into %ld",sJym,&nAccountTime);
       if(nRetVal>0)
            nRetVal=RunSelect("select to_number(ccs) from dl$jymcsfb "
                         "where cjym=%s and nbh=34 into %ld",sJym,&nLastTime);
       
       GetSysLkrq();
       
       strcpy(sResult,".");
       if(nRetVal>0 && nAccountTime>0 && nSysLkrq>nLastTime && nSysSfm>nAccountTime)
       {
          if(!nFlag) AutoRunTask("dldj",TASK_MOBILE_SIGN_OUT,"%s","%s",sJym,sResult);
          else nFlag=1;
          nRetVal=AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sResult);
       }
       CloseDatabase();
       sleep(1200);                         
   }
}

void dsUnicomFileTrans()
{

   long nYck,nDsf; 
   char sYcJym[16],sDsJym[16];
   
   signal(SIGHUP,SIG_IGN);
   printf("\nUnicom file exchanger started...\n联通文件通信服务进程启动完毕...\n");
   for(;;)
   {
       printf("\n正在连接数据库...");

       if(Dlsrv_ConnectDatabase()==0)
       {
       	   printf("\n连接数据库失败,等待10分钟...");
           sleep(600);
       	   continue;
       }

       nYck=RunSelect("select cjym from dl$jym where ncxms=111 into %s",sYcJym);
       nDsf=RunSelect("select cjym from dl$jym where ncxms=110 into %s",sDsJym);
       
       GetSysLkrq();
       
       if(nYck<=0 && nDsf<=0)
       {
       	   printf("\n没有联通代收费的相关业务...");
       	   CloseDatabase();
       	   return ;
       }
       
       printf("\n形成并发送文件...");
       if(nYck>0) dsFormAndSendYckFile(sYcJym);
       if(nDsf>0) dsFormAndSendDsfFile(sDsJym);

       CloseDatabase();	
       printf("\n等待10分钟...");
       
       sleep(600);
   }

}

//预存款文件
void dsFormAndSendYckFile(char * sJym)
{

     FILE * fp;
     char path[31],username[21],password[21],remotehost[21],remotedir[41];
     char sFileName[81],sDljgh[21],sYhbz[21],sCzydh[7],sJh[10];
     char sRemotePath[31];
     long nRetVal,nCscs,nLkrq,nSfm,nXh,nAccountTime,nCount,nId,nLastDate=0;
     double dSjk;
     
     sprintf(path,"sendfile/1/%ld",nSysLkrq);
     CreateDir(path);
     
     RunSelect("select to_number(ccs) from dl$jymcsfb where cjym=%s and nbh=34 into %ld",
               sJym,&nLastDate);           
     if(nLastDate>=nSysLkrq) return;               
     if(RunSelect("select * from dl$zwmx where ncsbz<=1 and cjym=%s",sJym)<=0) return;

     nRetVal=RunSelect("select * from dl$zwmx where ncsbz=0 and cjym=%s and substr(clxbz,19,1)='0'",sJym);
                       
     RunSelect("select ccs from dl$jymcsfb where nbh=39 and cjym=%s into %s",sJym,sDljgh);
     RunSelect("select ccs from dl$jymcsfb where nbh=49 and cjym=%s into %s",sJym,username);
     RunSelect("select ccs from dl$jymcsfb where nbh=48 and cjym=%s into %s",sJym,password);
     RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=42 and cjym=%s into %ld",
                sJym,&nAccountTime);

     if(nRetVal>0)
     {
        nCscs=0;
        RunSelect("select to_number(ccs) from dl$jymcsfb where \
                   nbh=40 and cjym=%s into %ld",sJym,&nCscs);
        if(RunSelect("select * from dl$zwmx where cjym=%s "
                     "and nlkrq=%ld and ncsbz=1 ",sJym,nSysLkrq)<=0) nCscs=0;

        nCscs++;
        nCscs%=1000;
        RunSql("update dl$jymcsfb set ccs='%ld' where cjym=%s and nbh=40",nCscs,sJym);
        RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=47 into %s",sJym,sRemotePath);
      
        sprintf(sFileName,"%s/dsf%s%08ld%04ld",path,sDljgh,nSysLkrq,nCscs);
        sprintf(remotedir,"%s/dsf%s%08ld%04ld",sRemotePath,sDljgh,nSysLkrq,nCscs);
        
        printf("\n正在形成联通预存款次文件：%s...",sFileName);

        if(!(fp=fopen(sFileName,"wb")))
        {
             RollbackWork();
             return;
        }
        	
        nCount=0;
        nId=OpenCursor("select nxh,cyhbz,dsjk,cjh,cczydh,nlkrq,nsfm from dl$zwmx where"
        " cjym=%s and nxzbz>=2 and ncsbz=0",sJym);
        if(nId<0)
        {
                fclose(fp);
                RollbackWork();
                return ;
        }
        for(;;)
        {
               if(FetchCursor(nId,"%ld%s%lf%s%s%ld%ld",
                  &nXh,sYhbz,&dSjk,sJh,sCzydh,&nLkrq,&nSfm)<=0) break;
               nCount++;
               fprintf(fp,"%-12.12s,%12.2lf,%-2ld,%-9s,%-4.4s,%02ld/%02ld/%04ld,%-6ld\n",
                      sYhbz,dSjk,1,sJh,sCzydh,(nLkrq%10000)/100,(nLkrq%10000)%100,(nLkrq/10000),nSfm);

               RunSql("update dl$zwmx set ncsbz=1,ncscs=%ld where cjym=%s  and ncsbz=0 and nxh=%ld",
                       nCscs,sJym,nXh);
        }
        CloseCursor(nId);
        fclose(fp);
        printf("\n已形成预收款次文件：%s,共%ld条记录，正在传输...\n",sFileName,nCount);
        if(SendFileByFtp("dl_gs_1",username,password,remotedir,sFileName)==0) CommitWork();
        else
        {
              RollbackWork();
              //bell();
              printf("\n无法向联通公司传送文件：%s\n",sFileName,nCount);
        }      

     }     	
     
     if(nAccountTime>nSysSfm) return;
     if(RunSelect("select * from dl$zwmx where ncsbz=2 where "
                  "cjym=%s and nlkrq=%ld",sJym,nSysLkrq)>0) return;
     nRetVal=RunSelect("select * from dl$zwmx where ncsbz=1 and cjym=%s",sJym);
            
     if(nRetVal>0)
     {
        RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=46 into %s",sJym,sRemotePath);
        sprintf(sFileName,"%s/dsdzwj%s%08ld",path,sDljgh,nSysLkrq);
        sprintf(remotedir,"%s/dsdzwj%s%08ld",sRemotePath,sDljgh,nSysLkrq);
        
        printf("\n正在形成联通预存款日文件：%s...",sFileName);

        if(!(fp=fopen(sFileName,"wb")))
        {
             RollbackWork();
             return;
        }
        	
        nCount=0;
        nId=OpenCursor("select nxh,cyhbz,dsjk,cjh,cczydh,nlkrq,nsfm from dl$zwmx where "
        "cjym=%s and nxzbz>=2 and ncsbz=1 ",sJym);
        if(nId<0)
        {
                fclose(fp);
                RollbackWork();
                return ;
        }
        for(;;)
        {
               if(FetchCursor(nId,"%ld%s%lf%s%s%ld%ld",
                  &nXh,sYhbz,&dSjk,sJh,sCzydh,&nLkrq,&nSfm)<=0) break;
               nCount++;
               fprintf(fp,"%-12.12s,%12.2lf,%-2ld,%-9s,%-4.4s,%02ld/%02ld/%04ld,%-6ld\n",
                      sYhbz,dSjk,1,sJh,sCzydh,(nLkrq%10000)/100,(nLkrq%10000)%100,(nLkrq/10000),nSfm);
               RunSql("update dl$zwmx set ncsbz=2 where cjym=%s and ncsbz=1 and nxh=%ld",sJym,nXh);
        }
        CloseCursor(nId);
        fclose(fp);
        if(SendFileByFtp("dl_gs_1",username,password,remotedir,sFileName)==0)
        {
              RunSql("update dl$jymcsfb set ccs='%ld' where cjym=%s and nbh=34",nSysLkrq,sJym);
              CommitWork();
        }      
        else
        {
              RollbackWork();
              //bell();
              printf("\n无法向联通公司传送文件：%s\n",sFileName,nCount);
        }      

     }     
     CommitWork();

}

void dsFormAndSendDsfFile(char * sJym)
{

     FILE * fp;
     char path[31],username[21],password[21],remotehost[21],remotedir[41];
     char sRemotePath[31];
     char sFileName[81],sDljgh[21],sYhbz[21],sCzydh[7],sJh[10];
     long nRetVal,nCscs,nLkrq,nSfm,nXh,nAccountTime,nCount,nSfkfs,nId,nMonthDay,nLastDate;
     double dSjk;
     
     sprintf(path,"sendfile/1/%ld",nSysLkrq);
     CreateDir(path);
     
     if(RunSelect("select * from dl$zwmx where ncsbz<=1 and cjym=%s",sJym)<=0) return;

     nRetVal=RunSelect("select * from dl$zwmx where ncsbz=0 and cjym=%s "
                       "and substr(clxbz,19,1)='0'",sJym);
                       
     RunSelect("select ccs from dl$jymcsfb where nbh=39 and cjym=%s into %s",sJym,sDljgh);
     RunSelect("select ccs from dl$jymcsfb where nbh=49 and cjym=%s into %s",sJym,username);
     RunSelect("select ccs from dl$jymcsfb where nbh=48 and cjym=%s into %s",sJym,password);
     RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=42 and cjym=%s into %ld",
                sJym,&nAccountTime);
     RunSelect("select to_number(ccs) from dl$jymcsfb where nbh=41 and cjym=%s into %ld",
                sJym,&nMonthDay);

     printf("\nExist:%ld,time=%ld,%ld",nRetVal,nSysSfm,nAccountTime);
     if(nRetVal>0 && nSysSfm>nAccountTime)
     {
      
        if(RunSelect("select * from dl$zwmx where ncsbz>=1 and "
                     "cjym=%s and nlkrq>=%ld and substr(clxbz,19,1)='0'",
                     sJym,nSysLkrq)>0) return;

        RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=47 into %s",sJym,sRemotePath);

        sprintf(sFileName,"%s/LT%08ld01.txt",path,nSysLkrq);
        sprintf(remotedir,"%s/LT%08ld01.txt",sRemotePath,nSysLkrq);
        
        printf("\n正在形成联通代收日文件：%s...",sFileName);

        if(!(fp=fopen(sFileName,"wb")))
        {
             RollbackWork();
             return;
        }
        nCount=0;
        nId=OpenCursor("select nxh,cyhbz,dsjk,nsfkfs from dl$zwmx "
                       "where cjym=%s and nxzbz>=2 and ncsbz=0 and substr(clxbz,19,1)='0'",sJym);
        if(nId<0)
        {
                fclose(fp);
                RollbackWork();
                return ;
        }
        for(;;)
        {

               if(FetchCursor(nId,"%ld%s%lf%ld",&nXh,sYhbz,&dSjk,&nSfkfs)<=0) break;
               nCount++;
               fprintf(fp,"%-11.11s|%12.2lf|%1ld|\n",sYhbz,dSjk,nSfkfs);
               RunSql("update dl$zwmx set ncsbz=1 where cjym=%s and nxh=%ld",sJym,nXh);
        }
        CloseCursor(nId);
        fclose(fp);
        printf("\n已形成代收费日文件：%s,共%ld条记录，正在传输...\n",sFileName,nCount);
        if(SendFileByFtp("dl_gs_1",username,password,remotedir,sFileName)==0) CommitWork();
        else
        {
              RollbackWork();
              //bell();
              printf("\n无法向联通公司传送文件：%s\n",sFileName,nCount);
        }      
     }     	
     
     RunSelect("select to_number(ccs) from dl$jymcsfb where cjym=%s and nbh=34 into %ld",
               sJym,&nLastDate);           
     if(nLastDate>=nSysLkrq/100) return;               
     if(nAccountTime>nSysSfm||nSysLkrq%100<nMonthDay) return;
     nRetVal=RunSelect("select * from dl$zwmx where ncsbz=1 and cjym=%s",sJym);
            
     if(nRetVal>0)
     {
        RunSelect("select ccs from dl$jymcsfb where cjym=%s and nbh=46 into %s",sJym,sRemotePath);
        sprintf(sFileName,"%s/LT%06ld.txt",path,nSysLkrq/100);
        sprintf(remotedir,"%s/LT%06ld.txt",sRemotePath,nSysLkrq/100);
        
        printf("\n正在形成联通代收费月文件：%s...",sFileName);

        if(!(fp=fopen(sFileName,"wb")))
        {
             RollbackWork();
             return;
        }
        	
        nCount=0;
        nId=OpenCursor("select nxh,cyhbz,dsjk,nsfkfs,nlkrq from dl$zwmx "
        "where cjym=%s and nxzbz>=2 and ncsbz=1 and substr(clxbz,19,1)='0'",sJym);
        if(nId<0)
        {
                fclose(fp);
                RollbackWork();
                return ;
        }
        for(;;)
        {
               if(FetchCursor(nId,"%ld%s%lf%ld%ld",&nXh,sYhbz,&dSjk,&nSfkfs,&nLkrq)<=0) break;
               nCount++;
               fprintf(fp,"%-11.11s|%-10.10s|%12.2lf|%1ld|%02ld/%02ld/%04ld|\n",
               sYhbz,sDljgh,dSjk,nSfkfs,(nLkrq%10000)/100,(nLkrq%10000)%100,(nLkrq/10000));
               RunSql("update dl$zwmx set ncsbz=2 where cjym=%s and nxh=%ld",sJym,nXh);
        }
        CloseCursor(nId);
        fclose(fp);
        if(SendFileByFtp("dl_gs_1",username,password,remotedir,sFileName)==0)
        {
              RunSql("update dl$jymcsfb set ccs='%ld' where cjym=%s and nbh=34",nSysLkrq/100,sJym);
              CommitWork();
        }      
        else
        {
              RollbackWork();
              //bell();
              printf("\n无法向联通公司传送文件：%s\n",sFileName);
        }      

     }

}

void GetSysLkrq()
{
	struct tm now_time;
	time_t now; 
	time(&now);
	now_time=*localtime(&now);
	
	nSysLkrq=(now_time.tm_year+1900)*10000+(now_time.tm_mon+1)*100+now_time.tm_mday;
	nSysSfm=now_time.tm_hour*10000+now_time.tm_min*100+now_time.tm_sec;
}

void  CreateDir(char * path)
{
      char buf[81];
      
      if(access(path,0))
      {
      	 sprintf(buf,"mkdir -p  %s",path);
      	 system(buf);
      	 sprintf(buf,"chmod 777 %s",path);
      	 system(buf);
      }
}

void dsB2iAccount()
{
   long nRetVal,nAccountTime=0,nLastTime=0;
   char sJym[16],sResult[256];
   
   signal(SIGHUP,SIG_IGN);
   printf("\nB2i(BX) Account server started...\n保险自动对帐进程启动完毕...\n");
   for(;;)
   {
       printf("\n正在连接数据库...");

       if(Dlsrv_ConnectDatabase()==0)
       {
       	   printf("\n连接数据库失败,等待10分钟...");
           sleep(600);
       	   continue;
       }

       nRetVal=RunSelect("select cjym from dl$jym where ncxms=821 into %s",sJym);
       
       if(nRetVal<=0)
       {
       	   printf("\n没有保险代收费交易，程序退出...");
       	   CloseDatabase();
       	   return;
       }
       
       nRetVal=RunSelect("select * from dl$zwmx where ncsbz=1 and cjym=%s",sJym);
       
       if(nRetVal>0)
            nRetVal=RunSelect("select to_number(ccs) from dl$jymcsfb "
                         "where cjym=%s and nbh=35 into %ld",sJym,&nAccountTime);
       if(nRetVal>0)
            nRetVal=RunSelect("select to_number(ccs) from dl$jymcsfb "
                         "where cjym=%s and nbh=34 into %ld",sJym,&nLastTime);
       
       GetSysLkrq();
       
       strcpy(sResult,".");
       if(nRetVal>0 && nAccountTime>0 && nSysLkrq>nLastTime && nSysSfm>nAccountTime)
       {
          nRetVal=AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sResult);
       }
       CloseDatabase();
       sleep(1200);                         
   }
	
}

void dsTelAccount()
{
   long nRetVal,nAccountTime=0,nLastTime=0;
   char sJym[16],sResult[256];
   
   signal(SIGHUP,SIG_IGN);
   printf("\nTelecom Account server started...\n电信自动对帐进程启动完毕...\n");
   for(;;)
   {
       printf("\n正在连接数据库...");

       if(Dlsrv_ConnectDatabase()==0)
       {
       	   printf("\n连接数据库失败,等待10分钟...");
           sleep(600);
       	   continue;
       }

       nRetVal=RunSelect("select cjym from dl$jym where ncxms=140 into %s",sJym);
       
       if(nRetVal<=0)
       {
       	   printf("\n没有固定电话代收费交易，程序退出...");
       	   CloseDatabase();
       	   return;
       }
       
       nRetVal=RunSelect("select * from dl$zwmx where ncsbz=1 and cjym=%s",sJym);
       
       if(nRetVal>0)
            nRetVal=RunSelect("select to_number(ccs) from dl$jymcsfb "
                         "where cjym=%s and nbh=35 into %ld",sJym,&nAccountTime);
       if(nRetVal>0)
            nRetVal=RunSelect("select to_number(ccs) from dl$jymcsfb "
                         "where cjym=%s and nbh=34 into %ld",sJym,&nLastTime);
       
       GetSysLkrq();
       
       strcpy(sResult,".");
       if(nRetVal>0 && nAccountTime>0 && nSysLkrq>nLastTime && nSysSfm>nAccountTime)
       {
          nRetVal=AutoRunTask("dldj",TASK_PAY_CHECKACCOUNT,"%s","%s",sJym,sResult);
       }
       CloseDatabase();
       printf("\nTEST:电信对帐成功:日期:[%ld]时间:[%ld]",nSysLkrq,nSysSfm);
       sleep(1200);                         
   }
	
}

//太平洋文件通信进程
void dsTpyFileTrans()
{
   long nRetVal,nAccountTime=0,nDate=0;
   char sJym[16],sResult[256],remotedir[41],sRemotePath[41];
   char username[21],password[21],sFileName[81]=".",path[40];
   
   signal(SIGHUP,SIG_IGN);
   printf("\nTPY(BX) FileTranc server started...\n太平洋保险自动传输进程启动完毕...\n");
   for(;;)
   {
       printf("\n正在连接数据库...");

       if(Dlsrv_ConnectDatabase()==0)
       {
       	   printf("\n连接数据库失败,等待10分钟...");
           sleep(600);
       	   continue;
       }

       nRetVal=RunSelect("select cjym from dl$jym where ncxms=333 into %s",sJym);
       
       if(nRetVal<=0)
       {
       	   printf("\n没有太平洋保险代收费交易，程序退出...");
       	   CloseDatabase();
       	   return;
       }
       
     RunSelect("select ccs from dl$jymcsfb where nbh=49 and cjym=%s into %s",sJym,username);
     RunSelect("select ccs from dl$jymcsfb where nbh=48 and cjym=%s into %s",sJym,password);
     RunSelect("select ccs from dl$jymcsfb where nbh=47 and cjym=%s into %s",sJym,sRemotePath);
     RunSelect("select to_number(ccs) from dl$jymcsfb where cjym=%s and nbh=42 into %ld",sJym,&nAccountTime);
     RunSelect("select to_number(ccs) from dl$jymcsfb where cjym=%s and nbh=34 into %ld",sJym,&nDate);
     GetSysLkrq();
     strcpy(sResult,".");
     if(nAccountTime>0 && nSysSfm>nAccountTime && nDate<nSysLkrq)
     {
          nRetVal=dsPostToTpyFile(sJym,sResult);
          if(nRetVal!=0) {CloseDatabase(); return;}
	  printf("\n已形成太平洋保险代收费日文件：%s,正在传输...\n",sResult);
  	  
  	  sprintf(sFileName,"YZBX%8ld.txt",nSysLkrq);
	  sprintf(remotedir,"%s/SEND/%s",sRemotePath,sFileName);
	  if(SendFileByFtp("dl_gs_12",username,password,remotedir,sResult)==0)
	  {
	  	RunSql("update dl$ywmx_%t set ncsbz=2 where nxzbz=1 and nlkrq<=%ld",sJym,nSysLkrq);
	  	if(RunSql("update dl$jymcsfb set ccs='%ld' where cjym=%s and nbh=34",nSysLkrq,sJym)>=0)
	      	CommitWork();
          }
          else  printf("\n无法向太平洋公司传送文件：%s\n",sFileName);
          
          //自动获得文件
          strcpy(path,"recvfile/9");
    	  CreateDir("olddata");
    	  CreateDir(path);
    	  
    	  sprintf(sFileName,"syz0002d%8ld.txt",nSysLkrq);
          sprintf(remotedir,"%s/RECV/%s",sRemotePath,sFileName);
          sprintf(sResult,"%s/%s",path,sFileName);

          if(GetFileByFtp("dl_gs_12",username,password,remotedir,sResult)==0)
          {
          	if(TpyFileImport(sJym,path,sFileName)==0) 
          	{
        		sprintf(remotedir,"%s/RECV/tpyrk.log",sRemotePath);
        		SendFileByFtp("dl_gs_12",username,password,remotedir,"./tpyrk.log");
          		printf("\nTEST:太保入库成功:日期:[%ld]时间:[%ld]",nSysLkrq,nSysSfm);
          	} //文件入库
          }
          else  printf("\n无法收到太平洋公司传送文件：%s\n",sFileName);

     }//end if
     CloseDatabase();
     sleep(1800);
   }//end for
}

long dsPostToTpyFile(char * cJym,char * sFileName)
{
    FILE *fp;
    long nId,nRetVal;
    long nLkrq,nSfm,nBl1;
    char path[31];
    char buf[80],sYhbz[21],sFph[20],sYhbz1[21],sJh[11],sCzydh[8],sJm[30],sCzymc[12];
    double dSjk=0;
     
    strcpy(path,"sendfile/9");
    CreateDir(path);
    
    sprintf(sFileName,"%s/YZBX%8ld.txt",path,nSysLkrq);
    if(!(fp=fopen(sFileName,"wb")))
    {
       RollbackWork();
       return -1;
    }

    nId=OpenCursor("select cyhbz,cyhbz1,nbl1,dsfmx1,nlkrq,nsfm,cbl8,cjh,cczydh from dl$ywmx_%t "
       "where  dsjk>0 and nxzbz=1 and ncsbz<>2",cJym);

    if(nId<0)
    {
    	fclose(fp);
        RollbackWork();
        return -1;
    }
    
    while(1)
    {
	sFph[0]='\0';
    	nRetVal=FetchCursor(nId,"%s%s%ld%lf%ld%ld%s%s%s",
    		sYhbz,sYhbz1,&nBl1,&dSjk,&nLkrq,&nSfm,sFph,sJh,sCzydh);
    	if(nRetVal<=0) break;
    	
    	RunSelect("select cczymc from dl$czyb where cczydh=%s into %s",sCzydh,sCzymc);
    	RunSelect("select cjm from dl$jh where cjh=%s into %s",sJh,sJm);
    	
    	fprintf(fp,"%s|%s|%ld|%.2lf|%02ld/%02ld/%ld %02ld:%02ld:%02ld|%s|%s|%s|%s|%s||\n",
    		sYhbz,sYhbz1,nBl1,dSjk,
    		(nLkrq%10000)/100,(nLkrq%10000)%100,nLkrq/10000,
    		nSfm/10000,(nSfm%10000)/100,(nSfm%10000)%100,
    		sFph,sJh,sJm,sCzydh,sCzymc);
    }//end message
    CloseCursor(nId);
    fclose(fp);
    
    return 0;
}

long TpyFileImport(char * sJym,char * sDirect,char * sFileName)
{

    FILE * fp;
    char sDump[40],sRealFileName[100],sData[400],sLsh[41],sRealLsh[41];
    long nId,nXh,nYsnd,nYsyf,nDay;
    char sYhbz[20],sYhbz1[22],sYhmc[41],sId[20],sName[20],sYjrq[20],sQq[20],sZq[20],sRq[20],sFsmc[10],sZt[2],sBt[11],sYw[9],sYwy[11],sSf[9],sSfy[11];
    double dTotalZk=0,dSjk;
    long nBz,nQc,nNx,nFsm;
    char * offset,temp[12];
   
    sprintf(sRealFileName,"%s/%s",sDirect,sFileName);
    
    GetSysLkrq();
           
    //printf("\n正在清除历史数据...");
    //if(RunSql("delete dl$ywmx_%t where nxzbz=3",sJym)>0) CommitWork();
    
    printf("\n正在入库...");
    nXh=0;
    GenerateLsh(1,sJym,"271501001",sLsh);
    
    if(!(fp=fopen(sRealFileName,"rt")))
    {
    	printf("\n打开文件出错!");
    	return -1;
    }
   
    for(;;)
    {
    	memset(sData,0,sizeof(sData));
    	if(getline(sData,fp)<=0) break;
    	alltrim(sData);
        offset=sData;
        nXh++;
        sprintf(sRealLsh,"%-32.32s%08ld",sLsh,nXh);
  
    	offset=FetchStringToArg(sData,"%s|%s|%s|%d|%s|%s|%f|%d|%d|%s|"
    			"%s|%s|%s|%s|%s|%d|%s|%s|%s|%s|%s|"
    			"%s|%s|%s|%s|%s|%s|%s|%s|",
        	sYhbz,sYhbz1,sYhmc,&nBz,sId,sName,&dSjk,&nQc,&nNx,sYjrq,
        	sQq,sZq,temp,temp,sRq,&nFsm,sFsmc,sZt,sBt,temp,temp,
        	sYw,sYwy,sSf,sSfy,temp,temp,temp,temp);
       
        if(!offset)
        {
            nId=-1;
            continue;
        }
        
 	if(RunSelect("select ROWNUM from dl$ywmx_%t where cyhbz=%s and cyhbz1=%s and \
			nbl1=%ld and cbl7=2 and nxzbz<>1",sJym,sYhbz,sYhbz1,nQc)>0)
	{
		RunSql("delete dl$ywmx_%t where cyhbz=%s and cyhbz1=%s and nbl1=%ld",
			sJym,sYhbz,sYhbz1,nQc);
	}
        
        FetchStringToArg(sYjrq,"%2d%1s%2d%1s%4d%9s",&nYsyf,temp,&nDay,temp,&nYsnd,sDump);
        
        nId=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,"
        "nywlb,nxh,dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,"
        "dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,dsfmx18,dsfmx19,dsfmx20,"
        "dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,njscz2,djscz3,djscz4,nsl,"
        "dyjzk,dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,"
        "cbl3,cbl4,cbl5,cbl6,cbl7,cbl8,cbl9,cbl10,cbl11,cbl12,cbl13,cbl14,"
        "cbl15,cbl16,cbl17,cbl18,"
        "cjh,cczydh,nlkrq,nsfm,clxbz,nxzbz,ncsbz,cbz,cylsh,clsh) "
        "VALUES(%s,%ld,%ld,%s,%s,%s,%s,%ld,%ld,%lf,0,0,0,0,0,0,0,0,0,"
        "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'.',%ld,0,0,1,%lf,0,0,%lf,0,%ld,%ld,%ld,%ld,"
        "0,0,%s,%s,%s,%s,%s,%s,%s,'.',%s,%s,%s,%s,%s,'.','.','.','.','.',"
        "'0','XTWH',%ld,%ld,'0000000000',0,0,'.',%s,'0')",
        sJym,sJym,nYsnd,nYsyf,sYhbz,sYhbz1,sYhmc,sName,0,nXh,dSjk,
        nSysLkrq,dSjk,dSjk,nBz,nNx,nQc,nFsm,
        sId,sYjrq,sQq,sZq,sRq,sFsmc,sZt,sBt,sYw,sYwy,sSf,sSfy,
        nSysLkrq,nSysSfm,sRealLsh);
        
        if(nId<0)
        {
            {
            	FILE * fp;
            	fp=fopen("tpyrk.log","ab");
		sprintf(sDump,"入库出错日期:%ld,时间:%ld\n",nSysLkrq,nSysSfm);
            	fwrite(sDump,strlen(sDump),1,fp);
            	fwrite(sData,strlen(sData),1,fp);
            	fwrite("\n",1,1,fp);
            	fclose(fp);
            }//ftp 
            GetSqlErrInfo(sData);
	    printf("\nERROR:%s\n",sData);
            continue;
        }
	
        dTotalZk+=dSjk;
        if((nXh%1000)==0) CommitWork();    
    }

    CommitWork();
    fclose(fp);
    if(nId>0)
    {
        sprintf(sData,"olddata/%s",sFileName);
        rename(sRealFileName,sData);        
    }
    printf("入库成功，等待下次入库");
    return 0;
}
