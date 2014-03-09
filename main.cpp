#include "RegexLexParse.h"
#include "RegexParseCodeGen.h"
int main()
{
	RegexParseCodeGen ParseCodeGen(string("RegexParseConfig"));
	ParseCodeGen.DealAll();
	return 0;
}