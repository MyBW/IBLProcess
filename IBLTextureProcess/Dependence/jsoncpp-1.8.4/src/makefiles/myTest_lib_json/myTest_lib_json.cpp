#include <iostream>
#include <fstream>
#include <cassert>
#pragma warning (push) 
#pragma warning (disable: 4996) 
#include "json/json.h"
#pragma warning (pop)
//#include "json/json.h"
using namespace std;

int main()
{
	ifstream ifs;
	ifs.open("testjson.json");
	assert(ifs.is_open());

	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(ifs, root, false))
	{
		return -1;
	}

	std::string name = root["name"].asString();
	int age = root["age"].asInt();

	//string s1("\u6f2b\u53cb11\u5e744\u6708\u4e2d");
	//s1 = "大人";

	//cout << s1 << endl;

	std::cout << name << std::endl;
	std::cout << age << std::endl;

	return 0;
}