#include <stdio.h>
#include <stddef.h>

/***
\n    //换行
\r    //回车
\b    //退格
\f    //换页
\t    //水平制表符
\v    //垂直制表符
\a    //响声
\"    //双引号
\'    //单引号
\x??  //用小写 x 和两位数字(十六进制数)表示一个字符 
\???  //用三位数字(八进制)表示一个字符
***/

int main(void)
{
    char c = 'a';
    char cs[] = "abc";
    wchar_t wc = L'A';     /* 这个 L 也可以省略 */
    wchar_t ws[] = L"ABC"; 
    int n1 = 65535;
    int n2 = 0xFFFF;       /* 十六进制   */
    int n3 = 0Xffff;       /* 大小写都行 */
    int n4 = 0177777;      /* 八进制     */

    printf("%c, %s, %c, %S\n", c, cs, wc, ws);
    printf("%d, %d, %d, %d\n", n1, n2, n3, n4);
    
    getchar();
    return 0;
}

