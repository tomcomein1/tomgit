#include <iostream>
#include <vector>

using namespace std;

int main()
{
	int a[6] = {0, 1, 2, 3, 4, 5};
	vector<int> vec(a, a+6);
	
	vec.push_back(6000);
	vec.insert(vec.begin(), 80000);
	
	vector<int>::iterator itr;
	itr = vec.begin();
	itr++;
	vec.erase(itr);
	
	vector<int>::size_type i;
	
	for (i=0; i<vec.size(); i++)
	{
		cout << vec[i] <<endl;
	}

	return 0;
}

