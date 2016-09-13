#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void PrintInt(char* buffer, int data, ...)  
{  
    static char space[1024];  
    char temp[32];  
    int* start;  
    int count;  
      
    if(NULL == buffer)  
        return;  
  
    memset(space, 0, 1024);  
    memset(temp, 0, 32);  
    start = (int*) &buffer;  
    count = 0;  
  
    while(buffer[count]){  
        if(!strncmp(&buffer[count], "%d", strlen("%d"))){  
            start ++;  
//            itoa(*start, temp, 10);
            sprintf(temp, "%d", *start);
            strcat(space, temp);  
            count += 2;  
            continue;  
        }  
  
        space[strlen(space)] = buffer[count];  
        count ++;  
    }  
  
    memset(buffer, 0, strlen(buffer));  
    memmove(buffer, space, strlen(space));  
    return;  
}  

display()  
{  
    char buffer[32] = {"%d %d %d %d\n"};  
    PrintInt(buffer, 1, 2, 13, 4);  
    printf(buffer);  
}  

int main(void)
{
	return display();
}

