#include <stdio.h>
#include <stdlib.h>

typedef struct students
{
    char name[60];
    int  no;
    int  sex;
    int age;
} T_STUDENTS;

typedef struct link
{
    void *data;
    struct link *next;
} T_LINK;

char *str_to_upper(char *s);
void free_link(T_LINK *node);

/* ��ʼ������ */
T_LINK *init_link()
{
    T_LINK *node;

    node = (T_LINK *)malloc (sizeof(T_LINK) ) ;
    if (node == NULL)
    {
        fprintf(stderr, "malloc false!\n");
        exit(-1);
    }
    node->next = NULL;
    node->data = NULL;

    printf ("init link ok\n");

    return node;
}

/* �������� */
int insert_link_tail(T_LINK *head, void *data)
{
    T_LINK *L = head;
    T_LINK *node;

    node = (T_LINK *)malloc (sizeof(T_LINK) );
    if (node == NULL)
    {
        fprintf(stderr, "malloc false!\n");
        exit (-1);
    }

    node->data = data;
    node->next = NULL;

    while ( L->next != NULL )
    {
        L = L->next;
    }

    L->next = node;

    printf ("insert_link_tail succful\n");

    return 0;
}

/**ͷ�巨**/
int insert_link_head(T_LINK *head, void *data)
{
    T_LINK *L = head->next;
    T_LINK *node;

    node = (T_LINK *) malloc (sizeof(T_LINK) );
    if (node == NULL)
    {
        fprintf(stderr, "malloc false!\n");
        exit (-1);
    }

    node->data = data;

    node->next = L;
    head->next = node;

    printf ("insert_link_head succful\n");

    return 0;
}

/*ɾ������ڵ�*/
int delete_link(T_LINK *head, int age)
{
    T_LINK *L, *R;

    L = head->next;

    while( ((T_STUDENTS *)L->data)->age != age && L != NULL)
    {
        R = L;
        L = L->next;
    }

    if (L == NULL)
        printf("The record is not exist.\n");
    else
    {
        R->next = L->next;
        free (L);
        printf ("free L.\n");
        printf ("Delete succesful.\n");
    }

    return 0;
}

/* �ͷ����� */
void free_link(T_LINK *head)
{
    T_LINK *L = head;
    T_LINK *node = NULL;

    printf ("----- L = [%0xd]\n", L);
    while (L != NULL)
    {
        node = L;
        L = L->next;
        free(node);
        node = NULL;
        printf ("free_link... L \n");
    }
}

int prt_link(T_LINK *node)
{
    T_LINK *L = node->next;

    while (L != NULL)
    {
        printf("name = %s, age=%d\n",
               ((T_STUDENTS *)L->data)->name,
               ((T_STUDENTS *)L->data)->age);
        L = L->next;
    }

    return 0;
}

/*��ջ����*/
int get_link_data (T_LINK *head, void **data)
{
    T_LINK *L = head;
    T_LINK *node = head->next;

    if (node != NULL)
    {
        (*data) = node->data;
        L->next = node->next;
        free (node);
        printf ("free node... \n");
        node = NULL;
    }
    else
    {
        printf ("��ջ\n");
    }
    return 0;
}

/* �ַ���ת���ɴ�д */
char *str_to_upper(char *str)
{
    char *p = str;

    while (*str != '\0')
    {
        if (*str >= 'a' && *str <= 'z')
        {
            *str -= ' ';
        }
        str++;
    }

    return p;
}

int Sysmmetry(long n)
{
    long i, temp;
    i = n;
    temp = 0;

    while(i)
    {
        temp = temp * 10 + i % 10;
        i /= 10;
        printf("tmp = %d , i = %d\n", temp, i);
    }
    return(temp == n);
}

int cal_125_100()
{
    int number = 0, m;

    for (m = 0; m <= 100; m += 5)
    {
        number += (m + 2) / 2;
    }

    printf ("number = %d\n", number);
    return number;
}

int main()
{
    int n;
    char s1[24] = "fdafaDFDkjl";

    T_LINK *head = NULL;
    T_STUDENTS st[4];
    T_STUDENTS *data;

    head = init_link ();
    printf ("==== head = [%0xd]\n", head);

    strcpy(st[0].name, "XL");
    st[0].age = 5;
    insert_link_head(head, (void *)&st[0]);

    strcpy(st[1].name, "TOM");
    st[1].age = 10;
    insert_link_head (head, (void *)&st[1]);

    strcpy(st[2].name, "TOM1");
    st[2].age = 20;
    insert_link_head (head, (void *)&st[2]);

    strcpy(st[3].name, "TOM2");
    st[3].age = 30;
    insert_link_head (head, (void *)&st[3]);

    get_link_data (head, (void *)&data );
    printf ("data->name = %s ,data->age = %d\n",
            (data)->name,
            (data)->age);

    get_link_data (head, (void *)&data );
    printf ("data->name = %s ,data->age = %d\n",
            (data)->name,
            (data)->age);

    printf ("==== head = [%0xd]\n", head);
    prt_link(head);

    printf ("==== head = [%0xd]\n", head);
    free_link(head);
    printf ("==== head = [%0xd]\n", head);

#if 0

    printf("upper(pp)=[%s]\n", str_to_upper(s1) );
    printf("s1=[%s]\n", s1);

    cal_125_100();

    printf(" (����) ������һ��������λ����\n");
    scanf("%d", &n);
    if (n < 1000)
    {
        printf("��������ܳ�Ϊ������\n");
        return 0;
    }

    printf("circl_num = %d\n", Sysmmetry(n));
#endif

    return 0;
}

