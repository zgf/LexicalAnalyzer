#include "RegexDFA.h"

int main()
{
	//[a-znb]*|ab(c[a\\b]|cb+?dd)
	string Pattern("a{1,2}");
	LexParse Lex(Pattern);
	RegexParse Parse;
	Parse.ParsingRegex(Lex.TokenStream);
	DFA DfaMachine(Parse.GetCharMap(), Parse.GetAst(), Parse.GetAstRootIndex(), Parse.GetAstNodeList());
	string Text("a");
	cout << DfaMachine.RunDfa(Text);
	cout << endl;
	cout << endl;
	cout << endl;
	string _Pattern("ba{0,2}");
	LexParse _Lex(_Pattern);
	RegexParse _Parse;
	_Parse.ParsingRegex(_Lex.TokenStream);
	DFA _DfaMachine(_Parse.GetCharMap(), _Parse.GetAst(), _Parse.GetAstRootIndex(), _Parse.GetAstNodeList());
	string _Text("b");
	cout << _DfaMachine.RunDfa(_Text);
	return 0;
}