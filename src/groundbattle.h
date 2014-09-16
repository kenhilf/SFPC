#ifndef _groundbattle_h_
#define _groundbattle_h_

#include <list>
#include "core.h"
#include "gamestring.h"
#include "battlecutscene.h"
#include "gamewindow.h"
#include "combatevent.h"
#include "targetingbracket.h"
class TileMap;

typedef std::list<CombatEvent*> CombatEventList;
typedef std::vector<Actor*> ActorList;
typedef std::vector<Position> PosList;

class GroundBattle
{
public:
	static GroundBattle& Instance() { static GroundBattle instance;  return instance; };
	~GroundBattle();

	void StartBattle();
	void EndBattle();
	void Update(const float ticks);
	void PlaceAlly(const Position& pos);

	void AddEvent(CombatEvent* pEvent);
	void AddPriorityEvent(CombatEvent* pEvent);
	void ClearEvents();

	Actor& NextTurn();
	Actor& GetActiveActor();
	Actor& GetCurrTarget();
	ActorList& GetPlayerList();
	void ShowMoveRange();

	void Attack();
	bool SpecificAttack(Actor* pAttacker, Actor* pTarget);
	void FindTargets();
	bool SilentTargetCheck(Actor* pAttacker, Actor* pDefender = NULL);
	void NextTarget();
	void PrevTarget();
	bool IsTargeting();
	
	void WinLossCheck();
	void RemoveActorFromBattlefield(Actor* pActor);

	void ShowCombatText(const std::string& text);
	void HideCombatText();
	AutoAdvanceWindow& GetCombatText();

	void StartCutscene();
	void EndCutscene();
	bool IsCutsceneRunning() const;
	void SetCSPlayerPose(const BattleAniFrames::Type pose);
	void SetCSEnemyPose(const BattleAniFrames::Type pose);

	void DebugToHit(const int toHit);
	void ResetCEData();

private:
	GroundBattle();
	GroundBattle(const GroundBattle& rhs);
	GroundBattle& operator=(const GroundBattle& rhs);

	bool m_bActive;
	Position m_roundStartPos;
	ActorList m_players;
	ActorList m_enemies;
	int m_numEnemies;
	ActorList m_allActors;
	PosList m_startPositions;
	int m_turn;
	int m_round;
	int m_toHit;
	int m_numDead;
	GameString m_debugHUD;
	GameString m_bigText;
	Actor* m_pPlayerLeader;
	Actor* m_pEnemyLeader;
	Actor* m_pAttacker;
	ActorList m_targets;
	int m_currTarget;
	TargetingBracket m_targetBracket;
	TileMap* m_pCurrMap;

	BattleCutscene* m_pCutscene;
	AutoAdvanceWindow m_autoWin;

	CombatEventList m_eventList;
	CombatEvent* m_pCurrEvent;
	CombatEventData m_CEData;
};

#endif //_groundbattle_h_