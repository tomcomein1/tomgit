#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 2048
#define ARRAY_SIZE( ARRAY ) (sizeof (ARRAY) / sizeof (ARRAY[0]))

typedef int (*FP)(void*);    /*结构体表示函数指针 */

char *rtrim(char *str);
void HexDump(char *buf,int len,int addr);

struct str_errfile{
	char fd1[3+1];
	char fd2[11+1];
	char fd3[11+1];
	char fd4[6+1];
	char fd5[10+1]; 
	char fd6[19+1];
	char fd7[12+1];
	char fd8[4+1];
	char fd9[6+1];
	char fd10[4+1];
	char fd11[8+1];
	char fd12[12+1];
	char fd13[2+1];
	char fd14[6+1];
	char fd15[11+1];
	char fd16[11+1];
	char fd17[6+1];
	char fd18[2+1];
	char fd19[3+1];
	char fd20[12+1];
	char fd21[12+1];
	char fd22[12+1];
	char fd23[12+1];
	char fd24[12+1];
	char fd25[12+1];
	char fd26[4+1];
	char fd27[11+1];
	char fd28[19+1];
	char fd29[11+1];
	char fd30[19+1];
	char fd31[10+1];
	char fd32[3+1];
	char fd33[1+1];
	char fd34[1+1];
	char fd35[4+1];
	char fd36[12+1];
	char fd37[1+1];
	char fd38[2+1];
};

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

/**callback function, must modify this function **/
int process_errfile(void *s){
   const int units[]={
		3,11,11,6,10, 19,12,4,6,4,
		8,12,2,6,11, 11,6,2,3,12,
		12,12,12,12,12, 4,11,19,11,19,
		10,3,1,1,4, 12,1,2 };
   struct str_errfile err;

   /* printf("acct no|tran amt|old tran time|fee1|fee2|message type|txcode_fd3|fd18|brch_fd32|tmno_fd41|transysno_fd11|errmsg|\n"); */
   /** msgtype=0220 && txcode=200000 && errmsg=9707 th **/
   memset(&err, 0, sizeof(err));
   get_clear_file_str((char *)s, units, ARRAY_SIZE(units), (void *) &err);

   printf("%s|%10.2f|%s|%10.2f|%10.2f|%s|%s|%s|%s|%s|%s|%s|||\n", 
	err.fd6, atof(err.fd7)/100, err.fd31, atof(err.fd20)/100, atof(err.fd21)/100, 
	err.fd8, err.fd9, err.fd10, rtrim(err.fd2), err.fd11, err.fd4, err.fd26); 

   /* HexDump((char *)&err, sizeof(struct str_errfile), (int)&err); */
   return 0;
}

int read_file_process(const char *filename, FP callfunc) {
	FILE *fp=NULL;
	char line[MAX_LINE_LEN];
        int i=0;

	if( !(fp = fopen(filename, "r"))) {
		printf("open file error[%s]\n", filename);
		return -1;
	}

	while(!feof(fp)) {
		memset(line, 0, sizeof(line));
		if(fgets(line, MAX_LINE_LEN, fp)) {
			/* printf("[%s]", line); */
			callfunc((void*)line);
		}
	}
	return 0;
}

int get_clear_file_str(const char *line, const int units[], int usize, void *dest)
{
	char *p=dest;
	int i,l;

	l=0;
	for(i=0;i<usize; i++) {
		memcpy(p, line+l, units[i]);
		p=p+units[i]+1;
		*p = '\0';
		l=l+units[i]+1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	read_file_process(argv[1], process_errfile);
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
