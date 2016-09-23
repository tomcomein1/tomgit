#include <iostream>

using namespace std;

void print(int a[], int n){  
    for(int j= 0; j<n; j++){  
        cout<<a[j] <<"  ";  
    }  
    cout<<endl;  
}  
  
//��r[i��m]��r[m +1 ��n]�鲢����������rf[i��n]  
void Merge(ElemType *r,ElemType *rf, int i, int m, int n)  
{  
    int j,k;  
    for(j=m+1,k=i; i<=m && j <=n ; ++k){  
        if(r[j] < r[i]) rf[k] = r[j++];  
        else rf[k] = r[i++];  
    }  
    while(i <= m)  rf[k++] = r[i++];  
    while(j <= n)  rf[k++] = r[j++];  
    print(rf,n+1);  
}  
  
void MergeSort(ElemType *r, ElemType *rf, int lenght)  
{   
    int len = 1;  
    ElemType *q = r ;  
    ElemType *tmp ;  
    while(len < lenght) {  
        int s = len;  
        len = 2 * s ;  
        int i = 0;  
        while(i+ len <lenght){  
            Merge(q, rf,  i, i+ s-1, i+ len-1 ); //�Եȳ��������ӱ�ϲ�  
            i = i+ len;  
        }  
        if(i + s < lenght){  
            Merge(q, rf,  i, i+ s -1, lenght -1); //�Բ��ȳ��������ӱ�ϲ�  
        }  
        tmp = q; q = rf; rf = tmp; //����q,rf���Ա�֤��һ�˹鲢ʱ���Դ�q �鲢��rf  
    }  
}  
  
  
int main(){  
    int a[10] = {3,1,5,7,2,4,9,6,10,8};  
    int b[10];  
    MergeSort(a, b, 10);  
    print(b,10);  
    cout<<"�����";  
    print(a,10);  
  
}  
