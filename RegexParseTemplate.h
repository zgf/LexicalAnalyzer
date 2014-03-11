#pragma once
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
	Termination(bool Chose) :Symbol(Chose)
	{
	}
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
	map<Symbol, int> NextStauts;
};
//产生式
class Production
{
public:
	shared_ptr<Nonterminal> Head;
	vector<shared_ptr<Symbol>> Body;
	int BodySize;
	Production(shared_ptr<Nonterminal>& tHead, vector<shared_ptr<Symbol>>& tBody) : Head(tHead), Body(tBody)
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
		//GrammarList = {vector<Symbol>({}), vector<Symbol>({})};
		//</initGrammarMap>
	}

	void initGrammarMap()
	{
		for(int i = 0; i < GrammarList.size(); i++)
		{
			GrammarMap.insert(make_pair(GrammarList[i].Head->Name, i));
		}
	}
	//创建LR(0)核心项集
	void CreatLR0ItemSet()
	{
		//构造第一个Lr(0)核心项集
		Stauts StartSet;
		StartSet.Index = 0;
		//开始产生式
		StartSet.ItemList.push_back(Triple<int, int, shared_ptr<Symbol>>({0, 0}));
		//第一个项集压栈
		CreatItemClourse(StartSet.ItemList);
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
				auto FindIter = GrammarMap.equal_range(GetNonTermName(CurrentItem.First, CurrentItem.Second));
				if(FindIter.first == FindIter.second)
				{
					//非终结符号找不到索引肿么可能- -;
					abort();
				}
				else
				{
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
	//获取非终结符号的名字,如果是终结符号,挂掉
	string GetNonTermName(int Index, int Position)
	{
		if(CouldExpand(Index, Position))
		{
			return std::move(static_cast<Nonterminal*>( GrammarList[Index].Body[Position].get() )->Name);
		}
		else
		{
			abort();
			return string();
		}
	}
private:
	//文法 0号是产生式头
	vector<Production> GrammarList;

	//LR(0)项集
	vector<Stauts> LRItemSet;

	//文法头到产生式体的映射表
	unordered_multimap<string, int> GrammarMap;
};