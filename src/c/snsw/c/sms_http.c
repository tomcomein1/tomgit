#include "sms_http.h"

static const char *LINKS[20]={"/sms/send", "/sms/receive", "/sms/report", "/sms/test.php", "\0"};
const char END[]={"\r\n"};
const char END_HEADER[]={"\r\n\r\n"};

int newsms_http_cli(void *sndbuf, int link_num, char *retbuf)
{
	int sockfd=0;
	int ret=0, h, i, times, length, iLen;
	char str1[BASESIZE*4];
	char rcvbuf[BASESIZE*4];
	char temp[BASESIZE*2];
	fd_set t_set1;
	struct timeval  tv;
        char *p=(char *)sndbuf;
        
	printf("http_cli start...[%s]\n", p);

	/* sockfd=connectsock(SVR_ADDR, SVR_PORT, "tcp"); */
	memset(str1, 0, sizeof(str1));
	set_http_header(str1, LINKS[link_num], strlen(p), "POST");
	/** set body **/
	strcat(str1, p);

	sockfd=connectsock(SVR_ADDR, SVR_PORT, "tcp");
	printf("connect sockfd=%d\n", sockfd);
	if(sockfd<0) {
		printf("connect socket error!\n");
		exit(-1);
	}

	times=0;
        length=strlen(str1);
        p=str1; 
	printf("send[%d]\n%s\n", strlen(p), p);
	iLen=0;
	while(1) {
		ret= send(sockfd, p+iLen, length-iLen, 0);
		if(ret<0)
		{
			if(errno=EINTR){
				continue;
			}
			return(ret);
		} else if(ret==0) {
			times++;
			if(times>3) {
				printf("[%s][%d]连接次数过多， error [%d]",__FILE__,__LINE__, errno);
				close(sockfd);
				return -1;
			}
		} else{
			iLen+=ret;
			if(iLen>=length) {
				break;
			}
		}
	}

	FD_ZERO(&t_set1);
	FD_SET(sockfd, &t_set1);
	memset(&tv, 0, sizeof(tv));
	tv.tv_sec=TIMEOUT_SECOND;

	memset(rcvbuf, 0, sizeof(rcvbuf));
	iLen=0;
	while(1) {
		FD_ZERO(&t_set1);
		FD_SET(sockfd, &t_set1);

		ret= select(sockfd +1, &t_set1, NULL, NULL, &tv);
		if(ret<0)
		{       /*时钟中断*/
			if(errno==EINTR)
			{
				fprintf(stderr, "[%s][%d] clock interrupt",__FILE__,__LINE__);
				continue;
			} else
			{
				fprintf(stderr,"select error [%s][%d][%d]",__FILE__,__LINE__,errno);

				exit(1);
			}
		} else if(ret==0) {
			/*等待超时*/
			fprintf(stderr,"%s,%d,TIMEOUT!",__FILE__,__LINE__);
			close(sockfd);
			return -2;
		}

		memset(temp, 0, sizeof(temp));
		ret = recv(sockfd, temp, sizeof(temp), 0);
/* fprintf(stdout, "temp[%d]=%s\n", ret, temp); */
		if(ret<0)
		{
			fprintf(stderr,"[%s][%d]CbmConnect error [%d]",__FILE__,__LINE__, errno);
			close(sockfd);
			return -1;
		}
		if(ret == 0)
		{
			fprintf(stdout,"%s,%d, Recv OK!\n",__FILE__,__LINE__);
			break;
		} 

		iLen+=ret;
		if(iLen<=sizeof(temp) && iLen== strlen(rcvbuf) ) break;

		memcpy(rcvbuf+strlen(rcvbuf), temp, ret);
		printf("iLen=%d len=[%d]\n", iLen, strlen(rcvbuf)); 
	}
	close(sockfd);

	/* printf("[%s][%d] 接收到的报文rcvbuf[%d]\n[%s]",__FILE__,__LINE__,strlen(rcvbuf), rcvbuf); */

	de_chunked(rcvbuf, iLen, retbuf); 

	return 0;
}

int check_http_status(char *http)
{
   char *p=NULL;
   p=strstr(http, "HTTP/1.1");
   if(strncmp(p+9, "200", 3) == 0) return 0;
   else return -1;
}

int process_return_pkg(char *recv, char *dest) {
     char *p=NULL;
     char tmp[10]={0}; 
     int len=0;
     const char findstr[]="Content-Length:";
 
     if (p=strstr(recv, CHUNKED)) {
         p=strstr(recv, END_HEADER);
         strcpy(dest, p+strlen(END_HEADER)+strlen(END));
     }
     else {
         p=strstr(recv, findstr);
         if(p) {
            strncpy(tmp, p+strlen(findstr), sizeof(tmp)); 
            len=atoi(tmp);
            strcpy(dest, recv+strlen(recv)-len);  
         }
     }
     return 0;
}

int set_http_header(char *header,const char *url, const int n, const char *method )
{
	sprintf(header+strlen(header), "%s %s HTTP/1.1\r\n", method, url);
	sprintf(header+strlen(header), "Host: %s:%d\r\n", SVR_ADDR, SVR_PORT);
	strcat(header, "Content-Type: application/x-www-form-urlencoded;charset=utf-8 \r\n"); 
	/* strcat(header, "Content-Type: text/plain;charset=utf-8 \r\n"); */
	/**length**/
	sprintf(header+strlen(header), "Content-Length: %d\r\n", n);
	sprintf(header+strlen(header), "Connection: close \r\n");
	strcat(header, "\r\n");
	return 0;
}

/*编码转换程序*/
char *para_iconv(char* from_charset, char* to_charset, char *inbuf) 
{
    size_t inlen;
    size_t outlen;

    char * in;
    char * out;
    char * outbuf;
    iconv_t cd;
    
    inlen = strlen(inbuf);
/*    printf("inlen[%d]", inlen ); */
    if(inbuf == NULL || inlen == 0)
    {
        fprintf(stderr,"要转码的字符串为空或长度为0");
        return NULL;
    }
    
    cd = iconv_open(to_charset, from_charset);
    if(cd ==(iconv_t) -1)
    {
        fprintf(stderr,"打开字符集(%s  %s)失败：%s", to_charset, from_charset, strerror(errno));
        return NULL;
    }
    
    outlen = 2 * inlen - 1;     
    outbuf = (char *)malloc(outlen);
    if( outbuf == NULL )
    {
        fprintf(stderr,"malloc失败：outbuf");
        return NULL;
    }
    memset(outbuf, 0x00, outlen);
    
    in = inbuf;
    out = outbuf;
    
    if( iconv(cd, &in, &inlen, &out, &outlen) != 0)     
    {
        iconv_close(cd);
        free(outbuf);
        fprintf(stderr,"字符集转换失败：%s", strerror(errno));
        return NULL; 
    }
    
    iconv_close(cd);
    
/*    fprintf(stdout, "报文自[%s]到[%s]转换成功", from_charset, to_charset ); */
    return outbuf;              
}


static unsigned char hexchars[] = "0123456789ABCDEF";

static int php_htoi(char *s)
{
	int value;
	int c;

	c = ((unsigned char *)s)[0];
	if (isupper(c))
		c = tolower(c);
	value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

	c = ((unsigned char *)s)[1];
	if (isupper(c))
		c = tolower(c);
	value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

	return (value);
}

char *php_url_encode(char const *s, int len, int *new_length)
{
	register unsigned char c;
	unsigned char *to, *start;
	unsigned char const *from, *end;
	
	from = (unsigned char *)s;
	end  = (unsigned char *)s + len;
	start = to = (unsigned char *) calloc(1, 3*len+1);

	while (from < end) 
	{
		c = *from++;

		if (c == ' ') 
		{
			*to++ = '+';
		} 
		else if ((c < '0' && c != '-' && c != '.') ||
				 (c < 'A' && c > '9') ||
				 (c > 'Z' && c < 'a' && c != '_') ||
				 (c > 'z')) 
		{
			to[0] = '%';
			to[1] = hexchars[c >> 4];
			to[2] = hexchars[c & 15];
			to += 3;
		}
		else 
		{
			*to++ = c;
		}
	}
	*to = 0;
	if (new_length) 
	{
		*new_length = to - start;
	}
	return (char *) start;
}


int php_url_decode(char *str, int len)
{
	char *dest = str;
	char *data = str;

	while (len--) 
	{
		if (*data == '+') 
		{
			*dest = ' ';
		}
		else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2))) 
		{
			*dest = (char) php_htoi(data + 1);
			data += 2;
			len -= 2;
		} 
		else 
		{
			*dest = *data;
		}
		data++;
		dest++;
	}
	*dest = '\0';
	return dest - str;
}

/**查找替换字符串**/
void StringReplace(char *pszInput, char *pszOld, char *pszNew,char *pszOutput,int nOutputlen)
{
        int nLen=0;
        char *s, *p;
        s = pszInput;
        while (s != NULL)
        {
                p = strstr(s, pszOld);

                if (p == NULL )
                {
                        memcpy(pszOutput+nLen,s,strlen(s)+nLen>nOutputlen?nOutputlen-nLen:strlen(s));
                        return ;
                }
                memcpy(pszOutput+nLen,s,p-s+nLen>nOutputlen?nOutputlen-nLen:p-s);
                nLen+=p-s+nLen>nOutputlen?nOutputlen-nLen:p-s;
                if(nLen>=nOutputlen)
                {
                        return;
                }
                memcpy(pszOutput+nLen,pszNew,strlen(pszNew)+nLen>nOutputlen?nOutputlen-nLen:strlen(pszNew));
                nLen+=strlen(pszNew)+nLen>nOutputlen?nOutputlen-nLen:strlen(pszNew);
                if(nLen>=nOutputlen)
                {
                        return;
                }
                s+=strlen(pszOld)+p-s;
        }
        return ;
}

/** 客户端建立socket连接 返回socket值 **/
int connectsock(const char *host, const int service, const char *transport )
{
	struct sockaddr_in sin;
	int s, type;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = PF_INET;
	sin.sin_port = htons(service);
	sin.sin_addr.s_addr = inet_addr(host);

	/* Use protocol to choose a socket type */
	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

	/* Allocate a socket */
	s = socket(sin.sin_family, type, 0);
	if (s < 0)
	{
		fprintf(stderr, "can't create socket: %s", strerror(errno));
		return -1;
	}

	/* Connect the socket */
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		fprintf(stderr, "不能连接到%s.%s: %s", host, service, strerror(errno));
		return -1;
	}

	return s;
}

int set_pkg_public(char *dest)
{
	return sprintf(dest+strlen(dest), "user=%s&pass=%s&api=%s&code=0&", USER,PASS,USER);
}

int set_pkg_message(char *dest, char *key, char *value)
{
	return sprintf(dest+strlen(dest), "%s=%s&", key, value);
}

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

/**0:gbk to utf-8 1:utf-8 to gbk**/
int convert_buffer(char *src, char *dest, int flag){
     char *p=NULL;
     if(!flag) {
        p=para_iconv("GBK", "UTF-8", src);
     } else { 
        p=para_iconv("UTF-8", "GBK", src);
     }
     if(p==NULL) return -1;
     strcpy(dest, p);
     free(p);p=NULL;
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

int hex2int(unsigned char *hex)
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


int de_chunked(char *s, int len, char *dest)
{
     char *p=NULL;
     char *pstart=NULL;
     char *pend=NULL;
     char lhex[10]={0};
     char stemp[BASESIZE*4];
     char strs[BASESIZE*4];
     int  ilen=0;

     if(strstr(s, CHUNKED) == NULL) {
	memcpy(dest, s, strlen(s));
	return 0;
     }

     p=strstr(s, END_HEADER);
     if(p==NULL) {
        fprintf(stderr, "ERROR: not found header!\n");
	return -1;
     }
     p = p + strlen(END_HEADER);
     
     memcpy(dest, s, p-s);

do{
     pstart = p;

     pend=strstr(pstart, END); 
     if(pend==NULL) {
        fprintf(stderr, "ERROR: not found pend!\n");
	return -1;
     }

     memset(lhex, 0, sizeof(lhex));
     memcpy(lhex, pstart, pend-pstart);
     ilen=hex2int(lhex);
/*     fprintf(stdout, "lhex=[%s] ilen=[%d] p[%d]\n", lhex, ilen, strlen(p)); */

     if (ilen==0) break;
     memset(stemp, 0, sizeof(stemp));
     memcpy(stemp, pend, ilen);
/*     convert_buffer(stemp, strs, 1);  */
     strcat(dest, stemp);
     
/*   HexDump(stemp, strlen(stemp), (int)stemp); */
     
     p=pend+strlen(END);

/*   HexDump(p, strlen(p), (int)p); */

     p=p+ilen;
     p=p+strlen(END);
}while(p && *p!='\0');

/* printf("%s\n", dest); */

     return 0;
}

