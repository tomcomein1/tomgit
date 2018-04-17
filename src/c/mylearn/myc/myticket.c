/*¸£²Ê»úÑ¡*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define RED_AREA 33
#define BLUE_AREA 16
#define USLEEP_TIME 8888
#define SLEEP_TIME 1
#define swap(pa, pb) (pa^=pb, pb^=pa, pa^=pb) 
int red_number[7];

int main(int argc, char *args[])
{
    int j = 5;
    printf ("* * * * * !!!! lottery ticket 5000000£¤  * * * * * * * * * * * * \n\n");
    printf ("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * \n");

    j=atol(args[1]);
    if (j==0) j=1;

    while (j--)
    {
        init_red_number();
        select_ticket();
        sleep (SLEEP_TIME);
    }

    printf ("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * \n\n");
}

int init_red_number()
{
   int i=0;
   for (i=0; i<7; i++)
       red_number[i] = 0;
}

int select_ticket()
{
    int blue_number;
    int i = 0, j=0;

    srand((unsigned)time(NULL)); 

    for (i=0; i<6; i++)
    {
        red_number[i] = (rand() % RED_AREA + 1); 

        usleep (SLEEP_TIME);
        while(chk_unique(red_number, i) )
        {
            red_number[i] = (rand() % RED_AREA + 1); 
            usleep (USLEEP_TIME);
        }
    }
    
    blue_number = (rand() % BLUE_AREA + 1); 

    sort_num();
   
    
    printf ("*\tred number: ");
    for (i=0; i<6; i++)
        printf ("[%02d] ", red_number[i]);
    
    printf ("\n*\tblue number: [%02d] \n", blue_number);

    return 0;

}

int chk_unique(int number[7], int i )
{
    int k = number[i];
    int j = 0;
    
    if (k == 0) return 1;

    for (j=0; j<i; j++)
    {
       if (number[j] == k) return 1;
    }

    return 0;
}

/*ÅÅÐò*/
int sort_num()
{
        int i=0, j=0;
        int k = 0;
        
        for (i=0; i<7; i++)
        {
                for (j=i+1; j<6; j++)
                {
                    /* printf ("i[%d]=%d j[%d]=%d\n", i, red_number[i], j, red_number[j]); */
                        if (red_number[j] < red_number[i])
                        {
                            k = red_number[i];
                                red_number[i] = red_number[j];
                                red_number[j] = k;
                                /*½»»»*/
                        }
                }
        }
}