#include <stdio.h>
#include <stdlib.h>

int cmp (const void *a , const void *b )
{
    return *(int *)a - *(int *)b;
}

int Mycmp(const void *a, const void *b)
{
	return (*(double*)a > *(double*)b ? 1:-1);
}


/** 字符串 **/
int Mycmp3(const void *a ,const void * b)
{
	return strcmp( (char *)a, (char *)b );
}

int main(void)
{
	int num[10];
	double dnum[10];
	int i=0;

	for (i=9; i>=0; i--) {
		num[i] = i;
		dnum[i] = i*1.10;
	}
	qsort(num, 10, sizeof(num[0]), cmp);
	for (i=0; i<10; i++) printf("%d ", num[i] );
	printf("\n");

	qsort(dnum, 10, sizeof(dnum[0]), Mycmp);
	for (i=0; i<10; i++) printf("%.2f ", dnum[i] );
	printf("\n");
	
}

