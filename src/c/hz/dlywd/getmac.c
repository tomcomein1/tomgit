#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zydes.h"
#include "zydesx.h"

int GetMac(const char* caInBuf,int iInLen,const char* caMacKey,char* caMac)
{                         
    /***                        return code: 0  成功
                                             -1 其他错误
    ***/                  
                          
    WORD pKey[8];         
    unsigned int tmp_mac[8];
    int pip_key[64];      
    int subkey[16][48];   
    char *ptr_mac_source; 
    unsigned int i = 0,j = 0;
    char *ptr_tmpmacbuffer = NULL;
                          
    ptr_tmpmacbuffer = malloc(iInLen + 1);
                          
    memset(ptr_tmpmacbuffer, 0, iInLen + 1);
    memcpy(ptr_tmpmacbuffer, caInBuf, iInLen);
                          
    for (i = 0; i < 8; i++)
        pKey[i] = caMacKey[i];
                          
    i = 0;                
                          
    ptr_mac_source = ptr_tmpmacbuffer;
                          
/*                        
    for (j=1;j<iInLen;j++) {
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
*/                        
    for (j=1;j<iInLen;j++) {
        if( ( isalnum(ptr_mac_source[j])) ||
            ( ptr_mac_source[j] == ' ' ) ||  
            ( ptr_mac_source[j] == ',' ) ||  
            ( ptr_mac_source[j] == '.' ) ) { 
            if(ptr_mac_source[j] == ' ') {
                if (ptr_mac_source[j+1]!=' ') {
                    continue;
                }         
                strcpy(ptr_mac_source+j,ptr_mac_source+j+1);
                j--;      
            }             
        }                 
    }                     
                          
                          
    for(j = 0;j < 8 && j < iInLen;j ++) 
        tmp_mac[j] = ptr_mac_source[j];
                          
/*printf("no.1 source=[%02X%02X%02X%02X%02X%02X%02X%02X]\n", tmp_mac[0],tmp_mac[1],tmp_mac[2],tmp_mac[3],tmp_mac[4],tmp_mac[5],tmp_mac[6],tmp_mac[7]);*/
                          
    bit2ten(pKey,pip_key);
    keyinit(pip_key,subkey);
    endes(tmp_mac,1,subkey);
                          
    if(iInLen > 8) {      
        ptr_mac_source += 8;
        while (ptr_mac_source[0] != 0) {
/*printf("source=[%02X%02X%02X%02X%02X%02X%02X%02X]\n", tmp_mac[0],tmp_mac[1],tmp_mac[2],tmp_mac[3],tmp_mac[4],tmp_mac[5],tmp_mac[6],tmp_mac[7]);*/
/*printf("       [");*/   
            for(i = 0; i<8;i++) {
                tmp_mac[i] ^= ptr_mac_source[0];
/*printf(" %c", ptr_mac_source[0]);*/
                ptr_mac_source ++;
                if(ptr_mac_source[0] == 0) break;
            }             
/*printf("source=[%02X%02X%02X%02X%02X%02X%02X%02X]\n", tmp_mac[0],tmp_mac[1],tmp_mac[2],tmp_mac[3],tmp_mac[4],tmp_mac[5],tmp_mac[6],tmp_mac[7]);*/
            endes(tmp_mac,1,subkey);
        }                 
    }                     
                          
    for(j = 0; j < 4; j ++) {
        sprintf(caMac + j*2,"%02X",tmp_mac[j]);
    }                     
    caMac[8] = 0;         
    free(ptr_tmpmacbuffer);
                          
    return 8;             
                          
 }      
 
 
int EnCrypt(const char* caInBuf,const char* caKey,char *caOutBuf)
{                         
    char caTmpBuffer[17]; 
    int i;                
                          
/*printf("ENCRYPT::inbuf=[%s]  key=[%s]\n", caInBuf,caKey);*/
    memset(caTmpBuffer, 0, sizeof(caTmpBuffer));
    DesEncrypt(caKey,caInBuf,caTmpBuffer);
    /*printf("caTmpBuffer = %s\n",caTmpBuffer);*/
                          
    for(i = 0; i < 8; i ++) {
         caOutBuf[i] = ASC2BCD(caTmpBuffer + (i*2));
    }                     
  /*  printf("caOutBuf = %s\n",caOutBuf);*/
    /*DesEncrypt(caKey,caInBuf,caOutBuf);*/
                          
    return 0;             
 }                        
                          
int DeCrypt(const char* caInBuf,const char* caKey,char* caOutBuf)
{                         
                          
/*printf("DECRYPT::inbuf=[%s]  key=[%s]\n", caInBuf, caKey);*/
                          
    DesDecrypt(caKey,caInBuf,caOutBuf);
    return 0;             
}                         
 
              
