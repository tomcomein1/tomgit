#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*key = (tag % constant ) 其中 constant 为质数 ， 并且小于哈希表容量 。这样哈希值就分布的比较均匀，效率就不会受影响。*/

#define CONSTANT 7

typedef struct _NODE {
	int data;
	struct _NODE* next;
}NODE;


typedef struct _HASH_TABLE {
	NODE* value[CONSTANT];
}HASH_TABLE;

//创建hash表
HASH_TABLE* create_hash_table()
{
	HASH_TABLE* pHashTbl = (HASH_TABLE*)malloc(sizeof(HASH_TABLE));
	memset(pHashTbl, 0, sizeof(HASH_TABLE) );
	return pHashTbl;
}

//在hash表中查找数据
NODE* find_data_in_hash(HASH_TABLE* pHashTbl, int data) 
{
	NODE* pNode;
	if (NULL == pHashTbl)
		return NULL;
	
	if(NULL== (pNode=pHashTbl->value[data % CONSTANT]))
		return NULL;
	while(pNode) {
		if(data==pNode->data)
			return pNode;
		pNode=pNode->next;
	}
	return NULL;
}

//在hash表当中插入数据
int insert_data_into_hash(HASH_TABLE* pHashTbl, int data)
{
	NODE* pNode;
	if (NULL==pHashTbl)
		return -1;
	if (NULL==pHashTbl->value[data % CONSTANT]) {
		pNode=(NODE*)malloc(sizeof(NODE));
		memset(pNode, 0, sizeof(NODE));
		pNode->data = data;
		pHashTbl->value[data % CONSTANT]=pNode;
		return 0;
	}

	if (NULL != find_data_in_hash(pHashTbl, data))
		return -1;
	pNode = pHashTbl->value[data % CONSTANT];
	while(NULL != pNode->next)
		pNode=pNode->next;

	pNode->next = (NODE*)malloc(sizeof(NODE));
	memset(pNode->next, 0, sizeof(NODE));
	pNode->next->data = data;
	return 0;	
}
 

//从hash表删除娄据
int delete_data_from_hash(HASH_TABLE* pHashTbl, int data)  
{  
    NODE* pHead;  
    NODE* pNode;  
    if(NULL == pHashTbl || NULL == pHashTbl->value[data % CONSTANT])  
        return -1;  
  
    if(NULL == (pNode = find_data_in_hash(pHashTbl, data)))  
        return -1;  
  
    if(pNode == pHashTbl->value[data % CONSTANT]){  
        pHashTbl->value[data % CONSTANT] = pNode->next;  
        goto final;  
    }  
  
    pHead = pHashTbl->value[data % CONSTANT];  
    while(pNode != pHead ->next)  
        pHead = pHead->next;  
    pHead->next = pNode->next;  
  
final:  
    free(pNode);  
    return 0;  
}  

