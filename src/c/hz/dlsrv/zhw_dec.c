#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <signal.h>
#include "zhw_dec.h"
#include "sys/wait.h"

#define EN0 0 /* MODE == encrypt */
#define DE1 1 /* MODE == decrypt */

typedef struct {
  unsigned int ek[32];
  unsigned int dk[32];
} des_ctx;

static char sUserEnv[101];
static char sPassEnv[101];
static long terminated;

void cleanup(int sig);

static void deskey(unsigned char* key, short edf);
/*                hexkey[8]     MODE
 * Sets the internal key register according to the hexadecimal
 * key contained in the 8 bytes of hexkey, according to the DES,
 * for encryption or decryption according to MODE.
 */
static void usekey(unsigned int* from);
/*                cookedkey[32]
 * Loads the internal key register with the data in cookedkey.
 */

static void cpkey(unsigned int * into);
/*                cookedkey[32]
 * Copies the contents of the internal key register into the storage
 * located at &cookedkey[0]
 */

static void des(unsigned char* inblock, unsigned char* outblock);

/*                from[8]       to[8]
 * Encrypts/Decrypts (according to the key currently loaded in the
 * internal key register) one block of eight bytes at address 'from'
 * into the block at address 'to'. They can be the same.
 */

static void scrunch(unsigned char* outof, unsigned int *into);
static void unscrun(unsigned int* outof, unsigned char* into);
static void desfunc(unsigned int* block, unsigned int* keys);
static void cookey(unsigned int * raw1);

static unsigned int KnL[32] = { 0L };
static unsigned int KnR[32] = { 0L };
static unsigned int Kn3[32] = { 0L };
static unsigned char Df_Key[24] = 
{
  0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
  0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
  0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67
};

static unsigned short bytebit[8] = 
{
  0200, 0100, 040, 020, 010, 04, 02, 01
};

static unsigned int bigbyte[24] =
{
  0x800000L, 0x400000L, 0x200000L, 0x100000L,
  0x80000L,  0x40000L,  0x20000L,  0x10000L, 
  0x8000L,   0x4000L,   0x2000L,   0x1000L, 
  0x800L,    0x400L,    0x200L,    0x100L,
  0x80L,     0x40L,     0x20L,     0x10L,
  0x8L,      0x4L,      0x2L,      0x1L
};

/***************************************************************
 * Data Structure: left shift table for key schedule totrot
 * Description:
 *    The rotation left shift for key schedule. In main encrypt loop,
 *    56-bit keys(exclude the check bits 8,16,etc.) pass into pc1, and
 *    then divide into 2 groups, every group(28-bit) rotation left 
 *    shift LS[i] bits in the i-th loop. At last, the two groups(56-bit)
 *    pass into pc2.
 *
 ***************************************************************/

static const unsigned char totrot[16] =
{
  1, 2, 4, 6, 8, 10, 12, 14, 15, 17, 19, 21, 23, 25, 27, 28
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
static const unsigned char pc1[56] = 
{
  56, 48, 40, 32, 24, 16,  8,  0, 57, 49, 41, 33, 25, 17,
   9,  1, 58, 50, 42, 34, 26, 18, 10,  2, 59, 51, 43, 35,
  62, 54, 46, 38, 30, 22, 14,  6, 61, 53, 45, 37, 29, 21,
  13,  5, 60, 52, 44, 36, 28, 20, 12,  4, 27, 19, 11,  3
};
  
static const unsigned char pc2[48] =
{
  13, 16, 10, 23,  0,  4,     2, 27, 14,  5, 20,  9,
  22, 18, 11,  3, 25,  7,    15,  6, 26, 19, 12,  1,
  40, 51, 30, 36, 46, 54,    29, 39, 50, 44, 32, 47,
  43, 48, 38, 55, 33, 52,    45, 41, 49, 35, 28, 31
};

static void deskey(unsigned char* key, short edf)
{
  int i, j, l, m, n;
  unsigned char pclm[56], pcr[56];
  unsigned int kn[32];
  
  for (j = 0; j < 56; j++)
  {
    l = pc1[j];
    m = l & 07;
    pclm[j] = (key[l >> 3] & bytebit[m]) ? 1 : 0;
  }
  for(i = 0; i < 16; i++)
  {
    if(edf == DE1)
      m = (15 - i) << 1;
    else
      m = i << 1;
    n = m + 1;
    kn[m] = kn[n] = 0L;
    for (j = 0; j < 28; j++)
    {
      l = j + totrot[i];
      if(l < 28)
        pcr[j] = pclm[l];
      else
        pcr[j] = pclm[l - 28];
    }
    for (j = 28; j < 56; j++)
    {
      l = j + totrot[i];
      if(l < 56)
        pcr[j] = pclm[l];
      else
        pcr[j] = pclm[l - 28];
    }
    for (j = 0; j < 24; j++)
    {
      if (pcr[pc2[j]])
        kn[m] |= bigbyte[j];
      if (pcr[pc2[j + 24]])
        kn[n] |= bigbyte[j];
    }
  }
  cookey(kn);
  return;
}

static void cookey(unsigned int* raw1)
{
  unsigned int *cook, *raw0;
  unsigned int dough[32];
  int i;
  
  cook = dough;
  for(i = 0; i < 16; i++, raw1++)
  {
    raw0 = raw1++;
    *cook    = (*raw0 & 0x00fc0000L) << 6;
    *cook   |= (*raw0 & 0x00000fc0L) << 10;
    *cook   |= (*raw1 & 0x00fc0000L) >> 10;
    *cook++ |= (*raw1 & 0x00000fc0L) >> 6;
    *cook    = (*raw0 & 0x0003f000L) << 12;
    *cook   |= (*raw0 & 0x0000003fL) << 16;
    *cook   |= (*raw1 & 0x0003f000L) >> 4;
    *cook++ |= (*raw1 & 0x0000003fL);
  }
  usekey(dough);
  return;
}

static void cpkey(unsigned int* into)
{
  unsigned int *from, *endp;
  
  from = KnL, endp = &KnL[32];
  while (from < endp)
    *into++ = *from++;
  return;
}

static void usekey(unsigned int* from)
{
  unsigned int *to, *endp;
  
  to = KnL, endp = &KnL[32];
  while (to < endp)
    *to++ = *from++;
  return;
}

static void des(unsigned char* inblock, unsigned char* outblock)
{
  unsigned int work[2];
  
  scrunch(inblock, work);
  desfunc(work, KnL);
  unscrun(work, outblock);
  return;
}

static void scrunch(unsigned char* outof, unsigned int* into)
{
  *into    = (*outof++ & 0xffL) << 24;
  *into   |= (*outof++ & 0xffL) << 16;
  *into   |= (*outof++ & 0xffL) <<  8;
  *into++ |= (*outof++ & 0xffL);
  *into    = (*outof++ & 0xffL) << 24;
  *into   |= (*outof++ & 0xffL) << 16;
  *into   |= (*outof++ & 0xffL) <<  8;
  *into   |= (*outof   & 0xffL);
  return;
}  

static void unscrun(unsigned int* outof, unsigned char* into)
{
  *into++ = (*outof >> 24) & 0xffL;
  *into++ = (*outof >> 16) & 0xffL;
  *into++ = (*outof >>  8) & 0xffL;
  *into++ =  *outof++      & 0xffL;
  *into++ = (*outof >> 24) & 0xffL;
  *into++ = (*outof >> 16) & 0xffL;
  *into++ = (*outof >>  8) & 0xffL;
  *into   =  *outof        & 0xffL;
  return;
}

static unsigned int SP1[64] =
{
  0x01010400l, 0x00000000L, 0x00010000L, 0x01010404L,
  0x01010004L, 0x00010404L, 0x00000004L, 0x00010000L,
  0x00000400L, 0x01010400L, 0x01010404L, 0x00000400L,
  0x01000404L, 0x01010004L, 0x01000000L, 0x00000004L,
  
  0x00000404L, 0x01000400L, 0x01000400L, 0x00010400L,
  0x00010400L, 0x01010000L, 0x01010000L, 0x01000404L,
  0x00010004L, 0x01000004L, 0x01000004L, 0x00010004L,
  0x00000000L, 0x00000404L, 0x00010404L, 0x01000000L,
  
  0x00010000L, 0x01010404L, 0x00000004L, 0x01010000L,
  0x01010400L, 0x01000000L, 0x01000000L, 0x00000400L,
  0x01010004L, 0x00010000L, 0x00010400L, 0x01000004L,
  0x00000400L, 0x00000004L, 0x01000404L, 0x00010404L,
  
  0x01010404L, 0x00010004L, 0x01010000L, 0x01000404L,
  0x01000004L, 0x00000404L, 0x00010404L, 0x01010400L,
  0x00000404L, 0x01000400L, 0x01000400L, 0x00000000L,
  0x00010004L, 0x00010400L, 0x00000000L, 0x01010004L
};

static unsigned int SP2[64] =
{
  0x80108020L, 0x80008000L, 0x00008000L, 0x00108020L,
  0x00100000L, 0x00000020L, 0x80100020L, 0x80008020L,
  0x80000020L, 0x80108020L, 0x80108000L, 0x80000000L,
  0x80008000L, 0x00100000L, 0x00000020L, 0x80100020L,
  
  0x00108000L, 0x00100020L, 0x80008020L, 0x00000000L,
  0x80000000L, 0x00008000L, 0x00108020L, 0x80100000L,
  0x00100020L, 0x80000020L, 0x00000000L, 0x00108000L,
  0x00008020L, 0x80108000L, 0x80100000L, 0x00008020L,
  
  0x00000000L, 0x00108020L, 0x80100020L, 0x00100000L,
  0x80008020L, 0x80100000L, 0x80108000L, 0x00008000L,
  0x80100000L, 0x80008000L, 0x00000020L, 0x80108020L,
  0x00108020L, 0x00000020L, 0x00008000L, 0x80000000L,

  0x00008020L, 0x80108000L, 0x00100000L, 0x80000020L,
  0x00100020L, 0x80008020L, 0x80000020L, 0x00100020L,
  0x00108000L, 0x00000000L, 0x80008000L, 0x00008020L,
  0x80000000L, 0x80100020L, 0x80108020L, 0x00108000L
};

static unsigned int SP3[64] =
{
  0x00000208L, 0x08020200L, 0x00000000L, 0x08020008L,
  0x08000200L, 0x00000000L, 0x00020208L, 0x08000200L,
  0x00020008L, 0x08000008L, 0x08000008L, 0x00020000L, 
  0x08020208L, 0x00020008L, 0x08020000L, 0x00000208L,

  0x08000000L, 0x00000008L, 0x08020200L, 0x00000200L,
  0x00020200L, 0x08020000L, 0x08020008L, 0x00020208L,
  0x08000208L, 0x00020200L, 0x00020000L, 0x08000208L,
  0x00000008L, 0x08020208L, 0x00000200L, 0x08000000L,
  
  0x08020200L, 0x08000000L, 0x00020008L, 0x00000208L,
  0x00020000L, 0x08020200L, 0x08000200L, 0x00000000L,
  0x00000200L, 0x00020008L, 0x08020208L, 0x08000200L,
  0x08000008L, 0x00000200L, 0x00000000L, 0x08020008L,
  
  0x08000208L, 0x00020000L, 0x08000000L, 0x08020208L,
  0x00000008L, 0x00020208L, 0x00020200L, 0x08000008L,
  0x08020000L, 0x08000208L, 0x00000208L, 0x08020000L,
  0x00020208L, 0x00000008L, 0x08020008L, 0x00020200L
};

static unsigned int SP4[64] =
{
  0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
  0x00802080L, 0x00800081L, 0x00800001L, 0x00002001L,
  0x00000000L, 0x00802000L, 0x00802000L, 0x00802081L,
  0x00000081L, 0x00000000L, 0x00800080L, 0x00800001L,
  
  0x00000001L, 0x00002000L, 0x00800000L, 0x00802001L,
  0x00000080L, 0x00800000L, 0x00002001L, 0x00002080L,
  0x00800081L, 0x00000001L, 0x00002080L, 0x00800080L,
  0x00002000L, 0x00802080L, 0x00802081L, 0x00000081L,

  0x00800080L, 0x00800001L, 0x00802000L, 0x00802081L,
  0x00000081L, 0x00000000L, 0x00000000L, 0x00802000L,
  0x00002080L, 0x00800080L, 0x00800081L, 0x00000001L,
  0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
  
  0x00802081L, 0x00000081L, 0x00000001L, 0x00002000L,
  0x00800001L, 0x00002001L, 0x00802080L, 0x00800081L,
  0x00002001L, 0x00002080L, 0x00800000L, 0x00802001L,
  0x00000080L, 0x00800000L, 0x00002000L, 0x00802080L
};

static unsigned int SP5[64] =
{
  0x00000100L, 0x02080100L, 0x02080000L, 0x42000100L,
  0x00080000L, 0x00000100L, 0x40000000L, 0x02080000L,
  0x40080100L, 0x00080000L, 0x02000100L, 0x40080100L,
  0x42000100L, 0x42080000L, 0x00080100L, 0x40000000L,

  0x02000000L, 0x40080000L, 0x40080000L, 0x00000000L,
  0x40000100L, 0x42080100L, 0x42080100L, 0x02000100L,
  0x42080000L, 0x40000100L, 0x00000000L, 0x42000000L,
  0x02080100L, 0x02000000L, 0x42000000L, 0x00080100L,
  
  0x00080000L, 0x42000100L, 0x00000100L, 0x02000000L,
  0x40000000L, 0x02080000L, 0x42000100L, 0x40080100L,
  0x02000100L, 0x40000000L, 0x42080000L, 0x02080100L,
  0x40080100L, 0x00000100L, 0x02000000L, 0x42080000L,
  
  0x42080100L, 0x00080100L, 0x42000000L, 0x42080100L,
  0x02080000L, 0x00000000L, 0x40080000L, 0x42000000L,
  0x00080100L, 0x02000100L, 0x40000100L, 0x00080000L,
  0x00000000L, 0x40080000L, 0x02080100L, 0x40000100L
};

static unsigned int SP6[64] =
{
  0x20000010L, 0x20400000L, 0x00004000L, 0x20404010L,
  0x20400000L, 0x00000010L, 0x20404010L, 0x00400000L,
  0x20004000L, 0x00404010L, 0x00400000L, 0x20000010L,
  0x00400010L, 0x20004000L, 0x20000000L, 0x00004010L,
  
  0x00000000L, 0x00400010L, 0x20004010L, 0x00004000L,
  0x00404000L, 0x20004010L, 0x00000010L, 0x20400010L,
  0x20400010L, 0x00000000L, 0x00404010L, 0x20404000L,
  0x00004010L, 0x00404000L, 0x20404000L, 0x20000000L,
  
  0x20004000L, 0x00000010L, 0x20400010L, 0x00404000L,
  0x20404010L, 0x00400000L, 0x00004010L, 0x20000010L,
  0x00400000L, 0x20004000L, 0x20000000L, 0x00004010L,
  0x20000010L, 0x20404010L, 0x00404000L, 0x20400000L,

  0x00404010L, 0x20404000L, 0x00000000L, 0x20400010L,
  0x00000010L, 0x00004000L, 0x20400000L, 0x00404010L,
  0x00004000L, 0x00400010L, 0x20004010L, 0x00000000L,
  0x20404000L, 0x20000000L, 0x00400010L, 0x20004010L
};

static unsigned int SP7[64] =
{
  0x00200000L, 0x04200002L, 0x04000802L, 0x00000000L,
  0x00000800L, 0x04000802L, 0x00200802L, 0x04200800L,
  0x04200802L, 0x00200000L, 0x00000000L, 0x04000002L,
  0x00000002L, 0x04000000L, 0x04200002L, 0x00000802L,
  
  0x04000800L, 0x00200802L, 0x00200002L, 0x04000800L,
  0x04000002L, 0x04200000L, 0x04200800L, 0x00200002L,
  0x04200000L, 0x00000800L, 0x00000802L, 0x04200802L,
  0x00200800L, 0x00000002L, 0x04000000L, 0x00200800L,
  
  0x04000000L, 0x00200800L, 0x00200000L, 0x04000802L,
  0x04000802L, 0x04200002L, 0x04200002L, 0x00000002L,
  0x00200002L, 0x04000000L, 0x04000800L, 0x00200000L,
  0x04200800L, 0x00000802L, 0x00200802L, 0x04200800L,

  0x00000802L, 0x04000002L, 0x04200802L, 0x04200000L,
  0x00200800L, 0x00000000L, 0x00000002L, 0x04200802L,
  0x00000000L, 0x00200802L, 0x04200000L, 0x00000800L,
  0x04000002L, 0x04000800L, 0x00000800L, 0x00200002L
};

static unsigned int SP8[64] =
{
  0x10001040L, 0x00001000L, 0x00040000L, 0x10041040L,
  0x10000000L, 0x10001040L, 0x00000040L, 0x10000000L,
  0x00040040L, 0x10040000L, 0x10041040L, 0x00041000L,
  0x10041000L, 0x00041040L, 0x00001000L, 0x00000040L,
  
  0x10040000L, 0x10000040L, 0x10001000L, 0x00001040L,
  0x00041000L, 0x00040040L, 0x10040040L, 0x10041000L,
  0x00001040L, 0x00000000L, 0x00000000L, 0x10040040L,
  0x10000040L, 0x10001000L, 0x00041040L, 0x00040000L,
  
  0x00041040L, 0x00040000L, 0x10041000L, 0x00001000L,
  0x00000040L, 0x10040040L, 0x00001000L, 0x00041040L,
  0x10001000L, 0x00000040L, 0x10000040L, 0x10040000L,
  0x10040040L, 0x10000000L, 0x00040000L, 0x10001040L,
  
  0x00000000L, 0x10041040L, 0x00040040L, 0x10000040L,
  0x10040000L, 0x10001000L, 0x10001040L, 0x00000000L,
  0x10041040L, 0x00041000L, 0x00041000L, 0x00001040L,
  0x00001040L, 0x00040040L, 0x10000000L, 0x10041000L
};

static void desfunc(unsigned int* block, unsigned int* keys)
{
  unsigned int fval, work, right, left;
  int round;
  
  left = block[0];
  right = block[1];
  work = ((left >> 4) ^ right) & 0x0f0f0f0fL;
  right ^= work;
  left ^= (work << 4);
  work = ((left >> 16) ^ right) & 0x0000ffffL;
  right ^= work;
  left ^= (work << 16);
  work = ((right >> 2) ^ left) & 0x33333333L;
  left ^= work;
  right ^= (work << 2);
  work = ((right >> 8) ^ left) & 0x00ff00ffL;
  left ^= work;
  right ^= (work << 8);
  right = ((right << 1) | ((right >> 31) & 1L)) & 0xffffffffL;
  work = (left ^ right) & 0xaaaaaaaaL;
  left ^= work;
  right ^= work;
  left = ((left << 1) | ((left >> 31) & 1L)) & 0xffffffffL;
   
  for(round = 0; round < 8; round++)
  {
    work  = (right << 28) | (right >> 4);
    work ^= *keys++;
    fval  = SP7[ work        & 0x3fL];
    fval |= SP5[(work >>  8) & 0x3fL];
    fval |= SP3[(work >> 16) & 0x3fL];
    fval |= SP1[(work >> 24) & 0x3fL];
    work  = right ^ *keys++;
    fval |= SP8[ work        & 0x3fL];
    fval |= SP6[(work >>  8) & 0x3fL];
    fval |= SP4[(work >> 16) & 0x3fL];
    fval |= SP2[(work >> 24) & 0x3fL];
    left ^= fval;
    work  = (left << 28) | (left >> 4);
    work ^= *keys++;
    fval  = SP7[ work        & 0x3fL];
    fval |= SP5[(work >>  8) & 0x3fL];
    fval |= SP3[(work >> 16) & 0x3fL];
    fval |= SP1[(work >> 24) & 0x3fL];
    work  = left ^ *keys++;
    fval |= SP8[ work        & 0x3fL];
    fval |= SP6[(work >>  8) & 0x3fL];
    fval |= SP4[(work >> 16) & 0x3fL];
    fval |= SP2[(work >> 24) & 0x3fL];
    right ^= fval;
  }
  right = (right << 31) | (right >> 1);
  work = (left ^ right) & 0xaaaaaaaaL;
  left ^= work;
  right ^= work;
  left = (left << 31) | (left >> 1);
  work = ((left >> 8) ^ right) & 0x00ff00ffL;
  right ^= work;
  left ^= (work << 8);
  work = ((left >> 2) ^ right) & 0x33333333L;
  right ^= work;
  left ^= (work << 2);
  work = ((right >> 16) ^ left) & 0x0000ffffL;
  left ^= work;
  right ^= (work << 16);
  work = ((right >> 4) ^ left) & 0x0f0f0f0fL;
  left ^= work;
  right ^= (work << 4);
  *block++ = right;
  *block = left;
  return;
}

/* Validation sets:
 *
 * Single-length key, single-length plaintext
 * Key   : 0123 4567 89ab cdef
 * Plain : 0123 4567 89ab cde7
 * Cipher : c957 4425 6a5e d31d
 *
 **********************************************/
static void des_key(des_ctx *dc, unsigned char* key)
{
  deskey(key, EN0);
  cpkey(dc->ek);
  deskey(key,DE1);
  cpkey(dc->dk);
}

/* Encrypt several blocks in ECB mode. Caller is responsible for
   short  blocks. */
static void des_enc(des_ctx* dc, unsigned char* data, int blocks)
{
  unsigned int work[2];
  int i;
  unsigned char *cp;
  
  cp = data;
  for(i = 0; i < blocks; i++)
  {
    scrunch(cp, work);
    desfunc(work, dc->ek);
    unscrun(work, cp);
    cp += 8;
  }
}

static void des_dec(des_ctx* dc, unsigned char* data, int blocks)
{
  unsigned int work[2];
  int i;
  unsigned char *cp;
  
  cp = data;
  for(i = 0; i < blocks; i++)
  {
    scrunch(cp, work);
    desfunc(work, dc->dk);
    unscrun(work, cp);
    cp += 8;
  }
}  
  
/*
  与综合网接口：加密
  返回值：1 成功 0 失败
*/
int zhw_enc(char* sUserName, char* sPassWord)
{
  des_ctx dc;
  int i;
  unsigned char *cp, key[8] = "kxz72128";
  char s[41];
  char sOut[81];
  int nCount;
  int nLen;
  int nVal;
  
  nLen = strlen(sUserName);
  if(nLen > 30 || nLen == 0)
  {
    printf("\n用户名长度应在1-30之间!\n");
    return 0;
  }
  
  nCount = (nLen - 1) / 8 + 1;
  sprintf(s, "%-*s", nCount * 8, sUserName);

  cp = (unsigned char*)s;
  
  des_key(&dc, key);
  des_enc(&dc, cp, nCount);

  for(i = 0; i < nCount * 8; i++)
  {
    nVal = *(cp + i);
    if(nVal >= 0 && nVal <= 25) 
    {
      sOut[2 * i] = 'a';
      sOut[2 * i + 1] = 'a' + nVal;
    }
    else if(nVal >= 26 && nVal <= 51) 
    {
      sOut[2 * i] = 'b';
      sOut[2 * i + 1] = 'a' + nVal - 26;
    }
    else if(nVal >= 52 && nVal <= 77) 
    {
      sOut[2 * i] = 'c';
      sOut[2 * i + 1] = 'a' + nVal - 52;
    }
    else if(nVal >= 78 && nVal <= 103) 
    {
      sOut[2 * i] = 'd';
      sOut[2 * i + 1] = 'a' + nVal - 78;
    }
    else if(nVal >= 104 && nVal <= 129) 
    {
      sOut[2 * i] = 'e';
      sOut[2 * i + 1] = 'a' + nVal - 104;
    }
    else if(nVal >= 130 && nVal <= 155) 
    {
      sOut[2 * i] = 'f';
      sOut[2 * i + 1] = 'a' + nVal - 130;
    }
    else if(nVal >= 156 && nVal <= 181) 
    {
      sOut[2 * i] = 'g';
      sOut[2 * i + 1] = 'a' + nVal - 156;
    }
    else if(nVal >= 182 && nVal <= 207) 
    {
      sOut[2 * i] = 'h';
      sOut[2 * i + 1] = 'a' + nVal - 182;
    }
    else if(nVal >= 208 && nVal <= 233) 
    {
      sOut[2 * i] = 'i';
      sOut[2 * i + 1] = 'a' + nVal - 208;
    }
    else if(nVal >= 234 && nVal <= 255) 
    {
      sOut[2 * i] = 'j';
      sOut[2 * i + 1] = 'a' + nVal - 234;
    }
    else
    {
      printf("\n加密字符值不在0-255之间!\n");
      return 0;
    }
  }
  sOut[2 * nCount * 8] = '\0';
  sprintf(sUserEnv, "HZUSER=%s", sOut);
  if(putenv(sUserEnv))
  {
      printf("\n不能生成环境变量HZUSER!\n");
      return 0;
  }

  nLen = strlen(sPassWord);
  if(nLen > 30 || nLen == 0)
  {
    printf("\n口令长度应在1-30之间!\n");
    return 0;
  }
  
  nCount = (nLen - 1) / 8 + 1;
  sprintf(s, "%-*s", nCount * 8, sPassWord);

  cp = (unsigned char*)s;
  
  des_key(&dc, key);
  des_enc(&dc, cp, nCount);
  
  for(i = 0; i < nCount * 8; i++)
  {
    nVal = *(cp + i);
    if(nVal >= 0 && nVal <= 25) 
    {
      sOut[2 * i] = 'a';
      sOut[2 * i + 1] = 'a' + nVal;
    }
    else if(nVal >= 26 && nVal <= 51) 
    {
      sOut[2 * i] = 'b';
      sOut[2 * i + 1] = 'a' + nVal - 26;
    }
    else if(nVal >= 52 && nVal <= 77) 
    {
      sOut[2 * i] = 'c';
      sOut[2 * i + 1] = 'a' + nVal - 52;
    }
    else if(nVal >= 78 && nVal <= 103) 
    {
      sOut[2 * i] = 'd';
      sOut[2 * i + 1] = 'a' + nVal - 78;
    }
    else if(nVal >= 104 && nVal <= 129) 
    {
      sOut[2 * i] = 'e';
      sOut[2 * i + 1] = 'a' + nVal - 104;
    }
    else if(nVal >= 130 && nVal <= 155) 
    {
      sOut[2 * i] = 'f';
      sOut[2 * i + 1] = 'a' + nVal - 130;
    }
    else if(nVal >= 156 && nVal <= 181) 
    {
      sOut[2 * i] = 'g';
      sOut[2 * i + 1] = 'a' + nVal - 156;
    }
    else if(nVal >= 182 && nVal <= 207) 
    {
      sOut[2 * i] = 'h';
      sOut[2 * i + 1] = 'a' + nVal - 182;
    }
    else if(nVal >= 208 && nVal <= 233) 
    {
      sOut[2 * i] = 'i';
      sOut[2 * i + 1] = 'a' + nVal - 208;
    }
    else if(nVal >= 234 && nVal <= 255) 
    {
      sOut[2 * i] = 'j';
      sOut[2 * i + 1] = 'a' + nVal - 234;
    }
    else
    {
      printf("\n加密字符值不在0-255之间!\n");
      return 0;
    }
  }
  sOut[2 * nCount * 8] = '\0';
  sprintf(sPassEnv, "HZPASS=%s", sOut);
  if(putenv(sPassEnv))
  {
      printf("\n不能生成环境变量HZPASS!\n");
      return 0;
  }

  return 1;
}

void cleanup(int sig)
{
    int status;
    waitpid(-1,&status,WNOHANG);
    terminated=1;
}


/*
  与综合网接口：解密
  返回值：1 成功 0 失败
*/
int zhw_dec(char* sUserName, char* sPassWord)
{
  des_ctx dc;
  int i;
  unsigned char *cp, key[8] = "kxz72128";
  char s[41];
  int nCount;
  int nLen;
  int nVal;

  cp = (unsigned char*)getenv("HZUSER");
  if(cp == NULL)
  {
    return 0;
  }

  nLen = strlen((char*)cp);
  nCount = nLen / 8 / 2;
  if(nLen != nCount * 8 * 2)
  {
    printf("环境变量HZUSER非法!\n");
    return 0;
  }
  
  for(i = 0; i < nCount * 8; i++)
  {
    nVal = *(cp + 2 * i);
    if(nVal == 'a') 
    {
      s[i] = *(cp + 2 * i + 1) - 'a';
    }
    else if(nVal == 'b')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 26;
    }
    else if(nVal == 'c')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 52;
    }
    else if(nVal == 'd')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 78;
    }
    else if(nVal == 'e')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 104;
    }
    else if(nVal == 'f')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 130;
    }
    else if(nVal == 'g')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 156;
    }
    else if(nVal == 'h')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 182;
    }
    else if(nVal == 'i')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 208;
    }
    else if(nVal == 'j')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 234;
    }
    else
    {
      printf("\n加密字符非法!\n");
      return 0;
    }
  }
  s[nCount * 8] = '\0';
  
  cp = (unsigned char*)s;
  
  des_key(&dc, key);
  des_dec(&dc, cp, nCount);
  strcpy(sUserName, s);

  cp = (unsigned char*)getenv("HZPASS");
  if(cp == NULL)
  {
    return 0;
  }

  nLen = strlen((char*)cp);
  nCount = nLen / 8 / 2;
  if(nLen != nCount * 8 * 2)
  {
    printf("环境变量HZPASS非法!\n");
    return 0;
  }
  
  for(i = 0; i < nCount * 8; i++)
  {
    nVal = *(cp + 2 * i);
    if(nVal == 'a') 
    {
      s[i] = *(cp + 2 * i + 1) - 'a';
    }
    else if(nVal == 'b')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 26;
    }
    else if(nVal == 'c')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 52;
    }
    else if(nVal == 'd')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 78;
    }
    else if(nVal == 'e')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 104;
    }
    else if(nVal == 'f')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 130;
    }
    else if(nVal == 'g')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 156;
    }
    else if(nVal == 'h')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 182;
    }
    else if(nVal == 'i')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 208;
    }
    else if(nVal == 'j')
    {
      s[i] = *(cp + 2 * i + 1) - 'a' + 234;
    }
    else
    {
      printf("\n加密字符非法!\n");
      return 0;
    }
  }
  s[nCount * 8] = '\0';
  
  cp = (unsigned char*)s;
  
//  des_key(&dc, key);
  des_dec(&dc, cp, nCount);
  strcpy(sPassWord, s);

  return 1;
}    




