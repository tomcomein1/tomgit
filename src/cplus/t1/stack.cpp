#include <iostream>
#include <string>

using namespace std;

template <class T, int MAXSIZE>
class Stack {
private:
        T Items[MAXSIZE];
        int top;
public:
        Stack<T,MAXSIZE>( ) {
                cout<<"call Stack"<<endl;       
                top=0;
        }
        ~Stack<T,MAXSIZE>( ) { cout<<"end Stack"<<endl; }

        bool Push(const T &item);
        bool Pop(T &item);
};

/*
template <class T, int MAXSIZE>Stack<T,MAXSIZE>::Stack(){
        top = 0;
}
*/

template <class T, int MAXSIZE> bool Stack<T, MAXSIZE>::Push(const T &Item) {
        if (top < MAXSIZE) {
                Items[top++] = Item;
                return true;
        }
        else return false;

}

template <class T, int MAXSIZE> bool Stack<T, MAXSIZE>::Pop(T &Item) {
        if (top > 0) {
                Item = Items[--top];
                return true;
        }
        else return false;
}

int main(int argc, char *argv[])
{
        Stack<char, 10> *pStack = new Stack<char, 10>();
        pStack->Push('A');
        pStack->Push('B');
        pStack->Push('C');
        pStack->Push('D');
        pStack->Push('E');

        for(int i=0; i<10; i++) {
                char Tmp;
                if (pStack->Pop(Tmp))
                        cout<<Tmp<<endl;
        }
        delete pStack;

        Stack<int, 10> *iStack = new Stack<int, 10>();
        iStack->Push(2);
        iStack->Push(100);
        iStack->Push(200);
        iStack->Push(323432);

        for(int i=0; i<10; i++) {
                int Tmp;
                if (iStack->Pop(Tmp))
                        cout<<Tmp<<endl;
        }
        delete iStack;
        

        return 0;

}

