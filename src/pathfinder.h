#ifndef _pathfinder_h_
#define _pathfinder_h_

#include <vector>
#include <deque>
#include <algorithm>
#include <functional>
#include "position.h"
#include "core.h"
class Actor;

struct PathNode
{
	PathNode() : m_pos(Position(0, 0)), m_costToHere(0.0f), m_heuristic(0.0f), m_f(0.0f),
				 m_pParent(NULL) 
	{
	
	}

	Position m_pos;
	float m_f;
	float m_costToHere;
	float m_heuristic;
	PathNode* m_pParent;
};

struct NodeComp 
{
	bool operator()(const PathNode* lhs, const PathNode* rhs) const
	{
		return (lhs->m_f > rhs->m_f);
	}
};

typedef std::vector<PathNode*> NodeVector;
typedef std::deque<PathNode*> NodeDeque;
typedef std::vector<PathNode*>::iterator NV_iter;

class NodeHeap
{
public:
	NodeHeap();
	~NodeHeap();
	void Reset();

	NodeVector& GetVect();
	void PushBack(PathNode* pNode);
	PathNode* Pop();
	bool FindNodeByPos(const Position& target, NV_iter& result);
	void Erase(const NV_iter& iter);
	void CleanUpHeap();
	bool IsEmpty();

private:
	NodeVector m_nodeVect;
};

class Pathfinder
{
public:
	Pathfinder(Actor* pActor, const int searchLimit = 1000, const float avgStep = 1.15f);
	~Pathfinder();
	void Reset();

	void FindPath(const Position& targetPos, int pathBreak = 0);
	bool DoPathfindingStep();
	void ProcessResult();
	float ManhattanDist(Actor* pEnemy) const;

private:
	float ManhattanDist(PathNode* pNode) const;

	Actor* m_pActor;
	PathNode* m_pCurrNode;
	Position m_startPos;
	Position m_targetPos;
	int m_pathBreak;
	int m_searchLimit;
	int m_nodesSearched;
	float m_avgStepCost;

	NodeHeap m_Open;
	NodeVector m_Closed;
	NodeDeque m_result;

	Position m_offsets[4];
	Pixel32 red;
	Pixel32 blue;
};

#endif //_pathfinder_h_