#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "des.h"

#define ENCRYPT 1
#define DECRYPT 0

typedef struct des_key_schedule {
  unsigned char b[16][48];
} des_key_schedule;

static void unbit(unsigned char *to, unsigned char *from, int len);
static void pkbit(unsigned char *to, unsigned char *from, int len);
static void perm(unsigned char *to, const unsigned char *op, unsigned char *from, int bits);
static void rot(unsigned char *r, int len, int bits);
static void add2(unsigned char *to, unsigned char *from, int bits);
static void subs(unsigned char *o, unsigned char *i, int s);
static int des_set_key(unsigned char *key, des_key_schedule *ks);
static void des_func(unsigned char* in, unsigned char* out, unsigned char* key, int enc);
static void des_encrypt_char(unsigned char* sPlain, unsigned char* sCipher,unsigned char* sKey);
static void des_decrypt_char(unsigned char* sCipher, unsigned char* sPlain, unsigned char* sKey);

/***************************************************************
 * Data Structure:initial permutation IP
 *
 * Description:
 *     The first permutation for input data. 64 bits ----> 64 bits.
 *
 ***************************************************************/
static const unsigned char IP[64] =
{
  58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4,
  62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8,
  57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3,
  61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7
};

/****************************************************************
 * Data Structure:final permutation FP
 *
 * Description:
 *     The inverse of initial permutation(IP). 64 bits ----> 64 bits
 *
 ****************************************************************/
static const unsigned char FP[64] =
{
  40, 8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
  38, 6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
  36, 4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
  34, 2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25
};

/***************************************************************
 * Data Structure: left shift table for key schedule LS
 *
 * Description:
 *    The rotation left shift for key schedule. In main encrypt loop,
 *    56-bit keys(exclude the check bits 8,16,etc.) pass into PC1, and
 *    then divide into 2 groups, every group(28-bit) rotation left 
 *    shift LS[i] bits in the i-th loop. At last, the two groups(56-bit)
 *    pass into PC2.
 *
 ***************************************************************/

static const unsigned char LS[16] =
{
   1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

/***************************************************************
 * Data Structure:permuted-choice tables for key schedule pc1, pc2
 *
 * Description:
 *     For input 56-bit keys, it produce the other order keys via
 *     the permuted-choice tables. 
 *     56 bits ----> 56 bits.
 *
 ***************************************************************/

/* Use the key schedule specified in the standard ( ANSI X3.92-1981 ), */
static const unsigned char PC1[56] = 
{
  57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
  10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
  63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
  14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};
  
static const unsigned char PC2[48] =
{
  14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
  23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
  41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
  44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

/****************************************************************
 * Data Structure:bit-selection table E 
 *
 * Descripton:
 *     bit-selection and expansion algriothm: 32 bits ----> 48 bits.
 *
 ****************************************************************/

static const unsigned char E[48] =
{
  32,  1, 2,  3,  4,   5,  4,  5,  6,  7,  8,  9,
  8,   9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
  16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
  24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};

/***************************************************************
 * Data Structure:substitution tables ST
 *
 * Description:
 *     48 bits ----> 32 bits. (6 bits --> 4 bits)  
 *     selection-compression substitution table: Divide input 48 bits 
 *     into 8 groups, 6 bits per group. Then pass this 8 groups into 
 *     8 seperate S-box, where every S-box is a noNLINEar substitution
 *     network with 6 bits input and 4 bits output. The following are
 *     8 selection-compression functions for 8 S-box, every function
 *     contains a 2-dimension array(4 X 16 = 64 elements).  
 *
 ***************************************************************/

static const unsigned char ST[8][64] = {
  14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
   0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
   4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
  15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,

  15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
   3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
   0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
  13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,

  10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
  13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
  13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
   1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,

   7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
  13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
  10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
   3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,

   2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
  14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
   4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
  11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,

  12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
  10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
   9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
   4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,

   4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
  13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
   1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
   6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,

  13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
   1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
   7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
   2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};

/***************************************************************
 * Data Structure:permutation table P
 *
 * Description:
 *    32 bits ----> 32 bits
 *    Bit's index exchange.
 *
 ***************************************************************/
static const unsigned char P[32] = {
  16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
   2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25
};

/***************************************************************
 * Routine:unbit(to, from, len) -- unpack len bytes to bits
 *
 * Description:
 *     Convert bytes array into bits stream.
 *
 *  Parameter:
 *        to : Output; The pointer to output bits stream.
 *      from : Input;  The input char (byte) array.
 *       len : Input;  The length of input char array.
 *
 ***************************************************************/

static void unbit(unsigned char *to, unsigned char *from, int len)
{
  int i, j, t;

  for (i = 0; i < len; i++)
  {
    for (j = 0, t = *from++; j < 8; j++)
      *to++ = (t >> (7 - j)) & 1;
  }
}

/***************************************************************
 * Routine:pkbit(to, from, len) -- pack bits to len bytes
 *
 * Description:
 *     Convert bits stream int char(byte) array.
 *
 *  Parameter:
 *        to : Output; The pointer to output char array.
 *      from : Input;  The pointer to input bits stream.
 *       len : Input;  The length of output char array.
 ***************************************************************/

static void pkbit(unsigned char *to, unsigned char *from, int len)
{
  int i, j, t;

  for (i = 0; i < len; i++)
  {
    for (j = 0, t = 0; j < 8; j++)
      t |= *from++ << (7 - j);
    *to++ = t;
  }
}

/***************************************************************
 * Routine:perm(to, OP, from, bits) -- do permutation
 *
 * Description:
 *     Permutation map from one array into another through the
 *     p-table.
 *
 *  Parameter:
 *        to : Output; The pointer to output bits stream.
 *        op : Input;  The pointer to permutation table.
 *      from : Input;  The pointer of input bits stream.
 *      bits : Input;  The length of input bits stream.
 *
 ***************************************************************/
static void perm(unsigned char *to, const unsigned char *op, unsigned char *from, int bits)
{
  int i;

  for (i = 0; i < bits; i++)
    to[i] = from[op[i]-1];
}

/**************************************************************
 *
 * Routine: rot(to, from, len, bits) -- rotate left
 *
 * Description:
 *     Rotate left shift.
 *
 **************************************************************/

static void rot(unsigned char *r, int len, int bits)
{
  int i,t;

  if (len > 1)
  {
    for (i = 0; i < bits; i++)
    {
      t = r[0];
      memmove(&r[0], &r[1], len - 1);
      /* Note: memmove(dst, src, len) ensure the original ovlapped 
       *       region are copied before being overwritten if some
       *       regions of SRC and DST overlap. 
       */
      r[len-1] = t;
    }
  }
}

/**********************************************************************
 * 
 * Routine:des_set_key(key,ks) 
 *
 * Description:
 *        generate a keyschedule
 *
 * Parameter:
 *               key:  Input; The input 64-bit key stream. 
 *  des_key_schedule: Output; The output scheduled key's array
 *      
 **********************************************************************/

static int des_set_key(unsigned char *key, des_key_schedule *ks)
{
  int i;
  unsigned char cd[56];
 
  /* Permutation keys through PC1 P-table. */
  perm(cd, PC1, key, 56);
  
  for (i = 0; i < 16; i++)
  {
    rot(&cd[0], 28, LS[i]);
    rot(&cd[28], 28, LS[i]);
    perm(ks->b[i], PC2, cd, 48);
  }

  return 1;
}

/***************************************************************
 * 
 * Routine: add2(to, from, bits) 
 *
 * Description: 
 *     add bits modulo 2 (that is, EOR)
 *
 ***************************************************************/

static void add2(unsigned char *to, unsigned char *from, int bits)
{
  int i;

  for (i = 0; i < bits; i++)
    to[i] ^= from[i];
}

/****************************************************************
 *
 * Routine:subs(o,i,s) 
 *
 * Description:
 *       substitute 6 bits to 4 bits using ST[s]
 *
 ****************************************************************/

static void subs(unsigned char *o, unsigned char *i, int s) 
{
  int t;

  /* Note: The following calc equal convert index to
   *       
   *       (i[0]i[5]) X (i[1]i[2]i[3]i[4])
   */

  t = ST[s][
    (i[0] << 5) | (i[5] << 4) | /* 1st and 6th bit, row index */
    (i[1] << 3) | (i[2] << 2) | /* 2nd to 5th bit, col index */
    (i[3] << 1) |  i[4]];
  o[0] = (t >> 3) & 1;
  o[1] = (t >> 2) & 1;
  o[2] = (t >> 1) & 1;
  o[3] =  t & 1;
}

/*********************************************************************
 *
 * Routine:des_func(in,out,key, enc) 
 *
 * Description:
 *     DES core function
 *
 * Parameter:
 *     in: Input;  The pointer to the input plaintext 64 bits stream
 *    out: Output; The pointer to the output cipher 64 bits stream 
 *    key: Input;  The pointer to the 64 bits key value 
 *    enc: Input;  The flag 
 *                 1 ---- Encrypt
 *                 0 ---- Decrypt
 *
 **********************************************************************/
static void des_func(unsigned char* in, unsigned char* out, unsigned char* key, int enc)
{
  int i,j;
  int nIndex;
  unsigned char lr[64], rsto[32], si[48], so[32];
  unsigned char *left;
  unsigned char *right;
  des_key_schedule k_shedule;
  
  left = lr;
  right = lr + 32;

  perm(lr, IP, in, 64); /* initial permutation */
 
  des_set_key(key, &k_shedule); /* Generate key shedule */ 

  /* 16 rounds */
  for (i = 0; i < 16; i++)
  {
    memcpy(rsto, right,32); /* store right part */
    
    perm(si, E, right, 48); /* E-box: permute to subst. input */
    
    /* Add key (Enc/Dec order) */
    if (enc)
       nIndex = i;
    else
       nIndex = 15 - i;
    add2(si, (unsigned char *)k_shedule.b[nIndex], 48);   

    for (j = 0; j < 8; j++)     /* 48 to 32 bit substitute */
      subs(&so[j*4], &si[j*6], j);
    
    perm(right, P, so, 32); /* permute substitution output: end F*/
    
    add2(right, left, 32); /* add left part */
    
    memcpy(left, rsto, 32); /* old right part will be new left */
  }
  
  memcpy(left, right, 32); /* swap left and right part */
  
  memcpy(right,rsto, 32);
  
  perm(out, FP, lr, 64); /* final permutation */
}

/*************************************
**
**函数: DesEncString
**目的: 加密给定长度的串(可为字符串，二进制串，各种类型数组等)
**参数: sKey 密钥串. 一般为8个字节长(实际加密时只用其前8个字节)
**      nKeyLen 密钥长度. 如果密钥为字符串(以'\0'结尾)，值可为0; 
**              如果密钥不足8个字节, 密钥串后面不足的字节以'\0'填充
**      sSrc 加密前的串. 一般为8个倍数字节长
**      nSrcLen 加密前的串长度. 如果该串为字符串(以'\0'结尾)，值可为0; 
**              如果该串长度不足8的倍数个字节, 该串后面不足的字节以'\0'填充
**      sDest 加密后的串. 为8个倍数字节长
**      pnDestLen 加密后的串长度指针. 须先赋存放加密后的串的缓冲长度值,
**                函数执行后返回加密后的串长度
**                如果该指针为NULL, 表示缓冲区足够大并且不需要返回加密后的串长度 
**返回值: 0 成功
**        -1 密钥长度或加密前的串长度或加密后的串长度小于0
**        -2 存放加密后的串的缓冲不够大
**        -3 密钥为空或串指针为空,或者加密前的串为空或串指针为空,或者加密后的串指针为空
**
*************************************/
int DesEncString(char* sKey, int nKeyLen, char* sSrc, int nSrcLen, char* sDest, int* pnDestLen)
{
  unsigned char b_Plain[64];
  unsigned char b_Cipher[64];
  unsigned char b_Key[64];
  int nCipherLen;
  char sTempKey[8];
  char sPlain[8];
  char sCipher[8];
  int n;

  if(nKeyLen < 0 || nSrcLen < 0 || pnDestLen  && *pnDestLen < 0)
  {
    return -1;
  }

  if(sKey == NULL || sSrc == NULL || sDest == NULL)
  {
    return -3;
  }   

  if(nKeyLen == 0)
  {
    nKeyLen = strlen(sKey);
    if(nKeyLen == 0)
    {
      return -3;
    }
  }
  memset(sTempKey, 0, 8);
  memcpy(sTempKey, sKey, (nKeyLen < 8) ? nKeyLen : 8);

  if(nSrcLen == 0)
  {
    nSrcLen = strlen(sSrc);
    if(nSrcLen == 0)
    {
      return -3;
    }
  }
  
  nCipherLen = ((nSrcLen - 1) / 8 + 1) * 8;
  if(pnDestLen && *pnDestLen != 0 && nCipherLen > *pnDestLen)
  {
    return -2;
  }
  if(pnDestLen)
  {
    *pnDestLen = nCipherLen;
  }
   
  for(n = 0; n < nCipherLen; n +=8 )
  {
    memset(sPlain, 0, 8);
    memcpy(sPlain, sSrc + n, (nCipherLen - n < 8) ? nCipherLen - n : 8);
    
    unbit(b_Plain, (unsigned char*)sPlain, 8);
    unbit(b_Key, (unsigned char*)sTempKey, 8);
    memset(b_Cipher, 0, 64);

    des_func(b_Plain, b_Cipher, b_Key, ENCRYPT);

    pkbit((unsigned char*)sDest + n, b_Cipher, 8);
  }
  
  return 0;
}

/*************************************
**
**函数: DesDecString
**目的: 解密给定长度的串
**参数: sKey 密钥串. 一般为8个字节长(实际解密时只用其前8个字节)
**      nKeyLen 密钥长度. 如果密钥为字符串(以'\0'结尾)，值可为0; 
**              如果密钥不足8个字节, 密钥串后面不足的字节以'\0'填充
**      sSrc 解密前的串. 为8个倍数字节长
**      nSrcLen 解密前的串长度. 该串长度必须为8的倍数个字节
**      sDest 存放解密后的串. 为nSrcLen字节长, 如果是字符串, 调用者自己负责在尾部加'\0'
**      nDestLen 存放解密后的串的缓冲长度值. 如果该长度为0, 表示缓冲区足够大(至少nSrclen)
**返回值: 0 成功
**        -1 密钥长度或解密前的串长度或解密后的串长度小于0
**        -2 存放解密后的串的缓冲不够大
**        -3 密钥为空或串指针为空,或者加密前的串为空或串指针为空,或者加密后的串指针为空
**        -4 加密前的串长度为0或不是8的倍数
**
*************************************/
int DesDecString(char* sKey, int nKeyLen, char* sSrc, int nSrcLen, char* sDest, int nDestLen)
{
  unsigned char b_Plain[64];
  unsigned char b_Cipher[64];
  unsigned char b_Key[64];
  int nCipherLen;
  char sTempKey[8];
  char sPlain[8];
  char sCipher[8];
  int n;

  if(nKeyLen < 0 || nSrcLen < 0 || nDestLen < 0)
  {
    return -1;
  }

  if(sKey == NULL || sSrc == NULL || sDest == NULL)
  {
    return -3;
  }   

  if(nKeyLen == 0)
  {
    nKeyLen = strlen(sKey);
    if(nKeyLen == 0)
    {
      return -3;
    }
  }
  memset(sTempKey, 0, 8);
  memcpy(sTempKey, sKey, (nKeyLen < 8) ? nKeyLen : 8);

  if(nSrcLen == 0 || nSrcLen != (nSrcLen / 8) * 8)
  {
    return -4;
  }
  
  nCipherLen = nSrcLen;
  if(nDestLen && nCipherLen > nDestLen)
  {
    return -2;
  }
   
  for(n = 0; n < nCipherLen; n += 8)
  {
    memset(sCipher, 0, 8);
    memcpy(sCipher, sSrc + n, 8);
    
    unbit(b_Cipher, (unsigned char*)sCipher, 8);
    unbit(b_Key, (unsigned char*)sTempKey, 8);
    memset(b_Plain, 0, 64);

    des_func(b_Cipher, b_Plain, b_Key, DECRYPT);
    
    pkbit((unsigned char*)sDest + n, b_Plain, 8);
  }
 
  return 0;
}

/**注释: 例子
**      char sPlain[49], sCipher[48];
**      char sKey[9];
**      int nCipherLen;
**      ...
**      strcpy(sKey, "abcdefgh");
**      strcpy(sPlain, "This string is an example.");
**      nCipherLen = sizeof(sCipher); // 如果sCipher为字符指针(char*), 不能用sizof, 用实际申请的缓冲长度
**      // 加密
**      if(DesEncString(sKey, 0, sPlain, 0, sCipher, &nCipherLen))
**      {
**        // 错误处理
**        ...
**      }
**      ...
**      // 解密
**      if(DesDecString(sKey, 0, sCipher, nCipherLen, sPlain, 0))
**      {
**        // 错误处理
**        ...
**      }
**      sPlain[nCipherLen] = '\0';
*/
/*
main()
{
    char sPlain[100] = "This string is an example.";
    char sCipher[100];
    int nCipherLen = 0;
    int i;
    char sKey[9] = "abcdefgh";

    printf("DES算法实例\n", sKey);
    printf("密钥为:%s\n", sKey);
    printf("加密前的字符串为:%s\n", sPlain);
    printf("DesEncString返回值:%d\n", DesEncString(sKey, 0, sPlain, 0, sCipher, &nCipherLen));
    printf("加密后的串ASCII码为:\n");
    for(i = 0; i < nCipherLen; i++)
      printf("%02x ", ((unsigned int) sCipher[i]) & 0x00ff);
    printf("\n");

    printf("DesDecString返回值:%d\n", DesDecString(sKey, 0, sCipher, nCipherLen,  sPlain, 0));
    printf("解密后的串ASCII码为:\n");
    for(i = 0; i < nCipherLen; i++)
      printf("%02x ", ((unsigned int) sPlain[i]) & 0x00ff);
    printf("\n");
    sPlain[nCipherLen] = '\0';
    printf("解密后的字符串为:%s\n", sPlain);
}
*/



