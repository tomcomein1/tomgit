#define MAXSIZE 100   
#include<stdio.h>   
#include<string.h>   
#include<stdlib.h>   
typedef struct CodeDict   
{   
    int code;   
    char * Dict_Elem;   
}Code_Dict;
   
typedef struct Dictionary   
{   
  Code_Dict  DictBody[MAXSIZE];   
  int cur_pos;   
  int maxsize;   
}Dictionary;
   
typedef struct LinkNode   
{   
    char data;   
    struct  LinkNode *next;   
}LinkNode;   

typedef struct LinkBody   
{   
    LinkNode *head;   
    int size;   
}Link;   
   
char Input[MAXSIZE];   
Dictionary Dict;   
   
   
bool CreateLink(Link &link)   
{   
    if(link.head=(LinkNode *)malloc(sizeof(LinkNode)))   
    {   
        link.head ->data =' ';   
        link.head ->next =NULL;   
        link.size =0;   
        return true;   
    }   
    else return false;   
}   
void Find_or_Insert(Link &link, char input)   
{   
         /*�������в���Input[i]Ԫ�أ���������������������*/   
    LinkNode *pre,*p,*q;   
    pre=NULL;p=link.head;   
    while(p)   
    {   
        //����������������Ƿ�����inputƥ���Ԫ��   
        if(p->data<input)   
        {   
            pre=p;   
            p=p->next;   
        }   
        else    
            break;   
   
    }/*ѭ������*/   
   if(p==NULL||p->data >input)   
   {   
       pre->next =(LinkNode *)malloc(sizeof(LinkNode));   
       pre->next ->data =input;/*��Ҫ����Ϊ��Ԫ�ظ�ֵ*/   
       pre=pre->next ;   
       pre->next =p;   
       link.size ++;   /*�������в������Ԫ��*/   
   }   
   else   
   {   
       ;/*˵���������Ѿ�����Input���Ԫ��*/   
   }   
}   
InsertOrder(Link &result)   
{   
/* �����ҹ����ظ�Ԫ��*/   
 int i,j,len;   
 if(CreateLink(result))   
 {   
     //��ڼ���������Ƿ�ɹ�   
   for(i=0;i<strlen(Input);i++)   
   {   
       if(Input[i]!='\0')   
       {   
          Find_or_Insert(result,Input[i]);   
               /*�������в���Input[i]Ԫ�أ���������������������*/   
       }   
   }   
     
 }   
 else   
 {   
     printf("The memeory dosen't alloc rightly");   
     exit(0);   
 }   
}//InsertOrder   
void InitDict( Link link)   
{   
    /*���ݴ������������У���ʼ���ֵ�*/   
    LinkNode *pre ,*p;   
    int pos=0;   
    Dict.cur_pos=0;   
    p=link.head ->next ;   
    while(p)   
    {   
      Dict.DictBody [pos].code =pos;   
       Dict.DictBody [pos].Dict_Elem=(char *) malloc(2*sizeof(char));   
       Dict.DictBody [pos].Dict_Elem[0]=p->data ;   
       Dict.DictBody [pos].Dict_Elem[1]='\0';/*�����ַ�Ҫע��һ��Ҫע�����β�ַ����������ܵ����ַ�������ؿ⺯��*/   
           Dict.cur_pos++; pos++;   
            p=p->next ;         
    }   
        Dict.maxsize =1;/*��������������Ϊ1*/   
}   
void TraveList(Link link)   
{   
    LinkNode *p;   
    p=link.head ;   
    while(p)   
    {   
        printf("%c ",p->data );   
        p=p->next;   
    }   
    printf("\n");   
//  printf("The count is %d",link.size  );   
}   
void TravelDict()   
{   
    for(int i=0;i<Dict.cur_pos;i++)   
    {   
        printf("%d ",Dict.DictBody [i].code );   
        puts(Dict.DictBody[i].Dict_Elem );   
        //printf("\n");   
    }   
}   
void InputPro()   
{   
Link result;   
InsertOrder(result);//������Ԫ�ؽ�������,�������ظ�Ԫ��.   
//TraveList(result);       
InitDict(result);//���������õ������ظ�Ԫ�ذ�˳������ֵ�   
//TravelDict();   
}   
   
int Find_in_Dict(char *s)   
{   
    /*���ֵ��в��ң����ҵ����ҵ��������ֵ��е�λ�ã����򷵻�-1*/   
    int res;   
 for(int i=0;i<Dict.cur_pos;i++)   
 {   
     if(( res=strcmp(s,Dict.DictBody[i].Dict_Elem ))!=0);   
         else    
         {   
               
              return i;/*�ҵ������������ֵ��е�λ��*/   
         }   
   
 }   
       if(i>=Dict.cur_pos)   
                 return -1;/*�Ҳ���������-1*/   
   
}   
void Insert_Dict(char *s,int len)   
{/*���ֵ��в�����Ԫ��*/   
int pos=Dict.cur_pos++ ,k ;   
Dict.DictBody [pos].code=pos;   
Dict.DictBody [pos].Dict_Elem =(char*)malloc((len)*sizeof(char));   
strcpy(Dict.DictBody [pos].Dict_Elem,s);/*�˴��ȽϿ���,�Ժ����*/   
Dict.DictBody [pos].Dict_Elem [len-1]='\0';   
if(Dict.maxsize <len)   
     Dict.maxsize =len;   
}   
void FreeDict()   
{/*�ͷ�Ϊ�ֵ����Ŀռ�*/   
    for(int i=0;i<Dict.cur_pos ;i++)   
    {      
        free(Dict.DictBody[i].Dict_Elem );   
       
    }   
}   
/*LZWѹ���㷨*/   
void LZW_Compress()   
{   
char *s,c;   
int len_input;   
len_input=strlen(Input);   
s=(char *)malloc((Dict.maxsize+2)*sizeof(char));   
s[Dict.maxsize +1]='\0';   
s[0]=Input[0];   
for(int j=1,i=1;i<len_input;i++)   
{   
 c=Input[i];s[j++]=c;s[j]='\0';   
 if(Find_in_Dict(s)>=0)   
 {   
  ;   
 }   
else   
 if(Find_in_Dict(s)==-1)   
{   
s[j-1]='\0';   
printf(" %d",Find_in_Dict(s));/*���ѹ����Ĵ���*/   
s[j-1]=c;s[j]='\0';   
Insert_Dict(s,j+1);   
j=0;   
free(s);/*�ͷ�S*/   
s=(char*)malloc((Dict.maxsize +2)*sizeof(char));   
s[Dict.maxsize +1]='\0';   
/*����Ϊs����ռ�*/   
s[j++]=c;   
}   
}//for ends   
   
if(j>=1)   
{   
s[j]='\0';   
printf(" %d\n",Find_in_Dict(s));   
}   
free(s);   
}/*the ends*/   
   
void main()   
{   
Dict.cur_pos =0;   
printf("Please Input the String\n");   
gets(Input);   
if(strlen(Input)!=0)   
{   
InputPro();   
printf("The LZW output is:\n");   
LZW_Compress();   
printf("And the dictionary is:\n");   
TravelDict();   
 FreeDict();   
}   
else   
{   
    printf("Nothing has been input!\n");   
}   
}