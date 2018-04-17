 /*    programmer:chenbo
 *    
 *    created: 2001.11.06
 *
 */

#include<sys/stat.h>
#include<dirent.h>
#include"dlpublic.h"
#include"query.h"
#include"tools.h"
#include"dl_wjrk.h"
#define  T  0x09  //tab��ֵ

extern long nSysLkrq,nSysSfm;
extern int errno;
extern char sSysJsdh[10];

void FileExchange()  //�ļ�������ͨѶ��ʹ��
{
  
    char sJym[16],sJymc[51];
    long nCxms;
    
    clrscr();
    
    if(SelectJym(sJym,4)<=0) return;
    RunSelect("select ncxms from dl$jym where cjym=%s into %ld",sJym,&nCxms);
    
    switch(nCxms)
    {
        case 110:
             UnicomSearchFile(sJym);
             break;
        case 610:
             GasSearchFile(sJym);
             break;
        case 521:
             TaxSearchFile(sJym);
             break;
	case 222:
	     if(strstr(sSysJsdh,"2715"))
	          MsTerraSearchFile(sJym);
	     else TerraSearchFile(sJym);
	     break;     
	case 333:
	     TpySearchFile(sJym);
	     break;
	case 125:
	     DfSearchFile(sJym);
	     break;
        default:
             MessageBox("�ý��ײ���Ҫ�ļ����...",1);
             break;
    }
    
}


void UnicomSearchFile(char * sJym)
{
    
    char path[51]="recvfile/1";
    char sFileName[81],buf[100];
    char * offset;
    DIR * pdir;
    struct dirent * pdirent;
    struct stat fstat;
    long nKey;
    
    
    
    CreateDir(path);
    CreateDir("olddata");
    
    
    clrscr();
    if(MessageBox("    ����:�µ������ļ����󽫸���ԭ�е���ʷ����,����ϵͳ"
    "����������֮ǰ����һ��ϵͳ����!,Ҫ������?",0)!=KEY_YES) return;
    if(!(pdir=opendir(path)))
    {
    	MessageBox("�޷���Ŀ¼:recvfile/1",1);
    	return;
    }
    while(pdirent=readdir(pdir))
    {
    	sprintf(sFileName,"%s/%s",path,pdirent->d_name);
        stat(sFileName, &fstat);

        if(S_ISREG(fstat.st_mode))
        {
            if(pdirent->d_name[0]!='L' && pdirent->d_name[0]!='I' ||
               !strstr(pdirent->d_name,".txt") || strlen(pdirent->d_name)<12) continue;
            if(offset=strstr(sFileName,".Z"))
            {
            	sprintf(buf,"uncompresss -f %s",sFileName);
            	system(buf);
            	offset[0]='\0';
            }
         
            outtext("\n�Ƿ��ļ�%s���(y/n)",sFileName);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') continue;
            UnicomFileImport(sJym,path,pdirent->d_name);
        }
        
    }//end while
    closedir(pdir);
}

void UnicomFileImport(char * sJym,char * sDirect,char * sFileName)
{

    FILE * fp;
    long nYwlb,nYsnd,nYsyf,nXh,nId=1,nKey;
    char sDump[81],sRealFileName[100],sData[2000],sLsh[41],sRealLsh[41];
    char sYhbz[41],sYhmc[81],sYhbz1[31];
    char * offset;
    double dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,
           dsfmx7,dsfmx8,dsfmx9,dsfmx10,dsfmx11,dsfmx12,
           dsfmx13,dsfmx14,dsfmx15,dsfmx16,dYjzk,dWjk,dBl1,dBl2,dTotalZk=0.0;
    
    sprintf(sRealFileName,"%s/%s",sDirect,sFileName);
    if(sFileName[0]=='L') nYwlb=0;
    else   nYwlb=2;
    
    
    GetSysLkrq();
    clrscr();
    if(nYwlb==0)
       FetchStringToArg(sFileName+2,"%4d%2d",&nYsnd,&nYsyf);
    else       
       FetchStringToArg(sFileName+5,"%4d%2d",&nYsnd,&nYsyf);
       
    outtext("\n���������ʷ����...");
    RunSql("delete dl$ywmx_%t where nywlb=%ld and "
    "((nysnd=%ld and nysyf<=%ld) or nysnd<%ld)",sJym,nYwlb,nYsnd,nYsyf,nYsnd);
    CommitWork();
    
    outtext("\n�������...");
    nXh=0;
    GenerateLsh(1,sJym,sSysJsdh,sLsh);
    
    if(!(fp=fopen(sRealFileName,"rt")))
    {
    	outtext("\n���ļ�����!");
    	keyb();
    	return;
    }
   
    for(;;)
    {
    	memset(sData,0,sizeof(sData));
    	if(getline(sData,fp)<=0) break;
    	if(strlen(sData)<80) continue;
    	
        offset=sData;
        while(offset[0])
        {
             if(offset[0]=='\\' || offset[0]=='\'') offset[0]=' ';
             offset++;
        }

        nXh++;
        sprintf(sRealLsh,"%-32.32s%08ld",sLsh,nXh);
        offset=FetchStringToArg(sData,
               "%s|%s|%s|%s|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%f|%s|",
               sDump,sYhbz,sYhbz1,sYhmc,&dsfmx1,&dsfmx2,&dsfmx3,&dsfmx4,&dsfmx5,&dsfmx6,
               &dsfmx7,&dsfmx8,&dsfmx9,&dsfmx10,&dsfmx11,&dsfmx12,&dsfmx13,&dWjk,&dYjzk,
               &dBl1,&dBl2,&dsfmx14,&dsfmx15,&dsfmx16,sDump);
        
        if(!offset)
        {
            clrscr();
            outtext("\n�ļ���ʽ����[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nId=-1;
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        
        alltrim(sYhmc);
        alltrim(sYhbz);
        

        nId=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,"
        "nywlb,nxh,dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,"
        "dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,dsfmx18,dsfmx19,dsfmx20,"
        "dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,njscz2,djscz3,djscz4,nsl,"
        "dyjzk,dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,"
        "cbl3,cbl4,cbl5,cbl6,cbl7,cjh,cczydh,nlkrq,nsfm,clxbz,nxzbz,ncsbz,cbz,cylsh,clsh) "
        "VALUES(%s,%ld,%ld,%s,%s,'.',%s,%ld,%ld,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,"
        "%lf,%lf,%lf,%lf,%lf,%lf,0,0,0,0,0,0,0,0,0,'.',%ld,0,0,1,%lf,0,%lf,%lf,0,0,0,0,0,"
        "%lf,%lf,'.','.','.','.','.','.','.',%s,'XTWH',%ld,%ld,'0000000000',0,0,'.',%s,%s)",
        sJym,sJym,nYsnd,nYsyf,sYhbz,sYhbz1,sYhmc,nYwlb,nXh,dsfmx1,dsfmx2,dsfmx3,dsfmx4,
        dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,
        dsfmx16,nSysLkrq,dYjzk,dYjzk,dWjk,dBl1,dBl2,sSysJsdh,nSysLkrq,nSysSfm,sRealLsh,sRealLsh);
        
        if(nId<0)
        {
            clrscr();
            GetSqlErrInfo(sData);
            outtext("\n������[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        
        dTotalZk+=dYjzk;
        if((nXh%1000)==0) CommitWork();    
        outtext("\n %8ld    %4ld    %2ld    %s    %.2lf",nXh,nYsnd,nYsyf,sYhbz,dYjzk);
    }
    CommitWork();
    fclose(fp);
    if(nId>0)
    {
        sprintf(sData,"olddata/%s",sFileName);
        rename(sRealFileName,sData);        
    	outtext("\n�ļ�������,����%ld��,�ܽ��:%.2lf",nXh,dTotalZk);
    	keyb();
    }	
}

void GasSearchFile(char * sJym)
{
    
    char path[51]="recvfile/5";
    char sFileName[81],buf[100];
    char * offset;
    DIR * pdir;
    struct dirent * pdirent;
    struct stat fstat;
    long nKey;
    
    
    
    CreateDir(path);
    CreateDir("olddata");
    
    
    clrscr();
    if(MessageBox("    ����:�µ������ļ����󽫸���ԭ�е���ʷ����,����ϵͳ"
    "����������֮ǰ����һ��ϵͳ����!,Ҫ������?",0)!=KEY_YES) return;
    if(!(pdir=opendir(path)))
    {
    	MessageBox("�޷���Ŀ¼:recvfile/5",1);
    	return;
    }
    while(pdirent=readdir(pdir))
    {
    	sprintf(sFileName,"%s/%s",path,pdirent->d_name);
        stat(sFileName, &fstat);
		
        if(S_ISREG(fstat.st_mode))
        {
        	if(!strstr(pdirent->d_name,"gz"))
            	continue;
            if(offset=strstr(sFileName,".Z"))
            {
            	sprintf(buf,"uncompresss -f %s",sFileName);
            	system(buf);
            	offset[0]='\0';
            }
         	
         	outtext("\n�Ƿ��ļ�%s���(y/n)",sFileName);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') continue;
            GasFileImport(sJym,path,pdirent->d_name);
        }
        
    }//end while
    closedir(pdir);
}

void GasFileImport(char * sJym,char * sDirect,char * sFileName)
{

    FILE * fp;
    long nYwlb,nYsnd,nYsyf,nXh,nId=1,nKey,nJscz2;
    char sDump[81],sRealFileName[100],sData[2000],sLsh[41],sRealLsh[41];
    char sYhbz[41],sYhmc[81],sYhbz1[31];
    char * offset;
    double dSfmx[6],dSfmx7,dPrice,dTotal,dSum,dTotalZk=0.0,dYjzk;
    
    sprintf(sRealFileName,"%s/%s",sDirect,sFileName);
    
    GetSysLkrq();
    clrscr();
       
    outtext("\n���������ʷ����...");
    RunSql("delete dl$ywmx_%t where nlkrq<=%ld",sJym,nSysLkrq-10000);
    
    outtext("\n�������...");
    nXh=0;
    GenerateLsh(1,sJym,sSysJsdh,sLsh);
    
    if(!(fp=fopen(sRealFileName,"rt")))
    {
    	outtext("\n���ļ�����!");
    	keyb();
    	return;
    }
   
    for(;;)
    {
    	memset(sData,0,sizeof(sData));
    	if(getline(sData,fp)<=0) break;
    	if(strlen(sData)<60) continue;
    	alltrim(sData);
    	
        offset=sData;
        nXh++;
        sprintf(sRealLsh,"%-32.32s%08ld",sLsh,nXh);
        
        offset=FetchStringToArg(sData+1,
               "%d|%d|%s|%s|%s|%d|%s|%f|%f|%f|%f|%f|%d|",   //��������ֶ�ʡ��
               &nYsnd,&nYsyf,sYhbz,sYhbz1,sYhmc,&nYwlb,sDump,
               &dPrice,&dTotal,&dSfmx7,&dSum,&dYjzk,&nJscz2);
                
        memset(dSfmx,0,sizeof(dSfmx));
        dSfmx[(nYwlb==1)*3]=dTotal;
        dSfmx[(nYwlb==1)*3+1]=dPrice;
        dSfmx[(nYwlb==1)*3+2]=dSum;

        if(!offset)
        {
            clrscr();
            outtext("\n�ļ���ʽ����[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nId=-1;
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        
        alltrim(sYhmc);
        alltrim(sYhbz);
        

        nId=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,"
        "nywlb,nxh,dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,"
        "dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,dsfmx18,dsfmx19,dsfmx20,"
        "dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,njscz2,djscz3,djscz4,nsl,"
        "dyjzk,dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,"
        "cbl3,cbl4,cbl5,cbl6,cbl7,cjh,cczydh,nlkrq,nsfm,clxbz,nxzbz,ncsbz,cbz,cylsh,clsh) "
        "VALUES(%s,%ld,%ld,%s,%s,'.',%s,%ld,%ld,%lf,%lf,%lf,%lf,%lf,%lf,0,0,0,0,"
        "0,0,0,0,0,0,0,0,0,0,0,0,0,0,%lf,'.',%ld,0,0,1,%lf,0,%lf,0,0,0,0,0,0,"
        "0,0,'.','.','.','.','.','.','.','0','XTWH',%ld,%ld,'0000000000',0,0,'.',%s,'0')",
        sJym,sJym,nYsnd,nYsyf,sYhbz,sYhbz1,sYhmc,0,nXh,dSfmx[0],dSfmx[1],dSfmx[2],dSfmx[3],
        dSfmx[4],dSfmx[5],dSfmx7,nJscz2,dYjzk,dYjzk,nSysLkrq,nSysSfm,sRealLsh);
        
        if(nId<0)
        {
            clrscr();
            GetSqlErrInfo(sData);
            outtext("\n������[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        
        dTotalZk+=dYjzk;
        if((nXh%1000)==0) CommitWork();    
        outtext("\n %8ld    %4ld    %2ld    %s    %.2lf",nXh,nYsnd,nYsyf,sYhbz,dYjzk);
    }
    CommitWork();
    fclose(fp);
    if(nId>0)
    {
        sprintf(sData,"olddata/%s",sFileName);
        rename(sRealFileName,sData);        
    	outtext("\n�ļ�������,����%ld��,�ܽ��:%.2lf",nXh,dTotalZk);
    	keyb();
    }	
}

//////////////////////////////��˰/////////////////////////////
void TaxSearchFile(char * sJym)
{
    
    char path[51]="recvfile/10";
    char sFileName[81],buf[100];
    char * offset;
    DIR * pdir;
    struct dirent * pdirent;
    struct stat fstat;
    long nKey;
    
    
    
    CreateDir(path);
    CreateDir("olddata");
    
    
    clrscr();
    if(MessageBox("    ����:�µ������ļ����󽫸���ԭ�е���ʷ����,����ϵͳ"
    "����������֮ǰ����һ��ϵͳ����!,Ҫ������?",0)!=KEY_YES) return;
    if(!(pdir=opendir(path)))
    {
    	MessageBox("�޷���Ŀ¼:recvfile/10",1);
    	return;
    }
    while(pdirent=readdir(pdir))
    {
    	sprintf(sFileName,"%s/%s",path,pdirent->d_name);
        stat(sFileName, &fstat);
		
        if(S_ISREG(fstat.st_mode))
        {
            if(!strstr(pdirent->d_name,"out"))
            	continue;
            if(offset=strstr(sFileName,".Z"))
            {
            	sprintf(buf,"uncompresss -f %s",sFileName);
            	system(buf);
            	offset[0]='\0';
            }
         	
         	outtext("\n�Ƿ��ļ�%s���(y/n)",sFileName);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') continue;
            TaxFileImport(sJym,path,pdirent->d_name);
        }
        
    }//end while
}

void TaxFileImport(char * sJym,char * sDirect,char * sFileName)
{
    FILE * fp;
    long nYwlb,nYsnd,nYsyf,nXh,nId=1,nKey,nKm,nXjrq,nSkrq;
    char sDump[81],sRealFileName[100],sData[400],sLsh[41],sRealLsh[41],sKssl[12];
    char sZsjg[41],sDylx[3],sZspl[3],sHdbz[2],sSwjg[21],sDsgx[21];
    char sZclx[21],sDjh[20],sQydz[50],sJz[2],sSzmc[10],sPmgs[3],sPmmc[20];
    char sKmbm[8]=".",sKmmc[30]=".",sJkgk[20]=".";
    double dKssl=0,dXssr,dSl,dDqje,dSjje;
    double dTotalZk=0;
    char sYhbz[41],sYhmc[81]=".",sYhbz1[61]=".";
    char * offset;
   
    sprintf(sRealFileName,"%s/%s",sDirect,sFileName);
    
    GetSysLkrq();
    clrscr();
       
    outtext("\n���������ʷ����...");
    RunSql("delete dl$ywmx_%t where nlkrq<=%ld",sJym,nSysLkrq-600);
    
    outtext("\n�������...");
    nXh=0;
    GenerateLsh(1,sJym,sSysJsdh,sLsh);
    
    if(!(fp=fopen(sRealFileName,"rt")))
    {
    	outtext("\n���ļ�����!");
    	keyb();
    	return;
    }
   
    for(;;)
    {
    	memset(sData,0,sizeof(sData));
    	if(getline(sData,fp)<=0) break;
    	alltrim(sData);
        offset=sData;
        nXh++;
        sprintf(sRealLsh,"%-32.32s%08ld",sLsh,nXh);
/*|9999|31578400010|
  02|11|5320984|1|�ɶ��й�˰���־�|����������ҵ|������ҵ|��˰����|510106901957504|�ɶ���·���̼���װ��ʵҵ�������ι�˾��ó|
  �ɶ��л𳵱�վ��һ��|2003|02|||11||20030315|20030315|1|
  ��ֵ˰|1|��ҵ||0.00|100|0.00|0.00||*/
  
        offset=FetchStringToArg(sData+6,"%s|"
                "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|"
                "%s|%d|%d|%s|%s|%d|%s|%d|%d|%s|"
                "%s|%s|%s|%s|%f|%f|%f|%f|%s|",
                sYhbz1,
                sDylx,sZspl,sYhbz,sHdbz,sSwjg,sDsgx,sZclx,sZsjg,sDjh,sYhmc,
                sQydz,&nYsnd,&nYsyf,sKmbm,sKmmc,&nKm,sJkgk,&nXjrq,&nSkrq,sJz,
                sSzmc,sPmgs,sPmmc,sKssl,&dXssr,&dSl,&dDqje,&dSjje,sDump);
       
        /*printf("[%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%s|%s|%d|%s|%d|%d|%s|%s|%s|%s|%f|%f|%f|%f|%f|%s|\n",
                sDylx,sZspl,sYhbz,sHdbz,sSwjg,sDsgx,sZclx,sZsjg,sDjh,sYhmc,
                sQydz,nYsnd,nYsyf,sKmbm,sKmmc,nKm,sJkgk,nXjrq,nSkrq,sJz,
                sSzmc,sPmgs,sPmmc,dKssl,dXssr,dSl,dDqje,dSjje,sDump);*/

        if(!offset)
        {
            clrscr();
            outtext("\n�ļ���ʽ����[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nId=-1;
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        
        
        if(strlen(sKmbm)<1) strcpy(sKmbm,".");
        if(strlen(sKmmc)<1) strcpy(sKmmc,".");
        if(sJkgk[0]=='\0') strcpy(sJkgk,".");
        if(sDsgx[0]=='\0') strcpy(sDsgx,".");
        if(sZsjg[0]=='\0') strcpy(sZsjg,".");
        if(sSzmc[0]=='\0') strcpy(sSzmc,".");
        if(sDjh[0]=='\0') strcpy(sDjh,".");
        dKssl=atol(sKssl);
        
        nId=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,"
        "nywlb,nxh,dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,"
        "dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,dsfmx18,dsfmx19,dsfmx20,"
        "dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,njscz2,djscz3,djscz4,nsl,"
        "dyjzk,dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,"
        "cbl3,cbl4,cbl5,cbl6,cbl7,cbl8,cbl9,cbl10,cbl11,cbl12,cbl13,cbl14,"
        "cbl15,cbl16,cbl17,cbl18,"
        "cjh,cczydh,nlkrq,nsfm,clxbz,nxzbz,ncsbz,cbz,cylsh,clsh) "
        "VALUES(%s,%ld,%ld,%s,%s,%s,%s,%ld,%ld,%lf,%lf,%lf,%lf,%lf,0,0,0,0,0,"
        "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'.',%ld,0,0,1,%lf,0,0,%lf,0,0,%ld,%ld,%ld,"
        "0,0,%s,%s,%s,%s,%s,'.',%s,'.',%s,%s,%s,%s,%s,%s,"
        "%s,'.','.','.',"
        "'0','XTWH',%ld,%ld,'0000000000',0,0,'.',%s,'0')",
        sJym,sJym,nYsnd,nYsyf,sYhbz,sYhbz1,sDylx,sYhmc,0,nXh,dKssl,dXssr,dSl,dDqje,dSjje,
        nSysLkrq,dSjje,dSjje,nKm,nXjrq,nSkrq,
        sZspl,sZclx,sDjh,sJkgk,sSzmc,sHdbz,sKmmc,sKmbm,sZsjg,sPmgs,sPmmc,sSwjg,sQydz,
        nSysLkrq,nSysSfm,sRealLsh);
        
        if(nId<0)
        {
            clrscr();
            GetSqlErrInfo(sData);
            outtext("\n������[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        
        dTotalZk+=dSjje;
        if((nXh%1000)==0) CommitWork();    
        outtext("\n %8ld    %4ld    %2ld    %s    %.2lf",nXh,nYsnd,nYsyf,sYhbz,dSjje);
    }
    RunSql("update dl$jymcsfb set ccs=to_char(%ld) \
           where cjym=%s and nbh=1",nXjrq,sJym);
    CommitWork();
    fclose(fp);
    if(nId>0)
    {
        sprintf(sData,"olddata/%s",sFileName);
        rename(sRealFileName,sData);        
    	outtext("\n�ļ�������,����%ld��,�ܽ��:%.2lf",nXh,dTotalZk);
    	keyb();
    }	
}
/////////////////��˰/////////////////////////////////
void TerraSearchFile(char * sJym)
{
    
    char path[51]="recvfile/11";
    char sFileName[81],buf[100];
    char * offset;
    DIR * pdir;
    struct dirent * pdirent;
    struct stat fstat;
    long nKey;
    
    
    
    CreateDir(path);
    CreateDir("olddata");
    
    
    clrscr();
    if(MessageBox("    ����:�µ������ļ����󽫸���ԭ�е���ʷ����,����ϵͳ"
    "����������֮ǰ����һ��ϵͳ����!,Ҫ������?",0)!=KEY_YES) return;
    if(!(pdir=opendir(path)))
    {
    	MessageBox("�޷���Ŀ¼:recvfile/11",1);
    	return;
    }
    while(pdirent=readdir(pdir))
    {
    	sprintf(sFileName,"%s/%s",path,pdirent->d_name);
        stat(sFileName, &fstat);
		
        if(S_ISREG(fstat.st_mode))
        {
            if(!strstr(pdirent->d_name,"txt"))
            	continue;
            if(offset=strstr(sFileName,".Z"))
            {
            	sprintf(buf,"uncompresss -f %s",sFileName);
            	system(buf);
            	offset[0]='\0';
            }
         	
         	outtext("\n�Ƿ��ļ�%s���(y/n)",sFileName);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') continue;
            TerraFileImport(sJym,path,pdirent->d_name);
        }
        
    }//end while
}

void TerraFileImport(char * sJym,char * sDirect,char * sFileName)
{

    FILE * fp;
    char sDump[81],sRealFileName[100],sData[400],sLsh[41],sRealLsh[41];
    long nId,nXh,nKey,nYsnd,nYsyf;
    char qybm[21],qymc[31],qydz[61],zclx[21],zsjg[25],
    	sksj[13],sz[21],sm[27],swjg[13],spbz[9];
    double kssl,jsyj,sl,yjje,sjje,dTotalZk;
    char * offset,temp[10];
   
    sprintf(sRealFileName,"%s/%s",sDirect,sFileName);
    
    GetSysLkrq();
    clrscr();
       
    outtext("\n���������ʷ����...");
    RunSql("delete dl$ywmx_%t where nlkrq<=%ld",sJym,nSysLkrq-600);
    
    outtext("\n�������...");
    nXh=0;
    GenerateLsh(1,sJym,sSysJsdh,sLsh);
    
    if(!(fp=fopen(sRealFileName,"rt")))
    {
    	outtext("\n���ļ�����!");
    	keyb();
    	return;
    }
   
    for(;;)
    {
    	memset(sData,0,sizeof(sData));
    	if(getline(sData,fp)<=0) break;
    	alltrim(sData);
        offset=sData;
        nXh++;
        sprintf(sRealLsh,"%-32.32s%08ld",sLsh,nXh);
  
    	/*qybm[20](yhbz)qymc[30](yhbz2)qydz[60](yhmc)zclx[20](1)zsjg[24](9)
    	sksj[12](2)sz[20](3)sm[26](10)swjg[12](4)spbz[8](5)
    	kssl(14.2)jsyj(14.2)sl(14.2)yjje(14.2)sjje(14.2)*/
        
        offset=FetchStringToArg(sData,
        	"%20s%30s%60s%20s%24s%12s%20s%26s%12s%8s"
        	"%14f%14f%14f%14f%14f",
            qybm,qymc,qydz,zclx,zsjg,sksj,sz,sm,swjg,spbz,
            &kssl,&jsyj,&sl,&yjje,&sjje);
       
		//printf("\nTEST:%s|%s|%s|%s|%s|%s|%s|%s|%lf|%lf|%lf|%lf|%lf|%s|%s|",
		//qybm,qymc,qydz,zclx,zsjg,sksj,sz,sm,kssl,jsyj,sl,yjje,sjje,swjg,spbz);

        if(!offset)
        {
            clrscr();
            outtext("\n�ļ���ʽ����[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nId=-1;
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        
        FetchStringToArg(sksj,"%4d%2s%2d",&nYsnd,&temp,&nYsyf);
		alltrim(qybm),alltrim(qydz),alltrim(qymc),alltrim(zclx),alltrim(zsjg),alltrim(sksj),alltrim(sz),alltrim(sm),alltrim(swjg),alltrim(spbz);
        
        nId=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,"
        "nywlb,nxh,dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,"
        "dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,dsfmx18,dsfmx19,dsfmx20,"
        "dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,njscz2,djscz3,djscz4,nsl,"
        "dyjzk,dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,"
        "cbl3,cbl4,cbl5,cbl6,cbl7,cbl8,cbl9,cbl10,cbl11,cbl12,cbl13,cbl14,"
        "cbl15,cbl16,cbl17,cbl18,"
        "cjh,cczydh,nlkrq,nsfm,clxbz,nxzbz,ncsbz,cbz,cylsh,clsh) "
        "VALUES(%s,%ld,%ld,%s,%s,%s,%s,%ld,%ld,%lf,%lf,%lf,%lf,%lf,0,0,0,0,0,"
        "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'.',%ld,0,0,1,%lf,0,0,%lf,0,0,0,0,0,"
        "0,0,%s,%s,%s,%s,%s,'.','.','.',%s,%s,'.','.','.','.','.','.','.','.',"
        "'0','XTWH',%ld,%ld,'0000000000',0,0,'.',%s,'0')",
        sJym,sJym,nYsnd,nYsyf,qybm,qybm,qymc,qydz,0,nXh,kssl,jsyj,sl,yjje,sjje,
        nSysLkrq,sjje,sjje,
		zclx,sksj,sz,swjg,spbz,zsjg,sm,
        nSysLkrq,nSysSfm,sRealLsh);
        
        if(nId<0)
        {
            clrscr();
            GetSqlErrInfo(sData);
            outtext("\n������[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }

        dTotalZk+=sjje;
        if((nXh%1000)==0) CommitWork();    
        outtext("\n %8ld    %4ld    %2ld    %s    %.2lf",nXh,nYsnd,nYsyf,qybm,sjje);
    }

    CommitWork();
    fclose(fp);
    if(nId>0)
    {
        sprintf(sData,"olddata/%s",sFileName);
        rename(sRealFileName,sData);        
    	outtext("\n�ļ�������,����%ld��,�ܽ��:%.2lf",nXh,dTotalZk);
    	keyb();
    }	
}

//////////////////////////////////////////////////////////////////
void TpySearchFile(char * sJym)
{
    
    char path[51]="recvfile/9";
    char sFileName[81],buf[100];
    char * offset;
    DIR * pdir;
    struct dirent * pdirent;
    struct stat fstat;
    long nKey;
    
    
    
    CreateDir(path);
    CreateDir("olddata");
    
    
    clrscr();
    if(MessageBox("    ����:�µ������ļ����󽫸���ԭ�е���ʷ����,����ϵͳ"
    "����������֮ǰ����һ��ϵͳ����!,Ҫ������?",0)!=KEY_YES) return;
    if(!(pdir=opendir(path)))
    {
    	MessageBox("�޷���Ŀ¼:recvfile/9",1);
    	return;
    }
    while(pdirent=readdir(pdir))
    {
    	sprintf(sFileName,"%s/%s",path,pdirent->d_name);
        stat(sFileName, &fstat);
		
        if(S_ISREG(fstat.st_mode))
        {
            if(!strstr(pdirent->d_name,"txt"))
            	continue;
            if(offset=strstr(sFileName,".Z"))
            {
            	sprintf(buf,"uncompresss -f %s",sFileName);
            	system(buf);
            	offset[0]='\0';
            }
         	
         	outtext("\n�Ƿ��ļ�%s���(y/n)",sFileName);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') continue;
     	
     	    if(strstr(sSysJsdh,"2701"))
	    	CdTpyFileImport(sJym,path,pdirent->d_name);
	    else  TpyFileImport(sJym,path,pdirent->d_name);
        }
        
    }//end while
}

void TpyFileImport(char * sJym,char * sDirect,char * sFileName)
{

    FILE * fp;
    char sDump[12],sRealFileName[100],sData[400],sLsh[41],sRealLsh[41];
    long nId,nXh,nKey,nYsnd,nYsyf,nDay;
    char sYhbz[20],sYhbz1[22],sYhmc[41],sId[20],sName[20],sYjrq[20],sQq[20],sZq[20],sRq[20],sFsmc[10],sZt[2],sBt[11],sYw[9],sYwy[11],sSf[9],sSfy[11];
    double dTotalZk=0,dSjk;
    long nBz,nQc,nNx,nFsm;
    char * offset,temp[12];
   
    sprintf(sRealFileName,"%s/%s",sDirect,sFileName);
    
    GetSysLkrq();
    clrscr();
       
    outtext("\n�������ʧЧ����...");
    if(RunSql("delete dl$ywmx_%t where nxzbz=3",sJym)>0) CommitWork();
    
    outtext("\n�������...");
    nXh=0;
    GenerateLsh(1,sJym,sSysJsdh,sLsh);
    
    if(!(fp=fopen(sRealFileName,"rt")))
    {
    	outtext("\n���ļ�����!");
    	keyb();
    	return;
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
            clrscr();
            outtext("\n�ļ���ʽ����[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nId=-1;
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        
 	/*printf("\nTEST:%s|%s|%s|%ld|%s|%s|%lf|%ld|%ld|%s|"
    			"%s|%s|%s|%ld|%s|%s|%s|"
    			"%s|%s|%s|%s|",
    			sYhbz,sYhbz1,sYhmc,nBz,sId,sName,dSjk,nQc,nNx,sYjrq,
        		sQq,sZq,sRq,nFsm,sFsmc,sZt,sBt,sYw,sYwy,sSf,sSfy);
        keyb();*/
	if(RunSelect("select ROWNUM from dl$ywmx_%t where cyhbz=%s and cyhbz1=%s and \
			nbl1=%ld and cbl7='2' and nxzbz<>1",sJym,sYhbz,sYhbz1,nQc)>0)
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
            clrscr();
            GetSqlErrInfo(sData);
            outtext("\n������[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
	
        dTotalZk+=dSjk;
        if((nXh%1000)==0) CommitWork();    
        outtext("\n %8ld    %4ld    %2ld    %s    %.2lf",nXh,nYsnd,nYsyf,sYhbz,dSjk);
    }

    CommitWork();
    fclose(fp);
    if(nId>0)
    {
        sprintf(sData,"olddata/%s",sFileName);
        rename(sRealFileName,sData);        
    	outtext("\n�ļ�������,����%ld��,�ܽ��:%.2lf",nXh,dTotalZk);
    	keyb();
    }	
}

//üɽ��˰������
void MsTerraSearchFile(char * sJym)
{
    
    char path[51]="recvfile/11";
    char sFileName[81],buf[100];
    char * offset;
    DIR * pdir;
    struct dirent * pdirent;
    struct stat fstat;
    long nKey;
    
    
    
    CreateDir(path);
    CreateDir("olddata");
    
    
    clrscr();
    if(MessageBox("    ����:�µ������ļ����󽫸���ԭ�е���ʷ����,����ϵͳ"
    "����������֮ǰ����һ��ϵͳ����!,Ҫ������?",0)!=KEY_YES) return;
    if(!(pdir=opendir(path)))
    {
    	MessageBox("�޷���Ŀ¼:recvfile/11",1);
    	return;
    }
    while(pdirent=readdir(pdir))
    {
    	sprintf(sFileName,"%s/%s",path,pdirent->d_name);
        stat(sFileName, &fstat);
		
        if(S_ISREG(fstat.st_mode))
        {
            if(!strstr(pdirent->d_name,"txt"))
            	continue;
            if(offset=strstr(sFileName,".Z"))
            {
            	sprintf(buf,"uncompresss -f %s",sFileName);
            	system(buf);
            	offset[0]='\0';
            }
         	
         	outtext("\n�Ƿ��ļ�%s���(y/n)",sFileName);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') continue;
            MsTerraFileImport(sJym,path,pdirent->d_name);
        }
        
    }//end while
}

void MsTerraFileImport(char * sJym,char * sDirect,char * sFileName)
{

    FILE * fp;
    char sDump[81],sRealFileName[100],sData[800],sLsh[41],sRealLsh[41];
    long nId,nXh,nKey,nYsnd,nYsyf,nCount,i,nJls=0;
    char qybh[12],jjxz[5],jjxzm[41],lsybh[20],qydz[60],qymc[80],ksh[12],ssrq[16],szbh[4],sz[32],smbh[5],sm[32],jch[10];
    double jsje,sl,sks,dTotalZk;
    char * offset;
   
    sprintf(sRealFileName,"%s/%s",sDirect,sFileName);
    
    GetSysLkrq();
    clrscr();
       
    outtext("\n���������ʷ����...");
    RunSql("delete dl$ywmx_%t where nlkrq<=%ld",sJym,nSysLkrq-600);
    
    outtext("\n�������...");
    nXh=0;
    GenerateLsh(1,sJym,sSysJsdh,sLsh);
    
    if(!(fp=fopen(sRealFileName,"rt")))
    {
    	outtext("\n���ļ�����!");
    	keyb();
    	return;
    }
   
    for(;;)
    {
    	memset(sData,0,sizeof(sData));
    	if(getline(sData,fp)<=0) break;
    	alltrim(sData);
        offset=sData;
          
    	/*��ҵ���|��������|~~|��˰��ʶ���|��ҵ��ַ|��ҵ����|
    	 ���Һ�|˰����������|N|˰�ֱ��1|˰��1|˰Ŀ���1| ˰Ŀ1|��˰��1|˰��1|˰����1|*/
    	 
    	nCount=0;
        offset=FetchStringToArg(sData,
        	"%s|%s|%s|%s|%s|%s|%s|%s|%d|",
            	qybh,jjxz,jjxzm,lsybh,qydz,qymc,ksh,ssrq,&nCount);
       
        if(!offset)
        {
            clrscr();
            outtext("\n�ļ���ʽ����[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nId=-1;
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        
        /*printf("\nTEST:%s|%s|%s|%s|%s|%s|%s|%s|%ld|\n",qybh,jjxz,jjxzm,lsybh,qydz,qymc,ksh,ssrq,nCount); */
        nYsnd=nSysLkrq/10000;
        nYsyf=(nSysLkrq%10000)/100;
	alltrim(qydz);
	alltrim(qymc);
/*	if(RunSelect("select ROWNUM from dl$ywmx_%t where cyhbz=%s and nysyf=%ld and nxzbz=0 ",
			sJym,qybh,nYsyf)>0)
	{
		if(RunSql("delete dl$ywmx_%t where cyhbz=%s and nysyf=%ld and nxzbz=0",sJym,qybh,nYsyf)>0) CommitWork();
	}//new___
*/
	nJls++;
        for(i=1;i<=nCount;i++)
        {
        	nXh++;
        	sprintf(sRealLsh,"%-32.32s%08ld",sLsh,nXh);
        	
        	offset=FetchStringToArg(offset,
        		"%s|%s|%s|%s|%f|%f|%f|%s|",
        		szbh,sz,smbh,sm,&jsje,&sl,&sks,jch);
        	        	   	
		if(smbh[0]=='\0') smbh[0]='0';

	        nId=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,"
	        "nywlb,nxh,dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,"
	        "dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,dsfmx18,dsfmx19,dsfmx20,"
	        "dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,njscz2,djscz3,djscz4,nsl,"
	        "dyjzk,dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,"
	        "cbl3,cbl4,cbl5,cbl6,cbl7,cbl8,cbl9,cbl10,cbl11,cbl12,cbl13,cbl14,"
	        "cbl15,cbl16,cbl17,cbl18,"
	        "cjh,cczydh,nlkrq,nsfm,clxbz,nxzbz,ncsbz,cbz,cylsh,clsh) "
	        "VALUES(%s,%ld,%ld,%s,%s,%s,%s,%ld,%ld,0,%lf,%lf,0,%lf,0,0,0,0,0,"
	        "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'.',%ld,0,0,1,%lf,0,0,%lf,0,0,0,%ld,0,"
	        "0,0,%s,%s,%s,%s,%s,%s,%s,'.',%s,%s,%s,'0','0','0','0','0','0','0',"
	        "'0','XTWH',%ld,%ld,'0000000000',0,0,'.',%s,'0')",
	        sJym,sJym,nYsnd,nYsyf,qybh,qybh,qymc,qydz,0,nXh,jsje,sl,sks,
	        nSysLkrq,sks,sks,i,
            	jjxzm,jjxz,sz,lsybh,ksh,ssrq,szbh,smbh,sm,jch,
	        nSysLkrq,nSysSfm,sRealLsh);
	        
	        if(nId<0)
	        {
	            clrscr();
	            GetSqlErrInfo(sData);
	            outtext("\n������[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
	            nKey=keyb();
	            if(nKey!='Y' && nKey!='y') break;
	 	    return;	
	        }
        	dTotalZk+=sks;
                outtext("\n[%ld] %8ld    %4ld    %2ld    %s    %.2lf",nJls,nXh,nYsnd,nYsyf,qybh,sks);
	}//end for
	if(nKey<0) break;
        if((nXh%1000)==0) CommitWork();    
    }

    CommitWork();
    fclose(fp);
    if(nId>0)
    {
        sprintf(sData,"olddata/%s",sFileName);
        rename(sRealFileName,sData);        
    	outtext("\n�ļ�������,����%ld��,�ܽ��:%.2lf",nXh,dTotalZk);
    	keyb();
    }	
}
//���
void DfSearchFile(char * sJym)
{
    
    char path[51]="recvfile/6";
    char sFileName[81],buf[100];
    char * offset;
    DIR * pdir;
    struct dirent * pdirent;
    struct stat fstat;
    long nKey;
    
    CreateDir(path);
    CreateDir("olddata");
    
    
    clrscr();
    if(MessageBox("    ����:�µ������ļ����󽫸���ԭ�е���ʷ����,����ϵͳ"
    "����������֮ǰ����һ��ϵͳ����!,Ҫ������?",0)!=KEY_YES) return;
    if(!(pdir=opendir(path)))
    {
    	MessageBox("�޷���Ŀ¼:recvfile/6",1);
    	return;
    }
    while(pdirent=readdir(pdir))
    {
    	sprintf(sFileName,"%s/%s",path,pdirent->d_name);
        stat(sFileName, &fstat);
		
        if(S_ISREG(fstat.st_mode))
        {
            if(!strstr(pdirent->d_name,"txt"))
            	continue;
            if(offset=strstr(sFileName,".Z"))
            {
            	sprintf(buf,"uncompresss -f %s",sFileName);
            	system(buf);
            	offset[0]='\0';
            }
         	
         	outtext("\n�Ƿ��ļ�%s���(y/n)",sFileName);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') continue;
            DfFileImport(sJym,path,pdirent->d_name);
        }
        
    }//end while
}

void DfFileImport(char * sJym,char * sDirect,char * sFileName)
{

    FILE * fp;
    char sDump[81],sRealFileName[100],sData[800],sLsh[41],sRealLsh[41];
    char sYhbz[21],sYhmc[60],sYdlb[8],sGdsm[20],sQsrq[12],sJzrq[12],temp[6]=".";
    double dQs,dZs,dBl,dCjbl,dSjk=0,dTotalZk=0;
    long nKey,nId,nXh,nCount,nYsnd,nYsyf,nDay;
    char * offset;
   
    sprintf(sRealFileName,"%s/%s",sDirect,sFileName);
    
    GetSysLkrq();
    clrscr();
       
    //outtext("\n���������ʷ����...");
    if(RunSql("update dl$ywmx_%t set nxzbz=3 where nxzbz=0",sJym)>0) CommitWork();
    
    outtext("\n�������...");
    nXh=0;
    GenerateLsh(1,sJym,sSysJsdh,sLsh);
    
    if(!(fp=fopen(sRealFileName,"rt")))
    {
    	outtext("\n���ļ�����!");
    	keyb();
    	return;
    }
   
    for(;;)
    {
    	memset(sData,0,sizeof(sData));
    	if(getline(sData,fp)<=0) break;
    	alltrim(sData);
        offset=sData;
          
    	//50090361	���Ⱥ�	����[0.001]	�����	2004-07-09	992.00	1065.00	1	73.00	47.45	2004-08-01
    	//��־16 �û���40 �õ����4 ��������10 ʱ�� ���� ֹ�� ���� �������� ��� ����
    	nCount=0;
	//printf("TEST:[%s]",sData);
        offset=FetchStringToArg(sData,
        	"%s	%s	%s	%s	%s	%f	"
        	"%f	%f	%f	%f	%10s",
            	sYhbz,sYhmc,sYdlb,sGdsm,sQsrq,&dQs,&dZs,&dBl,&dCjbl,&dSjk,sJzrq);
       
        if(!offset)
        {
            clrscr();
            outtext("\n�ļ���ʽ����[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nId=-1;
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
	//printf("%s|%s|%s|%s|%s|%.2lf|%.2lf|%.2lf|%.2lf|%.2lf|%s|",sYhbz,sYhmc,sYdlb,sGdsm,sQsrq,dQs,dZs,dBl,dCjbl,dSjk,sJzrq);
        alltrim(sQsrq);
        FetchStringToArg(sQsrq,"%4d%1s%2d%1s%2d",&nYsnd,temp,&nYsyf,temp,&nDay);

        if(sQsrq[0]=='\0') strcpy(sQsrq,".");
        sJzrq[10]='\0';
	nXh++;
        sprintf(sRealLsh,"%-32.32s%08ld",sLsh,nXh);
        
        nId=RunSql("INSERT INTO dl$ywmx_%t(cjym,nysnd,nysyf,cyhbz,cyhbz1,cyhbz2,cyhmc,"
        "nywlb,nxh,dsfmx1,dsfmx2,dsfmx3,dsfmx4,dsfmx5,dsfmx6,dsfmx7,dsfmx8,dsfmx9,dsfmx10,"
        "dsfmx11,dsfmx12,dsfmx13,dsfmx14,dsfmx15,dsfmx16,dsfmx17,dsfmx18,dsfmx19,dsfmx20,"
        "dsfmx21,dsfmx22,dsfmx23,dsfmx24,dsfmx25,cjscz1,njscz2,djscz3,djscz4,nsl,"
        "dyjzk,dxzyjk,dsjk,dwjk,ddlsxf,nyfkfs,nsfkfs,nbl1,nbl2,dbl1,dbl2,cbl1,cbl2,"
        "cbl3,cbl4,cbl5,cbl6,cbl7,"
        "cjh,cczydh,nlkrq,nsfm,clxbz,nxzbz,ncsbz,cbz,cylsh,clsh) "
        "VALUES(%s,%ld,%ld,%s,%s,'.',%s,%ld,%ld,%lf,%lf,%lf,%lf,%lf,0,0,0,0,0,"
        "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'.',%ld,0,0,1,%lf,0,0,%lf,0,%ld,%ld,%ld,%ld,"
        "0,0,%s,%s,%s,%s,'.','.','.',"
        "'0','XTWH',%ld,%ld,'0000000000',0,0,'.',%s,'0')",
        sJym,sJym,nYsnd,nYsyf,sYhbz,sYhbz,sYhmc,0,nXh,dSjk,dQs,dZs,dCjbl,dBl,
        nSysLkrq,dSjk,dSjk,0,0,nDay,0,
        sYdlb,sGdsm,sQsrq,sJzrq,
        nSysLkrq,nSysSfm,sRealLsh);
        
        if(nId<0)
        {
            clrscr();
            GetSqlErrInfo(sData);
            outtext("\n������[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
	
        dTotalZk+=dSjk;
        if((nXh%1000)==0) CommitWork();    
        outtext("\n %8ld    %4ld    %2ld    %s    %.2lf",nXh,nYsnd,nYsyf,sYhbz,dSjk);
    }

    CommitWork();
    fclose(fp);
    if(nId>0)
    {
        sprintf(sData,"olddata/%s",sFileName);
        rename(sRealFileName,sData);
    	outtext("\n�ļ�������,����%ld��,�ܽ��:%.2lf",nXh,dTotalZk);
    	keyb();
    }
    return;
    
}


void CdTpyFileImport(char * sJym,char * sDirect,char * sFileName)
{

    FILE * fp;
    char sDump[12],sRealFileName[100],sData[400],sLsh[41],sRealLsh[41];
    long nId,nXh,nKey,nYsnd,nYsyf,nDay;
    char sYhbz[20],sYhbz1[20],sYhmc[61],sId[20],sName[20],sQq[20],sZq[20];
    double dTotalZk=0,dSjk;
    long nBz,nQc;
    char * offset,temp[12];
   
    sprintf(sRealFileName,"%s/%s",sDirect,sFileName);
    
    GetSysLkrq();
    clrscr();
       
    outtext("\n�������ʧЧ����...");
    if(RunSql("delete dl$ywmx_%t where nxzbz=3",sJym)>0) CommitWork();
    
    outtext("\n�������...");
    nXh=0;
    GenerateLsh(1,sJym,sSysJsdh,sLsh);
    
    if(!(fp=fopen(sRealFileName,"rt")))
    {
    	outtext("\n���ļ�����!");
    	keyb();
    	return;
    }
   
    for(;;)
    {
    	memset(sData,0,sizeof(sData));
    	if(getline(sData,fp)<=0) break;
    	alltrim(sData);
        offset=sData;
        nXh++;
        sprintf(sRealLsh,"%-32.32s%08ld",sLsh,nXh);
  
    	//1�����յ���16�ֽ�(�ַ���)2�����ִ���6�ֽ�(�ַ���)
	//3����������60�ֽ�(�ַ���)4�������ձ�־1�ֽ�(�ַ��� 1-ֻ�����գ�2-����������)
	//5��Ͷ�������֤18�ֽ�(�ַ���)6��Ͷ��������10�ֽ�(�ַ���)
	//7���ɷѽ��12�ֽ�(�����ͣ�decimal(12,2))8���ɷ��ڴ�2�ֽ�(����)
	//9���ɷ���ʼ��	11�ֽ�(������)10���ɷ���ֹ��11�ֽ�(������)
//CHD001AL1100002AL1101������(97)�����                                          1510103670119481   ��һ            172.00 72004/09/282005/09/27
    	offset=FetchStringToArg(sData,"%15s%6s%60s%1d%18s%12s%12f%2d%10s%10s",
        			sYhbz,sYhbz1,sYhmc,&nBz,sId,sName,&dSjk,&nQc,sQq,sZq);
       
        if(!offset)
        {
            clrscr();
            outtext("\n�ļ���ʽ����[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nId=-1;
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
        
 	/*printf("\nTEST:%s|%s|%s|%ld|%s|%s|%lf|%ld|%s|%s|",
    			sYhbz,sYhbz1,sYhmc,nBz,sId,sName,dSjk,nQc,sQq,sZq);
        keyb();*/
        
	if(RunSelect("select ROWNUM from dl$ywmx_%t where cyhbz=%s and cyhbz1=%s and \
			nbl1=%ld and nxzbz<>1",sJym,sYhbz,sYhbz1,nQc)>0)
	{
		RunSql("delete dl$ywmx_%t where cyhbz=%s and cyhbz1=%s and nbl1=%ld",
			sJym,sYhbz,sYhbz1,nQc);
	}
        
        FetchStringToArg(sQq,"%4d%1s%2d%1s%2d",&nYsnd,temp,&nYsyf,temp,&nDay);
        
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
        "0,0,%s,'.',%s,%s,'.','.','2','.','.','.','.','.','.','.','.','.','.','.',"
        "'0','XTWH',%ld,%ld,'0000000000',0,0,'.',%s,'0')",
        sJym,sJym,nYsnd,nYsyf,sYhbz,sYhbz1,sYhmc,sName,0,nXh,dSjk,
        nSysLkrq,dSjk,dSjk,nBz,0,nQc,0,
        sId,sQq,sZq,nSysLkrq,nSysSfm,sRealLsh);
        
        if(nId<0)
        {
            clrscr();
            GetSqlErrInfo(sData);
            outtext("\n������[��%ld��]:\n%s\n�Ƿ����(y/n)",nXh,sData);
            nKey=keyb();
            if(nKey!='Y' && nKey!='y') break;
            continue;
        }
	
        dTotalZk+=dSjk;
        if((nXh%1000)==0) CommitWork();    
        outtext("\n %8ld    %4ld    %2ld    %s    %.2lf",nXh,nYsnd,nYsyf,sYhbz,dSjk);
    }

    CommitWork();
    fclose(fp);
    if(nId>0)
    {
        sprintf(sData,"olddata/%s",sFileName);
        rename(sRealFileName,sData);        
    	outtext("\n�ļ�������,����%ld��,�ܽ��:%.2lf",nXh,dTotalZk);
    	keyb();
    }	
}
