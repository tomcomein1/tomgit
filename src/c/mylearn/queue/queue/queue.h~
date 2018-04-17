#if !defined(_QUEUE_H)

#include <stdlib.h>
#include <stdio.h>

// 定义数据域的数据结构，这里为了简单期间数据域有2个简单的属性组成。  
struct business_data  
{  
    char name[50];  
    int age;  
};  
  
// 定义队列的节点数据节点  
struct queue_node  
{  
    // 为了节约运行时期的内存，我们这里用指针来指向业务数据的节点，具体使用请根据实际情况来  
    struct queue_node *next; // 指向下一个节点  
    struct business_data *data; // 指向数据域  
};  
  
// 定义队列的头和尾，头尾在一起的，当队列空的时候头和尾巴都是指向头结点的啦，插入的时候在后面插入，取的时候在前面取，我们通常说的  
// 队列先进后出我们可能都理解成为在头部进，尾巴出了，其实尾巴进，头部出也是完全符合队列的性质的哈.  
struct queue  
{  
    struct queue_node *head; // 头指针  
    struct queue_node *tail; // 尾巴指针  
};  
  
// 定义一些操作，四个操作就够了，初始化、回收、插入、取..  
// 因为这个是纯c的代码，c里面不存在真正引用传递，所以这里会出现很多二重指针来实现引用传递的功能。  
  
/* 初始化 */  
int init_queue(struct queue **q);  
  
/* 回收 */  
int release_queue(struct queue **q);  
  
/* 插入 */  
int queue_put(struct queue **q, struct business_data *data);  
  
/* 取数据 */  
int queue_get(struct queue **q, struct business_data **data);  
  
// 再来一个辅助的打印查看数据的函数吧  
void queue_print(struct queue **q);  

#define _QUEUE_H
#endif

