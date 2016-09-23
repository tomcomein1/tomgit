#include <stdio.h>
class A{
public:
        virtual void vir_func1() {
                printf("v1 is called in A.\n");
                a1();
        }       

        virtual void vir_func2() {
                printf("v2 is called in A.\n");
        }
        void a1() {
                printf("a1 is called in A.\n");
                vir_func2();
        }
};

class B:public A {
        virtual void vir_func2() {
                printf("v2 is called in B.\n");
        }
};

int main(int argc, char *argv[]) 
{
        A a;
        a.vir_func1();
        printf(" OK.\n");
        B b;
        b.vir_func1();
        return 0;
}
