#include <stdio.h>
#include <math.h>

int main(void)
{
	int k = 0;
	
	do {
		printf ("请输入一个数[1-退出]: "); 
		scanf ("%d", &k);
		if (k > 1)
			ss (k);
	} while (k != 1);
}

int ss(int m)
{
	int i;
	int k = (int)sqrt(m);
	
	for (i=2; i<=k; i++)
		if (m%i == 0) break;
	if (i>k) printf ("[%d] 是个素数\n", m);
	else printf("[%d] 不是素数\n", m); 
	
}

