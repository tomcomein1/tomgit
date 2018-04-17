#include<iostream.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct node {
       char data;
       struct node *lchild,*rchild;//
 }BiTNode,*BiTree;
 
void CreatBiTree(BiTree &T)
{
        char ch;
        ch=getchar();
        if (ch == ' ')
          T = 0;
        else {
               T=(BiTNode*)malloc(sizeof(BiTNode));
               T->data=ch;//生成根节点
                CreatBiTree(T->lchild);//构造左子树
                CreatBiTree(T->rchild);//构造右子树
        }
}
void preorder(BiTree T)//前序遍历
{
        if (T!=NULL){
                printf ("%c",T->data);
                preorder(T->lchild);
                preorder(T->rchild);
  }
}
void inorder(BiTree T)//中序遍历
{
        if (T!=NULL){ 
       inorder(T->lchild);
                printf ("%c",T->data);
                inorder(T->rchild);
        }
}
void postorder(BiTree T)//后序遍历
{
        if (T!=NULL){
                postorder(T->lchild);
                postorder(T->rchild);
    printf ("%c",T->data);
  }
}
void main ()
{
cout<<"请输入要创建的二叉树包括空格:"<<endl ;
  BiTree  T;
  CreatBiTree(T);//创建二叉树
cout<<"前序遍历的结果为:"<<endl;
  preorder(T);
cout<<endl;
 cout<<"中序遍历的结果为:"<<endl;
 inorder(T);
 cout<<endl;
 cout<<"后序遍历的结果为:"<<endl;
 postorder(T);
}