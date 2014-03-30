#pragma once

/*
�����ַ�����
��
������
�������� ��:    (
��������  (:<name>
�ǲ����� (!
λ��ƥ��
Ԥ����λ��ƥ��
���� ^
��β $
���ʱ߽� \b
�ǵ��ʱ߽� \B
������  (?
����� <=
����� <!
�Ҳ��� >=
�Ҳ��� >!

�ظ�
̰��ƥ��
Ԥ�����ظ�
* �� +
�Զ����ظ�
{n,m} {n} {0,m} {m,0}
��̰��ƥ��
Ԥ�����ظ�
*? ?? +?
�Զ����ظ�
{n,m}? {n}? {,m}? {m,}?

�ַ���
�Զ����ַ�����
���� [ ]
���� [^ ]
Ԥ�����ַ�����
. \\W \\S \\D \\d \\s \\w
��ͨ�ַ�
ת���ַ� \\
��������  ${
ע�� ��#
ѡ�� |
ƽ����
(?(Name)A|B)
(?+Name)
(?-Name)

*/
/*
%%
��������
enum��RegexToken Tag
RegexToken��Ľṹ
LexParse��ṹ
�ɶԱ�ǩ��bimap

%%
����ƥ�䲿��

%%
�Զ��庯������

// Tag�� ƥ����� ���ɵ�RegexToken(Tag �ַ� ����λ�� )
*/
/*

*/
/*

*/

//%%
//������ͷ�ļ�����
#include <vector>
#include <algorithm>
#include <set>
#include <string>
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>
#include <iostream>
using namespace std;
//�������������� RegexToken���� enum���Ͷ������ﶨ��

//Tag��
enum class LexTag
{
	OtherChar,
	//��Ϊ��û�й���NFA��DFAȥ������,��������ֻ��Ӳ������
	NumberChar,
	RealWordChar,
	SimpleUnNamedCatch_Start,
	StringHead,
	StringTail,
	Mitipute_End,
	Closures_UnGreedy,
	PositiveClosures_UnGreedy,
	ChoseClosures_UnGreedy,
	Closures_Greedy,
	PositiveClosures_Greedy,
	ChoseClosures_Greedy,
	Repeat_Start,
	Repeat_And_BackRefer_End,
	Repeat_End_Greedy,
	CharSet_Start,
	CharSet_End,
	CharSet_Back_Start,
	AllCharSet,
	AllRealCharSet,
	AllSpaceCharSet,
	AllNumberCharSet,
	NoneAllRealCharSet,
	NoneAllSpaceCharSet,
	NoneAllNumberCharSet,
	ChangeMeanChar,
	ChoseSymbol,
	CharSetComponent,
	Comma,
};

//�������,ѡ��
class RegexToken
{
	string Data;
	int Index;//���ֵ�����
	LexTag Tag;
public:
	LexTag GetTag()
	{
		return Tag;
	}
	int GetIndex()
	{
		return Index;
	}
	string GetData()
	{
		return Data;
	}
	RegexToken(LexTag One, string& Two, int Three) :Tag(One), Data(Two), Index(Three)
	{
	}
};

/*
ע����Ҫ����

*/

//�ɶԳ��ֵı�ǩ:
/*

*/
class LexParse
{
public:
	bool StringBelongToRange(char CurrentChar, char start, char end)
	{
		return CurrentChar >= start && CurrentChar <= end;
	}

	//��Ϊû�й���NFA��DNAֻ������Ӳ������д��- -
	shared_ptr<RegexToken> GetCurrentNormalString(string& CurrentStr, int StartIndex)
	{
		if(StringBelongToRange(CurrentStr[0], 'a', 'z') || StringBelongToRange(CurrentStr[0], 'A', 'Z'))
		{
			return shared_ptr<RegexToken>(new RegexToken(LexTag::RealWordChar, CurrentStr, StartIndex));
		}
		else if(StringBelongToRange(CurrentStr[0], '0', '9'))
		{
			return shared_ptr<RegexToken>(new RegexToken(LexTag::NumberChar, CurrentStr, StartIndex));
		}
		else
		{
			return shared_ptr<RegexToken>(new RegexToken(LexTag::OtherChar, CurrentStr, StartIndex));
		}
	}

	LexParse(string& Target) : Pattern(Target), Length(Target.size())
	{
		initAll();

		TokenStream = LexParsing(Pattern);
	}
	//string DeleteNoteSign(string& SrcStr, string& StartNote, string& EndNote);
	//������ģ��
	vector<shared_ptr<RegexToken>> LexParsing(string& PatternStr, int LongestSize = 3, string& StartNote = string("(#"), string& EndNote = string(")"))
	{
		auto StartIndex = 0;
		//���ȶ�PatternStrԤ����,ȥ��ע��֮���
		PatternStr = DeleteNoteSign(PatternStr, StartNote, EndNote);
		if(PatternStr.back() != '$')
		{
			PatternStr.append("$");
		}
		//�������
		vector<shared_ptr<RegexToken>> ResultList;

		auto NextIndex = 0;
		auto Length = PatternStr.size();
		while(StartIndex < Length)
		{
			//��ƥ�䳤��4�Ĺؼ�������3 2 1,��1��ʱ�����ʧ��,�ͷ���NormalString
			for(int i = LongestSize; i >= 1; i--)
			{
				auto CurrentStr = Pattern.substr(StartIndex, i);
				if(RuleToTagList.find(CurrentStr) != RuleToTagList.end())
				{
					ResultList.push_back(ActionMap[RuleToTagList[CurrentStr]](StartIndex, std::ref(CurrentStr), std::ref(PatternStr)));
					break;
				}
				else if(i == 1)
				{
					ResultList.push_back(GetCurrentNormalString(CurrentStr, StartIndex));
					StartIndex += 1;
					break;
				}
			}
		}
		return move(ResultList);
	}
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
private:

	shared_ptr<RegexToken>CommaBody(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex += 1;
		if(StackList.back() == RuleToTagList[string("{")])
		{
			return std::move(shared_ptr<RegexToken>(new RegexToken(LexTag::Comma, CatchContent, ReStartIndex - 1)));
		}
		else
		{
			return std::move(shared_ptr<RegexToken>(new RegexToken(LexTag::OtherChar, CatchContent, ReStartIndex - 1)));
		}
	}
	shared_ptr<RegexToken> CharSetComponentBody(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex += 1;

		if(StackList.back() == RuleToTagList[string("[")])
		{
			return std::move(shared_ptr<RegexToken>(new RegexToken(LexTag::CharSetComponent, CatchContent, ReStartIndex - 1)));
		}
		else
		{
			return std::move(shared_ptr<RegexToken>(new RegexToken(LexTag::OtherChar, CatchContent, ReStartIndex - 1)));
		}
	}
	bool CheckStringHead(int ReStartIndex)
	{
		if(ReStartIndex == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	shared_ptr<RegexToken> StringHeadBody(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		if(CheckStringHead(ReStartIndex))
		{
			ReStartIndex += 1;
			return std::move(shared_ptr<RegexToken>(new RegexToken(LexTag::StringHead, CatchContent, ReStartIndex - 1)));
		}
		else
		{
			ReStartIndex += 1;
			return std::move(shared_ptr<RegexToken>(new RegexToken(LexTag::OtherChar, CatchContent, ReStartIndex - 1)));
		}
	}

	bool CheckStringTail(int ReStartIndex, string& PatternStr)
	{
		if(ReStartIndex == PatternStr.size() - 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	shared_ptr<RegexToken> StringTailBody(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		if(CheckStringTail(ReStartIndex, PatternStr))
		{
			ReStartIndex += 1;
			return std::move(shared_ptr<RegexToken>(new RegexToken(LexTag::StringTail, CatchContent, ReStartIndex - 1)));
		}
		else
		{
			ReStartIndex += 1;
			return std::move(shared_ptr<RegexToken>(new RegexToken(LexTag::OtherChar, CatchContent, ReStartIndex - 1)));
		}
	}
	string GetOneCharBuff(char Char)
	{
		return std::move(string(1, Char));
	}
	shared_ptr<RegexToken> ChangeMeanCharBody(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex += 2;
		return std::move(shared_ptr<RegexToken>(new RegexToken(LexTag::OtherChar, GetOneCharBuff(Pattern[ReStartIndex - 1]), ReStartIndex - 1)));
	}

	shared_ptr<RegexToken>SimpleUnNamedCatch_Start(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		StackList.push_back(RuleToTagList["("]);
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::SimpleUnNamedCatch_Start, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>StringHead(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		return std::move(StringHeadBody(ReStartIndex, CatchContent, PatternStr));
	}
	shared_ptr<RegexToken>StringTail(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		return std::move(StringTailBody(ReStartIndex, CatchContent, PatternStr));
	}
	shared_ptr<RegexToken>Mitipute_End(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		if(StartToEndList[StackList.back()] == RuleToTagList[string(")")])
		{
			StackList.pop_back();
		}
		else
		{
			abort();
		}
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::Mitipute_End, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>Closures_UnGreedy(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::Closures_UnGreedy, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>PositiveClosures_UnGreedy(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::PositiveClosures_UnGreedy, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>ChoseClosures_UnGreedy(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::ChoseClosures_UnGreedy, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>Closures_Greedy(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::Closures_Greedy, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>PositiveClosures_Greedy(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::PositiveClosures_Greedy, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>ChoseClosures_Greedy(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::ChoseClosures_Greedy, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>Repeat_Start(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		StackList.push_back(RuleToTagList["{"]);
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::Repeat_Start, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>Repeat_And_BackRefer_End(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		if(StartToEndList[StackList.back()] == RuleToTagList[string("}")])
		{
			StackList.pop_back();
		}
		else
		{
			abort();
		}
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::Repeat_And_BackRefer_End, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>Repeat_End_Greedy(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		if(StartToEndList[StackList.back()] == RuleToTagList[string("}?")])
		{
			StackList.pop_back();
		}
		else
		{
			abort();
		}
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::Repeat_End_Greedy, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>CharSet_Start(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		StackList.push_back(RuleToTagList["["]);
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::CharSet_Start, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>CharSet_End(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		if(StartToEndList[StackList.back()] == RuleToTagList[string("]")])
		{
			StackList.pop_back();
		}
		else
		{
			abort();
		}
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::CharSet_End, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>CharSet_Back_Start(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		StackList.push_back(RuleToTagList["[^"]);
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::CharSet_Back_Start, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>AllCharSet(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::AllCharSet, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>AllRealCharSet(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::AllRealCharSet, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>AllSpaceCharSet(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::AllSpaceCharSet, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>AllNumberCharSet(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::AllNumberCharSet, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>NoneAllRealCharSet(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::NoneAllRealCharSet, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>NoneAllSpaceCharSet(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::NoneAllSpaceCharSet, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>NoneAllNumberCharSet(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::NoneAllNumberCharSet, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>ChangeMeanChar(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		return std::move(ChangeMeanCharBody(ReStartIndex, CatchContent, PatternStr));
	}
	shared_ptr<RegexToken>ChoseSymbol(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		ReStartIndex = ReStartIndex + CatchContent.size();
		return shared_ptr<RegexToken>(new RegexToken(LexTag::ChoseSymbol, CatchContent, ReStartIndex - CatchContent.size()));
	}
	shared_ptr<RegexToken>CharSetComponent(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		return std::move(CharSetComponentBody(ReStartIndex, CatchContent, PatternStr));
	}
	shared_ptr<RegexToken>Comma(int& ReStartIndex, string& CatchContent, string& PatternStr)
	{
		return std::move(CommaBody(ReStartIndex, CatchContent, PatternStr));
	}

private:
	void initAll()
	{
		initActionMap();
		initTagToRule();
		initRuleToTag();
		initStartToEnd();
		initEndToStart();
	}

	void initTagToRule()
	{
		TagToRuleList.insert(make_pair(LexTag::SimpleUnNamedCatch_Start, "("));
		TagToRuleList.insert(make_pair(LexTag::StringHead, "^"));
		TagToRuleList.insert(make_pair(LexTag::StringTail, "$"));
		TagToRuleList.insert(make_pair(LexTag::Mitipute_End, ")"));
		TagToRuleList.insert(make_pair(LexTag::Closures_UnGreedy, "*"));
		TagToRuleList.insert(make_pair(LexTag::PositiveClosures_UnGreedy, "+"));
		TagToRuleList.insert(make_pair(LexTag::ChoseClosures_UnGreedy, "?"));
		TagToRuleList.insert(make_pair(LexTag::Closures_Greedy, "*?"));
		TagToRuleList.insert(make_pair(LexTag::PositiveClosures_Greedy, "+?"));
		TagToRuleList.insert(make_pair(LexTag::ChoseClosures_Greedy, "??"));
		TagToRuleList.insert(make_pair(LexTag::Repeat_Start, "{"));
		TagToRuleList.insert(make_pair(LexTag::Repeat_And_BackRefer_End, "}"));
		TagToRuleList.insert(make_pair(LexTag::Repeat_End_Greedy, "}?"));
		TagToRuleList.insert(make_pair(LexTag::CharSet_Start, "["));
		TagToRuleList.insert(make_pair(LexTag::CharSet_End, "]"));
		TagToRuleList.insert(make_pair(LexTag::CharSet_Back_Start, "[^"));
		TagToRuleList.insert(make_pair(LexTag::AllCharSet, "."));
		TagToRuleList.insert(make_pair(LexTag::AllRealCharSet, "\\w"));
		TagToRuleList.insert(make_pair(LexTag::AllSpaceCharSet, "\\s"));
		TagToRuleList.insert(make_pair(LexTag::AllNumberCharSet, "\\d"));
		TagToRuleList.insert(make_pair(LexTag::NoneAllRealCharSet, "\\W"));
		TagToRuleList.insert(make_pair(LexTag::NoneAllSpaceCharSet, "\\S"));
		TagToRuleList.insert(make_pair(LexTag::NoneAllNumberCharSet, "\\d"));
		TagToRuleList.insert(make_pair(LexTag::ChangeMeanChar, "\\"));
		TagToRuleList.insert(make_pair(LexTag::ChoseSymbol, "|"));
		TagToRuleList.insert(make_pair(LexTag::CharSetComponent, "-"));
		TagToRuleList.insert(make_pair(LexTag::Comma, ","));
	}
	void initRuleToTag()
	{
		RuleToTagList.insert(make_pair("(", LexTag::SimpleUnNamedCatch_Start));
		RuleToTagList.insert(make_pair("^", LexTag::StringHead));
		RuleToTagList.insert(make_pair("$", LexTag::StringTail));
		RuleToTagList.insert(make_pair(")", LexTag::Mitipute_End));
		RuleToTagList.insert(make_pair("*", LexTag::Closures_UnGreedy));
		RuleToTagList.insert(make_pair("+", LexTag::PositiveClosures_UnGreedy));
		RuleToTagList.insert(make_pair("?", LexTag::ChoseClosures_UnGreedy));
		RuleToTagList.insert(make_pair("*?", LexTag::Closures_Greedy));
		RuleToTagList.insert(make_pair("+?", LexTag::PositiveClosures_Greedy));
		RuleToTagList.insert(make_pair("??", LexTag::ChoseClosures_Greedy));
		RuleToTagList.insert(make_pair("{", LexTag::Repeat_Start));
		RuleToTagList.insert(make_pair("}", LexTag::Repeat_And_BackRefer_End));
		RuleToTagList.insert(make_pair("}?", LexTag::Repeat_End_Greedy));
		RuleToTagList.insert(make_pair("[", LexTag::CharSet_Start));
		RuleToTagList.insert(make_pair("]", LexTag::CharSet_End));
		RuleToTagList.insert(make_pair("[^", LexTag::CharSet_Back_Start));
		RuleToTagList.insert(make_pair(".", LexTag::AllCharSet));
		RuleToTagList.insert(make_pair("\\w", LexTag::AllRealCharSet));
		RuleToTagList.insert(make_pair("\\s", LexTag::AllSpaceCharSet));
		RuleToTagList.insert(make_pair("\\d", LexTag::AllNumberCharSet));
		RuleToTagList.insert(make_pair("\\W", LexTag::NoneAllRealCharSet));
		RuleToTagList.insert(make_pair("\\S", LexTag::NoneAllSpaceCharSet));
		RuleToTagList.insert(make_pair("\\d", LexTag::NoneAllNumberCharSet));
		RuleToTagList.insert(make_pair("\\", LexTag::ChangeMeanChar));
		RuleToTagList.insert(make_pair("|", LexTag::ChoseSymbol));
		RuleToTagList.insert(make_pair("-", LexTag::CharSetComponent));
		RuleToTagList.insert(make_pair(",", LexTag::Comma));
	}

	void initStartToEnd()
	{
		StartToEndList.insert(make_pair(RuleToTagList["("], RuleToTagList[")"]));
		StartToEndList.insert(make_pair(RuleToTagList["{"], RuleToTagList["}"]));
		StartToEndList.insert(make_pair(RuleToTagList["{"], RuleToTagList["}?"]));
		StartToEndList.insert(make_pair(RuleToTagList["["], RuleToTagList["]"]));
		StartToEndList.insert(make_pair(RuleToTagList["[^"], RuleToTagList["]"]));
	}
	void initEndToStart()
	{
		EndToStartList.insert(make_pair(RuleToTagList[")"], RuleToTagList["("]));
		EndToStartList.insert(make_pair(RuleToTagList["}"], RuleToTagList["{"]));
		EndToStartList.insert(make_pair(RuleToTagList["}?"], RuleToTagList["{"]));
		EndToStartList.insert(make_pair(RuleToTagList["]"], RuleToTagList["["]));
		EndToStartList.insert(make_pair(RuleToTagList["]"], RuleToTagList["[^"]));
	}
	void initActionMap()
	{
		ActionMap.insert(make_pair(LexTag::SimpleUnNamedCatch_Start, std::bind(&LexParse::SimpleUnNamedCatch_Start, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::StringHead, std::bind(&LexParse::StringHead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::StringTail, std::bind(&LexParse::StringTail, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::Mitipute_End, std::bind(&LexParse::Mitipute_End, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::Closures_UnGreedy, std::bind(&LexParse::Closures_UnGreedy, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::PositiveClosures_UnGreedy, std::bind(&LexParse::PositiveClosures_UnGreedy, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::ChoseClosures_UnGreedy, std::bind(&LexParse::ChoseClosures_UnGreedy, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::Closures_Greedy, std::bind(&LexParse::Closures_Greedy, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::PositiveClosures_Greedy, std::bind(&LexParse::PositiveClosures_Greedy, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::ChoseClosures_Greedy, std::bind(&LexParse::ChoseClosures_Greedy, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::Repeat_Start, std::bind(&LexParse::Repeat_Start, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::Repeat_And_BackRefer_End, std::bind(&LexParse::Repeat_And_BackRefer_End, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::Repeat_End_Greedy, std::bind(&LexParse::Repeat_End_Greedy, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::CharSet_Start, std::bind(&LexParse::CharSet_Start, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::CharSet_End, std::bind(&LexParse::CharSet_End, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::CharSet_Back_Start, std::bind(&LexParse::CharSet_Back_Start, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::AllCharSet, std::bind(&LexParse::AllCharSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::AllRealCharSet, std::bind(&LexParse::AllRealCharSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::AllSpaceCharSet, std::bind(&LexParse::AllSpaceCharSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::AllNumberCharSet, std::bind(&LexParse::AllNumberCharSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::NoneAllRealCharSet, std::bind(&LexParse::NoneAllRealCharSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::NoneAllSpaceCharSet, std::bind(&LexParse::NoneAllSpaceCharSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::NoneAllNumberCharSet, std::bind(&LexParse::NoneAllNumberCharSet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::ChangeMeanChar, std::bind(&LexParse::ChangeMeanChar, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::ChoseSymbol, std::bind(&LexParse::ChoseSymbol, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::CharSetComponent, std::bind(&LexParse::CharSetComponent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		ActionMap.insert(make_pair(LexTag::Comma, std::bind(&LexParse::Comma, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	}
public:
	vector<shared_ptr<RegexToken>>& ReturnTokenStream()
	{
		return TokenStream;
	}
private:
	//��ǩ��������ӳ��
	//int&�Ǵ��������λ��.���뺯���������µ�����λ��
	//	unordered_map<LexTag, function<shared_ptr<RegexToken>(int& ReStartIndex,string& CatchContent,string& PatternStr)>> ActionMap;
	unordered_map<LexTag, function<shared_ptr<RegexToken>(int&, string&, string&)>> ActionMap;
	//��ǩ������Ļ���ӳ��
	unordered_map<LexTag, string> TagToRuleList;
	unordered_map<string, LexTag> RuleToTagList;
	//�ɶԱ�ǩ֮��Ļ���ӳ��
	unordered_map<LexTag, LexTag> StartToEndList;
	unordered_multimap<LexTag, LexTag> EndToStartList;
	//��ĸ��������ӳ��
	//ò����ʱ�ò���
	//unordered_map<char, int>CharMap;

	//��ĸ���ַ���Χ��ӳ��

	string Pattern;
	int Length;
	//Ƕ�ױ�ǩջ
	//vector<LexTag>StackList;
	vector<LexTag>StackList;
public:
	vector<shared_ptr<RegexToken>> TokenStream;
};