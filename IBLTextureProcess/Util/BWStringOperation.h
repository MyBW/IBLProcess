#ifndef STRINGOPRATION_H_
#define STRINGOPRATION_H_
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
typedef std::basic_stringstream<char, std::char_traits<char>, std::allocator<char>> StringStreamBase;
typedef StringStreamBase StringStream;
typedef std::vector<std::string> StringVector;
class StringUtil
{
public:
	static void ToLowerCase( std::string &src);
	static void ToUpperCase( std::string &src);
	static bool Match(const std::string &text1, const std::string &text2, bool tmp);
	static std::vector<std::string> Split(const std::string &str, const std::string &splitor = " ", int maxSplitNum = 0);
	static bool EndsWith(const std::string& str, const std::string& pattern, bool lowerCase = true);
	static void DeleteChar(std::string& str, char ch);
	static std::string BLANK;
protected:
private:
};
#endif