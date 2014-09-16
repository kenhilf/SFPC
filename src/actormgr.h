#ifndef _actormgr_h_
#define _actormgr_h_

#include "actor.h"

class ActorMgr
{
public:
	// returns singleton instance
	static ActorMgr& Instance() { static ActorMgr instance;  return instance; };
	~ActorMgr();

	void AddPlayer(const ActorParams& ap);
	void AddNPC(const ActorParams& ap);
	void AddEnemy(const ActorParams& ap);

	bool HasActivePlayer() const;
	Actor& GetActivePlayer();
	Actor& GetNextPlayer();
	ActorList& GetPartyList();
	ActorList& GetEnemyList();

	void ClearNPCs();
	void ClearEnemies();

private:
	ActorMgr();
	ActorMgr(const ActorMgr& rhs);
	ActorMgr& operator=(const ActorMgr& rhs);

	ActorList m_players;
	ActorList m_npcs;
	ActorList m_enemies;

	int m_currActor;
};

#endif // _actormgr_h_