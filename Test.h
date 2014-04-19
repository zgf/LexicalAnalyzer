#include "RegexDFA.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
using std::shared_ptr;
using std::string;
using std::function;
using std::unordered_map;
//<HeadFile>
using std::string;
enum class TokenTag
{
	Id,
	Number,
	Include,
	HeadFile,
	Add,
	NullSpace,
	//<TokenTag>
};
class Token;

class Token
{
public:
	TokenTag Tag;
	string Val;
	int FindIndex;
public:
	Token(const TokenTag& tTag, const string& tVal, const int tFindIndex) :Tag(tTag), Val(tVal), FindIndex(tFindIndex)
	{
	}
};

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
		RegexMap.insert(make_pair(0, bind(&LexParse::Semantic0, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		RegexMap.insert(make_pair(1, bind(&LexParse::Semantic1, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		RegexMap.insert(make_pair(2, bind(&LexParse::Semantic2, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		RegexMap.insert(make_pair(3, bind(&LexParse::Semantic3, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		RegexMap.insert(make_pair(5, bind(&LexParse::Semantic5, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		RegexMap.insert(make_pair(6, bind(&LexParse::Semantic6, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		//<initRegexMap>
	}
	void initRegexList()
	{
		RegexList.push_back("[0-9]+");
		RegexList.push_back("\"{id}.h\"");
		RegexList.push_back("<[A-Za-z]([A-Za-z]|[0-9])*.h>");
		RegexList.push_back("\+");
		RegexList.push_back("[\t\n]+");
		RegexList.push_back("[A-Za-z]([A-Za-z]|[0-9])*");
		RegexList.push_back("#include");
		//<initRegexList>
	}
private:
	shared_ptr<Token>Semantic0(const int& FindIndex, const string& CatchContent, const string& SrcStr)
	{
		return shared_ptr<Token>(new Token(TokenTag::Number, CatchContent, FindIndex));
	}
	shared_ptr<Token>Semantic1(const int& FindIndex, const string& CatchContent, const string& SrcStr)
	{
		return shared_ptr<Token>(new Token(TokenTag::HeadFile, CatchContent, FindIndex));
	}
	shared_ptr<Token>Semantic2(const int& FindIndex, const string& CatchContent, const string& SrcStr)
	{
		return shared_ptr<Token>(new Token(TokenTag::HeadFile, CatchContent, FindIndex));
	}
	shared_ptr<Token>Semantic3(const int& FindIndex, const string& CatchContent, const string& SrcStr)
	{
		return shared_ptr<Token>(new Token(TokenTag::Add, CatchContent, FindIndex));
	}
	shared_ptr<Token>Semantic5(const int& FindIndex, const string& CatchContent, const string& SrcStr)
	{
		return shared_ptr<Token>(new Token(TokenTag::Id, CatchContent, FindIndex));
	}
	shared_ptr<Token>Semantic6(const int& FindIndex, const string& CatchContent, const string& SrcStr)
	{
		return shared_ptr<Token>(new Token(TokenTag::Include, CatchContent, FindIndex));
	}
	//<initActionFunction>
	void CreatDfa()
	{

	}
private:
	string Pattern;

	//<ClassMember>
private:
	unordered_map<int, function<shared_ptr<Token>(const int& FindIndex, const string& CatchContent, const string& SrcStr)>>RegexMap;
	vector<string> RegexList;
	//<UserDefineFunction>
};
