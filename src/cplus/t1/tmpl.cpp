#include <iostream>
using namespace std;

#define GET_ARRAY_LEN(array,len){len = (sizeof(array) / sizeof(array[0]));}

template<typename T>
void Print(T* pArray, int nCount) {
        for(int i=0; i<nCount; i++) {
                cout<<pArray[i]<<" ";
        }
        cout <<endl;
}

template<class T>
T Sum(T* pArray, int nMaxSize) {
        T tSum = 0;
        for (int i=0; i<nMaxSize; i++) {
                tSum += pArray[i];
        }
        return (T)tSum;
}

int main(int argc, char* argv[]) {
        char pArrayl[] = "ABCDEFG";

        int nLen=strlen(pArrayl);
        Print<char>(pArrayl, nLen);

        double pArray2[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
        GET_ARRAY_LEN(pArray2,nLen);
        double fSum = Sum<double>(pArray2, nLen);
        cout<<"fSUM:"<< fSum << endl;

        int pArray3[] = {2, 3, 3, 4, 5, 6};
        GET_ARRAY_LEN(pArray3,nLen);
        int iSum = Sum<int>(pArray3, nLen);
        cout<<"iSUM:"<< iSum << endl;
        return 0;       
}

