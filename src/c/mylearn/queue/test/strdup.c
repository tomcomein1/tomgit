#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <conio.h>

int main()
{
char *s="Golden Global View";
char *d;
/* clrscr(); */
d=strdup(s);
if(NULL != d) {
printf("%s\n",d);
free(d);
}
getchar();
return 0;
}
