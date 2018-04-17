#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#define MAX_LINE_LEN 2048
typedef char * ElementType 

struct strfile{
	ElementType data;
	struct strfile *link;
};

typedef struct strfile NODE;
typedef NODE *LINK;

static LINK first; /*键表第一个结点*/

/*释放键表结点资源*/
void freeLink(LINK head)
{
	LINK plist;
	int i = 1;

	/*键表头空，不释放*/
	if (head == NULL) return;

	while (head)
	{
		plist = head->link;
		if (head->data)
		    free (head->data);
		free (head);
		head = plist; /*偏移到下一结点*/
	printf ("free %d\n", i++);
	}
}


/*建立一个链表*/
LINK creatLink(const char *buffer)
{
    LINK pNewNode;
    
    /*分配一个新节点数据*/
    if ((pNewNode = (LINK)malloc(sizeof(NODE)))!=NULL)
    {
       if (buffer)
       {
	   pNewNode->data = (char *)malloc (strlen(buffer) + 1);
	   if (pNewNode->data)
               strncpy (pNewNode->data, buffer, strlen(buffer) );
       }
       pNewNode->link = NULL;
       return pNewNode;
    }
    else return NULL;
}

/*显示链表*/
void printLink(LINK head)
{
    LINK current = head;
    for( ; current; current=current->link)
        printf("%s",  current->data);
}

/*取得链表资源长度*/
int getLinkLength(LINK head)
{
    LINK plist = head;
    int len = 0;

    for (; plist; plist=plist->link)
	len += strlen(plist->data);

    return len;
}

/*查找字符数据*/
int findLinkData(LINK head, const char *str)
{
    LINK ptr = head;
    
    for (; ptr; ptr=ptr->link)
	if(strncmp(ptr->data, str, strlen(str)) == 0)
	    return 1;

    return 0;
}

/*记录链表数据到文件*/
int writeLink(LINK head, FILE *fw)
{
	LINK ptr = head;
	for (; ptr; ptr=ptr->link)
		if (fwrite (ptr, sizeof(NODE), 1, fw) <= 0) return -1;
	return 0;
}

/*取一行数据*/
int getLine(char * sline, FILE * fp)
{
    char ch=0;
    long nRec=0;

    #ifdef UNIX
    while(ch != '\n' && ch != EOF)
    #else
    while(ch != '\n' && !feof(fp))
    #endif
    {
        if (nRec >= MAX_LINE_LEN) break;
        ch=getc(fp);
        sline[nRec++]=ch;
    }

    if(nRec >= MAX_LINE_LEN) return -1;

    sline[nRec]='\0';
    if(ch == '\n') return 1;
    #ifdef UNIX
      if(ch == EOF) return 0;
    #else
      if(feof(fp)) return 0;   
    #endif

    return -1;
}

int insertList(LINK L,int i, const char *e)
{

   LINK p=L, s;
   int j=0;

   while(p && j<i-1)
   {
       p=p->link;
       ++j;
   }

   if(!p || j>i-1) return -1;

   s=(LINK)malloc(sizeof(NODE));
   if (s)
   {
       s->data = (char *) malloc (strlen(e)) ;
       if (s->data)
           strcpy(s->data, e);
   }
   else return -1;

   s->link = p->link;
   p->link = s;

   return 1;
}

/* delete list node */
int deleteList(LINK L, int i, char *e)
{
    LINK p=L, q;
    int j=0;

    while(p && j<i-1)
    {
        p=p->link;
        ++j;
    }

    if(!p->link || j>i-1) 
       return -1;

    q = p->link;
    p->link = q->link;
    strcpy(e, q->data);
    if (q->data)
	free (q->data);
    free(q);

    return 1;
}


void MergeList(LINK La, LINK Lb, LINK Lc)
{

/*已知单链线性表La和Lb的元素按值非递减排列 */
/*归并后得到新的单链线性表Lc,元素也按值非递减排列 */

   LINK pa = La->link; 
   LINK pb=Lb->link;
   LINK pc;

   Lc=pc=La;

   while(pa&&pb)
   {
        if(pa->data<=pb->data)
        {
            pc->link=pa;pc=pa;pa=pa->link;
        }
        else
        {
            pc->link=pb;pc=pb;pb=pb->link;
        }
    }
    pc->link=pa?pa:pb;
    free(Lb);
}


int main(int argc, char *argv[])
{
	char sLine[MAX_LINE_LEN];
	FILE *fp;
	LINK ptr=NULL, node;
	int npos;
	FILE *fw;
	int row = 1;

	if ((fp = fopen("test.txt", "r")) == NULL)
	{
		printf ("open file [test.txt] error!\n" );
		return -1;
	}

	while (1)
	{
		memset (sLine, 0x00, MAX_LINE_LEN);
		if (getLine(sLine, fp) <= 0)
			break;

		/** npos = strlen(sLine);
		while(sLine[--npos] == '\n') sLine[npos] = '\0';
                **/

		/** node = creatLink (sLine);

		if (ptr)
		{
		   ptr->link = node;
                   ptr = ptr->link;
		}
		else first = ptr = node;
		***/

	        if (first == NULL)
                     first = creatLink (sLine);
		else insertList (first, row++, sLine);
	}

	printf ("link length: %d\n", getLinkLength(first) );
	printf ("find %d\n", findLinkData(first, "fdsfa") );

	/* deleteList(first, row >> 1, sLine); */

	printLink(first);

	/*** if ((fp = fopen("output.txt", "wb")) == NULL)
	{
		printf ("open file [output.txt] error!\n" );
		return -1;
	} 

	writeLink(first, fw);  ***/

	freeLink(first); 

	fclose (fp);	
	/* fclose (fw);	*/

	return 0;
}

