#include <stdio.h>
#include <stdlib.h>
#define  SPICY_FOOD 1
#define  STRONG_SMELL_FOOD 2 

#define WEST (1)
#define EAST (2)

typedef struct _Tofu {
	int type;
	void (*eat)(struct _Visitor* pVisitor, struct _Tofu* pToFu);
}Tofu;

typedef struct _Visitor{
	int region;
	void (*process)(struct _Tofu* pTofu, struct _Visitor* pVistor);
}Visitor;


void eat(struct _Visitor* pVisitor, struct _Tofu* pTofu) {
	assert(NULL != pVistor && NULL != pTofu);
	pVisitor->process(pTofu, pVisitor);
}

void process(struct _Tofu* pTofu, struct _Visitor* pVistor) {
	assert(NULL != pVistor && NULL != pTofu);
	
	if (pTofu->type == SPICY_FOOD && pVistor->region == WEST ||
	pTofu->type == STRONG_SMELL_FOOD && pVisitor->region == EAST) {
		printf ("I like this food!\n");
		return ;
	}

	printf("I hate this food!\n");
}

int main(void)
{
	return 0;
}
