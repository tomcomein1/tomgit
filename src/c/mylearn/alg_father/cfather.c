#include <stdio.h>
#include <ctype.h>

#define MAXHIST 15 //直方图最大长度
#define MAXCHAR 128 //自负的最大数量，7位的ascii码一共有128个

int main()
{
    int c, i;
    int len; //表示每个字符出现频率的星号个数
    int maxvalue;
    int cc[MAXCHAR]; //统计每个字符出现次数的计数器

    //下面是统计输入的每个字符的次数
    for (i = 0; i < MAXCHAR; ++i)
        cc[i] = 0;

    while ((c = getchar()) != EOF)
    {
        if (c < MAXCHAR)
            ++cc[c];
    }

    maxvalue = 0;
    for (i = 1; i < MAXCHAR; ++i)
        if (cc[i] > maxvalue)
            maxvalue = cc[i];
    //打印这个直方图
    for (i = 1; i < MAXCHAR; ++i)
    {
        if (isprint(i))
            printf("%5d - %c - %5d:", i, i, cc[i]);
        else
            printf("%5d - - %5d:", i, cc[i]);

        if (cc[i] > 0)
        {
            if ((len = cc[i] * MAXHIST / maxvalue) <= 0)
                len = 1;
        }
        else
            len = 0;

        while (len > 0)
        {
            putchar('*');
            --len;
        }
        putchar('\n');
    }
}

