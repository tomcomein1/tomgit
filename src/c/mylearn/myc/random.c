#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* ibuflen---需要生成的随即串的长度 */
int genrandstr(char* pszbuf,int ibuflen)
{
    static const char sourchar[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    srand((unsigned)time(NULL));    
    for(int i=0;i<ibuflen;i++)
    {
        int x = rand()/(RAND_MAX/(sizeof(sourchar)-1));
        pszbuf[i] = sourchar[x];
    }
    pszbuf[ibuflen] = '\0';
}

/* 引入干扰因子，防止在同一秒内调用产生相同的结果 */
int genrandstr1(char* pszbuf,int ibuflen,const char* pszgene)
{
    static const char sourchar[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned unstart = time(NULL);
    while(*pszgene)
    {
        unstart += *pszgene++;
    }
    srand(unstart);    
    for(int i=0;i<ibuflen;i++)
    {
        int x = rand()/(RAND_MAX/(sizeof(sourchar)-1));
        pszbuf[i] = sourchar[x];
    }
    pszbuf[ibuflen] = '\0';
}


int main(int argc, char* argv[])
{
    printf("begin randstr,RAND_MAX=%d\n",RAND_MAX);
    char buf[9] = {0};
    genrandstr1(buf,8,"13893272826");
    printf("randstr=%s\n",buf);    
    genrandstr1(buf,8,"15213997368");
    printf("randstr=%s\n",buf);
    genrandstr1(buf,8,"18919312222");
    printf("randstr=%s\n",buf);
    genrandstr1(buf,8,"13893277645");
    printf("randstr=%s\n",buf);
    genrandstr1(buf,8,"changyeerer");
    printf("randstr=%s\n",buf);
    genrandstr1(buf,8,"fahfhhe;ll;d");
    printf("randstr=%s\n",buf);
    printf("end randstr\n");
    return 0;
}

