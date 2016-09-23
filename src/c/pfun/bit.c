#include <stdio.h>

int main(void)
{
    struct Bit {
        unsigned a: 1; /* 1 Bit, 取值范围: 0 - 1  */
        unsigned b: 2; /* 2 Bit, 取值范围: 0 - 3  */
        unsigned c: 3; /* 3 Bit, 取值范围: 0 - 7  */
        unsigned d: 4; /* 4 Bit, 取值范围: 0 - 15 */
    } B;

    B.a = 1;
    B.b = 3;
    B.c = 7;
    B.d = 15;

    printf("%d, %d, %d, %d\n", B.a, B.b, B.c, B.d);

    getchar();
    return 0;
}
