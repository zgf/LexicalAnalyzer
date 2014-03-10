#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "RegexLexParse.h"
using std::ifstream;
using std::string;
using std::istreambuf_iterator;
using std::move;
using std::vector;
class RegexParseCodeGen
{
private:

	string ParseConfig;
	string ParseTemplate;
	//	vector<RegexToken> TokenStream;
public:

	RegexParseCodeGen(string& ConfigFileName, string TemplateName)
	{
		this->ParseConfig = ReadFileContent(ConfigFileName);
		this->ParseTemplate = ReadFileContent(TemplateName);
	}
	void DealAll()
	{
		//清除注释
		ParseConfig = DeleteNoteSign(ParseConfig, string("/*"), string("*/"));
		ParseConfig = DeleteNoteSign(ParseConfig, string("//"), string("\n"));

		auto GammarStr = GetNoteSignContent(ParseConfig, string("<Grammar>"), string("</Grammar>"));
		//获得语句
		vector<string>Statement = CutByDefineCharacter(GammarStr, string(";"));
		//最后一个逗号后面的不必要
		Statement.pop_back();
		//获取文法产生式头和体的映射
		map<string, vector<string>> StatementMap = GetStatementMap(Statement);

		//lextag 和 终结符号的映射
		auto TokenDefineStr = GetNoteSignContent(ParseConfig, string("<TokenDefine>"), string("</TokenDefine>"));
		auto TermToTagMap = CreatTermToTagMap(TokenDefineStr);

		auto Result = ReplaceDefinePostion(ParseTemplate, string("//</initGrammarMap>"), CreatGrammarMapStr(TermToTagMap, StatementMap));
		CreateCppFile(string("Test.h"), Result);
	}
	//所有类型都转换为字符串
	template<typename T>
	string AllTypeToString(T Src)
	{
		stringstream ss;
		ss << Src;
		string Temp;
		ss >> Temp;
		return std::move(Temp);
	}
	//按指定字符分割字符串,如果指定字符前有\\,则忽视该处字符,如果字符被"直接包裹" 忽略字符
	vector<string> CutByDefineCharacter(string& SrcStr, string& DefineStr);
	//读取文件内容;
	string ReadFileContent(string& FileName);
	//删除区间含区间端点的内容.
	string DeleteNoteSign(string& SrcStr, string& StartNote, string& EndNote);
	//提取区间内内容,不包括区间端点
	string GetNoteSignContent(string& SrcStr, string& StartNote, string& EndNote);
	//去除字符串两端的空白和换行符
	string Trim(const string& Src);
	//获取文法的产生式头到体的映射
	map<string, vector<string>> GetStatementMap(vector<string>&Statement);

	//创建终结符号到lextag的映射
	map<string, string> CreatTermToTagMap(string DefinePairStr);

	//获取被空白分割的Token
	vector<string> GetSpaceCutToken(string Src);
	//创建添入模板initGrammarMap的内容
	string CreatGrammarMapStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap);

	//是空白字符?
	bool IsWhiteSpaceChar(char i);

	//是Config中特殊字符的字面值?
	bool IsStrLiteral(string& SrcStr, int FindIter);
	//替换Tempalte指定标记的位置的字符串.
	string RegexParseCodeGen::ReplaceDefinePostion(string& Template, string& TemplateSign, string& RePalceData);

	//输出内容到文件
	void RegexParseCodeGen::CreateCppFile(string& FilePatch, string& TextContent);
};