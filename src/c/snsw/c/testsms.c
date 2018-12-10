#include "sms_http.h"

int process_recv_info(char *rcvbuf, int link_num)
{
	char temp[BASESIZE*4];
	char buf[BASESIZE*4];
	char *p=NULL;
	int ret=0;

	if(check_http_status(rcvbuf)) {
		fprintf(stderr, "http error!\n");
		return -1;
	}

	memset(temp, 0, sizeof(temp));
	process_return_pkg(rcvbuf, temp); 

	if(strstr(temp, "OK") != NULL) {
		if(link_num == 0) {
			printf("[%s][%d]recv message:[%d][%s]",__FILE__,__LINE__,strlen(temp), temp);  
			return 0;
		}
	}

	memset(buf, 0, sizeof(buf));
	ret=convert_buffer(temp, buf, 1);
	if (ret) {
		fprintf(stderr, "1000error iconv false!\n");
		return -1;
	}

	printf("[%s][%d]recv message:[%d][%s]",__FILE__,__LINE__,strlen(buf), buf);  
	if(strstr(buf, "OK") == NULL) {
		return -1;
	}
	/**处理后续解出来的数据**/
	/*0,OK\r\n 18502823038,7349,0,处理成功\r\n*/
        parse_out_info(buf); 
	return 0;
}

int parse_out_info(char *outbuf)
{
	/* char outbuf[]={"0,OK\r\n18502823038,7349,0,处理成功\r\n13709004191,7359,100,处理失败\r\n"}; */
	char line[1024];
	char buf[4][20];
	char *p=outbuf;
	int  j=0;
	int  i=0;

	/* printf("buf:%s\n", p); */
	memset(buf, 0, sizeof(buf));
	while(p) {
		memset(line, 0, sizeof(line));
		p= get_sub_str(p, line, ',' , 1);
		i++;
		if (i<3) continue;
		strcpy(buf[j], line);

/*		printf("[%d]%s\n", j, buf[j]); */
		j++;
		if (j==4) {
			process_out_info(buf);
			j=0;
		}
	}
}

int process_out_info(char buf[4][20])
{
	int i=0;
	if(strcmp(buf[2], "0")==0) {
		return 0;
	}
	for(; i<4; i++) {
		printf("[%d]%s\n", i, buf[i]);
	}
}

int test_sms(int link){
	int length, ret;
	char msg[100]="这是一个测试短信";
	char temp[400]={
		0	};
	char s[1024];
	char rcvbuf[BASESIZE*4];

	memset(s, 0, sizeof(s));
	set_pkg_public(s);

	if(link==0) {
		set_pkg_message(s, "msgid", "00000240");
		/* set_pkg_message(s, "mobile", "18502823038,13301898676");*/
		set_pkg_message(s, "mobile", "18502823038");

		ret=convert_buffer(msg, temp, 0);
		if(ret) {
			fprintf(stderr, "error iconv false!\n");
			return -1;
		}
		set_pkg_message(s, "msg", php_url_encode(temp, strlen(temp), &length));
	}

	memset(rcvbuf, 0, sizeof(rcvbuf));
	ret=newsms_http_cli((void*)s, link, rcvbuf);
	if(!ret) {
		ret=process_recv_info(rcvbuf, link);
	}

	return ret;
}

int test_moblst(int mobcnt, char *moblst, char *content)
{
	int i=0;
	char phone_no[100];

	for(int i=1;i<=mobcnt; i++){
		memset(phone_no, 0, sizeof(phone_no));
		get_sub_str(moblst, phone_no, ',' , i);
		printf("[%d][%s][%s]\n", i, phone_no, content);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char outbuf[]={
		"0,OK\r\n18502823038,7349,0,处理成功\r\n13709004191,7359,100,处理失败\r\n"	};
	
	/* test_moblst(atoi(argv[1]), argv[2], argv[3]); */
	/* parse_out_info(outbuf); */
	printf("sms test start...[%s]\n", argv[1]);

	test_sms(atoi(argv[1]) );
	return 0;
}

