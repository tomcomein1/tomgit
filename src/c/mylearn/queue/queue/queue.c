#include "queue.h"  
  
int init_queue(struct queue **q)  
{  
    (*q) =  (struct queue *)malloc(sizeof(struct queue));  
    if((*q) == NULL)  
    {  
        // �ڴ�����ʧ�ܣ�ֻ���˳��ķ���.  
        return 1;  
    }  
  
    // ����ֻ��������ͷ��βָ�����ڵ��ڴ�������ѣ�Ŀǰ��ͷ�ڵ㻹û�������أ���Ҫ��ͷָ���β��ָ��ָ��ͷ����  
    // ���濪ʼ����ͷ��㣬Ȼ����ͷβָ��ָ��ͷ��㣬��ɳ�ʼ��  
    (*q) -> head = (*q) -> tail = (struct queue_node *)malloc(sizeof(struct queue_node));  
    (*q) -> head -> next = NULL;  
    (*q) -> head ->data = NULL;  
    if((*q) == NULL)  
    {  
        // ͬ���ڴ�ʧ�ܣ�ֻ���˳��ˣ��ǵ�Ҳ�������ָ������Ϊ�����Ƴ�����Ҫ��ָ�븳���Ǹ���ϰ��  
        return 1;  
    }  
      
    printf("[DEBUG]���г�ʼ���ɹ�...\n");  
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
    printf("[DEBUG]�ͷ����...\n");  
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
    // ����������нڵ���ָ���������  
    struct queue_node *node;  
    node = (struct queue_node *)malloc(sizeof(struct queue_node));  
    if(node == NULL)  
    {  
        // ͬ�ϣ������˳���  
        return 1;  
    }  
    node -> next = NULL;  
    node -> data = data;  
  
    // ��ӵ�������ȥ���ǵ�����ӵ����ĵط���Ҳ����β��  
    (*q) -> tail -> next = node;  
    (*q) -> tail = node;  
    printf("[DEBUG][name = %s, age = %d]����ڵ�...\n", data -> name, data -> age);  
    return 0;  
}  
  
int queue_get(struct queue **q, struct business_data **data)  
{  
    struct queue_node *node;  
    if((*q) -> head == (*q) -> tail)   
    {  
        // ������ʵ��û�������ˣ�����Ӧ����ʱ��Ҳ������ȡ����ֵ�ģ��������ﷵ��0��  
        return 0;  
    }  
      
    node = (*q) -> head -> next;  
    // ֵȡ����.  
    (*data) = node -> data;  
  
    (*q) -> head -> next = node -> next;  
      
    // �����ǰֻ��һ����������ô��ǰ���node���ͷŵ�����β��ָ���ָ����NULL�������ⲽ��������  
    if((*q) -> tail == node)   
    {  
        (*q) -> tail = (*q) -> head;  
    }  
    free(node);  
    printf("[DEBUG][name = %s, age = %d]ȡ���ڵ�...\n", (*data) -> name, (*data) -> age);  
    return 0; 
}

