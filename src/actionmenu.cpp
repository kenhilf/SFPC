#include "actionmenu.h"
#include <algorithm>
#include "audiomgr.h"
#include "menumgr.h"
#include "inputmgr.h"
#include "gamemodemgr.h"

//////////////////////
// ActionIcon       //
//////////////////////

ActionIcon::ActionIcon(const ActionName::Type action) :
	m_action(action)
{
	int i = action;
	m_ani.GetAnimationIndexer().Init(2, 250, AniType::Loop);
	m_ani.SetTileset(ActionIconMgr::Instance().GetTileset());
	m_ani.AddFrame(i);
	m_ani.AddFrame(i + ActionName::NumTypes);
}

const std::string ActionIcon::GetName(const bool allcaps) const
{
	return ActionIconMgr::Instance().ActionIconToText(m_action, allcaps);
}

const ActionName::Type ActionIcon::GetAction() const
{
	return m_action;
}

const Texture& ActionIcon::GetCurrentFrame() const
{
	return m_ani.GetActiveFrame();
}

void ActionIcon::DoUpdate(const float ticks)
{
	m_ani.DoUpdate(ticks);
}

void ActionIcon::Reset()
{
	m_ani.GetAnimationIndexer().Reset();
	m_ani.GetAnimationIndexer().SetFrame(0);
}

//////////////////////
// ActionMenu		//
//////////////////////

ActionMenu::ActionMenu(const ActionName::Type choice1, const ActionName::Type choice2, 
					   const Position& origin) :
	GameObject(LayerSpace::Window),
	m_activeChoice(0),
	m_lastChoice(0),
	m_numChoices(2),
	m_bChoiceLocked(false),
	m_bExpired(false),
	m_bAllCaps(false),
	m_miniWin(38, 22)
{
	SetLocalPos(origin);
	m_choices.push_back(ActionIcon(choice1));
	m_choices.push_back(ActionIcon(choice2));
	Init();
}

ActionMenu::ActionMenu(const ActionName::Type choice1, const ActionName::Type choice2, 
					   const ActionName::Type choice3, const ActionName::Type choice4, 
					   const Position& origin) : 
	GameObject(LayerSpace::Window),
	m_activeChoice(0),
	m_lastChoice(0),
	m_numChoices(4),
	m_bChoiceLocked(false),
	m_bExpired(false),
	m_bAllCaps(false),
	m_miniWin(62, 22)
{
	SetLocalPos(origin);
	m_choices.push_back(ActionIcon(choice1));
	m_choices.push_back(ActionIcon(choice2));
	m_choices.push_back(ActionIcon(choice3));
	m_choices.push_back(ActionIcon(choice4));
	Init();
}

ActionMenu::~ActionMenu()
{

}

void ActionMenu::DoUpdate(const float ticks)
{
	SetLocalPos(m_slider.GetLocalPos());
	m_choices[m_activeChoice].DoUpdate(ticks);
}

void ActionMenu::DoDraw(DrawMgr& drawMgr)
{
	Vector2f pos = GetWorldPos();
	if (m_numChoices == 2)
	{
		drawMgr.DrawQuad(m_choices[0].GetCurrentFrame(), pos, DrawSpace::Screen);
		pos.x = pos.x + TILE_WIDTH + (TILE_WIDTH/2);
		drawMgr.DrawQuad(m_choices[1].GetCurrentFrame(), pos, DrawSpace::Screen);
	}
	else if (m_numChoices == 4)
	{
		drawMgr.DrawQuad(m_choices[0].GetCurrentFrame(), Vector2f(pos.x + TILE_WIDTH, pos.y), DrawSpace::Screen);
		drawMgr.DrawQuad(m_choices[1].GetCurrentFrame(), Vector2f(pos.x + (TILE_WIDTH * 2), pos.y + (TILE_HEIGHT / 2)), DrawSpace::Screen);
		drawMgr.DrawQuad(m_choices[2].GetCurrentFrame(), Vector2f(pos.x + TILE_WIDTH, pos.y + TILE_HEIGHT), DrawSpace::Screen);
		drawMgr.DrawQuad(m_choices[3].GetCurrentFrame(), Vector2f(pos.x, pos.y + (TILE_HEIGHT / 2)), DrawSpace::Screen);
	}
}

void ActionMenu::ProcessInput()
{
	if (!m_bChoiceLocked && !m_bExpired)
	{
		if (m_numChoices == 2)
		{
			if (InputMgr::Instance().WasPressed(VK_LEFT) && m_activeChoice != 0)
				SelectAction(0);

			else if (InputMgr::Instance().WasPressed(VK_RIGHT) && m_activeChoice != 1)
				SelectAction(1);
		}
		else if (m_numChoices == 4)
		{
			if (InputMgr::Instance().WasPressed(VK_UP) && m_activeChoice != 0)
				SelectAction(0);

			else if (InputMgr::Instance().WasPressed(VK_RIGHT) && m_activeChoice != 1)
				SelectAction(1);

			else if (InputMgr::Instance().WasPressed(VK_DOWN) && m_activeChoice != 2)
				SelectAction(2);

			else if (InputMgr::Instance().WasPressed(VK_LEFT) && m_activeChoice != 3)
				SelectAction(3);
		}

		if (InputMgr::Instance().WasPressed(GEN_A))
			ConfirmAction();

		if (InputMgr::Instance().WasPressed(GEN_B))
			DismissMenu();
	}
}

Slider& ActionMenu::GetSlider()
{
	return m_slider;
}

bool ActionMenu::ChoiceHasChanged() const
{
	if (m_activeChoice == m_lastChoice)
		return false;
	else //(m_activeChoice != m_lastChoice)
		return true;
}

bool ActionMenu::ChoiceLocked() const
{
	return m_bChoiceLocked;
}

bool ActionMenu::IsExpired() const
{
	return m_bExpired;
}

void ActionMenu::SelectAction(const int choice)
{
	assert((choice >= 0) && (choice <= m_numChoices));
	m_choices[m_activeChoice].Reset();
	m_lastChoice = m_activeChoice;
	m_activeChoice = choice;
	m_miniWin.SetText(GetCurrChoiceText());
}

void ActionMenu::ConfirmAction()
{
	m_bChoiceLocked = true;

	if (m_numChoices == 4)
		m_slider.SetOffScreenPos(Vector2f(static_cast<float>(GAME_SCREEN_WIDTH), GetWorldPos().y));

	DismissMenu();
}

void ActionMenu::DismissMenu()
{
	m_bExpired = true;
	m_slider.SlideOffScreen();

	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
}

const std::string ActionMenu::GetCurrChoiceText() const
{
	ActionName::Type curr = GetCurrChoice();
	bool disabled = true;

	if (GameModeMgr::GetMode() == GameMode::Field)
	{
		switch (curr)
		{
			case ActionName::Member:	disabled = false;	break;
			case ActionName::Item:		disabled = false;	break;
			case ActionName::Search:	disabled = false;	break;
			case ActionName::Magic:		disabled = false;	break;
			case ActionName::Equip:		disabled = false;	break;
		}
	}
	else if (GameModeMgr::GetMode() == GameMode::Battle)
	{
		switch (curr)
		{
			case ActionName::Attack:	disabled = false;	break;
			case ActionName::Stay:		disabled = false;	break;
		}
	}

	std::string text;
	if (disabled)
		text = "*8" + m_choices[m_activeChoice].GetName(m_bAllCaps) + "*0";
	else
		text = m_choices[m_activeChoice].GetName(m_bAllCaps);

	return text;
}

ActionName::Type ActionMenu::GetCurrChoice() const
{
	return m_choices[m_activeChoice].GetAction();
}

void ActionMenu::Init()
{
	Vector2f pos = GetLocalPos();
	m_slider = Slider(pos);
	m_slider.SetSlideRate(800);
	if (m_numChoices == 2)
	{
		pos.x = GAME_SCREEN_WIDTH;
		m_miniWin.SetLocalPos(Position(68, 1));
	}
	else if (m_numChoices == 4)
	{
		m_bAllCaps = true;
		pos.y = GAME_SCREEN_HEIGHT;
		m_miniWin.SetLocalPos(Position(80, 25));
	}
	m_slider.SetOffScreenPos(pos);
	m_slider.SnapOffScreen();
	AddChild(m_slider);

	m_miniWin.IgnoreSlider(true);
	m_miniWin.SetText(GetCurrChoiceText());
	m_miniWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	AddChild(m_miniWin);
}

//////////////////////
// ActionIconMgr	//
//////////////////////

ActionIconMgr::ActionIconMgr()
{

}

void ActionIconMgr::Init()
{
	m_icons.LoadTiles("actionicons.bmp", ActionName::NumTypes, 2);
}

Tileset* ActionIconMgr::GetTileset()
{
	return &m_icons;
}

const Texture& ActionIconMgr::GetFrame(const ActionName::Type icon, const bool flash) const
{
	int i = icon;
	if (flash)
		i += m_icons.GetRowWidth();

	return m_icons.GetTile(i);
}

const std::string ActionIconMgr::ActionIconToText(const ActionName::Type icon, const bool allcaps)
{
	std::string temp;
	switch (icon)
	{
		case ActionName::Yes:		temp = "Yes";		break;
		case ActionName::No:		temp = "No";		break;
		case ActionName::Attack:	temp = "Attack";	break;
		case ActionName::Magic:		temp = "Magic";		break;
		case ActionName::Item:		temp = "Item";		break;
		case ActionName::Stay:		temp = "Stay";		break;
		case ActionName::Search:	temp = "Search";	break;
		case ActionName::Member:	temp = "Member";	break;
		case ActionName::Talk:		temp = "Talk";		break;
		case ActionName::Use:		temp = "Use";		break;
		case ActionName::Give:		temp = "Give";		break;
		case ActionName::Equip:		temp = "Equip";		break;
		case ActionName::Drop:		temp = "Drop";		break;
		case ActionName::Map:		temp = "Map";		break;
		case ActionName::Speed:		temp = "Speed";		break;
		case ActionName::Quit:		temp = "Quit";		break;
		case ActionName::Raise:		temp = "Raise";		break;
		case ActionName::Cure:		temp = "Cure";		break;
		case ActionName::Promote:	temp = "Promote";	break;
		case ActionName::Save:		temp = "Save";		break;
		case ActionName::Buy:		temp = "Buy";		break;
		case ActionName::Sell:		temp = "Sell";		break;
		case ActionName::Repair:	temp = "Repair";	break;
		case ActionName::Deals:		temp = "Deals";		break;
		case ActionName::Join:		temp = "Join";		break;
		case ActionName::Depot:		temp = "Depot";		break;
		case ActionName::Purge:		temp = "Purge";		break;
		case ActionName::Look:		temp = "Look";		break;
		case ActionName::Deposit:	temp = "Deposit";	break;
		case ActionName::Derive:	temp = "Derive";	break;
	};

	if (allcaps)
	{
		std::transform(temp.begin(), temp.end(), temp.begin(), toupper);
	}

	return temp;
}