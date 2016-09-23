#include <iostream>
#include <string>
using namespace std;

typedef struct PersonNode {
	char szName[256];// 姓名  
	struct PersonNode *next;
} PersonNode;

PersonNode *g_pPersonList[100];

int PersonHash(const char *szName)
{
	unsigned int hashvalue = 0;

	char temp[256] = {};
	int i = 0;
	strcpy_s(temp, 256, szName);

	while (temp[i] != 0)
	{
		hashvalue = temp[i] + (100 - 1) * hashvalue;
		hashvalue = hashvalue % 100;
		i++;
	}
	return hashvalue;
}

int AddPerson(const char *szName)
{
	PersonNode *temp = NULL;
	temp = new PersonNode;
	temp->next = NULL;
	if (temp == NULL)
	{
		return -1;
	}
	strcpy_s(temp->szName, 256, szName);

	unsigned int hashvalue = PersonHash(szName);
	if (g_pPersonList[hashvalue] == NULL)
	{
		g_pPersonList[hashvalue] = temp;
	}
	else
	{
		PersonNode *head = NULL;
		head = g_pPersonList[hashvalue];

		while (head->next != NULL)
		{
			head = head->next;
		}
		head->next = temp;
	}
	return 0;
}

void TravelPersons()
{
	PersonNode *head = NULL;
	int j = 0;
	char name[100];
	
	for (int i = 0; i < 100; i++)
	{
		if (g_pPersonList[i] != NULL)
		{
			head = g_pPersonList[i];
			j = 0;
			while (head != NULL)
			{
				j++;
				memset(name, 0, sizeof(name));
				strcpy_s(name, sizeof(name), head->szName);
				head = head->next;
			}
			printf("%s %d\n", name, j);
		}
	}
}

int main()
{
	for (int i = 0; i < 100; i++)
	{
		g_pPersonList[i] = NULL;
	}

	char name[256] = {};
	string q;

	for (int k = 0; k < 10; k++)
	{
		cout << "请输入姓名: ";
		cin >> name;

		if (AddPerson(name) < 0)
		{
			cout << "插入哈希表错\n";
			return -1;
		}
	}

	TravelPersons();

	system("pause");
	return 0;
}

