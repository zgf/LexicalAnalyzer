#include "RegexParse.h"

int main()
{
	//[a-znb]*|ab(c[a\\b]|cb+?dd)
	string Pattern("[a-znb]*|ab(c[a\\b]|cb+?dd)");
	LexParse Lex(Pattern);
	RegexParse Parse;
	Parse.ParsingRegex(Lex.TokenStream);
	return 0;
}