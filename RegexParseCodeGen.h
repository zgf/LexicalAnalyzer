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
		//���ע��
		ParseConfig = DeleteNoteSign(ParseConfig, string("/*"), string("*/"));
		ParseConfig = DeleteNoteSign(ParseConfig, string("//"), string("\n"));

		auto GammarStr = GetNoteSignContent(ParseConfig, string("<Grammar>"), string("</Grammar>"));
		//������
		vector<string>Statement = CutByDefineCharacter(GammarStr, string(";"));
		//���һ�����ź���Ĳ���Ҫ
		Statement.pop_back();
		//��ȡ�ķ�����ʽͷ�����ӳ��
		map<string, vector<string>> StatementMap = GetStatementMap(Statement);

		//lextag �� �ս���ŵ�ӳ��
		auto TokenDefineStr = GetNoteSignContent(ParseConfig, string("<TokenDefine>"), string("</TokenDefine>"));
		auto TermToTagMap = CreatTermToTagMap(TokenDefineStr);

		auto Result = ReplaceDefinePostion(ParseTemplate, string("//</initGrammarMap>"), CreatGrammarMapStr(TermToTagMap, StatementMap));
		CreateCppFile(string("Test.h"), Result);
	}
	//�������Ͷ�ת��Ϊ�ַ���
	template<typename T>
	string AllTypeToString(T Src)
	{
		stringstream ss;
		ss << Src;
		string Temp;
		ss >> Temp;
		return std::move(Temp);
	}
	//��ָ���ַ��ָ��ַ���,���ָ���ַ�ǰ��\\,����Ӹô��ַ�,����ַ���"ֱ�Ӱ���" �����ַ�
	vector<string> CutByDefineCharacter(string& SrcStr, string& DefineStr);
	//��ȡ�ļ�����;
	string ReadFileContent(string& FileName);
	//ɾ�����京����˵������.
	string DeleteNoteSign(string& SrcStr, string& StartNote, string& EndNote);
	//��ȡ����������,����������˵�
	string GetNoteSignContent(string& SrcStr, string& StartNote, string& EndNote);
	//ȥ���ַ������˵Ŀհ׺ͻ��з�
	string Trim(const string& Src);
	//��ȡ�ķ��Ĳ���ʽͷ�����ӳ��
	map<string, vector<string>> GetStatementMap(vector<string>&Statement);

	//�����ս���ŵ�lextag��ӳ��
	map<string, string> CreatTermToTagMap(string DefinePairStr);

	//��ȡ���հ׷ָ��Token
	vector<string> GetSpaceCutToken(string Src);
	//��������ģ��initGrammarMap������
	string CreatGrammarMapStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap);

	//�ǿհ��ַ�?
	bool IsWhiteSpaceChar(char i);

	//��Config�������ַ�������ֵ?
	bool IsStrLiteral(string& SrcStr, int FindIter);
	//�滻Tempalteָ����ǵ�λ�õ��ַ���.
	string RegexParseCodeGen::ReplaceDefinePostion(string& Template, string& TemplateSign, string& RePalceData);

	//������ݵ��ļ�
	void RegexParseCodeGen::CreateCppFile(string& FilePatch, string& TextContent);
};