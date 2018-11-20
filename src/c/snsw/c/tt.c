#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long pubf_int_hex2str(unsigned char *hex);

int main() {
  int A=1024;
  int B=2048;
  char fd125[]="QR0592312018110517055400025100250001   90880023   2900";
  char fd104[]="SI0506226123456789123张三                          BI00401";
  char d[100];
  char dest[20][128];
  int i =0;
   
  memset(&dest, 0x00, 20*128);

  get_sub_fdinfo(fd125, dest, "QR");
  /* get_sub_fdinfo(fd104, dest, "SI"); */
  for(i=0; i<20; i++) {
    printf("%02d=[%s]\n", i+1, dest[i]);
  }
/**
  printf("A=%d B=%d\n", A, B);
  printf("AVA=%d, A^A=%d\n", A|A, A&A );
  printf("AVB=%d=BVA=%d, A^B=%d=B^A=%d\n", A|B, B|A, A&B, B&A);
  printf("德.摩根律: ~(A|B)=%d=~A^~B=%d\n", ~(A|B), ~A&~B);
  printf("德.摩根律: ~(A&B)=%d=~AV~B=%d\n", ~(A&B), ~A|~B);
**/

}

/**取相应子域内容**/
int get_sub_fdinfo(unsigned char *source, char dest[20][128], const char *TAG) 
{
    char *sp=NULL;
    char *sp_tag=NULL;
    char sTmp[1024]="\0";
    char buf[100]="\0";
    char tag[3];
    int len=0;
    int i=0;
    
    memset(sTmp,0,sizeof(sTmp));
    sp_tag=strstr(source, TAG);
    if(sp_tag==NULL) {
       printf("not found tag[%s]\n", TAG);
       return 1;
    }
/*    printf("find:[%s]\n", sp_tag); */
    pubf_str_str2hex(sp_tag+strlen(TAG)+3,sTmp,strlen(sp_tag)-strlen(TAG)-3);
    sp=sTmp; 

    for(i=0; i<20; i++) {
      sprintf(tag, "%02d", i+1);
      /* printf("tag=%s\n", tag); */
      if (!sp) break;
      if (strncmp(sp, tag, 2)== 0)  {
         memset(buf, 0, sizeof(buf));
         len=get_sub_fd_str(sp, buf);
         sp+=len;
         memcpy(dest[i], buf, sizeof(buf));
      }
   }

}

int get_sub_fd_str(char *str, char *out) 
{
    char tmp[100];
    int len=0;

    str+=2;
    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, str, 2);
    len=pubf_int_hex2str(tmp); 
/* printf("len=[%d]\n", len); */

    str+=2;
    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, str, len*2);
    pubf_str_hex2str(tmp, out, sizeof(tmp));
    str+=len*2;

/* printf("out=[%s]\n", out); */
    return (len*2+4); 
}

int pubf_str_str2hex(unsigned char *str, unsigned char *hex, int str_len)
{
      for (int i = 0; i < str_len; ++i)
      {
              sprintf(hex + 2 * i, "%02X", str[i]);
      }
      return 0;
}
int pubf_str_hex2str(unsigned char *hex, unsigned char *str, int str_len)
{
       unsigned char tmp_buf[3] = {0};
       for (int i = 0; i < str_len; ++i)
       {
               memcpy(tmp_buf, hex + i * 2, 2);
               str[i] = strtol(tmp_buf, NULL, 16);
       }
       return 0;
}

long pubf_int_hex2str(unsigned char *hex)
{

        int i,t;
        long sum=0;
        for(i=0;hex[i];i++)
        {
                if(hex[i]<='9')
                {
                        t=hex[i]-'0';
                }else if(hex[i] >= 'a' && hex[i] <= 'z'){
                        t=hex[i]-'a'+10;
                }else if(hex[i] >= 'A' && hex[i] <= 'Z'){
                        t=hex[i]-'A'+10;
                }
                sum=sum*16+t;
        }
        return sum;
}
