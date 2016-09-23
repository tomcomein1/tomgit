#include <iostream>
using namespace std;

class Prototype {
public:
	virtual ~Prototype() { }
	virtual Prototype* Clone() const=0;
protected:
	Prototype() { }
};

Prototype* Prototype::Clone() const {
	return 0;
}

class ConcretePrototype:public Prototype {
public:
	ConcretePrototype() {}
	ConcretePrototype(const ConcretePrototype& cp);
	~ConcretePrototype() {}
	Prototype* Clone() const;
};


ConcretePrototype::ConcretePrototype(const ConcretePrototype& cp) {
	cout<<"ConcretePrototype copy..."<<endl;
}


Prototype* ConcretePrototype::Clone() const {
	return new ConcretePrototype(*this);
}

int main(int argc, char *argv[]) 
{
	Prototype* p = new ConcretePrototype();
cout << "..........."<<endl;
	Prototype* p1=p->Clone();

	return 0;
}
