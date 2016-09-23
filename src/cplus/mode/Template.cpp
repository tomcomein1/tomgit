#include <iostream>
using namespace std;

class AbstractClass {
public:
	virtual ~AbstractClass() {} 
	void TemplateMethod(); 
protected:
	virtual void PrimitiveOperation1() = 0;
	virtual void PrimitiveOperation2() = 0;
	AbstractClass() {}
		
};
void AbstractClass::TemplateMethod() {
	this->PrimitiveOperation1();
	this->PrimitiveOperation2();
}

class ConcreteClass1:public AbstractClass {
public:
	ConcreteClass1() {}
	~ConcreteClass1(){}
protected:
	void PrimitiveOperation1();
	void PrimitiveOperation2();
};

void ConcreteClass1::PrimitiveOperation1() {
	cout<<"ConcreteClass1...PrimitiveOperation1"<<endl;
}
void ConcreteClass1::PrimitiveOperation2() {
	cout<<"ConcreteClass1...PrimitiveOperation2"<<endl;
}

class ConcreteClass2:public AbstractClass {
public:
	ConcreteClass2() {}
	~ConcreteClass2(){}
protected:
	void PrimitiveOperation1();
	void PrimitiveOperation2();
};

void ConcreteClass2::PrimitiveOperation1() {
	cout<<"ConcreteClass1...PrimitiveOperation1"<<endl;
}
void ConcreteClass2::PrimitiveOperation2() {
	cout<<"ConcreteClass1...PrimitiveOperation2"<<endl;
}

//Template:

int main(int argc, char *argv[]) 
{
	AbstractClass *p1 = new ConcreteClass1();
	AbstractClass *p2 = new ConcreteClass2();
	
	p1->TemplateMethod();
	p2->TemplateMethod();

	return 0;

}

