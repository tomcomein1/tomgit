#include <iostream>
using namespace std;

enum Day{
        Saturday,
        Sunday = 0,
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday
};

void Prnt(Day day) {
        if (day==0) cout << "Weekend" <<endl;
        else cout << "Weekday" <<endl;
}

int main(int argc, char *argv[])
{
        enum Fruit {apple, pear, orange, banana};
        Fruit frt1 = apple;
        Fruit frt2 = pear;
        frt1 = (Fruit) 0;

        for (int i=apple; i<= banana; i++) {
                switch(i) {
                        case apple:
                        cout << "apple"<<endl;
                        break;
                        case pear:
                        cout << "pear"<<endl;
                        break;
                        default:break;
                }
        }
        Prnt(Saturday);
        Prnt(Sunday );
        Prnt(Monday);
        Prnt(Tuesday);
}
