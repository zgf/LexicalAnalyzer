#include "RegexDFA.h"
//<HeadFile>
using std::string;
enum class TokenTag
{
	//<TokenTag>
};
class Token;
//<TokenClass>
//<PreClassGlobal>
class LexParse
{
public:
	LexParse(string& tPattern) :Pattern(tPattern)
	{
	}
	void initRegexMap()
	{
		//<initRegexMap>
	}
	void initRegexList()
	{
		//<initRegexList>
	}
private:
	//<initActionFunction>
private:
	string Pattern;

	//<ClassMember>
private:
	unordered_map<int, function<shared_ptr<Token>(const int& FindIndex, const string& CatchContent, const string& SrcStr)>>RegexMap;
	vector<string> RegexList;
	//<UserDefineFunction>
};
