#include "RegexParseCodeGen.h"
#include <iostream>
string RegexParseCodeGen::ReadFileContent(string& FileName)
{
	ifstream Input(FileName);
	istreambuf_iterator<char> beg(Input), end;
	string StrData(beg, end);
	Input.close();
	return move(StrData);
}
bool RegexParseCodeGen::IsStrLiteral(string& SrcStr, int FindIter)
{
	return SrcStr[FindIter - 1] == '\\' || ( SrcStr[FindIter - 1] == '"' && SrcStr[FindIter + 1] == '"' );
}
vector<string> RegexParseCodeGen::CutByDefineCharacter(string& SrcStr, string& DefineStr)
{
	auto StartIndex = 0;
	vector<string> Result;
	vector<int>SignList;
	SignList.push_back(0 - DefineStr.size());
	while(true)
	{
		auto FindIter = SrcStr.find(DefineStr, StartIndex);
		if(FindIter == SrcStr.npos)
		{
			break;
		}
		else if(FindIter != 0 && IsStrLiteral(SrcStr, FindIter))
		{
			StartIndex = FindIter + DefineStr.size();
		}
		else
		{
			SignList.push_back(FindIter);
			StartIndex = FindIter + DefineStr.size();
		}
	}

	if(SignList.size() != 1)
	{
		for(int i = 0; i < SignList.size() - 1; i++)
		{
			auto Catch = SrcStr.substr(SignList[i] + DefineStr.size(), SignList[i + 1] - ( SignList[i] + DefineStr.size() ));
			if(!Catch.empty())
			{
				Result.push_back(Catch);
			}
		}
		Result.push_back(SrcStr.substr(SignList.back() + DefineStr.size()));
	}
	else
	{
		Result.push_back(SrcStr);
	}
	return std::move(Result);
	//auto FindIter = 0;
	//vector<string> Result;
	//auto OldIter = 0;
	//while(OldIter < SrcStr.size() && FindIter != SrcStr.npos)
	//{
	//	FindIter = SrcStr.find(DefineStr, OldIter);
	//	if (FindIter != SrcStr.npos)
	//	{
	//		if(FindIter != 0 && ( SrcStr[FindIter - 1] == '\\' || ( SrcStr[FindIter - 1] == '"' && SrcStr[FindIter + 1] == '"' ) ))
	//		{
	//			OldIter = FindIter + DefineStr.size();
	//		}
	//		else
	//		{
	//			Result.push_back(SrcStr.substr(OldIter, FindIter - OldIter));
	//			OldIter = FindIter + DefineStr.size();
	//		}
	//	}
	//}
	//Result.push_back(SrcStr.substr(OldIter));
	//return std::move(Result);
}
string RegexParseCodeGen::DeleteNoteSign(string& SrcStr, string& StartNote, string& EndNote)
{
	vector<int> PairList;
	auto StartIndex = 0;
	auto EndNoteSize = EndNote.size();
	PairList.push_back(0 - EndNote.size());
	while(StartIndex != SrcStr.npos)
	{
		StartIndex = SrcStr.find(StartNote, StartIndex);
		if(StartIndex != SrcStr.npos)
		{
			PairList.push_back(StartIndex);
			StartIndex = SrcStr.find(EndNote, StartIndex);
			PairList.push_back(StartIndex);
		}
	}
	string NewStr;
	if(!PairList.empty())
	{
		for(auto i = 0; i < PairList.size() - 1; i += 2)
		{
			NewStr = NewStr + SrcStr.substr(PairList[i] + EndNoteSize, ( PairList[i + 1] ) - ( PairList[i] + EndNoteSize ));
		}
		NewStr += SrcStr.substr(PairList.back() + EndNoteSize);
	}
	else
	{
		NewStr = SrcStr;
	}
	return std::move(NewStr);
}
string RegexParseCodeGen::GetNoteSignContent(string& SrcStr, string& StartNote, string& EndNote)
{
	vector<int> PairList;
	auto StartIndex = 0;
	auto StartNoteSize = StartNote.size();
	while(StartIndex != SrcStr.npos && StartIndex < SrcStr.size() - 1)
	{
		StartIndex = SrcStr.find(StartNote, StartIndex);
		if(StartIndex != SrcStr.npos)
		{
			PairList.push_back(StartIndex);
			StartIndex = SrcStr.find(EndNote, StartIndex + 1);
			PairList.push_back(StartIndex);
		}
	}
	string NewStr;
	if(!PairList.empty())
	{
		for(auto i = 0; i < PairList.size() - 1; i += 2)
		{
			NewStr = NewStr + SrcStr.substr(PairList[i] + StartNoteSize, ( PairList[i + 1] ) - ( PairList[i] + StartNoteSize ));
		}
	}
	else
	{
		NewStr = SrcStr;
	}
	return std::move(NewStr);
}
bool RegexParseCodeGen::IsWhiteSpaceChar(char i)
{
	return i == '\t' || i == '\n' || i == ' ';
}
string RegexParseCodeGen::Trim(const string& Src)
{
	auto StartIndex = 0;
	auto EndIndex = 0;
	for(auto i = 0; i < Src.size(); i++)
	{
		auto& Iter = Src[i];
		if(!IsWhiteSpaceChar(Iter))
		{
			StartIndex = i;
			break;
		}
	}
	auto NewSrc = Src.substr(StartIndex);
	for(int i = NewSrc.size() - 1; i >= 0; i--)
	{
		auto& Iter = NewSrc[i];
		if(!IsWhiteSpaceChar(Iter))
		{
			EndIndex = i + 1;
			return std::move(NewSrc.substr(0, EndIndex));
		}
	}

	return std::move(NewSrc);
}
map<string, vector<string>> RegexParseCodeGen::GetStatementMap(vector<string>&Statement)
{
	//产生式头部 -> 产生式体;
	map<string, vector<string>> StatementMap;
	int i = 0;
	for(auto& Iter : Statement)
	{
		/*if (i == 5)
		{
		int a = 0;
		}*/
		auto StatementBlock = CutByDefineCharacter(Iter, string("=>"));
		auto BodyList = CutByDefineCharacter(StatementBlock.back(), string("|"));
		StatementMap.insert(make_pair(Trim(StatementBlock.front()), BodyList));
		i++;
	}
	return std::move(StatementMap);
}
map<string, string> RegexParseCodeGen::CreatTermToTagMap(string DefinePairStr)
{
	//获得语句
	vector<string> PairStr = CutByDefineCharacter(DefinePairStr, string(";"));
	//语句都是;结尾
	PairStr.pop_back();
	map<string, string> Map;
	for(auto& Iter : PairStr)
	{
		auto TempList = CutByDefineCharacter(Iter, string(":"));
		Map.insert(make_pair(Trim(GetNoteSignContent(TempList[1], string("\""), string("\""))), Trim(GetNoteSignContent(TempList[0], string("\""), string("\"")))));
	}
	return std::move(Map);
}

vector<string> RegexParseCodeGen::GetSpaceCutToken(string Src)
{
	vector<pair<int, int>> RangeList;
	bool Chose = true;
	pair<int, int> TempPair;
	for(int i = 0; i <= Src.size(); i++)
	{
		if(Chose == true && ( !IsWhiteSpaceChar(Src[i]) ))
		{
			TempPair.first = i;
			Chose = !Chose;
		}
		else if(Chose == false && ( IsWhiteSpaceChar(Src[i]) || Src[i] == '\0' ))
		{
			TempPair.second = i;
			RangeList.push_back(TempPair);
			Chose = !Chose;
		}
	}
	vector<string> Result;
	for(auto Iter : RangeList)
	{
		Result.push_back(Src.substr(Iter.first, Iter.second - Iter.first));
	}
	return std::move(Result);
}
string RegexParseCodeGen::CreatGrammarListContentStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap)
{
	string TemplateStr = "";// "GrammarList = \n{\n ";
	for(auto& Iter : StatementMap)
	{
		//StrIter 是每一个产生式体的字符串
		auto HeadStr = Trim(Iter.first);
		for(auto StrIter : Iter.second)
		{
			//放vector<Symbol>字符串
			string TempSymbolStr = "GrammarList.push_back(Production(  shared_ptr<Symbol>( new Symbol(false,ParseTag::" + HeadStr + ")),vector<shared_ptr<Symbol>>({";
			auto ProductTokenList = GetSpaceCutToken(StrIter);
			for(auto TokenIter : ProductTokenList)
			{
				/*if (ProductTokenList.size() == 1)
				{
				int a = 0;
				}*/
				int FindIter = TokenIter.find("\"");
				if(FindIter != TokenIter.npos)
				{
					//需要替换的终结符号
					auto re = GetNoteSignContent(TokenIter, string("\""), string("\""));
					auto& test = TermToTagMap.find(re);
					TempSymbolStr = TempSymbolStr + "shared_ptr<Symbol>(new Symbol(true,ParseTag::" + test->second + ")),";
				}
				else
				{
					TempSymbolStr = TempSymbolStr + "shared_ptr<Symbol>(new Symbol(false,ParseTag::" + TokenIter + ")),";
				}
			}
			//清除最后一个逗号
			TempSymbolStr.erase(TempSymbolStr.size() - 1);
			TempSymbolStr = TempSymbolStr + "})));\n";
			TemplateStr = TemplateStr + TempSymbolStr;
		}
	}
	return std::move(TemplateStr);
}
string RegexParseCodeGen::CreatGrammarMapStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap, map<string, vector<string>>& StartStateMap)
{
	auto One = CreatGrammarListContentStr(TermToTagMap, StartStateMap);
	auto Two = CreatGrammarListContentStr(TermToTagMap, StatementMap);
	return std::move(One + Two);
	//return std::move(TemplateStr);
}
string RegexParseCodeGen::ReplaceDefinePostion(string& Template, string& TemplateSign, string& RePalceData)
{
	auto FindPos = Template.find(TemplateSign);
	auto FindNum = TemplateSign.size();
	return std::move(Template.replace(FindPos, FindNum, RePalceData));
}

void RegexParseCodeGen::CreateCppFile(string& FilePatch, string& TextContent)
{
	ofstream Output(FilePatch);
	Output.write(TextContent.c_str(), TextContent.size());
}

map<string, vector<string>> RegexParseCodeGen::GetSpecStateMap(string Src, string StartNode, string EndNote, string StatementCutSign)
{
	auto StartStatement = CutByDefineCharacter(GetNoteSignContent(Src, StartNode, EndNote), StatementCutSign);
	//最后一个分号不需要
	StartStatement.pop_back();
	return 	std::move(GetStatementMap(StartStatement));
}
vector<string> RegexParseCodeGen::GetTermSymbol(map<string, string>& DefineMap)
{
	vector<string>Result;
	for(auto i = DefineMap.begin(); i != DefineMap.end(); i++)
	{
		Result.push_back(i->second);
	}
	return Result;
}

vector<string> RegexParseCodeGen::GetAllSymbolName(map<string, string>&TermToTagMap, map<string, vector<string>>& StartStatementMap, map<string, vector<string>>& StatementMap)
{
	auto TermToTagList = GetTermSymbol(TermToTagMap);
	auto StartStatementList = GetMapStringKey(StartStatementMap);
	auto StatementList = GetMapStringKey(StatementMap);
	TermToTagList.insert(TermToTagList.end(), StartStatementList.begin(), StartStatementList.end());
	TermToTagList.insert(TermToTagList.end(), StatementList.begin(), StatementList.end());
	return std::move(TermToTagList);
}

string RegexParseCodeGen::CreatParseTagEnumStr(vector<string>& NameList)
{
	string ResultStr = "";
	for(auto Iter : NameList)
	{
		ResultStr = ResultStr + Iter + ",\n";
	}
	return std::move(ResultStr);
}