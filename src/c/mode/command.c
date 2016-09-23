//ÃüÁîÄ£Ê½

typedef struct _Post  
{  
    void (*do)(struct _Post* pPost);     
}Post;  

struct _Command  
{  
    void* pData;  
    void (*exe)(struct _Command* pCommand);  
  
}Command;  
  
void post_exe(struct _Command* pCommand)  
{  
    assert(NULL != pCommand);  
   
    (Post*)(pCommand->pData)->do((Post*)(pCommand->pData));  
    return;  
}  

typedef struct _Boss  
{  
    Command* pCommand;  
    void (*call)(struct _Boss* pBoss);  
}Boss;  
  
void boss_call(struct _Boss* pBoss)  
{  
    assert(NULL != pBoss);  
  
    pBoss->pCommand->exe(pBoss->pCommand);  
    return;  
}  

