#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct STASHtag{
    int size; /*Size of each space*/
    int quantity; /* Number of storage spaces */
    int next; /* next empty space */
    /*Dynamically allocated array of bytes:*/
    unsigned char *storage;
}Stash;

void initialize(Stash *s, int Size)
{
    s->size = Size;
    s->quantity = 0;
    s->storage = 0;
    s->next = 0;
}

void cleanup(Stash *s) 
{
    if (s->storage) {
        puts("freeing storage");
        free(s->storage);
    }
}

int add(Stash *s, void *element)
{
    /*enough space left ?*/
    if (s->next >= s->quantity)
        inflate(s, 100);
    /*Copy element into storage*/
    memcpy(&(s->storage[s->next * s->size]),
        element, s->size);
    s->next++;
    
    return (s->next - 1);  /*Index number*/
}

void *fetch(Stash *s, int index)
{
    if (index >= s->next || index < 0)
        return 0;    /*not out of bounds?*/
    /*produce pointer to desired element:*/
    return &(s->storage[index * s->size]);
            
}

int count(Stash *s)
{
    return s->next;
}

void inflate(Stash *s, int increase)
{
    void *v = 
        realloc(s->storage,
            (s->quantity + increase)
            * s->size);
    assert(v);
    s->storage = v;
    s->quantity += increase;
}
