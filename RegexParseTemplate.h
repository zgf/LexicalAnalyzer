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
			GrammarList[i].Head;
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
	}
	//������ıհ��������
	void CreatItemClourse(vector<Triple<int, int, Termination>>& ItemList, vector<Production>& Grammar)
	{
		for(int i = 0; i < ItemList.size(); i++)
		{
			auto& CurrentItem = ItemList[i];
			if(CouldExpand(CurrentItem.First, CurrentItem.Second, GrammarList))
			{
				//˵���Ƿ��ս����
			}
		}
	}
	//�ж��ǲ���.�ź����Ƿ��ս����
	//Index ���ķ���� Position�ǵ�X��λ�õĵ�
	bool CouldExpand(int Index, int Position, vector<Production>& Grammar)
	{
		return Grammar[Index].Body[Position]->IsTerminal == false;
	}

private:
	//�ķ� 0���ǲ���ʽͷ
	vector<Production> GrammarList;

	//LR(0)�
	vector<Stauts> LRItemSet;

	//�ķ�ͷ������ʽ���ӳ���
	//unordered_multimap<Nonterminal, int> GrammarMap;
};
