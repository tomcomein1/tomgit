#include <iostream>

using namespace std;

//class ConcreteElementA;
//class ConcreteElementB;

class Element;
class Visitor {
	virtual ~Visitor() {}
	virtual void ConcreteElementA(Element* elm)=0;
	virtual void ConcreteElementB(Element* elm)=0;
protected:
	Visitor() {}
};

class ConcreteVisitorA:public Visitor {
	ConcreteVisitorA() {}
	virtual ~ConcreteVisitorA() {}
	virtual void VisitConcreteElementA(Element* elm);
	virtual void VisitConcreteElementB(Element* elm);
};

void ConcreteVisitorA::VisitConcreteElementA(Element* elm) {
	cout<<"i will visit ConcreteElementA..."<<endl;
}
void ConcreteVisitorA::VisitConcreteElementB(Element* elm) {
	cout<<"i will visit ConcreteElementB..."<<endl;
}

class ConcreteVisitorB:public Visitor {
	ConcreteVisitorB() {}
	virtual ~ConcreteVisitorB() {}
	virtual void VisitConcreteElementA(Element* elm);
	virtual void VisitConcreteElementB(Element* elm);
};

void ConcreteVisitorB::VisitConcreteElementA(Element* elm) {
	cout<<"i will visit ConcreteElementA..."<<endl;
}
void ConcreteVisitorB::VisitConcreteElementB(Element* elm) {
	cout<<"i will visit ConcreteElementB..."<<endl;
}

class Element {
public:
	virtual ~Element() {}
	virtual void Accept(Visitor* vis)=0;
protected:
	Element() {}
};

void Element::Accept(Visitor* vis) { }


class ConcreteElementA:public Element {
	ConcreteElementA() {}
	~ConcreteElementA() {}
	void Accept(Visitor* vis);
};

void ConcreteElementA::Accept(Visitor* vis) {
	vis->VistConcreteElementA(this);
	cout<<"visiting ConcreteElementA..."<<endl;
}

class ConcreteElementB:public Element {
	ConcreteElementB() {}
	~ConcreteElementB() {}
	void Accept(Visitor* vis);
};


void ConcreteElementB::Accept(Visitor* vis) {
	vis->VistConcreteElementB(this);
	cout<<"visiting ConcreteElementB..."<<endl;
}


int main(int argc,char *argv[])
{
	Visitor* vis = new ConcreteVisitorA();
	Element* elm = new ConcreteElementA();
	elm->Accept(vis);

	return 0;
}
