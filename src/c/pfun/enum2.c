#include <stdio.h>

int main(void)
{
    enum ABC{AAA,BBB,CCC} e1=BBB, e2=BBB+1, e3=e2-2;
    
    printf("%d, %d, %d\n", e1, e2, e3);
    getchar();
    return 0;
}
