#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN 2048

extern char* get_sub_str(char *source, char *dest, int c, int n);
extern char *get_str_usesep(char *_source, int _sep, char *_dest);

int XLINE=0;

int main(int argc, char *argv[])
{
	int i=0;

   /**   printf("This is a resolve programe!\n");  
   for(i=0; i<argc; i++) {
	printf("%d:%s\n", i, argv[i]);
   } 
   ***/

	if (argc == 2) {
		process(argv[1], "|");
	} else if (argc==3) {
		process(argv[1], argv[2]);
	} else if (argc==4) {
                XLINE=atol(argv[3]);
                process(argv[1], argv[2]);
        } else {
		printf("%s <filename> [D|G|dilimiter] [line]\n", argv[0]);
	}
	return 0;
}

int process(char *filename, char *sep)
{
	FILE *fp = NULL;
	char line[MAX_STR_LEN];
	char buf[MAX_STR_LEN];
	char newline[MAX_STR_LEN];
	char *p=NULL;
        int row = 1;

	if((fp=fopen(filename, "r"))==NULL) {
		printf("open file [%s] error!\n", filename);
		return -1;
	}

	while(1) {
		memset(line, 0, sizeof(line));
		memset(newline, 0, sizeof(newline));
		if(get_line(line, fp, '\n')<=0) break;

                if(row==XLINE) {
                   if(sep[0]=='D') {
                      row++;
                      continue;
                   } else if(sep[0]=='G') {
                      printf("[%ld]: %s", XLINE,line);
                      break;
                   }
                }
                row++;
                if(sep[0]=='D') {
                   printf("%s", line);
                   continue;
                } else if (sep[0]=='G') continue;

		memset(buf, 0, sizeof(buf));
		p=get_str_usesep(line, ',', buf);
		strcat(newline, buf);
		strcat(newline, sep);
		while(p!=NULL) {
			p=get_str_usesep(p, ',', buf);
			strcat(newline, buf);
			strcat(newline, sep);
		}
		printf("%s\n", newline);
	}

	fclose(fp);
	return 0;
}


int test_usesep()
{
	char str[100]="2524,1,\"00001\",\"50001,aaa\"";
	char dd[50]="\0";
	char *p = NULL;
	int i=1;

	printf("%s\n", str);
	memset(dd, 0, sizeof(dd));
	p=get_str_usesep(str, ',', dd);
	printf("%d: %s\n", i++, dd);
	while(p != NULL) {
		p=get_str_usesep(p, ',', dd);
		printf("%d: %s\n", i++, dd);
	}

	return 0;
}

/*************************************************************
  功能: 取指定文件中每行数据
  返回 1 成功, 0 或 -1 失败
**************************************************************/
int get_line(char * sline, FILE * fp, int endch)
{
	int ch=0;
	long nRec=0;

	if (endch == 0) endch = '\0';

	while(ch!=endch && ch!=EOF)
	{
		if (nRec>=MAX_STR_LEN) break;
		ch=getc(fp);
		sline[nRec++]=ch;
	}

	if(nRec>=MAX_STR_LEN) return -1;

	sline[nRec]='\0';
	if(ch=='\n') return 1;
	if(ch==EOF)  return 0;

	return -1;
}

/* 按分割符取字符串中的数据 n第几个分割符数据 b分割符号 ，同上[TOM] */
char* get_sub_str(char *source, char *dest, int c, int n)
{
	char *p = source;
	int i = 1;

	c = c ? c : ' '; /*默认为空格*/
	if (n > 1)
	{
		while(1)
		{
			if ((i==n) || (*p=='\0') || (*p=='\n') ) break;
			if (*p++ == c) i++;
		}
	}

	/*  如果n<=1，依次取字符串 */
	while ((*p!='\0') && (*p!=c) && (*p!='\n'))
		*dest++ = *p++;

	*dest = '\0';

	return ((*p) ? ++p : NULL);
}

/*按分隔符取字符串操作, 返回余下的字串*/
char *get_str_usesep(char *_source, int _sep, char *_dest)
{
	char *p = _source;
	int i = 0;
	int dilimiter=_sep;

	if(*p == '\"') {
		*p++;
		dilimiter='\"';
	}

	while(*p != '\0')
	{
		if (*p == dilimiter) break;
		_dest[i] = *p;
		i++;
		p++;
	}
	if (*p != '\0') p++;
	if (*p == _sep) p++;
	if (*p == '\n') p++;

	_dest[i] = '\0';

	return ( (*p!='\0') ? p: NULL);
}

