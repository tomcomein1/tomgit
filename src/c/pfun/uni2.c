#include <stdio.h>
#include <limits.h>

int main(void)
{
    union MyUnion {
        unsigned char     n1;
        unsigned short    n2;
        unsigned int      n3; 
    } U = {0};

    printf("%10u, %10u, %10u\n", U.n1, U.n2, U.n3);

    U.n2 = USHRT_MAX;
    printf("%10u, %10u, %10u\n", U.n1, U.n2, U.n3);

    U.n3 = INT_MAX;
    printf("%10u, %10u, %10u\n", U.n1, U.n2, U.n3);

    U.n1 = 0;
    printf("%10u, %10u, %10u\n", U.n1, U.n2, U.n3);

    getchar();
    return 0;
}
