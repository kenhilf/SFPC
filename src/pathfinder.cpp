#include "pathfinder.h"
#include <cmath>
#include "gameobjectmgr.h"
#include "tilemap.h"

bool operator<(const PathNode& lhs, const PathNode& rhs)
{
	return ((lhs.m_costToHere + lhs.m_heuristic) < (rhs.m_costToHere + rhs.m_heuristic));
}

bool operator>(const PathNode& lhs, const PathNode& rhs)
{
	return ((lhs.m_costToHere + lhs.m_heuristic) > (rhs.m_costToHere + rhs.m_heuristic));
}

bool operator==(const PathNode& lhs, const PathNode& rhs)
{
	return ((lhs.m_pos == rhs.m_pos) && 
			(lhs.m_costToHere == rhs.m_costToHere) &&
			(lhs.m_heuristic == rhs.m_heuristic));
}

//////////////////////
// NodeHeap			//
//////////////////////

NodeHeap::NodeHeap()
{

}

NodeHeap::~NodeHeap()
{
	Reset();
}

void NodeHeap::Reset()
{
	for (NV_iter iter = m_nodeVect.begin(); iter != m_nodeVect.end(); ++iter)
	{
		delete (*iter);
	}
	m_nodeVect.clear();
}

std::vector<PathNode*>& NodeHeap::GetVect()
{
	return m_nodeVect;
}

void NodeHeap::PushBack(PathNode* pNode)
{
	m_nodeVect.push_back(pNode);
	push_heap(m_nodeVect.begin(), m_nodeVect.end(), NodeComp());
}

PathNode* NodeHeap::Pop()
{
	PathNode* pn = m_nodeVect.front();
	pop_heap(m_nodeVect.begin(), m_nodeVect.end(), NodeComp());
	m_nodeVect.pop_back();

	return pn;
}

bool NodeHeap::FindNodeByPos(const Position& target, NV_iter& result)
{
	NV_iter iter;
	for (iter = m_nodeVect.begin(); iter != m_nodeVect.end(); ++iter)
	{
		if ((*iter)->m_pos == target)
		{
			result = iter;
			break;
		}
	}

	if (iter == m_nodeVect.end())
		return false;
	else
		return true;
}

void NodeHeap::Erase(const NV_iter& iter)
{
	delete (*iter);
	m_nodeVect.erase(iter);
	make_heap(m_nodeVect.begin(), m_nodeVect.end(), NodeComp());
}

void NodeHeap::CleanUpHeap()
{
	make_heap(m_nodeVect.begin(), m_nodeVect.end(), NodeComp());
}

bool NodeHeap::IsEmpty()
{
	if (m_nodeVect.size() > 0)
		return true;
	else
		return false;
}

//////////////////////
// Pathfinder		//
//////////////////////

Pathfinder::Pathfinder(Actor* pActor, const int searchLimit, const float avgStep) :
	m_pActor(pActor),
	m_pCurrNode(NULL),
	m_pathBreak(0),
	m_searchLimit(searchLimit),
	m_avgStepCost(avgStep),
	m_nodesSearched(0)
{
	// set up offsets for creating neighbors: up, right, down, left
	m_offsets[0].x =  0; m_offsets[0].y = -1;
	m_offsets[1].x =  1; m_offsets[1].y =  0;
	m_offsets[2].x =  0; m_offsets[2].y =  1;
	m_offsets[3].x = -1; m_offsets[3].y =  0;

	red.r = 63;
	blue.b = 127;

	m_startPos = m_pActor->GetWorldPos().CoordToTileNum();
}

Pathfinder::~Pathfinder()
{
	for (NV_iter iter = m_Closed.begin(); iter != m_Closed.end(); ++iter)
	{
		delete (*iter);
	}
	m_Closed.clear();
}

void Pathfinder::Reset()
{
	for (NV_iter iter = m_Closed.begin(); iter != m_Closed.end(); ++iter)
	{
		delete (*iter);
	}
	m_Closed.clear();

	m_Open.Reset();
}

void Pathfinder::FindPath(const Position& targetPos, int pathBreak)
{
	m_pathBreak = pathBreak;
	m_targetPos = targetPos;

	TileMap& map = GameObjectMgr::Instance().GetTileMap();
	map.ClearDebugTint();
	Reset();

	// before we even get started, make sure the goal is a tile we can even enter
/*	if (!map.CanActorEnter(m_pActor, m_targetPos))
	{
		assert(false && "Pathfinder error: m_pActor can't enter the target!");
	}
*/
	// build the first node
	m_pCurrNode = new PathNode();
	m_pCurrNode->m_pos = m_pActor->GetWorldPos().CoordToTileNum();
	m_pCurrNode->m_heuristic = ManhattanDist(m_pCurrNode);
	m_pCurrNode->m_f = m_pCurrNode->m_heuristic;

	// put it into the Open list
	m_Open.PushBack(m_pCurrNode);
}

bool Pathfinder::DoPathfindingStep()
{
	TileMap& map = GameObjectMgr::Instance().GetTileMap();

	// while the current node is not equal to the goal
//	while ((m_pCurrNode->m_pos != m_targetPos) && (m_nodesSearched <= m_searchLimit))
	if ((m_pCurrNode->m_pos != m_targetPos) && (m_nodesSearched <= m_searchLimit))
	{
		// increment nodes searched
		m_nodesSearched++;

		// take the lowest rank node from Open to be the current
/*		if (m_Open.IsEmpty())
		{
			assert(false && "Pathfinder error: m_Open is empty!");
			return false;
		}*/
		m_pCurrNode = m_Open.Pop();

		// and place it into closed
		m_Closed.push_back(m_pCurrNode);

		// for each direction leading away from the current node
		for (int i = 0; i < 4; ++i)
		{
			// check to see if the next position is valid
			Position nextpos = m_pCurrNode->m_pos + m_offsets[i];
			
			// if the tile we're trying to move into is solid, occupied by an enemy, or
			// impassible to the unit's terrain walking type, give up on this neighbor 
			// and go to the next one in the loop
			if (nextpos != m_targetPos)
			{
				if (!map.CanActorEnter(m_pActor, nextpos))
					continue;
			}

			// get the cost to enter the tile and add the current node's cost to this point
			float cost = map.GetCostToEnter(m_pActor, nextpos) + m_pCurrNode->m_costToHere;
/*			if (cost >= 20.0f) //@TODO: need a better check than the hardcoded 20's in statblock
			{
				// this tile is considered impassible to this unit's terrain walking type,
				// so give up on this neighbor as well
				continue;
			}
			// since it's not impassible to this unit, 
			cost ;
*/
			// check to see if this candidate is in m_Open already
			NV_iter nviter;
			if (m_Open.FindNodeByPos(nextpos, nviter))
			{
				// if its cost is greater than our current proposed cost remove it from m_Open,
				// it will be replaced by our current candidate
				if ((*nviter)->m_costToHere > cost)
					m_Open.Erase(nviter);
			}

			// check to see if this candidate is in m_Closed already, if it is more expensive
			// than the current candidate, remove it so that we can replace it.  If it's 
			// cheaper, this path is no good.
			bool bCheaperInClosed = false;
			for (nviter = m_Closed.begin(); nviter != m_Closed.end(); ++nviter)
			{
				if ((*nviter)->m_pos == nextpos)
				{
					if ((*nviter)->m_costToHere > cost)
					{
						delete (*nviter);
						m_Closed.erase(nviter);
					}
					else
						bCheaperInClosed = true;
					break;
				}
			}
			
			// this neighbor is not in open and not cheaper in closed, so build that neighbor 
			// and add it to m_Open
			if (!bCheaperInClosed)
			{
				PathNode* pNeighbor = new PathNode();
				pNeighbor->m_pos = m_pCurrNode->m_pos + m_offsets[i];
				pNeighbor->m_costToHere = cost;
				pNeighbor->m_heuristic = ManhattanDist(pNeighbor);
				pNeighbor->m_f = pNeighbor->m_costToHere + pNeighbor->m_heuristic;
				pNeighbor->m_pParent = m_pCurrNode;
				m_Open.PushBack(pNeighbor);
				map.SetDebugTint(pNeighbor->m_pos, red);
				if (red.r < 255) 
					red.r += 5;
				if (red.r > 255) 
					red.r = 255;
			}
		}
	}

	//@TODO: this part is still a bit tangled, likely need to introduce some internal 
	// state tracking

	// we spent too long looking for the solution and are giving up
	if ((m_nodesSearched >= m_searchLimit) || (m_pCurrNode->m_pos != m_targetPos))
		return false;
	// if we got this far we (hopefully) found a path
	return true;
}

void Pathfinder::ProcessResult()
{
	TileMap& map = GameObjectMgr::Instance().GetTileMap();

	// rebuild path from goal to start by following parent pointers
	if (m_pCurrNode->m_pos == m_targetPos)
	{
		// reverse it into the m_result deque and keep that local copy for debugging
		m_result.push_front(m_pCurrNode);
		Position actorPos = m_pActor->GetWorldPos().CoordToTileNum();
		while (m_pCurrNode->m_pos != actorPos)
		{
			map.SetDebugTint(m_pCurrNode->m_pos, blue);
			if (blue.b < 255) 
				blue.b += 10;
			if (blue.b > 255) 
				blue.b = 255;
			m_pCurrNode = m_pCurrNode->m_pParent;
			m_result.push_front(m_pCurrNode);
		}

		// shave off the end a number of nodes equal to the path breakpoint
		// this is so a target enemy can be specified, even though we can't enter that tile.
		//@TODO: this probably will need considerable work for attack ranges greater than 1
		for (int i = 0; i < m_pathBreak; i++)
			m_result.pop_back();

		// then load it into the actor's path to be stored in a navigation friendly format
		Path& path = m_pActor->GetPath();
		path.Clear();
		Position lastPos = m_startPos;
		Position nextPos;
		NodeDeque::iterator iter = m_result.begin();
		++iter;
		for ( ; iter != m_result.end(); ++iter)
		{
			nextPos = (*iter)->m_pos;
			path.AddStep(nextPos - lastPos);
			lastPos = nextPos;
		}
	}
}

float Pathfinder::ManhattanDist(Actor* pEnemy) const
{
	Position attackerPos = m_pActor->GetWorldPos().CoordToTileNum();
	Position defenderPos = pEnemy->GetWorldPos().CoordToTileNum();

	return m_avgStepCost * static_cast<float>(abs(attackerPos.x - defenderPos.x) + 
											  abs(attackerPos.y - defenderPos.y));
}

float Pathfinder::ManhattanDist(PathNode* pNode) const
{
	return m_avgStepCost * static_cast<float>(abs(pNode->m_pos.x - m_targetPos.x) + 
											  abs(pNode->m_pos.y - m_targetPos.y));
}
