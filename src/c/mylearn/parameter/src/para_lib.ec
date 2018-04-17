#include "para_macro.h"

int WriteLog( char * fname, int line, char * fmt, ... )
{
    va_list args;
    FILE    *fp = NULL;
    char    str[ FILE_LEN+1 ];
    mode_t  mode;
    static struct timeval  tv ;
    static struct tm       *tp=NULL;

    gettimeofday (&tv, NULL );
    tp = (struct tm *)localtime( &tv.tv_sec );

    if( Plogdir[0] == 0x00 )
        sprintf( str , "%s/log/%s.log" , getenv("HOME") , "paraset" ) ;
    else
        sprintf( str , "%s/log/%s" , getenv("HOME") , Plogdir ) ;

    /* �Զ�����Ŀ¼ */
    if( access( (char *)dirname(str), F_OK ) != 0 ) {
        mode = umask(0);
        if(mkdirs(dirname(str), mode) != 0) {
           fprintf(stderr,"mkdir %s error errno=%d\n", dirname(str) , errno);
           return FAIL;
        }
        umask(mode);
    }

    fp = fopen ( str, "a");
    if ( ( fp == NULL ) && ( fp = stderr ) == NULL ) {
        fprintf(stderr,"Log File %s open Error!\n",str);
        return FAIL;
    }

    fprintf( fp, "[%-12.12s:% 4d %02d%02d%02d.%03d][% 8d]", fname, line, tp->tm_hour, tp->tm_min, tp->tm_sec, tv.tv_usec/1000,getpid() );
    va_start( args , fmt );
    vfprintf( fp, fmt, args);
    fprintf(fp, "\n" );
    va_end( args );

    if ( fp != stderr )
        fclose ( fp );

    return OK;
}

/*����Ŀ¼*/
int mkdirs(const char *path, mode_t mode)
{
  char tpath[ FILE_LEN+1 ];
  char *p = tpath;

  do{
      do
          *p++ = *path++;
      while ( (*path != '\0') && (*path != '/') );

    *p = '\0';
    if( (mkdir(tpath, (S_IRWXU|S_IRWXG|S_IRWXO)-mode) != 0) && (errno != EEXIST) )
        return FAIL;
  } while( *path!='\0' );

  return OK;
}

void print_time( char* tag )
{
    printf("%s\n", tag);
    return;
}

char * lower(char *str)
{
    long i, slen = strlen(str);
    for (i=0; (str[i] != '\0') && (i<=slen) ; i++)
    {
        if (str[i]>='A' && str[i]<='Z')
            str[i] |= 0x20;
    }

    return str;
}

/* ��дת�� */
char * upper(char *str)
{
	long i, slen = strlen(str);
	
	for (i=0; (str[i] != '\0') && (i<=slen) ; i++)
	{
		if (str[i]>='a' && str[i]<='z')
			str[i] &= 0x5F;
	}
	return str;
}

/* ɾ���ҿո� */
char *rtrim(char *str)
{
    /*ָ��ָ���ַ���β��*/
    long slen = strlen(str);
    
    while(slen--)
    {
        if (str[slen] != ' ' && str[slen] != '\t')
            break;

        str[slen] = '\0';
    }
    
    return str;
}

/*ɾ����ո�*/
char *ltrim(char * str)
{
	char * psource;
	char *pdest;
	
	psource = pdest = str;

	while (*psource == ' ' || *psource == '\t' ) psource++;

	if (psource != pdest)
	{
		while (*psource && psource) *pdest++ = *psource++;
		*pdest = '\0';
	}

	return str;
}

/*�ִ�������*/
char * rev_order_str(char *_source, char *_dest)
{
        int i = 0;
        int j = strlen(_source) - 1;

        while( j >= 0 )
                _dest[i++] = _source[j--];

        _dest[i] = '\0';

        return _dest;
}

/*���ָ���ȡ�ַ�������, �������µ��ִ�*/
char *get_str_usesep(char *_source, int _sep, char *_dest)
{
    char *p = _source;
    int i = 0;

    while(*p != '\0')
    {
        if (*p == _sep) break;
        _dest[i] = *p;
        i++;
        p++;
    }
    if (*p != '\0') p++;

    _dest[i] = '\0';

    return ( (*p!='\0') ? p: NULL);
}

/* ���ָ��ȡ�ַ����е����� n�ڼ����ָ������ b�ָ���� ��ͬ��[TOM] */
char* get_sub_str(char *source, char *dest, int c, int n)
{
	char *p = source;
	int i = 1;

	c = c ? c : ' '; /*Ĭ��Ϊ�ո�*/
	if (n > 1)
	{
		while(1)
		{
			if ((i==n) || (*p=='\0') || (*p=='\n') ) break;
			if (*p++ == c) i++;
		}
	}

	/*  ���n<=1������ȡ�ַ��� */
 	while ((*p!='\0') && (*p!=c) && (*p!='\n'))
 	        *dest++ = *p++;
 	
 	*dest = '\0';
 	
 	return ((*p) ? ++p : NULL);
}

/*************************************************************
  ����: ȡָ���ļ���ÿ������
  ���� 1 �ɹ�, 0 �� -1 ʧ��
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

/*���ָ���ȡ�ַ�������, �������µ��ִ�*/
char *get_mid_str(char *_source, int _s_sep, int _e_sep, char *_dest)
{
    char *p = _source;
    int i = 0;

    while(*p != '\0' && *p != _s_sep)
       p++;
    
    if (*p == _s_sep) p++;

    while(*p != '\0')
    {
        if (*p == _e_sep) break;
        
        _dest[i] = *p;
        i++;
        p++;
    }
    if (*p != '\0') p++;

    _dest[i] = '\0';

    return ( (*p!='\0') ? p: NULL);
}

/*����������ͬ�������͵�ֵ*/
int swap(void *x, void *y, int size)
{
        void *tmp=NULL;

        if ((tmp = malloc(size)) == NULL)
                return -1;
        memcpy (tmp, x, size);
        memcpy (x, y, size);
        memcpy (y, tmp, size);

        free(tmp);
        tmp = NULL;
        return 0;
}

char *get_xml_content(char *xml, char *tag, char *value)
{
     char *ph = NULL;
     char *pt = NULL;
     char tag_head[128];
     char tag_tail[128];
     int  len = 0;

     memset(tag_head, 0x00, sizeof(tag_head) );
     memset(tag_tail, 0x00, sizeof(tag_tail) );

     sprintf (tag_head, "<%s>", tag);
     sprintf (tag_tail, "</%s>", tag);

     ph = strstr(xml, tag_head);
     if (ph == NULL)
     {
         WriteLog( ERR_LVL, "in[%s] [%s] not found", xml,tag_head );
         value[0] = '\0';
         return NULL;
     }
     pt = strstr(ph, tag_tail);
     if (pt == NULL)
     {
        WriteLog( ERR_LVL, "in[%s] [%s] not found", xml, tag_tail );
        value[0] = '\0';
        return NULL;
     }

     len = strlen(tag_head);

     if (pt > ph)
     {
         strncpy(value, ph + len , (pt - ph) - len );
         value[ (pt-ph)-len ] = 0x00;
     }

     ph = NULL;
     pt = NULL;

     return value;

}

/*����ת������*/
char *para_iconv(char* from_charset, char* to_charset, char *inbuf) 
{
    size_t inlen;
    size_t outlen;

    char * in;
    char * out;
    char * outbuf;
    iconv_t cd;
    
    inlen = strlen(inbuf);
    WriteLog( ERR_LVL, "inlen[%d]", inlen );
    if(inbuf == NULL || inlen == 0)
    {
        WriteLog(ERR_LVL,"Ҫת����ַ���Ϊ�ջ򳤶�Ϊ0");
        return NULL;
    }
    
    cd = iconv_open(to_charset, from_charset);
    if(cd ==(iconv_t) -1)
    {
        WriteLog(ERR_LVL,"���ַ���(%s  %s)ʧ�ܣ�%s", to_charset, from_charset, strerror(errno));
        return NULL;
    }
    
    outlen = 2 * inlen - 1;     
    outbuf = (char *)malloc(outlen);
    if( outbuf == NULL )
    {
        WriteLog(ERR_LVL,"mallocʧ�ܣ�outbuf");
        return NULL;
    }
    memset(outbuf, 0x00, outlen);
    
    in = inbuf;
    out = outbuf;
    
    if( iconv(cd, &in, &inlen, &out, &outlen) != 0)     
    {
        iconv_close(cd);
        free(outbuf);
        WriteLog(ERR_LVL,"�ַ���ת��ʧ�ܣ�%s", strerror(errno));
        return NULL; 
    }
    
    iconv_close(cd);
    
    WriteLog( ERR_LVL, "������[%s]��[%s]ת���ɹ�", from_charset, to_charset );
    return outbuf;              
}

