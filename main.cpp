#include "RegexDFA.h"
#include <ctime>
void TestRegex(const char* tPattern,const char* tText)
{
	string Pattern(tPattern);
	LexParse Lex(Pattern);
	RegexParse Parse;
	Parse.ParsingRegex(Lex.TokenStream);
	DFA DfaMachine(Parse.GetCharMap(), Parse.GetAst(), Parse.GetAstRootIndex(), Parse.GetAstNodeList(),Parse.GetCharEndIndex());
	string Text(tText);
	if(DfaMachine.RunDfa(Text) == true)
	{
		cout << Pattern << "测试通过" << endl;
	}
	else
	{
		cout << Pattern << "测试不通过" << endl;
	}
}

int main()
{
	//[a-znb]*|ab(c[a\\b]|cb+?dd)
	// 	TestRegex("[a-znb]*|ab(c[a\\b]|cb+?dd)", "dsnbs");
	//
	auto start = clock();
	TestRegex("[a-znb]*|ab(c[a\\b]|cb+dd)", "dsnbs");
	TestRegex("[a-znb]*|1", "1");

	TestRegex("[a-znb]*|ab(c[a\\b]|cb+dd)", "abca");
	
	TestRegex("a*", "aaaa");
	TestRegex("ba*", "b");
	TestRegex("a+", "a");
	TestRegex("a+", "aaaa");
	
	TestRegex("[^a-z]", "b");
	TestRegex("[a-zAB]{4,5}|ab(c[a\\b]|cb+dd)", "abcbbdd");
	TestRegex("a?", "aa");
	TestRegex("ba?", "b");
	TestRegex("b|a", "b");
	TestRegex("[a-zA]", "A");
	TestRegex("[a-ze1-4]", "e");
	TestRegex("[a-z1-4]", "3");
	TestRegex("[a-z1-4]", "1");
	TestRegex("[a-ze1-4]+", "ew421dq3");
	TestRegex("[a-zA-Z]", "V");
	TestRegex("[a-zA]", "A");
	TestRegex("[ABVD]", "D");
	TestRegex("[a-z]", "b");
	TestRegex("[ABVD]", "D");
	TestRegex("(c|b)|a", "ba");
	TestRegex("(c|b)|a", "ca");
	TestRegex("[^a-zA]", "C");
	auto end = clock();
	cout << (double)start - end;
	int a = 0;
	cin >> a;
	return 0;
}