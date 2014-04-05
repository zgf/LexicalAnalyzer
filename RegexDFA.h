#pragma once
#include "RegexParse.h"
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
	virtual  ~FA()
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
		GreedyType IsGreedy = NodePtr->Greedy;

		auto ChildStauts = NfaNodeMap[NodePtr->LeftNodeIndex];
		vector<pair<int, int>>NewStautsList;
		NewStautsList.push_back(ChildStauts);
		if (EndNumber == INT_MAX)
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
	void CreatNFA(CharSet* NodePtr, int CurrentIndex)
	{
		auto StartIndex = CreatOneNode();
		auto EndIndex = CreatOneNode();
		auto StartNodePtr = GetNfaNodePtr(StartIndex);
		for(auto Iter = NodePtr->CharSetRange.begin(); Iter != NodePtr->CharSetRange.end();Iter++)
		{
			if (Iter->first == Iter->second)
			{
				//说明是单个字符
				StartNodePtr->NextMap.insert(make_pair(CharMap[Iter->first], EndIndex));
			}
			else
			{
				for(int i = Iter->first; i <= Iter->second;i++)
				{
					auto FindIter = StartNodePtr->NextMap.find(CharMap[i]);
					if (FindIter == StartNodePtr->NextMap.end())
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
	pair<int,int> CreatIsomorphismGraph(pair<int,int>StautsSet)
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
public:
	DFA(vector<int>& tCharMap, AstNode* tAstRoot, int Index, vector<AstNode*>& tAstNodeList) :CharMap(tCharMap), AstRoot(tAstRoot), AstRootIndex(Index), AstNodeList(tAstNodeList)
	{
		initFigureMap(AstRootIndex);
	}
	DFA() = delete;
	~DFA()
	{
	}

	void CreatDFA(Repeat* NodePtr, int CurrentIndex)
	{
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
	void CreatDFA(NormalChar* NodePtr, int CurrentIndex)
	{
		auto CurrentTag = AstNodeList[CurrentIndex]->Tag;

	}
	void CreatDFA(Link* NodePtr, int CurrentIndex)
	{
		auto CurrentTag = AstNodeList[CurrentIndex]->Tag;
		auto LeftIndex = AstNodeList[CurrentIndex]->LeftNodeIndex;
		auto RightIndex = AstNodeList[CurrentIndex]->RightNodeIndex;
	}
	void CreatDFA(ChoseSymbol* NodePtr, int CurrentIndex)
	{
		auto CurrentTag = AstNodeList[CurrentIndex]->Tag;
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
	void CreatDFA(CharSet* NodePtr, int CurrentIndex)
	{
		auto CurrentTag = AstNodeList[CurrentIndex]->Tag;
		FigureNode Figure;
		Figure.Nullable = false;
		Figure.FirstPos.insert(CurrentIndex);
		Figure.LastPos.insert(CurrentIndex);
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}
	void CreatDFA(StringTail* NodePtr, int CurrentIndex)
	{
		FigureNode Figure;
		AcceptIndex = CurrentIndex;
		auto CurrentTag = AstNodeList[CurrentIndex]->Tag;
		Figure.Nullable = false;
		Figure.FirstPos.insert(CurrentIndex);
		Figure.LastPos.insert(CurrentIndex);
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}




	void VisitSpecNode(Repeat* NodePtr, int CurrentIndex)
	{

	}
	void VisitSpecNode(NormalChar* NodePtr, int CurrentIndex)
	{
	}
	void VisitSpecNode(Link* NodePtr, int CurrentIndex)
	{
	}
	void VisitSpecNode(ChoseSymbol* NodePtr, int CurrentIndex)
	{
	}
	void VisitSpecNode(CharSet* NodePtr, int CurrentIndex)
	{
	}
	void VisitSpecNode(StringTail* NodePtr, int CurrentIndex)
	{
	}


private:

	/*void TraversalAst(int CurrentIndex,map<int,FigureNode>&FirgueMap,function<void()>& Functor)
	{
	int CurrentIndex = AstRootIndex;
	if(CurrentIndex == -1)
	{
	return;
	}

	if(HasLeftNode(CurrentIndex))
	{
	TraversalAst(AstNodeList[CurrentIndex]->LeftNodeIndex, FirgueMap,Functor);
	}
	if(HasRightNode(CurrentIndex))
	{
	TraversalAst(AstNodeList[CurrentIndex]->RightNodeIndex, FirgueMap,Functor);
	}
	Functor();
	}*/


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
};
void AstNode::Accept(FA& Dfa, int CurrentIndex)
{
	abort();
}
void Repeat::Accept(FA& Dfa, int CurrentIndex)
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