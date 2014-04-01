#include "RegexParse.h"

int main()
{
	string Pattern("[a-znb]*|ab(c[a]|cb+?dd)");
	LexParse Lex(Pattern);
	RegexParse Parse;
	Parse.ParsingRegex(Lex.TokenStream);
	return 0;
}