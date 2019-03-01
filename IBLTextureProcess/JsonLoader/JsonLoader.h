#pragma once
#include <string>
#include "json/json.h"
class JsonLoader
{
public:
	bool LoadJsonFile(std::string &FileName, Json::Value &Root);

};