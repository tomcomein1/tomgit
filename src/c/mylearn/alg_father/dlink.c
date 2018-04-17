////////////////////////////////////////////
//˫����ĳ�ʼ�������������룬���ң�ɾ����//
//Author:Wang Yong                        //
//Date: 2010.8.19                         //
////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>

typedef int ElemType;
////////////////////////////////////////////

// ����˫����������

typedef struct Node
{
    ElemType data;
    struct Node *prior;         //ָ��ǰ�����
    struct Node *next;          //ָ���̽��
} Node, *DLinkList;

////////////////////////////////////////////

//˫����Ľ���������β�巨����˫����
DLinkList DLinkListCreatT()
{
    Node *L, *p, *r;
    ElemType x;

    L = (Node *)malloc(sizeof(Node));//����ͷ���
    L->next = NULL;
    r = L;
    r->next = NULL;                          //r Ϊָ���ն˽���ָ��

    while(scanf("%d", &x) != EOF)   //����˫����Ԫ�أ�����˫����
    {
        p = (Node *)malloc(sizeof(Node));
        p->data = x;
        p->next = r->next;
        r->next = p;
        r = p;
    }
    r->next = NULL;
    return L;
}

/////////////////////////////////////////

//˫����Ĳ��ң�����Ԫ��Ϊx��λ��

int DLinkListFind(DLinkList L, ElemType x)
{
    DLinkList p;                //pΪ������
    int i = 1;

    p = L->next;

    while(p != NULL && p->data != x )//Ѱ��ֵΪx��Ԫ��**ע������ѭ������������д��
    {
        //ԭ�򣬵�p == NULL ʱ�� p->data �����
        ++i;                        //  for (i = 1, p = L->next; p; p = p->next, i++) {
        //  if (p->data == x) break;}
        p = p->next;
    }

    if(p == NULL)               //���û�ҵ�����0
        return 0;
    else return i;              //����ҵ�����i
}
/////////////////////////////////////////

//˫����Ĳ��룬��˫�����еĵ�i��λ�ò���ֵΪx��Ԫ��

DLinkList DLinkListInsert(DLinkList L, int i, ElemType x)
{
    DLinkList p, s;                         //sΪҪ����Ľ��
    int tempi;

    p = L->next;                         //�ӵ�һ�����λ�ÿ�ʼ����

    for(tempi = 1; tempi < i - 1; tempi++)
        p = p->next;
    s = (Node *)malloc(sizeof(Node));
    s->data = x;                         //��x��ֵ��s��������
    s->next = p->next;                        //��������
    p->next->prior = s;
    s->prior = p;
    p->next = s;

    return L;

}

//////////////////////////////////////////////

//˫�����ɾ����ɾ��˫�����е�i�����

DLinkList DLinkListDelete(DLinkList L, int i)
{
    int tempi = 1;
    DLinkList p;                        //pΪ���ҽ�㡣
    p = L->next;
    while((tempi++) != i && p != NULL)
    {
        p = p->next;
    }
    if(p == NULL)                       //����ǲ�����˫�����е�λ��
        printf("λ�ò��Ϸ���\n");
    else if(p->next == NULL)         //���һ��������⴦��ԭ�����һ�����p->nextû��prior
    {
        p->prior->next = NULL;
        free(p);
    }
    else                                //����ɾ������
    {
        p->prior->next = p->next;
        p->next->prior = p->prior;
        free(p);
    }
}

///////////////////////////////////////////
int main()
{
    DLinkList list, start;
    int i;
    ElemType x;

    list = DLinkListCreatT();
    for(start = list->next; start != NULL; start = start->next)
        printf("%d ", start->data);
    printf("\n");


    printf("������Ҫ����Ԫ�ص�ֵ��");
    scanf("%d", &x);
    i = DLinkListFind(list, x);
    if(i)
        printf("�������е�λ��Ϊ��%d\n", i);
    else
        printf("û�����Ԫ�ء�\n");
    printf("���������λ�ã�");
    scanf("%d", &i);
    printf("���������Ԫ�ص�ֵ��");
    scanf("%d", &x);
    DLinkListInsert(list, i, x);
    for(start = list->next; start != NULL; start = start->next)
        printf("%d ", start->data);
    printf("\n");
    printf("������Ҫɾ����λ�ã�");
    scanf("%d", &i);
    DLinkListDelete(list, i);
    for(start = list->next; start != NULL; start = start->next)
        printf("%d ", start->data);
    printf("\n");

    return 0;
}

