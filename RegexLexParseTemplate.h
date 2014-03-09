#include "RegexLexParse.h"
//语法分析符号
//基类
using std::unordered_multimap;
class Symbol
{
public:
	bool IsTerminal;
	//便于调试
	string RawName;
};
//非终结符号
class Nonterminal
{
public:
	string Name;
	//非终结符号对应的回调函数
	function<void()> CallBack;
};
//终结符号
class Termination
{
public:
	LexTag TermTag;
	//词素
	string Morpheme;
};

//状态集
class Stauts
{
	int Item;
	int Position;

};

class RegexParse
{
private:
	//文法映射表 0号是产生式头
	vector<vector<Symbol>> GrammarMap;


};
