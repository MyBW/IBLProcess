#include "BWStringOperation.h"
std::string StringUtil::BLANK = " ";
void StringUtil::ToLowerCase( std::string &src)
{
	std::transform(src.begin(), src.end(), src.begin(), tolower);
}
void StringUtil::ToUpperCase( std::string &src)
{
	std::transform(src.begin(), src.end(), src.begin(), toupper);
}

std::vector<std::string> StringUtil::Split(const std::string &str, const std::string &splitor /* = " " */ , int maxSplitNum)
{
	std::vector<std::string> final;
	if (str.empty())
	{
		return  final;
	}
	if (maxSplitNum == 0)
	{
		maxSplitNum = -1;
	}
	std::size_t pos1 = 0;
	std::size_t pos2;
	int num = 0;
	while (1)
	{
	
		pos1 = str.find_first_not_of(splitor, pos1);
		if (pos1 == std::string::npos)
		{
			break;
		}
		if (num == maxSplitNum)
		{
			std::string subString = str.substr(pos1, str.length() - pos1);
			final.push_back(subString);
			break;
		}
		pos2 = str.find_first_of(splitor, pos1);
		std::string substr = str.substr(pos1, pos2 - pos1);
		pos1 = pos2;
		final.push_back(substr);
		num++;
	}
	return final;
}
void StringUtil::DeleteChar(std::string& str, char ch)
{
	std::string final;
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == ch)
		{
			continue;
		}
		final += str[i];
	}
	str = final;
}
bool StringUtil::EndsWith(const std::string& str, const std::string& pattern, bool lowerCase /* = true */)
{
	size_t thisLen = str.length();
	size_t patternLen = pattern.length();
	if (thisLen < patternLen || patternLen == 0)
		return false;

	std::string endOfThis = str.substr(thisLen - patternLen, patternLen);
	if (lowerCase)
		StringUtil::ToLowerCase(endOfThis);

	return (endOfThis == pattern);
}