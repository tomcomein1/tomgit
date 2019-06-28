#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loadcups.h"

static char _StaticFileName[129]="\0";
/* 删除右空格 */
char *rtrim(char *str)
{
    /*指针指向字符中尾部*/
    long slen = strlen(str);

    while(slen--)
    {
        if (str[slen] != ' ' && str[slen] != '\t')
            break;

        str[slen] = '\0';
    }

    return str;
}

/*process OTFL,ITFL,ATFL**/
int process_tfl(char *s){
	const int units[]={
	11,6, 10, 19, 12,4, 6,4,8, 12,
 	2, 6, 11, 6,  2, 3, 12, 12, 12, 12,
  11, 19, 11, 19, 3, 1, 1, 1, 2, 10, 
  11, 1, 1, 19, 2, 1, 15,};
  struct t_cups_tfl tfl;
	
	 memset(&tfl, 0, sizeof(tfl));
   get_clear_file_str(s, units, ARRAY_SIZE(units), (void *) &tfl);
	 show_char_struct(units, ARRAY_SIZE(units), (void *)&tfl);
   return 0;
}

//打印字符数组的长度
int show_char_struct(const int units[], int array_len, void *src)
{
	 char *p=(char *)src;
	 char tmp[129];
   int i=0;
	
   for(i=0;i<array_len;i++){
			memset(tmp, 0, sizeof(tmp));
			memcpy(tmp, p, units[i]);
			rtrim(tmp);
			p=p+units[i]+1;
			#ifdef TEST
			printf("f%d[%d]:[%s]\n", i+1, strlen(tmp), tmp);
			#endif
			printf("%s|", tmp);
	 }
	 printf("\n");
	 return 0;
}

/**process COM**/
int process_com(char *s){
   const int units[]={
		11,11,6,10,19,12,12,12,4,6,
		4,8,15,12,2,6,11,6,2,3,
		12,12,12,1,3,1,1,10,11,1,
		2,2,12,14,
 };
   struct t_cups_com com;

   memset(&com, 0, sizeof(com));
   get_clear_file_str(s, units, ARRAY_SIZE(units), (void *) &com);
	 show_char_struct(units, ARRAY_SIZE(units), (void *)&com);
   return 0;
}

/**callback function, must modify this function **/
int process_errfile(char *s){
   const int units[]={
		3,11,11,6,10, 19,12,4,6,4,
		8,12,2,6,11, 11,6,2,3,12,
		12,12,12,12,12, 4,11,19,11,19,
		10,3,1,1,4, 12,1,2 };
   struct t_cups_err err;

   memset(&err, 0, sizeof(err));
   get_clear_file_str(s, units, ARRAY_SIZE(units), (void *) &err);
	 show_char_struct(units, ARRAY_SIZE(units), (void *)&err);

   /* HexDump((char *)&err, sizeof(struct str_errfile), (int)&err); */
   return 0;
}

int read_file_process(const char *filename, FP callfunc) {
	FILE *fp=NULL;
	char line[MAX_LINE_LEN];

	if( !(fp = fopen(filename, "r"))) {
		printf("open file error[%s]\n", filename);
		return -1;
	}

	while(!feof(fp)) {
		memset(line, 0, sizeof(line));
		if(fgets(line, MAX_LINE_LEN, fp)) {
			/* printf("[%s]", line); */
			callfunc(line);
		}
	}
	return 0;
}

/**解文件行 结构必需是字符串且按units中定义大小顺序**/
int get_clear_file_str(const char *line, const int units[], int usize, void *dest)
{
	char *p=dest;
	int i=0,l=0;

	if(line==NULL || line[0]=='\0') return -1;

	for(;i<usize; i++) {
		memcpy(p, line+l, units[i]);
		p=p+units[i]+1;
		*p = '\0';
		l=l+units[i]+1;
	}
	return 0;
}

void HexDump(char *buf,int len,int addr) {
    int i,j,k;
    char binstr[80];
 
    for (i=0;i<len;i++) {
        if (0==(i%16)) {
            sprintf(binstr,"%08x -",i+addr);
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
        } else if (15==(i%16)) {
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
            sprintf(binstr,"%s  ",binstr);
            for (j=i-15;j<=i;j++) {
                sprintf(binstr,"%s%c",binstr,('!'<buf[j]&&buf[j]<='~')?buf[j]:'.');
            }
            printf("%s\n",binstr);
        } else {
            sprintf(binstr,"%s %02x",binstr,(unsigned char)buf[i]);
        }
    }
    if (0!=(i%16)) {
        k=16-(i%16);
        for (j=0;j<k;j++) {
            sprintf(binstr,"%s   ",binstr);
        }
        sprintf(binstr,"%s  ",binstr);
        k=16-k;
        for (j=i-k;j<i;j++) {
            sprintf(binstr,"%s%c",binstr,('!'<buf[j]&&buf[j]<='~')?buf[j]:'.');
        }
        printf("%s\n",binstr);
    }
}

int help(const char *argv){
  return printf("\t%s <-e|-t|-c> <filename>\n", argv);
}

int main(int argc, char *argv[])
{
	if(argc==3) { 
		strcpy(_StaticFileName, argv[2]);
		printf("file:%s\n", _StaticFileName);
		if(memcmp(argv[1], "-e", 2)==0) {
		  read_file_process(argv[2], process_errfile);
		} else if(memcmp(argv[1], "-t", 2)==0){
		  read_file_process(argv[2], process_tfl);
		} else if (memcmp(argv[1], "-c", 2)==0){
			read_file_process(argv[2], process_com);
		}else {
			help(argv[0]);
		}
		
	} else {
		help(argv[0]);
	}
}

