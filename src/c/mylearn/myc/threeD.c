/*三Ｄ选号器，重零到999选出一个随机数*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
   int rand_no = 0;
   int i=0, max=0;

   max = atol(argv[1]);
   if (max <= 0) max = 1;

   srand((unsigned)time(NULL));

   for (i=0; i<max; i++)
   {
       rand_no = (rand() % 999 + 1);

       printf ("[%d]3D number: [%03d]\n", i+1, rand_no);
       usleep(8888);
   }
}

