#include <stdio.h>
#include <math.h>

int main(void)
{
	int k = 0;
	
	do {
		printf ("������һ����[1-�˳�]: "); 
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
	if (i>k) printf ("[%d] �Ǹ�����\n", m);
	else printf("[%d] ��������\n", m); 
	
}

