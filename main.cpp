#include "RegexDFA.h"

int main()
{
	//[a-znb]*|ab(c[a\\b]|cb+?dd)
	string Pattern("[a-znb]*");
	LexParse Lex(Pattern);
	RegexParse Parse;
	Parse.ParsingRegex(Lex.TokenStream);
	DFA DfaMachine(Parse.GetCharMap(), Parse.GetAst(), Parse.GetAstRootIndex(), Parse.GetAstNodeList());
	//Parse.DeleteAst();
	return 0;
}