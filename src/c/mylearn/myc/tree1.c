#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
typedef  int Status;
#define  TElemType char
#define  ERROR  0
#define  OK     1
#define  OVERFLOW  -2
#define  STACK_INIT_SIZE  100
#define  STACKINCRMENT     10
static int i=1;
static int n=0;
static char str[30];
typedef struct BiTNode 
{ 
 char data; 
 struct BiTNode *lchild,*rchild; //���Һ���ָ��
}BiTNode,*BiTree;

typedef struct//ջ�Ķ���
{
 struct BiTNode  *base;
 struct BiTNode   *top;
 int stacksize;
}SqStack;

Status Initstack(SqStack &S) //����һ����ջ
{
 S.base=(BiTNode * )malloc(STACK_INIT_SIZE * sizeof(BiTNode));
 if(!S.base)return(ERROR);
  S.top=S.base;
  S.stacksize=STACK_INIT_SIZE;
 return OK;
}

StackEmpty(SqStack S)
{
 if(S.top == S.base )
  return OK;
 else
  return ERROR;
}

Status Push(SqStack &S,BiTree e)  // ��ջ
{
 if(S.top - S.base >=S.stacksize)
 {
  S.base =(BiTNode * )realloc(S.base,(S.stacksize + STACKINCRMENT) * sizeof(BiTNode));
  if(!S.base )exit(OVERFLOW);
   S.top =S.base +  S.stacksize ;
  S.stacksize +=STACKINCRMENT;
 }
 if(e)
  *S.top=*e;
 else
 {
  S.top->data='#';
  S.top->lchild =NULL;
  S.top->rchild =NULL;
 }
    S.top++;
 return OK;
}

Status Pop(SqStack &S,BiTree &e)//��ջ
{
 if(S.top == S.base )return(ERROR);
  S.top--;
   e=S.top; 
 return OK; 
}

Status GetTop(SqStack S,BiTree &e)//ȡջ��Ԫ��
{
 if(S.base  == S.top )
  return(OVERFLOW);
 e=S.top-1;
 if(e->data == '#')
  return ERROR;
 else
  return OK;
}

char Fileread(int j)
{
 FILE * fp;
 int i=0;
 int t=0;
 t=j;
 if(num==1)
 {
  if((fp=fopen("a.txt","r"))==NULL)
  {
   printf("�ļ���ʧ�ܣ�\n");
   exit(0);
  }
  str[0]=fgetc(fp);
  while(str[i]!=EOF)
  {
   i++;
   str[i] = fgetc(fp); 
  }
  num=0;
  fclose(fp);
 }
 return str[t];
}

Status CreatBiTree(BiTree &T)//���򴴽���������T��#������� 
{ 
 char ch;
 ch=Fileread(j);
 j++;
 if(ch=='#')
  T=NULL;
 else 
 { 
  if(!(T=(BiTNode*)malloc(sizeof(BiTNode))))
   exit(OVERFLOW);
  T->data=ch; 
  CreatBiTree(T->lchild); 
  CreatBiTree(T->rchild); 
 } 
 return OK; 
} 

Status Visit(TElemType e)
{
 if(e!='#')
  printf("%c",e);
 return OK;
}

Status PreOrderTraverse(BiTree T)//�ݹ��ȸ����� 
{ 
 if(T)
 {
  if(Visit(T->data))
   if(PreOrderTraverse(T->lchild))
    if(PreOrderTraverse(T->rchild))
     return OK;
  return ERROR; 
 }
 else
  return OK;
} 

Status InOrderTraverse(BiTree T)//�ݹ��и����� 
{ 
 if(T)
 {
  if(InOrderTraverse(T->lchild))
   if(Visit(T->data))
    if(InOrderTraverse(T->rchild))
     return OK;
  return ERROR; 
 }
 else
  return OK;
} 


Status PostOrderTraverse(BiTree T)//�ݹ������� 
{ 
 if(T)
 { 
  if(PostOrderTraverse(T->lchild))
   if(PreOrderTraverse(T->rchild))
    if(Visit(T->data)) 
     return OK;
  return ERROR; 
 }
 else
  return OK;
} 

Status PreOrderTraverse2(BiTree T)//�ǵݹ��ȸ�����
{
 SqStack S;
 BiTree p;
 Initstack(S);
 Push(S,T);
 while(!StackEmpty(S))
 {
  while(GetTop(S,p) && p)//ջ����GetTop(S,p)����OK��P����ջ��Ԫ�أ���ΪNULL
  {
   Visit(p->data);
    Push(S,p->lchild);
  }
  Pop(S,p);//��ָ����ջ
  if(!StackEmpty(S))
  {
   Pop(S,p);
    Push(S,p->rchild);      
  }
  
 }
 return OK;
}

Status InOrderTraverse2(BiTree T)//�ǵݹ��и����� 
{
 SqStack S;
 BiTree p=T;
 Initstack(S);
   while(p||!StackEmpty(S))
  {
    if(p)
   {
  Push(S,p);
  p=p->lchild;
  }
    else
  {
  Pop(S,p);
  printf("%c",p->data);
  p=p->rchild;
    }
  }

 return OK;
}

Status PostOrderTraverse2(BiTree T)//�ǵݹ�������
{
 SqStack S;
 BiTree p,l,r;
    Initstack(S);
    Push(S, T);
    while(!StackEmpty(S))
    {
  Pop(S,p);
        l = p->lchild;
        r = p->rchild;
        if (l == NULL && r == NULL)
        {
            printf("%c", p->data);
        } 
    else 
      {
            p->lchild = NULL;
            p->rchild = NULL;
            Push(S, p);
            if (r != NULL) Push(S, r);
            if (l != NULL) Push(S, l);
        }
     }
 return OK;
}

Status Leafcount(BiTree T)//����Ҷ�ӽڵ� 
{ 
 if(!T)
  return ERROR; 
 else 
 {  
  if(!T->lchild&&!T->rchild)
   return OK; 
  else 
   return Leafcount(T->lchild)+Leafcount(T->rchild); 
} 
} 

Status Depth(BiTree T)//���ĸ߶� 
{ 
 if(!T)
  return ERROR; 
 else 
 { 
  int m=Depth(T->lchild); 
  int n=Depth(T->rchild); 
  return (m>n?m:n)+1; 
 } 
} 

void Revolute(BiTree &T)// ������������ 
{ 
 BiTree t; 
 t=T->lchild; 
 T->lchild=T->rchild; 
 T->rchild=t; 
 if(T->lchild)
  Revolute(T->lchild); 
 if(T->rchild)
  Revolute(T->rchild); 
} 
void main()
{
 BiTree T;

 printf("�������У�\n");
 CreatBiTree(T);
 printf("\n");

 printf("�ݹ����������\n"); 
 PreOrderTraverse(T);
 printf("\n"); 

 printf("�ݹ����������\n"); 
 InOrderTraverse(T);
 printf("\n"); 

 printf("�ݹ���������\n"); 
 PostOrderTraverse(T); 
 printf("\n\n"); 
 
 printf("�ǵݹ����������\n"); 
 PreOrderTraverse2(T);
 printf("\n"); 

 printf("�ǵݹ����������\n"); 
 InOrderTraverse2(T);
 printf("\n"); 

 printf("�ǵݹ���������\n"); 
 PostOrderTraverse2(T); 
 printf("\n\n"); 

 printf("��������Ҷ�ӽ�����Ϊ:%d\n",Leafcount(T)); 
 printf("�������ĸ߶�Ϊ:%d\n",Depth(T)); 
}
