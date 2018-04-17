#include <stdio.h>
#include <stdarg.h>

//”√”⁄≤‚ ‘
//cc -brtl -g -qcpluscmt -qlanglvl=extended -qalign=natural -DAIXUSE -D_ALL_SOURCE -o tt1 tt1.c

#define MAX_STRING_LEN 4096
void DebugInfo (const char *file, int line, char * format,...);

#define DBG(fmt, argl...) DebugInfo(__FILE__, __LINE__, fmt, ##argl)

char *get_str(char *source, char *dest, int sp);

int main()
{
	char source[100]="abcd+dddcc+pdafd.dat";

	char dest[100] = "\0";
	char *p = source; 
	int i = 10;

        do {
	    p = get_str(p, dest, '+');
	    DBG ("dest=%s", dest);
        } while (p && *p != '\0'); 

	return 0;
}

char *get_str(char *source, char *dest, int sp)
{
	char *p = source;

	while ( *p != sp && *p != '\0' )
        {
            *dest++ = *p++ ;
	}
	
	*dest = '\0';

	return ++p;
}


void DebugInfo (const char *file, int line, char * format,...)
{
	va_list pvar;
	char    buf[MAX_STRING_LEN];
	FILE*	fp = NULL;
	
	va_start(pvar,format);
	vsnprintf(buf, sizeof(buf), format, pvar);
	va_end(pvar);

	fprintf(stderr,"[%s:%d] %s\n", file, line, buf);

}

