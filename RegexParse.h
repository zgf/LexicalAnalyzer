#pragma once
#include "RegexLexParse.h"
using std::unordered_multimap;
// 文法符号enum类
enum class ParseTag
{
	CharSet,
	CharSetComponent,
	CharSetString,
	CharSetUnit,
	CharSet_Back_Start,
	CharSet_End,
	CharSet_Start,
	ChoseClosures_Greedy,
	ChoseSymbol,
	Closures_Greedy,
	Comma,
	CompleteCharSet,
	Express,
	ExpressComplete,
	Factor,
	Mitipute_End,
	NormalChar,
	NormalCharComplete,
	Number,
	NumberChar,
	OtherChar,
	PositiveClosures_Greedy,
	RealWordChar,
	Repeat,
	RepeatRight,
	Repeat_And_BackRefer_End,
	Repeat_Start,
	SimpleUnNamedCatch_Start,
	Start,
	StringHead,
	StringTail,
	SumNumber,
	Term,
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
//Token附加的属性
class Property
{
public:
	string Val;
	ParseTag Tag;
	//在源码中出现的Index
	int Index;

	//<PropertyMember>
	Property(string Value, ParseTag tTag, int tIndex) :Val(Value), Tag(tTag), Index(tIndex)
	{
	}
	Property()
	{
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

class FA;
class RegexParse;

enum class AstTag
{
	Link,
	ChoseSymbol,
	CharSet,
	Repeat,
	NormalChar,
	CharSetUnit,
	StringTail,
	Nullable,
};
class AstNode
{
public:
	AstTag Tag;
	int LeftNodeIndex;
	int RightNodeIndex;
	int ChildNumber;
	AstNode()
	{
	}
	virtual ~AstNode()
	{
	}
	AstNode(AstTag tTag, int tLeftNodeIndex, int tRightNodeIndex, int tChildNumber)
		:Tag(tTag), LeftNodeIndex(tLeftNodeIndex), RightNodeIndex(tRightNodeIndex), ChildNumber(tChildNumber)
	{
	}
public:
	virtual void Accept(FA& Dfa, int CurrentIndex) = 0;
	virtual void AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack) = 0;
};
class NormalChar :public AstNode
{
public:
	char Val;
	NormalChar(AstTag tTag, int tLeftNodeIndex, int tRightNodeIndex, int tChildNumber, char tVal)
		:AstNode(tTag, tLeftNodeIndex, tRightNodeIndex, tChildNumber), Val(tVal)
	{
	}
	NormalChar()
	{
	}
	~NormalChar()
	{
	}
	void Accept(FA& Dfa, int CurrentIndex);
	void AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack);
};
class Nullable : public AstNode
{
public:
	AstTag Tag;
	int LeftNodeIndex;
	int RightNodeIndex;
	int ChildNumber;
	Nullable()
	{
	}
	virtual ~Nullable()
	{
	}
	Nullable(AstTag tTag, int tLeftNodeIndex, int tRightNodeIndex, int tChildNumber)
		:Tag(tTag), LeftNodeIndex(tLeftNodeIndex), RightNodeIndex(tRightNodeIndex), ChildNumber(tChildNumber)
	{
	}
public:
	virtual void Accept(FA& Dfa, int CurrentIndex);
	void AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack);
};
enum class OppsType
{
	Unkown,
	True,
	False,
};

class CharSet :public AstNode
{
public:
	set<pair<int, int>> CharSetRange;
	OppsType Opps;
	CharSet()
	{
	}
	CharSet(AstTag tTag, int tLeftNodeIndex, int tRightNodeIndex, int tChildNumber, set<pair<int, int>>& tCharSetRange, OppsType tOpps)
		: AstNode(tTag, tLeftNodeIndex, tRightNodeIndex, tChildNumber), CharSetRange(tCharSetRange), Opps(tOpps)
	{
	}
	~CharSet()
	{
	}
	void Accept(FA& Dfa, int CurrentIndex);
	void AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack);
};
enum  class GreedyType
{
	Unkown,
	Greedy,
	UnGreedy,
};
class Repeat : public AstNode
{
public:
	GreedyType Greedy;
	pair<int, int>RepeatNumber;
	Repeat()
	{
	}
	Repeat(AstTag tTag, int tLeftNodeIndex, int tRightNodeIndex, int tChildNumber, GreedyType tGreedy, pair<int, int>&tRepeatNumber)
		: AstNode(tTag, tLeftNodeIndex, tRightNodeIndex, tChildNumber), Greedy(tGreedy), RepeatNumber(tRepeatNumber)
	{
	}
	~Repeat()
	{
	}
	void Accept(FA& Dfa, int CurrentIndex);
	void AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack);
};

class ChoseSymbol :public AstNode
{
public:
	~ChoseSymbol()
	{
	}
	ChoseSymbol()
	{
	}
	ChoseSymbol(AstTag tTag, int tLeftNodeIndex, int tRightNodeIndex, int tChildNumber)
		:AstNode(tTag, tLeftNodeIndex, tRightNodeIndex, tChildNumber)
	{
	}
	void Accept(FA& Dfa, int CurrentIndex);
	void AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack);
};
class StringTail : public AstNode
{
public:
	char Val;
	~StringTail()
	{
	}
	StringTail()
	{
	}
	StringTail(AstTag tTag, int tLeftNodeIndex, int tRightNodeIndex, int tChildNumber, char Value)
		:AstNode(tTag, tLeftNodeIndex, tRightNodeIndex, tChildNumber), Val(Value)
	{
	}
	void Accept(FA& Dfa, int CurrentIndex);
	void AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
	{
		abort();
	}
};
class Link :public AstNode
{
public:
	~Link()
	{
	}
	Link()
	{
	}
	Link(AstTag tTag, int tLeftNodeIndex, int tRightNodeIndex, int tChildNumber)
		:AstNode(tTag, tLeftNodeIndex, tRightNodeIndex, tChildNumber)
	{
	}
	void Accept(FA& Dfa, int CurrentIndex);
	void AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack);
};

//<Global>
class RegexParse
{
public:
	RegexParse() :CharMap(255)
	{
		initGrammarList();
		initGrammarMap();
		initTagMap();
		initSemanticMap();
		CreatLR1ItemSet();
	}
private:
	void initTagMap()
	{
		TagMap.insert(make_pair(LexTag::StringTail, ParseTag::StringTail));
		TagMap.insert(make_pair(LexTag::SimpleUnNamedCatch_Start, ParseTag::SimpleUnNamedCatch_Start));
		TagMap.insert(make_pair(LexTag::Mitipute_End, ParseTag::Mitipute_End));
		TagMap.insert(make_pair(LexTag::Closures_Greedy, ParseTag::Closures_Greedy));
		TagMap.insert(make_pair(LexTag::PositiveClosures_Greedy, ParseTag::PositiveClosures_Greedy));
		TagMap.insert(make_pair(LexTag::Comma, ParseTag::Comma));
		TagMap.insert(make_pair(LexTag::CharSetComponent, ParseTag::CharSetComponent));
		TagMap.insert(make_pair(LexTag::ChoseClosures_Greedy, ParseTag::ChoseClosures_Greedy));
		TagMap.insert(make_pair(LexTag::NumberChar, ParseTag::NumberChar));
		TagMap.insert(make_pair(LexTag::OtherChar, ParseTag::OtherChar));
		TagMap.insert(make_pair(LexTag::RealWordChar, ParseTag::RealWordChar));
		TagMap.insert(make_pair(LexTag::CharSet_Start, ParseTag::CharSet_Start));
		TagMap.insert(make_pair(LexTag::CharSet_Back_Start, ParseTag::CharSet_Back_Start));
		TagMap.insert(make_pair(LexTag::CharSet_End, ParseTag::CharSet_End));
		TagMap.insert(make_pair(LexTag::StringHead, ParseTag::StringHead));
		TagMap.insert(make_pair(LexTag::Repeat_Start, ParseTag::Repeat_Start));
		TagMap.insert(make_pair(LexTag::ChoseSymbol, ParseTag::ChoseSymbol));
		TagMap.insert(make_pair(LexTag::Repeat_And_BackRefer_End, ParseTag::Repeat_And_BackRefer_End));
		//<initTagMap>
	}
	void initGrammarList()
	{
		GrammarList.push_back(Production(Symbol(false, ParseTag::Start), vector<Symbol>({Symbol(false, ParseTag::Express)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSet), vector<Symbol>({Symbol(true, ParseTag::CharSet_Start), Symbol(false, ParseTag::CharSetString), Symbol(true, ParseTag::CharSet_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSet), vector<Symbol>({Symbol(true, ParseTag::CharSet_Back_Start), Symbol(false, ParseTag::CharSetString), Symbol(true, ParseTag::CharSet_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSetString), vector<Symbol>({Symbol(false, ParseTag::CharSetUnit), Symbol(false, ParseTag::CharSetString)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSetString), vector<Symbol>({Symbol(false, ParseTag::CharSetUnit)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSetUnit), vector<Symbol>({Symbol(false, ParseTag::NormalChar), Symbol(true, ParseTag::CharSetComponent), Symbol(false, ParseTag::NormalChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSetUnit), vector<Symbol>({Symbol(false, ParseTag::NormalChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CompleteCharSet), vector<Symbol>({Symbol(false, ParseTag::CharSet)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CompleteCharSet), vector<Symbol>({Symbol(false, ParseTag::CharSet), Symbol(false, ParseTag::Repeat)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Express), vector<Symbol>({Symbol(false, ParseTag::Term), Symbol(true, ParseTag::ChoseSymbol), Symbol(false, ParseTag::Express)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Express), vector<Symbol>({Symbol(false, ParseTag::Term)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::ExpressComplete), vector<Symbol>({Symbol(true, ParseTag::SimpleUnNamedCatch_Start), Symbol(false, ParseTag::Express), Symbol(true, ParseTag::Mitipute_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::ExpressComplete), vector<Symbol>({Symbol(true, ParseTag::SimpleUnNamedCatch_Start), Symbol(false, ParseTag::Express), Symbol(true, ParseTag::Mitipute_End), Symbol(false, ParseTag::Repeat)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Factor), vector<Symbol>({Symbol(false, ParseTag::ExpressComplete)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Factor), vector<Symbol>({Symbol(false, ParseTag::CompleteCharSet)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Factor), vector<Symbol>({Symbol(false, ParseTag::NormalCharComplete)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalChar), vector<Symbol>({Symbol(true, ParseTag::RealWordChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalChar), vector<Symbol>({Symbol(true, ParseTag::OtherChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalChar), vector<Symbol>({Symbol(false, ParseTag::Number)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalCharComplete), vector<Symbol>({Symbol(false, ParseTag::NormalChar), Symbol(false, ParseTag::Repeat)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalCharComplete), vector<Symbol>({Symbol(false, ParseTag::NormalChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Number), vector<Symbol>({Symbol(true, ParseTag::NumberChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::Repeat_Start), Symbol(false, ParseTag::RepeatRight), Symbol(true, ParseTag::Repeat_And_BackRefer_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::Closures_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::PositiveClosures_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::ChoseClosures_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatRight), vector<Symbol>({Symbol(false, ParseTag::SumNumber)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatRight), vector<Symbol>({Symbol(false, ParseTag::SumNumber), Symbol(true, ParseTag::Comma), Symbol(false, ParseTag::SumNumber)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::SumNumber), vector<Symbol>({Symbol(false, ParseTag::Number), Symbol(false, ParseTag::SumNumber)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::SumNumber), vector<Symbol>({Symbol(false, ParseTag::Number)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Term), vector<Symbol>({Symbol(false, ParseTag::Factor), Symbol(false, ParseTag::Term)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Term), vector<Symbol>({Symbol(false, ParseTag::Factor)})));
		//<initGrammarMap>
	}
	void initSemanticMap()
	{
		SemanticActionMap.insert(make_pair(0, bind(&RegexParse::Production0, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));

		SemanticActionMap.insert(make_pair(1, bind(&RegexParse::Production1, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(2, bind(&RegexParse::Production2, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(3, bind(&RegexParse::Production3, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(5, bind(&RegexParse::Production5, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(6, bind(&RegexParse::Production6, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(8, bind(&RegexParse::Production8, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(9, bind(&RegexParse::Production9, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(12, bind(&RegexParse::Production12, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(16, bind(&RegexParse::Production16, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(17, bind(&RegexParse::Production17, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(19, bind(&RegexParse::Production19, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(21, bind(&RegexParse::Production21, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(22, bind(&RegexParse::Production22, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(23, bind(&RegexParse::Production23, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(24, bind(&RegexParse::Production24, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(25, bind(&RegexParse::Production25, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(26, bind(&RegexParse::Production26, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(27, bind(&RegexParse::Production27, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(28, bind(&RegexParse::Production28, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(29, bind(&RegexParse::Production29, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(30, bind(&RegexParse::Production30, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(31, bind(&RegexParse::Production31, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));

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
	void CreatItemClourse(vector<Triple<int, int, set<ParseTag>>>& ItemList, unordered_map<int, unordered_map<int, set<ParseTag>>>& SymbolMap)
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
					TermSymbolSet = GetSymbolFirstSet(CurrentItem.first, CurrentItem.second + 1, SymbolMap);
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
						ItemList.push_back(Triple<int, int, set<ParseTag>>(Iter->second, 0, TermSymbolSet));
					}
					else if(ItemList[ResultIndex].TagMap.empty())
					{
						TermSymbolSet.swap(ItemList[ResultIndex].TagMap);
					}
					else if(ItemList[ResultIndex].TagMap != TermSymbolSet)
					{
						//查看该产生式是否缺少还未加入的后缀

						//	set<ParseTag> ResultSet;
						ItemList[ResultIndex].TagMap.insert(TermSymbolSet.begin(), TermSymbolSet.end());
						//	set_union(ItemList[ResultIndex].TagMap.begin(), ItemList[ResultIndex].TagMap.end(), TermSymbolSet.begin(), TermSymbolSet.end(), inserter(ResultSet, ResultSet.begin()));
						//ResultSet.swap(ItemList[ResultIndex].TagMap);
					}
				}
			}
		}
		//排序后对于是否存在该项集就容易很多了;

		//sort(ItemList.begin(), ItemList.end());
	}

	bool IsProductEnd(int Index, int Position)
	{
		return GrammarList[Index].BodySize == Position;
	}

	//获取Symbol的First集合
	set<ParseTag>& GetSymbolFirstSet(int Index, int Position, unordered_map<int, unordered_map<int, set<ParseTag>>>& SymbolMap)
	{
		if(FindSymbolMap(Index, Position, SymbolMap))
		{
			//已经存在
			return SymbolMap[Index][Position];
		}
		else
		{
			set<ParseTag> Result;
			auto& Sym = GetSymbol(Index, Position);
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
					auto& Find = GetSymbolFirstSet(Iter, 0, SymbolMap);
					Result.insert(Find.begin(), Find.end());
				}
			}
			SymbolMap[Index].insert(make_pair(Position, std::move(Result)));

			return SymbolMap[Index][Position];
		}
	}
	bool FindSymbolMap(int Index, int Position, unordered_map<int, unordered_map<int, set<ParseTag>>>& SymbolMap)
	{
		auto & FindIndexIter = SymbolMap.find(Index);
		if(FindIndexIter == SymbolMap.end())
		{
			return false;
		}
		else if(FindIndexIter->second.find(Position) == FindIndexIter->second.end())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	vector<int> GetProductBodyNumber(ParseTag NonTerminal)
	{
		//auto FindIter = GrammarMap.find(NonTerminal);
		auto& EqualPair = GrammarMap.equal_range(NonTerminal);

		vector<int> Result;
		for(auto& Iter = EqualPair.first; Iter != EqualPair.second; Iter++)
		{
			Result.push_back(Iter->second);
		}
		/*for(auto i = 0; i < GrammarMap.count(NonTerminal); i++, FindIter++)
		{
		Result.push_back(FindIter->second);
		}*/
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
		unordered_map<int, unordered_map<int, set<ParseTag>>> SymbolMap;
		//第一个项集的闭包计算
		CreatItemClourse(StartSet.ItemList, SymbolMap);

		//第一个项集压栈
		LR1ItemSet.push_back(StartSet);
		for(int i = 0; i < LR1ItemSet.size(); i++)
		{
			LR1GOTO(LR1ItemSet, i, SymbolMap);
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

	void LR1GOTO(vector<LR1Stauts>& LR1ItemSet, int TargetIndex, unordered_map<int, unordered_map<int, set<ParseTag>>>& SymbolMap)
	{
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
			CreatItemClourse(CurrentStauts.ItemList, SymbolMap);
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
		//这里是捕获的属性堆栈
		//直接用Property构建AST
		/*vector<int> StautsStack;
		vector<shared_ptr<Property>>CatchStack;*/
		StautsStack.push_back(0);
		auto GetIndex = StautsStack.back();
		for(auto i = 0; i < TokenStream.size();)
		{
			auto& Tag = TagMap[TokenStream[i]->GetTag()];
			auto& Val = TokenStream[i]->GetData();
			auto SrcIndex = TokenStream[i]->GetIndex();
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
					//在弹出前执行的语义片段

					//在这里执行语义片段
					auto FindAction = SemanticActionMap.find(CurrentItermList[ResultIndex].first);
					if(FindAction != SemanticActionMap.end())
					{
						FindAction->second(CatchStack, i, TokenStream);
					}
					//需要弹出的状态数量
					auto PopNumber = GrammarList[CurrentItermList[ResultIndex].first].BodySize;

					auto ProductHead = GrammarList[CurrentItermList[ResultIndex].first].Head;
					StautsStack.erase(StautsStack.end() - PopNumber, StautsStack.end());

					auto& FindIter = LR1ItemSet[StautsStack.back()].NextStauts.find(ProductHead.Tag);
					if(ProductHead.Tag == ParseTag::Start)
					{
						cout << "规约成功!";
						//AstRootNode = CatchStack.back();
						//AstStack;
						//AstNodeList;
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
						//这里可能有问题0 0...CatchStack和StautsStack不一定是一一对应关系
						CatchStack.erase(CatchStack.end() - PopNumber, CatchStack.end());
						CatchStack.push_back(shared_ptr<Property>(NewNode));
						GetIndex = FindIter->second;
					}
				}
				//可以规约
			}
			else
			{
				//执行移入.
				StautsStack.push_back(FindIter->second);
				CatchStack.push_back(shared_ptr<Property>(new Property(Val, Tag, SrcIndex)));
				i++;
				GetIndex = FindIter->second;
			}
		}
		return false;
	}
public:
	/*shared_ptr<Property>& GetAst()
	{
	return AstRootNode;
	}*/
private:

private:
	bool NeedChangeIndex(char Index)
	{
		return ( CharMap[Index] == 0 ) || ( CharMap[Index + 1] == CharMap[Index] ) || ( CharMap[Index - 1] == CharMap[Index] );
	}
	int TraversalAstSumNumber(shared_ptr<Property> Root)
	{
		return atoi(Root->Val.c_str());
	}
	void CreatRepeatNode(pair<int, int>&CharSetRange, GreedyType IsGreedy = GreedyType::Unkown)
	{
		int Index = AstNodeList.size();
		AstNodeList.push_back(new Repeat(AstTag::Repeat, -1, -1, 0, IsGreedy, CharSetRange));
		AstStack.push_back(Index);
	}
	void CreatCompleteNode()
	{
		auto RepeatNodeIndex = AstStack.back();
		auto ReaptNodePtr = dynamic_cast<Repeat*>( AstNodeList[RepeatNodeIndex] );
		int StartNumber = ReaptNodePtr->RepeatNumber.first;
		int EndNumber = ReaptNodePtr->RepeatNumber.second;

		if(StartNumber > EndNumber || StartNumber == INT_MAX || EndNumber == 0)
		{
			abort();
		}

		if(EndNumber == INT_MAX)
		{
			if(StartNumber == 0)
			{
				int EndIndex = AstStack.size() - 1;
				AstNodeList[AstStack[EndIndex]]->ChildNumber = 1;
				AstNodeList[AstStack[EndIndex]]->LeftNodeIndex = AstStack[EndIndex - 1];
				AstStack[EndIndex - 1] = AstStack[EndIndex];
				AstStack.pop_back();
			}
			else
			{
				int EndIndex = AstStack.size() - 1;
				AstNodeList[AstStack[EndIndex]]->ChildNumber = 1;
				AstNodeList[AstStack[EndIndex]]->LeftNodeIndex = AstStack[EndIndex - 1];
				AstStack[EndIndex - 1] = AstStack[EndIndex];
				AstStack.pop_back();

				auto RepeatIndex = AstStack.back();
				AstStack.pop_back();
				auto RootNodeIndex = AstNodeList[RepeatIndex]->LeftNodeIndex;
				vector<int> NeedCatList;
				for(auto i = 0; i < StartNumber; i++)
				{
					NeedCatList.push_back(CreatIsomorphismTree(RootNodeIndex));
				}
				AstStack.push_back(LinkVector(NeedCatList));
				AstStack.push_back(RepeatIndex);
				ReaptNodePtr->RepeatNumber.first = 0;
				SetupTwoChild(new Link(AstTag::Link, -1, -1, 0));
			}
		}
		else
		{
			AstStack.pop_back();
			auto RootNodeIndex = AstStack.back();
			AstStack.pop_back();
			AstStack.push_back(ReaptLimitedNumber(RootNodeIndex, StartNumber, EndNumber));
		}
	}
	int ReaptLimitedNumber(int RootNodeIndex, int StartNumber, int EndNumber)
	{
		vector<int> NeedChoseList;
		for(auto j = StartNumber; j <= EndNumber; j++)
		{
			vector<int> NeedCatList;
			if(j == 0)
			{
				NeedCatList.push_back(CreatOneNode(AstTag::Nullable));
			}
			else
			{
				for(auto i = 0; i < j; i++)
				{
					NeedCatList.push_back(CreatIsomorphismTree(RootNodeIndex));
				}
			}
			NeedChoseList.push_back(LinkVector(NeedCatList));
		}
		return ChoseCatVector(NeedChoseList);
	}

	int ChoseCatVector(vector<int>&NodeList)
	{
		if(NodeList.size() != 1)
		{
			for(auto i = 0; i < NodeList.size() - 1; i++)
			{
				auto NewNodeIndex = CreatOneNode(AstTag::ChoseSymbol);
				AstNodeList[NewNodeIndex]->ChildNumber = 2;
				AstNodeList[NewNodeIndex]->LeftNodeIndex = NodeList[i];
				AstNodeList[NewNodeIndex]->RightNodeIndex = NodeList[i + 1];
				NodeList[i + 1] = NewNodeIndex;
			}
		}

		return NodeList.back();
	}
	int LinkVector(vector<int>&NodeList)
	{
		if(NodeList.size() != 1)
		{
			for(auto i = 0; i < NodeList.size() - 1; i++)
			{
				auto NewNodeIndex = CreatOneNode(AstTag::Link);
				AstNodeList[NewNodeIndex]->ChildNumber = 2;
				AstNodeList[NewNodeIndex]->LeftNodeIndex = NodeList[i];
				AstNodeList[NewNodeIndex]->RightNodeIndex = NodeList[i + 1];
				NodeList[i + 1] = NewNodeIndex;
			}
		}

		return NodeList.back();
	}
	int CreatIsomorphismTree(int RootIndex)
	{
		map<int, int> NodeMap;
		list<int> Stack;
		Stack.push_back(RootIndex);
		NodeMap.insert(make_pair(RootIndex, CreatOneNode(AstNodeList[RootIndex]->Tag)));
		while(!Stack.empty())
		{
			auto CurrentIndex = Stack.front();
			GetAstNodePtr(CurrentIndex)->AdjustAst(*this, CurrentIndex, NodeMap, Stack);
			Stack.pop_front();
		}
		return NodeMap[RootIndex];
	}
	AstNode* GetAstNodePtr(int Index)
	{
		return AstNodeList[Index];
	}
	int CreatOneNode(AstTag Tag)
	{
		int Index = AstNodeList.size();
		if(Tag == AstTag::CharSet)
		{
			AstNodeList.push_back(new CharSet);
			AstNodeList.back()->Tag = AstTag::CharSet;
		}
		else if(Tag == AstTag::ChoseSymbol)
		{
			AstNodeList.push_back(new ChoseSymbol);
			AstNodeList.back()->Tag = AstTag::ChoseSymbol;
		}
		else if(Tag == AstTag::Link)
		{
			AstNodeList.push_back(new Link);
			AstNodeList.back()->Tag = AstTag::Link;
		}
		else if(Tag == AstTag::NormalChar)
		{
			AstNodeList.push_back(new NormalChar);
			AstNodeList.back()->Tag = AstTag::NormalChar;
		}
		else if(Tag == AstTag::Nullable)
		{
			AstNodeList.push_back(new Nullable);
			AstNodeList.back()->Tag = AstTag::Nullable;
		}
		else if(Tag == AstTag::Repeat)
		{
			AstNodeList.push_back(new Repeat);
			AstNodeList.back()->Tag = AstTag::Repeat;
		}
		else
		{
			abort();
		}
		AstNodeList.back()->LeftNodeIndex = -1;
		AstNodeList.back()->RightNodeIndex = -1;
		AstNodeList.back()->ChildNumber = 0;

		return Index;
	}
	void SetupTwoChild(AstNode* Father)
	{
		Father->RightNodeIndex = AstStack.back();
		AstStack.pop_back();
		Father->LeftNodeIndex = AstStack.back();
		AstStack.pop_back();
		Father->ChildNumber = 2;
		AstStack.push_back(AstNodeList.size());
		AstNodeList.push_back(Father);
	}
	void CreatNormalCharNode(shared_ptr<Property> Root)
	{
		AstStack.push_back(AstNodeList.size());
		AstNodeList.push_back(new NormalChar(AstTag::NormalChar, -1, -1, 0, Root->Val[0]));
	}

public:
	void AdjustAst(Link* NodePtr, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
	{
		auto NewNodePtr = (Link*)AstNodeList[NodeMap[CurrentIndex]];
		*NewNodePtr = *NodePtr;
		CopyNodeChild(AstNodeList[NodePtr->LeftNodeIndex], NodePtr->LeftNodeIndex, NodeMap, Stack);
		CopyNodeChild(AstNodeList[NodePtr->RightNodeIndex], NodePtr->RightNodeIndex, NodeMap, Stack);
	}
	void AdjustAst(ChoseSymbol* NodePtr, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
	{
		auto NewNodePtr = (ChoseSymbol*)AstNodeList[NodeMap[CurrentIndex]];
		*NewNodePtr = *NodePtr;
		CopyNodeChild(AstNodeList[NodePtr->LeftNodeIndex], NodePtr->LeftNodeIndex, NodeMap, Stack);
		CopyNodeChild(AstNodeList[NodePtr->RightNodeIndex], NodePtr->RightNodeIndex, NodeMap, Stack);
	}
	void AdjustAst(CharSet* NodePtr, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
	{
		auto NewNodePtr = (CharSet*)AstNodeList[NodeMap[CurrentIndex]];
		*NewNodePtr = *NodePtr;
	}
	void AdjustAst(NormalChar* NodePtr, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
	{
		auto NewNodePtr = (NormalChar*)AstNodeList[NodeMap[CurrentIndex]];
		*NewNodePtr = *NodePtr;
	}
	void AdjustAst(Nullable* NodePtr, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
	{
		auto NewNodePtr = (Nullable*)AstNodeList[NodeMap[CurrentIndex]];
		*NewNodePtr = *NodePtr;
	}
	void AdjustAst(Repeat* NodePtr, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
	{
		auto NewNodePtr = (Repeat*)AstNodeList[NodeMap[CurrentIndex]];
		*NewNodePtr = *NodePtr;
	}
	void CopyNodeChild(AstNode* NewNodePtr, int LeftNodeIndex, map<int, int>&NodeMap, list<int>& Stack)
	{
		if(NewNodePtr->LeftNodeIndex != -1)
		{
			auto& FindLeft = NodeMap.find(NewNodePtr->LeftNodeIndex);
			if(FindLeft == NodeMap.end())
			{
				auto LeftNewNodeIndex = CreatOneNode(AstNodeList[NewNodePtr->LeftNodeIndex]->Tag);
				NodeMap.insert(make_pair(NewNodePtr->LeftNodeIndex, LeftNewNodeIndex));
				Stack.push_back(NewNodePtr->LeftNodeIndex);
			}
			else
			{
				NewNodePtr->LeftNodeIndex = NodeMap[NewNodePtr->RightNodeIndex];
			}
		}
	}
	AstNode* GetAst()
	{
		return AstNodeList[AstStack.back()];
	}
	void DeleteAst()
	{
		for(auto i = 0; i < AstNodeList.size(); i++)
		{
			delete AstNodeList[i];
		}
	}
	vector<int> GetCharMap()
	{
		return CharMap;
	}
	int GetAstRootIndex()
	{
		return AstStack.back();
	}
	vector<AstNode*> GetAstNodeList()
	{
		return AstNodeList;
	}
	//<UserDefineFunc>
private:

	//Start Express
	void Production0(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		AstStack.push_back(AstNodeList.size());
		AstNodeList.push_back(new StringTail(AstTag::StringTail, -1, -1, 0, char(4)));
		SetupTwoChild(new Link(AstTag::Link, -1, -1, 0));
		CharMap[4] = IncreaseIndex++;
	};

	//CharSet "[" CharSetString "]"
	void Production1(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		auto CharSetPointer = dynamic_cast<CharSet*>( AstNodeList[AstStack.back()] );
		CharSetPointer->Opps = OppsType::False;
		CharSetPointer->Tag = AstTag::CharSet;
	};
	//CharSet "[^" CharSetString "]"
	void Production2(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		auto CharSetPointer = dynamic_cast<CharSet*>( AstNodeList[AstStack.back()] );
		CharSetPointer->Opps = OppsType::True;
		CharSetPointer->Tag = AstTag::CharSet;
	};
	//CharSetString CharSetUnit CharSetString
	void Production3(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		int RightSetIndex = AstStack.back();
		AstStack.pop_back();
		int LeftSetIndex = AstStack.back();
		AstStack.pop_back();
		auto LeftSetPointer = dynamic_cast<CharSet*>( AstNodeList[LeftSetIndex] );
		auto RightSetPointer = dynamic_cast<CharSet*>( AstNodeList[RightSetIndex] );
		set<pair<int, int>>NewSet;
		set_union(LeftSetPointer->CharSetRange.begin(), LeftSetPointer->CharSetRange.end(), RightSetPointer->CharSetRange.begin(), RightSetPointer->CharSetRange.end(), inserter(NewSet, NewSet.begin()));
		auto Index = AstNodeList.size();
		AstNodeList.push_back(new CharSet(AstTag::CharSetUnit, -1, -1, 0, NewSet, OppsType::Unkown));
		AstStack.push_back(Index);
	};
	//CharSetUnit NormalChar "-" NormalChar
	void Production5(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		for(int i = CatchStack[CatchStack.size() - 3]->Val[0]; i < CatchStack[CatchStack.size() - 1]->Val[0]; i++)
		{
			if(NeedChangeIndex(i))
			{
				CharMap[i] = IncreaseIndex;
			}
		}
		IncreaseIndex++;
		set<pair<int, int>>CharSetRange;
		CharSetRange.insert(pair<int, int>(CatchStack[CatchStack.size() - 3]->Val[0], CatchStack[CatchStack.size() - 1]->Val[0]));
		auto Index = AstNodeList.size();
		AstNodeList.push_back(new CharSet(AstTag::CharSetUnit, -1, -1, 0, CharSetRange, OppsType::Unkown));
		AstStack.pop_back();
		AstStack.pop_back();
		AstStack.push_back(Index);
	};
	//CharSetUnit NormalChar
	void Production6(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		set<pair<int, int>>CharSetRange;
		CharSetRange.insert(pair<int, int>(CatchStack[CatchStack.size() - 1]->Val[0], CatchStack[CatchStack.size() - 1]->Val[0]));
		auto Index = AstNodeList.size();
		AstNodeList.push_back(new CharSet(AstTag::CharSetUnit, 0, 0, 0, CharSetRange, OppsType::Unkown));
		AstStack.pop_back();
		AstStack.push_back(Index);
	};
	//CompleteCharSet CharSet Repeat
	void Production8(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		CreatCompleteNode();
	};
	//Express Term  "|" Express
	void Production9(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		SetupTwoChild(new ChoseSymbol(AstTag::ChoseSymbol, -1, -1, 0));
	};
	//ExpressComplete "(" Express ")" Repeat
	void Production12(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		CreatCompleteNode();
	};
	//NormalChar  "RealWordChar"
	void Production16(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		if(NeedChangeIndex(CatchStack[CatchStack.size() - 1]->Val[0]))
		{
			CharMap[CatchStack[CatchStack.size() - 1]->Val[0]] = IncreaseIndex;
			IncreaseIndex++;
		}
		CreatNormalCharNode(CatchStack[CatchStack.size() - 1]);
		NewNode->Val = CatchStack[CatchStack.size() - 1]->Val;
	};
	//NormalChar "OtherChar"
	void Production17(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		if(NeedChangeIndex(CatchStack[CatchStack.size() - 1]->Val[0]))
		{
			CharMap[CatchStack[CatchStack.size() - 1]->Val[0]] = IncreaseIndex;
			IncreaseIndex++;
		}
		CreatNormalCharNode(CatchStack[CatchStack.size() - 1]);
		NewNode->Val = CatchStack[CatchStack.size() - 1]->Val;
	};
	//NormalCharComplete NormalChar Repeat
	void Production19(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		CreatCompleteNode();
	};
	//Number "NumberChar"
	void Production21(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		if(NeedChangeIndex(CatchStack[CatchStack.size() - 1]->Val[0]))
		{
			CharMap[CatchStack[CatchStack.size() - 1]->Val[0]] = IncreaseIndex;
			IncreaseIndex++;
		}
		CreatNormalCharNode(CatchStack[CatchStack.size() - 1]);
		NewNode->Val = CatchStack[CatchStack.size() - 1]->Val;
	};
	//Repeat "{" RepeatRight "}"
	void Production22(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		if(CatchStack[CatchStack.size() - 1]->Val == "}")
		{
			dynamic_cast<Repeat*>( AstNodeList[AstStack.back()] )->Greedy = GreedyType::Greedy;
		}
		else if(CatchStack[CatchStack.size() - 1]->Val == "}?")
		{
			dynamic_cast<Repeat*>( AstNodeList[AstStack.back()] )->Greedy = GreedyType::UnGreedy;
		}
		else
		{
			abort();
		}
	};
	//Repeat "*"

	void Production23(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		pair<int, int>CharSetRange;
		CharSetRange.first = 0;
		CharSetRange.second = INT_MAX;
		CreatRepeatNode(CharSetRange, GreedyType::Greedy);
	};
	//Repeat "+"

	void Production24(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		pair<int, int>CharSetRange;
		CharSetRange.first = 1;
		CharSetRange.second = INT_MAX;
		CreatRepeatNode(CharSetRange, GreedyType::Greedy);
	};
	//Repeat "?"
	void Production25(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		pair<int, int>CharSetRange;
		CharSetRange.first = 0;
		CharSetRange.second = 1;
		CreatRepeatNode(CharSetRange, GreedyType::Greedy);
	};
	//RepeatRight SumNumber
	void Production26(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		pair<int, int>CharSetRange;
		CharSetRange.first = CharSetRange.second = atoi(CatchStack[CatchStack.size() - 1]->Val.c_str());
		AstStack.pop_back();
		CreatRepeatNode(CharSetRange);
	};
	//RepeatRight SumNumber "," SumNumber
	void Production27(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		pair<int, int>CharSetRange;
		CharSetRange.first = atoi(CatchStack[CatchStack.size() - 3]->Val.c_str());
		CharSetRange.second = atoi(CatchStack[CatchStack.size() - 1]->Val.c_str());
		AstStack.pop_back();
		AstStack.pop_back();
		CreatRepeatNode(CharSetRange);
	};
	//SumNumber Number SumNumber
	void Production28(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		NewNode->Val = CatchStack[CatchStack.size() - 2]->Val + CatchStack[CatchStack.size() - 1]->Val;
	};
	//SumNumber 	Number
	void Production29(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		NewNode->Val = CatchStack[CatchStack.size() - 1]->Val;
	};
	//Term  Factor Term
	void Production30(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());

		SetupTwoChild(new Link(AstTag::Link, -1, -1, 0));
	};
	//Term Factor
	void Production31(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property());
	};

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
	unordered_map<int, function<void(vector<shared_ptr<Property>>&, int, vector<shared_ptr<RegexToken>>&)>> SemanticActionMap;
	//捕获属性的状态栈和属性栈
	vector<int> StautsStack;
	vector<shared_ptr<Property>>CatchStack;
	//每次执行语义动作时候构造的新节点挂载位置
	shared_ptr<Property> NewNode;

	vector<int>CharMap;
	int IncreaseIndex = 1;
	vector<AstNode*>AstNodeList;
	vector<int>AstStack;

	//<DataMember>

	//AST根节点
	//	shared_ptr<Property> AstRootNode;
};
void Link::AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
{
	Parse.AdjustAst(this, CurrentIndex, NodeMap, Stack);
}
void ChoseSymbol::AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
{
	Parse.AdjustAst(this, CurrentIndex, NodeMap, Stack);
}
void Repeat::AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
{
	Parse.AdjustAst(this, CurrentIndex, NodeMap, Stack);
}
void CharSet::AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
{
	Parse.AdjustAst(this, CurrentIndex, NodeMap, Stack);
}
void Nullable::AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
{
	Parse.AdjustAst(this, CurrentIndex, NodeMap, Stack);
}
void NormalChar::AdjustAst(RegexParse& Parse, int CurrentIndex, map<int, int>&NodeMap, list<int>& Stack)
{
	Parse.AdjustAst(this, CurrentIndex, NodeMap, Stack);
}