#pragma once
#include "RegexLexParse.h"
using std::unordered_multimap;
// �ķ�����enum��
enum class ParseTag
{
	StringTail,
	SimpleUnNamedCatch_Start,
	Mitipute_End,
	Comma,
	CharSetComponent,
	ChoseClosures_Greedy,
	ChoseClosures_UnGreedy,
	Closures_Greedy,
	Closures_UnGreedy,
	NumberChar,
	OtherChar,
	PositiveClosures_Greedy,
	PositiveClosures_UnGreedy,
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
	CharSetUnit,
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
	//<ParseTag>
};
//�﷨��������

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
//��������Symbol��Hash��equal_to�ķº���
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
	//�ڼ����ķ�
	X first;
	//��X���ķ��ĵ�Y��
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
//Token���ӵ�����
class Property
{
public:
	string Val;
	ParseTag Tag;
	//��Դ���г��ֵ�Index
	int Index;

	bool Opps_Greedy;

	shared_ptr<Property> RightNode;
	shared_ptr<Property> LeftNode;
	int ChildNumber;
	//<PropertyMember>
	Property(string Value, ParseTag tTag, int tIndex) :Val(Value), Tag(tTag), Index(tIndex)
	{
	}
	Property()
	{
	}
};

//
//״̬��
class LR1Stauts
{
public:
	//z״̬�����
	//int Index;
	// ���������index.��β
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
//����ʽ
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
		TagMap.insert(make_pair(LexTag::StringTail, ParseTag::StringTail));
		TagMap.insert(make_pair(LexTag::SimpleUnNamedCatch_Start, ParseTag::SimpleUnNamedCatch_Start));
		TagMap.insert(make_pair(LexTag::Mitipute_End, ParseTag::Mitipute_End));
		TagMap.insert(make_pair(LexTag::Comma, ParseTag::Comma));
		TagMap.insert(make_pair(LexTag::CharSetComponent, ParseTag::CharSetComponent));
		TagMap.insert(make_pair(LexTag::ChoseClosures_Greedy, ParseTag::ChoseClosures_Greedy));
		TagMap.insert(make_pair(LexTag::ChoseClosures_UnGreedy, ParseTag::ChoseClosures_UnGreedy));
		TagMap.insert(make_pair(LexTag::Closures_Greedy, ParseTag::Closures_Greedy));
		TagMap.insert(make_pair(LexTag::Closures_UnGreedy, ParseTag::Closures_UnGreedy));
		TagMap.insert(make_pair(LexTag::NumberChar, ParseTag::NumberChar));
		TagMap.insert(make_pair(LexTag::OtherChar, ParseTag::OtherChar));
		TagMap.insert(make_pair(LexTag::PositiveClosures_Greedy, ParseTag::PositiveClosures_Greedy));
		TagMap.insert(make_pair(LexTag::PositiveClosures_UnGreedy, ParseTag::PositiveClosures_UnGreedy));
		TagMap.insert(make_pair(LexTag::RealWordChar, ParseTag::RealWordChar));
		TagMap.insert(make_pair(LexTag::CharSet_Start, ParseTag::CharSet_Start));
		TagMap.insert(make_pair(LexTag::CharSet_Back_Start, ParseTag::CharSet_Back_Start));
		TagMap.insert(make_pair(LexTag::CharSet_End, ParseTag::CharSet_End));
		TagMap.insert(make_pair(LexTag::StringHead, ParseTag::StringHead));
		TagMap.insert(make_pair(LexTag::Repeat_Start, ParseTag::Repeat_Start));
		TagMap.insert(make_pair(LexTag::ChoseSymbol, ParseTag::ChoseSymbol));
		TagMap.insert(make_pair(LexTag::Repeat_And_BackRefer_End, ParseTag::Repeat_And_BackRefer_End));
		TagMap.insert(make_pair(LexTag::Repeat_End_Greedy, ParseTag::Repeat_End_Greedy));
		//<initTagMap>
	}
	void initGrammarList()
	{
		GrammarList.push_back(Production(Symbol(false, ParseTag::Start), vector<Symbol>({Symbol(false, ParseTag::Express)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSet), vector<Symbol>({Symbol(true, ParseTag::CharSet_Start), Symbol(false, ParseTag::CharSetString), Symbol(true, ParseTag::CharSet_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSet), vector<Symbol>({Symbol(true, ParseTag::CharSet_Back_Start), Symbol(false, ParseTag::CharSetString), Symbol(true, ParseTag::CharSet_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSetString), vector<Symbol>({Symbol(false, ParseTag::CharSetUnit), Symbol(false, ParseTag::CharSetString)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSetUnit), vector<Symbol>({Symbol(false, ParseTag::NormalChar), Symbol(true, ParseTag::CharSetComponent), Symbol(false, ParseTag::NormalChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSetUnit), vector<Symbol>({Symbol(false, ParseTag::NormalChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CompleteCharSet), vector<Symbol>({Symbol(false, ParseTag::CharSet)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CompleteCharSet), vector<Symbol>({Symbol(false, ParseTag::CharSet), Symbol(false, ParseTag::Repeat)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CompleteFactor), vector<Symbol>({Symbol(false, ParseTag::Factor), Symbol(false, ParseTag::Repeat)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CompleteFactor), vector<Symbol>({Symbol(false, ParseTag::Factor)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Express), vector<Symbol>({Symbol(false, ParseTag::Term), Symbol(true, ParseTag::ChoseSymbol), Symbol(false, ParseTag::Express)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Express), vector<Symbol>({Symbol(false, ParseTag::Term)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Factor), vector<Symbol>({Symbol(true, ParseTag::SimpleUnNamedCatch_Start), Symbol(false, ParseTag::Express), Symbol(true, ParseTag::Mitipute_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Factor), vector<Symbol>({Symbol(false, ParseTag::CompleteCharSet)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Factor), vector<Symbol>({Symbol(false, ParseTag::NormalStringComplete)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalChar), vector<Symbol>({Symbol(true, ParseTag::NumberChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalChar), vector<Symbol>({Symbol(true, ParseTag::RealWordChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalChar), vector<Symbol>({Symbol(true, ParseTag::OtherChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalString), vector<Symbol>({Symbol(false, ParseTag::NormalChar), Symbol(false, ParseTag::NormalString)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalString), vector<Symbol>({Symbol(false, ParseTag::NormalChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalStringComplete), vector<Symbol>({Symbol(false, ParseTag::NormalString), Symbol(false, ParseTag::Repeat)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalStringComplete), vector<Symbol>({Symbol(false, ParseTag::NormalString)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::Repeat_Start), Symbol(false, ParseTag::RepeatRight), Symbol(false, ParseTag::RepeatEnd)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::Closures_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::Closures_UnGreedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::PositiveClosures_UnGreedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::PositiveClosures_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::ChoseClosures_UnGreedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::ChoseClosures_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatEnd), vector<Symbol>({Symbol(true, ParseTag::Repeat_And_BackRefer_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatEnd), vector<Symbol>({Symbol(true, ParseTag::Repeat_End_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatRight), vector<Symbol>({Symbol(false, ParseTag::SumNumber)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatRight), vector<Symbol>({Symbol(false, ParseTag::SumNumber), Symbol(true, ParseTag::Comma), Symbol(false, ParseTag::SumNumber)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::SumNumber), vector<Symbol>({Symbol(false, ParseTag::NumberChar), Symbol(false, ParseTag::SumNumber)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Term), vector<Symbol>({Symbol(false, ParseTag::CompleteFactor), Symbol(false, ParseTag::Term)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Term), vector<Symbol>({Symbol(false, ParseTag::CompleteFactor)})));
		//<initGrammarMap>
	}
	void initSemanticMap()
	{
		SemanticActionMap.insert(make_pair(2, bind(&RegexParse::Production2, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(3, bind(&RegexParse::Production3, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(4, bind(&RegexParse::Production4, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(5, bind(&RegexParse::Production5, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(6, bind(&RegexParse::Production6, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(7, bind(&RegexParse::Production7, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(8, bind(&RegexParse::Production8, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(9, bind(&RegexParse::Production9, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(10, bind(&RegexParse::Production10, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(11, bind(&RegexParse::Production11, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(12, bind(&RegexParse::Production12, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(13, bind(&RegexParse::Production13, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(14, bind(&RegexParse::Production14, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(15, bind(&RegexParse::Production15, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(16, bind(&RegexParse::Production16, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(17, bind(&RegexParse::Production17, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(18, bind(&RegexParse::Production18, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(19, bind(&RegexParse::Production19, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(20, bind(&RegexParse::Production20, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
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
		SemanticActionMap.insert(make_pair(32, bind(&RegexParse::Production32, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(33, bind(&RegexParse::Production33, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(34, bind(&RegexParse::Production34, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(35, bind(&RegexParse::Production35, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		SemanticActionMap.insert(make_pair(36, bind(&RegexParse::Production36, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));

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
	//������ıհ��������
	void CreatItemClourse(vector<Triple<int, int, set<ParseTag>>>& ItemList)
	{
		for(int i = 0; i < ItemList.size(); i++)
		{
			auto& CurrentItem = ItemList[i];
			if(CouldExpand(CurrentItem.first, CurrentItem.second))
			{
				//˵���Ƿ��ս����
				//�鿴�ڵ�ǰ����ʽ����,�÷��ս�����Ƿ��к�׺
				set<ParseTag> TermSymbolSet;
				if(IsProductEnd(CurrentItem.first, CurrentItem.second + 1))
				{
					//û�к�׺�Ͱ��ս���ż��Ϸ��ż̳й���
					TermSymbolSet = CurrentItem.TagMap;
				}
				else
				{
					//�к�׺�ͰѺ�׺���ż���
					TermSymbolSet = GetSymbolFirstSet(CurrentItem.first, CurrentItem.second + 1);
				}

				//��CouldExpand�����Ѿ����Ժ���,�϶��Ƿ��ս����
				auto GetTag = GetSymbolTag(CurrentItem.first, CurrentItem.second);
				auto FindIter = GrammarMap.equal_range(GetTag);

				for(auto Iter = FindIter.first; Iter != FindIter.second; Iter++)
				{
					//��ȡ���ǲ���ʽ�ı��
					//�鿴�ò���ʽ�Ƿ��Ѿ�����;
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
						//�鿴�ò���ʽ�Ƿ�ȱ�ٻ�δ����ĺ�׺

						set<ParseTag> ResultSet;
						set_union(ItemList[ResultIndex].TagMap.begin(), ItemList[ResultIndex].TagMap.end(), TermSymbolSet.begin(), TermSymbolSet.end(), inserter(ResultSet, ResultSet.begin()));
						ResultSet.swap(ItemList[ResultIndex].TagMap);
					}
				}
			}
		}
		//���������Ƿ���ڸ�������׺ܶ���;

		sort(ItemList.begin(), ItemList.end());
	}

	bool IsProductEnd(int Index, int Position)
	{
		return GrammarList[Index].BodySize == Position;
	}

	//��ȡSymbol��First����
	set<ParseTag> GetSymbolFirstSet(int Index, int Position)
	{
		set<ParseTag> Result;
		auto Sym = GetSymbol(Index, Position);
		if(Sym.IsTerminal == true)
		{
			//�ս����
			Result.insert(Sym.Tag);
		}
		else
		{
			//���ս����
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

	//�鿴�ñ�Ų���ʽ�Ƿ��Ѿ�����
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

	//�ж��ǲ���.�ź����Ƿ��ս����
	//Index ���ķ���� Position�ǵ�X��λ�õĵ�
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

	//����LR(0)�����

	void CreatLR1ItemSet()
	{
		//�����һ��Lr(0)�����
		LR1Stauts StartSet;
		//StartSet.Index = 0;
		//��ʼ����ʽ
		//��һ����ѹ���
		set<ParseTag>TagList;
		TagList.insert(ParseTag::StringTail);
		StartSet.ItemList.push_back(Triple<int, int, set<ParseTag>>({0, 0, TagList}));
		StartSet.CoreItermNumber = 1;
		//��һ����ıհ�����
		CreatItemClourse(StartSet.ItemList);

		//��һ���ѹջ
		LR1ItemSet.push_back(StartSet);
		for(int i = 0; i < LR1ItemSet.size(); i++)
		{
			LR1GOTO(LR1ItemSet, i);
		}
		AcceptIndex = GetAcceptIndex(LR1ItemSet);

		//auto Finish = CheckLR1GOTO();
		//auto chose = CheckIndex();
	}
	//��ȡ����״̬����
	int GetAcceptIndex(vector<LR1Stauts>& LR1ItemSet)
	{
		//��ȡ�������ʽ���ķ������б�
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

	//���LR0��Ƿ���ȷ �������ظ��
	bool CheckLR1GOTO()
	{
		auto Length = LR1ItemSet.size();
		LR1ItemSet.erase(unique(LR1ItemSet.begin(), LR1ItemSet.end()), LR1ItemSet.end());
		return LR1ItemSet.size() == Length;
	}

	void LR1GOTO(vector<LR1Stauts>& LR1ItemSet, int TargetIndex)
	{
		//������
		//��ŵ�ǰLR1Stauts�ĺ�һ�����ż���
		unordered_multimap<ParseTag, Triple<int, int, set<ParseTag>>> NextSet;
		auto& Target = LR1ItemSet[TargetIndex];
		for(auto& Iter : Target.ItemList)
		{
			//˵��û������ʽβ��
			if(Iter.second != ProductionSize(Iter.first))
			{
				NextSet.insert(make_pair(GetSymbolTag(Iter.first, Iter.second), Triple<int, int, set<ParseTag>>(Iter.first, Iter.second + 1, Iter.TagMap)));
			}
		}

		//����key����,ͬkey��ͬһ���
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
			//�ж��Ƿ���״̬���������;
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

	//�ж���������Ƿ���ڸ��,������ھͷ�������,�����ھͷ���-1
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

	//�Ƿ���ͬ�����
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

	//�鿴������Ƿ���ڵ������ʽβ���ĺ�����,���ڷ�������,�����ڷ���-1;
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
		//�����ǲ�������Զ�ջ
		//ֱ����Property����AST
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
				//�޷�����,��Ҫ��Լ����
				auto& CurrentItermList = LR1ItemSet[GetIndex].ItemList;
				auto ResultIndex = HasCurrentProductTailIterm(CurrentItermList, Tag);
				if(ResultIndex == -1)
				{
					//���ִ����ַ�
					cout << "Error! Index:" << TokenStream[i]->GetIndex() << " Data:" << TokenStream[i]->GetData() << endl;
					abort();
					//����Ӧ�öԴ�����д���
					// TO DO
				}
				else
				{
					//�ڵ���ǰִ�е�����Ƭ��

					//������ִ������Ƭ��
					auto FindAction = SemanticActionMap.find(CurrentItermList[ResultIndex].first);
					if(FindAction != SemanticActionMap.end())
					{
						FindAction->second(CatchStack, i, TokenStream);
					}
					//��Ҫ������״̬����
					auto PopNumber = GrammarList[CurrentItermList[ResultIndex].first].BodySize;

					auto ProductHead = GrammarList[CurrentItermList[ResultIndex].first].Head;
					StautsStack.erase(StautsStack.end() - PopNumber, StautsStack.end());

					auto& FindIter = LR1ItemSet[StautsStack.back()].NextStauts.find(ProductHead.Tag);
					if(ProductHead.Tag == ParseTag::Start)
					{
						cout << "��Լ�ɹ�!";
						return true;
					}
					else if(FindIter == LR1ItemSet[StautsStack.back()].NextStauts.end())
					{
						cout << "Error! pop������tag������״̬ʧ��";
						abort();
					}
					else
					{
						StautsStack.push_back(FindIter->second);
						CatchStack.erase(CatchStack.end() - PopNumber, CatchStack.end());
						CatchStack.push_back(shared_ptr<Property>(NewNode));
						GetIndex = FindIter->second;
					}
				}
				//���Թ�Լ
			}
			else
			{
				//ִ������.
				StautsStack.push_back(FindIter->second);
				CatchStack.push_back(shared_ptr<Property>(new Property(Val, Tag, SrcIndex)));
				i++;
				GetIndex = FindIter->second;
			}
		}
		return false;
	}
private:

	void OneLeftNodeSetup(ParseTag Tag, vector<shared_ptr<Property>>&CatchStack, string Value = "")
	{
		NewNode->Val = Value;
		NewNode->Tag = Tag;
		NewNode->LeftNode = CatchStack[CatchStack.size() - 1];
		NewNode->ChildNumber = 1;
	}
	void GetValue(ParseTag Tag, vector<shared_ptr<Property>>&CatchStack, string Value)
	{
		NewNode->Val = Value;
		NewNode->Tag = Tag;
		NewNode->ChildNumber = 0;
	}
	void TwoNodeSetup(ParseTag Tag, vector<shared_ptr<Property>>&CatchStack, string Value = "")
	{
		NewNode->Val = Value;
		NewNode->Tag = Tag;
		NewNode->LeftNode = CatchStack[CatchStack.size() - 2];
		NewNode->RightNode = CatchStack[CatchStack.size() - 1];
		NewNode->ChildNumber = 2;
	}
	//<UserDefineFunc>
private:

	void Production2(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Opps_Greedy = false;
		NewNode->Tag = ParseTag::CharSet;
		NewNode->LeftNode = CatchStack[CatchStack.size() - 1];
		NewNode->ChildNumber = 1;
	};
	void Production3(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Opps_Greedy = true;
		NewNode->Tag = ParseTag::CharSet;
		NewNode->LeftNode = CatchStack[CatchStack.size() - 1];
		NewNode->ChildNumber = 1;
	};
	void Production4(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		TwoNodeSetup(ParseTag::CharSetString, CatchStack);
	};
	void Production5(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		TwoNodeSetup(ParseTag::CharSetUnit, CatchStack);
	};
	void Production6(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		GetValue(ParseTag::CharSetUnit, CatchStack, CatchStack[CatchStack.size() - 1]->Val);
	};
	void Production7(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		OneLeftNodeSetup(ParseTag::CompleteCharSet, CatchStack);
	};
	void Production8(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		TwoNodeSetup(ParseTag::CompleteCharSet, CatchStack);
	};
	void Production9(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		TwoNodeSetup(ParseTag::CompleteFactor, CatchStack);
	};
	void Production10(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		OneLeftNodeSetup(ParseTag::CompleteFactor, CatchStack);
	};
	void Production11(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		TwoNodeSetup(ParseTag::ChoseSymbol, CatchStack);
	};
	void Production12(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		OneLeftNodeSetup(ParseTag::Express, CatchStack);
	};
	void Production13(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Val = "";
		NewNode->Tag = ParseTag::Factor;
		NewNode->LeftNode = CatchStack[CatchStack.size() - 2];
		NewNode->ChildNumber = 1;
	};
	void Production14(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		OneLeftNodeSetup(ParseTag::Factor, CatchStack);
	};
	void Production15(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		TwoNodeSetup(ParseTag::Factor, CatchStack);
	};
	void Production16(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		GetValue(ParseTag::NormalChar, CatchStack, CatchStack[CatchStack.size() - 1]->Val);
	};
	void Production17(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		GetValue(ParseTag::NormalChar, CatchStack, CatchStack[CatchStack.size() - 1]->Val);
	};
	void Production18(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		GetValue(ParseTag::NormalChar, CatchStack, CatchStack[CatchStack.size() - 1]->Val);
	};
	void Production19(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		TwoNodeSetup(ParseTag::NormalString, CatchStack);
	};
	void Production20(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		OneLeftNodeSetup(ParseTag::NormalString, CatchStack, CatchStack[CatchStack.size() - 1]->Val);
	};
	void Production21(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		TwoNodeSetup(ParseTag::NormalStringComplete, CatchStack);
	};
	void Production22(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		OneLeftNodeSetup(ParseTag::NormalStringComplete, CatchStack);
	};
	void Production23(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Opps_Greedy = CatchStack[CatchStack.size() - 1]->Opps_Greedy;
		NewNode->Tag = ParseTag::Repeat;
		NewNode->LeftNode = CatchStack[CatchStack.size() - 2];
		NewNode->ChildNumber = 1;
	};
	void Production24(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Opps_Greedy = true;
		NewNode->Tag = ParseTag::Repeat;
		NewNode->Val = "*";
		NewNode->ChildNumber = 0;
	};
	void Production25(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Opps_Greedy = false;
		NewNode->Tag = ParseTag::Repeat;
		NewNode->Val = "*";
		NewNode->ChildNumber = 0;
	};
	void Production26(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Opps_Greedy = false;
		NewNode->Tag = ParseTag::Repeat;
		NewNode->Val = "+";
		NewNode->ChildNumber = 0;
	};
	void Production27(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Opps_Greedy = true;
		NewNode->Tag = ParseTag::Repeat;
		NewNode->Val = "+";
		NewNode->ChildNumber = 0;
	};
	void Production28(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Opps_Greedy = false;
		NewNode->Tag = ParseTag::Repeat;
		NewNode->Val = "?";
		NewNode->ChildNumber = 0;
	};
	void Production29(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Opps_Greedy = true;
		NewNode->Tag = ParseTag::Repeat;
		NewNode->Val = "?";
		NewNode->ChildNumber = 0;
	};
	void Production30(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		GetValue(ParseTag::Repeat_And_BackRefer_End, CatchStack, "}");
	};
	void Production31(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		GetValue(ParseTag::Repeat_End_Greedy, CatchStack, "}?");
	};
	void Production32(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		OneLeftNodeSetup(ParseTag::RepeatRight, CatchStack);
	};
	void Production33(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Val = "";
		NewNode->Tag = ParseTag::RepeatRight;
		NewNode->LeftNode = CatchStack[CatchStack.size() - 3];
		NewNode->RightNode = CatchStack[CatchStack.size() - 1];
		NewNode->ChildNumber = 2;
	};
	void Production34(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();

		NewNode->Val = CatchStack[CatchStack.size() - 2]->Val + CatchStack[CatchStack.size() - 1]->Val;
		NewNode->Tag = ParseTag::SumNumber;
		NewNode->ChildNumber = 0;
	};
	void Production35(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		TwoNodeSetup(ParseTag::Term, CatchStack);
	};
	void Production36(vector<shared_ptr<Property>>& CatchStack, int StreamIndex, vector<shared_ptr<RegexToken>>& TokenStream)
	{
		NewNode = shared_ptr<Property>(new Property())._Get();
		OneLeftNodeSetup(ParseTag::Term, CatchStack);
	};

	//<SemanticFunc>

private:
	//�ķ� 0���ǲ���ʽͷ
	vector<Production> GrammarList;

	//LR(0)�
	vector<LR1Stauts> LR1ItemSet;

	//����LR(1)�GOTO��
	//�ķ�ͷ������ʽ���ӳ���
	unordered_multimap<ParseTag, int> GrammarMap;

	//����״̬�ı��.�ڸ�״̬�ϵ����ı���β�����߽���StringTail �ַ��ͱ�ʾ����
	int AcceptIndex;

	//LexTag ��ParseTag��ӳ��.
	unordered_map<LexTag, ParseTag> TagMap;

	//�ķ��б�����������Ƭ�ε�����
	unordered_map<int, function<void(vector<shared_ptr<Property>>&, int, vector<shared_ptr<RegexToken>>&)>> SemanticActionMap;
	//�������Ե�״̬ջ������ջ
	vector<int> StautsStack;
	vector<shared_ptr<Property>>CatchStack;
	//ÿ��ִ�����嶯��ʱ������½ڵ����λ��
	Property* NewNode;

	//<DataMember>
};
