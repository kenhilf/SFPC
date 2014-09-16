#include "groundbattle.h"
#include <sstream>
#include <algorithm>
#include "actor.h"
#include "audiomgr.h"
#include "windowmgr.h"
#include "gameobjectmgr.h"
#include "tilemap.h"
#include "actormgr.h"
#include "statblock.h"
#include "stlex.h"
#include "commandmgr.h"

// a compare function that compares two actor's by their AGI so the main list can be
// sorted for initiative
bool CompareAgi(Actor* lhs, Actor* rhs)
{
	if ((lhs->GetStatBlock().GetAgility()) > (rhs->GetStatBlock().GetAgility()))
		return true;
	else
		return false;
}

GroundBattle::GroundBattle() :
	m_bActive(false),
	m_roundStartPos(0, 0),
	m_numEnemies(0),
	m_turn(0),
	m_round(0),
	m_toHit(0),
	m_numDead(0),
	m_pPlayerLeader(NULL),
	m_pEnemyLeader(NULL),
	m_pAttacker(NULL),
	m_currTarget(0),
	m_pCurrMap(NULL),
	m_pCutscene(NULL),
	m_pCurrEvent(NULL)
{
	m_bigText.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_bigText.SetOffset(96, 86);
	m_bigText.SetScale(2.0f);
	m_bigText.SetLayerSpace(LayerSpace::Foreground);
}

GroundBattle::~GroundBattle()
{
	if (m_bActive)
	{
		EndBattle();
	}

	while (!m_eventList.empty())
	{
		delete m_eventList.front();
		m_eventList.pop_front();
	}
}

void GroundBattle::StartBattle()
{
	m_bActive = true;

	//@TODO: a start battle script should probably be run here, like for pre-battle cutscenes

	m_turn = -1;
	m_round = 1;

	m_toHit = 0;
	m_numDead = 0;

	m_players = ActorMgr::Instance().GetPartyList();
	m_enemies = ActorMgr::Instance().GetEnemyList();

	m_allActors.insert(m_allActors.end(), m_players.begin(), m_players.end());
	m_allActors.insert(m_allActors.end(), m_enemies.begin(), m_enemies.end());
	std::sort(m_allActors.begin(), m_allActors.end(), CompareAgi);
	m_pPlayerLeader = m_players[0];
	m_pEnemyLeader = NULL;
	m_pAttacker = NULL;
	m_roundStartPos = Position(0, 0);
	m_numEnemies = m_enemies.size();

	m_debugHUD.SetScale(0.33f);
//	GameObjectMgr::Instance().AddObject(m_debugHUD);

	m_pCurrMap = &GameObjectMgr::Instance().GetTileMap();

	// place allies starting from the second (first is leader, placed with Warp)
	assert(m_startPositions.size() < m_players.size());
	for (unsigned int i = 1; i <= m_startPositions.size(); ++i)
	{
		GameObjectMgr::Instance().AddObject(*m_players[i]);
		m_players[i]->TeleportTo(m_startPositions[i-1]);
	}
	
	GameObjectMgr::Instance().GetCamera().SnapToActor(m_allActors[0]);

	ResetCEData();
}

void GroundBattle::EndBattle()
{
	if (m_bActive)
	{
		m_bActive = false;
		WindowMgr::Instance().DismissLastModelessWindow(true);
		GameObjectMgr::Instance().RemoveObject(m_debugHUD);
		m_allActors.clear();
		m_players.clear();
		m_enemies.clear();
		m_startPositions.clear();
	}
}

void GroundBattle::Update(const float ticks)
{
	if (!m_bActive)
		return;

	// Show debug info
/*	std::stringstream temp;
	temp << "\n \n \n \n \n \nBattle Info: \nRound: " << m_round << "\nTurn: " << m_turn << "\nTargets:\n";
	for (unsigned int i = 0; i < m_targets.size(); i++)
	{
		temp << m_targets[i]->GetStatBlock().GetName() << "\n";
	}
	temp << "ToHit: " << m_toHit;
	m_debugHUD.SetString(temp.str());
*/
	// process events
	if ((m_pCurrEvent == NULL) && (m_eventList.size() > 0))
	{
		m_pCurrEvent = m_eventList.front();
		m_eventList.pop_front();
	}

	while (m_pCurrEvent)
	{
		if (m_pCurrEvent->Update(ticks))
		{
			delete m_pCurrEvent;
			m_pCurrEvent = NULL;

			if (m_eventList.size() > 0)
			{
				m_pCurrEvent = m_eventList.front();
				m_eventList.pop_front();
			}
		}
		else
			break;
	}
}

void GroundBattle::PlaceAlly(const Position& pos)
{
	m_startPositions.push_back(pos);
}

void GroundBattle::AddEvent(CombatEvent* pEvent)
{
	m_eventList.push_back(pEvent);
}

void GroundBattle::AddPriorityEvent(CombatEvent* pEvent)
{
	m_eventList.push_front(pEvent);
}

void GroundBattle::ClearEvents()
{
	while (!m_eventList.empty())
	{
		delete m_eventList.front();
		m_eventList.pop_front();
	}
}

Actor& GroundBattle::NextTurn()
{
	Camera& camera = GameObjectMgr::Instance().GetCamera();
	TileMap& currMap = GameObjectMgr::Instance().GetTileMap();

	WindowMgr::Instance().DismissLastModelessWindow();

	// Not sure if I want to keep the SF tradition of facing down after every action, 
	// but here it is.
	if (m_turn >= 0)
		m_allActors[m_turn]->SetFacing(Direction::Down);

	// Check if last turn's actor is alive or not.  If they died, we snap the camera to 
	// the next actor instead of sliding
	bool bLastActorDead = false;
	if (m_turn >= 0)
		bLastActorDead = !m_allActors[m_turn]->GetStatBlock().IsAlive();

	// Done with the last actor, move on to the next one.  If that one is dead, keep going.
	m_turn++;
	if (m_turn >= static_cast<int>(m_allActors.size()))
	{
		m_turn = 0;
		m_round++;
	}
	while (!(m_allActors[m_turn]->GetStatBlock().IsAlive()))
	{
		m_turn++;
		if (m_turn >= static_cast<int>(m_allActors.size()))
		{
			m_turn = 0;
			m_round++;
		}
	}

	currMap.ClearFlashMarkers();
	if (bLastActorDead)
		camera.SnapToActor(m_allActors[m_turn]);
	else
		camera.FollowNewActor(m_allActors[m_turn]);

	Position pos = m_allActors[m_turn]->GetLocalPos().CoordToTileNum();
	m_roundStartPos = pos;
	//@HACK: This gets everyone moving far enough doing GetMovement+1, as
	// characters are getting charged a step for the the square they start on,
	// addendum 6/18/12: and I am suddenly wondering if they are getting charged for
	// that tile's land effect, too
	currMap.CalculateWalkable(pos.x, pos.y, 
		static_cast<float>(m_allActors[m_turn]->GetStatBlock().GetMovement()+1), 
		m_allActors[m_turn]);

	WindowMgr::Instance().AddBattlefieldHUDGrp(m_allActors[m_turn]);

	if (m_allActors[m_turn]->GetTeam() == ActorTeam::Enemy)
	{
		m_CEData.pAttacker = m_allActors[m_turn];
		AddEvent(new CEDelay(2000));
		AddEvent(new CEEnemyAIMove(&m_CEData));
//		AddEvent(new CEEnemyAITryAttacking(&m_CEData));
	}

	return *m_allActors[m_turn];
}

Actor& GroundBattle::GetActiveActor()
{
	return *m_allActors[m_turn];
}

Actor& GroundBattle::GetCurrTarget()
{
	return *m_targets[m_currTarget];
}

ActorList& GroundBattle::GetPlayerList()
{
	return m_players;
}

void GroundBattle::ShowMoveRange()
{
	if (GameObjectMgr::Instance().CheckCurrentFocus(&m_targetBracket))
	{
		GameObjectMgr::Instance().PopFocusObject();
		GameObjectMgr::Instance().RemoveObject(m_targetBracket);
	}

	m_pCurrMap->ClearFlashMarkers();
	m_pCurrMap->CalculateWalkable(m_roundStartPos.x, m_roundStartPos.y,
		static_cast<float>(m_allActors[m_turn]->GetStatBlock().GetMovement()+1), 
		m_allActors[m_turn]);
}

void GroundBattle::Attack()
{
	if (GameObjectMgr::Instance().CheckCurrentFocus(&m_targetBracket))
	{
		GameObjectMgr::Instance().PopFocusObject();
		GameObjectMgr::Instance().RemoveObject(m_targetBracket);
	}

	ResetCEData();
	m_pAttacker = m_allActors[m_turn];
	m_CEData.pAttacker = m_allActors[m_turn];
	m_CEData.pDefender = m_targets[m_currTarget];
	AddEvent(new CEFadeOut(&m_CEData));
	AddEvent(new CEPlayBattleSong(&m_CEData));
	AddEvent(new CEStartCutscene());
	AddEvent(new CEFadeIn(&m_CEData));
	AddEvent(new CEAttack(&m_CEData));

	// Force the FadeOut from the queue into the current event slot
	// so that IsCutsceneRunning will properly report true
	if ((m_pCurrEvent == NULL) && (m_eventList.size() > 0))
	{
		m_pCurrEvent = m_eventList.front();
		m_eventList.pop_front();
	}
}

bool GroundBattle::SpecificAttack(Actor* pAttacker, Actor* pTarget)
{
	m_currTarget = 0;
	m_targets = m_pCurrMap->RangeCheck(*pAttacker);
	ShowMoveRange();
	
	ActorList::iterator iter = std::find(m_targets.begin(), m_targets.end(), pTarget);
	if (iter != m_targets.end())
	{
		m_targets.clear();
		m_targets.push_back(pTarget);
		Attack();
		return true;
	}

	return false;
}

void GroundBattle::FindTargets()
{
	m_currTarget = 0;
	m_targets = m_pCurrMap->RangeCheck(*m_allActors[m_turn]);
	if (m_targets.size() == 0)
	{
		AudioMgr::PlaySoundEffect("error.ogg");
		ShowMoveRange();
	}
	else if (m_targets.size() > 0)
	{
		GameObjectMgr::Instance().AddObject(m_targetBracket);
		GameObjectMgr::Instance().FocusOnObject(&m_targetBracket);
	}
}

bool GroundBattle::SilentTargetCheck(Actor* pAttacker, Actor* pDefender)
{
	m_currTarget = 0;
	m_targets = m_pCurrMap->RangeCheck(*pAttacker);
	ShowMoveRange();

	// counterattack check, prevents melee from counterattacking archers and vice versa
	if (pDefender && (m_targets.size() > 0)) 
	{
		ActorList::iterator iter = std::find(m_targets.begin(), m_targets.end(), pDefender);
		if (iter != m_targets.end())
			return true;
		else
			return false;
	}
	else // find any target
	{
		if (m_targets.size() > 0)
			return true;
		else
			return false;
	}
}

void GroundBattle::NextTarget()
{
	++m_currTarget;
	if (m_currTarget >= static_cast<int>(m_targets.size()))
		m_currTarget = 0;
}

void GroundBattle::PrevTarget()
{
	--m_currTarget;
	if (m_currTarget < 0)
		m_currTarget = m_targets.size() - 1;
}

bool GroundBattle::IsTargeting()
{
	if (GameObjectMgr::Instance().CheckCurrentFocus(&m_targetBracket))
		return true;
	else
		return false;
}

void GroundBattle::WinLossCheck()
{
	// player wins if all enemies are defeated
	if (m_enemies.size() == 0)
	{
		GameObjectMgr::Instance().GetCamera().SnapToActor(m_pPlayerLeader);
		m_pCurrMap->ClearFlashMarkers();
		AudioMgr::PlaySong("stinger_good.ogg");
		AudioMgr::LoopSong(false);
		m_bigText.SetOffset(32, 96);
		m_bigText.SetString("VICTORY!");
		GameObjectMgr::Instance().AddObject(m_bigText);
		EndBattle();
		CommandMgr::Instance().AddCommandManually(new GCWait(5000));
		CommandMgr::Instance().AddCommandManually(new GCShowOutroAndQuit());
		return;
	}

	// some sort of enemy leader check goes here, but not all battles will have an 
	// enemy leader...

	// if either all team members or the party leader are defeated, the player loses
	assert(m_pPlayerLeader != NULL);
	if ((m_players.size() == 0) || (!m_pPlayerLeader->GetStatBlock().IsAlive()))
	{
		AudioMgr::PlaySong("gameover.mp3");
		AudioMgr::LoopSong(false);
		m_pCurrMap->ClearFlashMarkers();
		m_bigText.SetOffset(96, 86);
		m_bigText.SetString("GAME\nOVER");
		GameObjectMgr::Instance().AddObject(m_bigText);
		EndBattle();
		CommandMgr::Instance().AddCommandManually(new GCWait(5000));
		CommandMgr::Instance().AddCommandManually(new GCShowOutroAndQuit());
		return;
	}
}

void GroundBattle::RemoveActorFromBattlefield(Actor* pActor)
{
	pActor->TeleportTo(Position(0, m_numDead++));

	if (pActor->GetTeam() == ActorTeam::Player)
		EraseFromSTLContainer(m_players, pActor);
	else // pActor == enemy
		EraseFromSTLContainer(m_enemies, pActor);
}

void GroundBattle::ShowCombatText(const std::string& text)
{
	assert(m_pCutscene != NULL);
	GameObjectMgr::Instance().RemoveObject(m_autoWin);
	m_autoWin.SetText(text);
	GameObjectMgr::Instance().AddObject(m_autoWin);
	GameObjectMgr::Instance().FocusOnObject(&m_autoWin);
}

void GroundBattle::HideCombatText()
{
	if (GameObjectMgr::Instance().CheckCurrentFocus(&m_autoWin))
	{
		GameObjectMgr::Instance().PopFocusObject();
		GameObjectMgr::Instance().RemoveObject(m_autoWin);
	}
}

AutoAdvanceWindow& GroundBattle::GetCombatText()
{
	return m_autoWin;
}

void GroundBattle::StartCutscene()
{
	assert(m_pCutscene == NULL);
	WindowMgr::Instance().DismissLastModelessWindow(true);

	if (m_CEData.pAttacker->GetTeam() == ActorTeam::Player)
		WindowMgr::Instance().AddBattleCutsceneHUDGrp(m_CEData.pAttacker, m_CEData.pDefender);
	else
		WindowMgr::Instance().AddBattleCutsceneHUDGrp(m_CEData.pDefender, m_CEData.pAttacker);

	if (m_CEData.pAttacker->GetTeam() == ActorTeam::Player)
		m_pCutscene = new BattleCutscene(m_CEData.pAttacker, m_CEData.pDefender);
	else
		m_pCutscene = new BattleCutscene(m_CEData.pDefender, m_CEData.pAttacker);

	GameObjectMgr::Instance().AddObject(*m_pCutscene);
}

void GroundBattle::EndCutscene()
{
	assert(m_pCutscene != NULL);

	GameObjectMgr::Instance().RemoveObject(*m_pCutscene);
	delete m_pCutscene;
	m_pCutscene = NULL;

//	m_pAttacker->SetMoveLock(false);
	m_pAttacker = NULL;

	AudioMgr::PlaySong("sf2battle.ogg");
	WindowMgr::Instance().DismissLastModelessWindow();

	WinLossCheck();

	// if battle is not over, go to the next character's turn
	if (m_bActive)
		NextTurn();
}

bool GroundBattle::IsCutsceneRunning() const
{
	if (m_pCutscene != NULL)
		return true;

	if (m_pCurrEvent != NULL)
	{
		EventID::Type eID = m_pCurrEvent->GetID();
		if ((eID == EventID::FadeIn) || (eID == EventID::FadeOut))
			return true;
	}

	return false;
}

void GroundBattle::SetCSPlayerPose(const BattleAniFrames::Type pose)
{
	assert(m_pCutscene != NULL);
	m_pCutscene->SetPlayerPose(pose);
}

void GroundBattle::SetCSEnemyPose(const BattleAniFrames::Type pose)
{
	assert(m_pCutscene != NULL);
	m_pCutscene->SetEnemyPose(pose);
}

void GroundBattle::DebugToHit(const int toHit)
{
	m_toHit = toHit;
}

void GroundBattle::ResetCEData()
{
	m_CEData.pAttacker = NULL;
	m_CEData.pDefender = NULL;

	m_CEData.atkPhase = AttackPhase::FirstAttack;

	m_CEData.totalExp = 0;
	m_CEData.totalGold = 0;
}
