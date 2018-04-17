#include <stdlib.h>
#include <stdio.h>

#define NELEMS(array) (sizeof(array) / sizeof(array[0]))
#define ARRAY_LEN 10

/*自定义整数比较函数*/
int icmp (const void *p1, const void *p2)
{
    int v1, v2;

    v1 = *(int *) p1;
    v2 = *(int *) p2;

    if (v1 < v2)
        return -1;
    else if (v1 == v2)
        return 0;
    else
        return 1;
}

/*自定义字串比较函数*/
int scmp (const void *p1, const void *p2)
{
    char *v1, *v2;

    v1 = *(char **) p1;
    v2 = *(char **) p2;
    return strcmp(v1, v2);
}

int main(void)
{
    int i = 0;
    int iarray[ARRAY_LEN] = {23, 44, 50, 12, 324, 34, 23, 2, 19, 20};
    char *sarray[ARRAY_LEN] = {"abcd", "qqqq", "hell", "pfda", "asss",
                               "fda", "a", "bcd", "fdja", "2131"
                              };

    printf ("int array sort befor... \n");
    prt_iarray(iarray, NELEMS(iarray));

    qsort (iarray, ARRAY_LEN, sizeof(iarray[0]), icmp);

    printf ("int array sort after... \n");
    prt_iarray(iarray, NELEMS(iarray));

    printf ("char array sort befor... \n");
    prt_sarray( sarray, NELEMS(sarray) );

    qsort (sarray, ARRAY_LEN, sizeof(sarray[0]), scmp);

    printf ("char array sort after... \n");
    prt_sarray(sarray, NELEMS(sarray));

    return 0;
}

/*遍列数组*/
int prt_iarray(int array[], int arr_size)
{
    int i = 0;
    for (i = 0; i < arr_size; i++)
    {
        printf ("%d ", array[i]);
    }
    printf ("\n");

    return 0;
}

/*遍列数组*/
int prt_sarray(char *array[], int arr_size)
{
    int i = 0;

    for (i = 0; i < arr_size; i++)
    {
        printf ("%s ", array[i]);
    }
    printf ("\n");

    return 0;
}

