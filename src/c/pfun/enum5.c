#include <stdio.h>

int main(void)
{
    enum ABC{AAA=2,BBB=2,CCC} e1, e2, e3;

    e1 = AAA;
    e2 = BBB;
    e3 = CCC;
    
    printf("%d, %d, %d\n", e1, e2, e3);
    getchar();
    return 0;
}
