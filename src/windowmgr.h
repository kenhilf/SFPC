#ifndef _windowmgr_h_
#define _windowmgr_h_

#include <vector>
class GameWindowGroup;
class Actor;

typedef std::vector<GameWindowGroup*> GameWindowGroupList;

class WindowMgr
{
public:
	// returns singleton instance
	static WindowMgr& Instance() { static WindowMgr instance;  return instance; };
	~WindowMgr();
	
	void Update(const float ticks);
	void DismissLastModelessWindow(const bool silent = false);

	void AddCharStatsWinGrp(Actor* pActor);
	void AddTeamWinGrp();
	void AddTeamMagicWinGrp();
	void AddTeamItemWinGrp();
	void AddBattlefieldHUDGrp(Actor* pActor);
	void AddBattleCutsceneHUDGrp(Actor* pPlayer, Actor* pEnemy);
	void AddTalkingHeadsGrp(Actor* pActor, const std::string& text);
	void AddDialogueGrp(const std::string& text);
	void AddAutoAdvanceGrp(const std::string& text);
	void AddShoppingGrp();

	bool HasActiveWindows();
	bool HasModelessWindows();

private:
	WindowMgr();
	WindowMgr(const WindowMgr& rhs);
	WindowMgr& operator=(const WindowMgr& rhs);

	GameWindowGroupList m_activeWindowGroups;
	GameWindowGroupList m_modelessWindowGroups;
};

#endif //_windowmgr_h_