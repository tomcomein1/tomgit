/*双色球随机生成六个红色号 1-33，和一个蓝色号 1-16 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXNO 1000
#define SLEEPTIME 888

int main(int argc, char **argv)
{
  int red_source[34];
  int red_dest[7];

  int blue_source[17];
  int blue_dest[2];
  
  int i = 0, max = 1;

  srand((unsigned)time(NULL)); 
  max = atol(argv[1]);
  if (max <= 0) max = 1;
   
  print_titlle();


  for (i=0; i<max; i++)
  {
      init_number(red_source, 33);
      /* print_number(red_source, 33); */
      select_number(red_source, red_dest, 33, 6);
      sort_number(red_dest, 6);
      printf ("red number: ");
      print_number(red_dest, 6);

      /**---------------------------**/
      init_number(blue_source, 16);
      /* print_number(blue_source, 16);  */

      select_number(blue_source, blue_dest, 16, 1);
      printf ("blue number: ");
      print_number(blue_dest, 1);
      usleep(88888);
  }

  printf ("============================================\n");

}

/*在数组中选择生成一个随机数*/
int select_number(int source[], int dest[], int max, int no)
{
     int i = 0;
     int j = 0;
     int number = max;

     for (i=1; i<=no; i++)
     {
         while (1) /**/
         {
             j = rand() % number + 1;
             if (j) break;
         }

         /* printf ("no=%d, j=%d \n", number, j); */

         dest[i] = source[j] ;

         del_number(source, dest[i], number);
         /* print_number(source, max); */
         number--;
         usleep(SLEEPTIME);
     }
}

/*对选中的随机数在源数据中删除该数*/
int del_number(int source[], int number, int max)
{
     int i = 0; 
     /** printf ("=%d=%d=\n", number, max); **/
     for(i=1; i<=max; i++)
     {
         if (source[i] == number) 
         {
             source[i] = source[max];
             source[max] = 0;
             break;
         }
     }
}


/*初始化源数据*/
int init_number(int number[], int max_no)
{
   int i = 0;
   
   for (i=1; i<=max_no; i++)
       number[i] = i;
}

/*打印数据*/
int print_number(int number[], int max_no)
{
   int i=0;
   
   for (i=1; i<=max_no; i++)
       printf ("[%02d] ", number[i]); 
   printf ("\n");
}

/*排序*/
int sort_number(int source[], int count)
{
    int iTemp;
    int i, j;
    
    for(i=1; i<=count-1; i++)
    {
    	  for(j=i+1; j<=count; j++)
    	  { 
    	 	  if(source[j]< source[i])
          {
             iTemp = source[i];
             source[i] = source[j];
             source[j] = iTemp;
      	  }
        }
     }
}

int print_titlle()
{
   printf ("\n 双色球随机选号器 \n");
   printf ("============================================\n");
}

