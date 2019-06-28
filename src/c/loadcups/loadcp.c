#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 2048
#define ARRAY_SIZE( ARRAY ) (sizeof (ARRAY) / sizeof (ARRAY[0]))
char *rtrim(char *str);

const int err_units[]={
		3,11,11,6,10, 19,12,4,6,4,
		8,12,2,6,11, 11,6,2,3,12,
		12,12,12,12,12, 4,11,19,11,19,
		10,3,1,1,4, 12,1,2 };
const int com_units[]={
		11,11,6,10,19,12,12,12,4,6,
		4,8,15,12,2,6,11,6,2,3,
		12,12,12,1,3,1,1,10,11,1,
		2,2,12,14
 	};
const int tfl_units[]={
	11,6, 10, 19, 12,4, 6,4,8, 12,
 	2, 6, 11, 6,  2, 3, 12, 12, 12, 12,
  11, 19, 11, 19, 3, 1, 1, 1, 2, 10, 
  11, 1, 1, 19, 2, 1, 15};

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

/*打印字符数组的长度*/
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

/**根据传入格式长度及数组长度动态分配内存**/
int dynamic_memory(const int units[], const int array_len, char *s){
		void *mem=NULL;
		int i=0;
		int sum=0;
		
		for(i=0;i<array_len;i++){
			sum+=units[i];
			sum++;
		}
		mem=malloc(sum);
		if(mem==NULL){
			printf("ERROR memory!\n");
			return -1;
		}
		memset(mem, 0x00, sum);
		get_clear_file_str(s, units, array_len, mem);
		show_char_struct(units, array_len, mem);
		free(mem);
		mem=NULL;
		
		return 0;
}


/*new 动态调用函数*/
int read_file_process_new(const char *filename, const int units[], const int array_len) {
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
			dynamic_memory(units, array_len, line);
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
int help(const char *argv){
  return printf("\t%s <-e|-t|-c> <filename>\n", argv);
}

int main(int argc, char *argv[])
{
	if(argc==3) { 
		strcpy(_StaticFileName, argv[2]);
		printf("file:%s\n", _StaticFileName);
		if(memcmp(argv[1], "-e", 2)==0) {
		  read_file_process_new(argv[2], err_units, ARRAY_SIZE(err_units));
		} else if(memcmp(argv[1], "-t", 2)==0){
		  read_file_process_new(argv[2], tfl_units, ARRAY_SIZE(tfl_units));
		} else if (memcmp(argv[1], "-c", 2)==0){
			read_file_process_new(argv[2], com_units, ARRAY_SIZE(com_units));
		}else {
			help(argv[0]);
		}
		
	} else {
		help(argv[0]);
	}
}

