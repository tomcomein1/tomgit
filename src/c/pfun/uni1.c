#include <stdio.h>

int main(void)
{
    union MyUnion {
        short  n1;
        int    n2;
        double n3;
    };
    
    printf("%u, %u\n", sizeof(union MyUnion), sizeof(double));

    getchar();
    return 0;
}

