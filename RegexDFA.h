#pragma once
#include "RegexParse.h"
#include <fstream>
using std::ofstream;
class FigureNode
{
public:
	set<int> FollowPos;

	set<int> FirstPos;
	set<int> LastPos;
	bool Nullable;
};

class FA
{
public:
	virtual void VisitSpecNode(Repeat* NodePtr, int CurrentIndex) = 0;
	virtual void VisitSpecNode(NormalChar* NodePtr, int CurrentIndex) = 0;
	virtual void VisitSpecNode(Link* NodePtr, int CurrentIndex) = 0;
	virtual void VisitSpecNode(ChoseSymbol* NodePtr, int CurrentIndex) = 0;
	virtual void VisitSpecNode(CharSet* NodePtr, int CurrentIndex) = 0;
	virtual void VisitSpecNode(StringTail* NodePtr, int CurrentIndex) = 0;
	virtual void VisitSpecNode(Nullable* NodePtr, int CurrentIndex) = 0;

	virtual  ~FA()
	{
	}
};
struct MatchInfo
{
	string MatchContent;
	int StartIndex;
	int EndIndex;
	MatchInfo(string tMatchContent, int tStartIndex, int tEndIndex) :MatchContent(tMatchContent), StartIndex(tStartIndex), EndIndex(tEndIndex)
	{

	}
};

class NfaNode
{
public:
	unordered_map<int, int>NextMap;
	set<int> NullNextMap;
};
class NFA :public FA
{
private:
	vector<int> CharMap;
	AstNode* AstRoot;
	int AstRootIndex;
	vector<AstNode*>AstNodeList;

	//nfa接受状态的index.
	int AcceptStautsIndex;
	//nfa 开始状态的index
	int StartStautsIndex;
	//int是AST节点的索引,pair是该索引对应的nfa节点链的开始和结束状态
	unordered_map<int, pair<int, int>> NfaNodeMap;
	vector<NfaNode*> NfaNodeList;
public:
	NFA(vector<int>& tCharMap, AstNode* tAstRoot, int Index, vector<AstNode*>& tAstNodeList) :CharMap(tCharMap), AstRoot(tAstRoot), AstRootIndex(Index), AstNodeList(tAstNodeList)
	{
		initNFAStuats();
	}
	NFA() = delete;
	~NFA()
	{
	}

private:
	void initNFAStuats()
	{
		CreatNFA(AstRootIndex);
		auto Pair = GetNodeStartAndEnd(AstRootIndex);
		StartStautsIndex = Pair.first;
		AcceptStautsIndex = Pair.second;
	}
	void CreatNFA(int CurrentIndex)
	{
		if(CurrentIndex == -1)
		{
			return;
		}

		if(HasLeftNode(CurrentIndex))
		{
			CreatNFA(AstNodeList[CurrentIndex]->LeftNodeIndex);
		}
		if(HasRightNode(CurrentIndex))
		{
			CreatNFA(AstNodeList[CurrentIndex]->RightNodeIndex);
		}
		//后缀遍历
		AstNodeList[CurrentIndex]->Accept(*this, CurrentIndex);
	}
	void CreatNFA(Repeat* NodePtr, int CurrentIndex)
	{
		auto StartIndex = CreatOneNode();
		auto EndIndex = CreatOneNode();
		//获取重复次数
		auto& StartNumber = NodePtr->RepeatNumber.first;
		auto& EndNumber = NodePtr->RepeatNumber.second;
		//获得是不是贪婪匹配
		//GreedyType IsGreedy = NodePtr->Greedy;
		auto ChildStauts = NfaNodeMap[NodePtr->LeftNodeIndex];
		vector<pair<int, int>>NewStautsList;
		NewStautsList.push_back(ChildStauts);
		if(EndNumber == INT_MAX)
		{
			for(size_t i = 1; i <= StartNumber; i++)
			{
				NewStautsList.push_back(CreatIsomorphismGraph(ChildStauts));
			}
			GetNfaNodePtr(EndIndex)->NullNextMap.insert(StartIndex);
		}
		else if(EndNumber == 0)
		{
			abort();
		}
		else
		{
			for(size_t i = 1; i <= EndNumber; i++)
			{
				NewStautsList.push_back(CreatIsomorphismGraph(ChildStauts));
			}
			if(StartNumber == 0)
			{
				auto StartNodePtr = GetNfaNodePtr(StartIndex);
				StartNodePtr->NullNextMap.insert(EndIndex);
			}
			else if(StartNumber == INT_MAX)
			{
				abort();
			}
			else
			{
				for(auto Index = StartNumber - 1; Index <= EndNumber - 1; Index++)
				{
					auto CurrentNodePtr = GetNfaNodePtr(NewStautsList[Index].second);
					CurrentNodePtr->NullNextMap.insert(EndIndex);
				}
			}
		}
		for(auto i = 0; i < NewStautsList.size() - 1; i++)
		{
			GetNfaNodePtr(NewStautsList[i].second)->NullNextMap.insert(NewStautsList[i + 1].first);
		}
		GetNfaNodePtr(StartIndex)->NullNextMap.insert(NewStautsList.front().first);
	}
	void CreatNFA(NormalChar* NodePtr, int CurrentIndex)
	{
		auto StartIndex = CreatOneNode();
		auto EndIndex = CreatOneNode();
		GetNfaNodePtr(StartIndex)->NextMap.insert(make_pair(CharMap[NodePtr->Val], EndIndex));
		this->NfaNodeMap.insert(make_pair(CurrentIndex, pair<int, int>(StartIndex, EndIndex)));
	}
	void CreatNFA(Link* NodePtr, int CurrentIndex)
	{
		auto LeftIndexPair = GetNodeStartAndEnd(NodePtr->LeftNodeIndex);
		auto RightIndexPair = GetNodeStartAndEnd(NodePtr->RightNodeIndex);
		*GetNfaNodePtr(LeftIndexPair.second) = *GetNfaNodePtr(RightIndexPair.first);
		this->NfaNodeMap.insert(make_pair(CurrentIndex, pair<int, int>(LeftIndexPair.first, RightIndexPair.second)));
	}
	void CreatNFA(ChoseSymbol* NodePtr, int CurrentIndex)
	{
		auto LeftIndexPair = GetNodeStartAndEnd(NodePtr->LeftNodeIndex);
		auto RightIndexPair = GetNodeStartAndEnd(NodePtr->RightNodeIndex);
		auto StartIndex = CreatOneNode();
		auto EndIndex = CreatOneNode();
		GetNfaNodePtr(StartIndex)->NullNextMap.insert(LeftIndexPair.first);
		GetNfaNodePtr(StartIndex)->NullNextMap.insert(RightIndexPair.first);
		GetNfaNodePtr(LeftIndexPair.second)->NullNextMap.insert(EndIndex);
		GetNfaNodePtr(RightIndexPair.second)->NullNextMap.insert(EndIndex);
		this->NfaNodeMap.insert(make_pair(CurrentIndex, pair<int, int>(StartIndex, EndIndex)));
	}
	void CreatNFA(Nullable* NodePtr, int CurrentIndex)
	{
	}
	void CreatNFA(CharSet* NodePtr, int CurrentIndex)
	{
		auto StartIndex = CreatOneNode();
		auto EndIndex = CreatOneNode();
		auto StartNodePtr = GetNfaNodePtr(StartIndex);
		for(auto Iter = NodePtr->CharSetRange.begin(); Iter != NodePtr->CharSetRange.end(); Iter++)
		{
			if(Iter->first == Iter->second)
			{
				//说明是单个字符
				StartNodePtr->NextMap.insert(make_pair(CharMap[Iter->first], EndIndex));
			}
			else
			{
				for(int i = Iter->first; i <= Iter->second; i++)
				{
					auto FindIter = StartNodePtr->NextMap.find(CharMap[i]);
					if(FindIter == StartNodePtr->NextMap.end())
					{
						StartNodePtr->NextMap.insert(make_pair(CharMap[i], EndIndex));
					}
				}
			}
		}
		this->NfaNodeMap.insert(make_pair(CurrentIndex, pair<int, int>(StartIndex, EndIndex)));
	}
	void CreatNFA(StringTail* NodePtr, int CurrentIndex)
	{
		auto StartIndex = CreatOneNode();
		auto EndIndex = CreatOneNode();
		GetNfaNodePtr(StartIndex)->NullNextMap.insert(EndIndex);
		this->NfaNodeMap.insert(make_pair(CurrentIndex, pair<int, int>(StartIndex, EndIndex)));
	}
	NfaNode* GetNfaNodePtr(int NodeIndex)
	{
		return NfaNodeList[NodeIndex];
	}
	pair<int, int> GetNodeStartAndEnd(int AstNodeIndex)
	{
		return std::move(pair<int, int>(NfaNodeMap[AstNodeIndex].first, NfaNodeMap[AstNodeIndex].second));
	}
	int CreatOneNode()
	{
		int Index = NfaNodeList.size();
		NfaNodeList.push_back(new NfaNode());
		return Index;
	}

	bool IsLeaf(int CurrentIndex)
	{
		return !HasLeftNode(CurrentIndex) && !HasRightNode(CurrentIndex);
	}

	bool HasLeftNode(int CurrentIndex)
	{
		return AstNodeList[CurrentIndex]->LeftNodeIndex != -1;
	}
	bool HasRightNode(int CurrentIndex)
	{
		return AstNodeList[CurrentIndex]->RightNodeIndex != -1;
	}
	//拷贝一个NFA状态图子图的结构
	pair<int, int> CreatIsomorphismGraph(pair<int, int>StautsSet)
	{
		//老新节点映射
		map<int, int> StautsMap;
		list<int> Stack;
		Stack.push_back(StautsSet.first);
		auto NewNode = CreatOneNode();
		StautsMap.insert(make_pair(StautsSet.first, NewNode));
		while(!Stack.empty())
		{
			auto CurrentStauts = Stack.front();
			Stack.pop_front();
			auto OldNodePtr = GetNfaNodePtr(CurrentStauts);
			auto NewNodePtr = GetNfaNodePtr(StautsMap[CurrentStauts]);
			*NewNodePtr = *OldNodePtr;
			for(auto& Iter = NewNodePtr->NextMap.begin(); Iter != NewNodePtr->NextMap.end(); Iter++)
			{
				auto& FindIter = StautsMap.find(Iter->second);
				if(FindIter != StautsMap.end())
				{
					//该旧节点有新节点映射
					Iter->second = StautsMap[Iter->second];
				}
				else
				{
					auto NewIndex = CreatOneNode();
					StautsMap.insert(make_pair(Iter->second, NewIndex));
					Iter->second = NewIndex;
					Stack.push_back(NewIndex);
				}
			}
			for(auto& Iter = NewNodePtr->NullNextMap.begin(); Iter != NewNodePtr->NullNextMap.end();)
			{
				auto& FindIter = StautsMap.find(*Iter);
				if(FindIter != StautsMap.end())
				{
					//该旧节点有新节点映射
					auto NewIndex = StautsMap[*Iter];
					Iter = NewNodePtr->NullNextMap.erase(Iter);
					NewNodePtr->NullNextMap.insert(NewIndex);
				}
				else
				{
					auto NewIndex = CreatOneNode();
					StautsMap.insert(make_pair(*Iter, NewIndex));
					Iter = NewNodePtr->NullNextMap.erase(Iter);
					NewNodePtr->NullNextMap.insert(NewIndex);
					Stack.push_back(NewIndex);
				}
			}
		}
		return std::move(pair<int, int>(StautsMap[StautsSet.first], StautsMap[StautsSet.second]));
	}
public:

	void VisitSpecNode(Repeat* NodePtr, int CurrentIndex)
	{
		CreatNFA(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(NormalChar* NodePtr, int CurrentIndex)
	{
		CreatNFA(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(Link* NodePtr, int CurrentIndex)
	{
		CreatNFA(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(ChoseSymbol* NodePtr, int CurrentIndex)
	{
		CreatNFA(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(CharSet* NodePtr, int CurrentIndex)
	{
		CreatNFA(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(StringTail* NodePtr, int CurrentIndex)
	{
		CreatNFA(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(Nullable* NodePtr, int CurrentIndex)
	{
		CreatNFA(NodePtr, CurrentIndex);
	}
};
class DFA :public FA
{
private:
	vector<int> CharMap;
	AstNode* AstRoot;
	int AstRootIndex;
	map<int, FigureNode>FirgueMap;
	vector<AstNode*>AstNodeList;
	//接受状态的index.
	int AcceptIndex;

	//CharMap的最后一个索引
	int CharEndIndex;

	unordered_map<string, unordered_map<int, string>>DfaMap;
	string DfaStart;
public:

	//打印 DfaMap;
	void PrintDfaMap()
	{
		ofstream OutPut("info.txt");
		OutPut << "DfaMap:" << std::endl;
		for(auto Iter = DfaMap.begin(); Iter != DfaMap.end();Iter++)
		{
			OutPut << "Stauts: \"" << Iter->first << "\"" << std::endl;
			OutPut << "Jump:";
			for(auto JumpIter = Iter->second.begin(); JumpIter != Iter->second.end();JumpIter++)
			{
				OutPut << JumpIter->first << " \"" << JumpIter->second << "\"" << std::endl;
			}
		}
	}

	DFA(vector<int>& tCharMap, AstNode* tAstRoot, int Index, vector<AstNode*>& tAstNodeList, int tCharEndIndex) :CharMap(tCharMap), AstRoot(tAstRoot), AstRootIndex(Index), AstNodeList(tAstNodeList), CharEndIndex(tCharEndIndex)
	{
		initFigureMap(AstRootIndex);
		CreatDFA();
		MinningDfa();
		//PrintDfaMap();
	}
	DFA() = delete;
	~DFA()
	{
	}
	bool RunDfa(string& Text)
	{
		string End = to_string(AcceptIndex);
		auto CurrentStauts = DfaStart;
		for(auto j = 0; j < Text.size(); j++)
		{
			for(auto i = j; i < Text.size(); i++)
			{
				if(DfaMap[CurrentStauts].find(CharMap[Text[i]]) == DfaMap[CurrentStauts].end())
				{
					i = Text.size();
				}
				else
				{
					CurrentStauts = DfaMap[CurrentStauts][CharMap[Text[i]]];
				}
				if(CurrentStauts.find(End) != CurrentStauts.npos)
				{
					return true;
				}
			}
		}
		return false;
	}
	void FigureNodeProperty(Repeat* NodePtr, int CurrentIndex)
	{
		//cout << "FigureNodeProperty(Repeat* NodePtr, int CurrentIndex)" << endl;
		auto CurrentTag = AstNodeList[CurrentIndex]->Tag;
		FigureNode Figure;
		auto CurrentNodePtr = dynamic_cast<Repeat*>( AstNodeList[CurrentIndex] );
		auto& LeftNodeIter = FirgueMap.find(AstNodeList[CurrentIndex]->LeftNodeIndex);
		auto& LeftFigure = LeftNodeIter->second;

		//获取子节点的开始状态和结束状态

		//获取重复次数
		auto& StartNumber = NodePtr->RepeatNumber.first;
		auto& EndNumber = NodePtr->RepeatNumber.second;
		//获得是不是贪婪匹配
		GreedyType IsGreedy = NodePtr->Greedy;

		if(LeftNodeIter != FirgueMap.end())
		{
			//first和second计算
			Figure.FirstPos.insert(LeftFigure.FirstPos.begin(), LeftFigure.FirstPos.end());
			Figure.LastPos.insert(LeftFigure.LastPos.begin(), LeftFigure.LastPos.end());
		}

		//Nullable计算
		if(CurrentNodePtr->RepeatNumber.first == 0)
		{
			Figure.Nullable = true;
		}
		else
		{
			Figure.Nullable = false;
		}

		//Follow计算
		for(auto Iter = Figure.LastPos.begin(); Iter != Figure.LastPos.end(); Iter++)
		{
			FirgueMap[*Iter].FollowPos.insert(Figure.FirstPos.begin(), Figure.FirstPos.end());
		}
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}
	void FigureNodeProperty(NormalChar* NodePtr, int CurrentIndex)
	{
		//cout << "FigureNodeProperty(NormalChar* NodePtr, int CurrentIndex)" << endl;
		FigureNode Figure;
		Figure.Nullable = false;
		Figure.FirstPos.insert(CurrentIndex);
		Figure.LastPos.insert(CurrentIndex);
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}
	void FigureNodeProperty(Link* NodePtr, int CurrentIndex)
	{
		//cout << " FigureNodeProperty(Link* NodePtr, int CurrentIndex)" << endl;
		FigureNode Figure;
		auto LeftIndex = AstNodeList[CurrentIndex]->LeftNodeIndex;
		auto RightIndex = AstNodeList[CurrentIndex]->RightNodeIndex;
		auto& FindLeftIter = FirgueMap.find(LeftIndex);
		auto& FindRightIter = FirgueMap.find(RightIndex);
		if(FindLeftIter != FirgueMap.end() && FindRightIter != FirgueMap.end())
		{
			auto& LeftFigure = FindLeftIter->second;
			auto& RightFigure = FindRightIter->second;
			Figure.Nullable = LeftFigure.Nullable && RightFigure.Nullable;
			if(LeftFigure.Nullable)
			{
				set_union(LeftFigure.FirstPos.begin(), LeftFigure.FirstPos.end(), RightFigure.FirstPos.begin(), RightFigure.FirstPos.end(), inserter(Figure.FirstPos, Figure.FirstPos.begin()));
			}
			else
			{
				Figure.FirstPos = LeftFigure.FirstPos;
			}
			if(RightFigure.Nullable)
			{
				set_union(LeftFigure.LastPos.begin(), LeftFigure.LastPos.end(), RightFigure.LastPos.begin(), RightFigure.LastPos.end(), inserter(Figure.LastPos, Figure.LastPos.begin()));
			}
			else
			{
				Figure.LastPos = RightFigure.LastPos;
			}
			for(auto Iter = LeftFigure.LastPos.begin(); Iter != LeftFigure.LastPos.end(); Iter++)
			{
				FirgueMap[*Iter].FollowPos.insert(RightFigure.FirstPos.begin(), RightFigure.FirstPos.end());
			}
		}
		else
		{
			abort();
		}

		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}
	void FigureNodeProperty(ChoseSymbol* NodePtr, int CurrentIndex)
	{
		//cout << " FigureNodeProperty(ChoseSymbol* NodePtr, int CurrentIndex)" << endl;
		FigureNode Figure;
		auto LeftIndex = AstNodeList[CurrentIndex]->LeftNodeIndex;
		auto RightIndex = AstNodeList[CurrentIndex]->RightNodeIndex;
		auto& FindLeftIter = FirgueMap.find(LeftIndex);
		auto& FindRightIter = FirgueMap.find(RightIndex);
		if(FindLeftIter != FirgueMap.end() && FindRightIter != FirgueMap.end())
		{
			auto& LeftFigure = FindLeftIter->second;
			auto& RightFigure = FindRightIter->second;
			Figure.Nullable = LeftFigure.Nullable || RightFigure.Nullable;

			set_union(LeftFigure.FirstPos.begin(), LeftFigure.FirstPos.end(), RightFigure.FirstPos.begin(), RightFigure.FirstPos.end(), inserter(Figure.FirstPos, Figure.FirstPos.begin()));

			set_union(LeftFigure.LastPos.begin(), LeftFigure.LastPos.end(), RightFigure.LastPos.begin(), RightFigure.LastPos.end(), inserter(Figure.LastPos, Figure.LastPos.begin()));
		}
		else
		{
			abort();
		}
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}
	void FigureNodeProperty(CharSet* NodePtr, int CurrentIndex)
	{
		//cout << " FigureNodeProperty(CharSet* NodePtr, int CurrentIndex)" << endl;

		FigureNode Figure;
		Figure.Nullable = false;
		Figure.FirstPos.insert(CurrentIndex);
		Figure.LastPos.insert(CurrentIndex);
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}
	void FigureNodeProperty(StringTail* NodePtr, int CurrentIndex)
	{
		//cout << "FigureNodeProperty(StringTail* NodePtr, int CurrentIndex)" << endl;

		FigureNode Figure;
		AcceptIndex = CurrentIndex;
		Figure.Nullable = false;
		Figure.FirstPos.insert(CurrentIndex);
		Figure.LastPos.insert(CurrentIndex);
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}
	void FigureNodeProperty(Nullable* NodePtr, int CurrentIndex)
	{
		//cout << "FigureNodeProperty(Nullable* NodePtr, int CurrentIndex)" << endl;
		FigureNode Figure;
		Figure.Nullable = true;
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}

	void VisitSpecNode(Repeat* NodePtr, int CurrentIndex)
	{
		//先调整子树结构
		// Old节点到新节点的映射
		/*map<int, int> NodeMap;
		vector<int>Stack;
		Stack.push_back(NodePtr->LeftNodeIndex);*/
		FigureNodeProperty(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(NormalChar* NodePtr, int CurrentIndex)
	{
		FigureNodeProperty(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(Link* NodePtr, int CurrentIndex)
	{
		FigureNodeProperty(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(ChoseSymbol* NodePtr, int CurrentIndex)
	{
		FigureNodeProperty(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(CharSet* NodePtr, int CurrentIndex)
	{
		FigureNodeProperty(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(StringTail* NodePtr, int CurrentIndex)
	{
		FigureNodeProperty(NodePtr, CurrentIndex);
	}
	void VisitSpecNode(Nullable* NodePtr, int CurrentIndex)
	{
		FigureNodeProperty(NodePtr, CurrentIndex);
	}
	
	unordered_map<int,string> initToStringMap()
	{
		unordered_map<int, string> IndexToString;
		//创建figureMap的key名到string的映射
		for(auto& Iter = FirgueMap.begin(); Iter != FirgueMap.end(); Iter = FirgueMap.upper_bound(Iter->first))
		{
			IndexToString.insert(make_pair(Iter->first,to_string(Iter->first)));
		}
		return std::move(IndexToString);
	}
	//根据figuremap内容,创建DFA
	void CreatDFA()
	{
		//获取开始状态集合
		// DtranList里面都是未标记的集合.即不再DfaMap里面
		//unordered_map<string, unordered_map<int, int>>DfaMap;
		auto IndexToString = initToStringMap();
		list<pair<string, set<int>>> DtranList;
		string NewDfaNodeName;
		set<int> initSet;

		for(auto& Iter = FirgueMap[AstRootIndex].FirstPos.begin(); Iter != FirgueMap[AstRootIndex].FirstPos.end(); Iter++)
		{
			NewDfaNodeName += IndexToString[*Iter];
			initSet.insert(*Iter);
		}
		DtranList.push_back(pair<string, set<int>>(NewDfaNodeName, initSet));
		DfaStart = NewDfaNodeName;
		unordered_multimap<int, int> SymbolJumpList;
		//标记数组
		set<string> SignSet;
		SignSet.insert(NewDfaNodeName);
		while(!DtranList.empty())
		{
			auto& CrrentDfaStauts = DtranList.front();
			for(auto& Iter = CrrentDfaStauts.second.begin(); Iter != CrrentDfaStauts.second.end(); Iter++)
			{
				auto CurrentIndex = *Iter;
				if(AstNodeList[CurrentIndex]->Tag == AstTag::CharSet)
				{
					CharSet* CurrentAstNode = (CharSet*)AstNodeList[*Iter];
					GetCharSetSymbolJumpList(SymbolJumpList, CurrentAstNode, CurrentIndex);
					
				}
				else if(AstNodeList[CurrentIndex]->Tag == AstTag::NormalChar)
				{
					NormalChar* CurrentAstNode = (NormalChar*)AstNodeList[CurrentIndex];
					//该键值对是否存在
					if(!HasThisPair(SymbolJumpList, CharMap[CurrentAstNode->Val], CurrentIndex))
					{
						SymbolJumpList.insert(make_pair(CharMap[CurrentAstNode->Val], CurrentIndex));
					}
				}
				else if(AstNodeList[CurrentIndex]->Tag == AstTag::StringTail)
				{
					StringTail* CurrentAstNode = (StringTail*)AstNodeList[CurrentIndex];
					//该键值对是否存在
					if(!HasThisPair(SymbolJumpList, CharMap[CurrentAstNode->Val], CurrentIndex))
					{
						SymbolJumpList.insert(make_pair(CharMap[CurrentAstNode->Val], CurrentIndex));
					}
				}
			}
			for(auto& SymbolIter = SymbolJumpList.begin(); SymbolIter != SymbolJumpList.end();)
			{
				auto KeyCount = SymbolJumpList.count(SymbolIter->first);
				auto Key = SymbolIter->first;
				set<int> FollowSet;
				string FollowSetName;
				for(auto i = 0; i < KeyCount; i++, SymbolIter++)
				{
					FollowSet.insert(FirgueMap[SymbolIter->second].FollowPos.begin(), FirgueMap[SymbolIter->second].FollowPos.end());
				}
				for(auto& FollowIter : FollowSet)
				{
					FollowSetName += IndexToString[FollowIter];
				}

				auto Find = DfaMap.find(CrrentDfaStauts.first);
				if(Find == DfaMap.end())
				{
					unordered_map<int, string>TempMap;
					TempMap.insert(make_pair(Key, FollowSetName));
					DfaMap.insert(make_pair(CrrentDfaStauts.first, TempMap));
				}
				else
				{
					DfaMap[CrrentDfaStauts.first].insert(make_pair(Key, FollowSetName));
				}

				auto& FindIter = DfaMap.find(FollowSetName);
				auto& FindSet = SignSet.find(FollowSetName);
				if(FindIter == DfaMap.end() && FindSet == SignSet.end())
				{
					DtranList.emplace_back(pair<string, set<int>>(FollowSetName, FollowSet));
					SignSet.insert(FollowSetName);
				}
				
			}
			DtranList.pop_front();
			SymbolJumpList.clear();
		}
	}
	//最小化DFA
	void MinningDfa()
	{
		auto StautsGroup = GetStautGroupList();
		//看看有没有元素是有两个以上的DFA节点,有的话说明DFA需要最小化
		for(auto& Iter = StautsGroup.begin(); Iter != StautsGroup.end();Iter++)
		{
			if (Iter->size() != 1)
			{
				set<string> CurrentSet = *Iter;
				string NewStautsName = *CurrentSet.begin();
				CurrentSet.erase(CurrentSet.begin());
				ModifyDfaStauts(CurrentSet, NewStautsName);
			}
		}
	}
	void ModifyDfaStauts(set<string>& OldStautsName,string& NewStautsName)
	{
		for(auto& Iter = DfaMap.begin(); Iter != DfaMap.end();Iter++)
		{
			for(auto& NextIter = Iter->second.begin(); NextIter != Iter->second.end();NextIter++)
			{
				if (OldStautsName.find(NextIter->second) != OldStautsName.end())
				{
					DfaMap[Iter->first][NextIter->first] = NewStautsName;
				}
			}
		}
		for(auto& Iter = OldStautsName.begin(); Iter!= OldStautsName.end();Iter++)
		{
			DeleteDfaNode(*Iter);
		}
	}
	bool DeleteDfaNode(const string& NodeName)
	{
		auto& FindIter = DfaMap.find(NodeName);
		if (FindIter == DfaMap.end())
		{
			return false;
		}
		else
		{
			DfaMap.erase(FindIter->first);
			return true;
		}
	}
	//获取最小化的DFA状态集合分组;
	set<set<string>> GetStautGroupList()
	{
		//获取接受状态和非接受状态做初始集合划分集合
		auto AcceptSet = GetAcceptSet();
		auto NonAcceptSet = GetNonAcceptSet();
		set<set<string>>Result;
		list<set<string>>Queue;
		Queue.emplace_back(NonAcceptSet);
		Queue.emplace_back(AcceptSet);
		//	int c = 0;
		while(!Queue.empty())
		{
			//快速跳出双重循环,使用了goto =.=


			auto& CurrentSet = Queue.front();
			for(auto i = 0; i < this->CharEndIndex; i++)
			{
				unordered_multimap<string, string>IndexToSetMap;
				for(auto& Iter = CurrentSet.begin(); Iter != CurrentSet.end(); Iter++)
				{
					auto& FindIter = DfaMap[*Iter].find(i);
					if(FindIter == DfaMap[*Iter].end())
					{
						//不存在这个索引上的转换
						IndexToSetMap.insert(make_pair("", *Iter));
					}
					else
					{
						IndexToSetMap.insert(make_pair(DfaMap[*Iter][i], *Iter));
					}
				}
				//查看是不是IndexToSetMap里面只有一种key
				auto Count = 0;
				for(auto& Iter = IndexToSetMap.begin(); Iter != IndexToSetMap.end(); Iter = IndexToSetMap.upper_bound(Iter->first))
				{
					Count++;
				}
				if(Count != 1)
				{
					//说明当前字符可以区分状态,
					// 把分开后的状态加入list,当前状态从list删除,
					// 然后不再执行之后的Char测试.直接开始查看下一个list头
					for(auto& Iter = IndexToSetMap.begin(); Iter != IndexToSetMap.end();)
					{
						auto KeyCount = IndexToSetMap.count(Iter->first);
						set<string>NewStauts;
						for(auto j = 0; j < KeyCount; j++, Iter++)
						{

							NewStauts.insert(Iter->second);
						}
						Queue.push_back(move(NewStauts));
					}
					Queue.pop_front();
					goto WhileEnd;
				}
			}
			//如果能执行到这里,说明没有index能区分当前的状态集合;也就是说当前集合是最终的DFA节点的状态集合
			Result.insert(move(Queue.front()));
			Queue.pop_front();
		WhileEnd:;
		}
		return std::move(Result);
	}
	//获取接受状态集合
	set<string> GetAcceptSet()
	{
		set<string> Result;
		string AcceptStr = to_string(this->AcceptIndex);
		for(auto Iter = DfaMap.begin(); Iter != DfaMap.end();Iter = DfaMap.upper_bound(Iter->first))
		{
			if (Iter->first.find(AcceptStr) != Iter->first.npos)
			{
				Result.insert(Iter->first);
			}
		}
		return std::move(Result);
	}
	//获取非接受状态集合
	set<string> GetNonAcceptSet()
	{
		set<string> Result;
		string AcceptStr = to_string(this->AcceptIndex);
		for(auto Iter = DfaMap.begin(); Iter != DfaMap.end(); Iter = DfaMap.upper_bound(Iter->first))
		{
			if(Iter->first.find(AcceptStr) == Iter->first.npos)
			{
				Result.insert(Iter->first);
			}
		}
		return std::move(Result);
	}
private:
	//是否已经存在了这个pair
	bool HasThisPair(unordered_multimap<int, int>& SymbolJumpList, int Key, int Value)
	{
		auto& Range = SymbolJumpList.equal_range(Key);
		for(; Range.first != Range.second; Range.first++)
		{
			if(Range.first->second == Value)
			{
				return true;
			}
		}
		return false;
	}
	void GetCharSetSymbolJumpList(unordered_multimap<int, int>& SymbolJumpList, CharSet* CurrentAstNode, int CurrentIndex)
	{
		if(CurrentAstNode->Opps == OppsType::False)
		{
			for(auto& RangeIter = CurrentAstNode->CharSetRange.begin(); RangeIter != CurrentAstNode->CharSetRange.end(); RangeIter++)
			{
				if(RangeIter->first == RangeIter->second)
				{
					if(!HasThisPair(SymbolJumpList, CharMap[RangeIter->first], CurrentIndex))
					{
						SymbolJumpList.insert(make_pair(CharMap[RangeIter->first], CurrentIndex));
					}
				}
				else
				{
					for(auto i = RangeIter->first; i <= RangeIter->second; i++)
					{
						if(!HasThisPair(SymbolJumpList, CharMap[i], CurrentIndex))
						{
							SymbolJumpList.insert(make_pair(CharMap[i], CurrentIndex));
						}
					}
				}
			}
			
		}
		else if(CurrentAstNode->Opps == OppsType::True)
		{
			set<int> Temp;
			for(auto& RangeIter = CurrentAstNode->CharSetRange.begin(); RangeIter != CurrentAstNode->CharSetRange.end(); RangeIter++)
			{
				if(RangeIter->first == RangeIter->second)
				{
					Temp.insert(CharMap[RangeIter->first]);
				}
				else
				{
					for(auto i = RangeIter->first; i <= RangeIter->second; i++)
					{
						if (Temp.find(CharMap[i])== Temp.end())
						{
							Temp.insert(CharMap[i]);
						}
					}
				}
			}
			set<int> Result;
			for(auto i = 0; i <= CharEndIndex;i++)
			{
				Result.insert(i);
			}
			for(auto& Iter = Temp.begin(); Iter != Temp.end();Iter++)
			{
				Result.erase(*Iter);
			}
			for(auto Iter = Result.begin(); Iter != Result.end();Iter++)
			{
				if(!HasThisPair(SymbolJumpList, *Iter, CurrentIndex))
				{
					SymbolJumpList.insert(make_pair(*Iter, CurrentIndex));
				}
			}
		}
	}
	void initFigureMap(int CurrentIndex)
	{
		if(CurrentIndex == -1)
		{
			return;
		}

		if(HasLeftNode(CurrentIndex))
		{
			initFigureMap(AstNodeList[CurrentIndex]->LeftNodeIndex);
		}
		if(HasRightNode(CurrentIndex))
		{
			initFigureMap(AstNodeList[CurrentIndex]->RightNodeIndex);
		}
		//后缀遍历
		AstNodeList[CurrentIndex]->Accept(*this, CurrentIndex);
	}

	bool IsLeaf(int CurrentIndex)
	{
		return !HasLeftNode(CurrentIndex) && !HasRightNode(CurrentIndex);
	}

	bool HasLeftNode(int CurrentIndex)
	{
		return AstNodeList[CurrentIndex]->LeftNodeIndex != -1;
	}
	bool HasRightNode(int CurrentIndex)
	{
		return AstNodeList[CurrentIndex]->RightNodeIndex != -1;
	}
public:
//正则匹配函数
	bool IsMatch(string& Text,int StartIndex = 0)
	{
		string EndStr = to_string(AcceptIndex);
		auto CurrentStauts = DfaStart;
		for(auto j = StartIndex; j < Text.size(); j++)
		{
			for(auto i = j; i < Text.size(); i++)
			{
				if(DfaMap[CurrentStauts].find(CharMap[Text[i]]) == DfaMap[CurrentStauts].end())
				{
					i = Text.size();
				}
				else
				{
					CurrentStauts = DfaMap[CurrentStauts][CharMap[Text[i]]];
				}
				if(CurrentStauts.find(EndStr) != CurrentStauts.npos)
				{
					return true;
				}
			}
		}
		return false;
	}
	MatchInfo Match(string& Text, int StartIndex = 0)
	{
		string EndStr = to_string(AcceptIndex);
		auto CurrentStauts = DfaStart;
		for(auto j = StartIndex; j < Text.size(); j++)
		{
			for(auto i = j; i < Text.size(); i++)
			{
				if(DfaMap[CurrentStauts].find(CharMap[Text[i]]) == DfaMap[CurrentStauts].end())
				{
					//匹配失败,看看是否在终结状态了
					if(CurrentStauts.find(EndStr) != CurrentStauts.npos)
					{
						//此时i位置是匹配失败的那个字符,索引匹配的内容i 要减 1
						return std::move(MatchInfo(Text.substr(j, i - j ), j, i - 1));
					}
					i = Text.size();
				}
				else
				{
					CurrentStauts = DfaMap[CurrentStauts][CharMap[Text[i]]];
					if(i == Text.size() - 1 && CurrentStauts.find(EndStr) != CurrentStauts.npos)
					{
						//说明整个text串都被DFA匹配完了,如果这时候DFA也到了接受状态,就接受这个串
						return std::move(MatchInfo(Text, j, i));
					}
				}
			}
		}
		return std::move(MatchInfo("", -1, -1));
	}
	//线性搜索 
	vector<MatchInfo> MatchAll(string& Text, int StartIndex = 0)
	{
		vector<MatchInfo>Result;
		while(StartIndex < Text.size())
		{
			auto MatchInformation = Match(Text, StartIndex);
			if (MatchInformation.StartIndex != -1)
			{
				Result.emplace_back(move(MatchInformation));
				StartIndex = Result.back().EndIndex + 1;
			}
			else
			{
				StartIndex = INT_MAX;
			}
		}
		return std::move(Result);
	}
	//替换正则匹配的第一个子串内容
	string ReplaceMatch(string& Text,string& ReplaceData)
	{
		string Temp;
		auto& FindInfo = Match(Temp);
		if (FindInfo.MatchContent == "")
		{
			return std::move(Temp);
		}
		else
		{
			return std::move(Temp.replace(FindInfo.StartIndex, FindInfo.EndIndex - FindInfo.StartIndex + 1, ReplaceData));
		}
	}
	//线性替换
	string ReplaceMatches(string& Text, string& ReplaceData)
	{
		auto FindList = MatchAll(Text);
		auto Temp = Text;
		for(auto i = 0; i < FindList.size();i++)
		{
			auto& FindInfo = FindList[i];
			Temp = Temp.replace(FindInfo.StartIndex, FindInfo.EndIndex - FindInfo.StartIndex + 1, ReplaceData);
		}
		return std::move(Temp);
	}
};

void AstNode::Accept(FA& Dfa, int CurrentIndex)
{
	abort();
}
void Repeat::Accept(FA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void Nullable::Accept(FA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void CharSet::Accept(FA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void Link::Accept(FA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void NormalChar::Accept(FA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void ChoseSymbol::Accept(FA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void StringTail::Accept(FA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}