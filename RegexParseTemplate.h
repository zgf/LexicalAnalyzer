#pragma once
#include "RegexLexParse.h"
using std::unordered_multimap;
// �ķ�����enum��
enum class ParseTag
{
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

template<typename X, typename Y>
class Pair
{
public:
	//�ڼ����ķ�
	X first;
	//��X���ķ��ĵ�Y��
	Y second;
	Pair(X a, Y b) : first(a), second(b)
	{
	}
	bool operator!=( Pair& a )
	{
		return this->first != a.first || this->second != a.second;
	}
	friend bool operator==( Pair& a, Pair& b )
	{
		return a.first == b.first && a.second == b.second;
	}
	friend bool operator<( Pair& a, Pair& b )
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

template<typename X, typename Y, typename Z>
class Triple
{
public:
	//�ڼ����ķ�
	X First;
	//��X���ķ��ĵ�Y��
	Y Second;
	//��ǰ�����ַ�
	Z Third;
	Triple(X a, Y b) :First(a), Second(b)
	{
	}
	Triple(X a, Y b, Z c) :First(a), Second(b), Third(c)
	{
	}
	Triple(Z c) :Third(c)
	{
	}
};

//״̬��
class Stauts
{
public:
	//z״̬�����
	int Index;
	vector<Pair<int, int>> ItemList;
	map<Symbol, int> NextStauts;
	friend bool operator== ( Stauts& Left, Stauts&Right )
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
	friend	bool operator<( Stauts&Left, Stauts& Right )
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
		GrammarList.push_back(Production(Symbol(false, ParseTag::Start), vector<Symbol>({Symbol(false, ParseTag::Factor)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSet), vector<Symbol>({Symbol(true, ParseTag::CharSet_Start), Symbol(false, ParseTag::CharSetString), Symbol(true, ParseTag::CharSet_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSet), vector<Symbol>({Symbol(true, ParseTag::CharSet_Back_Start), Symbol(false, ParseTag::CharSetString), Symbol(true, ParseTag::CharSet_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSetString), vector<Symbol>({Symbol(false, ParseTag::CharSetString), Symbol(true, ParseTag::CharSetComponent), Symbol(false, ParseTag::NormalString)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSetString), vector<Symbol>({Symbol(false, ParseTag::CharSetString), Symbol(false, ParseTag::NormalString)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CharSetString), vector<Symbol>({Symbol(false, ParseTag::NormalString)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CompleteCharSet), vector<Symbol>({Symbol(false, ParseTag::CharSet)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CompleteCharSet), vector<Symbol>({Symbol(false, ParseTag::CharSet), Symbol(false, ParseTag::Repeat)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CompleteFactor), vector<Symbol>({Symbol(false, ParseTag::Factor), Symbol(false, ParseTag::Repeat)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::CompleteFactor), vector<Symbol>({Symbol(false, ParseTag::Factor)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Express), vector<Symbol>({Symbol(false, ParseTag::Express), Symbol(true, ParseTag::ChoseSymbol), Symbol(false, ParseTag::Term)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Express), vector<Symbol>({Symbol(false, ParseTag::Term)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Factor), vector<Symbol>({Symbol(true, ParseTag::SimpleUnNamedCatch_Start), Symbol(false, ParseTag::Express), Symbol(true, ParseTag::Mitipute_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Factor), vector<Symbol>({Symbol(false, ParseTag::CompleteCharSet)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Factor), vector<Symbol>({Symbol(false, ParseTag::NormalStringComplete)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalChar), vector<Symbol>({Symbol(true, ParseTag::NumberChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalChar), vector<Symbol>({Symbol(true, ParseTag::RealWordChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalChar), vector<Symbol>({Symbol(true, ParseTag::OtherChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalString), vector<Symbol>({Symbol(false, ParseTag::NormalString), Symbol(false, ParseTag::NormalChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalString), vector<Symbol>({Symbol(false, ParseTag::NormalChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalStringComplete), vector<Symbol>({Symbol(false, ParseTag::NormalString), Symbol(false, ParseTag::Repeat)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::NormalStringComplete), vector<Symbol>({Symbol(false, ParseTag::NormalString)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::Repeat_Start), Symbol(false, ParseTag::RepeatRight), Symbol(false, ParseTag::RepeatEnd)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::Closures_UnGreedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::Closures_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::PositiveClosures_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::PositiveClosures_UnGreedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::ChoseClosures_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Repeat), vector<Symbol>({Symbol(true, ParseTag::ChoseClosures_UnGreedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatEnd), vector<Symbol>({Symbol(true, ParseTag::Repeat_And_BackRefer_End)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatEnd), vector<Symbol>({Symbol(true, ParseTag::Repeat_End_Greedy)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatRight), vector<Symbol>({Symbol(true, ParseTag::Comma), Symbol(false, ParseTag::SumNumber)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatRight), vector<Symbol>({Symbol(false, ParseTag::SumNumber), Symbol(true, ParseTag::Comma)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatRight), vector<Symbol>({Symbol(false, ParseTag::SumNumber)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::RepeatRight), vector<Symbol>({Symbol(false, ParseTag::SumNumber), Symbol(true, ParseTag::Comma), Symbol(false, ParseTag::SumNumber)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::SumNumber), vector<Symbol>({Symbol(false, ParseTag::SumNumber), Symbol(false, ParseTag::NumberChar)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Term), vector<Symbol>({Symbol(false, ParseTag::Term), Symbol(false, ParseTag::CompleteFactor)})));
		GrammarList.push_back(Production(Symbol(false, ParseTag::Term), vector<Symbol>({Symbol(false, ParseTag::CompleteFactor)})));
	}

	void initGrammarMap()
	{
		for(int i = 0; i < GrammarList.size(); i++)
		{
			GrammarMap.insert(make_pair(GrammarList[i].Head.Tag, i));
		}
	}

	//������ıհ��������
	void CreatItemClourse(vector<Pair<int, int>>& ItemList)
	{
		for(int i = 0; i < ItemList.size(); i++)
		{
			auto& CurrentItem = ItemList[i];
			if(CouldExpand(CurrentItem.first, CurrentItem.second))
			{
				//˵���Ƿ��ս����
				auto FindIter = GrammarMap.equal_range(GetSymbolTag(CurrentItem.first, CurrentItem.second));
				for(auto Iter = FindIter.first; Iter != FindIter.second; Iter++)
				{
					//��ȡ���ǲ���ʽ�ı��
					//�鿴�ò���ʽ�Ƿ��Ѿ�����;
					if(!HasAddThisProduct(ItemList, Iter->second))
					{
						ItemList.push_back(Pair<int, int>(Iter->second, 0));
					}
				}
			}
		}
		//���������Ƿ���ڸ�������׺ܶ���;

		sort(ItemList.begin(), ItemList.end());
	}

	//�鿴�ñ�Ų���ʽ�Ƿ��Ѿ�����
	bool HasAddThisProduct(vector<Pair<int, int>>& ItemList, int Number)
	{
		for(auto i = 0; i < ItemList.size(); i++)
		{
			if(ItemList[i].first == Number)
			{
				return true;
			}
		}
		return false;
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

	void CreatLR0ItemSet()
	{
		//�����һ��Lr(0)�����
		Stauts StartSet;
		StartSet.Index = 0;
		//��ʼ����ʽ
		//��һ����ѹ���
		StartSet.ItemList.push_back(Pair<int, int>({0, 0}));
		//��һ����ıհ�����
		CreatItemClourse(StartSet.ItemList);

		//��һ���ѹջ
		LR0ItemSet.push_back(StartSet);
		for(int i = 0; i < LR0ItemSet.size(); i++)
		{
			LR0GOTO(LR0ItemSet[i]);
		}
		auto Finish = CheckLR0GOTO();
	}
	//���LR0��Ƿ���ȷ �������ظ��
	bool CheckLR0GOTO()
	{
		auto Length = LR0ItemSet.size();
		LR0ItemSet.erase(unique(LR0ItemSet.begin(), LR0ItemSet.end()), LR0ItemSet.end());
		return LR0ItemSet.size() == Length;
	}

	void LR0GOTO(Stauts& Target)
	{
		//������
		//��ŵ�ǰStauts�ĺ�һ�����ż���

		unordered_multimap<ParseTag, Pair<int, int>> XSet;
		for(auto& Iter : Target.ItemList)
		{
			//˵��û������ʽβ��
			if(Iter.second != ProductionSize(Iter.first))
			{
				XSet.insert(make_pair(GetSymbolTag(Iter.first, Iter.second), Pair<int, int>(Iter.first, Iter.second + 1)));
			}
		}
		//����key����,ͬkey��ͬһ���
		for(auto KeyIter = XSet.begin(); KeyIter != XSet.end(); KeyIter = XSet.upper_bound(KeyIter->first))
		{
			Stauts CurrentStauts;
			auto KeyCount = XSet.count(KeyIter->first);
			auto i = 0;
			for(auto MinIter = KeyIter; i < KeyCount; MinIter++, i++)
			{
				CurrentStauts.ItemList.push_back(MinIter->second);
			}
			CreatItemClourse(CurrentStauts.ItemList);
			//�ж��Ƿ���״̬���������;
			auto result = HasThisItemSet(CurrentStauts);
			if(result == -1)
			{
				CurrentStauts.Index = LR0ItemSet.size();
				LR0ItemSet.push_back(CurrentStauts);
			}
		}
	}

	//�ж���������Ƿ���ڸ��,������ھͷ�������,�����ھͷ���-1
	int HasThisItemSet(Stauts& Target)
	{
		for(int i = 0; i < LR0ItemSet.size(); i++)
		{
			if(HasSameItemList(Target, LR0ItemSet[i]))
			{
				return i;
			}
		}
		return -1;
	}

	//�Ƿ���ͬ�����
	bool HasSameItemList(Stauts& Left, Stauts& Right)
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
			return true;
		}
		else
		{
			return false;
		}
	}
	Symbol GetSymbol(int Index, int Position)
	{
		return GrammarList[Index].Body[Position];
	}
private:
	//�ķ� 0���ǲ���ʽͷ
	vector<Production> GrammarList;

	//LR(0)�
	vector<Stauts> LR0ItemSet;

	//�ķ�ͷ������ʽ���ӳ���
	unordered_multimap<ParseTag, int> GrammarMap;
};
