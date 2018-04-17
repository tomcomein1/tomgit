#include <stdio.h>
#include <ctype.h>

#define MAXHIST 15 //ֱ��ͼ��󳤶�
#define MAXCHAR 128 //�Ը������������7λ��ascii��һ����128��

int main()
{
    int c, i;
    int len; //��ʾÿ���ַ�����Ƶ�ʵ��ǺŸ���
    int maxvalue;
    int cc[MAXCHAR]; //ͳ��ÿ���ַ����ִ����ļ�����

    //������ͳ�������ÿ���ַ��Ĵ���
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
    //��ӡ���ֱ��ͼ
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

