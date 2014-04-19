#pragma once;
#include "RegexDFA.h"
#include <fstream>
class StringHandling
{
	//替换指定记号的内容.但是只替换一次.
	string ReplaceDefinePostion(string& Template, string& TemplateSign, string& RePalceData)
	{
		auto FindPos = Template.find(TemplateSign);
		auto FindNum = TemplateSign.size();
		return std::move(Template.replace(FindPos, FindNum, RePalceData));
	}
	//替换全部指定记号的内容

	string   replace_all_distinct(string&   str, const   string&   old_value, const   string&   new_value)
	{
		for(string::size_type pos(0); pos != string::npos; pos += new_value.length())
		{
			if(( pos = str.find(old_value, pos) ) != string::npos)
				str.replace(pos, old_value.length(), new_value);
			else   break;
		}
		return   move(str);
	}
	//创建文件
	void CreateCppFile(string& FilePatch, string& TextContent)
	{
		ofstream Output(FilePatch);
		Output.write(TextContent.c_str(), TextContent.size());
	}
	//读取文件
	static string ReadFileContent(string& FileName)
	{
		ifstream Input(FileName);
		istreambuf_iterator<char> beg(Input), end;
		string StrData(beg, end);
		Input.close();
		return move(StrData);
	}
	//清除指定区域内的内容,包括界定符
	static string DeleteNoteSign(string& SrcStr, string& StartNote, string& EndNote)
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
	//获取标示符内内容,不包括界定符号
	string GetNoteSignContent(string& SrcStr, string& StartNote, string& EndNote)
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
};
class LexCodeGen
{
public:
	LexCodeGen(string& ConfigFileName, string& TemplateFileName)
	{
		LexConfig = ReadFileContent(ConfigFileName);
		LexTemplate = ReadFileContent(TemplateFileName);

		//清除注释
		LexConfig = DeleteNoteSign(LexConfig, string("/*"), string("*/"));
		LexConfig = DeleteNoteSign(LexConfig, string("//"), string("\n"));

		//插入各个区域内的内容.
		vector<string> AreaList = {"PreClassGlobal", "HeadFile", "TokenTag", "TokenClass", "ClassMember"};
		for(auto& Iter : AreaList)
		{
			LexTemplate = ReplaceDefinePostion(LexTemplate, string("//<" + Iter + ">"), GetNoteSignContent(LexConfig, string("<" + Iter + ">"), string("</" + Iter + ">")) + "//<" + Iter + ">");
		}

		//获取TypeDefine区域内容.
		string TypeDefineStr = GetNoteSignContent(LexConfig, string("<TypeDefine>"), string("</TypeDefine>"));
		auto AliasPairList = GetAliasStringPairList(TypeDefineStr);
		auto& AliasMap = ReplaceAliasInRule(AliasPairList);
		//获取Action的内容
		string ActionStr = GetNoteSignContent(LexConfig, string("<Action>"), string("</Action>"));
		auto ActionPairList = GetAliasStringPairList(ActionStr);
		auto& ActionMap = ReplaceActionAlias(ActionPairList, AliasMap);
		//初始化RegexList.
		auto& RegexListAndActionPair = CreatRegexListStr(ActionMap);
		//语义动作函数的内容字符串和对应的regex编号的Map;
		auto& ActionContentMap = RegexListAndActionPair.first;
		LexTemplate = ReplaceDefinePostion(LexTemplate, string("//<initRegexList>"), RegexListAndActionPair.second + "//<initRegexList>");

		//Creat语义动作Str.注意,这里函数对ActionMap修改了.
		auto& SemanticStr = CreatActionFunctionStr(ActionContentMap);
		LexTemplate = ReplaceDefinePostion(LexTemplate, string("//<initActionFunction>"), SemanticStr + "//<initActionFunction>");
		//初始化RegexMap
		auto& RegexMapStr = CreatinitRegexMapStr(ActionContentMap);
		LexTemplate = ReplaceDefinePostion(LexTemplate, string("//<initRegexMap>"), RegexMapStr + "//<initRegexMap>");

		CreateCppFile(string("Test.h"), LexTemplate);
	}
	~LexCodeGen()
	{
	}
	//初始化语义动作的函数字符串.并将ActionMap的value调整为函数名
	string CreatActionFunctionStr(unordered_map<int, string>&ActionMap)
	{
		string ActionFunctionStr;

		for(auto& Iter = ActionMap.begin(); Iter != ActionMap.end(); Iter++)
		{
			string FunctionName = "Semantic" + std::to_string(Iter->first);
			string FunctionHead = "shared_ptr<Token>" + FunctionName + "(const int& FindIndex,const string& CatchContent,const string& SrcStr)\n";
			ActionFunctionStr += FunctionHead + "{\n" + Iter->second + "\n}\n";
			Iter->second = FunctionName;
		}
		return std::move(ActionFunctionStr);
	}
	pair<unordered_map<int, string>, string> CreatRegexListStr(unordered_map<string, string>&ActionMap)
	{
		int count = 0;
		unordered_map<int, string> ResultLeft;
		string initRegexListStr;
		for(auto&Iter : ActionMap)
		{
			initRegexListStr += "RegexList.push_back(\"" + Iter.first + "\");\n";
			if(Iter.second.find("Continue") == Iter.second.npos)
			{
				ResultLeft.insert(make_pair(count, Iter.second));
			}
			count++;
		}
		return std::move(pair<unordered_map<int, string>, string>(ResultLeft, initRegexListStr));
	}
	string CreatinitRegexMapStr(unordered_map<int, string>&ActionMap)
	{
		string Str;
		for(auto Iter = ActionMap.begin(); Iter != ActionMap.end(); Iter++)
		{
			Str += "RegexMap.insert(make_pair(" + to_string(Iter->first) + ", bind(&LexParse::" + Iter->second + ",this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)));\n";
		}
		return std::move(Str);
	}

	//替换Action区域内的Alias
	unordered_map<string, string> ReplaceActionAlias(vector<pair<string, string>>& ActionList, unordered_map<string, string>& AliasMap)
	{
		for(auto& Iter : ActionList)
		{
			auto& FindIter = AliasMap.find(Iter.first);
			if(FindIter != AliasMap.end())
			{
				Iter.first = AliasMap[Iter.first];
			}
			else
			{
				Iter.first = DeleteHeadAndTailChar(Iter.first);
			}
		}
		return std::move(unordered_map<string, string>(ActionList.begin(), ActionList.end()));
	}
	// 获取别名到模式的pair
	vector<pair<string, string>> GetAliasStringPairList(string& TypeDefineStr)
	{
		vector<pair<string, string>> Result;
		auto SymbolList = FindAllSymbolPostion(TypeDefineStr);
		auto& DefinePairStrList = CutByDefineCharacter(TypeDefineStr, string(";"), '{', '}');
		//去除最后一个分号后面的内容
		DefinePairStrList.pop_back();
		for(auto& Iter : DefinePairStrList)
		{
			auto& PairList = CutByDefineCharacter(Iter, string(":"), '{', '}');
			Result.push_back(make_pair(Trim(PairList[0]), DeleteHeadAndTailChar(Trim(PairList[1]))));
		}

		return std::move(Result);
	}
	//去除字符串两端的一个字符
	string DeleteHeadAndTailChar(string& Target)
	{
		Target.pop_back();
		return std::move(Target.erase(0, 1));
	}
	//替换掉模式中的别名
	unordered_map<string, string> ReplaceAliasInRule(vector<pair<string, string>>& RuleList)
	{
		vector<string> NeedReplace;
		unordered_map<string, string> Result;
		for(auto i = 0; i < RuleList.size(); i++)
		{
			for(auto j = 0; j < i; j++)
			{
				auto& FindIter = "{" + RuleList[j].first + "}";
				auto FindIndex = RuleList[i].second.find(FindIter);
				if(FindIndex != RuleList[i].second.npos)
				{
					RuleList[i].second = replace_all_distinct(RuleList[i].second, FindIter, RuleList[j].second);
				}
			}
		}
		Result.insert(RuleList.begin(), RuleList.end());
		return std::move(Result);
	}
	//创建文件
	void CreateCppFile(string& FilePatch, string& TextContent)
	{
		ofstream Output(FilePatch);
		Output.write(TextContent.c_str(), TextContent.size());
	}
	//读取文件
	string ReadFileContent(string& FileName)
	{
		ifstream Input(FileName);
		istreambuf_iterator<char> beg(Input), end;
		string StrData(beg, end);
		Input.close();
		return move(StrData);
	}
	//判断指定字符是不是空白字符
	bool IsWhiteSpaceChar(char i)
	{
		return i == '\t' || i == '\n' || i == ' ';
	}

	//去除字符串两端空白
	string Trim(const string& Src)
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

	//清除指定区域内的内容,包括界定符
	string DeleteNoteSign(string& SrcStr, string& StartNote, string& EndNote)
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
	//获取标示符内的内容,不包括界定符号
	string GetNoteSignContent(string& SrcStr, string& StartNote, string& EndNote)
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
	//替换指定记号的内容.但是只替换一次.
	string ReplaceDefinePostion(string& Template, string& TemplateSign, string& RePalceData)
	{
		auto FindPos = Template.find(TemplateSign);
		auto FindNum = TemplateSign.size();
		return std::move(Template.replace(FindPos, FindNum, RePalceData));
	}
	//替换全部指定记号的内容

	string   replace_all_distinct(string&   str, const   string&   old_value, const   string&   new_value)
	{
		for(string::size_type pos(0); pos != string::npos; pos += new_value.length())
		{
			if(( pos = str.find(old_value, pos) ) != string::npos)
				str.replace(pos, old_value.length(), new_value);
			else   break;
		}
		return   move(str);
	}
	// 查看指定位置字符是否被转义
	bool HasChangedMean(string& SrcStr, int FindIter)
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

	//查看是否是字面值
	bool IsStrLiteral(string& SrcStr, int FindIter, vector<int> SymbolList)
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
			for(auto i = 0; i < SymbolList.size(); i++)
			{
				if(SymbolList[i] > FindIter)
				{
					return i % 2;
				}
			}
			return false;
		}
	}
	//查找所有非字面值引号位置
	vector<int> FindAllSymbolPostion(string& SrcStr, char Sign = '"')
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
	//获取最长嵌套内容匹配的最后一个字符索引
	int GetLongestNestedEndIndex(pair<char, char>& NestedSign, string& Src, int SrcIndex, vector<int>& SymbolList)
	{
		int StackList = 0;
		StackList = 1;
		auto StartIndex = SrcIndex;
		auto Length = 1;
		while(StackList != 0)
		{
			SrcIndex += 1;
			Length += 1;

			if(!IsStrLiteral(Src, SrcIndex, SymbolList))
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

	//字符串查找,跳过字面值

	int FindTrueCharIndex(string& SrcStr, char JumpSign, int StartIndex, vector<int>& SymbolList)
	{
		for(auto i = StartIndex; i < SrcStr.size(); i++)
		{
			if(SrcStr[i] == JumpSign)
			{
				if(!IsStrLiteral(SrcStr, i, SymbolList))
				{
					return i;
				}
			}
		}
		return -1;
	}
	//获取要跳过的符号对范围列表
	vector<pair<int, int>> GetNeedJumpList(string& SrcStr, string& DefineStr, char JumpStart, char JumpEnd, vector<int>& SymbolList)
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
				auto FindEndIndex = GetLongestNestedEndIndex(pair<char, char>(JumpStart, JumpEnd), SrcStr, FindStartIndex, SymbolList);
				Temp.second = FindEndIndex;
				JumpList.push_back(Temp);
				StartIndex = FindEndIndex;
			}
		}
		return std::move(JumpList);
	}

	//获取Str中某个字串所有内容的列表

	vector<int> GetNeedSignList(string& SrcStr, string& DefineStr, vector<int>& SymbolList)
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
	vector<int> GetNeedSignList(string& SrcStr, string& DefineStr, vector<pair<int, int>>& JumpList, vector<int>& SymbolList)
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
	//指定元素在范围索引列表里面
	template<typename T>
	//这里是因为右值的原因,FindIter是unsigned int类型,强转int后生成临时变量,不能绑定左值引用,所以这里要用T&& 绑定右值,延长生命周期
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
	//按指定字符分割字符串,如果指定字符前有\\,则忽视该处字符,如果字符被"直接包裹" 忽略字符,如果字符在jump区域内,忽略字符
	vector<string> CutByDefineCharacter(string& SrcStr, string& DefineStr, char JumpStart, char JumpEnd)
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
				if(i == 7)
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
	//按指定字符分割字符串,如果指定字符前有\\,则忽视该处字符,如果字符被"直接包裹" 忽略字符

	vector<string> CutByDefineCharacter(string& SrcStr, string& DefineStr, vector<int>&SymbolList)
	{
		vector<string> Result;

		auto SignList = GetNeedSignList(SrcStr, DefineStr, SymbolList);
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
private:
	string LexConfig;
	string LexTemplate;
};