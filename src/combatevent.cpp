#include "combatevent.h"
#include "actor.h"
#include "audiomgr.h"
#include "groundbattle.h"
#include "gameobjectmgr.h"
#include "statblock.h"

// a compare function that compares two actor's by their current HP so an enemy's target list
// can be sorted by remaining HP
bool CompareCurrHP(Actor* lhs, Actor* rhs)
{
	if ((lhs->GetStatBlock().GetCurrHP()) > (rhs->GetStatBlock().GetCurrHP()))
		return true;
	else
		return false;
}

//////////////////////
// CombatEvent		//
//////////////////////

CombatEvent::CombatEvent(CombatEventData* pData, const EventID::Type id) :
	m_pData(pData),
	m_eventID(id),
	m_eventPhase(0),
	m_delay(0),
	m_pAttacker(NULL),
	m_pDefender(NULL)
{
	if (m_pData != NULL)
	{
		if ((m_pData->atkPhase == AttackPhase::FirstAttack)  ||
			(m_pData->atkPhase == AttackPhase::SecondAttack) ||
			(m_pData->atkPhase == AttackPhase::DoneAttacking))
		{
			m_pAttacker = m_pData->pAttacker;
			m_pDefender = m_pData->pDefender;
		}
		// else this is a counterattack, so switch our perspective on who is 
		// CURRENTLY attacking/defending while preserving our original data
		else
		{	 
			m_pAttacker = m_pData->pDefender;
			m_pDefender = m_pData->pAttacker;
		}
	}
}

CombatEvent::~CombatEvent()
{

}

EventID::Type CombatEvent::GetID()
{
	return m_eventID;
}

bool CombatEvent::Update(float ticks)
{
	return true;
}

std::string CombatEvent::GetParamsAsString()
{
	return "NYI";
}

///////////////////
// CEAttack		 //
///////////////////

CEAttack::CEAttack(CombatEventData* pData) :
	CombatEvent(pData, EventID::Attack)
{
	
}

bool CEAttack::Update(float ticks)
{
	if (m_eventPhase == 0)
	{
		std::string temp = m_pAttacker->GetStatBlock().GetName();

		if (m_pData->atkPhase == AttackPhase::FirstAttack)
			temp += " attacks!";

		if ((m_pData->atkPhase == AttackPhase::FirstCounter) ||
			(m_pData->atkPhase == AttackPhase::SecondCounter))
			temp += "'s counterattack!";

		if (m_pData->atkPhase == AttackPhase::SecondAttack)
			temp += "'s second attack!";

		assert(m_pData->atkPhase < 4);

		GroundBattle::Instance().ShowCombatText(temp);
		m_eventPhase++;
	}

	if (GroundBattle::Instance().GetCombatText().AtEndOfString())
		m_delay += ticks;

	if ((m_delay >= 1000.0f) && (m_eventPhase == 1))
	{
		GroundBattle::Instance().HideCombatText();
		m_eventPhase++;
	}

	if ((m_delay >= 1500.0f) && (m_eventPhase == 2))
	{
		if (m_pAttacker->GetTeam() == ActorTeam::Player)
			GroundBattle::Instance().SetCSPlayerPose(BattleAniFrames::Attack);
		else
			GroundBattle::Instance().SetCSEnemyPose(BattleAniFrames::Attack);
		m_eventPhase++;
	}

	if ((m_delay >= 1800.0f) && (m_eventPhase == 3))
	{
		const int toHit = (rand() % 20) + 1;
		GroundBattle::Instance().DebugToHit(toHit);

		// if attack was a normal hit, try to evade
		// currently flat 5% chance
		if ((toHit > 3) && (toHit < 20))
		{
			const int evadeChance = (rand() % 20) + 1;
			if (evadeChance == 20)
			{
				GroundBattle::Instance().AddEvent(new CEDelay(500));
				GroundBattle::Instance().AddEvent(new CEEvade(m_pData));
				return true;
			}
		}

		if (toHit == 20)
		{
			GroundBattle::Instance().AddEvent(new CEDelay(500));
			GroundBattle::Instance().AddEvent(new CECriticalHit(m_pData));
		}
		else if (toHit >= 4)
		{
			GroundBattle::Instance().AddEvent(new CEDelay(500));
			GroundBattle::Instance().AddEvent(new CENormalHit(m_pData));
		}
		else // tohit <= 3
		{
			GroundBattle::Instance().AddEvent(new CEDelay(500));
			GroundBattle::Instance().AddEvent(new CEMiss(m_pData));
		}
		return true;
	}
	
	return false;
}

///////////////////
// CECriticalHit //
///////////////////

CECriticalHit::CECriticalHit(CombatEventData* pData) :
	CombatEvent(pData, EventID::CriticalHit)
{
	
}

bool CECriticalHit::Update(float ticks)
{
	if (m_eventPhase == 0)
	{
		GroundBattle::Instance().ShowCombatText("Critical Hit!!");
		m_eventPhase++;
	}	

	if (GroundBattle::Instance().GetCombatText().AtEndOfString())
		m_delay += ticks;

	if ((m_delay >= 1000.0f) && (m_eventPhase == 1))
	{
		int dmg = ((m_pData->pAttacker->GetStatBlock().GetAttack() - 
					m_pData->pDefender->GetStatBlock().GetDefense()) * 2);
		if (dmg < 1) dmg = 1;

		GroundBattle::Instance().HideCombatText();
		GroundBattle::Instance().AddEvent(new CEDelay(500));
		GroundBattle::Instance().AddEvent(new CETakeDamage(m_pData, dmg));
		return true;
	}
	
	return false;
}

///////////////////
// CENormalHit	 //
///////////////////

CENormalHit::CENormalHit(CombatEventData* pData) :
	CombatEvent(pData, EventID::NormalHit)
{
	
}

bool CENormalHit::Update(float ticks)
{
	int dmg = (m_pAttacker->GetStatBlock().GetAttack() - 
			   m_pDefender->GetStatBlock().GetDefense());
	if (dmg < 1)
		dmg = 1;

	GroundBattle::Instance().AddEvent(new CETakeDamage(m_pData, dmg));

	return true;
}

///////////////////
// CEMiss		 //
///////////////////

CEMiss::CEMiss(CombatEventData* pData) :
	CombatEvent(pData, EventID::Miss)
{
	
}

bool CEMiss::Update(float ticks)
{
	if (m_eventPhase == 0)
	{
		std::string temp = m_pAttacker->GetStatBlock().GetName() + " missed!";
		GroundBattle::Instance().ShowCombatText(temp);
		m_eventPhase++;
	}

	if (GroundBattle::Instance().GetCombatText().AtEndOfString())
		m_delay += ticks;

	if ((m_delay >= 1000.0f) && (m_eventPhase == 1))
	{
		GroundBattle::Instance().SetCSEnemyPose(BattleAniFrames::Idle);
		GroundBattle::Instance().SetCSPlayerPose(BattleAniFrames::Idle);
		GroundBattle::Instance().HideCombatText();
		GroundBattle::Instance().AddEvent(new CEDelay(500));
		GroundBattle::Instance().AddEvent(new CEDeathCheck(m_pData));
		return true;
	}
	
	return false;
}

///////////////////
// CEEvade		 //
///////////////////

CEEvade::CEEvade(CombatEventData* pData) :
	CombatEvent(pData, EventID::Miss)
{
	
}

bool CEEvade::Update(float ticks)
{
	if (m_eventPhase == 0)
	{
		std::string temp = m_pDefender->GetStatBlock().GetName() + " evades the attack!";
		GroundBattle::Instance().ShowCombatText(temp);
		m_eventPhase++;
	}

	if (GroundBattle::Instance().GetCombatText().AtEndOfString())
		m_delay += ticks;

	if ((m_delay >= 0.0f) && (m_eventPhase == 1))
	{
		if (m_pAttacker->GetTeam() == ActorTeam::Player)
			GroundBattle::Instance().SetCSEnemyPose(BattleAniFrames::Evade);
		else
			GroundBattle::Instance().SetCSPlayerPose(BattleAniFrames::Evade);
		m_eventPhase++;
	}

	if ((m_delay >= 1500.0f) && (m_eventPhase == 2))
	{
		GroundBattle::Instance().SetCSEnemyPose(BattleAniFrames::Idle);
		GroundBattle::Instance().SetCSPlayerPose(BattleAniFrames::Idle);

		GroundBattle::Instance().HideCombatText();
		GroundBattle::Instance().AddEvent(new CEDelay(500));
		GroundBattle::Instance().AddEvent(new CEDeathCheck(m_pData));
		return true;
	}
	
	return false;
}

///////////////////
// CETakeDamage	 //
///////////////////

CETakeDamage::CETakeDamage(CombatEventData* pData, const int dmg, const DamageElement::Type elem) :
	CombatEvent(pData, EventID::TakeDamage),
	m_dmg(dmg),
	m_dmgElem(elem)
{
	
}

bool CETakeDamage::Update(float ticks)
{
	if (m_eventPhase == 0)
	{
		std::stringstream temp;
		temp << m_pDefender->GetStatBlock().GetName() << " got damaged by " 
			 << m_dmg << ".";
		GroundBattle::Instance().ShowCombatText(temp.str());
		//@TODO: modify StatBlock::TakeDamage to include resistance checks
		m_pDefender->GetStatBlock().TakeDamage(m_dmg);
		m_eventPhase++;
	}	

	if (GroundBattle::Instance().GetCombatText().AtEndOfString())
		m_delay += ticks;

	if ((m_delay >= 1000.0f) && (m_eventPhase == 1))
	{
		if (m_pAttacker->GetTeam() == ActorTeam::Player)
			GroundBattle::Instance().SetCSPlayerPose(BattleAniFrames::Idle);
		else
			GroundBattle::Instance().SetCSEnemyPose(BattleAniFrames::Idle);

		GroundBattle::Instance().HideCombatText();
		GroundBattle::Instance().AddEvent(new CEDelay(500));
		GroundBattle::Instance().AddEvent(new CEDeathCheck(m_pData));
		return true;
	}
	else
		return false;
}

///////////////////
// CEDeathCheck	 //
///////////////////

CEDeathCheck::CEDeathCheck(CombatEventData* pData) :
	CombatEvent(pData, EventID::DeathCheck)
{
	
}

bool CEDeathCheck::Update(float ticks)
{
	if (m_pDefender->GetStatBlock().IsAlive())
	{
		if (m_pData->atkPhase == AttackPhase::FirstAttack)
		{
			// check for first counter.  
			//@TEMP: counter attack rate set high at 1 in 4 for demo purposes
			int ctrAtk = (rand() % 4) + 1;
			if ((ctrAtk >= 4) &&
				GroundBattle::Instance().SilentTargetCheck(m_pData->pDefender, m_pData->pAttacker))// >= 4)
			{
				GroundBattle::Instance().AddEvent(new CEAddExp(m_pData));
				m_pData->atkPhase = AttackPhase::FirstCounter;
				GroundBattle::Instance().AddEvent(new CEAttack(m_pData));
				return true;
			}
			// if not countering, check second attack
			//@TEMP: double attack rate set high at 1 in 4 for demo purposes
			int dblAtk = (rand() % 4) + 1;
			if (dblAtk >= 4)
			{
				GroundBattle::Instance().AddEvent(new CEAddExp(m_pData));
				m_pData->atkPhase = AttackPhase::SecondAttack;
				GroundBattle::Instance().AddEvent(new CEAttack(m_pData));
			}
			// if not countering or second attacking, we're done
			else
			{
				GroundBattle::Instance().AddEvent(new CEAddExp(m_pData));
				m_pData->atkPhase = AttackPhase::DoneAttacking;
				GroundBattle::Instance().AddEvent(new CEAwardExp(m_pData));
			}
			return true;
		}
		if (m_pData->atkPhase == AttackPhase::FirstCounter)
		{
			// check for second attack.
			int dblAtk = (rand() % 4) + 1;
			if (dblAtk >= 4)
			{
				GroundBattle::Instance().AddEvent(new CEAddExp(m_pData));
				m_pData->atkPhase = AttackPhase::SecondAttack;
				GroundBattle::Instance().AddEvent(new CEAttack(m_pData));
			}
			// if no second attack, we're done.
			else
			{
				GroundBattle::Instance().AddEvent(new CEAddExp(m_pData));
				m_pData->atkPhase = AttackPhase::DoneAttacking;
				GroundBattle::Instance().AddEvent(new CEAwardExp(m_pData));
			}
			return true;
		}
		if (m_pData->atkPhase == AttackPhase::SecondAttack)
		{
			// check for second counter.
			//@TEMP: counter attack rate set high at 1 in 4 for testing purposes
			int ctrAtk = (rand() % 4) + 1;
			if ((ctrAtk >= 4) &&
				GroundBattle::Instance().SilentTargetCheck(m_pData->pDefender, m_pData->pAttacker))// >= 4)
			{
				GroundBattle::Instance().AddEvent(new CEAddExp(m_pData));
				m_pData->atkPhase = AttackPhase::SecondCounter;
				GroundBattle::Instance().AddEvent(new CEAttack(m_pData));
			}
			// if not, we're done.
			else
			{
				GroundBattle::Instance().AddEvent(new CEAddExp(m_pData));
				m_pData->atkPhase = AttackPhase::DoneAttacking;
				GroundBattle::Instance().AddEvent(new CEAwardExp(m_pData));
			}
			return true;
		}
		if (m_pData->atkPhase == AttackPhase::SecondCounter)
		{
			// we're done.
			GroundBattle::Instance().AddEvent(new CEAddExp(m_pData));
			m_pData->atkPhase = AttackPhase::DoneAttacking;
			GroundBattle::Instance().AddEvent(new CEAwardExp(m_pData));
		}
		return true;
	}
	else // defender was defeated
	{
		if (m_eventPhase == 0)
		{
			std::string temp;
			if (m_pDefender->GetTeam() == ActorTeam::Player)
				temp = m_pDefender->GetStatBlock().GetName() + " is exhausted...";
			else // defender == enemy
				temp = m_pDefender->GetStatBlock().GetName() + " was\ndefeated!";

			GroundBattle::Instance().ShowCombatText(temp);
			GroundBattle::Instance().RemoveActorFromBattlefield(m_pDefender);

			m_pAttacker->GetStatBlock().AddKill();
			m_pDefender->GetStatBlock().AddDefeat();

			m_eventPhase++;
		}

		if (GroundBattle::Instance().GetCombatText().AtEndOfString())
			m_delay += ticks;

		if ((m_delay >= 1000.0f) && (m_eventPhase == 1))
		{
			GroundBattle::Instance().HideCombatText();
			GroundBattle::Instance().AddEvent(new CEDelay(500));
			GroundBattle::Instance().AddEvent(new CEAddExp(m_pData));
			GroundBattle::Instance().AddEvent(new CEAddGold(m_pData));
			GroundBattle::Instance().AddEvent(new CEAwardExp(m_pData));

			return true;
		}
	}

	return false;
}

///////////////////
// CEAddExp		 //
///////////////////

CEAddExp::CEAddExp(CombatEventData* pData) :
	CombatEvent(pData, EventID::AddExp)
{
	
}

bool CEAddExp::Update(float ticks)
{
	// players get exp, enemies do not
	if (m_pAttacker->GetTeam() == ActorTeam::Player)
	{
		//@TODO: need a CalculateExp function.  Soft cap at 48, hard cap at 50, etc.
		m_pData->totalExp += 1;
	}
	return true;
}

///////////////////
// CEAddGold	 //
///////////////////

CEAddGold::CEAddGold(CombatEventData* pData) :
	CombatEvent(pData, EventID::AddGold)
{
	
}

bool CEAddGold::Update(float ticks)
{
	// players get gold, enemies do not
	if (m_pAttacker->GetTeam() == ActorTeam::Player)
	{
		//@TODO: add gold value to actors, GameCommand to set it in actor setup scripts
		m_pData->totalGold += 10;
	}
	return true;
}

///////////////////
// CEAwardExp	 //
///////////////////

CEAwardExp::CEAwardExp(CombatEventData* pData) :
	CombatEvent(pData, EventID::AwardExp)
{

}

bool CEAwardExp::Update(float ticks)
{
	// If no exp was earned, we're done.  Enemies do not earn exp.
	if (m_pData->totalExp == 0)
	{
		GroundBattle::Instance().AddEvent(new CEDelay(1500));
		GroundBattle::Instance().AddEvent(new CEFadeOut(m_pData));
		GroundBattle::Instance().AddEvent(new CEEndCutscene());
		return true;
	}
	else // exp was earned at some point
	{
		Actor* pPlayer = NULL;
		if (m_pAttacker->GetTeam() == ActorTeam::Player)
			pPlayer = m_pAttacker;
		else // defender is the player
			pPlayer = m_pDefender;

		// before we go handing out exp, make sure the player is alive!
		// they may have gotten killed on a counterattack
		if (!pPlayer->GetStatBlock().IsAlive())
		{
			GroundBattle::Instance().AddEvent(new CEDelay(1500));
			GroundBattle::Instance().AddEvent(new CEFadeOut(m_pData));
			GroundBattle::Instance().AddEvent(new CEEndCutscene());
			return true;
		}

		if (m_eventPhase == 0)
		{
			std::stringstream temp;
			temp << pPlayer->GetStatBlock().GetName() << " earned "
				 << m_pData->totalExp << "\nEXP. points.";
			GroundBattle::Instance().ShowCombatText(temp.str());
			m_eventPhase++;
		}

		if (GroundBattle::Instance().GetCombatText().AtEndOfString())
			m_delay += ticks;

		if ((m_delay >= 1000.0f) && (m_eventPhase == 1))
		{
			GroundBattle::Instance().HideCombatText();
			
			// if the player has broken the 100 exp mark, level up
			if (pPlayer->GetStatBlock().AddExp(m_pData->totalExp))
			{
				GroundBattle::Instance().AddEvent(new CEDelay(500));
				GroundBattle::Instance().AddEvent(new CELevelUp(m_pData));
				return true;
			}

			// if there's gold to award at this point, award it
			if (m_pData->totalGold > 0)
			{	
				GroundBattle::Instance().AddEvent(new CEDelay(500));
				GroundBattle::Instance().AddEvent(new CEAwardGold(m_pData));
			}
			else // no gold to award, we're done
			{
				GroundBattle::Instance().AddEvent(new CEDelay(1500));
				GroundBattle::Instance().AddEvent(new CEFadeOut(m_pData));
				GroundBattle::Instance().AddEvent(new CEEndCutscene());
			}

			return true;
		}
	}
	return false;
}

///////////////////
// CEAwardGold	 //
///////////////////

CEAwardGold::CEAwardGold(CombatEventData* pData) :
	CombatEvent(pData, EventID::AwardGold)
{
	
}

bool CEAwardGold::Update(float ticks)
{
	if ((m_eventPhase == 0) && (m_pData->totalGold > 0))
	{
		std::stringstream temp;
		temp << "Found " << m_pData->totalGold << " gold coins.";
		//@TODO: put gold into party wallet
		GroundBattle::Instance().ShowCombatText(temp.str());
		m_eventPhase++;
	}	

	if (GroundBattle::Instance().GetCombatText().AtEndOfString())
		m_delay += ticks;

	if ((m_delay >= 1000.0f) && (m_eventPhase == 1))
	{
		GroundBattle::Instance().HideCombatText();
		GroundBattle::Instance().AddEvent(new CEDelay(1500));
		GroundBattle::Instance().AddEvent(new CEFadeOut(m_pData));
		GroundBattle::Instance().AddEvent(new CEEndCutscene());
		return true;
	}
	return false;
}

///////////////////
// CELevelUp	 //
///////////////////

CELevelUp::CELevelUp(CombatEventData* pData) :
	CombatEvent(pData, EventID::LevelUp)
{
	
}

bool CELevelUp::Update(float ticks)
{
	if (m_eventPhase == 0)
	{
		std::stringstream temp;
		temp << m_pAttacker->GetStatBlock().GetName() << " became level " 
			 << m_pAttacker->GetStatBlock().GetLevel()+1 << "!     "
			 << "\nAttack increased by 1.     @"
			 << "\nDefense increased by 1.     @"
			 << "\nAgility increased by 1.     @"
			 << "\nHP increased by 1.     @"
			 << "\nMP increased by 1.";
		GroundBattle::Instance().ShowCombatText(temp.str());
		m_eventPhase++;
	}	

	if (GroundBattle::Instance().GetCombatText().AtEndOfString())
		m_delay += ticks;

	if ((m_delay >= 1000.0f) && (m_eventPhase == 1))
	{
		GroundBattle::Instance().HideCombatText();
		GroundBattle::Instance().AddEvent(new CEDelay(500));
		if (m_pData->totalGold > 0)
			GroundBattle::Instance().AddEvent(new CEAwardGold(m_pData));
		else
		{
			GroundBattle::Instance().AddEvent(new CEFadeOut(m_pData));
			GroundBattle::Instance().AddEvent(new CEEndCutscene());
		}

		return true;
	}
	else
		return false;
}

///////////////////
// CEDelay		 //
///////////////////

CEDelay::CEDelay(const unsigned int timeInMS) :
	CombatEvent(NULL, EventID::Delay),
	m_delayLength(static_cast<float>(timeInMS))
{
	
}

bool CEDelay::Update(float ticks)
{
	m_delay += ticks;
	if (m_delay >= m_delayLength)
		return true;
	else
		return false;
}

///////////////////
// CEFadeIn		 //
///////////////////

CEFadeIn::CEFadeIn(CombatEventData* pData) :
	CombatEvent(pData, EventID::FadeIn),
	m_fader(200)
{

}

bool CEFadeIn::Update(float ticks)
{
	if (m_delay == 0)
	{
		GameObjectMgr::Instance().AddObject(m_fader);
		GameObjectMgr::Instance().FocusOnObject(&m_fader);
		m_fader.FadeIn();
	}

	m_delay += ticks;

	if (m_fader.DoneFadingIn())
	{
		GameObjectMgr::Instance().PopFocusObject();
		GameObjectMgr::Instance().RemoveObject(m_fader);
		return true;
	}

	return false;
}

///////////////////
// CEFadeOut	 //
///////////////////

CEFadeOut::CEFadeOut(CombatEventData* pData) :
	CombatEvent(pData, EventID::FadeOut),
	m_fader(200)
{

}

bool CEFadeOut::Update(float ticks)
{
	if (m_delay == 0)
	{
		GameObjectMgr::Instance().AddObject(m_fader);
		GameObjectMgr::Instance().FocusOnObject(&m_fader);
		m_fader.FadeOut();
	}

	m_delay += ticks;

	if (m_fader.DoneFadingOut())
	{
		GameObjectMgr::Instance().PopFocusObject();
		GameObjectMgr::Instance().RemoveObject(m_fader);
		return true;
	}

	return false;
}

//////////////////////
// CEPlayBattleSong	//
//////////////////////

CEPlayBattleSong::CEPlayBattleSong(CombatEventData* pData) :
	CombatEvent(pData, EventID::PlayBattleSong)
{
	
}

bool CEPlayBattleSong::Update(float ticks)
{
	if (m_pData->pAttacker->GetTeam() == ActorTeam::Player)
		AudioMgr::PlaySong("heroattack.mp3");
	else
		AudioMgr::PlaySong("enemyattack.mp3");

	return true;
}

/////////////////////
// CEStartCutscene //
/////////////////////

CEStartCutscene::CEStartCutscene() :
	CombatEvent(NULL, EventID::StartCutscene)
{
	
}

bool CEStartCutscene::Update(float ticks)
{
	GroundBattle::Instance().StartCutscene();
	return true;
}

///////////////////
// CEEndCutscene //
///////////////////

CEEndCutscene::CEEndCutscene() :
	CombatEvent(NULL, EventID::EndCutscene)
{
	
}

bool CEEndCutscene::Update(float ticks)
{
	GroundBattle::Instance().AddEvent(new CEFadeIn(m_pData));
	GroundBattle::Instance().EndCutscene();
	return true;
}

///////////////////////////
// CEEnemyAITryAttacking //
///////////////////////////

CEEnemyAITryAttacking::CEEnemyAITryAttacking(CombatEventData* pData) :
	CombatEvent(pData, EventID::EnemyAITryAttacking),
	m_thinkTime(0)
{
	// randomize fake "thinking" time
//	m_thinkTime = static_cast<float>((rand() % 1000)) + 1500.0f;
//	m_thinkTime = 500.0f;
}

bool CEEnemyAITryAttacking::Update(float ticks)
{
	m_delay += ticks;

	if (m_delay >= m_thinkTime) 	
	{
		if ((m_pData->pAttacker->GetTeam() == ActorTeam::Enemy) &&
			(m_pData->pAttacker->GetPath().IsFinished()))
		{
 			if (GroundBattle::Instance().SilentTargetCheck(m_pData->pAttacker))
				GroundBattle::Instance().SpecificAttack(m_pData->pAttacker, m_pData->pDefender);
			else
				GroundBattle::Instance().NextTurn();

		}
		return true;
	}

	return false;
}

///////////////////
// CEEnemyAIMove //
///////////////////

CEEnemyAIMove::CEEnemyAIMove(CombatEventData* pData) :
	CombatEvent(pData, EventID::EnemyAIMove),
	m_thinkTime(0),
	m_pathfinder(m_pData->pAttacker)
{
	// randomize fake "thinking" time
//	m_thinkTime = static_cast<float>((rand() % 1000)) + 1500.0f;
	m_thinkTime = 500.0f;
	
}

bool CEEnemyAIMove::Update(float ticks)
{
	if (m_delay <= 0.0f)
	{
		ActorList& players = GroundBattle::Instance().GetPlayerList();
		ActorList candidates;

		for (unsigned int i = 0; i < players.size(); i++)
		{
			if (m_pathfinder.ManhattanDist(players[i]) <= 
				(m_pData->pAttacker->GetStatBlock().GetMovement() + 1))
				candidates.push_back(players[i]);
		}

		if (candidates.size() > 0)
		{
			std::sort(candidates.begin(), candidates.end(), CompareCurrHP);
			//@TODO: implement ranges greater than 1
			int maxrange = 1;//m_pData->pAttacker->GetStatBlock().GetAttackRange()
			m_pathfinder.FindPath(candidates[0]->GetWorldPos().CoordToTileNum(), maxrange);
			m_pData->pDefender = candidates[0];
		}
		else 
		{
			GroundBattle::Instance().NextTurn();
			return true;
		}
	}

	m_delay += ticks;

	if ((m_delay >= m_thinkTime) &&	(m_pData->pAttacker->GetTeam() == ActorTeam::Enemy))
	{
		m_thinkTime += 100.0f;
		if (m_pathfinder.DoPathfindingStep())
		{
			m_pathfinder.ProcessResult();
//			GroundBattle::Instance().AddEvent(new CEWaitForMovement(m_pData));
			GroundBattle::Instance().AddEvent(new CEEnemyAIWaitForMove(m_pData));
			GroundBattle::Instance().AddEvent(new CEEnemyAITryAttacking(m_pData));
			return true;
		}
	}

	return false;
}

//////////////////////////
// CEEnemyAIWaitForMove //
//////////////////////////

CEEnemyAIWaitForMove::CEEnemyAIWaitForMove(CombatEventData* pData) : 
	CombatEvent(pData, EventID::EnemyAIWaitForMove),
	m_delay(0.0f)
{

}

bool CEEnemyAIWaitForMove::Update(float ticks)
{
	if (m_pData->pAttacker->GetPath().IsFinished())
	{
		m_delay += ticks;
		if (m_delay > 500.0f)
			return true;
	}

	return false;
}

///////////////////
// CEEndBattle	 //
///////////////////

CEEndBattle::CEEndBattle() :
	CombatEvent(NULL, EventID::EndBattle)
{
	
}

bool CEEndBattle::Update(float ticks)
{
	GroundBattle::Instance().EndBattle();
	return true;
}