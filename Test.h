#pragma once
#include "RegexLexParse.h"
//�﷨��������
//����
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
//���ս����
class Nonterminal :public Symbol
{
public:
	string Name;
	//���ս���Ŷ�Ӧ�Ļص�����
public:
	Nonterminal(bool Chose, string NameStr) : Symbol(Chose), Name(NameStr)
	{
	}
};
//�ս����
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
	map<Symbol, int> NextStauts;
};
//����ʽ
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
		GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Start")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"Factor"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"CharSet")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::CharSet_Start )),shared_ptr<Symbol>(new Nonterminal(false,"CharSetString")),shared_ptr<Symbol>(new Termination(true,LexTag::CharSet_End ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"CharSet")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::CharSet_Back_Start )),shared_ptr<Symbol>(new Nonterminal(false,"CharSetString")),shared_ptr<Symbol>(new Termination(true,LexTag::CharSet_End ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"CharSetString")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"CharSetString")),shared_ptr<Symbol>(new Termination(true,LexTag::CharSetComponent )),shared_ptr<Symbol>(new Nonterminal(false,"NormalString"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"CharSetString")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"CharSetString")),shared_ptr<Symbol>(new Nonterminal(false,"NormalString"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"CharSetString")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"NormalString"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"CompleteCharSet")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"CharSet"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"CompleteCharSet")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"CharSet")),shared_ptr<Symbol>(new Nonterminal(false,"Repeat"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"CompleteFactor")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"Factor")),shared_ptr<Symbol>(new Nonterminal(false,"Repeat"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"CompleteFactor")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"Factor"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Express")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"Express")),shared_ptr<Symbol>(new Termination(true,LexTag::ChoseSymbol )),shared_ptr<Symbol>(new Nonterminal(false,"Term"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Express")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"Term"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Factor")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::SimpleUnNamedCatch_Start )),shared_ptr<Symbol>(new Nonterminal(false,"Express")),shared_ptr<Symbol>(new Termination(true,LexTag::Mitipute_End ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Factor")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"CompleteCharSet"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Factor")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"NormalStringComplete"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"NormalChar")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::NumberChar ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"NormalChar")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::RealWordChar ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"NormalChar")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::OtherChar  ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"NormalString")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"NormalString")),shared_ptr<Symbol>(new Nonterminal(false,"NormalChar"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"NormalString")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"NormalChar"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"NormalStringComplete")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"NormalString")),shared_ptr<Symbol>(new Nonterminal(false,"Repeat"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"NormalStringComplete")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"NormalString"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Repeat")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::Repeat_Start )),shared_ptr<Symbol>(new Nonterminal(false,"RepeatRight")),shared_ptr<Symbol>(new Nonterminal(false,"RepeatEnd"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Repeat")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::Closures_UnGreedy ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Repeat")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::Closures_Greedy ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Repeat")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::PositiveClosures_Greedy ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Repeat")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::PositiveClosures_UnGreedy ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Repeat")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::ChoseClosures_Greedy ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Repeat")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::ChoseClosures_UnGreedy ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"RepeatEnd")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::Repeat_And_BackRefer_End ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"RepeatEnd")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::Repeat_End_Greedy ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"RepeatRight")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Termination(true,LexTag::Comma )),shared_ptr<Symbol>(new Nonterminal(false,"SumNumber"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"RepeatRight")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"SumNumber")),shared_ptr<Symbol>(new Termination(true,LexTag::Comma ))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"RepeatRight")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"SumNumber"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"RepeatRight")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"SumNumber")),shared_ptr<Symbol>(new Termination(true,LexTag::Comma )),shared_ptr<Symbol>(new Nonterminal(false,"SumNumber"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"SumNumber")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"SumNumber")),shared_ptr<Symbol>(new Nonterminal(false,"Number"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Term")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"Term")),shared_ptr<Symbol>(new Nonterminal(false,"CompleteFactor"))})));
GrammarList.push_back(Production(  shared_ptr<Nonterminal>( new Nonterminal(false,"Term")),vector<shared_ptr<Symbol>>({shared_ptr<Symbol>(new Nonterminal(false,"CompleteFactor"))})));

	}

	void initGrammarMap()
	{
		for(int i = 0; i < GrammarList.size(); i++)
		{
			GrammarMap.insert(make_pair(GrammarList[i].Head->Name, i));
		}
	}
	//����LR(0)�����
	void CreatLR0ItemSet()
	{
		//�����һ��Lr(0)�����
		Stauts StartSet;
		StartSet.Index = 0;
		//��ʼ����ʽ
		StartSet.ItemList.push_back(Triple<int, int, shared_ptr<Symbol>>({0, 0}));
		//��һ���ѹջ
		CreatItemClourse(StartSet.ItemList);
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
				auto FindIter = GrammarMap.equal_range(GetNonTermName(CurrentItem.First, CurrentItem.Second));
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
	//��ȡ���ս���ŵ�����,������ս����,�ҵ�
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
	//�ķ� 0���ǲ���ʽͷ
	vector<Production> GrammarList;

	//LR(0)�
	vector<Stauts> LRItemSet;

	//�ķ�ͷ������ʽ���ӳ���
	unordered_multimap<string, int> GrammarMap;
};