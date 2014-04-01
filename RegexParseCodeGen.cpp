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

// 查看指定位置字符是否被转义
bool RegexParseCodeGen::HasChangedMean(string& SrcStr, int FindIter)
{
	if(FindIter == 0)
	{
		return false;
	}
	else
	{
		return SrcStr[FindIter - 1] == '\\';
	}
}

//查找所有非字面值引号位置
vector<int> RegexParseCodeGen::FindAllSymbolPostion(string& SrcStr, char Sign )
{
	vector<int> Result;
	for(auto i = 0; i < SrcStr.size(); i++)
	{
		if(SrcStr[i] == '"')
		{
			if(!HasChangedMean(SrcStr, i))
			{
				Result.push_back(i);
			}
		}
	}
	return std::move(Result);
}

//
//template<typename U>
//int BinarySearch(vector<U>& Container, U& Target,int Low,int Height)
//{
//	if(Low > Height)
//	{
//		return -1;
//	}
//	int Mid = Low + Height / 2;
//	if (Container[Mid] == Target)
//	{
//		return Mid;
//	}
//	else if(Container[Mid] < Target)
//	{
//		return BinarySearch(Container, Target, 0, Mid - 1);
//	}
//	else
//	{
//		return BinarySearch(Container, Target, Mid + 1, Height);
//	}
//}

bool RegexParseCodeGen::IsStrLiteral(string& SrcStr, int FindIter,vector<int> SymbolList)
{
	if(FindIter == 0)
	{
		return false;
	}
	else if(HasChangedMean(SrcStr, FindIter))
	{
		return true;
	}
	else
	{
		for(auto i = 0; i < SymbolList.size();i++)
		{
			if (SymbolList[i] > FindIter)
			{
				return i %2;
			}
		}
		return false;
	}
}


//其实查找字面值应该是,找指定字符是否在""内或者前面有\\这样的.
bool RegexParseCodeGen::IsStrLiteral(string& SrcStr, int FindIter)
{
	int Count = 0;
	if (FindIter == 0)
	{
		return false;
	}
	else if(HasChangedMean(SrcStr, FindIter))
	{
		return true;
	}
	else
	{
		for(auto i = 0; i < FindIter;i++)
		{
			if (SrcStr[i] == '"')
			{
				if(!HasChangedMean(SrcStr, i))
				{
					Count++;
				}
			}
			
		}
	}
	return Count % 2;
}

//查看该字符是不是字面值,字面值要跳过,字面值是指被""包围或者前面有\\的字符

int RegexParseCodeGen::GetLongestNestedEndIndex(pair<char, char>& NestedSign, string& Src, int SrcIndex,vector<int>& SymbolList)
{
	int StackList = 0;
	StackList = 1;
	auto StartIndex = SrcIndex;
	auto Length = 1;
	while(StackList != 0)
	{
		SrcIndex += 1;
		Length += 1;

		if (!IsStrLiteral(Src,SrcIndex,SymbolList))
		{
			if(Src[SrcIndex] == NestedSign.first)
			{
				StackList += 1;
			}
			else if(Src[SrcIndex] == NestedSign.second)
			{
				StackList -= 1;
			}
		}
		
	}
	return StartIndex + Length;
}
string RegexParseCodeGen::GetLongestNestedContent(pair<char, char>& NestedSign, string& Src, int SrcIndex,vector<int>& SymbolList)
{
	string Symbol;
	auto StartIndex = Src.find(NestedSign.first,SrcIndex);
	//这里的Index是超尾
	auto EndIndex = GetLongestNestedEndIndex(NestedSign, Src, StartIndex,SymbolList);
	StartIndex++;
	Symbol = Src.substr(StartIndex, EndIndex - StartIndex - 1);
	return std::move(Symbol);
}

//字符串查找,跳过字面值

int RegexParseCodeGen::FindTrueCharIndex(string& SrcStr, char JumpSign, int StartIndex,vector<int>& SymbolList)
{
	for(auto i = StartIndex; i < SrcStr.size();i++)
	{
		if (SrcStr[i] == JumpSign)
		{
			if(!IsStrLiteral(SrcStr, i, SymbolList))
			{
				return i;
			}
		}
		
	}
	return -1;
}

string RegexParseCodeGen::CreatIndexToActionStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap)
{
	return string();
}

vector<pair<int, int>> RegexParseCodeGen::GetNeedJumpList(string& SrcStr, string& DefineStr, char JumpStart, char JumpEnd,vector<int>& SymbolList)
{
	auto StartIndex = 0;
	auto FindStartIndex = 0;
	vector<pair<int, int>> JumpList;
	while(true)
	{
		FindStartIndex = FindTrueCharIndex(SrcStr, JumpStart, StartIndex, SymbolList);
		
		if(FindStartIndex == -1)
		{
			break;
		}
		else
		{
			pair<int, int>Temp;
			Temp.first = FindStartIndex;
			auto FindEndIndex = GetLongestNestedEndIndex(pair<char, char>(JumpStart, JumpEnd), SrcStr, FindStartIndex,SymbolList);
			Temp.second = FindEndIndex;
			JumpList.push_back(Temp);
			StartIndex = FindEndIndex;
		}
	}
	return std::move(JumpList);



	//auto StartIndex = 0;
	//auto FindStartIndex = 0;
	//vector<pair<int, int>> JumpList;
	//while(true)
	//{
	//	FindStartIndex = SrcStr.find(JumpStart, StartIndex);

	//	if(FindStartIndex == SrcStr.npos)
	//	{
	//		break;
	//	}
	//	else if(FindStartIndex != 0 && IsStrLiteral(SrcStr, FindStartIndex))
	//	{
	//		StartIndex = StartIndex + 1;
	//	}
	//	else
	//	{
	//		pair<int, int>Temp;
	//		Temp.first = FindStartIndex;
	//		auto FindEndIndex = GetLongestNestedEndIndex(pair<char, char>(JumpStart, JumpEnd), SrcStr, FindStartIndex);
	//		Temp.second = FindEndIndex;
	//		JumpList.push_back(Temp);
	//		StartIndex = FindEndIndex;
	//	}
	//}
	//return std::move(JumpList);
}
vector<int> RegexParseCodeGen::GetNeedSignList(string& SrcStr, string& DefineStr, vector<pair<int, int>>& JumpList,vector<int>& SymbolList)
{
	auto StartIndex = 0;
	vector<int> SignList;
	SignList.push_back(0 - DefineStr.size());
	while(true)
	{
		auto FindIter = SrcStr.find(DefineStr, StartIndex);
		if(FindIter == SrcStr.npos)
		{
			break;
		}
		else if(FindIter != 0 && IsStrLiteral(SrcStr, FindIter, SymbolList))
		{
			StartIndex = FindIter + DefineStr.size();
		}
		else
		{
			auto FindIndex = IsInTheRangeList(JumpList, (int)FindIter);
			if(FindIndex != -1)
			{
				StartIndex = JumpList[FindIndex].second;
			}
			else
			{
				SignList.push_back(FindIter);
				StartIndex = FindIter + DefineStr.size();
			}
		}
	}
	return move(SignList);
}
vector<int> RegexParseCodeGen::GetNeedSignList(string& SrcStr, string& DefineStr, vector<int>& SymbolList)
{
	vector<int>SignList;
	auto StartIndex = 0;
	SignList.push_back(0 - DefineStr.size());
	while(true)
	{
		auto FindIter = SrcStr.find(DefineStr, StartIndex);
		if(FindIter == SrcStr.npos)
		{
			break;
		}
		else if(FindIter != 0 && IsStrLiteral(SrcStr, FindIter, SymbolList))
		{
			StartIndex = FindIter + DefineStr.size();
		}
		else
		{
			SignList.push_back(FindIter);
			StartIndex = FindIter + DefineStr.size();
		}
	}
	return move(SignList);
}

vector<string> RegexParseCodeGen::CutByDefineCharacter(string& SrcStr, string& DefineStr, char JumpStart, char JumpEnd)
{
	//TODO
	//获取花括号的嵌套范围

	vector<string>Result;
	vector<int>ReferSymbolList = FindAllSymbolPostion(SrcStr);
	auto JumpList = GetNeedJumpList(SrcStr, DefineStr, JumpStart, JumpEnd, ReferSymbolList);

	auto SignList = GetNeedSignList(SrcStr, DefineStr, JumpList, ReferSymbolList);
	if(SignList.size() != 1)
	{
		for(int i = 0; i < SignList.size() - 1; i++)
		{
			if (i== 7)
			{
				int a = 0;
			}
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
}

vector<string> RegexParseCodeGen::CutByDefineCharacter(string& SrcStr, string& DefineStr, vector<int>&SymbolList)
{
	vector<string> Result;

	auto SignList = GetNeedSignList(SrcStr, DefineStr,SymbolList);
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
		
	
		auto StatementBlock = CutByDefineCharacter(Iter, string("=>"), FindAllSymbolPostion(Iter));
		auto BodyList = CutByDefineCharacter(StatementBlock.back(), string("|"), FindAllSymbolPostion(StatementBlock.back()));
		StatementMap.insert(make_pair(Trim(StatementBlock.front()), BodyList));
		i++;
	}
	return std::move(StatementMap);
}
map<string, string> RegexParseCodeGen::CreatTermToTagMap(string DefinePairStr)
{
	//获得语句
	vector<string> PairStr = CutByDefineCharacter(DefinePairStr, string(";"), FindAllSymbolPostion(DefinePairStr));
	//语句都是;结尾
	PairStr.pop_back();
	map<string, string> Map;
	for(auto& Iter : PairStr)
	{
		auto TempList = CutByDefineCharacter(Iter, string(":"), FindAllSymbolPostion(Iter));
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
int RegexParseCodeGen::FindSignLastIndex(string& Target, string& Sign)
{
	auto FindIter = Target.find(Sign);
	auto LastIndex = -1;
	if(FindIter != Target.npos)
	{
		while(FindIter != Target.npos)
		{
			FindIter = FindIter + Sign.size();
			if(FindIter >= Target.size())
			{
				break;
			}
			else
			{
				LastIndex = FindIter;
				FindIter = Target.find(Sign, FindIter);
			}
		}
	}

	return LastIndex;
}

int RegexParseCodeGen::HasSemanticAction(string& Target)
{
	string Sign = "\"{\"";
	for(auto i = 0; i < Target.size(); i++)
	{
		auto LastIndex = FindSignLastIndex(Target, Sign);
		auto FindIndex = 0;
		LastIndex = LastIndex != -1 ? LastIndex : 0;
		FindIndex = Target.find('{', LastIndex);
		if(FindIndex != Target.npos)
		{
			return FindIndex;
		}
		else
		{
			return -1;
		}
	}
	return -1;
}
string RegexParseCodeGen::CreatGrammarListContentStr(map<string, string>&TermToTagMap, map<string, vector<string>>& StatementMap)
{
	static int Count = 0;
	string TemplateStr = "";// "GrammarList = \n{\n ";
	string FuncStr = "";
	string BindStr = "";
	for(auto& Iter : StatementMap)
	{
		if (Iter.first =="Repeat")
		{
			int a = 0;
		}
		//StrIter 是每一个产生式体的字符串
		auto HeadStr = Trim(Iter.first);
		for(auto& StrIter : Iter.second)
		{
			//放vector<Symbol>字符串

			auto SemanticStartIndex = HasSemanticAction(StrIter);
			string SemanticStr;
			if(SemanticStartIndex != -1)
			{
				auto ReferSymbolList = FindAllSymbolPostion(StrIter);
				SemanticStr = GetLongestNestedContent(pair<char, char>('{', '}'), StrIter, SemanticStartIndex, ReferSymbolList);
				
				StrIter = StrIter.erase(StrIter.find('{' + SemanticStr + '}'), SemanticStr.size() + 2);
			}

			string TempSymbolStr = "GrammarList.push_back(Production( Symbol(false,ParseTag::" + HeadStr + "),vector<Symbol>({";

			Count++;

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
					/*auto re = GetNoteSignContent(TokenIter, string("\""), string("\""));*/
					auto& test = TermToTagMap.find(GetNoteSignContent(TokenIter, string("\""), string("\"")));
						TempSymbolStr = TempSymbolStr + "Symbol(true,ParseTag::" + test->second + "),";
				}
				else
				{
					TempSymbolStr = TempSymbolStr + "Symbol(false,ParseTag::" + TokenIter + "),";
				}
			}
			//清除最后一个逗号
			TempSymbolStr.erase(TempSymbolStr.size() - 1);
			TempSymbolStr = TempSymbolStr + "})));\n";

			//将语义片段构造成函数放到指定位置
			if(!SemanticStr.empty())
			{
				string CountStr = to_string(Count);
				string FuncName = "Production" + CountStr ;
				string FuncParam = "(vector<shared_ptr<Property>>& CatchStack,int StreamIndex,vector<shared_ptr<RegexToken>>& TokenStream)";
				string PreInsertContent = "NewNode= shared_ptr<Property> (new Property())._Get();\n";
				string TailInsertContent = "";
				FuncStr = FuncStr + "void " + FuncName + FuncParam + "\n{\n" + PreInsertContent + Repalce$$And$iStr(SemanticStr) + TailInsertContent + "\n};\n";
				//添加语义内容的绑定

				BindStr = BindStr + "SemanticActionMap.insert(make_pair(" + CountStr + ", bind(&RegexParse::Production" + CountStr + ", this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)));\n ";
			}

			TemplateStr = TemplateStr + TempSymbolStr;
		}
	}
	FuncStr = FuncStr + "\n//<SemanticFunc>\n";
	BindStr = BindStr + "\n//<initSemanticMap>\n";

	//加入Action函数
	ParseTemplate = ReplaceDefinePostion(ParseTemplate, string("//<SemanticFunc>"), FuncStr);

	//初始化action函数的绑定
	ParseTemplate = ReplaceDefinePostion(ParseTemplate, string("//<initSemanticMap>"), BindStr);

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

	/*if(TemplateSign == "//<PropertyMember>")
	{
		for(auto i = 1500; i < Template.size(); i++)
		{
			if(i == 2000)
			{
				break;
			}
			cout << Template[i];
		}
	}*/
	
	return std::move(Template.replace(FindPos, FindNum, RePalceData));
}

void RegexParseCodeGen::CreateCppFile(string& FilePatch, string& TextContent)
{
	ofstream Output(FilePatch);
	Output.write(TextContent.c_str(), TextContent.size());
}

map<string, vector<string>> RegexParseCodeGen::GetSpecStateMap(string Src, string StartNode, string EndNote, string StatementCutSign)
{
	if (StartNode == "<Grammar>")
	{
		int a = 0;
	}
	auto Content = GetNoteSignContent(Src, StartNode, EndNote);
	auto StartStatement = CutByDefineCharacter(Content, StatementCutSign, '{', '}');
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
string RegexParseCodeGen::CreateTagMapStr(map<string, string>& TermToMap)
{
	string Content;
	for(auto& Iter : TermToMap)
	{
		Content = Content + "TagMap.insert(make_pair(LexTag::" + Iter.second + ",ParseTag::" + Iter.second + "));\n";
	}
	return std::move(Content);
}