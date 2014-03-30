#pragma once
#include "RegexLexParse.h"
using std::unordered_multimap;
// 文法符号enum类
enum class ParseTag
{

	//<ParseTag>
};
//语法分析符号

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
//创建对于Symbol的Hash和equal_to的仿函数
struct HashValue
{
	size_t operator()(const Symbol& _Keyval)
	{
		std::hash<ParseTag> a;
		return std::move(a(_Keyval.Tag));
	}
};
struct EqualTo
{
	bool operator()(const Symbol& _Left, const Symbol& _Right) const
	{
		return _Left.IsTerminal == _Right.IsTerminal && _Left.Tag == _Right.Tag;
	}
};

template<typename X, typename Y, typename Z = set<ParseTag>>
class Triple
{
public:
	//第几条文法
	X first;
	//第X条文法的第Y项
	Y second;
	Z TagMap;
	Triple(X a, Y b) : first(a), second(b)
	{
	}
	Triple(X a, Y b, vector<ParseTag>& tList) :first(a), second(b)
	{
		TagMap.insert(tList.begin(), tList.end());
	}
	Triple(X a, Y b, set<ParseTag>& tMap) :first(a), second(b)
	{
		TagMap.insert(tMap.begin(), tMap.end());
	}
	bool operator !=( Triple& a )
	{
		return this->first != a.first || this->second != a.second || a.TagMap != this->TagMap;
	}
	friend bool operator==( Triple& a, Triple& b )
	{
		return a.first == b.first && a.second == b.second && a.TagMap == b.TagMap;
	}
	friend bool operator<( Triple& a, Triple& b )
	{
		if(a.first == b.first)
		{
			return a.second < b.second;
		}
		else
		{
			return a.first < b.first;
		}
	}
};
//
//状态集
class LR1Stauts
{
public:
	//z状态集编号
	//int Index;
	// 核心项结束index.超尾
	int CoreItermNumber;
	vector<Triple<int, int, set<ParseTag>>> ItemList;

	unordered_map<ParseTag, int> NextStauts;
	friend bool operator== ( LR1Stauts& Left, LR1Stauts&Right )
	{
		if(Left.ItemList.size() == Right.ItemList.size())
		{
			for(auto i = 0; i < Left.ItemList.size(); i++)
			{
				if(Left.ItemList[i] != Right.ItemList[i])
				{
					return false;
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	friend	bool operator<( LR1Stauts&Left, LR1Stauts& Right )
	{
		if(Left.ItemList.size() == Right.ItemList.size())
		{
			for(auto i = 0; i < Left.ItemList.size(); i++)
			{
				if(Left.ItemList[i].first == Right.ItemList[i].first)
				{
					if(Left.ItemList[i].second == Right.ItemList[i].second)
					{
						continue;
					}
					else
					{
						return Left.ItemList[i].second < Right.ItemList[i].second;
					}
				}
				else
				{
					return Left.ItemList[i].first < Right.ItemList[i].first;
				}
			}
		}
		else
		{
			return Left.ItemList.size() < Right.ItemList.size();
		}

		abort();
		return false;
	}
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

//<Global>
class RegexParse
{
public:
	RegexParse()
	{
		initGrammarList();
		initGrammarMap();
		initTagMap();
		CreatLR1ItemSet();
	}
private:
	void initTagMap()
	{

		//<initTagMap>
	}
	void initGrammarList()
	{

		//<initGrammarMap>
	}
	void initSemanticMap()
	{
		
		//<initSemanticMap>
	}
	void initGrammarMap()
	{
		for(int i = 0; i < GrammarList.size(); i++)
		{
			GrammarMap.insert(make_pair(GrammarList[i].Head.Tag, i));
		}
	}
private:
	//计算项集的闭包并加入项集
	void CreatItemClourse(vector<Triple<int, int, set<ParseTag>>>& ItemList)
	{
		for(int i = 0; i < ItemList.size(); i++)
		{
			auto& CurrentItem = ItemList[i];
			if(CouldExpand(CurrentItem.first, CurrentItem.second))
			{
				//说明是非终结符号
				//查看在当前产生式体中,该非终结符号是否有后缀
				set<ParseTag> TermSymbolSet;
				if(IsProductEnd(CurrentItem.first, CurrentItem.second + 1))
				{
					//没有后缀就把终结符号集合符号继承过来
					TermSymbolSet = CurrentItem.TagMap;
				}
				else
				{
					//有后缀就把后缀符号加入
					TermSymbolSet = GetSymbolFirstSet(CurrentItem.first, CurrentItem.second + 1);
				}

				//在CouldExpand里面已经测试好了,肯定是非终结符号
				auto GetTag = GetSymbolTag(CurrentItem.first, CurrentItem.second);
				auto FindIter = GrammarMap.equal_range(GetTag);

				for(auto Iter = FindIter.first; Iter != FindIter.second; Iter++)
				{
					//获取的是产生式的编号
					//查看该产生式是否已经加入;
					auto ResultIndex = HasAddThisProduct(ItemList, Iter->second, 0);
					if(-1 == ResultIndex)
					{
						ItemList.push_back({Iter->second, 0, TermSymbolSet});
					}
					else if(ItemList[ResultIndex].TagMap.empty())
					{
						TermSymbolSet.swap(ItemList[ResultIndex].TagMap);
					}
					else if(ItemList[ResultIndex].TagMap != TermSymbolSet)
					{
						//查看该产生式是否缺少还未加入的后缀

						set<ParseTag> ResultSet;
						set_union(ItemList[ResultIndex].TagMap.begin(), ItemList[ResultIndex].TagMap.end(), TermSymbolSet.begin(), TermSymbolSet.end(), inserter(ResultSet, ResultSet.begin()));
						ResultSet.swap(ItemList[ResultIndex].TagMap);
					}
				}
			}
		}
		//排序后对于是否存在该项集就容易很多了;

		sort(ItemList.begin(), ItemList.end());
	}

	bool IsProductEnd(int Index, int Position)
	{
		return GrammarList[Index].BodySize == Position;
	}

	//获取Symbol的First集合
	set<ParseTag> GetSymbolFirstSet(int Index, int Position)
	{
		set<ParseTag> Result;
		auto Sym = GetSymbol(Index, Position);
		if(Sym.IsTerminal == true)
		{
			//终结符号
			Result.insert(Sym.Tag);
		}
		else
		{
			//非终结符号
			for(auto& Iter : GetProductBodyNumber(Sym.Tag))
			{
				auto& Find = GetSymbolFirstSet(Iter, 0);
				Result.insert(Find.begin(), Find.end());
			}
		}
		return std::move(Result);
	}

	vector<int> GetProductBodyNumber(ParseTag NonTerminal)
	{
		vector<int> Result;
		auto FindIter = GrammarMap.find(NonTerminal);
		for(auto i = 0; i < GrammarMap.count(NonTerminal); i++, FindIter++)
		{
			Result.push_back(FindIter->second);
		}
		return std::move(Result);
	}

	//查看该编号产生式是否已经加入
	int HasAddThisProduct(vector<Triple<int, int, set<ParseTag>>>& ItemList, int Index, int Position)
	{
		for(auto i = 0; i < ItemList.size(); i++)
		{
			if(ItemList[i].first == Index && ItemList[i].second == Position)
			{
				return i;
			}
		}
		return -1;
	}

	//判断是不是.号后面是非终结符号
	//Index 是文法编号 Position是第X个位置的点
	bool CouldExpand(int Index, int Position)
	{
		if(Position == GrammarList[Index].BodySize)
		{
			return false;
		}
		else
		{
			return GrammarList[Index].Body[Position].IsTerminal == false;
		}
	}
	ParseTag GetSymbolTag(int Index, int Position)
	{
		return GrammarList[Index].Body[Position].Tag;
	}
	//GOTO(int,int,X)
	int ProductionSize(int Index)
	{
		return GrammarList[Index].BodySize;
	}

	//创建LR(0)核心项集

	void CreatLR1ItemSet()
	{
		//构造第一个Lr(0)核心项集
		LR1Stauts StartSet;
		//StartSet.Index = 0;
		//开始产生式
		//第一个项压入项集
		set<ParseTag>TagList;
		TagList.insert(ParseTag::StringTail);
		StartSet.ItemList.push_back(Triple<int, int, set<ParseTag>>({0, 0, TagList}));
		StartSet.CoreItermNumber = 1;
		//第一个项集的闭包计算
		CreatItemClourse(StartSet.ItemList);

		//第一个项集压栈
		LR1ItemSet.push_back(StartSet);
		for(int i = 0; i < LR1ItemSet.size(); i++)
		{
			LR1GOTO(LR1ItemSet, i);
		}
		AcceptIndex = GetAcceptIndex(LR1ItemSet);

		//auto Finish = CheckLR1GOTO();
		//auto chose = CheckIndex();
	}
	//获取接受状态索引
	int GetAcceptIndex(vector<LR1Stauts>& LR1ItemSet)
	{
		//获取最初产生式的文法符号列表
		vector<ParseTag> AcceptList(GrammarList[0].Body.size());
		for(auto i = 0; i < GrammarList[0].BodySize; i++)
		{
			AcceptList[i] = GrammarList[0].Body[i].Tag;
		}
		auto GetIndex = 0;
		for(auto& Iter : AcceptList)
		{
			GetIndex = LR1ItemSet[GetIndex].NextStauts.find(Iter)->second;
		}
		return GetIndex;
	}

	//检测LR0项集是否正确 不存在重复项集
	bool CheckLR1GOTO()
	{
		auto Length = LR1ItemSet.size();
		LR1ItemSet.erase(unique(LR1ItemSet.begin(), LR1ItemSet.end()), LR1ItemSet.end());
		return LR1ItemSet.size() == Length;
	}

	void LR1GOTO(vector<LR1Stauts>& LR1ItemSet, int TargetIndex)
	{
		//今日做
		//存放当前LR1Stauts的后一个符号集合
		unordered_multimap<ParseTag, Triple<int, int, set<ParseTag>>> NextSet;
		auto& Target = LR1ItemSet[TargetIndex];
		for(auto& Iter : Target.ItemList)
		{
			//说明没到产生式尾部
			if(Iter.second != ProductionSize(Iter.first))
			{
				NextSet.insert(make_pair(GetSymbolTag(Iter.first, Iter.second), Triple<int, int, set<ParseTag>>(Iter.first, Iter.second + 1, Iter.TagMap)));
			}
		}

		//根据key分类,同key到同一个项集
		for(auto KeyIter = NextSet.begin(); KeyIter != NextSet.end(); KeyIter = NextSet.upper_bound(KeyIter->first))
		{
			LR1Stauts CurrentStauts;
			auto KeyCount = NextSet.count(KeyIter->first);
			auto i = 0;
			for(auto MinIter = KeyIter; i < KeyCount; MinIter++, i++)
			{
				CurrentStauts.ItemList.push_back(MinIter->second);
			}
			CurrentStauts.CoreItermNumber = KeyCount;
			if(LR1ItemSet.size() == 11)
			{
				int a = 0;
			}
			CreatItemClourse(CurrentStauts.ItemList);
			//判断是否新状态在项集集合中;
			auto result = HasThisItemSet(CurrentStauts);
			if(result == -1)
			{
				LR1ItemSet[TargetIndex].NextStauts.insert(make_pair(KeyIter->first, LR1ItemSet.size()));
				LR1ItemSet.push_back(CurrentStauts);
			}
			else
			{
				LR1ItemSet[TargetIndex].NextStauts.insert(make_pair(KeyIter->first, result));
			}
		}
	}

	//判断项集集合中是否存在该项集,如果存在就返回索引,不存在就返回-1
	int HasThisItemSet(LR1Stauts& Target)
	{
		for(int i = 0; i < LR1ItemSet.size(); i++)
		{
			if(HasSameItemList(Target, LR1ItemSet[i]))
			{
				return i;
			}
		}
		return -1;
	}

	//是否有同样的项集
	bool HasSameItemList(LR1Stauts& Left, LR1Stauts& Right)
	{
		auto Length = Left.ItemList.size();
		if(Length == Right.ItemList.size())
		{
			for(auto i = 0; i < Length; i++)
			{
				if(Left.ItemList[i] != Right.ItemList[i])
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}

		return true;
	}
	Symbol GetSymbol(int Index, int Position)
	{
		return GrammarList[Index].Body[Position];
	}

	//查看该项集中是否存在到达产生式尾部的核心项,存在返回索引,不存在返回-1;
	int HasCurrentProductTailIterm(vector<Triple<int, int, set<ParseTag>>>& LR1ItermList, ParseTag Tag)
	{
		vector<int> Result;
		for(auto j = 0; j < LR1ItermList.size(); j++)
		{
			auto& CurrentIterm = LR1ItermList[j];
			if(IsProductEnd(CurrentIterm.first, CurrentIterm.second) && CurrentIterm.TagMap.find(Tag) != CurrentIterm.TagMap.end())
			{
				return j;
			}
		}
		return -1;
	}
public:
	bool ParsingRegex(vector<shared_ptr<RegexToken>>& TokenStream)
	{
		vector<int> StautsStack;
		StautsStack.push_back(0);
		auto GetIndex = StautsStack.back();
		for(auto i = 0; i < TokenStream.size();)
		{
			auto& Tag = TagMap[TokenStream[i]->GetTag()];
			auto& FindIter = LR1ItemSet[GetIndex].NextStauts.find(Tag);
			if(FindIter == LR1ItemSet[GetIndex].NextStauts.end())
			{
				//无法移入,需要规约后尝试
				auto& CurrentItermList = LR1ItemSet[GetIndex].ItemList;
				auto ResultIndex = HasCurrentProductTailIterm(CurrentItermList, Tag);
				if(ResultIndex == -1)
				{
					//出现错误字符
					cout << "Error! Index:" << TokenStream[i]->GetIndex() << " Data:" << TokenStream[i]->GetData() << endl;
					abort();
					//这里应该对错误进行处理
					// TO DO
				}
				else
				{
					//需要弹出的状态数量
					auto PopNumber = GrammarList[CurrentItermList[ResultIndex].first].BodySize;

					auto ProductHead = GrammarList[CurrentItermList[ResultIndex].first].Head;
					StautsStack.erase(StautsStack.end() - PopNumber, StautsStack.end());

					auto& FindIter = LR1ItemSet[StautsStack.back()].NextStauts.find(ProductHead.Tag);
					if(ProductHead.Tag == ParseTag::Start)
					{
						cout << "规约成功!";
						return true;
					}
					else if(FindIter == LR1ItemSet[StautsStack.back()].NextStauts.end())
					{
						cout << "Error! pop后移入tag进入新状态失败";
						abort();
					}
					else
					{
						StautsStack.push_back(FindIter->second);
						GetIndex = FindIter->second;
						auto FindAction = SemanticActionMap.find(CurrentItermList[ResultIndex].first);
						if(FindAction != SemanticActionMap.end())
						{
							FindAction->second();
						}
						//在这里执行语义片段
					}
				}
				//可以规约
			}
			else
			{
				StautsStack.push_back(FindIter->second);
				i++;
				GetIndex = FindIter->second;
			}
		}
		return false;
	}
private:
	//<UserDefineFunc>
private:

	
	//<SemanticFunc>

private:
	//文法 0号是产生式头
	vector<Production> GrammarList;

	//LR(0)项集
	vector<LR1Stauts> LR1ItemSet;

	//创建LR(1)项集GOTO表
	//文法头到产生式体的映射表
	unordered_multimap<ParseTag, int> GrammarMap;

	//接受状态的编号.在该状态上到达文本串尾部或者接受StringTail 字符就表示接受
	int AcceptIndex;

	//LexTag 到ParseTag的映射.
	unordered_map<LexTag, ParseTag> TagMap;

	//文法列表索引到语义片段的索引
	unordered_map<int, function<void()>> SemanticActionMap;

	//<DataMember>
};
