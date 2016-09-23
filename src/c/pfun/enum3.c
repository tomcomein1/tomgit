#include <stdio.h>

int main(void)
{
    enum {AAA,BBB,CCC} e=BBB;
    
    printf("%d\n", e);

    e++;
    printf("%d\n", e);
    
    getchar();
    return 0;
}

