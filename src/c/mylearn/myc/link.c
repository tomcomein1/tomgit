#include <stdio.h>
#include <malloc.h>

#define ERROR 0
#define OK 1
#define EQUAL 1
#define LIST_INIT_SIZE 100
#define LISTINCREMENT 10

struct STU {
	char name [20];
	char stuno [10];
	int age;
	int score;
} stu[50];

typedef struct STU ElemType;

struct LNODE {
	ElemType data;
	struct LNODE *next;
};

typedef struct LNODE LNode;
typedef struct LNODE* LinkList;

int init (LinkList *L)
{
	*L = (LNode *) malloc (sizeof(LNode));
	if (!L) exit(ERROR);
	(*L)->next = NULL;
	return OK; 
}

void ListFree(LinkList L)
{
    LinkList p;
    
    while(L)
    {
        p=L->next;
        free(L);
        L=p;
    }
}

int ListLength(LinkList L)
{
	int j=0;
	while (L->next)
	{
		L=L->next;
		j++;
	}
	return j;
}

int GetElem(LinkList L, int i, ElemType *e)
{
	LinkList p;
	int j;
	
	p = L->next; j = 1;
	while (p&&j < i) {
		p = p->next ; ++j;
	}
	if (!p || j > 1 ) return ERROR;
	*e = p->data;
	return OK;
}

int EqualList(ElemType *e1, ElemType *e2)
{
	if (strcmp(e1->name, e2->name) == 0)
		return 1;
	else return 0;
}

int Less_EqualList(ElemType *e1,ElemType *e2)
{
  if (strcmp(e1->name,e2->name)<=0)
    return 1;
  else
    return 0;
}

int LocateElem(LinkList La,ElemType e,int type)
{
  int i;
  LinkList p;
  p=La;
  switch (type)
    {
      case EQUAL:
	  while(p->next)
	    {
	      p=p->next;
	      if(EqualList(&p->data,&e))
	       return 1;
	    }
	  return 0;
	break;
      default:
	break;
    }
  return 0;
}


void MergeList(LinkList La,LinkList Lb,LinkList *Lc)
{
  LinkList pa,pb,pc;
  pa=La->next;pb=Lb->next;
  *Lc=pc=La;
  while(pa && pb)
  {
    if(Less_EqualList(&pa->data,&pb->data))
	{
	  pc->next=pa;pc=pa;pa=pa->next;
	}
      else
	{
	  pc->next=pb;pc=pb;pb=pb->next;
	}
  }
  pc->next=pa?pa:pb;
  free(Lb);
}

int printlist(LinkList L)
{
  int i;
  LinkList p;
  p=L;
  printf("name       stuno        age     score\n");
  while(p->next)
    {
      p=p->next;
      printf("%-10s %s\t%d\t%d\n",  p->data.name,  p->data.stuno,
	  p->data.age,  p->data.score);
    }
  printf("\n");
}

int ListInsert(LinkList L,int i,ElemType e)
{
  LinkList p,s;
  int j;
  p=L;j=0;
  while(p&&j<i-1)
    {
      p=p->next;
      ++j;
    }
  if(!p||j>i-1) return ERROR;
  s=(LinkList)malloc(sizeof(LNode));
  s->data=e;
  s->next=p->next;
  p->next=s;
  return OK;
}/*ListInsert Before i */

void main(void)
{
  struct STU e;
  LinkList La,Lb,Lc;

  /* clrscr(); */

  printf("\n\n-------------------List Demo is running...----------------\n\n");
  printf("First is InsertList function.\n");
  init(&La);

  strcpy(e.name,"stu1");
  strcpy(e.stuno,"100001");
  e.age=80;
  e.score=1000;
  ListInsert(La,1,e);
  strcpy(e.name,"stu3");
  strcpy(e.stuno,"100002");
  e.age=80;
  e.score=1000;
  ListInsert(La,2,e);

  printlist(La);
  getchar(); 

  strcpy(e.name,"stu5");
  strcpy(e.stuno,"100003");
  e.age=80;
  e.score=1000;
  ListInsert(La,3,e);

  printlist(La);
  getchar();

  init(&Lb);

  strcpy(e.name,"stu2");
  strcpy(e.stuno,"100001");
  e.age=80;
  e.score=1000;
  ListInsert(Lb,1,e);
  strcpy(e.name,"stu4");
  strcpy(e.stuno,"100002");
  e.age=80;
  e.score=1000;
  ListInsert(Lb,2,e);

  strcpy(e.name,"stu6");
  strcpy(e.stuno,"100001");
  e.age=80;
  e.score=1000;
  ListInsert(Lb,3,e);

  printlist(Lb);
  getchar();

  MergeList(La, Lb, &Lc);
  printlist(Lc);
  ListFree(Lc);
  getchar();

  printf("\n\n\n\n\n\nWelcome to visit http://zmofun.heha.net !\n\n\n\n\n\n\n");

}

