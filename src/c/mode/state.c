/*    状态模式是协议交互中使用得比较多的模式。比如说，在不同的协议中，都会存在启动、保持、中止等基本状态。那么怎么灵活地转变这些状态就是我们需要考虑的事情。假设现在有一个state，
**/

truct _State  
{  
    void (*process)();  
    struct _State* (*change_state)();  
  
}State;

void normal_process()  
{  
    printf("normal process!\n");  
}  

struct _State* change_state()  
{  
    State* pNextState = NULL;  
  
    pNextState = (struct _State*)malloc(sizeof(struct _State));  
    assert(NULL != pNextState);  
  
    pNextState->process = next_process;  
    pNextState->change_state = next_change_state;  
    return pNextState;  
}  

typedef struct _Context  
{  
    State* pState;  
    void (*change)(struct _Context* pContext);  
      
}Context;  
  
void context_change(struct _Context* pContext)  
{  
    State* pPre;  
    assert(NULL != pContext);  
  
    pPre = pContext->pState;  
    pContext->pState = pPre->changeState();  
    free(pPre);  
    return;     
}  


