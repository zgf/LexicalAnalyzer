#include "RegexDFA.h"
#include <ctime>

void TestRegex(const char* tPattern, const char* tText, bool Chose)
{
	string Pattern(tPattern);
	LexParse Lex(Pattern);
	RegexParse Parse;
	Parse.ParsingRegex(Lex.TokenStream);
	DFA DfaMachine(Parse.GetCharMap(), Parse.GetAst(), Parse.GetAstRootIndex(), Parse.GetAstNodeList(), Parse.GetCharEndIndex());
	string Text(tText);
	if(DfaMachine.RunDfa(Text) == Chose)
	{
		cout << Pattern << "测试通过" << endl;
	}
	else
	{
		cout << Pattern << "测试不通过" << endl;
	}
}
void TestRegexTrue(const char* tPattern, const char* tText)
{
	TestRegex(tPattern, tText, true);
}
void TestRegexFalse(const char* tPattern, const char* tText)
{
	TestRegex(tPattern, tText, false);
}
void GetMatchContent(const char* tPattern, const char* tText)
{
	string Pattern(tPattern);
	LexParse Lex(Pattern);
	RegexParse Parse;
	Parse.ParsingRegex(Lex.TokenStream);
	DFA DfaMachine(Parse.GetCharMap(), Parse.GetAst(), Parse.GetAstRootIndex(), Parse.GetAstNodeList(), Parse.GetCharEndIndex());
	string Text(tText);
	auto Info = DfaMachine.Match(Text);
	cout << Info.MatchContent<<endl;
	cout << Info.StartIndex<<" "<<Info.EndIndex<<endl;
}
int main()
{
	//[a-znb]*|ab(c[a\\b]|cb+?dd)
	// 	TestRegex("[a-znb]*|ab(c[a\\b]|cb+?dd)", "dsnbs");
	//
	auto start = clock();
	//TestRegexTrue("b|a", "b");
	//TestRegexTrue("[a-znb]*|ab(c[a\\b]|cb+dd)", "dsnbs");
	//TestRegexTrue("[a-znb]*|1", "1");

	//GetMatchContent("[a-znb]*", "ewqabcaerwrw");
	GetMatchContent("ab(c[a\\b]|cb+dd)", "ewqabcaerwrw");
	/*
	TestRegexTrue("a*", "aaaa");
	TestRegexTrue("ba*", "b");
	TestRegexTrue("a+", "a");
	TestRegexTrue("a+", "aaaa");
	
	TestRegexFalse("[^a-z]", "b");
	TestRegexTrue("[a-zAB]{4,5}|ab(c[a\\b]|cb+dd)", "abcbbdd");
	TestRegexTrue("a?", "aa");
	TestRegexTrue("ba?", "b");
	TestRegexTrue("b|a", "b");
	TestRegexTrue("[a-zA]", "A");
	TestRegexTrue("[a-ze1-4]", "e");
	TestRegexTrue("[a-z1-4]", "3");
	TestRegexTrue("[a-z1-4]", "1");
	TestRegexTrue("[a-ze1-4]+", "ew421dq3");
	TestRegexTrue("[a-zA-Z]", "V");
	TestRegexTrue("[a-zA]", "A");
	TestRegexTrue("[ABVD]", "D");
	TestRegexTrue("[a-z]", "b");
	TestRegexTrue("[ABVD]", "D");
	TestRegexTrue("(c|b)|a", "ba");
	TestRegexTrue("(c|b)|a", "ca");
	TestRegexTrue("[^a-zA]", "C");*/
	auto end = clock();
	cout << (double)end - start;
	int a = 0;
	cin >> a;
	return 0;
}