#include <iostream>

using namespace std;
class shape {
protected:
        double r;
public:
        shape(double x) {
                r=x;
        }
        virtual void Area()=0;
        virtual void Peri()=0;
};

class circle:public shape {
public:
        circle(double x):shape(x) {} 
        void Area() {
                cout << "The circle area is :"<<endl;
                cout << 3.14*r*r <<endl;
        }       

        void Peri() {
                cout<<"The circle perimeter is:"<<endl;
                cout<<2*3.14*r<<endl;
        }
};

class square:public shape {
public:
        square(double x):shape(x) {}
                
        void Area() {
                cout << "The square area is :"<<endl;
                cout << r*r <<endl;
        }       

        void Peri() {
                cout<< "The square perimeter is:"<<endl;
                cout<<4*r<<endl;
        }
};

//生成一个圆外切正方形派生类
class ex_square:public shape {
public:
        ex_square(double x):shape(x) {}
        void Area() {
                cout << "The ex_square area is :"<<endl;
                cout<<4*r*r<<endl;
        }
        void Peri() {
                cout << "The ex_square perimeter is :"<<endl;
                cout<<8*r<<endl;
        }
        
};

int main(int argc, char *argv[]) 
{
        shape *pr;
        circle ob1(5);
        square ob2(5);
        ex_square ob3(5);

        pr = &ob1;
        pr->Area();
        pr->Peri();

        pr = &ob2;
        pr->Area();
        pr->Peri();

        pr = &ob3;
        pr->Area();
        pr->Peri();

        return 0;
                
}
