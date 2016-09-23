//继承性
typedef struct _parent  
{  
    int data_parent;  
  
}Parent;  
  
typedef struct _Child  
{  
    struct _parent parent;  
    int data_child;  
  
}Child;  
/*   在设计C语言继承性的时候，我们需要做的就是把基础数据放在继承的结构的首位置即可。这样，不管是数据的访问、数据的强转、数据的访问都不会有什么问题。 */

//封装性

struct _Data;  
  
typedef  void (*process)(struct _Data* pData);  
  
typedef struct _Data  
{  
    int value;  
    process pProcess;  
      
}Data; 
/* 封装性的意义在于，函数和数据是绑在一起的，数据和数据是绑在一起的。这样，我们就可以通过简单的一个结构指针访问到所有的数据，遍历所有的函数。封装性，这是类拥有的属性，当然也是数据结构体拥有的属性。 */

//多态
typedef struct _Play  
{  
    void* pData;  
    void (*start_play)(struct _Play* pPlay);  
}Play;  

/*  多态，就是说用同一的接口代码处理不同的数据。比如说，这里的Play结构就是一个通用的数据结构，我们也不清楚pData是什么数据，start_play是什么处理函数？但是，我们处理的时候只要调用pPlay->start_play(pPlay)就可以了。剩下来的事情我们不需要管，因为不同的接口会有不同的函数去处理，我们只要学会调用就可以了 */
