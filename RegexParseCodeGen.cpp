#include "RegexParseCodeGen.h"
string RegexParseCodeGen::ReadFileContent(string& FileName)
{
	ifstream Input(FileName);
	istreambuf_iterator<char> beg(Input), end;
	string StrData(beg, end);
	Input.close();
	return move(StrData);
}
vector<string> RegexParseCodeGen::CutByDefineCharacter(string& SrcStr, string& DefineStr)
{
	auto FindIter = 0;
	vector<string> Result;
	auto OldIter = 0;
	while(OldIter < SrcStr.size() && FindIter != SrcStr.npos)
	{
		FindIter = SrcStr.find(DefineStr, OldIter);
		if (FindIter != SrcStr.npos)
		{
			if(FindIter != 0 && SrcStr[FindIter - 1] == '\\')
			{
				OldIter = FindIter + DefineStr.size();
			}
			else
			{
				Result.push_back(SrcStr.substr(OldIter, FindIter - OldIter));
				OldIter = FindIter + DefineStr.size();
			}
		}
	}
	Result.push_back(SrcStr.substr(OldIter));
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
			NewStr = NewStr + SrcStr.substr(PairList[i] + StartNoteSize, ( PairList[i + 1] ) - ( PairList[i] + StartNoteSize ));
		}
	}
	else
	{
		NewStr = SrcStr;
	}
	return std::move(NewStr);
}

string RegexParseCodeGen::Trim(string& Src)
{
	auto StartIndex = 0;
	auto EndIndex = 0;
 	for(auto i = 0; i < Src.size(); i++)
	{
		auto& Iter = Src[i];
		if(Iter == '\t' || Iter == '\n' || Iter == ' ')
		{
			continue;
		}
		else
		{
			StartIndex = i;
			break;
		}
	}
	Src = Src.substr(StartIndex);
	for(int i = Src.size() - 1; i >= 0; i--)
	{
		auto& Iter = Src[i];
		if(Iter == '\t' || Iter == '\n' || Iter == ' ')
		{
			continue;
		}
		else
		{
			EndIndex = i + 1;
			break;
		}
	}

	return std::move(Src.substr(0, EndIndex));
}