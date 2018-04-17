#include <stdio.h>
#include <stdlib.h>

int swap(void *x, void *y, int size)
{
        void *tmp=NULL;

        if ((tmp = malloc(size)) == NULL)
                return -1;
        memcpy (tmp, x, size);
        memcpy (x, y, size);
        memcpy (y, tmp, size);

        free(tmp);
        tmp = NULL;
        return 0;
}

int main(void)
{
	float f1 = 100.02, f2 = 23.34;
	
	printf ("swap after: f1 = %.2f, f2 = %.2f\n", f1, f2);
	swap (&f1, &f2, sizeof(float) );
	printf ("swap befor: f1 = %.2f, f2 = %.2f\n", f1, f2);
	
	return 0;
}

