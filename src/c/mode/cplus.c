//�̳���
typedef struct _parent  
{  
    int data_parent;  
  
}Parent;  
  
typedef struct _Child  
{  
    struct _parent parent;  
    int data_child;  
  
}Child;  
/*   �����C���Լ̳��Ե�ʱ��������Ҫ���ľ��ǰѻ������ݷ��ڼ̳еĽṹ����λ�ü��ɡ����������������ݵķ��ʡ����ݵ�ǿת�����ݵķ��ʶ�������ʲô���⡣ */

//��װ��

struct _Data;  
  
typedef  void (*process)(struct _Data* pData);  
  
typedef struct _Data  
{  
    int value;  
    process pProcess;  
      
}Data; 
/* ��װ�Ե��������ڣ������������ǰ���һ��ģ����ݺ������ǰ���һ��ġ����������ǾͿ���ͨ���򵥵�һ���ṹָ����ʵ����е����ݣ��������еĺ�������װ�ԣ�������ӵ�е����ԣ���ȻҲ�����ݽṹ��ӵ�е����ԡ� */

//��̬
typedef struct _Play  
{  
    void* pData;  
    void (*start_play)(struct _Play* pPlay);  
}Play;  

/*  ��̬������˵��ͬһ�Ľӿڴ��봦��ͬ�����ݡ�����˵�������Play�ṹ����һ��ͨ�õ����ݽṹ������Ҳ�����pData��ʲô���ݣ�start_play��ʲô�����������ǣ����Ǵ����ʱ��ֻҪ����pPlay->start_play(pPlay)�Ϳ����ˡ�ʣ�������������ǲ���Ҫ�ܣ���Ϊ��ͬ�Ľӿڻ��в�ͬ�ĺ���ȥ��������ֻҪѧ����þͿ����� */
