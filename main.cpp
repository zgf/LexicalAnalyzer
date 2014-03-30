#include "RegexParseCodeGen.h"
//#include "RegexParseTemplate.h"

int main()
{
	RegexParseCodeGen ParseCodeGen(string("RegexParseConfig.txt"), string("RegexParseTemplate.h"));
	ParseCodeGen.DealAll();
	//wstring Pattern(L"a(b|c)c*|b");
	//LexParse Lex(Pattern);
	//RegexParse Parse;

	//Parse.ParsingRegex(Lex.TokenStream);
	return 0;
}