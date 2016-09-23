#include <stdio.h>

int main(void)
{
    typedef int * PINT;

    int num = 123;
    PINT p = &num;

    printf("%d, %p\n", *p, p);
    
    getchar();
    return 0;
}
