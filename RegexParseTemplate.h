#pragma once
#include "RegexLexParse.h"
using std::unordered_multimap;
// 文法符号enum类
enum class ParseTag
{
	//<ParseTag>
	StringTail,
	SimpleUnNamedCatch_Start,
	Mitipute_End,
	Closures_Greedy,
	Closures_UnGreedy,
	PositiveClosures_Greedy,
	PositiveClosures_UnGreedy,
	Comma,
	CharSetComponent,
	ChoseClosures_Greedy,
	ChoseClosures_UnGreedy,
	NumberChar,
	OtherChar,
	RealWordChar,
	CharSet_Start,
	CharSet_Back_Start,
	CharSet_End,
	StringHead,
	Repeat_Start,
	ChoseSymbol,
	Repeat_And_BackRefer_End,
	Repeat_End_Greedy,
	Start,
	CharSet,
	CharSetString,
	CompleteCharSet,
	CompleteFactor,
	Express,
	Factor,
	NormalChar,
	NormalString,
	NormalStringComplete,
	Repeat,
	RepeatEnd,
	RepeatRight,
	SumNumber,
	Term,
};

//语法分析符号
//基类
class Symbol
{
public:
	bool IsTerminal;
	ParseTag Tag;
public:
	Symbol(bool Chose, ParseTag tTag) : IsTerminal(Chose), Tag(tTag)
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
	Triple(X a, Y b, Z c) : First(a), Second(b), Third(c)
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
	vector<Triple<int, int, shared_ptr<Symbol>>> ItemList;
	map<shared_ptr<Symbol>, int> NextStauts;
};
//产生式
class Production
{
public:
	shared_ptr<Symbol> Head;
	vector<shared_ptr<Symbol>> Body;
	int BodySize;
	Production(shared_ptr<Symbol>& tHead, vector<shared_ptr<Symbol>>& tBody) : Head(tHead), Body(tBody)
	{
		BodySize = Body.size();
	}
};

class RegexParse
{
public:
	RegexParse()
	{
		initGrammarList();
		initGrammarMap();
		CreatLR0ItemSet();
	}
private:
	void initGrammarList()
	{
		//<initGrammarMap>
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Start)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::Factor))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSet)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::CharSet_Start)), shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSetString)), shared_ptr<Symbol>(new Symbol(true, ParseTag::CharSet_End))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSet)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::CharSet_Back_Start)), shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSetString)), shared_ptr<Symbol>(new Symbol(true, ParseTag::CharSet_End))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSetString)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSetString)), shared_ptr<Symbol>(new Symbol(true, ParseTag::CharSetComponent)), shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalString))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSetString)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSetString)), shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalString))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSetString)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalString))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::CompleteCharSet)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSet))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::CompleteCharSet)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::CharSet)), shared_ptr<Symbol>(new Symbol(false, ParseTag::Repeat))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::CompleteFactor)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::Factor)), shared_ptr<Symbol>(new Symbol(false, ParseTag::Repeat))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::CompleteFactor)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::Factor))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Express)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::Express)), shared_ptr<Symbol>(new Symbol(true, ParseTag::ChoseSymbol)), shared_ptr<Symbol>(new Symbol(false, ParseTag::Term))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Express)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::Term))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Factor)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::SimpleUnNamedCatch_Start)), shared_ptr<Symbol>(new Symbol(false, ParseTag::Express)), shared_ptr<Symbol>(new Symbol(true, ParseTag::Mitipute_End))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Factor)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::CompleteCharSet))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Factor)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalStringComplete))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalChar)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::NumberChar))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalChar)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::RealWordChar))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalChar)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::OtherChar))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalString)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalString)), shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalChar))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalString)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalChar))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalStringComplete)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalString)), shared_ptr<Symbol>(new Symbol(false, ParseTag::Repeat))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalStringComplete)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::NormalString))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Repeat)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::Repeat_Start)), shared_ptr<Symbol>(new Symbol(false, ParseTag::RepeatRight)), shared_ptr<Symbol>(new Symbol(false, ParseTag::RepeatEnd))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Repeat)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::Closures_UnGreedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Repeat)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::Closures_Greedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Repeat)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::PositiveClosures_Greedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Repeat)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::PositiveClosures_UnGreedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Repeat)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::ChoseClosures_Greedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Repeat)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::ChoseClosures_UnGreedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::RepeatEnd)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::Repeat_And_BackRefer_End))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::RepeatEnd)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::Repeat_End_Greedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::RepeatRight)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, ParseTag::Comma)), shared_ptr<Symbol>(new Symbol(false, ParseTag::SumNumber))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::RepeatRight)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::SumNumber)), shared_ptr<Symbol>(new Symbol(true, ParseTag::Comma))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::RepeatRight)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::SumNumber))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::RepeatRight)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::SumNumber)), shared_ptr<Symbol>(new Symbol(true, ParseTag::Comma)), shared_ptr<Symbol>(new Symbol(false, ParseTag::SumNumber))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::SumNumber)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::SumNumber)), shared_ptr<Symbol>(new Symbol(false, ParseTag::NumberChar))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Term)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::Term)), shared_ptr<Symbol>(new Symbol(false, ParseTag::CompleteFactor))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, ParseTag::Term)), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, ParseTag::CompleteFactor))})));
	}

	void initGrammarMap()
	{
		for(int i = 0; i < GrammarList.size(); i++)
		{
			GrammarMap.insert(make_pair(GrammarList[i].Head->Tag, i));
		}
	}
	//创建LR(0)核心项集
	void CreatLR0ItemSet()
	{
		//构造第一个Lr(0)核心项集
		Stauts StartSet;
		StartSet.Index = 0;
		//开始产生式
		//第一个项压入项集
		StartSet.ItemList.push_back(Triple<int, int, shared_ptr<Symbol>>({0, 0}));
		//第一个项集的闭包计算
		CreatItemClourse(StartSet.ItemList);

		//第一个项集压栈
		LRItemSet.push_back(StartSet);
	}

	//计算项集的闭包并加入项集
	void CreatItemClourse(vector<Triple<int, int, shared_ptr<Symbol>>>& ItemList)
	{
		for(int i = 0; i < ItemList.size(); i++)
		{
			auto& CurrentItem = ItemList[i];
			if(CouldExpand(CurrentItem.First, CurrentItem.Second))
			{
				//说明是非终结符号
				auto FindIter = GrammarMap.equal_range(GetSymbolTag(CurrentItem.First, CurrentItem.Second));
				for(auto Iter = FindIter.first; Iter != FindIter.second; Iter++)
				{
					//获取的是产生式的编号
					//查看该产生式是否已经加入;
					if(!HasAddThisProduct(ItemList, Iter->second))
					{
						ItemList.push_back(Triple<int, int, shared_ptr<Symbol>>(Iter->second, 0));
					}
				}
			}
		}
	}

	//查看该编号产生式是否已经加入
	bool HasAddThisProduct(vector<Triple<int, int, shared_ptr<Symbol>>>& ItemList, int Number)
	{
		for(auto i = 0; i < ItemList.size(); i++)
		{
			if(ItemList[i].First == Number)
			{
				return true;
			}
		}
		return false;
	}

	//判断是不是.号后面是非终结符号
	//Index 是文法编号 Position是第X个位置的点
	bool CouldExpand(int Index, int Position)
	{
		return GrammarList[Index].Body[Position]->IsTerminal == false;
	}
	ParseTag GetSymbolTag(int Index, int Position)
	{
		return GrammarList[Index].Body[Position]->Tag;
	}

	Stauts LR0GOTO(Stauts& Target)
	{
		//今日做

	}
private:
	//文法 0号是产生式头
	vector<Production> GrammarList;

	//LR(0)项集
	vector<Stauts> LRItemSet;

	//文法头到产生式体的映射表
	unordered_multimap<ParseTag, int> GrammarMap;
};