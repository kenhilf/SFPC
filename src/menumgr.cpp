#include "menumgr.h"
#include "windowmgr.h"
#include "audiomgr.h"
#include "gamemodemgr.h"
#include "groundbattle.h"
#include "gameobjectmgr.h"

MenuMgr::MenuMgr()
{

}

MenuMgr::~MenuMgr()
{
	ActionMenuList::iterator iter = m_activeMenus.begin();
	for ( ; iter != m_activeMenus.end(); ++iter)
	{
		delete (*iter);
	}
}

void MenuMgr::Update()
{
	if (HasActiveMenus())
	{
		ActionMenu* pAM = NULL;

		// has the menu at the end of the list gotten a result and moved totally off screen?
		if (m_activeMenus.back()->IsExpired() && m_activeMenus.back()->GetSlider().IsOffScreen())
		{
			if (m_activeMenus.back()->ChoiceLocked())
			{
				m_results.push_back(m_activeMenus.back()->GetCurrChoice());
			}

			pAM = m_activeMenus.back();
			m_activeMenus.pop_back();
			if (GameObjectMgr::Instance().CheckCurrentFocus(pAM))
				GameObjectMgr::Instance().PopFocusObject();

			GameObjectMgr::Instance().RemoveObject(*pAM);
			delete pAM;

			// if any more menus are left on the pile, call the next one back onto the screen,
			// they should still be in the Focus list in GameObjectMgr
			if (HasActiveMenus())
			{
				pAM = m_activeMenus.back();
				pAM->GetSlider().SetOffScreenPos(Vector2f(pAM->GetWorldPos().x, static_cast<float>(GAME_SCREEN_HEIGHT)));
				pAM->GetSlider().SnapOffScreen();
				pAM->GetSlider().SlideOntoScreen();

				// swish sounds for menus
				AudioMgr::PlaySoundEffect("swish1.ogg", 0);
				AudioMgr::PlaySoundEffect("swish1.ogg", 80);
			}
		}
	}
}

void MenuMgr::AddYesNoMenu()
{
	m_activeMenus.push_back(new ActionMenu(ActionName::Yes, ActionName::No));
	GameObjectMgr::Instance().AddObject(*m_activeMenus.back());
	m_activeMenus.back()->GetSlider().SlideOntoScreen();
	GameObjectMgr::Instance().FocusOnObject(m_activeMenus.back());

	// swish sounds for menus
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
}

void MenuMgr::AddFourDirMenu(const ActionName::Type choice1, const ActionName::Type choice2,
							 const ActionName::Type choice3, const ActionName::Type choice4, 
							 const int defaultChoice)
{
	if (HasActiveMenus())
	{
		ActionMenu* pAM = m_activeMenus.back();
		pAM->GetSlider().SetOffScreenPos(Vector2f(pAM->GetWorldPos().x, static_cast<float>(GAME_SCREEN_HEIGHT)));
		pAM->GetSlider().SlideOffScreen();
	}
	m_activeMenus.push_back(new ActionMenu(choice1, choice2, choice3, choice4));
	assert((defaultChoice >=0) && (defaultChoice <= 4));
	m_activeMenus.back()->SelectAction(defaultChoice);
	GameObjectMgr::Instance().AddObject(*m_activeMenus.back());
	m_activeMenus.back()->GetSlider().SlideOntoScreen();
	GameObjectMgr::Instance().FocusOnObject(m_activeMenus.back());

	// swish sounds for menus
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
}

const bool MenuMgr::HasActiveMenus() const
{
	if (m_activeMenus.size() > 0)
		return true;
	else
		return false;
}

const bool MenuMgr::HasResults() const
{
	if (m_results.size() > 0)
		return true;
	else
		return false;
}

ActionName::Type MenuMgr::GetLastResult()
{
	assert(HasResults());
	ActionName::Type result = m_results.back();
	m_results.pop_back();
	return result;
}

void MenuMgr::ProcessResults()
{
	if (HasResults())
	{
		ActionName::Type result = GetLastResult();
		const GameMode::Type gamemode = GameModeMgr::GetMode();

		if (gamemode == GameMode::Field)
		{
			switch(result)
			{
				case ActionName::Member:
					WindowMgr::Instance().AddTeamWinGrp();
				break;
				case ActionName::Item:
					MenuMgr::Instance().AddFourDirMenu(ActionName::Use, ActionName::Equip, ActionName::Drop, ActionName::Give);
				break;
				case ActionName::Search:
					WindowMgr::Instance().AddDialogueGrp("BOWIE investigated the area.^Nothing was there.");				
				break;
				case ActionName::Magic:
					WindowMgr::Instance().AddTeamMagicWinGrp();
				break;
				case ActionName::Use:
					AudioMgr::PlaySoundEffect("error.ogg");
					//WindowMgr::Instance().AddTeamItemWinGrp();
				break;
				case ActionName::Equip:
					WindowMgr::Instance().AddTeamItemWinGrp();
				break;
				case ActionName::Drop:
					AudioMgr::PlaySoundEffect("error.ogg");
					//WindowMgr::Instance().AddTeamItemWinGrp();
				break;
				case ActionName::Give:
					AudioMgr::PlaySoundEffect("error.ogg");
					//WindowMgr::Instance().AddTeamItemWinGrp();
				break;
				case ActionName::Buy:
					WindowMgr::Instance().AddShoppingGrp();
				break;
				default:	break;
			}
		}
		else if (gamemode == GameMode::Battle)
		{
			switch(result)
			{
				case ActionName::Attack:
					GroundBattle::Instance().FindTargets();
				break;
				case ActionName::Stay:
					GroundBattle::Instance().NextTurn();
				break;

				default:
					AudioMgr::PlaySoundEffect("error.ogg");	
				break;
			}
		}
	}
}
