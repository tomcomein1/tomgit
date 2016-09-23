#include <iostream>
#include <list>
#include <string>

using namespace std;

typedef string State;

class Subject;

class Observer {
public:
	virtual ~Observer();
	virtual void Update(Subject* sub) = 0;
	virtual void PrintInfo() = 0;
protected:
	Observer();
	State _st;
};

Observer::Observer() {
	_st='\0';
}
Observer::~Observer() { }

class ConcreteObserverA:public Observer{
public:
	virtual Subject* GetSubject();
	ConcreteObserverA(Subject* sub);
	virtual ~ConcreteObserverA();
	void Update(Subject* sub);
	void PrintInfo();
private:
	Subject* _sub;
};

ConcreteObserverA::ConcreteObserverA(Subject* sub){
	_sub = sub;
	_sub->Attach(this);	
}
ConcreteObserverA::~ConcreteObserverA{
	_sub->Detach(this);
	if (_sub != 0) {
		delete _sub;
	}
}

Subject* ConcreteObserverA::GetSubject() {
	return _sub;
}

void ConcreteObserverA::PrintInfo() {
	cout << "ConcreteObserverA observer..." <<_sub->GetState()<<endl;
}

void ConcreteObserverA::Update(Subject* sub) {
	_st=sub->GetState();
	PrintInfo();
}

class ConcreteObserverB:public Observer{
public:
	virtual Subject* GetSubject();
	ConcreteObserverB(Subject* sub);
	virtual ~ConcreteObserverB();
	void Update(Subject* sub);
	void PrintInfo();
private:
	Subject* _sub;
};

ConcreteObserverB::ConcreteObserverB(Subject* sub){
        _sub = sub;
        _sub->Attach(this);
}
ConcreteObserverB::~ConcreteObserverA{
        _sub->Detach(this);
        if (_sub != 0) {
                delete _sub;
        }
}

Subject* ConcreteObserverB::GetSubject() {
        return _sub;
}

void ConcreteObserverB::PrintInfo() {
        cout << "ConcreteObserverB observer..." <<_sub->GetState()<<endl;
}

void ConcreteObserverB::Update(Subject* sub) {
        _st=sub->GetState();
        PrintInfo();
}

class Subject {
public:
	virtual ~Subject();
	virtual void Attach(Observer* obv);
	virtual void Detach(Observer* obv);
	virtual void Notify();
	virtual void SetState(const State& st)=0;
	virtual State GetState()=0;
protected:
	Subject();
private:
	list<Observer*>* _obvs;
};

Subject::Subject() {
	_obvs = new list<Observer*>;
}

Subject::~Subject() {
}

void Subject::Attach(Observer* obv) {
	_obvs->push_front(obv);
}

void Subject::Detach(Observer* obv) {
	_obvs->remove(obv);
}

void Subject::Notify() {
	list<Observer*>::iterator it;
	it = _obvs->begin();
	for( ; it!=_obvs->end(); it++) {
		(*it)->Update(this);
	}
}

class ConcreteSubject:pubilc Subject {
public:
	ConcreteSubject();
	~ConcreteSubject();
	State GetState();
	void SetState(const State& st);
private:
	State _st;
};

oncreteSubject::ConcreteSubject() {
	_st='\0';
}

oncreteSubject::~ConcreteSubject() {
}


State ConcreteSubject::GetState() {
	return _st;
}

void ConcreteSubject::SetState(const State& st) {
	_st = st;
}

int main(int argc, char *argv[])
{
	ConcreteSubject* sub=new ConcreteSubject();
	Observer* o1 = new ConcreteSubjectA(sub);
	Observer* o2 = new ConcreteSubjectB(sub);

	sub->SetState("old");
	sub->Notify();
	sub->SetState("new");
	sub->Notify();

	return 0;
}

