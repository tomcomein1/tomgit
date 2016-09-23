#include <stdio.h>

int add(int x, int y) {return(x + y);}
int sub(int x, int y) {return(x - y);}
int mul(int x, int y) {return(x * y);}
int div(int x, int y) {return(x / y);}

int main(void)
{
    int (*pf[4])(int, int) = {add, sub, mul, div};

    printf("%d\n", pf[0](9, 3)); /* 12 */
    printf("%d\n", pf[1](9, 3)); /* 6  */
    printf("%d\n", pf[2](9, 3)); /* 27 */
    printf("%d\n", pf[3](9, 3)); /* 3  */
    
    getchar();
    return 0;
}


