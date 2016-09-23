#include <stdio.h>

int add(int x, int y) {return(x + y);}
int sub(int x, int y) {return(x - y);}
int mul(int x, int y) {return(x * y);}
int div(int x, int y) {return(x / y);}

int main(void)
{
    int (*pf)(int, int);

    pf = add;
    printf("%d\n", pf(9, 3)); /* 12 */
    
    pf = sub;
    printf("%d\n", pf(9, 3)); /* 6  */
    
    pf = mul;
    printf("%d\n", pf(9, 3)); /* 27 */
    
    pf = div;
    printf("%d\n", pf(9, 3)); /* 3  */
    
    getchar();
    return 0;
}

