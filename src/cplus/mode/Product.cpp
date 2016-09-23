#include <iostream>

using namespace std;

//��Ʒ
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

//ʵ�ʲ�Ʒ
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

//����
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

//ʵ�ʹ���
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

//�������ɲ�Ʒ
Product* ConcreteFactory::CreateProduct() {
	return new ConcreteProduct();
}

int main(int argc, char *argv[])
{
	Factory *fac = new ConcreteFactory();  //����
	Product *p = fac->CreateProduct();     

	p = NULL;
	return 0;
}
