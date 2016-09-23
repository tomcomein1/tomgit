#include <iostream>

using namespace std;

//产品
class Product {
public:
	virtual ~Product()=0;
protected:
	Product();
private:	
};

Product::Product() {
	cout << "Product..."<<endl;
}

Product::~Product() {
	cout << "~Product..."<<endl;
}

//实际产品
class ConcreteProduct:public Product {
public:
	~ConcreteProduct();
	ConcreteProduct();
};

ConcreteProduct::ConcreteProduct() {
	cout << "ConcreteProduct..."<<endl;
}

ConcreteProduct::~ConcreteProduct() {
	cout << "~ConcreteProduct..."<<endl;
}

//工厂
class Product;
class Factory {
public:
	virtual ~Factory()=0;
	virtual Product* CreateProduct()=0;
protected:
	Factory();
};

Factory::Factory() {
	cout << "Factory..."<<endl;
}

Factory::~Factory() {
	cout << "~Factory..."<<endl;
}

//实际工厂
class ConcreteFactory:public Factory {
public:
	~ConcreteFactory();
	ConcreteFactory();
	Product* CreateProduct();
};

ConcreteFactory::ConcreteFactory(){
	cout<<"ConcreteFactory..."<<endl;
}

ConcreteFactory::~ConcreteFactory() {
	cout<<"~ConcreteFactory..."<<endl;
}

//工厂生成产品
Product* ConcreteFactory::CreateProduct() {
	return new ConcreteProduct();
}

int main(int argc, char *argv[])
{
	Factory *fac = new ConcreteFactory();  //工厂
	Product *p = fac->CreateProduct();     

	p = NULL;
	return 0;
}
