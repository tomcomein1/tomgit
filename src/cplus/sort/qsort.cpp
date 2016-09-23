#include <iostream>

using namespace std;

void print(int a[], int n){  
    for(int j= 0; j<n; j++){  
        cout<<a[j] <<"  ";  
    }  
    cout<<endl;  
}  
  
void swap(int *a, int *b)  
{  
    int tmp = *a;  
    *a = *b;  
    *b = tmp;  
}  
  
int partition(int a[], int low, int high)  
{  
    int privotKey = a[low];                 //��׼Ԫ��  
    while(low < high){                   //�ӱ�����˽�������м�ɨ��  
        while(low < high  && a[high] >= privotKey) --high; //��high ��ָλ����ǰ���������ൽlow+1 λ�á����Ȼ�׼Ԫ��С�Ľ������Ͷ�  
        swap(&a[low], &a[high]);  
        while(low < high  && a[low] <= privotKey ) ++low;  
        swap(&a[low], &a[high]);  
    }  
    print(a,10);  
    return low;  
}  
  
  
void qsort_improve(int r[ ],int low,int high, int k){  
    if( high -low > k ) { //���ȴ���kʱ�ݹ�, kΪָ������  
        int pivot = partition(r, low, high); // ���õ�Partition�㷨���ֲ���  
        qsort_improve(r, low, pivot - 1,k);  
        qsort_improve(r, pivot + 1, high,k);  
    }   
}   
void quickSort(int r[], int n, int k){  
    qsort_improve(r,0,n,k);//�ȵ��øĽ��㷨Qsortʹ֮��������  
  
    //���ò�������Ի���������������  
    for(int i=1; i<=n;i ++){  
        int tmp = r[i];   
        int j=i-1;  
        while(tmp < r[j]){  
            r[j+1]=r[j]; j=j-1;   
        }  
        r[j+1] = tmp;  
    }   
  
}   
  
  
  
int main(){  
    int a[10] = {3,1,5,7,2,4,9,6,10,8};  
    cout<<"��ʼֵ��";  
    print(a,10);  
    quickSort(a,9,4);  
    cout<<"�����";  
    print(a,10);  
  
}  
