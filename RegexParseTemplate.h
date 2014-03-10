#include "RegexLexParse.h"
//语法分析符号
//基类
using std::unordered_multimap;
class Symbol
{
public:
	bool IsTerminal;
public:
	Symbol(bool Chose) : IsTerminal(Chose)
	{
	}
};
//非终结符号
class Nonterminal :public Symbol
{
public:
	string Name;
	//非终结符号对应的回调函数
public:
	Nonterminal(bool Chose, string NameStr) : Symbol(Chose), Name(NameStr)
	{
	}
};
//终结符号
class Termination : public Symbol
{
public:
	LexTag TermTag;
public:
	Termination(bool Chose, LexTag Tag) : Symbol(Chose), TermTag(Tag)
	{
	}
};

template<typename X, typename Y, typename Z>
class Triple
{
public:
	//第几条文法
	X First;
	//第X条文法的第Y项
	Y Second;
	//向前看的字符
	Z Third;
};
//状态集
class Stauts
{
	/*int Item;
	int Position;*/
	//z状态集编号
	int Index;
	vector<Triple<int, int, Termination>> ItemList;
	map<Symbol, int> NextStauts;
};
//产生式
class Production
{
public:
	Symbol Head;
	vector<Symbol> Body;
	int BodySize;
	Production(Symbol& tHead, vector<Symbol>& tBody) : Head(tHead), Body(tBody)
	{
		BodySize = Body.size();
	}
};

class RegexParse
{
private:
	//文法映射表 0号是产生式头
	vector<Production> GrammarMap;

private:
	void initGrammarMap()
	{
		//GrammarMap = {vector<Symbol>({}), vector<Symbol>({})};
		//</initGrammarMap>
	}
};
