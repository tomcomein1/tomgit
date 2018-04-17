#include <iostream>
#include <list>

using namespace std;

int main()
{
	list <int> lst;
	
	//int a[5] = {1,2,3,4,5};
	
	for (int i=0; i<5; i++)
	{
		lst.push_back(i);
	}
	
	lst.insert(lst.begin(), 1000);
	lst.push_back(60000);
	list <int>::iterator itr;
	
	for (itr=lst.begin();  itr != lst.end(); itr++)
	{
		cout << *itr << endl;
	}
	
	return 0;
}
