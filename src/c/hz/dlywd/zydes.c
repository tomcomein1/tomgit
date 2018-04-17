#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include "zydes.h"
#include "zydes64.h"

/* 
 * The following code is order to do the des descrypt and enscrypt 
 */

void keyinit(int key[], int subkey[16][48])
/* Key to use, stored as an array of Longs */
{
	int wk[56], wk2[48];   /* Storage for the two 28-bit key halves C and D */
	int i,j,k,temp;

	permu(key, wk, PC1, 56);       /* Permute key with PC1    */

	for (i = 0; i < 16; i++)       /* Form sixteen subkeys    */
	{
		for (j=0;j<keyrot[i];j++)
		{
			temp = wk[0];
			for (k=0;k<27;k++)
				wk[k] = wk[k+1];
			wk[27] = temp;
		}
		for (j=0;j<keyrot[i];j++)
		{
			temp = wk[28];
			for (k=28;k<55;k++)
				wk[k] = wk[k+1];
			wk[55] = temp;
		}
		permu(wk, wk2, PC2, 48);   /* Apply PC2 to form subkey */
		for (j=0;j<48;j++)
			subkey[i][j] = wk2[j];
		}
}

void endes(unsigned int b[], int ENCRYPTION_FLAG,int subkey[16][48])
{
	int work[64];      /*  64-bit working store       */
	int work2[64];
/*	int work3[64];*/
	int rwork[32];
	int lwork[32];
	int twork[32];
	int t2work[32];
	int erwork[48];
	int sval[8];
	int i,j,row,col,rocol,m,kk;
	/*int k,ll;*/

	bit2ten(b,work2);

	permu(work2, work, IP, 64);        /*  Apply IP to input block    */

	for (i=0;i<32;i++) lwork[i] = work[i];
	for (i=0;i<32;i++) rwork[i] = work[i+32];
	for (i=0;i<16;i++)                 /*  Perform the 16 rounds      */
	{
	  permu(rwork, erwork, E, 48);
	  /* expanded input data R(i) to 48 bits using fn E   */

	  if (ENCRYPTION_FLAG == 1)
		 kk = i;
	  else
		 kk = 15 - i;
	  vxor(subkey[kk],erwork,48);

	  for (j=0;j<48;j +=6)
	  {
		row = power(erwork[j+0],1) + power(erwork[j+5],0);
		col = power(erwork[j+1],3) + power(erwork[j+2],2)+ power(erwork[j+3],1) + power(erwork[j+4], 0);
		rocol = row * 16 + col;
		m = j / 6;
		sval[m] = S[m][rocol];
	  }

	  for (j=0;j<32;j +=4)
      {
	    m = j / 4;
	    if (sval[m] ==  0) { twork[j]=0; twork[j+1]=0; twork[j+2]=0 ; twork[j+3] = 0;}
		if (sval[m] ==  1) { twork[j]=0; twork[j+1]=0; twork[j+2]=0 ; twork[j+3] = 1;}
		if (sval[m] ==  2) { twork[j]=0; twork[j+1]=0; twork[j+2]=1 ; twork[j+3] = 0;}
	    if (sval[m] ==  3) { twork[j]=0; twork[j+1]=0; twork[j+2]=1 ; twork[j+3] = 1;}
	    if (sval[m] ==  4) { twork[j]=0; twork[j+1]=1; twork[j+2]=0 ; twork[j+3] = 0;}
	    if (sval[m] ==  5) { twork[j]=0; twork[j+1]=1; twork[j+2]=0 ; twork[j+3] = 1;}
	    if (sval[m] ==  6) { twork[j]=0; twork[j+1]=1; twork[j+2]=1 ; twork[j+3] = 0;}
	    if (sval[m] ==  7) { twork[j]=0; twork[j+1]=1; twork[j+2]=1 ; twork[j+3] = 1;}
	    if (sval[m] ==  8) { twork[j]=1; twork[j+1]=0; twork[j+2]=0 ; twork[j+3] = 0;}
	    if (sval[m] ==  9) { twork[j]=1; twork[j+1]=0; twork[j+2]=0 ; twork[j+3] = 1;}
	    if (sval[m] == 10) { twork[j]=1; twork[j+1]=0; twork[j+2]=1 ; twork[j+3] = 0;}
	    if (sval[m] == 11) { twork[j]=1; twork[j+1]=0; twork[j+2]=1 ; twork[j+3] = 1;}
	    if (sval[m] == 12) { twork[j]=1; twork[j+1]=1; twork[j+2]=0 ; twork[j+3] = 0;}
	    if (sval[m] == 13) { twork[j]=1; twork[j+1]=1; twork[j+2]=0 ; twork[j+3] = 1;}
	    if (sval[m] == 14) { twork[j]=1; twork[j+1]=1; twork[j+2]=1 ; twork[j+3] = 0;}
	    if (sval[m] == 15) { twork[j]=1; twork[j+1]=1; twork[j+2]=1 ; twork[j+3] = 1;}
      }

	permu(twork, t2work, P, 32);   /*  32-bit permutation P to B  */
	vxor(lwork,t2work,32);

	for (j=0;j<32;j++) lwork[j] = rwork[j];
	for (j=0;j<32;j++) rwork[j] = t2work[j];
   }

  for (j=0 ;j<32;j++) work[j] = rwork[j];
  for (j=32;j<64;j++) work[j] = lwork[j - 32];
  permu(work, work2, FP ,64);        /*  Perform Inverse IP      */

  ten2bit(work2,b);
}


void bit2ten(unsigned wkin[], int wkout[])
{
	int i,j,k=0;
	unsigned int aa;
	for (i=0;i<8;i++) {
		for (j=0;j<8;j++) {
			aa = 0x00;
			aa = wkin[i] >> (7-j);
			aa = aa & 0x01;
 		    if (aa == 0x01) wkout[k] = 1;
			else wkout[k] = 0;
			k++;
        }	
	}
}

void ten2bit(int wkin[],unsigned wkout[])
{
  int i,j,k=0;
  unsigned int aa;
  for (i=0;i<8;i++) {
	  wkout[i] = 0x00;
	  for (j=0;j<8;j++) {
		  aa = 0x01;
		  if (wkin[k] == 1) {
			  aa = aa << (7-j);
			  wkout[i] = wkout[i] | aa;
		  }
		  k +=1;
	  }
  }
}

void permu(int wkin[],int wkout[],char perm[],int num)
  /* Input 64-bit block after permutation  */
  /* Output 64-bit block to be permuted    */
  /* Permutation array                     */
{

  int i,m;

  for (i=0;i<num;i++) {
	m = perm[i] - 1;
	wkout[i] = wkin[m];
  }
}

void vxor(int a[],int b[],int t)
{
   int k,temp;
   /*int j;*/
   for (k=0;k<t;k++) {
	   if (a[k] == 0 && b[k] == 0)  temp = 0;
	   if (a[k] == 1 && b[k] == 1)  temp = 0;
	   if (a[k] == 0 && b[k] == 1)  temp = 1;
	   if (a[k] == 1 && b[k] == 0)  temp = 1;
	   b[k] = temp;
   }
}

int power(int a, int b)
{
   switch (b)
   {
	   case 0 : if (a == 1) return(1); else return(0);
	   case 1 : if (a == 1) return(2); else return(0);
	   case 2 : if (a == 1) return(4); else return(0);
	   case 3 : if (a == 1) return(8); else return(0);
   }
   return 0;
}

void do_decrypt(char key[],char EncryptedData[],char AtmID[],char PIN[])
{
  int i, j;
  unsigned int keyint[8];
  int Key[64];
  int subkey[16][48];
  unsigned int work[8];

  for ( i=0; i<8; i++)
	  keyint[i] = key[i];
  bit2ten(keyint, Key);   /* transfer bit to byte */
  keyinit(Key , subkey);  /* find out the sub_key of des algorithm */ 

  for (i=0, j=0; j<8; i+=2 , j++)   /* packed the 16 bytes data to 8 bytes */ 
	 work[j] = ((EncryptedData[i] - 0x30) << 4) | (EncryptedData[i+1] - 0x30);

  endes(work, 0, subkey);   /* use des algorithm decrypt the 8 bytes data */

  /* From the decrypted data, expand first 2 bytes to AtmID (4 bytes), and last 3 bytes to PIN (6 bytes) */
  for (i=0,j=0; i<2; i++,j+=2)
  {
    AtmID[j] = ((work[i] & 0xf0) >> 4) + 0x30;
	AtmID[j+1] = (work[i] & 0x0f) + 0x30;
  }
  AtmID[4] = 0;

  for (i=5,j=0; i<8; i++, j+=2)
  {
	PIN[j] = ((work[i] & 0xf0) >> 4) + 0x30;
	PIN[j+1] = (work[i] & 0x0f) + 0x30;
  }
  PIN[6] = 0;
}

void do_encrypt(char key[],char AtmID[],char PIN[],char EncryptedData[])
{
  int i, j;
  unsigned int keyint[8];
  int Key[64];
  int subkey[16][48];
  unsigned int work[8];

  for ( i=0; i<8; i++)
	  keyint[i] = key[i];
  bit2ten(keyint, Key);
  keyinit(Key , subkey);

  /* pack the AtmID and PIN */
  for (i=0, j=0; j<2; i+=2 , j++)
	 work[j] = ((AtmID[i] - 0x30) << 4) | (AtmID[i+1] - 0x30);

  for (j=2; j<5; j++)
	 work[j] = 0;

  for (j=5,i=0; j<8; j++, i+=2)
     work[j] = ((PIN[i] - 0x30) << 4) | (PIN[i+1] - 0x30);

  /* encrypt AtmID(first 2 bytes) and PIN (last 3 bytes) */
  endes(work, 1, subkey);
  /* expand the encrypted data(work) form 8 bytes to 16 bytes */
  for (i=0,j=0; i<8; i++, j+=2)
  {
	EncryptedData[j] = ((work[i] & 0xf0) >> 4) + 0x30;
	EncryptedData[j+1] = (work[i] & 0x0f) + 0x30;
  }
  EncryptedData[16] = 0;
}

int gen_pin(unsigned int pinkey[8], unsigned int pin_source[8], 
            unsigned int pin_target[8], int flag)
{
    /* 加密/解密pin
     * pinkey:      pin key明文
     * clear_pin:   源pin
     * target_pin:  目标pin
     * flag:        加密/解密标志  0:解密 1:加密
     */
    int pip_key[64];
    int subkey[16][48];
    unsigned int tmp_pin[8];
    int i;

    for(i = 0; i < 8; i++)
        tmp_pin[i] = pin_source[i];

    bit2ten(pinkey,pip_key);
    keyinit(pip_key,subkey);

    if(flag == ENCRYPT)
        endes(tmp_pin, ENCRYPT, subkey);
    else
        endes(tmp_pin, DECRYPT, subkey);

    for(i = 0; i < 8; i++)
        pin_target[i] = tmp_pin[i];

    return 0;
}

int gen_mac(char *mac_source, unsigned int mac_key[8], char *mac_value)
{
    unsigned int  tmp_mac[9];
    int pip_key[64];
    int subkey[16][48];
    char *ptr_mac_source;
    int i,j;

    ptr_mac_source = mac_source;

    for (j=1;j<strlen(mac_source);j++) {
        if( ( isalnum(ptr_mac_source[j])) ||
            ( ptr_mac_source[j] == 32 ) ||  
            ( ptr_mac_source[j] == 44 ) ||  
            ( ptr_mac_source[j] == 46 ) ) { 
            if(ptr_mac_source[j] == ' ') {
                if (ptr_mac_source[j+1]!=' ') {
                    continue;
                }
                strcpy(ptr_mac_source+j,ptr_mac_source+j+1);
                j--;
            }
        }
    }

    for(j = 0;j < 8 && j < strlen(ptr_mac_source);j ++) 
        tmp_mac[j] = ptr_mac_source[j];

    bit2ten(mac_key,pip_key);
    keyinit(pip_key,subkey);
    endes(tmp_mac,1,subkey);

    if(strlen(ptr_mac_source)>8) {
        ptr_mac_source += 8;
        while (ptr_mac_source[0] != 0) {
            for(i = 0; i<8;i++) {
                tmp_mac[i] ^= ptr_mac_source[0];
                ptr_mac_source ++;
                if(ptr_mac_source[0]==0) break;
            }
            endes(tmp_mac,1,subkey);
        }
    }
 
    for(j = 0; j < 4; j ++) {
        sprintf(mac_value + j*2,"%02X",tmp_mac[j]);
    }
    mac_value[8] = 0;

    return 8;
}

int getpinkey(unsigned int PinKey[8])
{
    FILE* fpKey;
    char KeyFile[256];
    unsigned int pKey[8];
    unsigned int siKey[8];
    char sKey[17];
    int I = 0;
    
    pKey[0] = 0x35;  pKey[4] = 0x35; 
    pKey[1] = 0x31;  pKey[5] = 0x31; 
    pKey[2] = 0x35;  pKey[6] = 0x35; 
    pKey[3] = 0x31;  pKey[7] = 0x31; 
    
    
    memset(KeyFile,0,sizeof(KeyFile));
    strcpy(KeyFile,getenv("PKF"));
    if (strlen(KeyFile) == 0)
    {
        ErrDebug("PKF Not Set In .profile!");
        return -1; 
    }
    fpKey = fopen(KeyFile,"r");
    if (fpKey == NULL)
    {
        ErrDebug("Open Key File %s Error!ErrNo = [%d]",KeyFile,errno);
        return -2;
    }
    fgets(sKey,17,fpKey);
    fclose(fpKey);
    if (strlen(sKey) != 16)
    {
        ErrDebug("Get Private Key Failed!PKey = [%s]",sKey);
        return -3;
    }
    printf("sKey = [%s]\n",sKey);
    for (I = 0;I < 8;I++)
    {
        siKey[I] = ASC2BCD(sKey + (I*2));
    }
    
    gen_pin(pKey,siKey,PinKey,DECRYPT);
    
    return 0;
}

int getmackey(unsigned int MacKey[8])
{
    FILE* fpKey;
    char KeyFile[256];
    unsigned int pKey[8];
    unsigned int siKey[8];
    char sKey[17];
    int I = 0;
    
    pKey[0] = 0x31;  pKey[4] = 0x31; 
    pKey[1] = 0x35;  pKey[5] = 0x35; 
    pKey[2] = 0x31;  pKey[6] = 0x31; 
    pKey[3] = 0x35;  pKey[7] = 0x35; 
    
    memset(KeyFile,0,sizeof(KeyFile));
    strcpy(KeyFile,getenv("MKF"));
    
    if (strlen(KeyFile) == 0)
    {
        ErrDebug("MKF Not Set In .profile!");
        return -1; 
    }
    fpKey = fopen(KeyFile,"r");
    if (fpKey == NULL)
    {
        ErrDebug("Open Key File %s Error!ErrNo = [%d]",KeyFile,errno);
        return -2;
    }
    fgets(sKey,17,fpKey);
    fclose(fpKey);
    if (strlen(sKey) != 16)
    {
        ErrDebug("Get Mac Key Failed!PMKey = [%s]",sKey);
        return -3;
    }
    
    printf("sKey = [%s]\n",sKey);
    for (I = 0;I < 8;I++)
    {
        siKey[I] = ASC2BCD(sKey + (I*2));
    }
    
    gen_pin(pKey,siKey,MacKey,DECRYPT);
    
    return 0;
}

void ErrDebug(char* fmt,...)
{
    time_t  t;
    struct tm       * tm1;
    char  ErrFile[28];
    char temp[512];
    int  iPid = 0;
    char Format[1024];
    va_list values;
    
    FILE* fperr;
    
    strcpy(ErrFile,"/tmp/Error.debug");
    fperr = fopen(ErrFile,"a");
    if (fperr == NULL)
    {
        perror(ErrFile);
        return;
    }
    
    t = time(NULL);
    tm1 = localtime(&t);
    iPid = getpid();    
    sprintf(temp,"*****%02d/%02d/%02d-%02d:%02d:%02d.%d*****",tm1->tm_mon+1,
            tm1->tm_mday,(tm1->tm_year+1900)%100,tm1->tm_hour,tm1->tm_min,
            tm1->tm_sec,iPid);
    fprintf(fperr,"%s\n",temp);
    
    strcpy(Format,fmt);
    if (fmt[strlen(fmt)-1] != '\n')
    strcat(Format,"\n");
    
    
    va_start(values,Format);
    (void)vfprintf(fperr,Format,values);
    va_end(values);
    
    fclose(fperr);
} 

WORD ASC2BCD(const char *szHex)
{
    unsigned int i, i1;
    if(szHex[0] >= '0' && szHex[0] <= '9') i = szHex[0] - '0';
    else if(szHex[0] >= 'A' && szHex[0] <= 'F')
             i = szHex[0] - 'A' + 10;

    if(szHex[1] >= '0' && szHex[1] <= '9') i1 = szHex[1] - '0';
    else if(szHex[1] >= 'A' && szHex[1] <= 'F')
             i1 = szHex[1] - 'A' + 10;

    return (unsigned int) (i * 16 + i1);
}


