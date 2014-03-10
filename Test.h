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
	Triple(X a, Y b) : First(a), Second(b)
	{

	}
	Triple(X a, Y b, Z c) : Triple(a, b), Third(c)
	{

	}
	Triple(Z c) :Third(c)
	{

	}
};
//状态集
class Stauts
{
public:
	//z状态集编号
	int Index;
	vector<Triple<int, int, Termination>> CoreItemList;
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
	void initGrammarMap()
	{
		//GrammarMap = {vector<Symbol>({}), vector<Symbol>({})};
		GrammarMap =
		{
			Production(Nonterminal(false, "CharSet"), vector<Symbol>({Termination(true, LexTag::CharSet_Start), Nonterminal(false, "CharSetString"), Termination(true, LexTag::CharSet_End)})),
			Production(Nonterminal(false, "CharSet"), vector<Symbol>({Termination(true, LexTag::CharSet_Back_Start), Nonterminal(false, "CharSetString"), Termination(true, LexTag::CharSet_End)})),
			Production(Nonterminal(false, "CharSetString "), vector<Symbol>({Nonterminal(false, "CharSetString"), Termination(true, LexTag::CharSetComponent), Nonterminal(false, "NormalString")})),
			Production(Nonterminal(false, "CharSetString "), vector<Symbol>({Nonterminal(false, "CharSetString"), Nonterminal(false, "NormalString")})),
			Production(Nonterminal(false, "CharSetString "), vector<Symbol>({Nonterminal(false, "NormalString")})),
			Production(Nonterminal(false, "CompleteCharSet "), vector<Symbol>({Nonterminal(false, "CharSet")})),
			Production(Nonterminal(false, "CompleteCharSet "), vector<Symbol>({Nonterminal(false, "CharSet"), Nonterminal(false, "Repeat")})),
			Production(Nonterminal(false, "CompleteFactor "), vector<Symbol>({Nonterminal(false, "Factor"), Nonterminal(false, "Repeat")})),
			Production(Nonterminal(false, "CompleteFactor "), vector<Symbol>({Nonterminal(false, "Factor")})),
			Production(Nonterminal(false, "Express "), vector<Symbol>({Nonterminal(false, "Express"), Termination(true, LexTag::ChoseSymbol), Nonterminal(false, "Term")})),
			Production(Nonterminal(false, "Express "), vector<Symbol>({Nonterminal(false, "Term")})),
			Production(Nonterminal(false, "Factor "), vector<Symbol>({Termination(true, LexTag::SimpleUnNamedCatch_Start), Nonterminal(false, "Express"), Termination(true, LexTag::Mitipute_End)})),
			Production(Nonterminal(false, "Factor "), vector<Symbol>({Nonterminal(false, "CompleteCharSet")})),
			Production(Nonterminal(false, "Factor "), vector<Symbol>({Nonterminal(false, "NormalStringComplete")})),
			Production(Nonterminal(false, "NormalChar "), vector<Symbol>({Termination(true, LexTag::NumberChar)})),
			Production(Nonterminal(false, "NormalChar "), vector<Symbol>({Termination(true, LexTag::RealWordChar)})),
			Production(Nonterminal(false, "NormalChar "), vector<Symbol>({Termination(true, LexTag::OtherChar)})),
			Production(Nonterminal(false, "NormalString "), vector<Symbol>({Nonterminal(false, "NormalString"), Nonterminal(false, "NormalChar")})),
			Production(Nonterminal(false, "NormalString "), vector<Symbol>({Nonterminal(false, "NormalChar")})),
			Production(Nonterminal(false, "NormalStringComplete "), vector<Symbol>({Nonterminal(false, "NormalString"), Nonterminal(false, "Repeat")})),
			Production(Nonterminal(false, "NormalStringComplete "), vector<Symbol>({Nonterminal(false, "NormalString")})),
			Production(Nonterminal(false, "Repeat "), vector<Symbol>({Termination(true, LexTag::Repeat_Start), Nonterminal(false, "RepeatRight"), Nonterminal(false, "RepeatEnd")})),
			Production(Nonterminal(false, "Repeat "), vector<Symbol>({Termination(true, LexTag::Closures_UnGreedy)})),
			Production(Nonterminal(false, "Repeat "), vector<Symbol>({Termination(true, LexTag::Closures_Greedy)})),
			Production(Nonterminal(false, "Repeat "), vector<Symbol>({Termination(true, LexTag::PositiveClosures_Greedy)})),
			Production(Nonterminal(false, "Repeat "), vector<Symbol>({Termination(true, LexTag::PositiveClosures_UnGreedy)})),
			Production(Nonterminal(false, "Repeat "), vector<Symbol>({Termination(true, LexTag::ChoseClosures_Greedy)})),
			Production(Nonterminal(false, "Repeat "), vector<Symbol>({Termination(true, LexTag::ChoseClosures_UnGreedy)})),
			Production(Nonterminal(false, "RepeatEnd "), vector<Symbol>({Termination(true, LexTag::Repeat_And_BackRefer_End)})),
			Production(Nonterminal(false, "RepeatEnd "), vector<Symbol>({Termination(true, LexTag::Repeat_End_Greedy)})),
			Production(Nonterminal(false, "RepeatRight "), vector<Symbol>({Termination(true, LexTag::Comma), Nonterminal(false, "SumNumber")})),
			Production(Nonterminal(false, "RepeatRight "), vector<Symbol>({Nonterminal(false, "SumNumber"), Termination(true, LexTag::Comma)})),
			Production(Nonterminal(false, "RepeatRight "), vector<Symbol>({Nonterminal(false, "SumNumber")})),
			Production(Nonterminal(false, "RepeatRight "), vector<Symbol>({Nonterminal(false, "SumNumber"), Termination(true, LexTag::Comma), Nonterminal(false, "SumNumber")})),
			Production(Nonterminal(false, "Start "), vector<Symbol>({Nonterminal(false, "Factor")})),
			Production(Nonterminal(false, "SumNumber "), vector<Symbol>({Nonterminal(false, "SumNumber"), Nonterminal(false, "Number")})),
			Production(Nonterminal(false, "Term "), vector<Symbol>({Nonterminal(false, "Term"), Nonterminal(false, "CompleteFactor")})),
			Production(Nonterminal(false, "Term "), vector<Symbol>({Nonterminal(false, "CompleteFactor")})),
		};
	}
	//创建LR(0)核心项集
	void CreatLR0ItemSet()
	{
		Stauts StartSet;
		StartSet.Index = 0;
		//开始产生式
		StartSet.CoreItemList.push_back(Triple<int, int, Termination>({0, 0}));


	}
	//计算项集的闭包并加入项集
	void CreatItemClourse(vector<Triple<int,int,Termination>>& ItemList)
	{
		
	}
private:
	//文法映射表 0号是产生式头
	vector<Production> GrammarMap;

	//LR(0)项集
	vector<Stauts> LRItemSet;


};
