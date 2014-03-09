#pragma once
#include <string>
#include <vector>
#include <fstream>
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
	vector<RegexToken> TokenStream;
public:
	
	RegexParseCodeGen(string& ConfigFileName/*,vector<RegexToken>& TokenArr*/)
	{
		this->ParseConfig = ReadFileContent(ConfigFileName);
		//this->TokenStream = TokenArr;
	}
	void DealAll()
	{
		//清除注释
		ParseConfig = DeleteNoteSign(ParseConfig, string("/*"), string("*/"));
		ParseConfig = DeleteNoteSign(ParseConfig, string("//"), string("\n"));
		ParseConfig = GetNoteSignContent(ParseConfig, string("<Grammer>"), string("</Grammer>"));
		//获得语句
		vector<string>Statement = CutByDefineCharacter(ParseConfig, string(";"));

		//去除空白String
		for(auto Iter = Statement.begin(); Iter != Statement.end(); )
		{
			if (Trim(*Iter).size() == 0)
			{
				Iter = Statement.erase(Iter);
			}
			else
			{
				Iter++;
			}
		}

		//产生式头部 -> 产生式体;
		map<string, vector<string>> StatementMap;
		for ( auto& Iter : Statement)
		{
			auto StatementBlock = CutByDefineCharacter(Iter, string("=>"));
			auto BodyList = CutByDefineCharacter(StatementBlock.back(), string("|"));
			StatementMap.insert(make_pair(StatementBlock.front(), BodyList));
		}

	}
	//按指定字符分割字符串,如果指定字符前有\\,则忽视该处字符
	vector<string> CutByDefineCharacter(string& SrcStr, string& DefineStr);
	//读取文件内容;
	string ReadFileContent(string& FileName);
	//删除区间含区间端点的内容.
	string DeleteNoteSign(string& SrcStr, string& StartNote, string& EndNote);
	//提取区间内内容,不包括区间端点
	string GetNoteSignContent(string& SrcStr, string& StartNote, string& EndNote);
	//去除字符串两端的空白和换行符
	string Trim(string& Src);
	
};