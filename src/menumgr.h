#ifndef _menumgr_h_
#define _menumgr_h_

#include "actionmenu.h"

typedef std::vector<ActionMenu*> ActionMenuList;
typedef std::vector<ActionName::Type> ActionNameList;

class MenuMgr
{
public:
	// returns singleton instance
	static MenuMgr& Instance() { static MenuMgr instance;  return instance; };
	~MenuMgr();
	
	void Update();
	void AddYesNoMenu();
	void AddFourDirMenu(const ActionName::Type choice1, const ActionName::Type choice2, 
						const ActionName::Type choice3, const ActionName::Type choice4, 
						const int defaultChoice = 0);
	const bool HasActiveMenus() const;
	const bool HasResults() const;
	ActionName::Type GetLastResult();
	void ProcessResults();

private:
	MenuMgr();
	MenuMgr(const MenuMgr& rhs);
	MenuMgr& operator=(const MenuMgr& rhs);

	ActionMenuList m_activeMenus;
	ActionNameList m_results;
};

#endif //_menumgr_h_