#include <stdio.h>

const volatile int num = 123;

int main(void)
{    
    printf("%d\n", num);
    getchar();
    return 0;
}

