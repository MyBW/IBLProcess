#include "JsonLoader.h"
#include "json/json.h"
#include <fstream>
bool JsonLoader::LoadJsonFile(std::string &FileName , Json::Value &Root)
{
	Json::Reader reader;// Ω‚Œˆjson”√Json::Reader   
	std::ifstream is;
	is.open(FileName, std::ios::binary);
	bool IsOpenSucc = reader.parse(is, Root, false);
	is.close();
	return IsOpenSucc;
}

