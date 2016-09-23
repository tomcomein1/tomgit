#include <stdio.h>

typedef int (*pfun)(int, int);

int add(int x, int y) {return(x + y);}
int sub(int x, int y) {return(x - y);}
int mul(int x, int y) {return(x * y);}
int div(int x, int y) {return(x / y);}

int math1(int(*pfun)(int, int), int x, int y) {
    return pfun(x, y);
}

int math(pfun pf, int x, int y) {
    return pf(x, y);
}

int main(void)
{
    printf("%d\n", math(add, 9, 3)); /* 12 */
    printf("%d\n", math(sub, 9, 3)); /* 6  */
    printf("%d\n", math(mul, 9, 3)); /* 27 */
    printf("%d\n", math(div, 9, 3)); /* 3  */
    
    getchar();
    return 0;
}

