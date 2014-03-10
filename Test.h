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
	Triple(X a, Y b, Z c) : Triple(a, b), Third(c)
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
	vector<Triple<int, int, Termination>> CoreItemList;
	map<Symbol, int> NextStauts;

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
	//����LR(0)�����
	void CreatLR0ItemSet()
	{
		Stauts StartSet;
		StartSet.Index = 0;
		//��ʼ����ʽ
		StartSet.CoreItemList.push_back(Triple<int, int, Termination>({0, 0}));


	}
	//������ıհ��������
	void CreatItemClourse(vector<Triple<int,int,Termination>>& ItemList)
	{
		
	}
private:
	//�ķ�ӳ��� 0���ǲ���ʽͷ
	vector<Production> GrammarMap;

	//LR(0)�
	vector<Stauts> LRItemSet;


};
