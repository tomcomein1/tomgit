#include <iostream>

using namespace std;

class Singleton {
public:
	static Singleton* getInstance();
protected:
	Singleton() {
		cout<<"Singleton..."<<endl;
	}
	~Singleton() {
		cout<<"~Singleton..."<<endl;
	}
private:
	static Singleton* _instance;
};

Singleton* Singleton::_instance = 0;

Singleton* Singleton::getInstance() {
	if (_instance == 0) {
		cout << "Create Singleton" << endl;
		_instance = new Singleton();
	}
	return _instance;
}

int main(int argc, char *argv[])
{
	Singleton *sgn = Singleton::getInstance();
	Singleton *sgn1 = Singleton::getInstance();

	if (sgn == sgn1) cout <<"OK"<<endl;
	return 0;
}

