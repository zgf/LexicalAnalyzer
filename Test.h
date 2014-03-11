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
//����
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
////���ս����
//class Symbol :public Symbol
//{
//public:
//	string Name;
//	//���ս���Ŷ�Ӧ�Ļص�����
//public:
//	Symbol(bool Chose, string& NameStr) : Symbol(Chose), Name(NameStr)
//	{
//	}
//
//};
////�ս����
//class Symbol : public Symbol
//{
//public:
//	LexTag TermTag;
//public:
//	Symbol(bool Chose) :Symbol(Chose)
//	{
//	}
//	Symbol(bool Chose, LexTag Tag) : Symbol(Chose), TermTag(Tag)
//	{
//	}
//};

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
//״̬��
class Stauts
{
public:
	//z״̬�����
	int Index;
	vector<Triple<int, int, shared_ptr<Symbol>>> ItemList;
	map<shared_ptr<Symbol>, int> NextStauts;
};
//����ʽ
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
		/*	GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Start")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "Factor"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "CharSet")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::CharSet_Start)), shared_ptr<Symbol>(new Symbol(false, "CharSetString")), shared_ptr<Symbol>(new Symbol(true, LexTag::CharSet_End))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "CharSet")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::CharSet_Back_Start)), shared_ptr<Symbol>(new Symbol(false, "CharSetString")), shared_ptr<Symbol>(new Symbol(true, LexTag::CharSet_End))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "CharSetString")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "CharSetString")), shared_ptr<Symbol>(new Symbol(true, LexTag::CharSetComponent)), shared_ptr<Symbol>(new Symbol(false, "NormalString"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "CharSetString")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "CharSetString")), shared_ptr<Symbol>(new Symbol(false, "NormalString"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "CharSetString")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "NormalString"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "CompleteCharSet")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "CharSet"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "CompleteCharSet")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "CharSet")), shared_ptr<Symbol>(new Symbol(false, "Repeat"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "CompleteFactor")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "Factor")), shared_ptr<Symbol>(new Symbol(false, "Repeat"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "CompleteFactor")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "Factor"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Express")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "Express")), shared_ptr<Symbol>(new Symbol(true, LexTag::ChoseSymbol)), shared_ptr<Symbol>(new Symbol(false, "Term"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Express")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "Term"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Factor")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::SimpleUnNamedCatch_Start)), shared_ptr<Symbol>(new Symbol(false, "Express")), shared_ptr<Symbol>(new Symbol(true, LexTag::Mitipute_End))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Factor")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "CompleteCharSet"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Factor")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "NormalStringComplete"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "NormalChar")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::NumberChar))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "NormalChar")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::RealWordChar))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "NormalChar")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::OtherChar))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "NormalString")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "NormalString")), shared_ptr<Symbol>(new Symbol(false, "NormalChar"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "NormalString")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "NormalChar"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "NormalStringComplete")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "NormalString")), shared_ptr<Symbol>(new Symbol(false, "Repeat"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "NormalStringComplete")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "NormalString"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Repeat")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::Repeat_Start)), shared_ptr<Symbol>(new Symbol(false, "RepeatRight")), shared_ptr<Symbol>(new Symbol(false, "RepeatEnd"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Repeat")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::Closures_UnGreedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Repeat")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::Closures_Greedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Repeat")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::PositiveClosures_Greedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Repeat")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::PositiveClosures_UnGreedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Repeat")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::ChoseClosures_Greedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Repeat")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::ChoseClosures_UnGreedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "RepeatEnd")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::Repeat_And_BackRefer_End))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "RepeatEnd")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::Repeat_End_Greedy))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "RepeatRight")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(true, LexTag::Comma)), shared_ptr<Symbol>(new Symbol(false, "SumNumber"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "RepeatRight")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "SumNumber")), shared_ptr<Symbol>(new Symbol(true, LexTag::Comma))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "RepeatRight")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "SumNumber"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "RepeatRight")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "SumNumber")), shared_ptr<Symbol>(new Symbol(true, LexTag::Comma)), shared_ptr<Symbol>(new Symbol(false, "SumNumber"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "SumNumber")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "SumNumber")), shared_ptr<Symbol>(new Symbol(false, "Number"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Term")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "Term")), shared_ptr<Symbol>(new Symbol(false, "CompleteFactor"))})));
		GrammarList.push_back(Production(shared_ptr<Symbol>(new Symbol(false, "Term")), vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Symbol(false, "CompleteFactor"))})));*/
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
	//����LR(0)�����
	void CreatLR0ItemSet()
	{
		//�����һ��Lr(0)�����
		Stauts StartSet;
		StartSet.Index = 0;
		//��ʼ����ʽ
		//��һ����ѹ���
		StartSet.ItemList.push_back(Triple<int, int, shared_ptr<Symbol>>({0, 0}));
		//��һ����ıհ�����
		CreatItemClourse(StartSet.ItemList);

		//��һ���ѹջ
		LRItemSet.push_back(StartSet);
	}

	//������ıհ��������
	void CreatItemClourse(vector<Triple<int, int, shared_ptr<Symbol>>>& ItemList)
	{
		for(int i = 0; i < ItemList.size(); i++)
		{
			auto& CurrentItem = ItemList[i];
			if(CouldExpand(CurrentItem.First, CurrentItem.Second))
			{
				//˵���Ƿ��ս����
				auto FindIter = GrammarMap.equal_range(GetSymbolTag(CurrentItem.First, CurrentItem.Second));
				if(FindIter.first == FindIter.second)
				{
					//���ս�����Ҳ���������ô����- -;
					abort();
				}
				else
				{
					for(auto Iter = FindIter.first; Iter != FindIter.second; Iter++)
					{
						//��ȡ���ǲ���ʽ�ı��
						//�鿴�ò���ʽ�Ƿ��Ѿ�����;
						if(!HasAddThisProduct(ItemList, Iter->second))
						{
							ItemList.push_back(Triple<int, int, shared_ptr<Symbol>>(Iter->second, 0));
						}
					}
				}
			}
		}
	}

	//�鿴�ñ�Ų���ʽ�Ƿ��Ѿ�����
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

	//�ж��ǲ���.�ź����Ƿ��ս����
	//Index ���ķ���� Position�ǵ�X��λ�õĵ�
	bool CouldExpand(int Index, int Position)
	{
		return GrammarList[Index].Body[Position]->IsTerminal == false;
	}
	ParseTag GetSymbolTag(int Index, int Position)
	{
		return GrammarList[Index].Body[Position]->Tag;
	}
	//��ȡ���ս���ŵ�����,������ս����,�ҵ�
	/*string GetNonTermName(int Index, int Position)
	{
	if(CouldExpand(Index, Position))
	{
	return std::move(static_cast<Symbol*>( GrammarList[Index].Body[Position].get() )->Name);
	}
	else
	{
	abort();
	return string();
	}*/
private:
	//�ķ� 0���ǲ���ʽͷ
	vector<Production> GrammarList;

	//LR(0)�
	vector<Stauts> LRItemSet;

	//�ķ�ͷ������ʽ���ӳ���
	unordered_multimap<ParseTag, int> GrammarMap;
};