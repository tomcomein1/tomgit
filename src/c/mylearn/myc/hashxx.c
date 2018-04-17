/* hash.h */
#ifndef _HASH_H_
#define _HASH_H_
 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
 
#define HASH_GENE 4
#define OVER_ (65535*65535)
 
int hash(char *key_str, unsigned long MAX_SS_NUM);
 
#endif //_HASH_H_

/* hash.c */
#include "hash.h"
 
int hash(char *key_str, unsigned long MAX_SS_NUM)
{
    register unsigned int h;
    register unsigned char *p;
 
    for(h=0, p = (unsigned char *)key_str; *p ; p++)
    {
        h = 31 * h + *p;
    }
 
    return (h % MAX_SS_NUM);
}
