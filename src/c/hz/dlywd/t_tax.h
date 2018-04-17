#ifndef T_TAX_H
#define T_TAX_H

long dsTaskTaxQuery(char * inbuf,RECORD * Client);
long dsTaskTax1Query(char * inbuf,RECORD * Client);
long dsTaskTaxPayConfirm(char * inbuf,char * outbuf);
static int dsTaxSendAndRecv(int nsock,char * sinbuf,char * soutbuf);
static void GetTaxErrorInfo(char * sError,char * sInfo);
long dsTaxCheckAccount(char * inbuf,char * outbuf);

typedef struct //单张税票报文
{
	char czspl[3];
	char chdbz[2];
	char czgswjg[31];
	char cdsgx[31];
	char czclx[31];
	char czsjg[31];
	char cnsrbm[21];
	char cnsdjh[21];
	char cqymc[51];
	char cqydz[51];
	long nysnd;
	long nysyf;
	char cskgk[31];
	long nxjrq;
	long nskxjrq;
	long nszgs;
	char csbxh[31];
	char cszmc[31];
	char cyskmbm[7];
	char cyskmmc[41];
	char cyskmjc[31];
	long ncljz;
	long npmgs;
	char cpmmc[41];
	double dkssl;
	double dxssr;
	double dsl;
	double ddqje;
	double dsjje;
	char cbz[51];
}SWMX;

static long dsTaxYwmxRk(char *sLsh,char *sCzydh,char * sBuffer,long *nCount);


#endif
