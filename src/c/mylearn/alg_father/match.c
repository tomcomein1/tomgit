/*
 * match.c
 *
 *  Created on: 2012-12-4
 *      Author: Administrator
 */
#include <stdio.h>

#define NELEMS(array) (sizeof(array) / sizeof(array[0]))

int matchhere(char *,char *);

int matchstar(int c,char *regexp,char *text){
     char *t;

     for(t = text; *t != '\0' && (*t == c || c=='.'); t++);
     do{
          if(matchhere(regexp, t))return 1;
     }while(t-->text);

     return 0;
}

int matchhere(char *regexp, char *text) {
     if (regexp[0] == '\0') {
          return 1;
     }
     if (regexp[1] == '*') {
          return matchstar(regexp[0], regexp + 2, text);
     }
     if (regexp[0] == '$' && regexp[1] == '\0') {
          return *text == '\0';
     }
     if( (text[0] != '\0') && (regexp[0]=='.' || regexp[0]==text[0]) ) {
          return matchhere(regexp+1, text+1);
     }

     return 0;
}

int match(char *regexp, char *text) {
     if (regexp[0] == '^')
          return matchhere(regexp + 1, text);
     do {
          if (matchhere(regexp, text)) {
               return 1;
          }
     } while (*text++ != '\0');
     return 0;
}

int main(){
     char regexp[20] = "12";
     char text[100] = "123";
     
     printf ("regexp: size [%d]\n", NELEMS(regexp) );
     printf ("text: size [%d]\n", NELEMS(text) );
     
     if(match(regexp, text)){
          puts("ok");
     }
     else {
         puts ("not found");
     }
     
     return 0;
}

