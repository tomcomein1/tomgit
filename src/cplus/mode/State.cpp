#include <iostream>
#include <stdbool.h>

using namespace std;

//Stateģʽ
class Context;
class State {
public:
	State() {}
	virtual ~State() {}
	virtual void OperationInterface(Context*)=0;
	virtual void OperationChangeState(Context*)=0;
protected:
	bool ChangeState(Context* con, State* st);
};

void State::OperationInterface(Context* con) {
	cout<<"State::..."<<endl;
}

void State::ChangeState(Context* con, State* st) {
	con->ChangeState(st);
	return true;
}

void State::OperationChangeState(Context* con) {}

class ConcreteStateA:public State{
public:
	ConcreteStateA() {}
	virtual ~ConcreteStateA() {}
	virtual void OperationInterface(Context*);
	virtual void OperationChangeState(Context*);
};

void ConcreteStateA::OperationInterface(Context* con) {
	cout<<"ConcreteStateA::OperationInterface"<<endl;
}
void ConcreteStateA::OperationChangeState(Context* con) {
	OperationInterface(con);
	this->ChangeState(con, new ConcreteStateB);
}

class ConcreteStateB:public State{
public:
	ConcreteStateB() {}
	virtual ~ConcreteStateB() {}
	virtual void OperationInterface(Context*);
	virtual void OperationChangeState(Context*);
};
void ConcreteStateB::OperationInterface(Context* con) {
	cout<<"ConcreteStateB::OperationInterface"<<endl;
}
void ConcreteStateB::OperationChangeState(Context* con) {
	OperationInterface(con);
	this->ChangeState(con, new ConcreteStateA);
}

