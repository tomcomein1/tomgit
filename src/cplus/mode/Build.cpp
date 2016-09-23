#include <iostream>
#include <string>

using namespace std;

class Product {
public:
	Product() { 
	  ProducePart();
	  cout << "return a product"<<endl;
	}
	~Product() {}
	void ProducePart();
};

void Product::ProducePart() {
	cout << "build part of product.."<<endl;
}

class ProductPart{
public:
	ProductPart() {
		cout << "~productpart.."<<endl;
	}
	~ProductPart() {}

	ProductPart* BuildPart();
};

ProductPart* ProductPart::BuildPart() {
	return new ProductPart;
}

class Builder {
public:
	virtual ~Builder() {}
	virtual void BuildPartA(const string& buildPara) = 0;	
	virtual void BuildPartB(const string& buildPara) = 0;	
	virtual void BuildPartC(const string& buildPara) = 0;	
	virtual Product* GetProduct()=0;
protected:
	Builder() {}
};

//Builder::Builder() { }
//Builder::~Builder() { }

class ConcreteBuilder:public Builder {
public:
	ConcreteBuilder() {}
	~ConcreteBuilder() {}
	void BuildPartA(const string&  buildPara);
	void BuildPartB(const string&  buildPara);
	void BuildPartC(const string&  buildPara);
	Product* GetProduct();
};

void ConcreteBuilder::BuildPartA(const string&  buildPara) {
	cout<<"Step1:Build PartA..."<<buildPara<<endl;
}

void ConcreteBuilder::BuildPartB(const string&  buildPara) {
	cout<<"Step1:Build PartB..."<<buildPara<<endl;
}

void ConcreteBuilder::BuildPartC(const string&  buildPara) {
	cout<<"Step1:Build PartC..."<<buildPara<<endl;
}


Product* ConcreteBuilder::GetProduct() {
	BuildPartA("pre-defined");
	BuildPartB("pre-defined");
	BuildPartC("pre-defined");

	return new Product();
}


class Director {
public:
	Director(Builder* bld) { _bld=bld; }
	~Director() {}
	void Construct();
protected:
private:
	Builder* _bld;
};

void Director::Construct(){
	_bld->BuildPartA("user-defined");
	_bld->BuildPartB("user-defined");
	_bld->BuildPartC("user-defined");
}

int main(int argc, char *argv[]) 
{
	Director* d = new Director(new ConcreteBuilder());
	d->Construct();

	return 0;
}

