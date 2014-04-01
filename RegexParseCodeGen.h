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
		//���ע��
		ParseConfig = DeleteNoteSign(ParseConfig, string("/*"), string("*/"));
		ParseConfig = DeleteNoteSign(ParseConfig, string("//"), string("\n"));

		//���<Global>��������
		//global����ģ��
		ParseTemplate = ReplaceDefinePostion(ParseTemplate, string("//<Global>"), GetNoteSignContent(ParseConfig, string("<Global>"), string("</Global>")) + "//<Global>");

		// ��ȡDataMember�������ݲ���ģ��
		ParseTemplate = ReplaceDefinePostion(ParseTemplate, string("//<DataMember>"), GetNoteSignContent(ParseConfig, string("<DataMember>"), string("</DataMember>")) + "//<DataMember>");

		// ��ȡPropertyMember�������ݲ���ģ��
		
		ParseTemplate = ReplaceDefinePostion(ParseTemplate, string("//<PropertyMember>"), GetNoteSignContent(ParseConfig, string("<PropertyMember>"), string("</PropertyMember>")) + "//<PropertyMember>");
	
		// ��ȡ�û�����ĺ���,����ģ��
		auto UserDefineFuncStr = GetNoteSignContent(ParseConfig, string("<UserDefineFunc>"), string("</UserDefineFunc>"));
		//�滻��UserDefineFuncStr�е�$$��$i;
		UserDefineFuncStr = Repalce$$And$iStr(UserDefineFuncStr);
		ParseTemplate = ReplaceDefinePostion(ParseTemplate, string("//<UserDefineFunc>"), UserDefineFuncStr + "//<UserDefineFunc>");

		//���<Grammar>������ķ�ӳ��
		auto StatementMap = GetSpecStateMap(ParseConfig, string("<Grammar>"), string("</Grammar>"), string(";"));
		//���<FirstGrammar>����ʼ����ʽ���ӳ��
		auto StartStatementMap = GetSpecStateMap(ParseConfig, string("<FirstGrammar>"), string("</FirstGrammar>"), string(";"));
		//��ȡ�ķ�����ʽͷ�����ӳ��

		//��ȡlexTag �� �ս���ŵ�ӳ��
		auto TermToTagMap = CreatTermToTagMap(GetNoteSignContent(ParseConfig, string("<TokenDefine>"), string("</TokenDefine>")));

		//��ȡ���з�����
		auto NameList = GetAllSymbolName(TermToTagMap, StartStatementMap, StatementMap);
		auto NameEnumStr = CreatParseTagEnumStr(NameList);
		//��ʼ��ParseTag Enum��
		
		ParseTemplate = ReplaceDefinePostion(ParseTemplate, string("//<ParseTag>"), NameEnumStr + "//<ParseTag>");

		//��ʼ��TagMap
		ParseTemplate = ReplaceDefinePostion(ParseTemplate, string("//<initTagMap>"), CreateTagMapStr(TermToTagMap) + "//<initTagMap>");
		//����ģ��
		auto Content = ReplaceDefinePostion(ParseTemplate, string("//<initGrammarMap>"), CreatGrammarMapStr(TermToTagMap, StatementMap, StartStatementMap) + "//<initGrammarMap>");

		//�����ļ�
		CreateCppFile(string("Test.h"), Content);
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
	//ָ��Ԫ���ڷ�Χ�����б�����
	template<typename T>
	//��������Ϊ��ֵ��ԭ��,FindIter��unsigned int����,ǿתint��������ʱ����,���ܰ���ֵ����,��������Ҫ��T&& ����ֵ,�ӳ���������
	int IsInTheRangeList(vector<pair<T, T>>& RangeList, T&& Target)
	{
		for(auto i = 0; i < RangeList.size(); i++)
		{
			auto& Iter = RangeList[i];
			if(Target > Iter.first&& Target < Iter.second)
			{
				return i;
			}
		}
		return -1;
	}
	//�ַ�������,��������ֵ

	int RegexParseCodeGen::FindTrueCharIndex(string& SrcStr, char JumpSign, int StartIndex,vector<int>& SymbolList);


	//��ָ���ַ��ָ��ַ���,���ָ���ַ�ǰ��\\,����Ӹô��ַ�,����ַ���"ֱ�Ӱ���" �����ַ�
	vector<string> CutByDefineCharacter(string& SrcStr, string& DefineStr,vector<int>& SymbolList);

	//��ָ���ַ��ָ��ַ���,���ָ���ַ�ǰ��\\,����Ӹô��ַ�,����ַ���"ֱ�Ӱ���" �����ַ�,����ַ���jump������,�����ַ�
	vector<string> CutByDefineCharacter(string& SrcStr, string& DefineStr, char JumpStart, char JumpEnd);

	// �鿴ָ��λ���ַ��Ƿ�ת��
	bool RegexParseCodeGen::HasChangedMean(string& SrcStr, int FindIter);

	//�������з�����ֵ����λ��
	vector<int> RegexParseCodeGen::FindAllSymbolPostion(string& SrcStr, char Sign = '"');

	//��ȡ�Ƕ������ƥ��
	string RegexParseCodeGen::GetLongestNestedContent(pair<char, char>& NestedSign, string& Src, int SrcIndex, vector<int>& SymbolList);
	//��ȡ�Ƕ������ƥ������һ���ַ�����
	int RegexParseCodeGen::GetLongestNestedEndIndex(pair<char, char>& NestedSign, string& Src, int SrcIndex, vector<int>& SymbolList);
	//��ȡ�ļ�����;
	string ReadFileContent(string& FileName);
	//ɾ�����京����˵������.
	string DeleteNoteSign(string& SrcStr, string& StartNote, string& EndNote);
	//��ȡ����������,����������˵�
	string GetNoteSignContent(string& SrcStr, string& StartNote, string& EndNote);
	//ȥ���ַ������˵Ŀհ׺ͻ��з�
	string Trim(const string& Src);
	//��ȡָ�������ڵ��ķ�����ʽͷ�����ӳ��
	map<string, vector<string>> GetSpecStateMap(string Src, string StartNode, string StatementCutSign, string EndNote);

	//��ȡ�ķ��Ĳ���ʽͷ�����ӳ��
	map<string, vector<string>> GetStatementMap(vector<string>&Statement);

	//�����ս���ŵ�lextag��ӳ��
	map<string, string> CreatTermToTagMap(string DefinePairStr);

	//��ȡ���հ׷ָ��Token
	vector<string> GetSpaceCutToken(string Src);
	//��������ģ��initGrammarMap������
	string CreatGrammarMapStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap, map<string, vector<string>>& StartStateMap);
	//��ȡҪ�����ķ��ŶԷ�Χ�б�
	vector<pair<int, int>> RegexParseCodeGen::GetNeedJumpList(string& SrcStr, string& DefineStr, char JumpStart, char JumpEnd,vector<int>& SymbolList);
	//��ȡStr��ĳ���ִ��������ݵ��б�
	vector<int> GetNeedSignList(string& SrcStr, string& DefineStr, vector<pair<int, int>>& JumpList,vector<int>& SymbolList);
	vector<int> GetNeedSignList(string& SrcStr, string& DefineStr,vector<int>&SymbolList);
	//�ǿհ��ַ�?
	bool IsWhiteSpaceChar(char i);

	//��Config�������ַ�������ֵ?
	bool IsStrLiteral(string& SrcStr, int FindIter);
	bool RegexParseCodeGen::IsStrLiteral(string& SrcStr, int FindIter, vector<int> SymbolList);

	//�滻Tempalteָ����ǵ�λ�õ��ַ���.
	string RegexParseCodeGen::ReplaceDefinePostion(string& Template, string& TemplateSign, string& RePalceData);

	//����ģ���ļ����ķ�List�����
	string CreatGrammarListContentStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap);

	//��Map�л�ȡ����ķ��ս���ź��ս������List
	vector<string> GetAllSymbolName(map<string, string>&TermToTagMap, map<string, vector<string>>& StartStatementMap, map<string, vector<string>>& StatementMap);
	//��ȡ���������е��ս������
	vector<string> GetTermSymbol(map<string, string>& DefineMap);
	//��ȡMap��key�ķ�����,key ->string.
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
	// ����ĳ�������ڴ��г������һ�ε�λ��
	int FindSignLastIndex(string& Target, string& Sign);

	//�����еķ�����ȥ���������ļ��е�ParseTagö����
	string CreatParseTagEnumStr(vector<string>& NameList);

	//����TagMap��Str
	string CreateTagMapStr(map<string, string>& TermToMap);

	//�����﷨�б�����������Ƭ�εİ�
	string RegexParseCodeGen::CreatIndexToActionStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap);

	//�鿴Ŀ�괮�д��ڲ���������Ƭ��
	int RegexParseCodeGen::HasSemanticAction(string& Target);

	string&   replace_all_distinct(string&   str, const   string&   old_value, const   string&   new_value)
	{
		for(string::size_type pos(0); pos != string::npos; pos += new_value.length())
		{
			if(( pos = str.find(old_value, pos) ) != string::npos)
				str.replace(pos, old_value.length(), new_value);
			else   break;
		}
		return   move(str);
	}

	//�滻��ָ���ַ����е�$$ ��$i
	string Repalce$$And$iStr(string& Target)
	{
		Target = Replace$$(Target);
		Target = Replace$i(Target);
		
		return std::move(Target);
	}
	string Replace$$(string& Target)
	{
		string Sym = "$$";
		auto FindIter = Target.find(Sym);
		if(FindIter != Target.npos)
		{
			string ReplaceData = "NewNode";
			Target = replace_all_distinct(Target, Sym, ReplaceData);
		}
		Sym = "NewNode.";
		FindIter = Target.find(Sym);
		if(FindIter != Target.npos)
		{
			string ReplaceData = "NewNode->";
			Target = replace_all_distinct(Target, Sym, ReplaceData);
		}
		return std::move(Target);
	}
	string Replace$i(string& Target)
	{
		//$i.X�滻
		for(auto i = 1;i<=10; i++)
		{

			auto Count = to_string(i);
			auto Sym = "$" + Count;
			auto FindIter = Target.find(Sym);
			if(FindIter != Target.npos)
			{
				auto ReplaceData = "CatchStack[CatchStack.size()-" + Count + "]";
				Target = replace_all_distinct(Target, Sym, ReplaceData);
			}
		}
		for(auto i = 1;i<=10; i++)
		{

			auto Count = to_string(i);
			auto Sym = "CatchStack[CatchStack.size()-" + Count + "].";
			auto FindIter = Target.find(Sym);
			if(FindIter != Target.npos)
			{
				auto ReplaceData = "CatchStack[CatchStack.size()-" + Count + "]->";
				Target = replace_all_distinct(Target, Sym, ReplaceData);
			}
		}
		return std::move(Target);
	}
	//������ݵ��ļ�
	void RegexParseCodeGen::CreateCppFile(string& FilePatch, string& TextContent);
};