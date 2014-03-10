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
};
//״̬��
class Stauts
{
	/*int Item;
	int Position;*/
	//z״̬�����
	int Index;
	vector<Triple<int, int, Termination>> ItemList;
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
	//�ķ�ӳ��� 0���ǲ���ʽͷ
	vector<Production> GrammarMap;

private:
	void initGrammarMap()
	{
		//GrammarMap = {vector<Symbol>({}), vector<Symbol>({})};
		//</initGrammarMap>
	}
};
