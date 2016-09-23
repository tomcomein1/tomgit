#include <iostream>

using namespace std;

void print(int a[], int n ,int i){  
    cout<<"��"<<i+1 <<"�� : ";  
    for(int j= 0; j<8; j++){  
        cout<<a[j] <<"  ";  
    }  
    cout<<endl;  
}  
/** 
 * �������Сֵ 
 * 
 * @return int ����ļ�ֵ 
 */  
int SelectMinKey(int a[], int n, int i)  
{  
    int k = i;  
    for(int j=i+1 ;j< n; ++j) {  
        if(a[k] > a[j]) k = j;  
    }  
    return k;  
}  
  
/** 
 * ѡ������ 
 * 
 */  
void selectSort(int a[], int n){  
    int key, tmp;  
    for(int i = 0; i< n; ++i) {  
        key = SelectMinKey(a, n,i);           //ѡ����С��Ԫ��  
        if(key != i){  
            tmp = a[i];  a[i] = a[key]; a[key] = tmp; //��СԪ�����iλ��Ԫ�ػ���  
        }  
        print(a,  n , i);  
    }  
}  

/** ��ѡ������ĸĽ�������Ԫѡ������ **/
void SelectSort(int r[],int n) {  
    int i ,j , min ,max, tmp;  
    for (i=1 ;i <= n/2;i++) {    
        // ��������n/2��ѡ������   
        min = i; max = i ; //�ֱ��¼������С�ؼ��ּ�¼λ��  
        for (j= i+1; j<= n-i; j++) {  
            if (r[j] > r[max]) {   
                max = j ; continue ;   
            }    
            if (r[j]< r[min]) {   
                min = j ;   
            }     
      }    
      //�ý����������ɷ�������������Ч��  
      tmp = r[i-1]; r[i-1] = r[min]; r[min] = tmp;  
      tmp = r[n-i]; r[n-i] = r[max]; r[max] = tmp;   
  
    }   
}  

int main(){  
    int a[8] = {3,1,5,7,2,4,9,6};  
    cout<<"��ʼֵ��";  
    for(int j= 0; j<8; j++){  
        cout<<a[j] <<"  ";  
    }  
    cout<<endl<<endl;  
    //selectSort(a, 8);  
    SelectSort(a, 8);  
    print(a,8,8);  
}  
