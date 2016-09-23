#include <iostream>

using namespace std;
// a class for simulating an integer memory cell.

class IntCell {
public:
//Construct the IntCell. Initial value is 0.
IntCell() {storedValue = 0;}
IntCell(int initialValue) { storedValue = initialValue; }
//  explicit IntCell( int initialValue = 0 ) : storedValue(intialValue) ;
int read() const { return storedValue; }
void write( int x) { storedValue = x; }

~IntCell() { cout << "~IntCell..."<<endl; }

private:
        int storedValue;
};


int main(void)
{
        IntCell ic = IntCell(0);

        ic.write(7);
        cout << ic.read() << endl;
}
