/*
    filename: get_date_time.c
    function: 取当前系统日期时间，
              按fmt指定字串格式
              [y|Y - 定义年
               m - 定义月
               d|D - 定放日
               H|h - 定义小时
               M - 定义分钟
               S|s - 定义秒
               ]
               其它分隔符原样输出
*/
#include<time.h>

void get_date_time(date_time, fmt)
char	*date_time;
const char *fmt;
{
 	long	tbuf;
 	struct tm *t;
	unsigned char c ;
	char format_time[30];
    const char format[] = "yyyymmddHMS";
    
	memset(format_time, 0x00, sizeof(format_time) );

 	time(&tbuf);
 	t = (struct tm *)localtime(&tbuf);

    if (*fmt == NULL)
	    fmt = format;

    /*解读成指定格式*/
	while(c=*fmt)
	{
		switch(c)
		{
			case 'y':
			case 'Y':
				sprintf(format_time + strlen(format_time), "%4.4d", t->tm_year + 1900);
				break;
			case 'm':
				sprintf(format_time + strlen(format_time), "%2.2d", t->tm_mon + 1);
				break;
			case 'd':
			case 'D':
				sprintf(format_time + strlen(format_time), "%2.2d", t->tm_mday);
				break;
			
			case 'H':
			case 'h':
				sprintf(format_time + strlen(format_time), "%2.2d", t->tm_hour);
				break;
			case 'M':
				sprintf(format_time + strlen(format_time), "%2.2d", t->tm_min);
				break;
			break;
			case 'S':
			case 's':
				sprintf(format_time + strlen(format_time), "%2.2d", t->tm_sec);
				break;
			
			default:
				sprintf(format_time + strlen(format_time), "%c", c ); 
				break;
		} /* end switch*/
		fmt++;
		while (c == *fmt) fmt++;

	} /*end while*/

	strcpy(date_time, format_time);

	return;
}


main()
{
	char str_dt[20];
	
	/*1. yymmddHMS */
	get_date_time(str_dt, "");
	printf ("Now date time is %s\n", str_dt);

	/*2. yy-mm-dd H:M:S */
	get_date_time(str_dt, "yyyy-mm-dd HH:M:S");
	printf ("fmt[yyyy-mm-dd HH:M:S] time is %s\n", str_dt);

	/*3. yy/mm/dd H:M:S */
	get_date_time(str_dt, "yy/mm/dd H:M:S");
	printf ("fmt[yy/mm/dd H:M:S] time is %s\n", str_dt);

	/*4. mm/dd/yy */
	get_date_time(str_dt, "m/d/Y");
	printf ("fmt[m/d/Y] time is %s\n", str_dt);

	/*4. H:M:S */
	get_date_time(str_dt, "H:M:S");
	printf ("fmt[H:M:S] time is %s\n", str_dt);
}

