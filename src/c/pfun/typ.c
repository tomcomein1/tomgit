#include <stdio.h>
#include <stddef.h>

/***
\n    //����
\r    //�س�
\b    //�˸�
\f    //��ҳ
\t    //ˮƽ�Ʊ��
\v    //��ֱ�Ʊ��
\a    //����
\"    //˫����
\'    //������
\x??  //��Сд x ����λ����(ʮ��������)��ʾһ���ַ� 
\???  //����λ����(�˽���)��ʾһ���ַ�
***/

int main(void)
{
    char c = 'a';
    char cs[] = "abc";
    wchar_t wc = L'A';     /* ��� L Ҳ����ʡ�� */
    wchar_t ws[] = L"ABC"; 
    int n1 = 65535;
    int n2 = 0xFFFF;       /* ʮ������   */
    int n3 = 0Xffff;       /* ��Сд���� */
    int n4 = 0177777;      /* �˽���     */

    printf("%c, %s, %c, %S\n", c, cs, wc, ws);
    printf("%d, %d, %d, %d\n", n1, n2, n3, n4);
    
    getchar();
    return 0;
}

