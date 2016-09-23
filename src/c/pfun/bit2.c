#include <stdio.h>

int main(void)
{
    struct Bit {
        unsigned b1: 1;
        unsigned b2: 1;
        float f;
    } B;
    
    B.b1 = 0;
    B.b2 = 1;
    B.f = 3.14;
    
    printf("%d, %d, %g\n", B.b1, B.b2, B.f); 

    getchar();
    return 0;
}

