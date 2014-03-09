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
		//���ע��
		ParseConfig = DeleteNoteSign(ParseConfig, string("/*"), string("*/"));
		ParseConfig = DeleteNoteSign(ParseConfig, string("//"), string("\n"));
		ParseConfig = GetNoteSignContent(ParseConfig, string("<Grammer>"), string("</Grammer>"));
		//������
		vector<string>Statement = CutByDefineCharacter(ParseConfig, string(";"));

		//ȥ���հ�String
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

		//����ʽͷ�� -> ����ʽ��;
		map<string, vector<string>> StatementMap;
		for ( auto& Iter : Statement)
		{
			auto StatementBlock = CutByDefineCharacter(Iter, string("=>"));
			auto BodyList = CutByDefineCharacter(StatementBlock.back(), string("|"));
			StatementMap.insert(make_pair(StatementBlock.front(), BodyList));
		}

	}
	//��ָ���ַ��ָ��ַ���,���ָ���ַ�ǰ��\\,����Ӹô��ַ�
	vector<string> CutByDefineCharacter(string& SrcStr, string& DefineStr);
	//��ȡ�ļ�����;
	string ReadFileContent(string& FileName);
	//ɾ�����京����˵������.
	string DeleteNoteSign(string& SrcStr, string& StartNote, string& EndNote);
	//��ȡ����������,����������˵�
	string GetNoteSignContent(string& SrcStr, string& StartNote, string& EndNote);
	//ȥ���ַ������˵Ŀհ׺ͻ��з�
	string Trim(string& Src);
	
};