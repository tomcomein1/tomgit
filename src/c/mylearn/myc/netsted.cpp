//:nested.h 

struct stack{
    struct link {
        void *data;
        link *next;
        void initialize(void *Data, link *Next);
    } *head;
void initialize();
void push (void *Data);
void *peek();
void *pop();
void cleanup();
};

#include <stdlib.h>
#include <assert.h>

void stack::link::initialize(
    void *Data,link *Next) {
    data = Data;
    next = Next;
}
   
void stack::initialize(){head = 0;}

void stack::push(void *Data){
    link *newlink = (link *) malloc (sizeof(link));
    assert(newlink);
    newlink->initialize(Data, head);
    head = newlink;
}

void *stack::peek() {return head->data;}
 
void *stack::pop(){
    if (head == 0) return 0;
    void *result = head->data;
    link *oldHead = head;
    head = head->next;
    free(oldHead);
    return result;
}

void stack::cleanup(){
    link *cusor = head;
    while(head) {
        cursor = cursor->next;
        free(head->data);
        free(head);
        head = cursor;
    }
}
