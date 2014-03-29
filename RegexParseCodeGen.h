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

	RegexParseCodeGen(string& ConfigFileName, string& TemplateName)
	{
		this->ParseConfig = ReadFileContent(ConfigFileName);
		this->ParseTemplate = ReadFileContent(TemplateName);
	}
	void DealAll()
	{
		//清除注释
		ParseConfig = DeleteNoteSign(ParseConfig, string("/*"), string("*/"));
		ParseConfig = DeleteNoteSign(ParseConfig, string("//"), string("\n"));

		//获得<Grammar>区域的文法映射
		auto StatementMap = GetSpecStateMap(ParseConfig, string("<Grammar>"), string("</Grammar>"), string(";"));
		//获得<FirstGrammar>区域开始产生式语句映射
		auto StartStatementMap = GetSpecStateMap(ParseConfig, string("<FirstGrammar>"), string("</FirstGrammar>"), string(";"));
		//获取文法产生式头和体的映射

		//获取lexTag 和 终结符号的映射
		auto TermToTagMap = CreatTermToTagMap(GetNoteSignContent(ParseConfig, string("<TokenDefine>"), string("</TokenDefine>")));

		//获取所有符号名
		auto NameList = GetAllSymbolName(TermToTagMap, StartStatementMap, StatementMap);
		auto NameEnumStr = CreatParseTagEnumStr(NameList);
		//初始化ParseTag Enum类
		ParseTemplate = ReplaceDefinePostion(ParseTemplate, string("//<ParseTag>"), NameEnumStr);

		//初始化TagMap
		ParseTemplate = ReplaceDefinePostion(ParseTemplate, string("//<initTagMap>"), CreateTagMapStr(TermToTagMap));
		//插入模板
		auto Content = ReplaceDefinePostion(ParseTemplate, string("//<initGrammarMap>"), CreatGrammarMapStr(TermToTagMap, StatementMap, StartStatementMap));
		
		//创建文件
		CreateCppFile(string("Test.h"), Content);
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
	//获取指定区域内的文法产生式头到体的映射
	map<string, vector<string>> GetSpecStateMap(string Src, string StartNode, string StatementCutSign, string EndNote);

	//获取文法的产生式头到体的映射
	map<string, vector<string>> GetStatementMap(vector<string>&Statement);

	//创建终结符号到lextag的映射
	map<string, string> CreatTermToTagMap(string DefinePairStr);

	//获取被空白分割的Token
	vector<string> GetSpaceCutToken(string Src);
	//创建添入模板initGrammarMap的内容
	string CreatGrammarMapStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap, map<string, vector<string>>& StartStateMap);

	//是空白字符?
	bool IsWhiteSpaceChar(char i);

	//是Config中特殊字符的字面值?
	bool IsStrLiteral(string& SrcStr, int FindIter);
	//替换Tempalte指定标记的位置的字符串.
	string RegexParseCodeGen::ReplaceDefinePostion(string& Template, string& TemplateSign, string& RePalceData);

	//创建模板文件中文法List的语句
	string CreatGrammarListContentStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap);

	//从Map中获取定义的非终结符号和终结符号名List
	vector<string> GetAllSymbolName(map<string, string>&TermToTagMap, map<string, vector<string>>& StartStatementMap, map<string, vector<string>>& StatementMap);
	//获取定义区域中的终结符号名
	vector<string> GetTermSymbol(map<string, string>& DefineMap);
	//获取Map的key的符号名,key ->string.
	template<typename T>
	vector<string> GetMapStringKey(T& DefineMap)
	{
		vector<string> Result;
		for(auto i = DefineMap.begin(); i != DefineMap.end(); i++)
		{
			Result.push_back(i->first);
		}
		return move(Result);
	}

	//用所有的符号名去定义生成文件中的ParseTag枚举类
	string CreatParseTagEnumStr(vector<string>& NameList);

	//创建TagMap的Str
	string CreateTagMapStr(map<string,string>& TermToMap);
	//输出内容到文件
	void RegexParseCodeGen::CreateCppFile(string& FilePatch, string& TextContent);
};