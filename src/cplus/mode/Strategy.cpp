#include <iostream>

using namespace std;

class Strategy {
public:
	Strategy() {}
	virtual ~Strategy() {
		cout<<"~Strategy......"<<endl;
	}
	virtual void AlgrithmInterface()=0;
};

void Strategy::AlgrithmInterface() {}

class ConcreteStrategyA:public Strategy {
public:
	ConcreteStrategyA() {}
	virtual ~ConcreteStrategyA() {
		cout<<"~ConcreteStrategyA......"<<endl;
	}
	void AlgrithmInterface();
};

void ConcreteStrategyA::AlgrithmInterface() {
	cout << "test ConcreteStrategyA....."<<endl;
}

class ConcreteStrategyB:public Strategy {
public:
	ConcreteStrategyB() {}
	virtual ~ConcreteStrategyB() {
		cout<<"~ConcreteStrategyB......"<<endl;
	}
	void AlgrithmInterface();
};

void ConcreteStrategyB::AlgrithmInterface() {
	cout << "test ConcreteStrategyB....."<<endl;
}

//Context: 委托模式关健类
class Strategy;
class Context {
public:
	Context(Strategy* stg) {
		_stg=stg;
	}
	~Context() {
		if (!_stg) delete _stg;
	}
	void DoAction();
private:
	Strategy* _stg;
};

void Context::DoAction() {
	_stg->AlgrithmInterface();
}

int main(int argc, char *argv[])
{
	Strategy* ps = new ConcreteStrategyA();
	Context* pc = new Context(ps);
	pc->DoAction();
	if (NULL != pc) delete pc;

	return 0;
}


