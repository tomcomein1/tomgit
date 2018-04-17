#include <stdio.h>

#define MAX_SIZE 300
typedef int Element ;

struct stack{
	int top;
	int lock;
	Element sta[MAX_SIZE] ;	
}st;

void initStatck()
{
	int i ;
	st.top = -1;
	st.lock = 0;
	for ( ; i<MAX_SIZE; i++)
		st.sta[i] = 0;
}

void setlock()
{
	st.lock = 1;
}

int islock()
{
	return st.lock ;
}

void freelock()
{
	st.lock = 0;
}

void push(Element data)
{
	if (islock()) return;
	if (st.top > MAX_SIZE-1) return;
	
	setlock();
	st.sta[++st.top] = data;
	freelock();
}

int isNull()
{
	return ( st.top == -1 );
}

Element pop()
{
	Element data;

	if (islock()) return -1;
	if (isNull()) return -1;
	setlock();
	 
	data = st.sta[st.top--];
	freelock();
	return data;
}

Element getStack()
{
	return st.sta[st.top];
}

/*Ë³Ðò²éÕÒ·¨²éÕÒ*/
Element findStack(Element x)
{
	int i = st.top;
	if (isNull() ) return -1;
	for (;i>=0; i--)
		if (st.sta[i] == x) return i;
	
}

void printStack()
{
	int i = st.top;
	for ( ; i >= 0; i-- ) 
		printf ("%d ", st.sta[i] );
	printf ("\n");
}

main()
{
	int i = 0;

	initStatck();
	for (i=32; i<200; i++)
		push (i);
	printStack();
	printf (" %d \n", pop() );
	printf (" %d \n", getStack());
	
}

