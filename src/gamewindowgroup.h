#ifndef _gamewindowgroup_h_
#define _gamewindowgroup_h_

#include "gamewindow.h"
#include "magic.h"
#include "item.h"
class Actor;

class GameWindowGroup : public GameObject
{
public:
	GameWindowGroup();
	virtual void SummonWindows();
	virtual void DismissWindows();
	virtual void DismissWindowsSilently();
	virtual bool IsExpired();
	virtual void ProcessInput();
	virtual void DoUpdate(const float ticks);

protected:
	bool m_bExpired;
};

class GWGCharacterStats : public GameWindowGroup
{
public:
	GWGCharacterStats(Actor* pActor);
	virtual void SummonWindows();
	virtual void DismissWindows();
	virtual bool IsExpired();

private:
	GameWindow m_statsWin;
	GameWindow m_faceWin;
	GameWindow m_kdWin;
	Actor* m_pActor;
	InventoryRenderer m_invRenderer;
	SpellBookRenderer m_sbRenderer;
};

class GWGTeam : public GameWindowGroup
{
public:
	GWGTeam();
	virtual void SummonWindows();
	virtual void DismissWindows();
	virtual bool IsExpired();
	virtual void ProcessInput();
	virtual void DoUpdate(const float ticks);

protected:
	GameWindow m_faceWin;
	GameWindow m_basicStatsWin;
	GameWindow m_charListWin;
	int m_actorIndex;
	int m_topRow;
	int m_viewableRows;
	InventoryRenderer m_invRenderer;
	SpellBookRenderer m_sbRenderer;
	GameString m_teamText;
};

class GWGTeamMagic : public GWGTeam
{
public:
	GWGTeamMagic();
	virtual void ProcessInput();
};

class GWGTeamItem : public GWGTeam
{
public:
	GWGTeamItem();
	virtual void ProcessInput();

protected:
	GameWindow m_equipWpnWin;
	GameWindow m_equipAccWin;
	int m_selection;
};

class GWGBattlefieldHUD : public GameWindowGroup
{
public:
	GWGBattlefieldHUD(Actor* pActor);
	virtual void SummonWindows();
	virtual void DismissWindows();
	virtual void DismissWindowsSilently();
	virtual bool IsExpired();
	virtual void DoUpdate(const float ticks);

private:
	GameWindow m_landEffectWin;
	GameWindow m_selfStatsWin;
	Actor* m_pActor;
	int m_statsWinWidth;
};

class GWGBattleCutsceneHUD : public GameWindowGroup
{
public:
	GWGBattleCutsceneHUD(Actor* pPlayer, Actor* pEnemy);
	virtual void SummonWindows();
	virtual void DismissWindows();
	virtual bool IsExpired();
	virtual void DoUpdate(const float ticks);

private:
	GameWindow m_playerWin;
	GameWindow m_enemyWin;
	Actor* m_pPlayer;
	Actor* m_pEnemy;
	int m_playerWinWidth;
	int m_enemyWinWidth;
};

class GWGTalkingHeads : public GameWindowGroup
{
public:
	GWGTalkingHeads(Actor* pActor, const std::string& text);
	virtual void SummonWindows();
	virtual void DismissWindows();
	virtual bool IsExpired();
	virtual void ProcessInput();

private:
	DialogueWindow m_diaWin;
	GameWindow m_faceWin;
	Actor* m_pActor;
};

class GWGDialogue : public GameWindowGroup
{
public:
	GWGDialogue(const std::string& text);
	virtual void SummonWindows();
	virtual void DismissWindows();
	virtual bool IsExpired();
	virtual void ProcessInput();

private:
	DialogueWindow m_diaWin;
};

class GWGAutoAdvance : public GameWindowGroup
{
public:
	GWGAutoAdvance(const std::string& text);
	virtual void SummonWindows();
	virtual void DismissWindows();
	virtual bool IsExpired();
	virtual void ProcessInput();

private:
	AutoAdvanceWindow m_autoWin;
};

class GWGShopping : public GameWindowGroup
{
public:
	GWGShopping();
	virtual void SummonWindows();
	virtual void DismissWindows();
	virtual bool IsExpired();
	virtual void ProcessInput();

private:
	GameWindow m_merchandiseWin;
	GameWindow m_itemNameWin;
	GameWindow m_goldWin;
};

#endif //_gamewindowgroup_h_