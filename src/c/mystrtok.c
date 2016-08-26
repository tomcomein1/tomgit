#include <stdio.h>  

// strtok源码剖析  
char* __cdecl MyStrtok(char * string, const char * control)  
{ 
    unsigned char *str;  
    const unsigned char *ctrl = (const unsigned char *)control;  
    static unsigned char* _TOKEN = NULL;   
    //注意这里使用了static类型，实际的strtok函数出于线程安全会使用TLS  
  
    //由于char类型占一个字节取值范围为0~255  
    //所以可以打个bool flag[255]这样的哈希表  
    //来记录哪些字符为delimiter characters  
    //然后根据《位操作基础篇之位操作全面总结》中的位操作与空间压缩  
    //http://blog.csdn.net/morewindows/article/details/7354571#t6  
    //可以将数组大小取255/8即32  
    unsigned char map[32];  
    int count;  
  
    // Clear control map  
    for (count = 0; count < 32; count++)  
        map[count] = 0;  
  
    // Set bits in delimiter table  
    do {  
        //map[*ctrl >> 3] |= (1 << (*ctrl & 7));//strtok原来的代码  
        map[*ctrl / 8] |= (1 << (*ctrl % 8));  
    } while (*ctrl++);  
  
    // Initialize str   
    // If string is NULL, set str to the saved pointer   
    //(i.e., continue breaking tokens out of the string from the last strtok call)  
    if (string != NULL)  
        str = (unsigned char *)string;  
    else  
        str = (unsigned char *)_TOKEN;  
  
    // Find beginning of token (skip over leading delimiters). Note that  
    // there is no token iff this loop sets str to point to the terminal  
    // null (*str == '\0')  
    //while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )//strtok原来的代码  
    while ( (map[*str / 8] & (1 << (*str % 8))) && *str )  
        str++;  
  
    string = (char *)str;  
  
    // Find the end of the token. If it is not the end of the string,  
    // put a null there.   
    for ( ; *str ; str++ )  
    {  
        //if ( map[*str >> 3] & (1 << (*str & 7)) ) //strtok原来的代码  
        if ( map[*str / 8] & (1 << (*str % 8)) )   
        {  
            *str++ = '\0';  
            break;  
        }  
    }  
  
    // Update nextoken (or the corresponding field in the per-thread data structure  
    _TOKEN = str;  
  
    // Determine if a token has been found.   
    if ( string == (char *)str )  
        return NULL;  
    else  
        return string;  
}  

int main()  
{  
    printf("   strtok源码剖析 位操作与空间压缩\n");    
    printf(" - http://blog.csdn.net/morewindows/article/details/8740315 -\n");    
    printf(" - By MoreWindows( http://blog.csdn.net/MoreWindows ) - \n\n");   
  
    //char szText[] = "MoreWindows  (By http://blog.csdn.net/MoreWindows)";  
    //char szFind[] = " ";  
    char szText[] = "ab,c...d(e)f(g)hj";  
    char szFind[] = ",.()";  
  
    printf("原字符串为: %s\n", szText);  
    printf("分隔后为: \n");  
    char *pToken = MyStrtok(szText, szFind);  
    while (pToken != NULL)  
    {  
        printf("%s\n", pToken);  
        pToken = MyStrtok(NULL, szFind);  
    }  
    return 0;  
}  

