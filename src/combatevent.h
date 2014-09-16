#ifndef _combatevent_h_
#define _combatevent_h_

#include <string>
#include "core.h"
#include "fader.h"
#include "pathfinder.h"
class Actor;

namespace EventID
{
	enum Type { NoEvent, 
				Attack, CriticalHit, NormalHit, Miss, Evade, TakeDamage, DeathCheck, 
				CounterAttack, SecondAttack, 
				AddExp, AddGold, AwardExp, AwardGold, LevelUp, 
				Delay, FadeIn, FadeOut, PlayBattleSong, 
				StartCutscene, EndCutscene, 
				EnemyAITryAttacking, EnemyAIMove, EnemyAIWaitForMove,
				EndBattle, 
				NumTypes };
}

namespace AttackPhase
{
	enum Type { FirstAttack, FirstCounter, 
				SecondAttack, SecondCounter, 
				DoneAttacking, NumTypes };
}

struct CombatEventData
{
	CombatEventData() : pAttacker(NULL), pDefender(NULL), atkPhase(AttackPhase::FirstAttack),
						totalExp(0), totalGold(0) {}

	Actor* pAttacker;
	Actor* pDefender;

	AttackPhase::Type atkPhase;

	int totalExp;
	int totalGold;
};

// base CombatEvent class
class CombatEvent
{
public:
	CombatEvent(CombatEventData* pData = NULL, const EventID::Type id = EventID::NoEvent);
	virtual ~CombatEvent();
	EventID::Type GetID();
	virtual bool Update(float ticks);
	virtual std::string GetParamsAsString();

protected:
	CombatEventData* m_pData;
	EventID::Type m_eventID;
	int m_eventPhase;
	float m_delay;
	Actor* m_pAttacker;
	Actor* m_pDefender;
};

////////////////////////
// Basic CombatEvents //
////////////////////////

class CEAttack : public CombatEvent
{
public:
	CEAttack(CombatEventData* pData);
	virtual ~CEAttack() {}
	virtual bool Update(float ticks);
};

class CECriticalHit : public CombatEvent
{
public:
	CECriticalHit(CombatEventData* pData);
	virtual ~CECriticalHit() {}
	virtual bool Update(float ticks);
};

class CENormalHit : public CombatEvent
{
public:
	CENormalHit(CombatEventData* pData);
	virtual ~CENormalHit() {}
	virtual bool Update(float ticks);
};

class CEMiss : public CombatEvent
{
public:
	CEMiss(CombatEventData* pData);
	virtual ~CEMiss() {}
	virtual bool Update(float ticks);
};

class CEEvade : public CombatEvent
{
public:
	CEEvade(CombatEventData* pData);
	virtual ~CEEvade() {}
	virtual bool Update(float ticks);
};

class CETakeDamage : public CombatEvent
{
public:
	CETakeDamage(CombatEventData* pData, const int dmg, const DamageElement::Type elem = DamageElement::Physical);
	virtual ~CETakeDamage() {}
	virtual bool Update(float ticks);

private:
	int m_dmg;
	DamageElement::Type m_dmgElem;
};

class CEDeathCheck : public CombatEvent
{
public:
	CEDeathCheck(CombatEventData* pData);
	virtual ~CEDeathCheck() {}
	virtual bool Update(float ticks);
};

class CECounterAttack : public CombatEvent
{
public:
	CECounterAttack(CombatEventData* pData);
	virtual ~CECounterAttack() {}
	virtual bool Update(float ticks);
};

class CESecondAttack : public CombatEvent
{
public:
	CESecondAttack(CombatEventData* pData);
	virtual ~CESecondAttack() {}
	virtual bool Update(float ticks);
};

class CEAddExp : public CombatEvent
{
public:
	CEAddExp(CombatEventData* pData);
	virtual ~CEAddExp() {}
	virtual bool Update(float ticks);
};

class CEAddGold : public CombatEvent
{
public:
	CEAddGold(CombatEventData* pData);
	virtual ~CEAddGold() {}
	virtual bool Update(float ticks);
};

class CEAwardExp : public CombatEvent
{
public:
	CEAwardExp(CombatEventData* pData);
	virtual ~CEAwardExp() {}
	virtual bool Update(float ticks);
};

class CEAwardGold : public CombatEvent
{
public:
	CEAwardGold(CombatEventData* pData);
	virtual ~CEAwardGold() {}
	virtual bool Update(float ticks);
};

class CELevelUp : public CombatEvent
{
public:
	CELevelUp(CombatEventData* pData);
	virtual ~CELevelUp() {}
	virtual bool Update(float ticks);
};

class CEDelay : public CombatEvent
{
public:
	CEDelay(const unsigned int timeInMS);
	virtual ~CEDelay() {}
	virtual bool Update(float ticks);

private:
	float m_delayLength;
};

class CEFadeIn : public CombatEvent
{
public:
	CEFadeIn(CombatEventData* pData);
	virtual ~CEFadeIn() {}
	virtual bool Update(float ticks);

private:
	Fader m_fader;
};

class CEFadeOut : public CombatEvent
{
public:
	CEFadeOut(CombatEventData* pData);
	virtual ~CEFadeOut() {}
	virtual bool Update(float ticks);

private:
	Fader m_fader;
};

class CEPlayBattleSong : public CombatEvent
{
public:
	CEPlayBattleSong(CombatEventData* pData);
	virtual ~CEPlayBattleSong() {}
	virtual bool Update(float ticks);
};

class CEStartCutscene : public CombatEvent
{
public:
	CEStartCutscene();
	virtual ~CEStartCutscene() {}
	virtual bool Update(float ticks);
};

class CEEndCutscene : public CombatEvent
{
public:
	CEEndCutscene();
	virtual ~CEEndCutscene() {}
	virtual bool Update(float ticks);
};

class CEEnemyAITryAttacking : public CombatEvent
{
public:
	CEEnemyAITryAttacking(CombatEventData* pData);
	virtual ~CEEnemyAITryAttacking() {}
	virtual bool Update(float ticks);

private:
	float m_thinkTime;
};

class CEEnemyAIMove : public CombatEvent
{
public:
	CEEnemyAIMove(CombatEventData* pData);
	virtual ~CEEnemyAIMove() {}
	virtual bool Update(float ticks);

private:
	float m_thinkTime;
	Pathfinder m_pathfinder;
};

class CEEnemyAIWaitForMove : public CombatEvent
{
public:
	CEEnemyAIWaitForMove(CombatEventData* pData);
	virtual ~CEEnemyAIWaitForMove() {}
	virtual bool Update(float ticks);

private:
	float m_delay;
};

class CEEndBattle : public CombatEvent
{
public:
	CEEndBattle();
	virtual ~CEEndBattle() {}
	virtual bool Update(float ticks);
};

#endif //_combatevent_h_