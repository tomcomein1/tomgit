#include <stdio.h>

void IntToBinary(unsigned num);

int main(void)
{
    int num=0;

    printf("input int num:\n");
    scanf("%d", &num);

    printf("num=%d\n", num);
    IntToBinary(num); /* 11111111 */
    printf("\n");
    getchar();
    return 0;
}

void IntToBinary(unsigned num) {
    int i = num % 2;
    if (num > 1) IntToBinary(num / 2);
    putchar(i ? '1' : '0');
    //    putchar('0' + i);  /* 可代替上面一句 */
}

