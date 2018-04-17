/*              通信机文件生成模块
 *
 *    programe:tom
 *    created: 2001.12.24
 *
 */

#include<sys/stat.h>
#include<dirent.h>
#include"dlpublic.h"
#include"query.h"
#include"tools.h"

extern long nSysLkrq,nSysSfm;
extern int errno;
extern char sSysJsdh[10];
extern char sSysCzymc[20];

long PostToTaxFile(char * cJym,long nFlag);
long PostToTerraFile(char * cJym,long nFlag);
long PostToTpyFile(char * cJym,long nFlag);
long PostToMsTerraFile(char * cJym,long nFlag);
long PostToPowerFile(char * cJym,long nFlag);
long CdPostToTpyFile(char * cJym,long nFlag);

void FileDownload()  //文件交换，通讯机使用
{
  
    char sJym[16],sJymc[51];
    long nCxms;
    
    clrscr();
    
    if(SelectJym(sJym,4)<=0) return;
    RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);
    
    if(MessageBox("    敬告:新的数据文件生成文件后,还可以再生成."
    "建议您检查一下!要继续吗?",0)!=KEY_YES) return;

    switch(nCxms)
    {
    	case 521:
	     PostToTaxFile(sJym,1);
	     break;
	case 222:
	     if(strstr(sSysJsdh,"2715"))
	          PostToMsTerraFile(sJym,1);
	     else PostToTerraFile(sJym,1);
	     break;
	case 333:
	     if(strstr(sSysJsdh,"2701"))
		  CdPostToTpyFile(sJym,1);    
	     else PostToTpyFile(sJym,1);
	     break;
	case 125:
	     PostToPowerFile(sJym,1);
	     break;
        default:
             MessageBox("该交易不需要文件入库...",1);
             break;
    }
        
}

////////////////////国税文件出库(非时实收费方式)/////////
long PostToTaxFile(char * cJym,long nFlag)
{
    FILE *fp;
    long nId,nRetVal,nQsrq,nJzrq=nSysLkrq;
    long nLkrq,nCscs=0,nCount,nKey,nXh;
    char path[31],sFileName[81];
    char sDylx[3],sYhbz1[40],sWszh[9],sZspl[3];
    char buf[80];
    char sBankCode[11],sKhh[50],sZh[50],sJh[10];
    double dSfmx1,dSfmx2,dSfmx3;
        
    clrscr();
    nQsrq=nSysLkrq-10;
    for(;;)
    {
       if(GetValue(4,2,"请输入汇总起始日期:%8ld",&nQsrq)==KEY_ESC) return;
       nKey=GetValue(4,3,"请输入汇总截止日期:%8ld",&nJzrq);
       if(nJzrq<nQsrq) continue;
       if(nKey==KEY_ESC) return;
       if(nKey==KEY_RETURN) break;
    }//end for

    strcpy(path,"sendfile/10");
    if(access(path,0))
    	mkdir(path,S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);

    sprintf(sFileName,"%s/TAX%8ld.txt",path,nSysLkrq);
     
    RunSelect("select ccs from dl$jymcsfb where \
    		cjym=%s and nbh=2 into %s",cJym,sBankCode);
    alltrim(sBankCode); 
    RunSelect("select ccs from dl$jymcsfb where \
    		cjym=%s and nbh=8 into %s",cJym,sKhh);
    
    printf("\n\n   正在形成国税汇总文件：%s...",sFileName);
    if(!(fp=fopen(sFileName,"wb")))
    {
       RollbackWork();
       return -1;
    }

    nCount=0;
    nId=OpenCursor("select cyhbz1,cyhbz2,cbl1,nlkrq,cbl8,dsfmx2,dsfmx3,dsfmx5,cjh from dl$ywmx_%t "
    		   "where nxzbz=1 and dsjk>0 and nlkrq between %ld and %ld order by cyhbz1",
	           cJym,nQsrq,nJzrq);

    if(nId<=0)
    {
    	fclose(fp);
        RollbackWork();
        return -1;
    }
    
    while(1)
    {
    	nRetVal=FetchCursor(nId,"%s%s%s%ld%s%lf%lf%lf%s",
    	                   sYhbz1,sDylx,sZspl,&nLkrq,sWszh,&dSfmx1,&dSfmx2,&dSfmx3,sJh);
    	if(nRetVal<=0) break;
    	RunSelect("select ckhh,czh from dl$khhgl where cjsdh=%s into %s%s",
    						sJh,sKhh,sZh);
    			
    	fprintf(fp,"|9999|%s|%s|%s|%s|%ld|%s||%-11.2lf|%-6.3lf|%-11.2lf|%s|%s|%s|\n",
    			sYhbz1,sBankCode,sDylx,sZspl,nLkrq,sWszh,dSfmx1,dSfmx2,dSfmx3,sKhh,sZh,sJh);
             	                   
    }
    CloseCursor(nId);
    fclose(fp);
    if(nFlag)
    {
       if(RunSql("update dl$ywmx_%t set nxzbz=3 where nxzbz=0 and "
               "nysyf<=%ld",cJym,(nQsrq%10000)/100)>=0)
       {
          CommitWork();
          sprintf(buf,"xtod %s > %s/inspxx.txt",sFileName,path);
          system(buf);
          outtext("\n   形成文件(%s/inspxx.txt)成功\n   按任意键返回......",path);
        }
        else outtext("\n   形成文件出错，请与鸿志公司联系......");
    }
    else
    {
    	sprintf(buf,"xtod %s > %s/inspday.txt",sFileName,path);
    	system(buf);
        outtext("\n   形成文件[%s]成功\n   按任意键返回......",sFileName);
    }
    keyb();    
    return 0;
}
/////////////////地税出库/////////////////////////////////
long PostToTerraFile(char * cJym,long nFlag)
{
    FILE *fp;
    long nId,nRetVal,nQsrq,nJzrq=nSysLkrq,nZh=2001;
    long nLkrq,nCscs=0,nCount,nKey,nXh;
    char path[31],sFileName[81];
    char buf[80],sYhbz[21],sFph[20];
     
    clrscr();
    nQsrq=nSysLkrq-10;
    for(;;)
    {
       if(GetValue(4,2,"请输入汇总起始日期:%8ld",&nQsrq)==KEY_ESC) return;
       if(GetValue(4,3,"请输入汇总截止日期:%8ld",&nJzrq)==KEY_ESC) return;
       if(nJzrq<nQsrq) continue;
       nKey=GetValue(4,4,"请输入本次发票字号:%4ld",&nZh);
       if(nKey==KEY_ESC) return;
       if(nKey==KEY_RETURN) break;
    }//end for

    strcpy(path,"sendfile/11");
    if(access(path,0))
    	mkdir(path,S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);

    sprintf(sFileName,"%s/DS%8ld.txt",path,nSysLkrq);
    
    printf("\n\n   正在形成地税汇总文件：%s...",sFileName);
    if(!(fp=fopen(sFileName,"wb")))
    {
       RollbackWork();
       return -1;
    }

    nCount=0;
    nId=OpenCursor("select cyhbz,cbl8,nlkrq from dl$ywmx_%t "
       "where  dsjk>0 and nxzbz=1 and nlkrq between %ld and %ld ",
	           cJym,nQsrq,nJzrq);

    if(nId<=0)
    {
    	fclose(fp);
        RollbackWork();
        return -1;
    }
    
    while(1)
    {
	sFph[0]='\0';
    	nRetVal=FetchCursor(nId,"%s%s%ld",sYhbz,sFph,&nLkrq);
    	if(nRetVal<=0) break;
    	
    	fprintf(fp,"%20s%7.7s%4ld  %8ld\n",sYhbz,sFph,nZh,nLkrq);
             	                   
    }
    CloseCursor(nId);
    fclose(fp);
    if(nFlag)
    {
       if(RunSql("update dl$ywmx_%t set nxzbz=3 where nxzbz=0 and "
               "nysyf<=%ld",cJym,(nQsrq%10000)/100)>=0)
       {
          CommitWork();
          sprintf(buf,"xtod %s > %s/terra.txt",sFileName,path);
          system(buf);
          outtext("\n   形成文件(%s/terra.txt)成功\n   按任意键返回......",path);
        }
        else outtext("\n   形成文件出错，请与鸿志公司联系......");
    }
    else
    {
    	sprintf(buf,"xtod %s > %s/terraday.txt",sFileName,path);
    	system(buf);
        outtext("\n   形成文件[%s]成功\n   按任意键返回......",sFileName);
    }
    keyb();    
    return 0;
}

void PostGasToFile(long ntype,char *cjymc,char *cjym)
{
    FILE *fp;
    long nId,nRetVal,nQsrq,nJzrq=nSysLkrq;
    long nLkrq,nCscs=0,nCount,nKey,nXh;
    char path[31],sFileName[81],sYn[3]=".";
    char buf[80],sYhbz[21],sYhmc[41];
    double dSjk=0;
     
    clrscr();
    nQsrq=nSysLkrq-10;
    for(;;)
    {
       if(GetValue(4,2,"请输入汇总起始日期:%8ld",&nQsrq)==KEY_ESC) return;
       nKey=GetValue(4,3,"请输入汇总截止日期:%8ld",&nJzrq);
       if(nKey==KEY_ESC) return;
       if(nKey==KEY_RETURN) break;
    }//end for

    clrscr();
    QueryLocal(
	"  用户号  用户名称                            收费日期  金额        ",
   	"%s %s %ld %.2lf",
   	"select cyhbz,cyhmc,nlkrq,dsjk from dl$ywmx_%t where nlkrq>=%ld and nlkrq<=%ld",
   		cjym,nQsrq,nJzrq);

    clrscr();
    if(GetValue(4,1,"是否进行文件出库操作?(Y/N):%1s",sYn)==KEY_ESC) return;
    upper(sYn);
    if(strcmp(sYn,"Y")) return;
    
    strcpy(path,"sendfile/5");
    if(access(path,0))
    	mkdir(path,S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);

    sprintf(sFileName,"%s/GAS%8ld.txt",path,nSysLkrq);
    
    printf("\n\n   正在形成天然气汇总文件：%s...",sFileName);
    if(!(fp=fopen(sFileName,"wb")))
    {
       RollbackWork();
       return;
    }

    nCount=0;
    nId=OpenCursor("select cyhbz,nlkrq,cyhmc,dsjk from dl$ywmx_%t "
       "where  dsjk>0 and nxzbz=1 and nlkrq between %ld and %ld ",
	           cjym,nQsrq,nJzrq);

    if(nId<=0)
    {
    	fclose(fp);
        RollbackWork();
        return;
    }
    
    while(1)
    {
    	nRetVal=FetchCursor(nId,"%s%ld%s%lf",sYhbz,&nLkrq,sYhmc,&dSjk);
    	if(nRetVal<=0) break;
    	
    	fprintf(fp,"%s|%ld|%s|%lf|\n",sYhbz,nLkrq,sYhmc,dSjk);

    }
    CloseCursor(nId);
    fclose(fp);

    //if(RunSql("update dl$ywmx_%t set nxzbz=3 where nxzbz=0 and "
    //           "nysyf<=%ld",cjym,(nQsrq%10000)/100)>=0)
    //{
    //      CommitWork();
      outtext("\n   形成文件(%s/%s)成功\n   按任意键返回......",path,sFileName);
    //}
    //else outtext("\n   形成文件出错，请与鸿志公司联系......");
    keyb();    
    return;
}

//太平洋保险//
long PostToTpyFile(char * cJym,long nFlag)
{
    FILE *fp;
    long nId,nRetVal,nQsrq,nJzrq=nSysLkrq,nZh=2001;
    long nLkrq,nCscs=0,nCount,nKey,nXh,nSfm,nBl1;
    char path[31],sFileName[81];
    char buf[80],sYhbz[21],sFph[20],sYhbz1[21],sJh[11],sCzydh[8],sJm[30],sCzymc[12];
    double dSjk=0;
     
    clrscr();
    nQsrq=nSysLkrq;
    for(;;)
    {
       if(GetValue(4,2,"请输入汇总起始日期:%8ld",&nQsrq)==KEY_ESC) return;
       nKey=GetValue(4,3,"请输入汇总截止日期:%8ld",&nJzrq);
       if(nJzrq<nQsrq) continue;
       if(nKey==KEY_ESC) return;
       if(nKey==KEY_RETURN) break;
    }//end for

    strcpy(path,"sendfile/9");
    if(access(path,0))
    	mkdir(path,S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);

    sprintf(sFileName,"%s/BX%8ld.txt",path,nSysLkrq);
    
    printf("\n\n   正在形成太平洋保险汇总文件：%s...",sFileName);
    if(!(fp=fopen(sFileName,"wb")))
    {
       RollbackWork();
       return -1;
    }

    nCount=0;
    nId=OpenCursor("select cyhbz,cyhbz1,nbl1,dsfmx1,nlkrq,nsfm,cbl8,cjh,cczydh from dl$ywmx_%t "
       "where  dsjk>0 and nxzbz=1 and nlkrq between %ld and %ld ",
	           cJym,nQsrq,nJzrq);

    if(nId<=0)
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
             	                  
    //*na=nData/10000;
    //*nb=(nData%10000)/100;
    //*nc=(nData%10000)%100;
 
    }//end message
    CloseCursor(nId);
    fclose(fp);
    if(nFlag)
    {
       if(strstr(sSysJsdh,"2715")) 
       	RunSql("update dl$ywmx_%t set ncsbz=2 where nxzbz=1 and nlkrq<=%ld",cJym,nSysLkrq);
       else
       	RunSql("update dl$ywmx_%t set nxzbz=3 where nxzbz=0 and nlkrq<=%ld",cJym,nSysLkrq);
       
       CommitWork();
    }

    outtext("\n   形成文件[%s]成功\n   按任意键返回......",sFileName);
    keyb();    
    return 0;
}

/////////////////眉山邮政地税出库/////////////////////////////////
long PostToMsTerraFile(char * cJym,long nFlag)
{
    FILE *fp;
    long nId,nRetVal,nQsrq,nJzrq=nSysLkrq,nZh=2001;
    long nLkrq,nCscs=0,nCount,nKey,nXh,nSfm;
    char path[31],sFileName[81];
    char buf[200],sFph[20],sLsh[41],sBuffer[1024],sRq[30];
    char qybh[12],jjxz[5],jjxzm[30],lsybh[20],qydz[41],qymc[60],ksh[12],ssrq[16],szbh[4],sz[32],smbh[5],sm[32],jch[10],ysbh[10];
    long nId1,nRet1;
    double jsje,sl,sks,dSjk;
     
    clrscr();
    nQsrq=nSysLkrq-10;
    for(;;)
    {
       if(GetValue(4,2,"请输入汇总起始日期:%8ld",&nQsrq)==KEY_ESC) return;
       nKey=GetValue(4,3,"请输入汇总截止日期:%8ld",&nJzrq);
       if(nJzrq<nQsrq) continue;
       if(nKey==KEY_ESC) return;
       if(nKey==KEY_RETURN) break;
    }//end for

    strcpy(path,"sendfile/11");
    if(access(path,0))
    	mkdir(path,S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);

    sprintf(sFileName,"%s/DS%8ld.txt",path,nSysLkrq);
    
    printf("\n\n   正在形成地税汇总文件：%s...",sFileName);
    if(!(fp=fopen(sFileName,"wb")))
    {
       RollbackWork();
       return -1;
    }

    nCount=0;
    nId=OpenCursor("select distinct cyhbz,cbl2,cbl1,cbl4,cyhmc,cyhbz2,cbl5,cbl6,nlkrq,nsfm,cbl8,clsh from dl$ywmx_%t "
       "where  dsjk>0 and nxzbz=1 and nlkrq between %ld and %ld ",
	           cJym,nQsrq,nJzrq);

    if(nId<=0)
    {
    	fclose(fp);
    	CloseCursor(nId);
        return -1;
    }
    
    while(1)
    {
	sFph[0]='\0';
    	nRetVal=FetchCursor(nId,"%s%s%s%s%s,%s%s%s%ld%ld,%s%s",
    			qybh,jjxz,jjxzm,lsybh,qydz,
    			qymc,ksh,ssrq,&nLkrq,&nSfm,
    			sFph,sLsh);
    	if(nRetVal<=0) break;
    	nId1=OpenCursor("select  cbl7,cbl3,cbl9,cbl10,dsfmx2,dsfmx3,dsfmx5,cbl11 from dl$ywmx_%t "
       		"where  clsh=%s order by nbl1 ",cJym,sLsh);
       	if(nId1<0)
    	{
    	 fclose(fp);
         CloseCursor(nId1);
         return -1;
    	}
    	
	dSjk=0;
	nCount=0;
	sBuffer[0]='\0';
    	sRq[0]='\0';
    	while(1)
    	{
    	   buf[0]='\0';
    	   nRet1=FetchCursor(nId1,"%s%s%s%s%lf%lf%lf%s",
    	   		szbh,sz,smbh,sm,&jsje,&sl,&sks,jch);
    	   if(nRet1<=0) break;
    	//700300  szh='31'	101900  szh='10'	070100  szh='05' 	048500  szh='21' and smh='3000'
    	//048300  szh='21' and smh='4200' 030400  szh='02'
    	   ysbh[0]='\0';
    	   if(!strcmp(sz,"31")) strcpy(ysbh,"700300");
    	   else if(!strcmp(szbh,"10")) strcpy(ysbh,"101900");
    	   else if(!strcmp(szbh,"05")) strcpy(ysbh,"070109");
    	   else if(!strcmp(szbh,"02")) strcpy(ysbh,"030400");
    	   else if((!strcmp(szbh,"05"))&&(!strcmp(smbh,"3000"))) strcpy(ysbh,"048500");
    	   else if((!strcmp(szbh,"21"))&&(!strcmp(smbh,"4200"))) strcpy(ysbh,"048390");
    	   else ysbh[0]='0';
    	   sprintf(buf,"%s|%s|%s|%s|%.2lf|%.2lf|%.2lf|%s|%s|",szbh,sz,smbh,sm,jsje,sl,sks,jch,ysbh);
    	   dSjk+=sks;
	   nCount++;
    	   strcat(sBuffer,buf);
    	}//end while
    	CloseCursor(nId1);
	sprintf(sRq,"%ld %ld:%02ld:%02ld",nLkrq,nSfm/10000,(nSfm%10000)/100,(nSfm%10000)%100);
	
    	fprintf(fp,"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"
    		   "%s|%.2lf|62||%ld|%s\n",
    		qybh,jjxz,jjxzm,lsybh,qydz,qymc,ksh,ssrq,sRq,sRq,
    		sFph,dSjk,nCount,sBuffer);

    }
    CloseCursor(nId);
    fclose(fp);
    if(nFlag)
    {
       if(RunSql("update dl$ywmx_%t set nxzbz=3 where nxzbz=0 and "
               "nysyf<=%ld",cJym,(nQsrq%10000)/100)>=0)
       {
          CommitWork();
          sprintf(buf,"xtod %s > %s/MDSWSZ%ld%02ld.txt",
			sFileName,path,nSysLkrq/10000,(nSysLkrq%10000)/100);
          system(buf);
          outtext("\n   形成文件(%s/MDSWSZyyyymm.txt)成功\n   按任意键返回......",path);
        }
        else outtext("\n   形成文件出错，请与鸿志公司联系......");
    }
    else
    {
    	sprintf(buf,"xtod %s > %s/terraday.txt",sFileName,path);
    	system(buf);
        outtext("\n   形成文件[%s]成功\n   按任意键返回......",sFileName);
    }
    keyb();    
    return 0;
}
long PostToPowerFile(char * cJym,long nFlag)
{
    FILE *fp;
    long nId,nRetVal,nQsrq,nJzrq=nSysLkrq;
    long nLkrq,nCscs=0,nCount,nKey;
    char path[31],sFileName[81];
    char sYhmc[60],sYhbz[20],buf[200];
    double dSjk=0;
     
    clrscr();
    nQsrq=nSysLkrq;
    for(;;)
    {
       if(GetValue(4,2,"请输入汇总起始日期:%8ld",&nQsrq)==KEY_ESC) return;
       nKey=GetValue(4,3,"请输入汇总截止日期:%8ld",&nJzrq);
       if(nJzrq<nQsrq) continue;
       if(nKey==KEY_ESC) return;
       if(nKey==KEY_RETURN) break;
    }//end for

    strcpy(path,"sendfile/6");
    if(access(path,0))
    	mkdir(path,S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);

    sprintf(sFileName,"%s/DF%8ld.txt",path,nSysLkrq);
    
    printf("\n\n   正在形成电费未收用户汇总文件：%s...",sFileName);
    if(!(fp=fopen(sFileName,"wb")))
    {
       RollbackWork();
       return -1;
    }

    nCount=0;
    if(nFlag)
    {
    	nId=OpenCursor("select cyhmc,cyhbz,dsfmx1 from dl$ywmx_%t "
       		"where  (nxzbz=0 or nxzbz=3) and nlkrq between %ld and %ld ",
	           cJym,nQsrq,nJzrq);
    }
    else 
    {
    	nId=OpenCursor("select cyhmc,cyhbz,dsfmx1 from dl$ywmx_%t "
       		"where  nxzbz=1 and nlkrq between %ld and %ld ",
	           cJym,nQsrq,nJzrq);
    }//end select
    if(nId<=0)
    {
    	fclose(fp);
        RollbackWork();
        return -1;
    }
    
    while(1)
    {

    	nRetVal=FetchCursor(nId,"%s%s%lf",sYhmc,sYhbz,&dSjk);
    	if(nRetVal<=0) break;
    	
    	fprintf(fp,"%s	%s	%.2lf\n",sYhmc,sYhbz,dSjk);

    }//end message
    CloseCursor(nId);
    fclose(fp);
    
    sprintf(buf,"xtod %s > %s/power.txt",sFileName,path);
    system(buf);
    outtext("\n   形成DOS格式文件[%s/power.txt]成功\n   按任意键返回......",path);
    keyb();
    return 0;
}

long CdPostToTpyFile(char * cJym,long nFlag)
{
    FILE *fp;
    long nId,nRetVal,nQsrq,nJzrq=nSysLkrq;
    long nLkrq,nCscs=0,nCount,nKey;
    char path[31],sFileName[81];
    char buf[80],sYhbz[21],sYhbz1[21],sYhmc[61],sId[20],sName[20],sQq[12],sZq[12];
    long nBl1,nBl2;
    double dSjk=0;
     
    clrscr();
    nQsrq=nSysLkrq;
    for(;;)
    {
       if(GetValue(4,2,"请输入汇总起始日期:%8ld",&nQsrq)==KEY_ESC) return;
       nKey=GetValue(4,3,"请输入汇总截止日期:%8ld",&nJzrq);
       if(nJzrq<nQsrq) continue;
       if(nKey==KEY_ESC) return;
       if(nKey==KEY_RETURN) break;
    }//end for

    strcpy(path,"sendfile/9");
    if(access(path,0))
    	mkdir(path,S_IREAD|S_IRGRP|S_IWRITE|S_IWGRP|S_IEXEC|S_IXGRP);

    sprintf(sFileName,"%s/BX%8ld.txt",path,nSysLkrq);
    
    printf("\n\n   正在形成太平洋保险汇总文件：%s...",sFileName);
    if(!(fp=fopen(sFileName,"wb")))
    {
       RollbackWork();
       return -1;
    }

    nCount=0;
    nId=OpenCursor("select cyhbz,cyhbz1,cyhbz2,nyfkfs,cbl1,cyhmc,dsjk,nbl1,cbl3,cbl4 from dl$ywmx_%t "
       "where  dsjk>0 and nxzbz=1 and nlkrq between %ld and %ld ",
	           cJym,nQsrq,nJzrq);

    if(nId<=0)
    {
    	fclose(fp);
        RollbackWork();
        return -1;
    }
    
    while(1)
    {

    	nRetVal=FetchCursor(nId,"%s%s%s%ld%s%s%lf%ld%s%s",
    		sYhbz,sYhbz1,sYhmc,&nBl1,sId,sName,&dSjk,&nBl2,sQq,sZq);
    	if(nRetVal<=0) break;
    	
    	fprintf(fp,"%15s%6s%-60s%1ld%-18s%-12s%12.2lf%2ld%10s%10s\n",
    		sYhbz,sYhbz1,sYhmc,nBl1,sId,sName,dSjk,nBl2,sQq,sZq);
             	        
    }//end message
    CloseCursor(nId);
    fclose(fp);
    
    RunSql("update dl$ywmx_%t set nxzbz=3 where nxzbz=0 and nlkrq<=%ld",cJym,nSysLkrq);
    CommitWork();
    
    outtext("\n   形成文件[%s]成功\n   按任意键返回......",sFileName);
    keyb();    
    return 0;
}
