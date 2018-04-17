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
         /*在链表中查找Input[i]元素，如果有则跳过，否则插入*/   
    LinkNode *pre,*p,*q;   
    pre=NULL;p=link.head;   
    while(p)   
    {   
        //遍历整个链表，检查是否有与input匹配的元素   
        if(p->data<input)   
        {   
            pre=p;   
            p=p->next;   
        }   
        else    
            break;   
   
    }/*循环结束*/   
   if(p==NULL||p->data >input)   
   {   
       pre->next =(LinkNode *)malloc(sizeof(LinkNode));   
       pre->next ->data =input;/*不要忘记为新元素赋值*/   
       pre=pre->next ;   
       pre->next =p;   
       link.size ++;   /*在链表中插入这个元素*/   
   }   
   else   
   {   
       ;/*说明链表中已经存在Input这个元素*/   
   }   
}   
InsertOrder(Link &result)   
{   
/* 排序并且过滤重复元素*/   
 int i,j,len;   
 if(CreateLink(result))   
 {   
     //入口检查链表创建是否成功   
   for(i=0;i<strlen(Input);i++)   
   {   
       if(Input[i]!='\0')   
       {   
          Find_or_Insert(result,Input[i]);   
               /*在链表中查找Input[i]元素，如果有则跳过，否则插入*/   
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
    /*根据处理后的输入序列，初始化字典*/   
    LinkNode *pre ,*p;   
    int pos=0;   
    Dict.cur_pos=0;   
    p=link.head ->next ;   
    while(p)   
    {   
      Dict.DictBody [pos].code =pos;   
       Dict.DictBody [pos].Dict_Elem=(char *) malloc(2*sizeof(char));   
       Dict.DictBody [pos].Dict_Elem[0]=p->data ;   
       Dict.DictBody [pos].Dict_Elem[1]='\0';/*对于字符要注意一般要注意其结尾字符，这样才能调用字符串的相关库函数*/   
           Dict.cur_pos++; pos++;   
            p=p->next ;         
    }   
        Dict.maxsize =1;/*编码的最长长度现在为1*/   
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
InsertOrder(result);//对输入元素进行排序,并过滤重复元素.   
//TraveList(result);       
InitDict(result);//将上面所得到的无重复元素按顺序放入字典   
//TravelDict();   
}   
   
int Find_in_Dict(char *s)   
{   
    /*在字典中查找，若找到若找到返回在字典中的位置，否则返回-1*/   
    int res;   
 for(int i=0;i<Dict.cur_pos;i++)   
 {   
     if(( res=strcmp(s,Dict.DictBody[i].Dict_Elem ))!=0);   
         else    
         {   
               
              return i;/*找到，返回其在字典中的位置*/   
         }   
   
 }   
       if(i>=Dict.cur_pos)   
                 return -1;/*找不到，返回-1*/   
   
}   
void Insert_Dict(char *s,int len)   
{/*向字典中插入新元素*/   
int pos=Dict.cur_pos++ ,k ;   
Dict.DictBody [pos].code=pos;   
Dict.DictBody [pos].Dict_Elem =(char*)malloc((len)*sizeof(char));   
strcpy(Dict.DictBody [pos].Dict_Elem,s);/*此处比较可疑,以后检验*/   
Dict.DictBody [pos].Dict_Elem [len-1]='\0';   
if(Dict.maxsize <len)   
     Dict.maxsize =len;   
}   
void FreeDict()   
{/*释放为字典分配的空间*/   
    for(int i=0;i<Dict.cur_pos ;i++)   
    {      
        free(Dict.DictBody[i].Dict_Elem );   
       
    }   
}   
/*LZW压缩算法*/   
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
printf(" %d",Find_in_Dict(s));/*输出压缩后的代码*/   
s[j-1]=c;s[j]='\0';   
Insert_Dict(s,j+1);   
j=0;   
free(s);/*释放S*/   
s=(char*)malloc((Dict.maxsize +2)*sizeof(char));   
s[Dict.maxsize +1]='\0';   
/*重新为s分配空间*/   
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