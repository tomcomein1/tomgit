#ifndef _DES_H
#define _DES_H


#define ENCRYPT 1
#define DECRYPT 0

typedef unsigned int  WORD;

int gen_pin(unsigned int pinkey[8], unsigned int pin_source[8], unsigned int pin_target[8], int flag);
int gen_mac(char *mac_source, unsigned int mac_key[8], char *mac_value);
void ten2bit(int wkin[],unsigned wkout[]);
void bit2ten(unsigned wkin[],int wkout[]);
void keyinit(int key[], int subkey[16][48]);
void endes(unsigned int b[],int ENCRYPTION_FLAG,int subkey[16][48]);
void permu(int wkin[],int wkout[], char perm[],int num);
void vxor(int a[],int b[],int t);
int power(int a, int b);

void ErrDebug(char* fmt,...);
WORD ASC2BCD(const char *szHex);
int getmackey(unsigned int MacKey[8]);
int getpinkey(unsigned int PinKey[8]);

#endif
