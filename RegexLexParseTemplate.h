#include "RegexLexParse.h"
//�﷨��������
//����
using std::unordered_multimap;
class Symbol
{
public:
	bool IsTerminal;
	//���ڵ���
	string RawName;
};
//���ս����
class Nonterminal
{
public:
	string Name;
	//���ս���Ŷ�Ӧ�Ļص�����
	function<void()> CallBack;
};
//�ս����
class Termination
{
public:
	LexTag TermTag;
	//����
	string Morpheme;
};

//״̬��
class Stauts
{
	int Item;
	int Position;

};

class RegexParse
{
private:
	//�ķ�ӳ��� 0���ǲ���ʽͷ
	vector<vector<Symbol>> GrammarMap;


};
