#include "queue.h"  
  
int init_queue(struct queue **q)  
{  
    (*q) =  (struct queue *)malloc(sizeof(struct queue));  
    if((*q) == NULL)  
    {  
        // 内存申请失败，只有退出的份了.  
        return 1;  
    }  
  
    // 这样只是申请了头和尾指针所在的内存区域而已，目前的头节点还没有申请呢，需要让头指针和尾巴指针指向头结点的  
    // 下面开始申请头结点，然后让头尾指针指向头结点，完成初始化  
    (*q) -> head = (*q) -> tail = (struct queue_node *)malloc(sizeof(struct queue_node));  
    (*q) -> head -> next = NULL;  
    (*q) -> head ->data = NULL;  
    if((*q) == NULL)  
    {  
        // 同样内存失败，只有退出了，记得也把上面的指针设置为空再推出，不要的指针赋空是个好习惯  
        return 1;  
    }  
      
    printf("[DEBUG]队列初始化成功...\n");  
    return 0;  
}  
  
int release_queue(struct queue **q)  
{  
    while((*q) -> head != NULL)  
    {  
        (*q) -> tail = (*q) -> head -> next;  
        free((*q) -> head);  
        (*q) -> head = (*q) ->tail;  
    }  
    free(*q);  
    printf("[DEBUG]释放完毕...\n");  
}  
  
void print_queue(struct queue **q)  
{  
    struct queue_node *node;  
    node = (*q) -> head;  
    while(node -> next != NULL)  
    {  
        printf("[name = %s,age = %d] -> ", node -> next ->data -> name, node ->next ->data -> age);  
        node = node -> next;  
    }  
    printf("\n");  
  
}  
  
int queue_put(struct queue **q, struct business_data *data)  
{  
    // 先申请个队列节点来指向这个数据  
    struct queue_node *node;  
    node = (struct queue_node *)malloc(sizeof(struct queue_node));  
    if(node == NULL)  
    {  
        // 同上，继续退出呗  
        return 1;  
    }  
    node -> next = NULL;  
    node -> data = data;  
  
    // 添加到队列中去，记得是添加到最后的地方，也就是尾部  
    (*q) -> tail -> next = node;  
    (*q) -> tail = node;  
    printf("[DEBUG][name = %s, age = %d]加入节点...\n", data -> name, data -> age);  
    return 0;  
}  
  
int queue_get(struct queue **q, struct business_data **data)  
{  
    struct queue_node *node;  
    if((*q) -> head == (*q) -> tail)   
    {  
        // 这里其实是没有数据了，但是应该有时候也是允许取道空值的，所以这里返回0吧  
        return 0;  
    }  
      
    node = (*q) -> head -> next;  
    // 值取走了.  
    (*data) = node -> data;  
  
    (*q) -> head -> next = node -> next;  
      
    // 如果当前只有一个数据域，那么此前如果node被释放掉，则尾部指针就指向了NULL，所以这步先来操作  
    if((*q) -> tail == node)   
    {  
        (*q) -> tail = (*q) -> head;  
    }  
    free(node);  
    printf("[DEBUG][name = %s, age = %d]取出节点...\n", (*data) -> name, (*data) -> age);  
    return 0; 
}

