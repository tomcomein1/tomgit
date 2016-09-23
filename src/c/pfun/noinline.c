#include <stdio.h>
#include <time.h>

int sum(int x) {
    return x + x;
}

int main(void)
{
    int count = 1000000000;
    int i;
    int num;
    
    time_t start, stop;

    time(&start);
    for (i = 0; i < count; i++) num = sum(i); 
    time(&stop);
    printf("调函数用时: %d 秒;\n", stop - start);  /* 8 */
    
    time(&start);
    for (i = 0; i < count; i++) num = i + i;    
    time(&stop);
    printf("非函数用时: %d 秒;\n", stop - start);  /* 4 */
    
    getchar();
    return 0;
}
