#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define S_SPLIT "$|$"

int main()
{
        char source[500]="a|a|aa$|$bb|bb|b$|$ccc|ccc$|$dddddd$|$";

        char *p_unit, *pstr;
        char *psub;
        char dest[300];
        char data[100];

        int  i = 1;
        int  j = 0;
        int  k = 0;

        pstr = source;
        while (pstr)
        {
                p_unit = strstr(pstr, S_SPLIT);
                printf ("p_unit[%0xd]=[%s]\n", p_unit, p_unit);
                printf ("pstr[%0xd]=[%s]\n", pstr, pstr);

                if (p_unit && pstr)
                {
                    memset(dest, 0x00, sizeof(dest) );
                    strncpy(dest, pstr, p_unit - pstr ); 
                }

                /** 此处处理业务逻辑 **/
                printf ("dest[%d]=[%s]\n", i++, dest);
                psub = dest;
                k = 1;
                while (*psub != '\0')
                {
                      j = 0;

                      memset(data, 0x00, sizeof(data) );
                      while ((*psub != '\0') && (*psub != '|') ) 
                            data[j++] = *psub++;
                      data[j] = '\0';
                      printf ("\tdata[%d]: [%s]\n", k++, data);
                      /** 根据位置拷贝data到相应变量 ***/
                      switch (k)
                      {
                          case 1:
                                break;
                          case 2:
                                break;
                          case 3:
                                break;
                      }
                      
                      psub++;
                }
        
                /** 如果只余下分隔符不再处理 **/
                if ( strcmp(p_unit, S_SPLIT ) == 0 )
                        break;
                /** 过滤掉分隔符 **/
                pstr = p_unit + strlen(S_SPLIT);  
        }

        return 0;
}
