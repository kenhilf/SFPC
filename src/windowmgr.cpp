#include "windowmgr.h"
#include "gameobjectmgr.h"
#include "gamewindowgroup.h"

WindowMgr::WindowMgr()
{
	m_modelessWindowGroups.reserve(4);
}

WindowMgr::~WindowMgr()
{
	GameWindowGroupList::iterator iter = m_activeWindowGroups.begin();
	for ( ; iter != m_activeWindowGroups.end(); ++iter)
	{
		delete (*iter);
	}
}

void WindowMgr::Update(const float ticks)
{
	if (HasActiveWindows())
	{
		m_activeWindowGroups.back()->DoUpdate(ticks);

		GameWindowGroup* pGWG;

		// has the wingrp at the end of the list expired?
		if (m_activeWindowGroups.back()->IsExpired())
		{
 			pGWG = m_activeWindowGroups.back();
			m_activeWindowGroups.pop_back();
			if (GameObjectMgr::Instance().CheckCurrentFocus(pGWG))
			{
				GameObjectMgr::Instance().PopFocusObject();
			}
			GameObjectMgr::Instance().RemoveObject(*pGWG);
			delete pGWG;
			// if any more windows are left on the pile, call the next one back 
			// onto the screen, they should still be in the Focus list in GameObjectMgr
			if (HasActiveWindows())
			{
				m_activeWindowGroups.back()->SummonWindows();
			}
		}
	}

	if (HasModelessWindows())
	{
		//@HACK: as is, this code will not completely clean out the modeless list if
		// it has multiple expired windows, it will at most clean out one per Update().
		GameWindowGroupList::iterator iter = m_modelessWindowGroups.begin();
		for ( ; iter != m_modelessWindowGroups.end(); ++iter)
		{
			if ((*iter)->IsExpired())
			{
				GameWindowGroup* pGWG = (*iter);
				m_modelessWindowGroups.erase(iter);
				GameObjectMgr::Instance().RemoveObject(*pGWG);
				delete pGWG;
				break;
			}
		}
	}
}

void WindowMgr::DismissLastModelessWindow(const bool silent)
{
	if (HasModelessWindows())
	{
		if (silent)
			m_modelessWindowGroups.back()->DismissWindowsSilently();
		else
			m_modelessWindowGroups.back()->DismissWindows();
	}
}

void WindowMgr::AddCharStatsWinGrp(Actor* pActor)
{
	m_activeWindowGroups.push_back(new GWGCharacterStats(pActor));
	GameObjectMgr::Instance().AddObject(*m_activeWindowGroups.back());
	m_activeWindowGroups.back()->SummonWindows();
	GameObjectMgr::Instance().FocusOnObject(m_activeWindowGroups.back());
}

void WindowMgr::AddTeamWinGrp()
{
	m_activeWindowGroups.push_back(new GWGTeam());
	GameObjectMgr::Instance().AddObject(*m_activeWindowGroups.back());
	m_activeWindowGroups.back()->SummonWindows();
	GameObjectMgr::Instance().FocusOnObject(m_activeWindowGroups.back());
}

void WindowMgr::AddBattlefieldHUDGrp(Actor* pActor)
{
	m_modelessWindowGroups.push_back(new GWGBattlefieldHUD(pActor));
	GameObjectMgr::Instance().AddObject(*m_modelessWindowGroups.back());
	m_modelessWindowGroups.back()->SummonWindows();
}

void WindowMgr::AddBattleCutsceneHUDGrp(Actor* pPlayer, Actor* pEnemy)
{
	m_modelessWindowGroups.push_back(new GWGBattleCutsceneHUD(pPlayer, pEnemy));
	GameObjectMgr::Instance().AddObject(*m_modelessWindowGroups.back());
	m_modelessWindowGroups.back()->SummonWindows();
}

void WindowMgr::AddTeamMagicWinGrp()
{
	m_activeWindowGroups.push_back(new GWGTeamMagic());
	GameObjectMgr::Instance().AddObject(*m_activeWindowGroups.back());
	m_activeWindowGroups.back()->SummonWindows();
	GameObjectMgr::Instance().FocusOnObject(m_activeWindowGroups.back());
}

void WindowMgr::AddTeamItemWinGrp()
{
	m_activeWindowGroups.push_back(new GWGTeamItem());
	GameObjectMgr::Instance().AddObject(*m_activeWindowGroups.back());
	m_activeWindowGroups.back()->SummonWindows();
	GameObjectMgr::Instance().FocusOnObject(m_activeWindowGroups.back());
}

void WindowMgr::AddTalkingHeadsGrp(Actor* pActor, const std::string& text)
{
	m_activeWindowGroups.push_back(new GWGTalkingHeads(pActor, text));
	GameObjectMgr::Instance().AddObject(*m_activeWindowGroups.back());
	m_activeWindowGroups.back()->SummonWindows();
	GameObjectMgr::Instance().FocusOnObject(m_activeWindowGroups.back());
}

void WindowMgr::AddDialogueGrp(const std::string& text)
{
	m_activeWindowGroups.push_back(new GWGDialogue(text));
	GameObjectMgr::Instance().AddObject(*m_activeWindowGroups.back());
	m_activeWindowGroups.back()->SummonWindows();
	GameObjectMgr::Instance().FocusOnObject(m_activeWindowGroups.back());
}

void WindowMgr::AddAutoAdvanceGrp(const std::string& text)
{
	m_activeWindowGroups.push_back(new GWGAutoAdvance(text));
	GameObjectMgr::Instance().AddObject(*m_activeWindowGroups.back());
	m_activeWindowGroups.back()->SummonWindows();
	GameObjectMgr::Instance().FocusOnObject(m_activeWindowGroups.back());
}

void WindowMgr::AddShoppingGrp()
{
	m_activeWindowGroups.push_back(new GWGShopping());
	GameObjectMgr::Instance().AddObject(*m_activeWindowGroups.back());
	m_activeWindowGroups.back()->SummonWindows();
	GameObjectMgr::Instance().FocusOnObject(m_activeWindowGroups.back());
}

bool WindowMgr::HasActiveWindows()
{
	if (m_activeWindowGroups.size() > 0)
		return true;
	else
		return false;
}

bool WindowMgr::HasModelessWindows()
{
	if (m_modelessWindowGroups.size() > 0)
		return true;
	else
		return false;
}