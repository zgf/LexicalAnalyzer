#include "RegexParseCodeGen.h"
#include "Test.h"

int main()
{
	//RegexParseCodeGen ParseCodeGen(string("RegexParseConfig.cpp"), string("RegexParseTemplate.h"));
	//ParseCodeGen.DealAll();
	string Pattern("a(b|c)c*|b");
	LexParse Lex(Pattern);
	RegexParse Parse;

	Parse.ParsingRegex(Lex.TokenStream);
	return 0;
}