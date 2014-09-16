#include "actormgr.h"
#include "gameobjectmgr.h"

ActorMgr::ActorMgr() :
	m_currActor(0)
{

}

ActorMgr::~ActorMgr()
{
	ActorList::iterator iter = m_players.begin();
	for ( ; iter != m_players.end(); ++iter)
	{
		delete (*iter);
	}

	iter = m_npcs.begin();
	for ( ; iter != m_npcs.end(); ++iter)
	{
		delete (*iter);
	}

	iter = m_enemies.begin();
	for ( ; iter != m_enemies.end(); ++iter)
	{
		delete (*iter);
	}
}

void ActorMgr::AddPlayer(const ActorParams& ap)
{
	Actor* pActor = new Actor(ap);
	m_players.push_back(pActor);
	GameObjectMgr::Instance().AddObject(*pActor);
}

void ActorMgr::AddNPC(const ActorParams& ap)
{
	Actor* pActor = new Actor(ap);
	m_npcs.push_back(pActor);
	pActor->SetTalkScriptNum(m_npcs.size()-1);
	GameObjectMgr::Instance().AddObject(*pActor);
}

void ActorMgr::AddEnemy(const ActorParams& ap)
{
	Actor* pActor = new Actor(ap);
	m_enemies.push_back(pActor);
	GameObjectMgr::Instance().AddObject(*pActor);
}

bool ActorMgr::HasActivePlayer() const
{
	if (m_players.size() > 0)
		return true;
	else
		return false;
}

Actor& ActorMgr::GetActivePlayer()
{
	//@TODO: actually track and control the active player instead of just returning the 0th actor
	assert(m_players.size() > 0);
	return *m_players[0];
}

Actor& ActorMgr::GetNextPlayer()
{
	m_currActor++;
	if (m_currActor >= static_cast<int>(m_players.size()))
		m_currActor = 0;
	return *m_players[m_currActor];
}

ActorList& ActorMgr::GetPartyList()
{
	assert(m_players.size() > 0);
	return m_players;
}

ActorList& ActorMgr::GetEnemyList()
{
	assert(m_enemies.size() > 0);
	return m_enemies;
}

void ActorMgr::ClearNPCs()
{
	ActorList::iterator iter = m_npcs.begin();
	for ( ; iter != m_npcs.end(); ++iter)
	{
		GameObjectMgr::Instance().RemoveActor((**iter));
		delete (*iter);
	}
	m_npcs.clear();
}

void ActorMgr::ClearEnemies()
{
	ActorList::iterator iter = m_enemies.begin();
	for ( ; iter != m_enemies.end(); ++iter)
	{
		GameObjectMgr::Instance().RemoveActor((**iter));
		delete (*iter);
	}
	m_enemies.clear();
}
