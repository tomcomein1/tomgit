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
 struct BiTNode *lchild,*rchild; //左右孩子指针
}BiTNode,*BiTree;

typedef struct//栈的定义
{
 struct BiTNode  *base;
 struct BiTNode   *top;
 int stacksize;
}SqStack;

Status Initstack(SqStack &S) //构造一个空栈
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

Status Push(SqStack &S,BiTree e)  // 进栈
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

Status Pop(SqStack &S,BiTree &e)//出栈
{
 if(S.top == S.base )return(ERROR);
  S.top--;
   e=S.top; 
 return OK; 
}

Status GetTop(SqStack S,BiTree &e)//取栈顶元素
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
   printf("文件打开失败！\n");
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

Status CreatBiTree(BiTree &T)//先序创建二叉链表T，#代表空树 
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

Status PreOrderTraverse(BiTree T)//递归先根遍历 
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

Status InOrderTraverse(BiTree T)//递归中根遍历 
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


Status PostOrderTraverse(BiTree T)//递归后根遍历 
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

Status PreOrderTraverse2(BiTree T)//非递归先根遍历
{
 SqStack S;
 BiTree p;
 Initstack(S);
 Push(S,T);
 while(!StackEmpty(S))
 {
  while(GetTop(S,p) && p)//栈不空GetTop(S,p)返回OK，P返回栈顶元素，不为NULL
  {
   Visit(p->data);
    Push(S,p->lchild);
  }
  Pop(S,p);//空指针退栈
  if(!StackEmpty(S))
  {
   Pop(S,p);
    Push(S,p->rchild);      
  }
  
 }
 return OK;
}

Status InOrderTraverse2(BiTree T)//非递归中根遍历 
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

Status PostOrderTraverse2(BiTree T)//非递归后根遍历
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

Status Leafcount(BiTree T)//计算叶子节点 
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

Status Depth(BiTree T)//树的高度 
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

void Revolute(BiTree &T)// 交换左右子树 
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

 printf("先序序列：\n");
 CreatBiTree(T);
 printf("\n");

 printf("递归先序遍历：\n"); 
 PreOrderTraverse(T);
 printf("\n"); 

 printf("递归中序遍历：\n"); 
 InOrderTraverse(T);
 printf("\n"); 

 printf("递归后序遍历：\n"); 
 PostOrderTraverse(T); 
 printf("\n\n"); 
 
 printf("非递归先序遍历：\n"); 
 PreOrderTraverse2(T);
 printf("\n"); 

 printf("非递归中序遍历：\n"); 
 InOrderTraverse2(T);
 printf("\n"); 

 printf("非递归后序遍历：\n"); 
 PostOrderTraverse2(T); 
 printf("\n\n"); 

 printf("二叉树的叶子结点个数为:%d\n",Leafcount(T)); 
 printf("二叉树的高度为:%d\n",Depth(T)); 
}
