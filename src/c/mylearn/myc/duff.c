#include <stdio.h>

#include <stdlib.h>

int main(int argc,char**argv){

int n;

if(argc<2){

printf("not enough arugment\n");

return -1;

}

n=atoi(argv[1]);

switch(n){

case 0: do {printf("%d ",0);

case 1: printf("%d ",1);

case 2: printf("%d ",2);

case 3: printf("%d ",3);

case 4: printf("%d ",4);

}while(--n>0);

}

printf ("\n");
return 0;

}