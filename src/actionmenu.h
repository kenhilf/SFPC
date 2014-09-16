#ifndef _actionmenu_h_
#define _actionmenu_h_

#include "texture.h"
#include "tileset.h"
#include "gameobject.h"
#include "animation.h"
#include "slider.h"
#include "gamewindow.h"

class ActionIcon
{
public:
	ActionIcon(const ActionName::Type action);
	const Texture& GetCurrentFrame() const;
	const std::string GetName(const bool allcaps) const;
	const ActionName::Type GetAction() const;
	void DoUpdate(const float ticks);
	void Reset();

private:
	ActionName::Type m_action;
	Animation m_ani;
};

class ActionMenu : public GameObject
{
public:
	ActionMenu(const ActionName::Type choice1, const ActionName::Type choice2, 
			   const Position& origin = Position(132, 158));
	ActionMenu(const ActionName::Type choice1, const ActionName::Type choice2, 
			   const ActionName::Type choice3, const ActionName::Type choice4, 
			   const Position& origin = Position(136, 184));
	virtual ~ActionMenu();
	virtual void DoUpdate(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);
	virtual void ProcessInput();
	Slider& GetSlider();
	bool ChoiceHasChanged() const;
	bool ChoiceLocked() const;
	bool IsExpired() const;
	void SelectAction(const int choice);
	void ConfirmAction();
	void DismissMenu();
	const std::string GetCurrChoiceText() const;
	ActionName::Type GetCurrChoice() const;

private:
	void Init();

	int m_activeChoice;
	int m_lastChoice;
	int m_numChoices;
	bool m_bChoiceLocked;
	bool m_bExpired;
	bool m_bAllCaps;
	typedef std::vector<ActionIcon> ActionIconList;
	ActionIconList m_choices;
	Slider m_slider;
	GameWindow m_miniWin;
};

class ActionIconMgr
{
public:
	// returns singleton instance
	static ActionIconMgr& Instance() { static ActionIconMgr instance;  return instance; };
	void Init();
	Tileset* GetTileset();
	const Texture& GetFrame(const ActionName::Type icon, const bool flash = false) const;
	const std::string ActionIconToText(const ActionName::Type icon, const bool allcaps = true);

private:
	ActionIconMgr();
	ActionIconMgr(const ActionIconMgr& rhs);
	ActionIconMgr& operator=(const ActionIconMgr& rhs);

	Tileset m_icons;
};

#endif //_actionmenu_h_