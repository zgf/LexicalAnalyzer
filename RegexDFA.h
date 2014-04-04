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
class DFA
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
	void VisitSpecNode(AstNode* NodePtr, int CurrentIndex)
	{
		abort();
	}
	void VisitSpecNode(Repeat* NodePtr, int CurrentIndex)
	{
		auto CurrentTag = AstNodeList[CurrentIndex]->Tag;
		FigureNode Figure;
		auto CurrentNodePtr = dynamic_cast<Repeat*>( AstNodeList[CurrentIndex] );
		auto& LeftNodeIter = FirgueMap.find(AstNodeList[CurrentIndex]->LeftNodeIndex);
		if(LeftNodeIter != FirgueMap.end())
		{
			auto& LeftFigure = LeftNodeIter->second;
			Figure.FirstPos.insert(LeftFigure.FirstPos.begin(), LeftFigure.FirstPos.end());
			Figure.LastPos.insert(LeftFigure.LastPos.begin(), LeftFigure.LastPos.end());
		}
		if(CurrentNodePtr->RepeatNumber.first == 0)
		{
			Figure.Nullable = true;
		}
		else
		{
			Figure.Nullable = false;
		}
		for(auto Iter = Figure.LastPos.begin(); Iter != Figure.LastPos.end(); Iter++)
		{
			FirgueMap[*Iter].FollowPos.insert(Figure.FirstPos.begin(), Figure.FirstPos.end());
		}
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}
	void VisitSpecNode(NormalChar* NodePtr, int CurrentIndex)
	{
		auto CurrentTag = AstNodeList[CurrentIndex]->Tag;
		FigureNode Figure;
		Figure.Nullable = false;
		Figure.FirstPos.insert(CurrentIndex);
		Figure.LastPos.insert(CurrentIndex);
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}
	void VisitSpecNode(Link* NodePtr, int CurrentIndex)
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
	void VisitSpecNode(ChoseSymbol* NodePtr, int CurrentIndex)
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
	void VisitSpecNode(CharSet* NodePtr, int CurrentIndex)
	{
		auto CurrentTag = AstNodeList[CurrentIndex]->Tag;
		FigureNode Figure;
		Figure.Nullable = false;
		Figure.FirstPos.insert(CurrentIndex);
		Figure.LastPos.insert(CurrentIndex);
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
	}
	void VisitSpecNode(StringTail* NodePtr, int CurrentIndex)
	{
		FigureNode Figure;
		AcceptIndex = CurrentIndex;
		auto CurrentTag = AstNodeList[CurrentIndex]->Tag;
		Figure.Nullable = false;
		Figure.FirstPos.insert(CurrentIndex);
		Figure.LastPos.insert(CurrentIndex);
		FirgueMap.insert(make_pair(CurrentIndex, Figure));
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
	void CreatNFA(map<int, FigureNode>&FirgueMap)
	{
		//根节点的Index的firstPos是开始状态.
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
};
void AstNode::Accept(DFA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void Repeat::Accept(DFA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void CharSet::Accept(DFA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void Link::Accept(DFA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void NormalChar::Accept(DFA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void ChoseSymbol::Accept(DFA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}
void StringTail::Accept(DFA& Dfa, int CurrentIndex)
{
	Dfa.VisitSpecNode(this, CurrentIndex);
}