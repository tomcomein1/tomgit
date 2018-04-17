#include <stdlib.h>  
#include <stdio.h>  
  
#include "queue.h"  
  
int main()  
{  
    struct queue *q;  
    struct business_data *data1, *data2, *data3, *data4;  
    struct business_data *get1, *get2, *get3, *get4;  

    data1 = data2 = data3 = data4 = NULL;  
    get1 = get2 = get3 = get4 = NULL;  
    q = NULL;  

    data1 = (struct business_data *)malloc(sizeof(struct business_data));  
    data1 -> age = 20;  
    strcpy(data1 ->name, "Lily");  
    data2 = (struct business_data *)malloc(sizeof(struct business_data));  
    data2 -> age = 21;  
    strcpy(data2 ->name, "Lucy");  
    data3 = (struct business_data *)malloc(sizeof(struct business_data));  
    data3 -> age = 22;  
    strcpy(data3 ->name, "Hanmeimei");  
    data4 = (struct business_data *)malloc(sizeof(struct business_data));  
    data4 -> age = 33;  
    strcpy(data4 ->name, "Xiaoli");  

    init_queue(&q);  
    queue_put(&q, data1);  
    queue_put(&q, data2);  
    queue_put(&q, data3);  
    queue_put(&q, data4);  

    print_queue(&q);  
    queue_get(&q, &get1);  
    queue_get(&q, &get2);  
    queue_get(&q, &get3);  
    queue_get(&q, &get4);
    release_queue(&q);  

    return 0;  
}  

