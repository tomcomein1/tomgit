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
               T->data=ch;//���ɸ��ڵ�
                CreatBiTree(T->lchild);//����������
                CreatBiTree(T->rchild);//����������
        }
}
void preorder(BiTree T)//ǰ�����
{
        if (T!=NULL){
                printf ("%c",T->data);
                preorder(T->lchild);
                preorder(T->rchild);
  }
}
void inorder(BiTree T)//�������
{
        if (T!=NULL){ 
       inorder(T->lchild);
                printf ("%c",T->data);
                inorder(T->rchild);
        }
}
void postorder(BiTree T)//�������
{
        if (T!=NULL){
                postorder(T->lchild);
                postorder(T->rchild);
    printf ("%c",T->data);
  }
}
void main ()
{
cout<<"������Ҫ�����Ķ����������ո�:"<<endl ;
  BiTree  T;
  CreatBiTree(T);//����������
cout<<"ǰ������Ľ��Ϊ:"<<endl;
  preorder(T);
cout<<endl;
 cout<<"��������Ľ��Ϊ:"<<endl;
 inorder(T);
 cout<<endl;
 cout<<"��������Ľ��Ϊ:"<<endl;
 postorder(T);
}